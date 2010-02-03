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

#include <qcoreapplication.h>
#include <qfileinfo.h>
#include <qdatastream.h>
#include <qudpsocket.h>
#include <qhostaddress.h>
#include <qhostinfo.h>
#include <qmap.h>
#include <QNetworkProxy>

#include <qstringlist.h>
#include "../network-settings.h"

Q_DECLARE_METATYPE(QHostAddress)

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QUdpSocket)

class tst_QUdpSocket : public QObject
{
    Q_OBJECT

public:
    tst_QUdpSocket();
    virtual ~tst_QUdpSocket();


public slots:
    void initTestCase_data();
    void init();
    void cleanup();
private slots:
    void constructing();
    void unconnectedServerAndClientTest();
    void broadcasting();
    void loop_data();
    void loop();
    void ipv6Loop_data();
    void ipv6Loop();
    void readLine();
    void pendingDatagramSize();
    void writeDatagram();
    void performance();
    void bindMode();
    void writeDatagramToNonExistingPeer_data();
    void writeDatagramToNonExistingPeer();
    void writeToNonExistingPeer_data();
    void writeToNonExistingPeer();
    void outOfProcessConnectedClientServerTest();
    void outOfProcessUnconnectedClientServerTest();
    void zeroLengthDatagram();

protected slots:
    void empty_readyReadSlot();
    void empty_connectedSlot();
};

tst_QUdpSocket::tst_QUdpSocket()
{
    Q_SET_DEFAULT_IAP
}

tst_QUdpSocket::~tst_QUdpSocket()
{
}

void tst_QUdpSocket::initTestCase_data()
{
    QTest::addColumn<bool>("setProxy");
    QTest::addColumn<int>("proxyType");

    QTest::newRow("WithoutProxy") << false << 0;
    QTest::newRow("WithSocks5Proxy") << true << int(QNetworkProxy::Socks5Proxy);
}

void tst_QUdpSocket::init()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
        QFETCH_GLOBAL(int, proxyType);
        if (proxyType == QNetworkProxy::Socks5Proxy) {
            QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080));
        }
    }
}

void tst_QUdpSocket::cleanup()
{
        QNetworkProxy::setApplicationProxy(QNetworkProxy::DefaultProxy);
}


//----------------------------------------------------------------------------------

void tst_QUdpSocket::constructing()
{
    QUdpSocket socket;

    QVERIFY(socket.isSequential());
    QVERIFY(!socket.isOpen());
    QVERIFY(socket.socketType() == QUdpSocket::UdpSocket);
    QCOMPARE((int) socket.bytesAvailable(), 0);
    QCOMPARE(socket.canReadLine(), false);
    QCOMPARE(socket.readLine(), QByteArray());
    QCOMPARE(socket.socketDescriptor(), -1);
    QCOMPARE(socket.error(), QUdpSocket::UnknownSocketError);
    QCOMPARE(socket.errorString(), QString("Unknown error"));

    // Check the state of the socket api
}

void tst_QUdpSocket::unconnectedServerAndClientTest()
{
    QUdpSocket serverSocket;

    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");

    QSignalSpy stateChangedSpy(&serverSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)));
    QVERIFY2(serverSocket.bind(), serverSocket.errorString().toLatin1().constData());
    QCOMPARE(stateChangedSpy.count(), 1);

    const char *message[] = {"Yo mista", "Yo", "Wassap"};

    QHostAddress serverAddress = QHostAddress::LocalHost;
    if (!(serverSocket.localAddress() == QHostAddress::Any))
        serverAddress = serverSocket.localAddress();

    for (int i = 0; i < 3; ++i) {
        QUdpSocket clientSocket;
        QCOMPARE(int(clientSocket.writeDatagram(message[i], strlen(message[i]),
                                               serverAddress, serverSocket.localPort())),
                int(strlen(message[i])));
        char buf[1024];
        QHostAddress host;
        quint16 port;
        QVERIFY(serverSocket.waitForReadyRead(5000));
        QCOMPARE(int(serverSocket.readDatagram(buf, sizeof(buf), &host, &port)),
                int(strlen(message[i])));
        buf[strlen(message[i])] = '\0';
        QCOMPARE(QByteArray(buf), QByteArray(message[i]));
    }
}

