/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include <qapplication.h>




#include <q3socket.h>
#include "../network-settings.h"
//TESTED_CLASS=
//TESTED_FILES=network/q3socket.h network/q3socket.cpp

class tst_Q3Socket : public QObject
{
    Q_OBJECT

public:
    tst_Q3Socket();
    virtual ~tst_Q3Socket();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void peerAddress_data();
    void peerAddress();
    void emitConnectionRefused();
    void readBufferSize();
    void connectionAttempts_data();
    void connectionAttempts();
    void canReadLine();

protected slots:
    void peerAddress_connected();
    void peerAddress_connectionClosed();
    void emitConnectionRefused_error(int);
    void connectionAttempts_connected();
    void connectionAttempts_error(int);

private:
    Q3Socket *socket;

    QHostAddress *peerAddress_addrConnected;
    uint peerAddress_portConnected;
    QHostAddress *peerAddress_addrClosed;
    uint peerAddress_portClosed;
    bool emitConnectionRefused_errorReceived;
};

tst_Q3Socket::tst_Q3Socket()
{
}

tst_Q3Socket::~tst_Q3Socket()
{
}

void tst_Q3Socket::initTestCase()
{
}

void tst_Q3Socket::cleanupTestCase()
{
}

void tst_Q3Socket::init()
{
}

void tst_Q3Socket::cleanup()
{
}

void tst_Q3Socket::peerAddress_data()
{
    QTest::addColumn<QString>("host");
    QTest::addColumn<uint>("port");
    QTest::addColumn<bool>("peerClosesConnection");
    QTest::addColumn<QString>("peerAddr");

    QTest::newRow( "echo" )    << QtNetworkSettings::serverLocalName() << (uint)7  << false
        << QtNetworkSettings::serverIP().toString();
    QTest::newRow( "daytime" ) << QtNetworkSettings::serverLocalName() << (uint)13 << true
        << QtNetworkSettings::serverIP().toString();
}

void tst_Q3Socket::peerAddress()
{
    // init
    QFETCH( QString, host );
    QFETCH( uint, port );
    QFETCH( QString, peerAddr );
    QFETCH( bool, peerClosesConnection );

    socket = new Q3Socket;
    connect( socket, SIGNAL(connected()), SLOT(peerAddress_connected()) );
    connect( socket, SIGNAL(connectionClosed()), SLOT(peerAddress_connectionClosed()) );

    peerAddress_addrConnected = 0;
    peerAddress_addrClosed = 0;

    // connect to host
    socket->connectToHost( host, port );
    QTestEventLoop::instance().enterLoop( 30 );
    if ( QTestEventLoop::instance().timeout() && peerAddress_addrConnected==0 )
	QFAIL( "Connection timed out" );

    // test
    QHostAddress pa;
    QVERIFY( pa.setAddress(peerAddr) );

    QTEST( peerAddress_addrConnected->toString(), "peerAddr" ); // results in nicer output than the test below
    QVERIFY( *peerAddress_addrConnected == pa );
    QCOMPARE( peerAddress_portConnected, port );

    if ( peerClosesConnection ) {
	QVERIFY( peerAddress_addrClosed != 0 );

	QCOMPARE( peerAddress_addrClosed->toString(), QString() ); // results in nicer output than the test below
	QVERIFY( *peerAddress_addrClosed == QHostAddress::Null );
	QCOMPARE( peerAddress_portClosed, (uint)0 );
    } else {
	QVERIFY( peerAddress_addrClosed == 0 );
    }

    // cleanup
    delete peerAddress_addrConnected;
    delete peerAddress_addrClosed;
    delete socket;
}

void tst_Q3Socket::peerAddress_connected()
{
    peerAddress_addrConnected = new QHostAddress( socket->peerAddress() );
    peerAddress_portConnected = socket->peerPort();
    QTestEventLoop::instance().changeInterval( 5 ); // enough time to get the closed
}

void tst_Q3Socket::peerAddress_connectionClosed()
{
    peerAddress_addrClosed = new QHostAddress( socket->peerAddress() );
    peerAddress_portClosed = socket->peerPort();
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Socket::emitConnectionRefused()
{
    Q3Socket sock;
    connect( &sock, SIGNAL(error(int)), SLOT(emitConnectionRefused_error(int)) );
    sock.connectToHost( QtNetworkSettings::serverLocalName(), 12331 );

    emitConnectionRefused_errorReceived = false;
    QTestEventLoop::instance().enterLoop( 30 );
    QVERIFY(emitConnectionRefused_errorReceived);
}

void tst_Q3Socket::emitConnectionRefused_error( int signum )
{
    if ( signum == Q3Socket::ErrConnectionRefused )
        emitConnectionRefused_errorReceived = true;

    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Socket::readBufferSize()
{
    const int bufferSize = 1024;

    Q3Socket sock;
    sock.setReadBufferSize(bufferSize);
    QCOMPARE((int)sock.readBufferSize(), bufferSize);

    sock.connectToHost("localhost", 0);

    QCOMPARE((int)sock.readBufferSize(), bufferSize);
}

void tst_Q3Socket::connectionAttempts_data()
{
    QTest::addColumn<QString>("host");
    QTest::addColumn<int>("port");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("fluke port 80") << QtNetworkSettings::serverLocalName() << 80 << true;
    QTest::newRow("fluke port 79") << QtNetworkSettings::serverLocalName() << 79 << false;
}

void tst_Q3Socket::connectionAttempts()
{
    QFETCH(QString, host);
    QFETCH(int, port);
    QFETCH(bool, expectedResult);

    Q3Socket sock;
    sock.connectToHost(host, port);
    connect(&sock, SIGNAL(connected()), SLOT(connectionAttempts_connected()));
    connect(&sock, SIGNAL(error(int)), SLOT(connectionAttempts_error(int)));
    QTestEventLoop::instance().enterLoop(10);

    if (QTestEventLoop::instance().timeout())
       return;

    QCOMPARE(sock.state() == Q3Socket::Connected, expectedResult);
}

void tst_Q3Socket::connectionAttempts_connected()
{
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Socket::connectionAttempts_error(int)
{
    QTestEventLoop::instance().exitLoop();
}

void tst_Q3Socket::canReadLine()
{
    QEventLoop loop;

    Q3Socket socket;
    QVERIFY(!socket.canReadLine());
    connect(&socket, SIGNAL(connected()), &loop, SLOT(quit()));
    // timeout error will fail in #238
    connect(&socket, SIGNAL(error(int)), &loop, SLOT(quit()));
    socket.connectToHost(QtNetworkSettings::serverLocalName(), 143);

    loop.exec();

    QCOMPARE(socket.state(), Q3Socket::Connected);

    while (!socket.canReadLine())
        QVERIFY(socket.waitForMore(5000) > 0);

    QVERIFY(socket.canReadLine());
    socket.readLine();
    QVERIFY(!socket.canReadLine());
    socket.ungetChar('\n');
    QVERIFY(socket.canReadLine());
}

QTEST_MAIN(tst_Q3Socket)
#include "tst_qsocket.moc"
