/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtNetwork/QtNetwork>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
// Current path (C:\private\<UID>) contains only ascii chars
//#define SRCDIR QDir::currentPath()
#define SRCDIR "."
#endif

#include "../network-settings.h"

class tst_NetworkSelfTest: public QObject
{
    Q_OBJECT
    QHostAddress cachedIpAddress;
public:
    tst_NetworkSelfTest();
    virtual ~tst_NetworkSelfTest();

    QHostAddress serverIpAddress();

private slots:
    void hostTest();
    void dnsResolution_data();
    void dnsResolution();
    void serverReachability();
    void remotePortsOpen_data();
    void remotePortsOpen();
    void fileLineEndingTest();

    // specific protocol tests
    void ftpServer();
    void imapServer();
    void httpServer();
    void httpsServer();
    void httpProxy();
    void httpProxyBasicAuth();
    void httpProxyNtlmAuth();
    void socks5Proxy();
    void socks5ProxyAuth();

    // ssl supported test
    void supportsSsl();
};

class Chat
{
public:
    enum Type {
        Reconnect,
        Send,
        Expect,
        SkipBytes,
        DiscardUntil,
        DiscardUntilDisconnect,
        Disconnect,
        RemoteDisconnect,
        StartEncryption
    };
    Chat(Type t, const QByteArray &d)
        : data(d), type(t)
    {
    }
    Chat(Type t, int val = 0)
        : value(val), type(t)
    {
    }

    static inline Chat send(const QByteArray &data)
    { return Chat(Send, data); }
    static inline Chat expect(const QByteArray &data)
    { return Chat(Expect, data); }
    static inline Chat discardUntil(const QByteArray &data)
    { return Chat(DiscardUntil, data); }
    static inline Chat skipBytes(int count)
    { return Chat(SkipBytes, count); }

    QByteArray data;
    int value;
    Type type;
};

static QString prettyByteArray(const QByteArray &array)
{
    // any control chars?
    QString result;
    result.reserve(array.length() + array.length() / 3);
    for (int i = 0; i < array.length(); ++i) {
        char c = array.at(i);
        switch (c) {
        case '\n':
            result += "\\n";
            continue;
        case '\r':
            result += "\\r";
            continue;
        case '\t':
            result += "\\t";
            continue;
        case '"':
            result += "\\\"";
            continue;
        default:
            break;
        }

        if (c < 0x20 || uchar(c) >= 0x7f) {
            result += '\\';
            result += QString::number(uchar(c), 8);
        } else {
            result += c;
        }
    }
    return result;
}

static bool doSocketRead(QTcpSocket *socket, int minBytesAvailable, int timeout = 4000)
{
    QTime timer;
    timer.start();
    forever {
        if (socket->bytesAvailable() >= minBytesAvailable)
            return true;
        if (socket->state() == QAbstractSocket::UnconnectedState
            || timer.elapsed() >= timeout)
            return false;
        if (!socket->waitForReadyRead(timeout - timer.elapsed()))
            return false;
    }
}

static bool doSocketFlush(QTcpSocket *socket, int timeout = 4000)
{
#ifndef QT_NO_OPENSSL
    QSslSocket *sslSocket = qobject_cast<QSslSocket *>(socket);
#endif
    QTime timer;
    timer.start();
    forever {
        if (socket->bytesToWrite() == 0
#ifndef QT_NO_OPENSSL
            && sslSocket->encryptedBytesToWrite() == 0
#endif
            )
            return true;
        if (socket->state() == QAbstractSocket::UnconnectedState
            || timer.elapsed() >= timeout)
            return false;
        if (!socket->waitForBytesWritten(timeout - timer.elapsed()))
            return false;
    }
}

