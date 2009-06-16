/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
#include <private/qnetworkrequest_p.h>
#include <private/qobject_p.h>
#include <private/qauthenticator_p.h>
#include <qnetworkproxy.h>
#include <qauthenticator.h>

#include <qbuffer.h>
#include <qpair.h>
#include <qhttp.h>
#include <qdebug.h>

#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslkey.h>
#    include <QtNetwork/qsslcipher.h>
#    include <QtNetwork/qsslconfiguration.h>
#endif



QT_BEGIN_NAMESPACE

const int QHttpNetworkConnectionPrivate::channelCount = 2;

QHttpNetworkConnectionPrivate::QHttpNetworkConnectionPrivate(const QString &hostName, quint16 port, bool encrypt)
: hostName(hostName), port(port), encrypt(encrypt),
  pendingAuthSignal(false), pendingProxyAuthSignal(false)
#ifndef QT_NO_NETWORKPROXY
  , networkProxy(QNetworkProxy::NoProxy)
#endif
{
}

QHttpNetworkConnectionPrivate::~QHttpNetworkConnectionPrivate()
{
    for (int i = 0; i < channelCount; ++i) {
        channels[i].socket->close();
        delete channels[i].socket;
    }
}

void QHttpNetworkConnectionPrivate::connectSignals(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);

    QObject::connect(socket, SIGNAL(bytesWritten(qint64)),
               q, SLOT(_q_bytesWritten(qint64)),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(connected()),
               q, SLOT(_q_connected()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(readyRead()),
               q, SLOT(_q_readyRead()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(disconnected()),
               q, SLOT(_q_disconnected()),
               Qt::DirectConnection);
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
               q, SLOT(_q_error(QAbstractSocket::SocketError)),
               Qt::DirectConnection);
#ifndef QT_NO_NETWORKPROXY
    QObject::connect(socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
               q, SLOT(_q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
               Qt::DirectConnection);
#endif

#ifndef QT_NO_OPENSSL
    QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
    QObject::connect(sslSocket, SIGNAL(encrypted()),
                     q, SLOT(_q_encrypted()),
                     Qt::DirectConnection);
    QObject::connect(sslSocket, SIGNAL(sslErrors(const QList<QSslError>&)),
               q, SLOT(_q_sslErrors(const QList<QSslError>&)),
               Qt::DirectConnection);
#endif
}

void QHttpNetworkConnectionPrivate::init()
{
    for (int i = 0; i < channelCount; ++i) {
#ifndef QT_NO_OPENSSL
        channels[i].socket = new QSslSocket;
#else
        channels[i].socket = new QTcpSocket;
#endif
        connectSignals(channels[i].socket);
    }
}

int QHttpNetworkConnectionPrivate::indexOf(QAbstractSocket *socket) const
{
    for (int i = 0; i < channelCount; ++i)
        if (channels[i].socket == socket)
            return i;
    return -1;
}

bool QHttpNetworkConnectionPrivate::isSocketBusy(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (channels[i].state & BusyState);
}

bool QHttpNetworkConnectionPrivate::isSocketWriting(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & WritingState));
}

bool QHttpNetworkConnectionPrivate::isSocketWaiting(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & WaitingState));
}

bool QHttpNetworkConnectionPrivate::isSocketReading(QAbstractSocket *socket) const
{
    int i = indexOf(socket);
    return (i != -1 && (channels[i].state & ReadingState));
}


void QHttpNetworkConnectionPrivate::appendData(QHttpNetworkReply &reply, const QByteArray &fragment, bool compressed)
{
    QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    ba->append(fragment);
    return;
}

qint64 QHttpNetworkConnectionPrivate::bytesAvailable(const QHttpNetworkReply &reply, bool compressed) const
{
    const QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    return ba->size();
}

qint64 QHttpNetworkConnectionPrivate::read(QHttpNetworkReply &reply, QByteArray &data, qint64 maxSize, bool compressed)
{
    QByteArray *ba = (compressed) ? &reply.d_func()->compressedData : &reply.d_func()->responseData;
    if (maxSize == -1 || maxSize >= ba->size()) {
        // read the whole data
        data = *ba;
        ba->clear();
    } else {
        // read only the requested length
        data = ba->mid(0, maxSize);
        ba->remove(0, maxSize);
    }
    return data.size();
}

void QHttpNetworkConnectionPrivate::eraseData(QHttpNetworkReply *reply)
{
    reply->d_func()->compressedData.clear();
    reply->d_func()->responseData.clear();
}

