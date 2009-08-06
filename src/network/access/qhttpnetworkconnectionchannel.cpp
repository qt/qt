/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qhttpnetworkconnection_p.h"
#include "qhttpnetworkconnectionchannel_p.h"

#include <qpair.h>
#include <qdebug.h>

#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslkey.h>
#    include <QtNetwork/qsslcipher.h>
#    include <QtNetwork/qsslconfiguration.h>
#endif

QT_BEGIN_NAMESPACE

// TODO: Put channel specific stuff here so it does not polute qhttpnetworkconnection.cpp

void QHttpNetworkConnectionChannel::init()
{
#ifndef QT_NO_OPENSSL
    if (connection->d_func()->encrypt)
        socket = new QSslSocket;
    else
        socket = new QTcpSocket;
#else
    socket = new QTcpSocket;
#endif

    QObject::connect(socket, SIGNAL(bytesWritten(qint64)),
                     this, SLOT(_q_bytesWritten(qint64)),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(connected()),
                     this, SLOT(_q_connected()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(readyRead()),
                     this, SLOT(_q_readyRead()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(disconnected()),
                     this, SLOT(_q_disconnected()),
                     Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                     this, SLOT(_q_error(QAbstractSocket::SocketError)),
                     Qt::DirectConnection);
#ifndef QT_NO_NETWORKPROXY
    QObject::connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
                     this, SLOT(_q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
                     Qt::DirectConnection);
#endif

#ifndef QT_NO_OPENSSL
    QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
    if (sslSocket) {
        // won't be a sslSocket if encrypt is false
        QObject::connect(sslSocket, SIGNAL(encrypted()),
                         this, SLOT(_q_encrypted()),
                         Qt::DirectConnection);
        QObject::connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)),
                         this, SLOT(_q_sslErrors(const QList<QSslError>&)),
                         Qt::DirectConnection);
    }
#endif
}


void QHttpNetworkConnectionChannel::close()
{
    socket->blockSignals(true);
    socket->close();
    socket->blockSignals(false);
    state = QHttpNetworkConnectionChannel::IdleState;
}


//private slots
void QHttpNetworkConnectionChannel::_q_readyRead()
{
    if (!socket)
        return; // ### error
    if (connection->d_func()->isSocketWaiting(socket) || connection->d_func()->isSocketReading(socket)) {
        state = QHttpNetworkConnectionChannel::ReadingState;
        if (reply)
            connection->d_func()->receiveReply(socket, reply);
    }
    // ### error
}

void QHttpNetworkConnectionChannel::_q_bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    if (!socket)
        return; // ### error
    // bytes have been written to the socket. write even more of them :)
    if (connection->d_func()->isSocketWriting(socket))
        connection->d_func()->sendRequest(socket);
    // otherwise we do nothing
}

void QHttpNetworkConnectionChannel::_q_disconnected()
{
    if (!socket)
        return; // ### error
    // read the available data before closing
    if (connection->d_func()->isSocketWaiting(socket) || connection->d_func()->isSocketReading(socket)) {
        state = QHttpNetworkConnectionChannel::ReadingState;
        if (reply)
            connection->d_func()->receiveReply(socket, reply);
    } else if (state == QHttpNetworkConnectionChannel::IdleState && resendCurrent) {
        // re-sending request because the socket was in ClosingState
        QMetaObject::invokeMethod(connection, "_q_startNextRequest", Qt::QueuedConnection);
    }
    state = QHttpNetworkConnectionChannel::IdleState;
}


void QHttpNetworkConnectionChannel::_q_connected()
{
    if (!socket)
        return; // ### error

    // improve performance since we get the request sent by the kernel ASAP
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

    // ### FIXME: if the server closes the connection unexpectedly, we shouldn't send the same broken request again!
    //channels[i].reconnectAttempts = 2;
    if (!pendingEncrypt) {
        state = QHttpNetworkConnectionChannel::IdleState;
        if (reply)
            connection->d_func()->sendRequest(socket);
        else
            close();
    }
}


void QHttpNetworkConnectionChannel::_q_error(QAbstractSocket::SocketError socketError)
{
    if (!socket)
        return;
    bool send2Reply = false;
    QNetworkReply::NetworkError errorCode = QNetworkReply::UnknownNetworkError;

    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        errorCode = QNetworkReply::HostNotFoundError;
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorCode = QNetworkReply::ConnectionRefusedError;
        break;
    case QAbstractSocket::RemoteHostClosedError:
        // try to reconnect/resend before sending an error.
        // while "Reading" the _q_disconnected() will handle this.
        if (state != QHttpNetworkConnectionChannel::IdleState && state != QHttpNetworkConnectionChannel::ReadingState) {
            if (reconnectAttempts-- > 0) {
                connection->d_func()->resendCurrentRequest(socket);
                return;
            } else {
                send2Reply = true;
                errorCode = QNetworkReply::RemoteHostClosedError;
            }
        } else {
            return;
        }
        break;
    case QAbstractSocket::SocketTimeoutError:
        // try to reconnect/resend before sending an error.
        if (state == QHttpNetworkConnectionChannel::WritingState && (reconnectAttempts-- > 0)) {
            connection->d_func()->resendCurrentRequest(socket);
            return;
        }
        send2Reply = true;
        errorCode = QNetworkReply::TimeoutError;
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        errorCode = QNetworkReply::ProxyAuthenticationRequiredError;
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        errorCode = QNetworkReply::SslHandshakeFailedError;
        break;
    default:
        // all other errors are treated as NetworkError
        errorCode = QNetworkReply::UnknownNetworkError;
        break;
    }
    QPointer<QObject> that = connection;
    QString errorString = connection->d_func()->errorDetail(errorCode, socket);
    if (send2Reply) {
        if (reply) {
            reply->d_func()->errorString = errorString;
            // this error matters only to this reply
            emit reply->finishedWithError(errorCode, errorString);
        }
        // send the next request
        QMetaObject::invokeMethod(that, "_q_startNextRequest", Qt::QueuedConnection);
    } else {
        // the failure affects all requests.
        emit connection->error(errorCode, errorString);
    }
    if (that) //signal emission triggered event loop
        close();
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnectionChannel::_q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator* auth)
{
    emit connection->proxyAuthenticationRequired(proxy, auth, connection);
}
#endif

void QHttpNetworkConnectionChannel::_q_uploadDataReadyRead()
{
    connection->d_func()->sendRequest(socket);
}

#ifndef QT_NO_OPENSSL
void QHttpNetworkConnectionChannel::_q_encrypted()
{
    if (!socket)
        return; // ### error
    state = QHttpNetworkConnectionChannel::IdleState;
    connection->d_func()->sendRequest(socket);
}

void QHttpNetworkConnectionChannel::_q_sslErrors(const QList<QSslError> &errors)
{
    if (!socket)
        return;
    //QNetworkReply::NetworkError errorCode = QNetworkReply::ProtocolFailure;
    emit connection->sslErrors(errors);
}
#endif

QT_END_NAMESPACE

#include "moc_qhttpnetworkconnectionchannel_p.cpp"

#endif // QT_NO_HTTP