//----------------------------------------------------------------------------------

void tst_QUdpSocket::broadcasting()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
        QFETCH_GLOBAL(int, proxyType);
        if (proxyType == QNetworkProxy::Socks5Proxy) {
            QSKIP("With socks5 Broadcast is not supported.", SkipSingle);
        }
    }
#ifdef Q_OS_AIX
    QSKIP("Broadcast does not work on darko", SkipAll);
#endif
    const char *message[] = {"Yo mista", "", "Yo", "Wassap"};

    for (int i = 0; i < 4; ++i) {
        QUdpSocket serverSocket;
        QVERIFY2(serverSocket.bind(QHostAddress::Any, 5000), serverSocket.errorString().toLatin1().constData());

        QCOMPARE(serverSocket.state(), QUdpSocket::BoundState);

        connect(&serverSocket, SIGNAL(readyRead()), SLOT(empty_readyReadSlot()));

        QUdpSocket broadcastSocket;

        for (int j = 0; j < 100; ++j) {
            broadcastSocket.writeDatagram(message[i], strlen(message[i]),
                QHostAddress::Broadcast, 5000);
            QTestEventLoop::instance().enterLoop(15);
            if (QTestEventLoop::instance().timeout()) {
#if defined(Q_OS_FREEBSD)
                QEXPECT_FAIL("",
                             "Broadcasting to 255.255.255.255 does not work on FreeBSD",
                             Abort);
                QVERIFY(false); // seems that QFAIL() doesn't respect the QEXPECT_FAIL() :/
#endif
                QFAIL("Network operation timed out");
            }
            QVERIFY(serverSocket.hasPendingDatagrams());

            do {
                QByteArray arr; arr.resize(serverSocket.pendingDatagramSize() + 1);
                QHostAddress host;
                quint16 port;
                QCOMPARE((int) serverSocket.readDatagram(arr.data(), arr.size() - 1, &host, &port),
                    (int) strlen(message[i]));
                arr.resize(strlen(message[i]));
                QCOMPARE(arr, QByteArray(message[i]));
            } while (serverSocket.hasPendingDatagrams());
        }
    }
}

//----------------------------------------------------------------------------------

void tst_QUdpSocket::loop_data()
{
    QTest::addColumn<QByteArray>("peterMessage");
    QTest::addColumn<QByteArray>("paulMessage");
    QTest::addColumn<bool>("success");

    QTest::newRow("\"Almond!\" | \"Joy!\"") << QByteArray("Almond!") << QByteArray("Joy!") << true;
    QTest::newRow("\"A\" | \"B\"") << QByteArray("A") << QByteArray("B") << true;
    QTest::newRow("\"AB\" | \"B\"") << QByteArray("AB") << QByteArray("B") << true;
    QTest::newRow("\"AB\" | \"BB\"") << QByteArray("AB") << QByteArray("BB") << true;
    QTest::newRow("\"A\\0B\" | \"B\\0B\"") << QByteArray::fromRawData("A\0B", 3) << QByteArray::fromRawData("B\0B", 3) << true;
    QTest::newRow("\"(nil)\" | \"(nil)\"") << QByteArray() << QByteArray() << true;
    QTest::newRow("Bigmessage") << QByteArray(600, '@') << QByteArray(600, '@') << true;
}