void QHttpNetworkConnectionPrivate::prepareRequest(HttpMessagePair &messagePair)
{
    QHttpNetworkRequest &request = messagePair.first;
    QHttpNetworkReply *reply = messagePair.second;

    // add missing fields for the request
    QByteArray value;
    // check if Content-Length is provided
    QIODevice *data = request.data();
    if (data && request.contentLength() == -1) {
        if (!data->isSequential())
            request.setContentLength(data->size());
        else
            bufferData(messagePair); // ### or do chunked upload
    }
    // set the Connection/Proxy-Connection: Keep-Alive headers
#ifndef QT_NO_NETWORKPROXY
    if (networkProxy.type() == QNetworkProxy::HttpCachingProxy)  {
        value = request.headerField("proxy-connection");
        if (value.isEmpty())
            request.setHeaderField("Proxy-Connection", "Keep-Alive");
    } else {
#endif
        value = request.headerField("connection");
        if (value.isEmpty())
            request.setHeaderField("Connection", "Keep-Alive");
#ifndef QT_NO_NETWORKPROXY
    }
#endif
    // set the gzip header
    value = request.headerField("accept-encoding");
    if (value.isEmpty()) {
#ifndef QT_NO_COMPRESS
        request.setHeaderField("Accept-Encoding", "gzip");
        request.d->autoDecompress = true;
#else
        // if zlib is not available set this to false always
        request.d->autoDecompress = false;
#endif
    }
    // set the User Agent
    value = request.headerField("user-agent");
    if (value.isEmpty())
        request.setHeaderField("User-Agent", "Mozilla/5.0");
    // set the host
    value = request.headerField("host");
    if (value.isEmpty()) {
        QByteArray host = QUrl::toAce(hostName);

        int port = request.url().port();
        if (port != -1) {
            host += ':';
            host += QByteArray::number(port);
        }

        request.setHeaderField("Host", host);
    }

    reply->d_func()->requestIsPrepared = true;
}

bool QHttpNetworkConnectionPrivate::ensureConnection(QAbstractSocket *socket)
{
    // make sure that this socket is in a connected state, if not initiate
    // connection to the host.
    if (socket->state() != QAbstractSocket::ConnectedState) {
        // connect to the host if not already connected.
        int index = indexOf(socket);
        // resend this request after we receive the disconnected signal
        if (socket->state() == QAbstractSocket::ClosingState) {
            channels[index].resendCurrent = true;
            return false;
        }
        channels[index].state = ConnectingState;
        channels[index].pendingEncrypt = encrypt;

        // This workaround is needed since we use QAuthenticator for NTLM authentication. The "phase == Done"
        // is the usual criteria for emitting authentication signals. The "phase" is set to "Done" when the
        // last header for Authorization is generated by the QAuthenticator. Basic & Digest logic does not
        // check the "phase" for generating the Authorization header. NTLM authentication is a two stage
        // process & needs the "phase". To make sure the QAuthenticator uses the current username/password
        // the phase is reset to Start.
        QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[index].authenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;
        priv = QAuthenticatorPrivate::getPrivate(channels[index].proxyAuthenticator);
        if (priv && priv->phase == QAuthenticatorPrivate::Done)
            priv->phase = QAuthenticatorPrivate::Start;

        QString connectHost = hostName;
        qint16 connectPort = port;

#ifndef QT_NO_NETWORKPROXY
        // HTTPS always use transparent proxy.
        if (networkProxy.type() != QNetworkProxy::NoProxy && !encrypt) {
            connectHost = networkProxy.hostName();
            connectPort = networkProxy.port();
        }
#endif
        if (encrypt) {
#ifndef QT_NO_OPENSSL
            QSslSocket *sslSocket = qobject_cast<QSslSocket*>(socket);
            sslSocket->connectToHostEncrypted(connectHost, connectPort);
            if (channels[index].ignoreSSLErrors)
                sslSocket->ignoreSslErrors();
#else
            emitReplyError(socket, channels[index].reply, QNetworkReply::ProtocolUnknownError);
#endif
        } else {
            socket->connectToHost(connectHost, connectPort);
        }
        return false;
    }
    return true;
}


bool QHttpNetworkConnectionPrivate::sendRequest(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);

    int i = indexOf(socket);
    switch (channels[i].state) {
    case IdleState: { // write the header
        if (!ensureConnection(socket)) {
            // wait for the connection (and encryption) to be done
            // sendRequest will be called again from either
            // _q_connected or _q_encrypted
            return false;
        }
        channels[i].written = 0; // excluding the header
        channels[i].bytesTotal = 0;
        if (channels[i].reply) {
            channels[i].reply->d_func()->clear();
            channels[i].reply->d_func()->connection = q;
            channels[i].reply->d_func()->autoDecompress = channels[i].request.d->autoDecompress;
        }
        channels[i].state = WritingState;
        channels[i].pendingEncrypt = false;
        // if the url contains authentication parameters, use the new ones
        // both channels will use the new authentication parameters
        if (!channels[i].request.url().userInfo().isEmpty()) {
            QUrl url = channels[i].request.url();
            QAuthenticator &auth = channels[i].authenticator;
            if (url.userName() != auth.user()
                || (!url.password().isEmpty() && url.password() != auth.password())) {
                auth.setUser(url.userName());
                auth.setPassword(url.password());
                copyCredentials(i, &auth, false);
            }
            // clear the userinfo,  since we use the same request for resending
            // userinfo in url can conflict with the one in the authenticator
            url.setUserInfo(QString());
            channels[i].request.setUrl(url);
        }
        createAuthorization(socket, channels[i].request);
#ifndef QT_NO_NETWORKPROXY
        QByteArray header = QHttpNetworkRequestPrivate::header(channels[i].request,
            (networkProxy.type() != QNetworkProxy::NoProxy));
#else
        QByteArray header = QHttpNetworkRequestPrivate::header(channels[i].request,
            false);
#endif
        socket->write(header);
        QIODevice *data = channels[i].request.d->data;
        QHttpNetworkReply *reply = channels[i].reply;
        if (reply && reply->d_func()->requestDataBuffer.size())
            data = &channels[i].reply->d_func()->requestDataBuffer;
        if (data && (data->isOpen() || data->open(QIODevice::ReadOnly))) {
            if (data->isSequential()) {
                channels[i].bytesTotal = -1;
                QObject::connect(data, SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadNoBuffer()));
                QObject::connect(data, SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadNoBuffer()));
            } else {
                channels[i].bytesTotal = data->size();
            }
        } else {
            channels[i].state = WaitingState;
            break;
        }
        // write the initial chunk together with the headers
        // fall through
    }
    case WritingState: { // write the data
        QIODevice *data = channels[i].request.d->data;
        if (channels[i].reply->d_func()->requestDataBuffer.size())
            data = &channels[i].reply->d_func()->requestDataBuffer;
        if (!data || channels[i].bytesTotal == channels[i].written) {
            channels[i].state = WaitingState; // now wait for response
            break;
        }

        QByteArray chunk;
        chunk.resize(ChunkSize);
        qint64 readSize = data->read(chunk.data(), ChunkSize);
        if (readSize == -1) {
            // source has reached EOF
            channels[i].state = WaitingState; // now wait for response
        } else if (readSize > 0) {
            // source gave us something useful
            channels[i].written += socket->write(chunk.data(), readSize);
            if (channels[i].reply)
                emit channels[i].reply->dataSendProgress(channels[i].written, channels[i].bytesTotal);
        }
        break;
    }
    case WaitingState:
    case ReadingState:
    case Wait4AuthState:
        // ignore _q_bytesWritten in these states
        // fall through
    default:
        break;
    }
    return true;
}

