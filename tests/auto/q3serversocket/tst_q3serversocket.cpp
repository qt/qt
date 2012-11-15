/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