void tst_QUdpSocket::loop()
{
    QFETCH(QByteArray, peterMessage);
    QFETCH(QByteArray, paulMessage);
    QFETCH(bool, success);

    QUdpSocket peter;
    QUdpSocket paul;

    QVERIFY2(peter.bind(), peter.errorString().toLatin1().constData());
    QVERIFY2(paul.bind(), paul.errorString().toLatin1().constData());

    QHostAddress peterAddress = QHostAddress::LocalHost;
    if (!(peter.localAddress() == QHostAddress::Any))
        peterAddress = peter.localAddress();
    QHostAddress pualAddress = QHostAddress::LocalHost;
    if (!(paul.localAddress() == QHostAddress::Any))
        pualAddress = paul.localAddress();

    QCOMPARE(peter.writeDatagram(peterMessage.data(), peterMessage.length(),
                                pualAddress, paul.localPort()), qint64(peterMessage.length()));
    QCOMPARE(paul.writeDatagram(paulMessage.data(), paulMessage.length(),
                               peterAddress, peter.localPort()), qint64(paulMessage.length()));

    QVERIFY(peter.waitForReadyRead(9000));
    QVERIFY(paul.waitForReadyRead(9000));
    char peterBuffer[16*1024];
    char paulBuffer[16*1024];
    if (success) {
        QCOMPARE(peter.readDatagram(peterBuffer, sizeof(peterBuffer)), qint64(paulMessage.length()));
        QCOMPARE(paul.readDatagram(paulBuffer, sizeof(peterBuffer)), qint64(peterMessage.length()));
    } else {
        QVERIFY(peter.readDatagram(peterBuffer, sizeof(peterBuffer)) != paulMessage.length());
        QVERIFY(paul.readDatagram(paulBuffer, sizeof(peterBuffer)) != peterMessage.length());
    }

    QCOMPARE(QByteArray(peterBuffer, paulMessage.length()), paulMessage);
    QCOMPARE(QByteArray(paulBuffer, peterMessage.length()), peterMessage);
}

//----------------------------------------------------------------------------------

void tst_QUdpSocket::ipv6Loop_data()
{
    loop_data();
}

void tst_QUdpSocket::ipv6Loop()
{
#if defined(Q_OS_SYMBIAN)
    QSKIP("Symbian IPv6 is not yet supported", SkipAll);
#endif
    QFETCH(QByteArray, peterMessage);
    QFETCH(QByteArray, paulMessage);
    QFETCH(bool, success);

    QUdpSocket peter;
    QUdpSocket paul;

    quint16 peterPort = 28124;
    quint16 paulPort = 28123;

    if (!peter.bind(QHostAddress::LocalHostIPv6, peterPort)) {
    QCOMPARE(peter.error(), QUdpSocket::UnsupportedSocketOperationError);
    } else {
    QVERIFY(paul.bind(QHostAddress::LocalHostIPv6, paulPort));

    QCOMPARE(peter.writeDatagram(peterMessage.data(), peterMessage.length(), QHostAddress("::1"),
                                    paulPort), qint64(peterMessage.length()));
    QCOMPARE(paul.writeDatagram(paulMessage.data(), paulMessage.length(),
                                   QHostAddress("::1"), peterPort), qint64(paulMessage.length()));

    char peterBuffer[16*1024];
    char paulBuffer[16*1024];
#if !defined(Q_OS_WINCE)
        QVERIFY(peter.waitForReadyRead(5000));
        QVERIFY(paul.waitForReadyRead(5000));
#else
        QVERIFY(peter.waitForReadyRead(15000));
        QVERIFY(paul.waitForReadyRead(15000));
#endif
    if (success) {
        QCOMPARE(peter.readDatagram(peterBuffer, sizeof(peterBuffer)), qint64(paulMessage.length()));
        QCOMPARE(paul.readDatagram(paulBuffer, sizeof(peterBuffer)), qint64(peterMessage.length()));
    } else {
        QVERIFY(peter.readDatagram(peterBuffer, sizeof(peterBuffer)) != paulMessage.length());
        QVERIFY(paul.readDatagram(paulBuffer, sizeof(peterBuffer)) != peterMessage.length());
    }

    QCOMPARE(QByteArray(peterBuffer, paulMessage.length()), paulMessage);
    QCOMPARE(QByteArray(paulBuffer, peterMessage.length()), peterMessage);
    }
}

void tst_QUdpSocket::empty_readyReadSlot()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_QUdpSocket::empty_connectedSlot()
{
    QTestEventLoop::instance().exitLoop();
}

//----------------------------------------------------------------------------------

void tst_QUdpSocket::readLine()
{
    QUdpSocket socket1;
    QVERIFY2(socket1.bind(), socket1.errorString().toLatin1().constData());

    QUdpSocket socket2;
    socket2.connectToHost("127.0.0.1", socket1.localPort());
    QVERIFY(socket2.waitForConnected(5000));
}