bool QHttpNetworkConnectionPrivate::emitSignals(QHttpNetworkReply *reply)
{
    // for 401 & 407 don't emit the data signals. Content along with these
    // responses are send only if the authentication fails.
    return (reply && reply->d_func()->statusCode != 401 && reply->d_func()->statusCode != 407);
}

bool QHttpNetworkConnectionPrivate::expectContent(QHttpNetworkReply *reply)
{
    // check whether we can expect content after the headers (rfc 2616, sec4.4)
    if (!reply)
        return false;
    if ((reply->d_func()->statusCode >= 100 && reply->d_func()->statusCode < 200)
        || reply->d_func()->statusCode == 204 || reply->d_func()->statusCode == 304)
        return false;
    if (reply->d_func()->request.operation() == QHttpNetworkRequest::Head)
        return !emitSignals(reply);
    if (reply->d_func()->contentLength() == 0)
        return false;
    return true;
}

void QHttpNetworkConnectionPrivate::emitReplyError(QAbstractSocket *socket,
                                                   QHttpNetworkReply *reply,
                                                   QNetworkReply::NetworkError errorCode)
{
    Q_Q(QHttpNetworkConnection);
    if (socket && reply) {
        // this error matters only to this reply
        reply->d_func()->errorString = errorDetail(errorCode, socket);
        emit reply->finishedWithError(errorCode, reply->d_func()->errorString);
        int i = indexOf(socket);
        // remove the corrupt data if any
        eraseData(channels[i].reply);
        closeChannel(i);
        // send the next request
        QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

#ifndef QT_NO_COMPRESS
bool QHttpNetworkConnectionPrivate::expand(QAbstractSocket *socket, QHttpNetworkReply *reply, bool dataComplete)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    qint64 total = bytesAvailable(*reply, true);
    if (total >= CHUNK || dataComplete) {
        int i = indexOf(socket);
         // uncompress the data
        QByteArray content, inflated;
        read(*reply, content, -1, true);
        int ret = Z_OK;
        if (content.size())
            ret = reply->d_func()->gunzipBodyPartially(content, inflated);
        int retCheck = (dataComplete) ? Z_STREAM_END : Z_OK;
        if (ret >= retCheck) {
            if (inflated.size()) {
                reply->d_func()->totalProgress += inflated.size();
                appendData(*reply, inflated, false);
                if (emitSignals(reply)) {
                    emit reply->readyRead();
                    // make sure that the reply is valid
                    if (channels[i].reply != reply)
                        return true;
                    emit reply->dataReadProgress(reply->d_func()->totalProgress, 0);
                    // make sure that the reply is valid
                    if (channels[i].reply != reply)
                        return true;

                }
            }
        } else {
            emitReplyError(socket, reply, QNetworkReply::ProtocolFailure);
            return false;
        }
    }
    return true;
}
#endif

void QHttpNetworkConnectionPrivate::receiveReply(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
    Q_ASSERT(socket);

    Q_Q(QHttpNetworkConnection);
    qint64 bytes = 0;
    QAbstractSocket::SocketState state = socket->state();
    int i = indexOf(socket);

    // connection might be closed to signal the end of data
    if (state == QAbstractSocket::UnconnectedState) {
        if (!socket->bytesAvailable()) {
            if (reply && reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                channels[i].state = IdleState;
                allDone(socket, reply);
            } else {
                // try to reconnect/resend before sending an error.
                if (channels[i].reconnectAttempts-- > 0) {
                    resendCurrentRequest(socket);
                } else {
                    reply->d_func()->errorString = errorDetail(QNetworkReply::RemoteHostClosedError, socket);
                    emit reply->finishedWithError(QNetworkReply::RemoteHostClosedError, reply->d_func()->errorString);
                    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
                }
            }
        }
    }

    // read loop for the response
    while (socket->bytesAvailable()) {
        QHttpNetworkReplyPrivate::ReplyState state = reply ? reply->d_func()->state : QHttpNetworkReplyPrivate::AllDoneState;
        switch (state) {
        case QHttpNetworkReplyPrivate::NothingDoneState:
        case QHttpNetworkReplyPrivate::ReadingStatusState: {
            qint64 statusBytes = reply->d_func()->readStatus(socket);
            if (statusBytes == -1) {
                // error reading the status, close the socket and emit error
                socket->close();
                reply->d_func()->errorString = errorDetail(QNetworkReply::ProtocolFailure, socket);
                emit reply->finishedWithError(QNetworkReply::ProtocolFailure, reply->d_func()->errorString);
                QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
                break;
            }
            bytes += statusBytes;
            channels[i].lastStatus = reply->d_func()->statusCode;
            break;
        }
        case QHttpNetworkReplyPrivate::ReadingHeaderState:
            bytes += reply->d_func()->readHeader(socket);
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState) {
                if (reply->d_func()->isGzipped() && reply->d_func()->autoDecompress) {
                    // remove the Content-Length from header
                    reply->d_func()->removeAutoDecompressHeader();
                } else {
                    reply->d_func()->autoDecompress = false;
                }
                if (reply && reply->d_func()->statusCode == 100) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::ReadingStatusState;
                    break; // ignore
                }
                if (emitSignals(reply))
                    emit reply->headerChanged();
                if (!expectContent(reply)) {
                    reply->d_func()->state = QHttpNetworkReplyPrivate::AllDoneState;
                    channels[i].state = IdleState;
                    allDone(socket, reply);
                    return;
                }
            }
            break;
        case QHttpNetworkReplyPrivate::ReadingDataState: {
            QBuffer fragment;
            fragment.open(QIODevice::WriteOnly);
            bytes = reply->d_func()->readBody(socket, &fragment);
            if (bytes) {
                appendData(*reply, fragment.data(), reply->d_func()->autoDecompress);
                if (!reply->d_func()->autoDecompress) {
                    reply->d_func()->totalProgress += fragment.size();
                    if (emitSignals(reply)) {
                        emit reply->readyRead();
                        // make sure that the reply is valid
                        if (channels[i].reply != reply)
                            return;
                        emit reply->dataReadProgress(reply->d_func()->totalProgress, reply->d_func()->bodyLength);
                        // make sure that the reply is valid
                        if (channels[i].reply != reply)
                            return;
                    }
                }
#ifndef QT_NO_COMPRESS
                else if (!expand(socket, reply, false)) { // expand a chunk if possible
                    return; // ### expand failed
                }
#endif
            }
            if (reply->d_func()->state == QHttpNetworkReplyPrivate::ReadingDataState)
                break;
            // everything done, fall through
            }
      case QHttpNetworkReplyPrivate::AllDoneState:
            channels[i].state = IdleState;
            allDone(socket, reply);
            break;
        default:
            break;
        }
    }
}