static void netChat(int port, const QList<Chat> &chat)
{
#ifndef QT_NO_OPENSSL
    QSslSocket socket;
#else
    QTcpSocket socket;
#endif

    socket.connectToHost(QtNetworkSettings::serverName(), port);
    qDebug() << 0 << "Connecting to server on port" << port;
    QVERIFY2(socket.waitForConnected(10000),
             QString("Failed to connect to server in step 0: %1").arg(socket.errorString()).toLocal8Bit());

    // now start the chat
    QList<Chat>::ConstIterator it = chat.constBegin();
    for (int i = 1; it != chat.constEnd(); ++it, ++i) {
        switch (it->type) {
            case Chat::Expect: {
                    qDebug() << i << "Expecting" << prettyByteArray(it->data);
                    if (!doSocketRead(&socket, it->data.length()))
                        QFAIL(QString("Failed to receive data in step %1: timeout").arg(i).toLocal8Bit());

                    // pop that many bytes off the socket
                    QByteArray received = socket.read(it->data.length());

                    // is it what we expected?
                    QVERIFY2(received == it->data,
                             QString("Did not receive expected data in step %1: data received was:\n%2")
                             .arg(i).arg(prettyByteArray(received)).toLocal8Bit());

                    break;
                }

            case Chat::DiscardUntil:
                qDebug() << i << "Discarding until" << prettyByteArray(it->data);
                while (true) {
                    // scan the buffer until we have our string
                    if (!doSocketRead(&socket, it->data.length()))
                        QFAIL(QString("Failed to receive data in step %1: timeout").arg(i).toLocal8Bit());

                    QByteArray buffer;
                    buffer.resize(socket.bytesAvailable());
                    socket.peek(buffer.data(), socket.bytesAvailable());

                    int pos = buffer.indexOf(it->data);
                    if (pos == -1) {
                        // data not found, keep trying
                        continue;
                    }

                    buffer = socket.read(pos + it->data.length());
                    qDebug() << i << "Discarded" << prettyByteArray(buffer);
                    break;
                }
                break;

            case Chat::SkipBytes: {
                    qDebug() << i << "Skipping" << it->value << "bytes";
                    if (!doSocketRead(&socket, it->value))
                        QFAIL(QString("Failed to receive data in step %1: timeout").arg(i).toLocal8Bit());

                    // now discard the bytes
                    QByteArray buffer = socket.read(it->value);
                    qDebug() << i << "Skipped" << prettyByteArray(buffer);
                    break;
                }

            case Chat::Send: {
                    qDebug() << i << "Sending" << prettyByteArray(it->data);
                    socket.write(it->data);
                    if (!doSocketFlush(&socket)) {
                        QVERIFY2(socket.state() == QAbstractSocket::ConnectedState,
                                 QString("Socket disconnected while sending data in step %1").arg(i).toLocal8Bit());
                        QFAIL(QString("Failed to send data in step %1: timeout").arg(i).toLocal8Bit());
                    }
                    break;
                }

            case Chat::Disconnect:
                qDebug() << i << "Disconnecting from host";
                socket.disconnectFromHost();

                // is this the last command?
                if (it + 1 != chat.constEnd())
                    break;

                // fall through:
            case Chat::RemoteDisconnect:
            case Chat::DiscardUntilDisconnect:
                qDebug() << i << "Waiting for remote disconnect";
                if (socket.state() != QAbstractSocket::UnconnectedState)
                    socket.waitForDisconnected(10000);
                QVERIFY2(socket.state() == QAbstractSocket::UnconnectedState,
                         QString("Socket did not disconnect as expected in step %1").arg(i).toLocal8Bit());

                // any data left?
                if (it->type == Chat::DiscardUntilDisconnect) {
                    QByteArray buffer = socket.readAll();
                    qDebug() << i << "Discarded in the process:" << prettyByteArray(buffer);
                }

                if (socket.bytesAvailable() != 0)
                    QFAIL(QString("Unexpected bytes still on buffer when disconnecting in step %1:\n%2")
                          .arg(i).arg(prettyByteArray(socket.readAll())).toLocal8Bit());
                break;

            case Chat::Reconnect:
                qDebug() << i << "Reconnecting to server on port" << port;
                socket.connectToHost(QtNetworkSettings::serverName(), port);
                QVERIFY2(socket.waitForConnected(10000),
                         QString("Failed to reconnect to server in step %1: %2").arg(i).arg(socket.errorString()).toLocal8Bit());
                break;

            case Chat::StartEncryption:
#ifdef QT_NO_OPENSSL
                QFAIL("Internal error: SSL required for this test");
#else
                qDebug() << i << "Starting client encryption";
                socket.ignoreSslErrors();
                socket.startClientEncryption();
                QVERIFY2(socket.waitForEncrypted(5000),
                         QString("Failed to start client encryption in step %1: %2").arg(i)
                         .arg(socket.errorString()).toLocal8Bit());
                break;
#endif
            }
    }
}