//----------------------------------------------------------------------------------

void tst_QUdpSocket::pendingDatagramSize()
{
    QUdpSocket server;
    QVERIFY2(server.bind(), server.errorString().toLatin1().constData());

    QHostAddress serverAddress = QHostAddress::LocalHost;
    if (!(server.localAddress() == QHostAddress::Any))
        serverAddress = server.localAddress();

    QUdpSocket client;
    QVERIFY(client.writeDatagram("this is", 7, serverAddress, server.localPort()) == 7);
    QVERIFY(client.writeDatagram(0, 0, serverAddress, server.localPort()) == 0);
    QVERIFY(client.writeDatagram("3 messages", 10, serverAddress, server.localPort()) == 10);

    char c = 0;
    QVERIFY(server.waitForReadyRead());
    if (server.hasPendingDatagrams()) {
#if defined Q_OS_HPUX && defined __ia64
        QEXPECT_FAIL("", "HP-UX 11i v2 can't determine the datagram size correctly.", Abort);
#endif
        QCOMPARE(server.pendingDatagramSize(), qint64(7));
        c = '\0';
        QCOMPARE(server.readDatagram(&c, 1), qint64(1));
        QCOMPARE(c, 't');
        c = '\0';
    } else {
        QSKIP("does not have the 1st datagram", SkipSingle);
    }

    if (server.hasPendingDatagrams()) {
        QCOMPARE(server.pendingDatagramSize(), qint64(0));
        QCOMPARE(server.readDatagram(&c, 1), qint64(0));
        QCOMPARE(c, '\0'); // untouched
    c = '\0';
    } else {
        QSKIP("does not have the 2nd datagram", SkipSingle);
    }

    if (server.hasPendingDatagrams()) {
        QCOMPARE(server.pendingDatagramSize(), qint64(10));
        QCOMPARE(server.readDatagram(&c, 1), qint64(1));
        QCOMPARE(c, '3');
    } else {
        QSKIP("does not have the 3rd datagram", SkipSingle);
    }
}


void tst_QUdpSocket::writeDatagram()
{
    QUdpSocket server;
    QVERIFY2(server.bind(), server.errorString().toLatin1().constData());

    QHostAddress serverAddress = QHostAddress::LocalHost;
    if (!(server.localAddress() == QHostAddress::Any))
        serverAddress = server.localAddress();

    QUdpSocket client;

    qRegisterMetaType<qint64>("qint64");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    for(int i=0;;i++) {
        QSignalSpy errorspy(&client, SIGNAL(error(QAbstractSocket::SocketError)));
        QSignalSpy bytesspy(&client, SIGNAL(bytesWritten(qint64)));

        qint64 written = client.writeDatagram(QByteArray(i * 1024, 'w'), serverAddress,
                                                  server.localPort());

        if (written != i * 1024) {
#if defined (Q_OS_HPUX)
            QSKIP("HP-UX 11.11 on hai (PA-RISC 64) truncates too long datagrams.", SkipSingle);
#endif
            QCOMPARE(bytesspy.count(), 0);
            QCOMPARE(errorspy.count(), 1);
            QCOMPARE(*static_cast<const int *>(errorspy.at(0).at(0).constData()),
                    int(QUdpSocket::DatagramTooLargeError));
            QCOMPARE(client.error(), QUdpSocket::DatagramTooLargeError);
            break;
        }
        QVERIFY(bytesspy.count() == 1);
        QCOMPARE(*static_cast<const qint64 *>(bytesspy.at(0).at(0).constData()),
                qint64(i * 1024));
        QCOMPARE(errorspy.count(), 0);
        if (!server.waitForReadyRead(5000))
            QSKIP(QString("UDP packet lost at size %1, unable to complete the test.").arg(i * 1024).toLatin1().data(), SkipSingle);
        QCOMPARE(server.pendingDatagramSize(), qint64(i * 1024));
        QCOMPARE(server.readDatagram(0, 0), qint64(0));
    }
}