void QHttpNetworkConnectionPrivate::allDone(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
#ifndef QT_NO_COMPRESS
    // expand the whole data.
    if (expectContent(reply) && reply->d_func()->autoDecompress && !reply->d_func()->streamEnd)
        expand(socket, reply, true); // ### if expand returns false, its an error
#endif
    // while handling 401 & 407, we might reset the status code, so save this.
    bool emitFinished = emitSignals(reply);
    handleStatus(socket, reply);
    // ### at this point there should be no more data on the socket
    // close if server requested
    int i = indexOf(socket);
    if (reply->d_func()->connectionCloseEnabled())
        closeChannel(i);
    // queue the finished signal, this is required since we might send new requests from
    // slot connected to it. The socket will not fire readyRead signal, if we are already
    // in the slot connected to readyRead
    if (emitFinished)
        QMetaObject::invokeMethod(reply, "finished", Qt::QueuedConnection);
    // reset the reconnection attempts after we receive a complete reply.
    // in case of failures, each channel will attempt two reconnects before emitting error.
    channels[i].reconnectAttempts = 2;
}

void QHttpNetworkConnectionPrivate::handleStatus(QAbstractSocket *socket, QHttpNetworkReply *reply)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    Q_Q(QHttpNetworkConnection);

    int statusCode = reply->statusCode();
    bool resend = false;

    switch (statusCode) {
    case 401:
    case 407:
        if (handleAuthenticateChallenge(socket, reply, (statusCode == 407), resend)) {
            if (resend) {
                eraseData(reply);
                sendRequest(socket);
            }
        } else {
            int i = indexOf(socket);
            emit channels[i].reply->headerChanged();
            emit channels[i].reply->readyRead();
            QNetworkReply::NetworkError errorCode = (statusCode == 407)
                ? QNetworkReply::ProxyAuthenticationRequiredError
                : QNetworkReply::AuthenticationRequiredError;
            reply->d_func()->errorString = errorDetail(errorCode, socket);
            emit q->error(errorCode, reply->d_func()->errorString);
            emit channels[i].reply->finished();
        }
        break;
    default:
        QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    }
}

void QHttpNetworkConnectionPrivate::copyCredentials(int fromChannel, QAuthenticator *auth, bool isProxy)
{
    Q_ASSERT(auth);

    // select another channel
    QAuthenticator* otherAuth = 0;
    for (int i = 0; i < channelCount; ++i) {
        if (i == fromChannel)
            continue;
        if (isProxy)
            otherAuth = &channels[i].proxyAuthenticator;
        else
            otherAuth = &channels[i].authenticator;
        // if the credentials are different, copy them
        if (otherAuth->user().compare(auth->user()))
            otherAuth->setUser(auth->user());
        if (otherAuth->password().compare(auth->password()))
            otherAuth->setPassword(auth->password());
    }
}