tst_NetworkSelfTest::tst_NetworkSelfTest()
{
    Q_SET_DEFAULT_IAP
}

tst_NetworkSelfTest::~tst_NetworkSelfTest()
{
}

QHostAddress tst_NetworkSelfTest::serverIpAddress()
{
    if (cachedIpAddress.protocol() == QAbstractSocket::UnknownNetworkLayerProtocol) {
        // need resolving
        QHostInfo resolved = QHostInfo::fromName(QtNetworkSettings::serverName());
        if(resolved.error() != QHostInfo::NoError ||
            !resolved.addresses().isEmpty()) {
            qWarning("QHostInfo::fromName failed (%d).", resolved.error());
            return QHostAddress(QHostAddress::Null);
        }
        cachedIpAddress = resolved.addresses().first();
    }
    return cachedIpAddress;
}

void tst_NetworkSelfTest::hostTest()
{
    // this is a localhost self-test
    QHostInfo localhost = QHostInfo::fromName("localhost");
    QCOMPARE(localhost.error(), QHostInfo::NoError);
    QVERIFY(!localhost.addresses().isEmpty());

    QTcpServer server;
    QVERIFY(server.listen());

    QTcpSocket socket;
    socket.connectToHost("127.0.0.1", server.serverPort());
    QVERIFY(socket.waitForConnected(10000));
}

void tst_NetworkSelfTest::dnsResolution_data()
{
    QTest::addColumn<QString>("hostName");
    QTest::newRow("local-name") << QtNetworkSettings::serverLocalName();
    QTest::newRow("fqdn") << QtNetworkSettings::serverName();
}

void tst_NetworkSelfTest::dnsResolution()
{
    QFETCH(QString, hostName);
    QHostInfo resolved = QHostInfo::fromName(hostName);
    QVERIFY2(resolved.error() == QHostInfo::NoError,
             QString("Failed to resolve hostname %1: %2").arg(hostName, resolved.errorString()).toLocal8Bit());
    QVERIFY2(resolved.addresses().size() > 0, "Got 0 addresses for server IP");

    cachedIpAddress = resolved.addresses().first();
}

void tst_NetworkSelfTest::serverReachability()
{
    // check that we get a proper error connecting to port 12346
    QTcpSocket socket;
    socket.connectToHost(QtNetworkSettings::serverName(), 12346);

    QTime timer;
    timer.start();
    socket.waitForConnected(10000);
    QVERIFY2(timer.elapsed() < 9900, "Connection to closed port timed out instead of refusing, something is wrong");

    QVERIFY2(socket.state() == QAbstractSocket::UnconnectedState, "Socket connected unexpectedly!");
    QVERIFY2(socket.error() == QAbstractSocket::ConnectionRefusedError,
             QString("Could not reach server: %1").arg(socket.errorString()).toLocal8Bit());
}

void tst_NetworkSelfTest::remotePortsOpen_data()
{
    QTest::addColumn<int>("portNumber");
    QTest::newRow("ftp") << 21;
    QTest::newRow("ssh") << 22;
    QTest::newRow("imap") << 143;
    QTest::newRow("http") << 80;
    QTest::newRow("https") << 443;
    QTest::newRow("http-proxy") << 3128;
    QTest::newRow("http-proxy-auth-basic") << 3129;
    QTest::newRow("http-proxy-auth-ntlm") << 3130;
    QTest::newRow("socks5-proxy") << 1080;
    QTest::newRow("socks5-proxy-auth") << 1081;
}