void tst_QUdpSocket::performance()
{
#if defined(Q_OS_SYMBIAN)
    // Large packets seems not to go through on Symbian
    // Reason might be also fragmentation due to VPN connection etc

    QFETCH_GLOBAL(bool, setProxy);
    QFETCH_GLOBAL(int, proxyType);

    int arrSize = 8192;
    if (setProxy && proxyType == QNetworkProxy::Socks5Proxy)
        arrSize = 1024;

    QByteArray arr(arrSize, '@');
#else
    QByteArray arr(8192, '@');
#endif // Q_OS_SYMBIAN

    QUdpSocket server;
    QVERIFY2(server.bind(), server.errorString().toLatin1().constData());

    QHostAddress serverAddress = QHostAddress::LocalHost;
    if (!(server.localAddress() == QHostAddress::Any))
        serverAddress = server.localAddress();

    QUdpSocket client;
    client.connectToHost(serverAddress, server.localPort());

    QTime stopWatch;
    stopWatch.start();

    qint64 nbytes = 0;
    while (stopWatch.elapsed() < 5000) {
        for (int i = 0; i < 100; ++i) {
            if (client.write(arr.data(), arr.size()) > 0) {
                do {
                    nbytes += server.readDatagram(arr.data(), arr.size());
                } while (server.hasPendingDatagrams());
            }
        }
    }

    float secs = stopWatch.elapsed() / 1000.0;
    qDebug("\t%.2fMB/%.2fs: %.2fMB/s", float(nbytes / (1024.0*1024.0)),
           secs, float(nbytes / (1024.0*1024.0)) / secs);

#if defined(Q_OS_SYMBIAN)
    if(nbytes == 0) {
        qDebug("No bytes passed through local UDP socket, since UDP socket write returns EWOULDBLOCK");
        qDebug("Should try with blocking sockets, but it is not currently possible due to Open C defect");
    }
#endif

}

void tst_QUdpSocket::bindMode()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
        QFETCH_GLOBAL(int, proxyType);
        if (proxyType == QNetworkProxy::Socks5Proxy) {
            QSKIP("With socks5 explicit port binding is not supported.", SkipAll);
        }
    }

    QUdpSocket socket;
    QVERIFY2(socket.bind(), socket.errorString().toLatin1().constData());
    QUdpSocket socket2;
    QVERIFY(!socket2.bind(socket.localPort()));
#if defined(Q_OS_SYMBIAN)
    if(RProcess().HasCapability(ECapabilityNetworkControl)) {
        qDebug("Test executed *with* NetworkControl capability");
        // In Symbian OS ReuseAddressHint together with NetworkControl capability
        // gives application *always* right to bind to port. I.e. it does not matter
        // if first socket was bound with any bind flag. Since autotests in Symbian
        // are currently executed with ALL -TCB rights, this path is the one executed.
        QVERIFY(socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
        socket.close();
        socket2.close();

        QVERIFY2(socket.bind(0, QUdpSocket::ShareAddress), socket.errorString().toLatin1().constData());
        QVERIFY(!socket2.bind(socket.localPort()));
        QVERIFY2(socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint), socket2.errorString().toLatin1().constData());
        socket.close();
        socket2.close();

        QVERIFY2(socket.bind(0, QUdpSocket::DontShareAddress), socket.errorString().toLatin1().constData());
        QVERIFY(!socket2.bind(socket.localPort()));
        QVERIFY(socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
        socket.close();
        socket2.close();
    } else {
        qDebug("Test executed *without* NetworkControl capability");
        // If we don't have NetworkControl capability, attempt to bind already bound
        // address will *always* fail. I.e. it does not matter if first socket was
        // bound with any bind flag.
        QVERIFY(!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
        socket.close();

        QVERIFY2(socket.bind(0, QUdpSocket::ShareAddress), socket.errorString().toLatin1().constData());
        QVERIFY(!socket2.bind(socket.localPort()));
        QVERIFY2(!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint), socket2.errorString().toLatin1().constData());
        socket.close();

        QVERIFY2(socket.bind(0, QUdpSocket::DontShareAddress), socket.errorString().toLatin1().constData());
        QVERIFY(!socket2.bind(socket.localPort()));
        QVERIFY(!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
        socket.close();
    }
#elif defined(Q_OS_UNIX)
    QVERIFY(!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
    socket.close();
    QVERIFY2(socket.bind(0, QUdpSocket::ShareAddress), socket.errorString().toLatin1().constData());
    QVERIFY2(socket2.bind(socket.localPort()), socket2.errorString().toLatin1().constData());
    socket2.close();
    QVERIFY2(socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint), socket2.errorString().toLatin1().constData());
#else

    // Depending on the user's privileges, this or will succeed or
    // fail. Admins are allowed to reuse the address, but nobody else.
    if (!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint), socket2.errorString().toLatin1().constData())
        qWarning("Failed to bind with QUdpSocket::ReuseAddressHint, user isn't an adminstrator?");
    socket.close();
    QVERIFY2(socket.bind(0, QUdpSocket::ShareAddress), socket.errorString().toLatin1().constData());
    QVERIFY(!socket2.bind(socket.localPort()));
    socket.close();
    QVERIFY2(socket.bind(0, QUdpSocket::DontShareAddress), socket.errorString().toLatin1().constData());
    QVERIFY(!socket2.bind(socket.localPort()));
    QVERIFY(!socket2.bind(socket.localPort(), QUdpSocket::ReuseAddressHint));
#endif
}