bool QHttpNetworkConnectionPrivate::handleAuthenticateChallenge(QAbstractSocket *socket, QHttpNetworkReply *reply,
                                                                bool isProxy, bool &resend)
{
    Q_ASSERT(socket);
    Q_ASSERT(reply);

    Q_Q(QHttpNetworkConnection);

    resend = false;
    //create the response header to be used with QAuthenticatorPrivate.
    QHttpResponseHeader responseHeader;
    QList<QPair<QByteArray, QByteArray> > fields = reply->header();
    QList<QPair<QByteArray, QByteArray> >::const_iterator it = fields.constBegin();
    while (it != fields.constEnd()) {
        responseHeader.addValue(QString::fromLatin1(it->first), QString::fromUtf8(it->second));
        it++;
    }
    //find out the type of authentication protocol requested.
    QAuthenticatorPrivate::Method authMethod = reply->d_func()->authenticationMethod(isProxy);
    if (authMethod != QAuthenticatorPrivate::None) {
        int i = indexOf(socket);
        //Use a single authenticator for all domains. ### change later to use domain/realm
        QAuthenticator* auth = 0;
        if (isProxy) {
            auth = &channels[i].proxyAuthenticator;
            channels[i].proxyAuthMehtod = authMethod;
        } else {
            auth = &channels[i].authenticator;
            channels[i].authMehtod = authMethod;
        }
        //proceed with the authentication.
        if (auth->isNull())
            auth->detach();
        QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(*auth);
        priv->parseHttpResponse(responseHeader, isProxy);

        if (priv->phase == QAuthenticatorPrivate::Done) {
            if ((isProxy && pendingProxyAuthSignal) ||(!isProxy && pendingAuthSignal)) {
                // drop the request
                eraseData(channels[i].reply);
                closeChannel(i);
                channels[i].lastStatus = 0;
                channels[i].state =  Wait4AuthState;
                return false;
            }
            // cannot use this socket until the slot returns
            channels[i].state = WaitingState;
            socket->blockSignals(true);
            if (!isProxy) {
                pendingAuthSignal = true;
                emit q->authenticationRequired(reply->request(), auth, q);
                pendingAuthSignal = false;
#ifndef QT_NO_NETWORKPROXY
            } else {
                pendingProxyAuthSignal = true;
                emit q->proxyAuthenticationRequired(networkProxy, auth, q);
                pendingProxyAuthSignal = false;
#endif
            }
            socket->blockSignals(false);
            // socket free to use
            channels[i].state = IdleState;
            if (priv->phase != QAuthenticatorPrivate::Done) {
                // send any pending requests
                copyCredentials(i,  auth, isProxy);
                QMetaObject::invokeMethod(q, "_q_restartPendingRequest", Qt::QueuedConnection);
            }
        }
        // changing values in QAuthenticator will reset the 'phase'
        if (priv->phase == QAuthenticatorPrivate::Done) {
            // authentication is cancelled, send the current contents to the user.
            emit channels[i].reply->headerChanged();
            emit channels[i].reply->readyRead();
            QNetworkReply::NetworkError errorCode =
                isProxy
                ? QNetworkReply::ProxyAuthenticationRequiredError
                : QNetworkReply::AuthenticationRequiredError;
            reply->d_func()->errorString = errorDetail(errorCode, socket);
            emit q->error(errorCode, reply->d_func()->errorString);
            emit channels[i].reply->finished();
            // ### at this point the reply could be deleted
            socket->close();
            // remove pending request on the other channels
            for (int j = 0; j < channelCount; ++j) {
                if (j != i && channels[j].state ==  Wait4AuthState)
                    channels[j].state = IdleState;
            }
            return true;
        }
        //resend the request
        resend = true;
        return true;
    }
    return false;
}

void QHttpNetworkConnectionPrivate::createAuthorization(QAbstractSocket *socket, QHttpNetworkRequest &request)
{
    Q_ASSERT(socket);

    int i = indexOf(socket);
    if (channels[i].authMehtod != QAuthenticatorPrivate::None) {
        if (!(channels[i].authMehtod == QAuthenticatorPrivate::Ntlm && channels[i].lastStatus != 401)) {
            QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[i].authenticator);
            if (priv && priv->method != QAuthenticatorPrivate::None) {
                QByteArray response = priv->calculateResponse(request.d->methodName(), request.d->uri(false));
                request.setHeaderField("authorization", response);
            }
        }
    }
    if (channels[i].proxyAuthMehtod != QAuthenticatorPrivate::None) {
        if (!(channels[i].proxyAuthMehtod == QAuthenticatorPrivate::Ntlm && channels[i].lastStatus != 407)) {
            QAuthenticatorPrivate *priv = QAuthenticatorPrivate::getPrivate(channels[i].proxyAuthenticator);
            if (priv && priv->method != QAuthenticatorPrivate::None) {
                QByteArray response = priv->calculateResponse(request.d->methodName(), request.d->uri(false));
                request.setHeaderField("proxy-authorization", response);
            }
        }
    }
}