void tst_NetworkSelfTest::remotePortsOpen()
{
#ifdef Q_OS_SYMBIAN
    if (qstrcmp(QTest::currentDataTag(), "http-proxy-auth-ntlm") == 0)
        QSKIP("NTML authentication not yet supported in Symbian", SkipSingle);
#endif

    QFETCH(int, portNumber);
    QTcpSocket socket;
    socket.connectToHost(QtNetworkSettings::serverName(), portNumber);

    if (!socket.waitForConnected(10000)) {
        if (socket.error() == QAbstractSocket::SocketTimeoutError)
            QFAIL(QString("Network timeout connecting to the server on port %1").arg(portNumber).toLocal8Bit());
        else
            QFAIL(QString("Error connecting to server on port %1: %2").arg(portNumber).arg(socket.errorString()).toLocal8Bit());
    }
    QVERIFY(socket.state() == QAbstractSocket::ConnectedState);
}


void tst_NetworkSelfTest::fileLineEndingTest()
{
    QString referenceName = SRCDIR "/rfc3252.txt";
    long long expectedReferenceSize = 25962;

    QString lineEndingType("LF");

    QFile reference(referenceName);
    QVERIFY(reference.open(QIODevice::ReadOnly));
    QByteArray byteLine = reference.readLine();
    if(byteLine.endsWith("\r\n"))
        lineEndingType = "CRLF";
    else if(byteLine.endsWith("\r"))
        lineEndingType = "CR";

    QString referenceAsTextData;
    QFile referenceAsText(referenceName);
    QVERIFY(referenceAsText.open(QIODevice::ReadOnly));
    referenceAsTextData = referenceAsText.readAll();

    QVERIFY2(expectedReferenceSize == referenceAsTextData.length(), QString("Reference file %1 has %2 as line ending and file size not matching - Git checkout issue !?!").arg(referenceName, lineEndingType).toLocal8Bit());
    QVERIFY2(!lineEndingType.compare("LF"), QString("Reference file %1 has %2 as line ending - Git checkout issue !?!").arg(referenceName, lineEndingType).toLocal8Bit());
}

static QList<Chat> ftpChat()
{
    return QList<Chat>() << Chat::expect("220")
            << Chat::discardUntil("\r\n")
            << Chat::send("USER anonymous\r\n")
            << Chat::expect("331")
            << Chat::discardUntil("\r\n")
            << Chat::send("PASS user@hostname\r\n")
            << Chat::expect("230")
            << Chat::discardUntil("\r\n")
            << Chat::send("QUIT\r\n")
            << Chat::expect("221")
            << Chat::discardUntil("\r\n")
            << Chat::RemoteDisconnect;
}

void tst_NetworkSelfTest::ftpServer()
{
    netChat(21, ftpChat());
}

void tst_NetworkSelfTest::imapServer()
{
    netChat(143, QList<Chat>()
            << Chat::expect("* OK ")
            << Chat::discardUntil("\r\n")
            << Chat::send("1 CAPABILITY\r\n")
            << Chat::expect("* CAPABILITY ")
            << Chat::discardUntil("1 OK")
            << Chat::discardUntil("\r\n")
            << Chat::send("2 LOGOUT\r\n")
            << Chat::discardUntil("2 OK")
            << Chat::discardUntil("\r\n")
            << Chat::RemoteDisconnect);
}

