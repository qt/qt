/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtCore/qthread.h>
#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/qnetworkproxy.h>
#include <QtNetwork/qsslcipher.h>
#include <QtNetwork/qsslconfiguration.h>
#include <QtNetwork/qsslkey.h>
#include <QtNetwork/qsslsocket.h>
#include <QtNetwork/qtcpserver.h>
#include <QtTest/QtTest>

#ifndef TEST_QNETWORK_PROXY
#define TEST_QNETWORK_PROXY
#endif
#ifdef TEST_QNETWORK_PROXY
#include <QNetworkProxy>
#include <QAuthenticator>
#endif

#include "../network-settings.h"

Q_DECLARE_METATYPE(QAbstractSocket::SocketState)
Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
#ifndef QT_NO_OPENSSL
Q_DECLARE_METATYPE(QSslSocket::SslMode)
typedef QList<QSslError::SslError> SslErrorList;
Q_DECLARE_METATYPE(SslErrorList)
Q_DECLARE_METATYPE(QSslError)
#endif

#if defined Q_OS_HPUX && defined Q_CC_GNU
// This error is delivered every time we try to use the fluke CA
// certificate. For now we work around this bug. Task 202317.
#define QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
#endif

#ifndef QT_NO_OPENSSL
class QSslSocketPtr: public QSharedPointer<QSslSocket>
{
public:
    inline QSslSocketPtr(QSslSocket *ptr = 0)
        : QSharedPointer<QSslSocket>(ptr)
    { }

    inline operator QSslSocket *() const { return data(); }
};
#endif

class tst_QSslSocket : public QObject
{
    Q_OBJECT

    int proxyAuthCalled;

public:
    tst_QSslSocket();
    virtual ~tst_QSslSocket();

    static void enterLoop(int secs)
    {
        ++loopLevel;
        QTestEventLoop::instance().enterLoop(secs);
    }

    static bool timeout()
    {
        return QTestEventLoop::instance().timeout();
    }

#ifndef QT_NO_OPENSSL
    QSslSocketPtr newSocket();
#endif

public slots:
    void initTestCase_data();
    void init();
    void cleanup();
    void proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth);

#ifndef QT_NO_OPENSSL
private slots:
    void constructing();
    void simpleConnect();
    void simpleConnectWithIgnore();

    // API tests
    void sslErrors_data();
    void sslErrors();
    void addCaCertificate();
    void addCaCertificates();
    void addCaCertificates2();
    void ciphers();
    void connectToHostEncrypted();
    void sessionCipher();
    void flush();
    void isEncrypted();
    void localCertificate();
    void mode();
    void peerCertificate();
    void peerCertificateChain();
    void privateKey();
    void protocol();
    void setCaCertificates();
    void setLocalCertificate();
    void setPrivateKey();
    void setProtocol();
    void setSocketDescriptor();
    void waitForEncrypted();
    void waitForConnectedEncryptedReadyRead();
    void startClientEncryption();
    void startServerEncryption();
    void addDefaultCaCertificate();
    void addDefaultCaCertificates();
    void addDefaultCaCertificates2();
    void defaultCaCertificates();
    void defaultCiphers();
    void resetDefaultCiphers();
    void setDefaultCaCertificates();
    void setDefaultCiphers();
    void supportedCiphers();
    void systemCaCertificates();
    void wildcard();
    void setEmptyKey();
    void spontaneousWrite();
    void setReadBufferSize();
    void waitForMinusOne();
    void verifyMode();
    void verifyDepth();
    void peerVerifyError();
    void disconnectFromHostWhenConnecting();
    void disconnectFromHostWhenConnected();
    void resetProxy();

    static void exitLoop()
    {
        // Safe exit - if we aren't in an event loop, don't
        // exit one.
        if (loopLevel > 0) {
            --loopLevel;
            QTestEventLoop::instance().exitLoop();
        }
    }

protected slots:
    void ignoreErrorSlot()
    {
        socket->ignoreSslErrors();
    }
    void untrustedWorkaroundSlot(const QList<QSslError> &errors)
    {
        if (errors.size() == 1 && errors.first().error() == QSslError::CertificateUntrusted)
            socket->ignoreSslErrors();
    }

private:
    QSslSocket *socket;
#endif // QT_NO_OPENSSL
private:
    static int loopLevel;
};

int tst_QSslSocket::loopLevel = 0;

tst_QSslSocket::tst_QSslSocket()
{
#ifndef QT_NO_OPENSSL
    qRegisterMetaType<QList<QSslError> >("QList<QSslError>");
    qRegisterMetaType<QSslError>("QSslError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QSslSocket::SslMode");
#endif
}

tst_QSslSocket::~tst_QSslSocket()
{

}

enum ProxyTests {
    NoProxy = 0x00,
    Socks5Proxy = 0x01,
    HttpProxy = 0x02,
    TypeMask = 0x0f,

    NoAuth = 0x00,
    AuthBasic = 0x10,
    AuthNtlm = 0x20,
    AuthMask = 0xf0
};

void tst_QSslSocket::initTestCase_data()
{
    QTest::addColumn<bool>("setProxy");
    QTest::addColumn<int>("proxyType");

    QTest::newRow("WithoutProxy") << false << 0;
#ifdef TEST_QNETWORK_PROXY
    QTest::newRow("WithSocks5Proxy") << true << int(Socks5Proxy);
    QTest::newRow("WithSocks5ProxyAuth") << true << int(Socks5Proxy | AuthBasic);

    QTest::newRow("WithHttpProxy") << true << int(HttpProxy);
    QTest::newRow("WithHttpProxyBasicAuth") << true << int(HttpProxy | AuthBasic);
    // uncomment the line below when NTLM works
//    QTest::newRow("WithHttpProxyNtlmAuth") << true << int(HttpProxy | AuthNtlm);
#endif
}