QHttpNetworkReply* QHttpNetworkConnectionPrivate::queueRequest(const QHttpNetworkRequest &request)
{
    Q_Q(QHttpNetworkConnection);

    // The reply component of the pair is created initially.
    QHttpNetworkReply *reply = new QHttpNetworkReply(request.url());
    reply->setRequest(request);
    reply->d_func()->connection = q;
    HttpMessagePair pair = qMakePair(request, reply);

    switch (request.priority()) {
    case QHttpNetworkRequest::HighPriority:
        highPriorityQueue.prepend(pair);
        break;
    case QHttpNetworkRequest::NormalPriority:
    case QHttpNetworkRequest::LowPriority:
        lowPriorityQueue.prepend(pair);
        break;
    }
    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    return reply;
}

void QHttpNetworkConnectionPrivate::unqueueRequest(QAbstractSocket *socket)
{
    Q_ASSERT(socket);

    int i = indexOf(socket);

    if (!highPriorityQueue.isEmpty()) {
        for (int j = highPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair &messagePair = highPriorityQueue[j];
            if (!messagePair.second->d_func()->requestIsPrepared)
                prepareRequest(messagePair);
            if (!messagePair.second->d_func()->requestIsBuffering) {
                channels[i].request = messagePair.first;
                channels[i].reply = messagePair.second;
                sendRequest(socket);
                highPriorityQueue.removeAt(j);
                return;
            }
        }
    }

    if (!lowPriorityQueue.isEmpty()) {
        for (int j = lowPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair &messagePair = lowPriorityQueue[j];
            if (!messagePair.second->d_func()->requestIsPrepared)
                prepareRequest(messagePair);
            if (!messagePair.second->d_func()->requestIsBuffering) {
                channels[i].request = messagePair.first;
                channels[i].reply = messagePair.second;
                sendRequest(socket);
                lowPriorityQueue.removeAt(j);
                return;
            }
        }
    }
}

void QHttpNetworkConnectionPrivate::closeChannel(int channel)
{
    QAbstractSocket *socket = channels[channel].socket;
    socket->blockSignals(true);
    socket->close();
    socket->blockSignals(false);
    channels[channel].state = IdleState;
}

void QHttpNetworkConnectionPrivate::resendCurrentRequest(QAbstractSocket *socket)
{
    Q_Q(QHttpNetworkConnection);
    Q_ASSERT(socket);
    int i = indexOf(socket);
    closeChannel(i);
    channels[i].resendCurrent = true;
    QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
}

QString QHttpNetworkConnectionPrivate::errorDetail(QNetworkReply::NetworkError errorCode, QAbstractSocket* socket)
{
    Q_ASSERT(socket);

    QString errorString;
    switch (errorCode) {
    case QNetworkReply::HostNotFoundError:
        errorString = QString::fromLatin1(QT_TRANSLATE_NOOP("QHttp", "Host %1 not found"))
                              .arg(socket->peerName());
        break;
    case QNetworkReply::ConnectionRefusedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Connection refused"));
        break;
    case QNetworkReply::RemoteHostClosedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Connection closed"));
        break;
    case QNetworkReply::TimeoutError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "HTTP request failed"));
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Proxy requires authentication"));
        break;
    case QNetworkReply::AuthenticationRequiredError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Host requires authentication"));
        break;
    case QNetworkReply::ProtocolFailure:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Data corrupted"));
        break;
    case QNetworkReply::ProtocolUnknownError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "Unknown protocol specified"));
        break;
    case QNetworkReply::SslHandshakeFailedError:
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "SSL handshake failed"));
        break;
    default:
        // all other errors are treated as QNetworkReply::UnknownNetworkError
        errorString = QLatin1String(QT_TRANSLATE_NOOP("QHttp", "HTTP request failed"));
        break;
    }
    return errorString;
}

void QHttpNetworkConnectionPrivate::removeReply(QHttpNetworkReply *reply)
{
    Q_Q(QHttpNetworkConnection);

    // remove the from active list.
    for (int i = 0; i < channelCount; ++i) {
        if (channels[i].reply == reply) {
            channels[i].reply = 0;
            closeChannel(i);
            QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
            return;
        }
    }
    // remove from the high priority queue
    if (!highPriorityQueue.isEmpty()) {
        for (int j = highPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair messagePair = highPriorityQueue.at(j);
            if (messagePair.second == reply) {
                highPriorityQueue.removeAt(j);
                QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
                return;
            }
        }
    }
    // remove from the low priority queue
    if (!lowPriorityQueue.isEmpty()) {
        for (int j = lowPriorityQueue.count() - 1; j >= 0; --j) {
            HttpMessagePair messagePair = lowPriorityQueue.at(j);
            if (messagePair.second == reply) {
                lowPriorityQueue.removeAt(j);
                QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
                return;
            }
        }
    }
}


//private slots
void QHttpNetworkConnectionPrivate::_q_readyRead()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    if (isSocketWaiting(socket) || isSocketReading(socket)) {
        int i = indexOf(socket);
        channels[i].state = ReadingState;
        if (channels[i].reply)
            receiveReply(socket, channels[i].reply);
    }
    // ### error
}

void QHttpNetworkConnectionPrivate::_q_bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    if (isSocketWriting(socket))
        sendRequest(socket);
    // otherwise we do nothing
}

void QHttpNetworkConnectionPrivate::_q_disconnected()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    // read the available data before closing
    int i = indexOf(socket);
    if (isSocketWaiting(socket) || isSocketReading(socket)) {
        channels[i].state = ReadingState;
        if (channels[i].reply)
            receiveReply(socket, channels[i].reply);
    } else if (channels[i].state == IdleState && channels[i].resendCurrent) {
        // re-sending request because the socket was in ClosingState
        QMetaObject::invokeMethod(q, "_q_startNextRequest", Qt::QueuedConnection);
    }
    channels[i].state = IdleState;
}

