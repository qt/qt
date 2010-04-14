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
#include <QtCore/QThread>
#include <QtCore/QSemaphore>
#include <QtCore/QElapsedTimer>
#include <QtNetwork/QTcpSocket>

#include "minihttpserver.h"
#include "../network-settings.h"

class tst_QTcpSocket_stresstest : public QObject
{
    Q_OBJECT
public:
    tst_QTcpSocket_stresstest();
    MiniHttpServer server;

public slots:
    void initTestCase_data();

private Q_SLOTS:
    void blockingConnectDisconnect();
};

tst_QTcpSocket_stresstest::tst_QTcpSocket_stresstest()
{
}

void tst_QTcpSocket_stresstest::initTestCase_data()
{
    QTest::addColumn<bool>("isLocalhost");
    QTest::addColumn<QString>("hostname");
    QTest::addColumn<int>("port");

    QTest::newRow("localhost") << false << "localhost" << server.port();
    QTest::newRow("remote") << true << QtNetworkSettings::serverName() << 80;
}

void tst_QTcpSocket_stresstest::blockingConnectDisconnect()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    for (int i = 0; i < 50; ++i) {
        qDebug("Attempt %d", i);
        QTcpSocket socket;
        socket.connectToHost(hostname, port);
        QVERIFY2(socket.waitForConnected(), "Timeout");

        socket.write("GET /qtest/bigfile HTTP/1.1\r\n"
                     "Connection: close\r\n"
                     "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                     "Host: " + hostname.toLatin1() + "\r\n"
                     "\r\n");
        while (socket.bytesToWrite())
            QVERIFY2(socket.waitForBytesWritten(), "Timeout");

        QElapsedTimer timeout;
        timeout.start();
        while (socket.state() == QAbstractSocket::ConnectedState && !timeout.hasExpired(10000)) {
            socket.waitForReadyRead();
            socket.readAll(); // discard
        }
    }
}

QTEST_MAIN(tst_QTcpSocket_stresstest);

#include "tst_qtcpsocket_stresstest.moc"