void tst_NetworkSelfTest::httpServer()
{
    netChat(80, QList<Chat>()
            // HTTP/0.9 chat:
            << Chat::send("GET /\r\n")
            << Chat::DiscardUntilDisconnect

            // HTTP/1.0 chat:
            << Chat::Reconnect
            << Chat::send("GET / HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Connection: close\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect

            // HTTP/1.0 POST:
            << Chat::Reconnect
            << Chat::send("POST / HTTP/1.0\r\n"
                          "Content-Length: 5\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "Hello")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect
            );
}

void tst_NetworkSelfTest::httpsServer()
{
#ifndef QT_NO_OPENSSL
    netChat(443, QList<Chat>()
            << Chat::StartEncryption
            << Chat::send("GET / HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Connection: close\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect);
#else
    QSKIP("SSL not enabled, cannot test", SkipAll);
#endif
}

void tst_NetworkSelfTest::httpProxy()
{
    netChat(3128, QList<Chat>()
            // proxy GET by IP
            << Chat::send("GET http://" + serverIpAddress().toString().toLatin1() + "/ HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Proxy-connection: close\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect

            // proxy GET by hostname
            << Chat::Reconnect
            << Chat::send("GET http://" + QtNetworkSettings::serverName().toLatin1() + "/ HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Proxy-connection: close\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect

            // proxy CONNECT by IP
            << Chat::Reconnect
            << Chat::send("CONNECT " + serverIpAddress().toString().toLatin1() + ":21 HTTP/1.0\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::discardUntil("\r\n\r\n")
            << ftpChat()

            // proxy CONNECT by hostname
            << Chat::Reconnect
            << Chat::send("CONNECT " + QtNetworkSettings::serverName().toLatin1() + ":21 HTTP/1.0\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::discardUntil("\r\n\r\n")
            << ftpChat()
            );
}

void tst_NetworkSelfTest::httpProxyBasicAuth()
{
    netChat(3129, QList<Chat>()
            // test auth required response
            << Chat::send("GET http://" + QtNetworkSettings::serverName().toLatin1() + "/ HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Proxy-connection: close\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("407 ")
            << Chat::discardUntil("\r\nProxy-Authenticate: Basic realm=\"")
            << Chat::DiscardUntilDisconnect

            // now try sending our credentials
            << Chat::Reconnect
            << Chat::send("GET http://" + QtNetworkSettings::serverName().toLatin1() + "/ HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Proxy-connection: close\r\n"
                          "Proxy-Authorization: Basic cXNvY2tzdGVzdDpwYXNzd29yZA==\r\n"
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("200 ")
            << Chat::DiscardUntilDisconnect);
}

void tst_NetworkSelfTest::httpProxyNtlmAuth()
{
#ifdef Q_OS_SYMBIAN
    QSKIP("NTML authentication not yet supported in Symbian", SkipAll);
#else
    netChat(3130, QList<Chat>()
            // test auth required response
            << Chat::send("GET http://" + QtNetworkSettings::serverName().toLatin1() + "/ HTTP/1.0\r\n"
                          "Host: " + QtNetworkSettings::serverName().toLatin1() + "\r\n"
                          "Proxy-connection: keep-alive\r\n" // NTLM auth will disconnect
                          "\r\n")
            << Chat::expect("HTTP/1.")
            << Chat::discardUntil(" ")
            << Chat::expect("407 ")
            << Chat::discardUntil("\r\nProxy-Authenticate: NTLM\r\n")
            << Chat::DiscardUntilDisconnect
            );
#endif
}

// SOCKSv5 is a binary protocol
static const char handshakeNoAuth[] = "\5\1\0";
static const char handshakeOkNoAuth[] = "\5\0";
static const char handshakeAuthPassword[] = "\5\1\2\1\12qsockstest\10password";
static const char handshakeOkPasswdAuth[] = "\5\2\1\0";
static const char handshakeAuthNotOk[] = "\5\377";
static const char connect1[] = "\5\1\0\1\177\0\0\1\0\25"; // Connect IPv4 127.0.0.1 port 21
static const char connect1a[] = "\5\1\0\1"; // just "Connect to IPv4"
static const char connect1b[] = "\0\25"; // just "port 21"
static const char connect2[] = "\5\1\0\3\11localhost\0\25"; // Connect hostname localhost 21
static const char connect2a[] = "\5\1\0\3"; // just "Connect to hostname"
static const char connected[] = "\5\0\0";

#define QBA(x) (QByteArray::fromRawData(x, -1 + sizeof(x)))

void tst_NetworkSelfTest::socks5Proxy()
{
    union {
        char buf[4];
        quint32 data;
    } ip4Address;
    ip4Address.data = qToBigEndian(serverIpAddress().toIPv4Address());

    netChat(1080, QList<Chat>()
            // IP address connection
            << Chat::send(QByteArray(handshakeNoAuth, -1 + sizeof handshakeNoAuth))
            << Chat::expect(QByteArray(handshakeOkNoAuth, -1 + sizeof handshakeOkNoAuth))
            << Chat::send(QByteArray(connect1, -1 + sizeof connect1))
            << Chat::expect(QByteArray(connected, -1 + sizeof connected))
            << Chat::expect("\1") // IPv4 address following
            << Chat::skipBytes(6) // the server's local address and port
            << ftpChat()

            // connect by IP
            << Chat::Reconnect
            << Chat::send(QByteArray(handshakeNoAuth, -1 + sizeof handshakeNoAuth))
            << Chat::expect(QByteArray(handshakeOkNoAuth, -1 + sizeof handshakeOkNoAuth))
            << Chat::send(QBA(connect1a) + QByteArray::fromRawData(ip4Address.buf, 4) + QBA(connect1b))
            << Chat::expect(QByteArray(connected, -1 + sizeof connected))
            << Chat::expect("\1") // IPv4 address following
            << Chat::skipBytes(6) // the server's local address and port
            << ftpChat()

            // connect to "localhost" by hostname
            << Chat::Reconnect
            << Chat::send(QByteArray(handshakeNoAuth, -1 + sizeof handshakeNoAuth))
            << Chat::expect(QByteArray(handshakeOkNoAuth, -1 + sizeof handshakeOkNoAuth))
            << Chat::send(QByteArray(connect2, -1 + sizeof connect2))
            << Chat::expect(QByteArray(connected, -1 + sizeof connected))
            << Chat::expect("\1") // IPv4 address following
            << Chat::skipBytes(6) // the server's local address and port
            << ftpChat()

            // connect to server by its official name
            << Chat::Reconnect
            << Chat::send(QByteArray(handshakeNoAuth, -1 + sizeof handshakeNoAuth))
            << Chat::expect(QByteArray(handshakeOkNoAuth, -1 + sizeof handshakeOkNoAuth))
            << Chat::send(QBA(connect2a) + char(QtNetworkSettings::serverName().size()) + QtNetworkSettings::serverName().toLatin1() + QBA(connect1b))
            << Chat::expect(QByteArray(connected, -1 + sizeof connected))
            << Chat::expect("\1") // IPv4 address following
            << Chat::skipBytes(6) // the server's local address and port
            << ftpChat()
            );
}

void tst_NetworkSelfTest::socks5ProxyAuth()
{
    netChat(1081, QList<Chat>()
            // unauthenticated connect -- will get error
            << Chat::send(QByteArray(handshakeNoAuth, -1 + sizeof handshakeNoAuth))
            << Chat::expect(QByteArray(handshakeAuthNotOk, -1 + sizeof handshakeAuthNotOk))
            << Chat::RemoteDisconnect

            // now try to connect with authentication
            << Chat::Reconnect
            << Chat::send(QByteArray(handshakeAuthPassword, -1 + sizeof handshakeAuthPassword))
            << Chat::expect(QByteArray(handshakeOkPasswdAuth, -1 + sizeof handshakeOkPasswdAuth))
            << Chat::send(QByteArray(connect1, -1 + sizeof connect1))
            << Chat::expect(QByteArray(connected, -1 + sizeof connected))
            << Chat::expect("\1") // IPv4 address following
            << Chat::skipBytes(6) // the server's local address and port
            << ftpChat()
            );
}

void tst_NetworkSelfTest::supportsSsl()
{
#ifdef QT_NO_OPENSSL
    QFAIL("SSL not compiled in");
#else
    QVERIFY(QSslSocket::supportsSsl());
#endif
}

QTEST_MAIN(tst_NetworkSelfTest)
#include "tst_networkselftest.moc"