void QHttpNetworkConnectionPrivate::_q_startNextRequest()
{
    // send the current request again
    if (channels[0].resendCurrent || channels[1].resendCurrent) {
        int i = channels[0].resendCurrent ? 0:1;
        QAbstractSocket *socket = channels[i].socket;
        channels[i].resendCurrent = false;
        channels[i].state = IdleState;
        if (channels[i].reply)
            sendRequest(socket);
        return;
    }
    // send the request using the idle socket
    QAbstractSocket *socket = channels[0].socket;
    if (isSocketBusy(socket)) {
        socket = (isSocketBusy(channels[1].socket) ? 0 :channels[1].socket);
    }

    if (!socket) {
        return; // this will be called after finishing current request.
    }
    unqueueRequest(socket);
}

void QHttpNetworkConnectionPrivate::_q_restartPendingRequest()
{
    // send the request using the idle socket
    for (int i = 0 ; i < channelCount; ++i) {
        QAbstractSocket *socket = channels[i].socket;
        if (channels[i].state ==  Wait4AuthState) {
            channels[i].state = IdleState;
            if (channels[i].reply)
                sendRequest(socket);
        }
    }
}

void QHttpNetworkConnectionPrivate::_q_connected()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    int i = indexOf(socket);
    // ### FIXME: if the server closes the connection unexpectedly, we shouldn't send the same broken request again!
    //channels[i].reconnectAttempts = 2;
    if (!channels[i].pendingEncrypt) {
        channels[i].state = IdleState;
        if (channels[i].reply)
            sendRequest(socket);
        else
            closeChannel(i);
    }
}


void QHttpNetworkConnectionPrivate::_q_error(QAbstractSocket::SocketError socketError)
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return;
    bool send2Reply = false;
    int i = indexOf(socket);
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
        if (channels[i].state != IdleState && channels[i].state != ReadingState) {
            if (channels[i].reconnectAttempts-- > 0) {
                resendCurrentRequest(socket);
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
        if (channels[i].state == WritingState &&  (channels[i].reconnectAttempts-- > 0)) {
            resendCurrentRequest(socket);
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
    QPointer<QObject> that = q;
    QString errorString = errorDetail(errorCode, socket);
    if (send2Reply) {
        if (channels[i].reply) {
            channels[i].reply->d_func()->errorString = errorString;
            // this error matters only to this reply
            emit channels[i].reply->finishedWithError(errorCode, errorString);
        }
        // send the next request
        QMetaObject::invokeMethod(that, "_q_startNextRequest", Qt::QueuedConnection);
    } else {
        // the failure affects all requests.
        emit q->error(errorCode, errorString);
    }
    if (that) //signals make enter the event loop
        closeChannel(i);
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnectionPrivate::_q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator* auth)
{
    Q_Q(QHttpNetworkConnection);
    emit q->proxyAuthenticationRequired(proxy, auth, q);
}
#endif

void QHttpNetworkConnectionPrivate::_q_dataReadyReadNoBuffer()
{
    Q_Q(QHttpNetworkConnection);
    // data emitted either readyRead()
    // find out which channel it is for
    QIODevice *sender = qobject_cast<QIODevice *>(q->sender());

    // won't match anything if the qobject_cast above failed
    for (int i = 0; i < channelCount; ++i) {
        if (sender == channels[i].request.data()) {
            sendRequest(channels[i].socket);
            break;
        }
    }
}

void QHttpNetworkConnectionPrivate::_q_dataReadyReadBuffer()
{
    Q_Q(QHttpNetworkConnection);
    QIODevice *sender = qobject_cast<QIODevice *>(q->sender());
    HttpMessagePair *thePair = 0;
    for (int i = 0; !thePair && i < lowPriorityQueue.size(); ++i)
        if (lowPriorityQueue.at(i).first.data() == sender)
            thePair = &lowPriorityQueue[i];

    for (int i = 0; !thePair && i < highPriorityQueue.size(); ++i)
        if (highPriorityQueue.at(i).first.data() == sender)
            thePair = &highPriorityQueue[i];

    if (thePair) {
        bufferData(*thePair);

        // are we finished buffering?
        if (!thePair->second->d_func()->requestIsBuffering)
            _q_startNextRequest();
    }
}

void QHttpNetworkConnectionPrivate::bufferData(HttpMessagePair &messagePair)
{
    Q_Q(QHttpNetworkConnection);
    QHttpNetworkRequest &request = messagePair.first;
    QHttpNetworkReply *reply = messagePair.second;
    Q_ASSERT(request.data());
    if (!reply->d_func()->requestIsBuffering) { // first time
        QObject::connect(request.data(), SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadBuffer()));
        QObject::connect(request.data(), SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadBuffer()));
        reply->d_func()->requestIsBuffering = true;
        reply->d_func()->requestDataBuffer.open(QIODevice::ReadWrite);
    }

    // always try to read at least one byte
    // ### FIXME! use a QRingBuffer
    qint64 bytesToRead = qMax<qint64>(1, request.data()->bytesAvailable());
    QByteArray newData;
    newData.resize(bytesToRead);
    qint64 bytesActuallyRead = request.data()->read(newData.data(), bytesToRead);

    if (bytesActuallyRead > 0) {
        // we read something
        newData.chop(bytesToRead - bytesActuallyRead);
        reply->d_func()->requestDataBuffer.write(newData);
    } else if (bytesActuallyRead == -1) { // last time
        QObject::disconnect(request.data(), SIGNAL(readyRead()), q, SLOT(_q_dataReadyReadBuffer()));
        QObject::disconnect(request.data(), SIGNAL(readChannelFinished()), q, SLOT(_q_dataReadyReadBuffer()));

        request.setContentLength(reply->d_func()->requestDataBuffer.size());
        reply->d_func()->requestDataBuffer.seek(0);
        reply->d_func()->requestIsBuffering = false;
    }
}

