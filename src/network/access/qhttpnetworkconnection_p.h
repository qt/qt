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

#ifndef QHTTPNETWORKCONNECTION_H
#define QHTTPNETWORKCONNECTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the Network Access API.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qabstractsocket.h>

#include <private/qobject_p.h>
#include <qauthenticator.h>
#include <qnetworkproxy.h>
#include <qbuffer.h>

#include <private/qhttpnetworkheader_p.h>
#include <private/qhttpnetworkrequest_p.h>
#include <private/qhttpnetworkreply_p.h>


#ifndef QT_NO_HTTP

#ifndef QT_NO_OPENSSL
#    include <QtNetwork/qsslsocket.h>
#    include <QtNetwork/qsslerror.h>
#else
#   include <QtNetwork/qtcpsocket.h>
#endif

QT_BEGIN_NAMESPACE

class QHttpNetworkRequest;
class QHttpNetworkReply;

class QHttpNetworkConnectionPrivate;
class Q_AUTOTEST_EXPORT QHttpNetworkConnection : public QObject
{
    Q_OBJECT
public:

    QHttpNetworkConnection(const QString &hostName, quint16 port = 80, bool encrypt = false, QObject *parent = 0);
    ~QHttpNetworkConnection();

    //The hostname to which this is connected to.
    QString hostName() const;
    //The HTTP port in use.
    quint16 port() const;

    //add a new HTTP request through this connection
    QHttpNetworkReply* sendRequest(const QHttpNetworkRequest &request);

#ifndef QT_NO_NETWORKPROXY
    //set the proxy for this connection
    void setCacheProxy(const QNetworkProxy &networkProxy);
    QNetworkProxy cacheProxy() const;
    void setTransparentProxy(const QNetworkProxy &networkProxy);
    QNetworkProxy transparentProxy() const;
#endif

    //enable encryption
    void enableEncryption();
    bool isEncrypted() const;

    //authentication parameters
    void setProxyAuthentication(QAuthenticator *authenticator);
    void setAuthentication(const QString &domain, QAuthenticator *authenticator);

#ifndef QT_NO_OPENSSL
    void setSslConfiguration(const QSslConfiguration &config);
    void ignoreSslErrors(int channel = -1);

Q_SIGNALS:
    void sslErrors(const QList<QSslError> &errors);
#endif

Q_SIGNALS:
#ifndef QT_NO_NETWORKPROXY
    //cannot be used with queued connection.
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator,
                                     const QHttpNetworkConnection *connection = 0);
#endif
    void authenticationRequired(const QHttpNetworkRequest &request, QAuthenticator *authenticator,
                                const QHttpNetworkConnection *connection = 0);
    void error(QNetworkReply::NetworkError errorCode, const QString &detail = QString());

private:
    Q_DECLARE_PRIVATE(QHttpNetworkConnection)
    Q_DISABLE_COPY(QHttpNetworkConnection)
    friend class QHttpNetworkReply;

    Q_PRIVATE_SLOT(d_func(), void _q_bytesWritten(qint64))
    Q_PRIVATE_SLOT(d_func(), void _q_readyRead())
    Q_PRIVATE_SLOT(d_func(), void _q_disconnected())
    Q_PRIVATE_SLOT(d_func(), void _q_startNextRequest())
    Q_PRIVATE_SLOT(d_func(), void _q_restartPendingRequest())
    Q_PRIVATE_SLOT(d_func(), void _q_connected())
    Q_PRIVATE_SLOT(d_func(), void _q_error(QAbstractSocket::SocketError))
#ifndef QT_NO_NETWORKPROXY
    Q_PRIVATE_SLOT(d_func(), void _q_proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*))
#endif
    Q_PRIVATE_SLOT(d_func(), void _q_dataReadyReadBuffer())
    Q_PRIVATE_SLOT(d_func(), void _q_dataReadyReadNoBuffer())

#ifndef QT_NO_OPENSSL
    Q_PRIVATE_SLOT(d_func(), void _q_encrypted())
    Q_PRIVATE_SLOT(d_func(), void _q_sslErrors(const QList<QSslError>&))
#endif
};




// private classes
typedef QPair<QHttpNetworkRequest, QHttpNetworkReply*> HttpMessagePair;


class QHttpNetworkConnectionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QHttpNetworkConnection)
public:
    QHttpNetworkConnectionPrivate(const QString &hostName, quint16 port, bool encrypt);
    ~QHttpNetworkConnectionPrivate();
    void init();
    void connectSignals(QAbstractSocket *socket);

    enum SocketState {
        IdleState = 0,          // ready to send request
        ConnectingState = 1,    // connecting to host
        WritingState = 2,       // writing the data
        WaitingState = 4,       // waiting for reply
        ReadingState = 8,       // reading the reply
        Wait4AuthState = 0x10,  // blocked for send till the current authentication slot is done
        BusyState = (ConnectingState|WritingState|WaitingState|ReadingState|Wait4AuthState)
    };

    enum { ChunkSize = 4096 };

    int indexOf(QAbstractSocket *socket) const;
    bool isSocketBusy(QAbstractSocket *socket) const;
    bool isSocketWriting(QAbstractSocket *socket) const;
    bool isSocketWaiting(QAbstractSocket *socket) const;
    bool isSocketReading(QAbstractSocket *socket) const;

    QHttpNetworkReply *queueRequest(const QHttpNetworkRequest &request);
    void unqueueRequest(QAbstractSocket *socket);
    void prepareRequest(HttpMessagePair &request);
    bool sendRequest(QAbstractSocket *socket);
    void receiveReply(QAbstractSocket *socket, QHttpNetworkReply *reply);
    void resendCurrentRequest(QAbstractSocket *socket);
    void closeChannel(int channel);
    void copyCredentials(int fromChannel, QAuthenticator *auth, bool isProxy);

    // private slots
    void _q_bytesWritten(qint64 bytes); // proceed sending
    void _q_readyRead(); // pending data to read
    void _q_disconnected(); // disconnected from host
    void _q_startNextRequest(); // send the next request from the queue
    void _q_restartPendingRequest(); // send the currently blocked request
    void _q_connected(); // start sending request
    void _q_error(QAbstractSocket::SocketError); // error from socket
#ifndef QT_NO_NETWORKPROXY
    void _q_proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *auth); // from transparent proxy
#endif
    void _q_dataReadyReadNoBuffer();
    void _q_dataReadyReadBuffer();

    void createAuthorization(QAbstractSocket *socket, QHttpNetworkRequest &request);
    bool ensureConnection(QAbstractSocket *socket);
    QString errorDetail(QNetworkReply::NetworkError errorCode, QAbstractSocket *socket);
    void eraseData(QHttpNetworkReply *reply);
#ifndef QT_NO_COMPRESS
    bool expand(QAbstractSocket *socket, QHttpNetworkReply *reply, bool dataComplete);
#endif
    void bufferData(HttpMessagePair &request);
    void removeReply(QHttpNetworkReply *reply);

    QString hostName;
    quint16 port;
    bool encrypt;

    struct Channel {
        QAbstractSocket *socket;
        SocketState state;
        QHttpNetworkRequest request; // current request
        QHttpNetworkReply *reply; // current reply for this request
        qint64 written;
        qint64 bytesTotal;
        bool resendCurrent;
        int lastStatus; // last status received on this channel
        bool pendingEncrypt; // for https (send after encrypted)
        int reconnectAttempts; // maximum 2 reconnection attempts
        QAuthenticatorPrivate::Method authMehtod;
        QAuthenticatorPrivate::Method proxyAuthMehtod;
        QAuthenticator authenticator;
        QAuthenticator proxyAuthenticator;
#ifndef QT_NO_OPENSSL
        bool ignoreSSLErrors;
#endif
        Channel() :state(IdleState), reply(0), written(0), bytesTotal(0), resendCurrent(false), reconnectAttempts(2),
            authMehtod(QAuthenticatorPrivate::None), proxyAuthMehtod(QAuthenticatorPrivate::None)
#ifndef QT_NO_OPENSSL
            , ignoreSSLErrors(false)
#endif
        {}
    };
    static const int channelCount;
    Channel channels[2]; // maximum of 2 socket connections to the server
    bool pendingAuthSignal; // there is an incomplete authentication signal
    bool pendingProxyAuthSignal; // there is an incomplete proxy authentication signal

    void appendData(QHttpNetworkReply &reply, const QByteArray &fragment, bool compressed);
    qint64 bytesAvailable(const QHttpNetworkReply &reply, bool compressed = false) const;
    qint64 read(QHttpNetworkReply &reply, QByteArray &data, qint64 maxSize, bool compressed);
    void emitReplyError(QAbstractSocket *socket, QHttpNetworkReply *reply, QNetworkReply::NetworkError errorCode);
    bool handleAuthenticateChallenge(QAbstractSocket *socket, QHttpNetworkReply *reply, bool isProxy, bool &resend);
    void allDone(QAbstractSocket *socket, QHttpNetworkReply *reply);
    void handleStatus(QAbstractSocket *socket, QHttpNetworkReply *reply);
    inline bool emitSignals(QHttpNetworkReply *reply);
    inline bool expectContent(QHttpNetworkReply *reply);

#ifndef QT_NO_OPENSSL
    void _q_encrypted(); // start sending request (https)
    void _q_sslErrors(const QList<QSslError> &errors); // ssl errors from the socket
    QSslConfiguration sslConfiguration(const QHttpNetworkReply &reply) const;
#endif

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy networkProxy;
#endif

    //The request queues
    QList<HttpMessagePair> highPriorityQueue;
    QList<HttpMessagePair> lowPriorityQueue;
};



QT_END_NAMESPACE

//Q_DECLARE_METATYPE(QHttpNetworkRequest)
//Q_DECLARE_METATYPE(QHttpNetworkReply)

#endif // QT_NO_HTTP

#endif