void tst_QSslSocket::init()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
#ifdef TEST_QNETWORK_PROXY
        QFETCH_GLOBAL(int, proxyType);
        QString fluke = QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first().toString();
        QNetworkProxy proxy;

        switch (proxyType) {
        case Socks5Proxy:
            proxy = QNetworkProxy(QNetworkProxy::Socks5Proxy, fluke, 1080);
            break;

        case Socks5Proxy | AuthBasic:
            proxy = QNetworkProxy(QNetworkProxy::Socks5Proxy, fluke, 1081);
            break;

        case HttpProxy | NoAuth:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, fluke, 3128);
            break;

        case HttpProxy | AuthBasic:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, fluke, 3129);
            break;

        case HttpProxy | AuthNtlm:
            proxy = QNetworkProxy(QNetworkProxy::HttpProxy, fluke, 3130);
            break;
        }
        QNetworkProxy::setApplicationProxy(proxy);
#endif
    }
}

void tst_QSslSocket::cleanup()
{
#ifdef TEST_QNETWORK_PROXY
    QNetworkProxy::setApplicationProxy(QNetworkProxy::DefaultProxy);
#endif
}

#ifndef QT_NO_OPENSSL
QSslSocketPtr tst_QSslSocket::newSocket()
{
    QSslSocket *socket = new QSslSocket;

    proxyAuthCalled = 0;
    connect(socket, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
            Qt::DirectConnection);

    return QSslSocketPtr(socket);
}
#endif

void tst_QSslSocket::proxyAuthenticationRequired(const QNetworkProxy &, QAuthenticator *auth)
{
    ++proxyAuthCalled;
    auth->setUser("qsockstest");
    auth->setPassword("password");
}

#ifndef QT_NO_OPENSSL

void tst_QSslSocket::constructing()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocket socket;

    QCOMPARE(socket.state(), QSslSocket::UnconnectedState);
    QCOMPARE(socket.mode(), QSslSocket::UnencryptedMode);
    QVERIFY(!socket.isEncrypted());
    QCOMPARE(socket.bytesAvailable(), qint64(0));
    QCOMPARE(socket.bytesToWrite(), qint64(0));
    QVERIFY(!socket.canReadLine());
    QVERIFY(socket.atEnd());
    QCOMPARE(socket.localCertificate(), QSslCertificate());
    QCOMPARE(socket.sslConfiguration(), QSslConfiguration::defaultConfiguration());
    QCOMPARE(socket.errorString(), QString("Unknown error"));
    char c = '\0';
    QTest::ignoreMessage(QtWarningMsg, "QIODevice::getChar: Closed device");
    QVERIFY(!socket.getChar(&c));
    QCOMPARE(c, '\0');
    QVERIFY(!socket.isOpen());
    QVERIFY(!socket.isReadable());
    QVERIFY(socket.isSequential());
    QVERIFY(!socket.isTextModeEnabled());
    QVERIFY(!socket.isWritable());
    QCOMPARE(socket.openMode(), QIODevice::NotOpen);
    QVERIFY(socket.peek(2).isEmpty());
    QCOMPARE(socket.pos(), qint64(0));
    QVERIFY(!socket.putChar('c'));
    QVERIFY(socket.read(2).isEmpty());
    QCOMPARE(socket.read(0, 0), qint64(-1));
    QVERIFY(socket.readAll().isEmpty());
    QTest::ignoreMessage(QtWarningMsg, "QIODevice::readLine: Called with maxSize < 2");
    QCOMPARE(socket.readLine(0, 0), qint64(-1));
    char buf[10];
    QCOMPARE(socket.readLine(buf, sizeof(buf)), qint64(-1));
    QTest::ignoreMessage(QtWarningMsg, "QIODevice::seek: The device is not open");
    QVERIFY(!socket.reset());
    QTest::ignoreMessage(QtWarningMsg, "QIODevice::seek: The device is not open");
    QVERIFY(!socket.seek(2));
    QCOMPARE(socket.size(), qint64(0));
    QVERIFY(!socket.waitForBytesWritten(10));
    QVERIFY(!socket.waitForReadyRead(10));
    QCOMPARE(socket.write(0, 0), qint64(-1));
    QCOMPARE(socket.write(QByteArray()), qint64(-1));
    QCOMPARE(socket.error(), QAbstractSocket::UnknownSocketError);
    QVERIFY(!socket.flush());
    QVERIFY(!socket.isValid());
    QCOMPARE(socket.localAddress(), QHostAddress());
    QCOMPARE(socket.localPort(), quint16(0));
    QCOMPARE(socket.peerAddress(), QHostAddress());
    QVERIFY(socket.peerName().isEmpty());
    QCOMPARE(socket.peerPort(), quint16(0));
    QCOMPARE(socket.proxy().type(), QNetworkProxy::DefaultProxy);
    QCOMPARE(socket.readBufferSize(), qint64(0));
    QCOMPARE(socket.socketDescriptor(), -1);
    QCOMPARE(socket.socketType(), QAbstractSocket::TcpSocket);
    QVERIFY(!socket.waitForConnected(10));
    QTest::ignoreMessage(QtWarningMsg, "QSslSocket::waitForDisconnected() is not allowed in UnconnectedState");
    QVERIFY(!socket.waitForDisconnected(10));
    QCOMPARE(socket.protocol(), QSsl::SslV3);

    QSslConfiguration savedDefault = QSslConfiguration::defaultConfiguration();

    // verify that changing the default config doesn't affect this socket
    QSslSocket::setDefaultCaCertificates(QList<QSslCertificate>());
    QSslSocket::setDefaultCiphers(QList<QSslCipher>());
    QVERIFY(!socket.caCertificates().isEmpty());
    QVERIFY(!socket.ciphers().isEmpty());

    // verify the default as well:
    QVERIFY(QSslConfiguration::defaultConfiguration().caCertificates().isEmpty());
    QVERIFY(QSslConfiguration::defaultConfiguration().ciphers().isEmpty());

    QSslConfiguration::setDefaultConfiguration(savedDefault);
}