// QHttpNetworkConnection

QHttpNetworkConnection::QHttpNetworkConnection(const QString &hostName, quint16 port, bool encrypt, QObject *parent)
    : QObject(*(new QHttpNetworkConnectionPrivate(hostName, port, encrypt)), parent)
{
    Q_D(QHttpNetworkConnection);
    d->init();
}

QHttpNetworkConnection::~QHttpNetworkConnection()
{
}

QString QHttpNetworkConnection::hostName() const
{
    Q_D(const QHttpNetworkConnection);
    return d->hostName;
}

quint16 QHttpNetworkConnection::port() const
{
    Q_D(const QHttpNetworkConnection);
    return d->port;
}

QHttpNetworkReply* QHttpNetworkConnection::sendRequest(const QHttpNetworkRequest &request)
{
    Q_D(QHttpNetworkConnection);
    return d->queueRequest(request);
}

void QHttpNetworkConnection::enableEncryption()
{
    Q_D(QHttpNetworkConnection);
    d->encrypt = true;
}

bool QHttpNetworkConnection::isEncrypted() const
{
    Q_D(const QHttpNetworkConnection);
    return d->encrypt;
}

void QHttpNetworkConnection::setProxyAuthentication(QAuthenticator *authenticator)
{
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].proxyAuthenticator = *authenticator;
}

void QHttpNetworkConnection::setAuthentication(const QString &domain, QAuthenticator *authenticator)
{
    Q_UNUSED(domain); // ### domain ?
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].authenticator = *authenticator;
}

#ifndef QT_NO_NETWORKPROXY
void QHttpNetworkConnection::setCacheProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QHttpNetworkConnection);
    d->networkProxy = networkProxy;
    // update the authenticator
    if (!d->networkProxy.user().isEmpty()) {
        for (int i = 0; i < d->channelCount; ++i) {
            d->channels[i].proxyAuthenticator.setUser(d->networkProxy.user());
            d->channels[i].proxyAuthenticator.setPassword(d->networkProxy.password());
        }
    }
}

QNetworkProxy QHttpNetworkConnection::cacheProxy() const
{
    Q_D(const QHttpNetworkConnection);
    return d->networkProxy;
}

void QHttpNetworkConnection::setTransparentProxy(const QNetworkProxy &networkProxy)
{
    Q_D(QHttpNetworkConnection);
    for (int i = 0; i < d->channelCount; ++i)
        d->channels[i].socket->setProxy(networkProxy);
}

QNetworkProxy QHttpNetworkConnection::transparentProxy() const
{
    Q_D(const QHttpNetworkConnection);
    return d->channels[0].socket->proxy();
}
#endif


// SSL support below
#ifndef QT_NO_OPENSSL
void QHttpNetworkConnectionPrivate::_q_encrypted()
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return; // ### error
    channels[indexOf(socket)].state = IdleState;
    sendRequest(socket);
}

void QHttpNetworkConnectionPrivate::_q_sslErrors(const QList<QSslError> &errors)
{
    Q_Q(QHttpNetworkConnection);
    QAbstractSocket *socket = qobject_cast<QAbstractSocket*>(q->sender());
    if (!socket)
        return;
    //QNetworkReply::NetworkError errorCode = QNetworkReply::ProtocolFailure;
    emit q->sslErrors(errors);
}

QSslConfiguration QHttpNetworkConnectionPrivate::sslConfiguration(const QHttpNetworkReply &reply) const
{
    for (int i = 0; i < channelCount; ++i)
        if (channels[i].reply == &reply)
            return static_cast<QSslSocket *>(channels[0].socket)->sslConfiguration();
    return QSslConfiguration(); // pending or done request
}

void QHttpNetworkConnection::setSslConfiguration(const QSslConfiguration &config)
{
    Q_D(QHttpNetworkConnection);
    // set the config on all channels
    for (int i = 0; i < d->channelCount; ++i)
        static_cast<QSslSocket *>(d->channels[i].socket)->setSslConfiguration(config);
}

void QHttpNetworkConnection::ignoreSslErrors(int channel)
{
    Q_D(QHttpNetworkConnection);
    if (channel == -1) { // ignore for all channels
        for (int i = 0; i < d->channelCount; ++i) {
            static_cast<QSslSocket *>(d->channels[i].socket)->ignoreSslErrors();
            d->channels[i].ignoreSSLErrors = true;
        }

    } else {
        static_cast<QSslSocket *>(d->channels[channel].socket)->ignoreSslErrors();
        d->channels[channel].ignoreSSLErrors = true;
    }
}


#endif //QT_NO_OPENSSL


QT_END_NAMESPACE

#include "moc_qhttpnetworkconnection_p.cpp"

#endif // QT_NO_HTTP