void tst_QUdpSocket::writeDatagramToNonExistingPeer_data()
{
    QTest::addColumn<bool>("bind");
    QTest::addColumn<QHostAddress>("peerAddress");
    QHostAddress localhost(QHostAddress::LocalHost);
#if !defined(Q_OS_SYMBIAN)
    QHostAddress remote = QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first();
#endif

    QTest::newRow("localhost-unbound") << false << localhost;
    QTest::newRow("localhost-bound") << true << localhost;
#if !defined(Q_OS_SYMBIAN)
    QTest::newRow("remote-unbound") << false << remote;
    QTest::newRow("remote-bound") << true << remote;
#endif
}

void tst_QUdpSocket::writeDatagramToNonExistingPeer()
{
    QFETCH(bool, bind);
    QFETCH(QHostAddress, peerAddress);

    quint16 peerPort = 33533 + int(bind);

    QUdpSocket sUdp;
    QSignalSpy sReadyReadSpy(&sUdp, SIGNAL(readyRead()));
    if (bind)
        QVERIFY(sUdp.bind());
    QCOMPARE(sUdp.writeDatagram("", 1, peerAddress, peerPort), qint64(1));
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sReadyReadSpy.count(), 0);
}

void tst_QUdpSocket::writeToNonExistingPeer_data()
{
    QTest::addColumn<QHostAddress>("peerAddress");
    QHostAddress localhost(QHostAddress::LocalHost);
#if !defined(Q_OS_SYMBIAN)
    QHostAddress remote = QHostInfo::fromName(QtNetworkSettings::serverName()).addresses().first();
#endif
    // write (required to be connected)
    QTest::newRow("localhost") << localhost;
#if !defined(Q_OS_SYMBIAN)
    QTest::newRow("remote") << remote;
#endif
}