void tst_QSslSocket::simpleConnect()
{
    if (!QSslSocket::supportsSsl())
        return;

    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    QSslSocket socket;
    QSignalSpy connectedSpy(&socket, SIGNAL(connected()));
    QSignalSpy hostFoundSpy(&socket, SIGNAL(hostFound()));
    QSignalSpy disconnectedSpy(&socket, SIGNAL(disconnected()));
    QSignalSpy connectionEncryptedSpy(&socket, SIGNAL(encrypted()));
    QSignalSpy sslErrorsSpy(&socket, SIGNAL(sslErrors(const QList<QSslError> &)));

    connect(&socket, SIGNAL(connected()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(modeChanged(QSslSocket::SslMode)), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(encrypted()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(exitLoop()));

    // Start connecting
    socket.connectToHost(QtNetworkSettings::serverName(), 993);
    QCOMPARE(socket.state(), QAbstractSocket::HostLookupState);
    enterLoop(10);

    // Entered connecting state
    QCOMPARE(socket.state(), QAbstractSocket::ConnectingState);
    QCOMPARE(connectedSpy.count(), 0);
    QCOMPARE(hostFoundSpy.count(), 1);
    QCOMPARE(disconnectedSpy.count(), 0);
    enterLoop(10);

    // Entered connected state
    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(socket.mode(), QSslSocket::UnencryptedMode);
    QVERIFY(!socket.isEncrypted());
    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(hostFoundSpy.count(), 1);
    QCOMPARE(disconnectedSpy.count(), 0);

    // Enter encrypted mode
    socket.startClientEncryption();
    QCOMPARE(socket.mode(), QSslSocket::SslClientMode);
    QVERIFY(!socket.isEncrypted());
    QCOMPARE(connectionEncryptedSpy.count(), 0);
    QCOMPARE(sslErrorsSpy.count(), 0);

    // Starting handshake
    enterLoop(10);
    QCOMPARE(sslErrorsSpy.count(), 1);
    QCOMPARE(connectionEncryptedSpy.count(), 0);
    QVERIFY(!socket.isEncrypted());
    QCOMPARE(socket.state(), QAbstractSocket::UnconnectedState);
}

void tst_QSslSocket::simpleConnectWithIgnore()
{
    if (!QSslSocket::supportsSsl())
        return;

    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    QSslSocket socket;
    this->socket = &socket;
    QSignalSpy encryptedSpy(&socket, SIGNAL(encrypted()));
    QSignalSpy sslErrorsSpy(&socket, SIGNAL(sslErrors(const QList<QSslError> &)));

    connect(&socket, SIGNAL(readyRead()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(encrypted()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(connected()), this, SLOT(exitLoop()));
    connect(&socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));
    connect(&socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(exitLoop()));

    // Start connecting
    socket.connectToHost(QtNetworkSettings::serverName(), 993);
    QCOMPARE(socket.state(), QAbstractSocket::HostLookupState);
    enterLoop(10);

    // Start handshake
    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    socket.startClientEncryption();
    enterLoop(10);

    // Done; encryption should be enabled.
    QCOMPARE(sslErrorsSpy.count(), 1);
    QVERIFY(socket.isEncrypted());
    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(encryptedSpy.count(), 1);

    // Wait for incoming data
    if (!socket.canReadLine())
        enterLoop(10);

    QCOMPARE(socket.readAll(), QByteArray("* OK [CAPABILITY IMAP4 IMAP4rev1 LITERAL+ ID AUTH=PLAIN SASL-IR] qt-test-server.qt-test-net Cyrus IMAP4 v2.3.11-Mandriva-RPM-2.3.11-6mdv2008.1 server ready\r\n"));

    socket.disconnectFromHost();
}

void tst_QSslSocket::sslErrors_data()
{
    QTest::addColumn<QString>("host");
    QTest::addColumn<int>("port");
    QTest::addColumn<SslErrorList>("expected");

    QTest::newRow(qPrintable(QtNetworkSettings::serverLocalName()))
        << QtNetworkSettings::serverLocalName()
        << 993
        << (SslErrorList() << QSslError::HostNameMismatch
                           << QSslError::SelfSignedCertificate);

    QTest::newRow("imap.trolltech.com")
        << "imap.trolltech.com"
        << 993
        << (SslErrorList() << QSslError::SelfSignedCertificateInChain);
}

void tst_QSslSocket::sslErrors()
{
    QFETCH(QString, host);
    QFETCH(int, port);
    QFETCH(SslErrorList, expected);

    QSslSocketPtr socket = newSocket();
    socket->connectToHostEncrypted(host, port);
    socket->waitForEncrypted(5000);

    SslErrorList output;
    foreach (QSslError error, socket->sslErrors())
        output << error.error();

#ifdef QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    if (output.last() == QSslError::CertificateUntrusted)
        output.takeLast();
#endif
    QCOMPARE(output, expected);
}

void tst_QSslSocket::addCaCertificate()
{
    if (!QSslSocket::supportsSsl())
        return;
}

void tst_QSslSocket::addCaCertificates()
{
    if (!QSslSocket::supportsSsl())
        return;
}

void tst_QSslSocket::addCaCertificates2()
{
    if (!QSslSocket::supportsSsl())
        return;
}

void tst_QSslSocket::ciphers()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocket socket;
    QCOMPARE(socket.ciphers(), QSslSocket::supportedCiphers());
    socket.setCiphers(QList<QSslCipher>());
    QVERIFY(socket.ciphers().isEmpty());
    socket.setCiphers(socket.defaultCiphers());
    QCOMPARE(socket.ciphers(), QSslSocket::supportedCiphers());
    socket.setCiphers(socket.defaultCiphers());
    QCOMPARE(socket.ciphers(), QSslSocket::supportedCiphers());

    // Task 164356
    socket.setCiphers("ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
}

void tst_QSslSocket::connectToHostEncrypted()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;

    QVERIFY(socket->addCaCertificates(QLatin1String(SRCDIR "certs/qt-test-server-cacert.pem")));
#ifdef QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    connect(&socket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(untrustedWorkaroundSlot(QList<QSslError>)));
#endif

    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);

    // This should pass unconditionally when using fluke's CA certificate.
    QVERIFY2(socket->waitForEncrypted(10000), qPrintable(socket->errorString()));

    socket->disconnectFromHost();
    QVERIFY(socket->waitForDisconnected());

    QCOMPARE(socket->mode(), QSslSocket::SslClientMode);

    socket->connectToHost(QtNetworkSettings::serverName(), 13);

    QCOMPARE(socket->mode(), QSslSocket::UnencryptedMode);

    QVERIFY(socket->waitForDisconnected());
}

void tst_QSslSocket::sessionCipher()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(ignoreErrorSlot()));
    QVERIFY(socket->sessionCipher().isNull());
    socket->connectToHost(QtNetworkSettings::serverName(), 443 /* https */);
    QVERIFY(socket->waitForConnected(5000));
    QVERIFY(socket->sessionCipher().isNull());
    socket->startClientEncryption();
    QVERIFY(socket->waitForEncrypted(5000));
    QVERIFY(!socket->sessionCipher().isNull());
    QVERIFY(QSslSocket::supportedCiphers().contains(socket->sessionCipher()));
    socket->disconnectFromHost();
    QVERIFY(socket->waitForDisconnected());
}

