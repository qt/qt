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

#include <qapplication.h>

#include <q3serversocket.h>
#include <q3socket.h>

//TESTED_CLASS=
//TESTED_FILES=

class TestServer : public Q3ServerSocket
{
    Q_OBJECT
public:

    TestServer( int port );
    ~TestServer();

    void newConnection( int socket );

signals:
    void acceptedClient( int socket );
};

class tst_Q3ServerSocket : public QObject
{
    Q_OBJECT

public:
    tst_Q3ServerSocket();
    virtual ~tst_Q3ServerSocket();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void accept();

protected slots:
    void acceptClient( int socket );

private:
    TestServer *serverSocket;
    Q3Socket *socket;
};

tst_Q3ServerSocket::tst_Q3ServerSocket()
{
}

tst_Q3ServerSocket::~tst_Q3ServerSocket()
{
}

void tst_Q3ServerSocket::initTestCase()
{
}

void tst_Q3ServerSocket::cleanupTestCase()
{
}

void tst_Q3ServerSocket::init()
{
}

void tst_Q3ServerSocket::cleanup()
{
}

void tst_Q3ServerSocket::accept()
{
    // init
    serverSocket = new TestServer( 12345 );
    connect( serverSocket, SIGNAL( acceptedClient( int ) ), SLOT( acceptClient( int ) ) );

    QVERIFY( serverSocket->port() == 12345 );

    socket = new Q3Socket;
    socket->connectToHost( "localhost", 12345 );

    QTestEventLoop::instance().enterLoop( 30 );

    delete serverSocket;
    delete socket;
}

void tst_Q3ServerSocket::acceptClient( int )
{
    QTestEventLoop::instance().exitLoop();
}

TestServer::TestServer( int port ) : Q3ServerSocket( port )
{
}

TestServer::~TestServer()
{
}

void TestServer::newConnection( int socket )
{
    emit acceptedClient( socket );
}

QTEST_MAIN(tst_Q3ServerSocket)
#include "tst_q3serversocket.moc"