void tst_QUdpSocket::writeToNonExistingPeer()
{
    QSKIP("Connected-mode UDP sockets and their behaviour are erratic", SkipAll);
    QFETCH(QHostAddress, peerAddress);
    quint16 peerPort = 34534;
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");

    QUdpSocket sConnected;
    QSignalSpy sConnectedReadyReadSpy(&sConnected, SIGNAL(readyRead()));
    QSignalSpy sConnectedErrorSpy(&sConnected, SIGNAL(error(QAbstractSocket::SocketError)));
    sConnected.connectToHost(peerAddress, peerPort, QIODevice::ReadWrite);

    // the first write succeeds...
    QCOMPARE(sConnected.write("", 1), qint64(1));

    // the second one should fail!
    QTest::qSleep(1000);                   // do not process events
    QCOMPARE(sConnected.write("", 1), qint64(-1));
    QCOMPARE(int(sConnected.error()), int(QUdpSocket::ConnectionRefusedError));

    // the third one will succeed...
    QCOMPARE(sConnected.write("", 1), qint64(1));
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(sConnectedReadyReadSpy.count(), 0);
    QCOMPARE(sConnectedErrorSpy.count(), 1);
    QCOMPARE(int(sConnected.error()), int(QUdpSocket::ConnectionRefusedError));

    // we should now get a read error
    QCOMPARE(sConnected.write("", 1), qint64(1));
    QTest::qSleep(1000);                   // do not process events
    char buf[2];
    QVERIFY(!sConnected.hasPendingDatagrams());
    QCOMPARE(sConnected.bytesAvailable(), Q_INT64_C(0));
    QCOMPARE(sConnected.pendingDatagramSize(), Q_INT64_C(-1));
    QCOMPARE(sConnected.readDatagram(buf, 2), Q_INT64_C(-1));
    QCOMPARE(int(sConnected.error()), int(QUdpSocket::ConnectionRefusedError));

    QCOMPARE(sConnected.write("", 1), qint64(1));
    QTest::qSleep(1000);                   // do not process events
    QCOMPARE(sConnected.read(buf, 2), Q_INT64_C(0));
    QCOMPARE(int(sConnected.error()), int(QUdpSocket::ConnectionRefusedError));

    // we should still be connected
    QCOMPARE(int(sConnected.state()), int(QUdpSocket::ConnectedState));
}

void tst_QUdpSocket::outOfProcessConnectedClientServerTest()
{
#if defined(Q_OS_WINCE) || defined (Q_OS_SYMBIAN)
    QSKIP("This test depends on reading data from QProcess (not supported on Qt/WinCE and Symbian).", SkipAll);
#endif
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else

    QProcess serverProcess;
    serverProcess.start(QLatin1String("clientserver/clientserver server 1 1"),
                        QIODevice::ReadWrite | QIODevice::Text);
    QVERIFY2(serverProcess.waitForStarted(3000),
             qPrintable("Failed to start subprocess: " + serverProcess.errorString()));

    // Wait until the server has started and reports success.
    while (!serverProcess.canReadLine())
        QVERIFY(serverProcess.waitForReadyRead(3000));
    QByteArray serverGreeting = serverProcess.readLine();
    QVERIFY(serverGreeting != QByteArray("XXX\n"));
    int serverPort = serverGreeting.trimmed().toInt();
    QVERIFY(serverPort > 0 && serverPort < 65536);

    QProcess clientProcess;
    clientProcess.start(QString::fromLatin1("clientserver/clientserver connectedclient %1 %2")
                        .arg(QLatin1String("127.0.0.1")).arg(serverPort),
                        QIODevice::ReadWrite | QIODevice::Text);
    QVERIFY2(clientProcess.waitForStarted(3000),
             qPrintable("Failed to start subprocess: " + clientProcess.errorString()));

    // Wait until the server has started and reports success.
    while (!clientProcess.canReadLine())
        QVERIFY(clientProcess.waitForReadyRead(3000));
    QByteArray clientGreeting = clientProcess.readLine();
    QCOMPARE(clientGreeting, QByteArray("ok\n"));

    // Let the client and server talk for 3 seconds
    QTest::qWait(3000);

    QStringList serverData = QString::fromLocal8Bit(serverProcess.readAll()).split("\n");
    QStringList clientData = QString::fromLocal8Bit(clientProcess.readAll()).split("\n");
    QVERIFY(serverData.size() > 5);
    QVERIFY(clientData.size() > 5);

    for (int i = 0; i < clientData.size() / 2; ++i) {
        QCOMPARE(clientData.at(i * 2), QString("readData()"));
        QCOMPARE(serverData.at(i * 3), QString("readData()"));

        QString cdata = clientData.at(i * 2 + 1);
        QString sdata = serverData.at(i * 3 + 1);
        QVERIFY(cdata.startsWith(QLatin1String("got ")));

        QCOMPARE(cdata.mid(4).trimmed().toInt(), sdata.mid(4).trimmed().toInt() * 2);
        QVERIFY(serverData.at(i * 3 + 2).startsWith(QLatin1String("sending ")));
        QCOMPARE(serverData.at(i * 3 + 2).trimmed().mid(8).toInt(),
                 sdata.mid(4).trimmed().toInt() * 2);
    }

    clientProcess.kill();
    QVERIFY(clientProcess.waitForFinished());
    serverProcess.kill();
    QVERIFY(serverProcess.waitForFinished());
#endif
}