void tst_QSslSocket::flush()
{
}

void tst_QSslSocket::isEncrypted()
{
}

void tst_QSslSocket::localCertificate()
{
}

void tst_QSslSocket::mode()
{
}

void tst_QSslSocket::peerCertificate()
{
}

void tst_QSslSocket::peerCertificateChain()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;

    QList<QSslCertificate> caCertificates = QSslCertificate::fromPath(QLatin1String(SRCDIR "certs/qt-test-server-cacert.pem"));
    QVERIFY(caCertificates.count() == 1);

    socket->addCaCertificates(caCertificates);
#ifdef QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    connect(&socket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(untrustedWorkaroundSlot(QList<QSslError>)));
#endif

    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QCOMPARE(socket->mode(), QSslSocket::UnencryptedMode);
    QVERIFY(socket->peerCertificateChain().isEmpty());
    QVERIFY2(socket->waitForEncrypted(10000), qPrintable(socket->errorString()));

    QList<QSslCertificate> certChain = socket->peerCertificateChain();
    QVERIFY(certChain.count() > 0);
    QCOMPARE(certChain.first(), socket->peerCertificate());

    socket->disconnectFromHost();
    QVERIFY(socket->waitForDisconnected());

    // connect again to a different server
    socket->connectToHostEncrypted("trolltech.com", 443);
    socket->ignoreSslErrors();
    QCOMPARE(socket->mode(), QSslSocket::UnencryptedMode);
    QVERIFY(socket->peerCertificateChain().isEmpty());
    QVERIFY2(socket->waitForEncrypted(10000), qPrintable(socket->errorString()));

    QCOMPARE(socket->peerCertificateChain().first(), socket->peerCertificate());
    QVERIFY(socket->peerCertificateChain() != certChain);

    socket->disconnectFromHost();
    QVERIFY(socket->waitForDisconnected());

    // now do it again back to the original server
    socket->connectToHost(QtNetworkSettings::serverName(), 443);
    QCOMPARE(socket->mode(), QSslSocket::UnencryptedMode);
    QVERIFY(socket->peerCertificateChain().isEmpty());
    QVERIFY2(socket->waitForConnected(10000), "Network timeout");

    socket->startClientEncryption();
    QVERIFY2(socket->waitForEncrypted(10000), qPrintable(socket->errorString()));

    QCOMPARE(socket->peerCertificateChain().first(), socket->peerCertificate());
    QVERIFY(socket->peerCertificateChain() == certChain);

    socket->disconnectFromHost();
    QVERIFY(socket->waitForDisconnected());
}

void tst_QSslSocket::privateKey()
{
}

void tst_QSslSocket::protocol()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;
    QList<QSslCertificate> certs = QSslCertificate::fromPath(SRCDIR "certs/qt-test-server-cacert.pem");

//    qDebug() << "certs:" << certs.at(0).issuerInfo(QSslCertificate::CommonName);
    socket->setCaCertificates(certs);
#ifdef QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    connect(&socket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(untrustedWorkaroundSlot(QList<QSslError>)));
#endif

//    qDebug() << "socket cert:" << socket->caCertificates().at(0).issuerInfo(QSslCertificate::CommonName);
    QCOMPARE(socket->protocol(), QSsl::SslV3);
    {
        // Fluke allows SSLv3.
        socket->setProtocol(QSsl::SslV3);
        QCOMPARE(socket->protocol(), QSsl::SslV3);
        socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        QCOMPARE(socket->protocol(), QSsl::SslV3);
        socket->abort();
        QCOMPARE(socket->protocol(), QSsl::SslV3);
        socket->connectToHost(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForConnected(), qPrintable(socket->errorString()));
        socket->startClientEncryption();
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        QCOMPARE(socket->protocol(), QSsl::SslV3);
        socket->abort();
    }
    {
        // Fluke allows TLSV1.
        socket->setProtocol(QSsl::TlsV1);
        QCOMPARE(socket->protocol(), QSsl::TlsV1);
        socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        QCOMPARE(socket->protocol(), QSsl::TlsV1);
        socket->abort();
        QCOMPARE(socket->protocol(), QSsl::TlsV1);
        socket->connectToHost(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForConnected(), qPrintable(socket->errorString()));
        socket->startClientEncryption();
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        QCOMPARE(socket->protocol(), QSsl::TlsV1);
        socket->abort();
    }
    {
        // Fluke allows SSLV2.
        socket->setProtocol(QSsl::SslV2);
        QCOMPARE(socket->protocol(), QSsl::SslV2);
        socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
        QVERIFY(socket->waitForEncrypted());
        QCOMPARE(socket->protocol(), QSsl::SslV2);
        socket->abort();
        QCOMPARE(socket->protocol(), QSsl::SslV2);
        socket->connectToHost(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForConnected(), qPrintable(socket->errorString()));
        socket->startClientEncryption();
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        socket->abort();
    }
    {
        // Fluke allows SSLV3, so it allows AnyProtocol.
        socket->setProtocol(QSsl::AnyProtocol);
        QCOMPARE(socket->protocol(), QSsl::AnyProtocol);
        socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
        QVERIFY(socket->waitForEncrypted());
        QCOMPARE(socket->protocol(), QSsl::AnyProtocol);
        socket->abort();
        QCOMPARE(socket->protocol(), QSsl::AnyProtocol);
        socket->connectToHost(QtNetworkSettings::serverName(), 443);
        QVERIFY2(socket->waitForConnected(), qPrintable(socket->errorString()));
        socket->startClientEncryption();
        QVERIFY2(socket->waitForEncrypted(), qPrintable(socket->errorString()));
        QCOMPARE(socket->protocol(), QSsl::AnyProtocol);
        socket->abort();
    }
}

void tst_QSslSocket::setCaCertificates()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocket socket;
    QCOMPARE(socket.caCertificates(), QSslSocket::defaultCaCertificates());
    socket.setCaCertificates(QSslCertificate::fromPath(SRCDIR "certs/qt-test-server-cacert.pem"));
    QCOMPARE(socket.caCertificates().size(), 1);
    socket.setCaCertificates(socket.defaultCaCertificates());
    QCOMPARE(socket.caCertificates(), QSslSocket::defaultCaCertificates());
}

void tst_QSslSocket::setLocalCertificate()
{
}

void tst_QSslSocket::setPrivateKey()
{
}

void tst_QSslSocket::setProtocol()
{
}

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    SslServer() : socket(0) { }
    QSslSocket *socket;

protected:
    void incomingConnection(int socketDescriptor)
    {
        socket = new QSslSocket(this);
        connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));

        QFile file(SRCDIR "certs/fluke.key");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QSslKey key(file.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
        QVERIFY(!key.isNull());
        socket->setPrivateKey(key);

        QList<QSslCertificate> localCert = QSslCertificate::fromPath(SRCDIR "certs/fluke.cert");
        QVERIFY(!localCert.isEmpty());
        QVERIFY(localCert.first().handle());
        socket->setLocalCertificate(localCert.first());

        QVERIFY(socket->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState));
        QVERIFY(!socket->peerAddress().isNull());
        QVERIFY(socket->peerPort() != 0);
        QVERIFY(!socket->localAddress().isNull());
        QVERIFY(socket->localPort() != 0);

        socket->startServerEncryption();
    }

protected slots:
    void ignoreErrorSlot()
    {
        socket->ignoreSslErrors();
    }
};

void tst_QSslSocket::setSocketDescriptor()
{
    if (!QSslSocket::supportsSsl())
        return;

    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    SslServer server;
    QVERIFY(server.listen());

    QEventLoop loop;
    QTimer::singleShot(5000, &loop, SLOT(quit()));

    QSslSocketPtr client = new QSslSocket;
    socket = client;
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));
    connect(client, SIGNAL(encrypted()), &loop, SLOT(quit()));

    client->connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

    loop.exec();

    QCOMPARE(client->state(), QAbstractSocket::ConnectedState);
    QVERIFY(client->isEncrypted());
    QVERIFY(!client->peerAddress().isNull());
    QVERIFY(client->peerPort() != 0);
    QVERIFY(!client->localAddress().isNull());
    QVERIFY(client->localPort() != 0);
}

void tst_QSslSocket::waitForEncrypted()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;

    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));
    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 443);

    QVERIFY(socket->waitForEncrypted(10000));
}

void tst_QSslSocket::waitForConnectedEncryptedReadyRead()
{
    if (!QSslSocket::supportsSsl())
        return;

    QSslSocketPtr socket = newSocket();
    this->socket = socket;

    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));
    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 993);

    QVERIFY(socket->waitForConnected(10000));
    QVERIFY(socket->waitForEncrypted(10000));
    QVERIFY(socket->waitForReadyRead(10000));
    QVERIFY(!socket->peerCertificate().isNull());
    QVERIFY(!socket->peerCertificateChain().isEmpty());
}

void tst_QSslSocket::startClientEncryption()
{
}

void tst_QSslSocket::startServerEncryption()
{
}

void tst_QSslSocket::addDefaultCaCertificate()
{
    if (!QSslSocket::supportsSsl())
        return;

    // Reset the global CA chain
    QSslSocket::setDefaultCaCertificates(QSslSocket::systemCaCertificates());

    QList<QSslCertificate> flukeCerts = QSslCertificate::fromPath(SRCDIR "certs/qt-test-server-cacert.pem");
    QCOMPARE(flukeCerts.size(), 1);
    QList<QSslCertificate> globalCerts = QSslSocket::defaultCaCertificates();
    QVERIFY(!globalCerts.contains(flukeCerts.first()));
    QSslSocket::addDefaultCaCertificate(flukeCerts.first());
    QCOMPARE(QSslSocket::defaultCaCertificates().size(), globalCerts.size() + 1);
    QVERIFY(QSslSocket::defaultCaCertificates().contains(flukeCerts.first()));

    // Restore the global CA chain
    QSslSocket::setDefaultCaCertificates(QSslSocket::systemCaCertificates());
}

void tst_QSslSocket::addDefaultCaCertificates()
{
}

void tst_QSslSocket::addDefaultCaCertificates2()
{
}

void tst_QSslSocket::defaultCaCertificates()
{
    if (!QSslSocket::supportsSsl())
        return;

    QList<QSslCertificate> certs = QSslSocket::defaultCaCertificates();
    QVERIFY(certs.size() > 1);
    QCOMPARE(certs, QSslSocket::systemCaCertificates());
}

void tst_QSslSocket::defaultCiphers()
{
}

void tst_QSslSocket::resetDefaultCiphers()
{
}

void tst_QSslSocket::setDefaultCaCertificates()
{
}

void tst_QSslSocket::setDefaultCiphers()
{
}

void tst_QSslSocket::supportedCiphers()
{
    if (!QSslSocket::supportsSsl())
        return;

    QList<QSslCipher> ciphers = QSslSocket::supportedCiphers();
    QVERIFY(ciphers.size() > 1);

    QSslSocket socket;
    QCOMPARE(socket.supportedCiphers(), ciphers);
    QCOMPARE(socket.defaultCiphers(), ciphers);
    QCOMPARE(socket.ciphers(), ciphers);
}