void tst_QUdpSocket::outOfProcessUnconnectedClientServerTest()
{
#if defined(Q_OS_WINCE) || defined (Q_OS_SYMBIAN)
    QSKIP("This test depends on reading data from QProcess (not supported on Qt/WinCE and Symbian).", SkipAll);
#endif
#if defined(QT_NO_PROCESS)
    QSKIP("Qt was compiled with QT_NO_PROCESS", SkipAll);
#else

    QProcess serverProcess;
    serverProcess.start(QLatin1String("clientserver/clientserver server 1 1"),
                        QIODevice::ReadWrite | QIODevice::Text);
    QVERIFY2(serverProcess.waitForStarted(3000),
             qPrintable("Failed to start subprocess: " + serverProcess.errorString()));

    // Wait until the server has started and reports success.
    while (!serverProcess.canReadLine())
        QVERIFY(serverProcess.waitForReadyRead(3000));
    QByteArray serverGreeting = serverProcess.readLine();
    QVERIFY(serverGreeting != QByteArray("XXX\n"));
    int serverPort = serverGreeting.trimmed().toInt();
    QVERIFY(serverPort > 0 && serverPort < 65536);

    QProcess clientProcess;
    clientProcess.start(QString::fromLatin1("clientserver/clientserver unconnectedclient %1 %2")
                        .arg(QLatin1String("127.0.0.1")).arg(serverPort),
                        QIODevice::ReadWrite | QIODevice::Text);
    QVERIFY2(clientProcess.waitForStarted(3000),
             qPrintable("Failed to start subprocess: " + clientProcess.errorString()));

    // Wait until the server has started and reports success.
    while (!clientProcess.canReadLine())
        QVERIFY(clientProcess.waitForReadyRead(3000));
    QByteArray clientGreeting = clientProcess.readLine();
    QCOMPARE(clientGreeting, QByteArray("ok\n"));

    // Let the client and server talk for 3 seconds
    QTest::qWait(3000);

    QStringList serverData = QString::fromLocal8Bit(serverProcess.readAll()).split("\n");
    QStringList clientData = QString::fromLocal8Bit(clientProcess.readAll()).split("\n");

    QVERIFY(serverData.size() > 5);
    QVERIFY(clientData.size() > 5);

    for (int i = 0; i < clientData.size() / 2; ++i) {
        QCOMPARE(clientData.at(i * 2), QString("readData()"));
        QCOMPARE(serverData.at(i * 3), QString("readData()"));

        QString cdata = clientData.at(i * 2 + 1);
        QString sdata = serverData.at(i * 3 + 1);
        QVERIFY(cdata.startsWith(QLatin1String("got ")));

        QCOMPARE(cdata.mid(4).trimmed().toInt(), sdata.mid(4).trimmed().toInt() * 2);
        QVERIFY(serverData.at(i * 3 + 2).startsWith(QLatin1String("sending ")));
        QCOMPARE(serverData.at(i * 3 + 2).trimmed().mid(8).toInt(),
                 sdata.mid(4).trimmed().toInt() * 2);
    }

    clientProcess.kill();
    QVERIFY(clientProcess.waitForFinished());
    serverProcess.kill();
    QVERIFY(serverProcess.waitForFinished());
#endif
}

void tst_QUdpSocket::zeroLengthDatagram()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    QUdpSocket receiver;
    QVERIFY(receiver.bind());

    QVERIFY(!receiver.waitForReadyRead(100));
    QVERIFY(!receiver.hasPendingDatagrams());

    QUdpSocket sender;
    QCOMPARE(sender.writeDatagram(QByteArray(), QHostAddress::LocalHost, receiver.localPort()), qint64(0));

    QVERIFY(receiver.waitForReadyRead(1000));
    QVERIFY(receiver.hasPendingDatagrams());

    char buf;
    QCOMPARE(receiver.readDatagram(&buf, 1), qint64(0));
}

QTEST_MAIN(tst_QUdpSocket)
#include "tst_qudpsocket.moc"