void tst_QSslSocket::systemCaCertificates()
{
    if (!QSslSocket::supportsSsl())
        return;

    QList<QSslCertificate> certs = QSslSocket::systemCaCertificates();
    QVERIFY(certs.size() > 1);
    QCOMPARE(certs, QSslSocket::defaultCaCertificates());
}

void tst_QSslSocket::wildcard()
{
	QSKIP("TODO: solve wildcard problem", SkipAll);

    if (!QSslSocket::supportsSsl())
        return;

    // Fluke runs an apache server listening on port 4443, serving the
    // wildcard fluke.*.troll.no.  The DNS entry for
    // fluke.wildcard.dev.troll.no, served by ares (root for dev.troll.no),
    // returns the CNAME fluke.troll.no for this domain. The web server
    // responds with the wildcard, and QSslSocket should accept that as a
    // valid connection.  This was broken in 4.3.0.
    QSslSocketPtr socket = newSocket();
    socket->addCaCertificates(QLatin1String(SRCDIR "certs/qt-test-server-cacert.pem"));
    this->socket = socket;
#ifdef QSSLSOCKET_CERTUNTRUSTED_WORKAROUND
    connect(socket, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(untrustedWorkaroundSlot(QList<QSslError>)));
#endif
    socket->connectToHostEncrypted(QtNetworkSettings::wildcardServerName(), 4443);

    QVERIFY2(socket->waitForEncrypted(3000), qPrintable(socket->errorString()));

    QSslCertificate certificate = socket->peerCertificate();
    QCOMPARE(certificate.subjectInfo(QSslCertificate::CommonName), QString(QtNetworkSettings::serverLocalName() + ".*." + QtNetworkSettings::serverDomainName()));
    QCOMPARE(certificate.issuerInfo(QSslCertificate::CommonName), QtNetworkSettings::serverName());

    socket->close();
}

class SslServer2 : public QTcpServer
{
protected:
    void incomingConnection(int socketDescriptor)
    {
        QSslSocket *socket = new QSslSocket(this);
        socket->ignoreSslErrors();

        // Only set the certificate
        QList<QSslCertificate> localCert = QSslCertificate::fromPath(SRCDIR "certs/fluke.cert");
        QVERIFY(!localCert.isEmpty());
        QVERIFY(localCert.first().handle());
        socket->setLocalCertificate(localCert.first());

        QVERIFY(socket->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState));

        socket->startServerEncryption();
    }
};

void tst_QSslSocket::setEmptyKey()
{
    if (!QSslSocket::supportsSsl())
        return;

    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    SslServer2 server;
    server.listen();

    QSslSocket socket;
    socket.connectToHostEncrypted("127.0.0.1", server.serverPort());

    QTestEventLoop::instance().enterLoop(2);

    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(socket.error(), QAbstractSocket::UnknownSocketError);
}

void tst_QSslSocket::spontaneousWrite()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    SslServer server;
    QSslSocket *receiver = new QSslSocket(this);
    connect(receiver, SIGNAL(readyRead()), SLOT(exitLoop()));

    // connect two sockets to each other:
    QVERIFY(server.listen(QHostAddress::LocalHost));
    receiver->connectToHost("127.0.0.1", server.serverPort());
    QVERIFY(receiver->waitForConnected(5000));
    QVERIFY(server.waitForNewConnection(0));

    QSslSocket *sender = server.socket;
    QVERIFY(sender);
    QVERIFY(sender->state() == QAbstractSocket::ConnectedState);
    receiver->setObjectName("receiver");
    sender->setObjectName("sender");
    receiver->ignoreSslErrors();
    receiver->startClientEncryption();

    // SSL handshake:
    connect(receiver, SIGNAL(encrypted()), SLOT(exitLoop()));
    enterLoop(1);
    QVERIFY(!timeout());
    QVERIFY(sender->isEncrypted());
    QVERIFY(receiver->isEncrypted());

    // make sure there's nothing to be received on the sender:
    while (sender->waitForReadyRead(10) || receiver->waitForBytesWritten(10)) {}

    // spontaneously write something:
    QByteArray data("Hello World");
    sender->write(data);

    // check if the other side receives it:
    enterLoop(1);
    QVERIFY(!timeout());
    QCOMPARE(receiver->bytesAvailable(), qint64(data.size()));
    QCOMPARE(receiver->readAll(), data);
}

void tst_QSslSocket::setReadBufferSize()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    SslServer server;
    QSslSocket *receiver = new QSslSocket(this);
    connect(receiver, SIGNAL(readyRead()), SLOT(exitLoop()));

    // connect two sockets to each other:
    QVERIFY(server.listen(QHostAddress::LocalHost));
    receiver->connectToHost("127.0.0.1", server.serverPort());
    QVERIFY(receiver->waitForConnected(5000));
    QVERIFY(server.waitForNewConnection(0));

    QSslSocket *sender = server.socket;
    QVERIFY(sender);
    QVERIFY(sender->state() == QAbstractSocket::ConnectedState);
    receiver->setObjectName("receiver");
    sender->setObjectName("sender");
    receiver->ignoreSslErrors();
    receiver->startClientEncryption();

    // SSL handshake:
    connect(receiver, SIGNAL(encrypted()), SLOT(exitLoop()));
    enterLoop(1);
    QVERIFY(!timeout());
    QVERIFY(sender->isEncrypted());
    QVERIFY(receiver->isEncrypted());

    QByteArray data(2048, 'b');
    receiver->setReadBufferSize(39 * 1024); // make it a non-multiple of the data.size()

    // saturate the incoming buffer
    while (sender->state() == QAbstractSocket::ConnectedState &&
           receiver->state() == QAbstractSocket::ConnectedState &&
           receiver->bytesAvailable() < receiver->readBufferSize()) {
        sender->write(data);
        //qDebug() << receiver->bytesAvailable() << "<" << receiver->readBufferSize() << (receiver->bytesAvailable() < receiver->readBufferSize());

        while (sender->bytesToWrite())
            QVERIFY(sender->waitForBytesWritten(10));

        // drain it:
        while (receiver->bytesAvailable() < receiver->readBufferSize() &&
               receiver->waitForReadyRead(10)) {}
    }

    //qDebug() << sender->bytesToWrite() << "bytes to write";
    //qDebug() << receiver->bytesAvailable() << "bytes available";

    // send a bit more
    sender->write(data);
    sender->write(data);
    sender->write(data);
    sender->write(data);
    QVERIFY(sender->waitForBytesWritten(10));

    qint64 oldBytesAvailable = receiver->bytesAvailable();

    // now unset the read buffer limit and iterate
    receiver->setReadBufferSize(0);
    enterLoop(1);
    QVERIFY(!timeout());

    QVERIFY(receiver->bytesAvailable() > oldBytesAvailable);
}

class SslServer3 : public QTcpServer
{
    Q_OBJECT
public:
    SslServer3() : socket(0) { }
    QSslSocket *socket;

protected:
    void incomingConnection(int socketDescriptor)
    {
        socket = new QSslSocket(this);
        connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(ignoreErrorSlot()));

        QFile file(SRCDIR "certs/fluke.key");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QSslKey key(file.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
        QVERIFY(!key.isNull());
        socket->setPrivateKey(key);

        QList<QSslCertificate> localCert = QSslCertificate::fromPath(SRCDIR "certs/fluke.cert");
        QVERIFY(!localCert.isEmpty());
        QVERIFY(localCert.first().handle());
        socket->setLocalCertificate(localCert.first());

        QVERIFY(socket->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState));
        QVERIFY(!socket->peerAddress().isNull());
        QVERIFY(socket->peerPort() != 0);
        QVERIFY(!socket->localAddress().isNull());
        QVERIFY(socket->localPort() != 0);
    }

protected slots:
    void ignoreErrorSlot()
    {
        socket->ignoreSslErrors();
    }
};

class ThreadedSslServer: public QThread
{
    Q_OBJECT
public:
    QSemaphore dataReadSemaphore;
    int serverPort;
    bool ok;

    ThreadedSslServer() : serverPort(-1), ok(false)
    { }

    ~ThreadedSslServer()
    {
        if (isRunning()) wait(2000);
        QVERIFY(ok);
    }

signals:
    void listening();

protected:
    void run()
    {
        // if all goes well (no timeouts), this thread will sleep for a total of 500 ms
        // (i.e., 5 times 100 ms, one sleep for each operation)

        SslServer3 server;
        server.listen(QHostAddress::LocalHost);
        serverPort = server.serverPort();
        emit listening();

        // delayed acceptance:
        QTest::qSleep(100);
        server.waitForNewConnection(2000);

        // delayed start of encryption
        QTest::qSleep(100);
        QSslSocket *socket = server.socket;
        socket->ignoreSslErrors();
        socket->startServerEncryption();
        if (!socket->waitForEncrypted(2000))
            return;             // error

        // delayed reading data
        QTest::qSleep(100);
        if (!socket->waitForReadyRead(2000))
            return;             // error
        socket->readAll();
        dataReadSemaphore.release();

        // delayed sending data
        QTest::qSleep(100);
        socket->write("Hello, World");
        while (socket->bytesToWrite())
            if (!socket->waitForBytesWritten(2000))
                return;         // error

        // delayed replying (reading then sending)
        QTest::qSleep(100);
        if (!socket->waitForReadyRead(2000))
            return;             // error
        socket->write("Hello, World");
        while (socket->bytesToWrite())
            if (!socket->waitForBytesWritten(2000))
                return;         // error

        // delayed disconnection:
        QTest::qSleep(100);
        socket->disconnectFromHost();
        if (!socket->waitForDisconnected(2000))
            return;             // error

        delete socket;
        ok = true;
    }
};

void tst_QSslSocket::waitForMinusOne()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    ThreadedSslServer server;
    connect(&server, SIGNAL(listening()), SLOT(exitLoop()));

    // start the thread and wait for it to be ready
    server.start();
    enterLoop(1);
    QVERIFY(!timeout());

    // connect to the server
    QSslSocket socket;
    socket.connectToHost("127.0.0.1", server.serverPort);
    QVERIFY(socket.waitForConnected(-1));
    socket.ignoreSslErrors();
    socket.startClientEncryption();

    // first verification: this waiting should take 200 ms
    QVERIFY2(socket.waitForEncrypted(-1), qPrintable(socket.errorString()));
    QVERIFY(socket.isEncrypted());
    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(socket.bytesAvailable(), Q_INT64_C(0));

    // second verification: write and make sure the other side got it (100 ms)
    socket.write("How are you doing?");
    QVERIFY(socket.bytesToWrite() != 0);
    QVERIFY(socket.waitForBytesWritten(-1));
    QVERIFY(server.dataReadSemaphore.tryAcquire(1, 2000));

    // third verification: it should wait for 100 ms:
    QVERIFY(socket.waitForReadyRead(-1));
    QVERIFY(socket.isEncrypted());
    QCOMPARE(socket.state(), QAbstractSocket::ConnectedState);
    QVERIFY(socket.bytesAvailable() != 0);

    // fourth verification: deadlock prevention:
    // we write and then wait for reading; the other side needs to receive before
    // replying (100 ms delay)
    socket.write("I'm doing just fine!");
    QVERIFY(socket.bytesToWrite() != 0);
    QVERIFY(socket.waitForReadyRead(-1));

    // fifth verification: it should wait for 200 ms more
    QVERIFY(socket.waitForDisconnected(-1));
}

class VerifyServer : public QTcpServer
{
    Q_OBJECT
public:
    VerifyServer() : socket(0) { }
    QSslSocket *socket;

protected:
    void incomingConnection(int socketDescriptor)
    {
        socket = new QSslSocket(this);

        socket->setPrivateKey(SRCDIR "certs/fluke.key");
        socket->setLocalCertificate(SRCDIR "certs/fluke.cert");
        socket->setSocketDescriptor(socketDescriptor);
        socket->startServerEncryption();
    }
};

void tst_QSslSocket::verifyMode()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    QSslSocket socket;
    QCOMPARE(socket.peerVerifyMode(), QSslSocket::AutoVerifyPeer);
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    QCOMPARE(socket.peerVerifyMode(), QSslSocket::VerifyNone);
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    socket.setPeerVerifyMode(QSslSocket::VerifyPeer);
    QCOMPARE(socket.peerVerifyMode(), QSslSocket::VerifyPeer);

    socket.connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QVERIFY(!socket.waitForEncrypted());

    QList<QSslError> expectedErrors = QList<QSslError>()
                                      << QSslError(QSslError::SelfSignedCertificate, socket.peerCertificate());
    QCOMPARE(socket.sslErrors(), expectedErrors);
    socket.abort();

    VerifyServer server;
    server.listen();

    QSslSocket clientSocket;
    clientSocket.connectToHostEncrypted("127.0.0.1", server.serverPort());
    clientSocket.ignoreSslErrors();

    QEventLoop loop;
    QTimer::singleShot(5000, &loop, SLOT(quit()));
    connect(&clientSocket, SIGNAL(encrypted()), &loop, SLOT(quit()));
    loop.exec();

    QVERIFY(clientSocket.isEncrypted());
    QVERIFY(server.socket->sslErrors().isEmpty());
}

void tst_QSslSocket::verifyDepth()
{
    QSslSocket socket;
    QCOMPARE(socket.peerVerifyDepth(), 0);
    socket.setPeerVerifyDepth(1);
    QCOMPARE(socket.peerVerifyDepth(), 1);
    QTest::ignoreMessage(QtWarningMsg, "QSslSocket::setPeerVerifyDepth: cannot set negative depth of -1");
    socket.setPeerVerifyDepth(-1);
    QCOMPARE(socket.peerVerifyDepth(), 1);
}

void tst_QSslSocket::peerVerifyError()
{
    QSslSocketPtr socket = newSocket();
    QSignalSpy sslErrorsSpy(socket, SIGNAL(sslErrors(QList<QSslError>)));
    QSignalSpy peerVerifyErrorSpy(socket, SIGNAL(peerVerifyError(QSslError)));

    socket->connectToHostEncrypted(QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first().toString(), 443);
    QVERIFY(!socket->waitForEncrypted(10000));
    QVERIFY(!peerVerifyErrorSpy.isEmpty());
    QVERIFY(!sslErrorsSpy.isEmpty());
    QCOMPARE(qVariantValue<QSslError>(peerVerifyErrorSpy.last().at(0)).error(), QSslError::HostNameMismatch);
    QCOMPARE(qVariantValue<QList<QSslError> >(sslErrorsSpy.at(0).at(0)).size(), peerVerifyErrorSpy.size());
}

void tst_QSslSocket::disconnectFromHostWhenConnecting()
{
    QSslSocketPtr socket = newSocket();
    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 993);
    socket->ignoreSslErrors();
    socket->write("XXXX LOGOUT\r\n");
    QAbstractSocket::SocketState state = socket->state();
    // without proxy, the state will be HostLookupState;
    // with    proxy, the state will be ConnectingState.
    QVERIFY(socket->state() == QAbstractSocket::HostLookupState ||
    		socket->state() == QAbstractSocket::ConnectingState);
    socket->disconnectFromHost();
    // the state of the socket must be the same before and after calling
    // disconnectFromHost()
    QCOMPARE(state, socket->state());
    QVERIFY(socket->state() == QAbstractSocket::HostLookupState ||
    		socket->state() == QAbstractSocket::ConnectingState);
    QVERIFY(socket->waitForDisconnected(5000));
    QCOMPARE(socket->state(), QAbstractSocket::UnconnectedState);
    // we did not call close, so the socket must be still open
    QVERIFY(socket->isOpen());
    QCOMPARE(socket->bytesToWrite(), qint64(0));
}

void tst_QSslSocket::disconnectFromHostWhenConnected()
{
    QSslSocketPtr socket = newSocket();
    socket->connectToHostEncrypted(QtNetworkSettings::serverName(), 993);
    socket->ignoreSslErrors();
    QVERIFY(socket->waitForEncrypted(5000));
    socket->write("XXXX LOGOUT\r\n");
    QCOMPARE(socket->state(), QAbstractSocket::ConnectedState);
    socket->disconnectFromHost();
    QCOMPARE(socket->state(), QAbstractSocket::ClosingState);
    QVERIFY(socket->waitForDisconnected(5000));
    QCOMPARE(socket->bytesToWrite(), qint64(0));
}

void tst_QSslSocket::resetProxy()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    // check fix for bug 199941

    QNetworkProxy goodProxy(QNetworkProxy::NoProxy);
    QNetworkProxy badProxy(QNetworkProxy::HttpProxy, "thisCannotWorkAbsolutelyNotForSure", 333);

    // make sure the connection works, and then set a nonsense proxy, and then
    // make sure it does not work anymore
    QSslSocket socket;
    socket.addCaCertificates(QLatin1String(SRCDIR "certs/qt-test-server-cacert.pem"));
    socket.setProxy(goodProxy);
    socket.connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QVERIFY2(socket.waitForConnected(10000), qPrintable(socket.errorString()));
    socket.abort();
    socket.setProxy(badProxy);
    socket.connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QVERIFY(! socket.waitForConnected(10000));

    // now the other way round:
    // set the nonsense proxy and make sure the connection does not work,
    // and then set the right proxy and make sure it works
    QSslSocket socket2;
    socket2.addCaCertificates(QLatin1String(SRCDIR "certs/qt-test-server-cacert.pem"));
    socket2.setProxy(badProxy);
    socket2.connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QVERIFY(! socket2.waitForConnected(10000));
    socket2.abort();
    socket2.setProxy(goodProxy);
    socket2.connectToHostEncrypted(QtNetworkSettings::serverName(), 443);
    QVERIFY2(socket2.waitForConnected(10000), qPrintable(socket.errorString()));
}

#endif // QT_NO_OPENSSL

QTEST_MAIN(tst_QSslSocket)
#include "tst_qsslsocket.moc"
