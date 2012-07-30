/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QSignalSpy>
#include <QTimer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QBuffer>

#include <private/qpacketprotocol_p.h>

#include "../shared/debugutil_p.h"

class tst_QPacketProtocol : public QObject
{
    Q_OBJECT

private:
    QTcpServer *m_server;
    QTcpSocket *m_client;
    QTcpSocket *m_serverConn;

private slots:
    void init();
    void cleanup();

    void maximumPacketSize();
    void setMaximumPacketSize();
    void setMaximumPacketSize_data();
    void send();
    void send_data();
    void packetsAvailable();
    void packetsAvailable_data();
    void clear();
    void read();
    void device();

    void tst_QPacket_clear();
};

void tst_QPacketProtocol::init()
{
    m_server = new QTcpServer(this);
    m_serverConn = 0;
    QVERIFY(m_server->listen(QHostAddress("127.0.0.1")));
	
    m_client = new QTcpSocket(this);
    m_client->connectToHost(m_server->serverAddress(), m_server->serverPort());
	
    QVERIFY(m_client->waitForConnected());
    QVERIFY(m_server->waitForNewConnection());
    m_serverConn = m_server->nextPendingConnection();
}

void tst_QPacketProtocol::cleanup()
{
    delete m_client;
    delete m_serverConn;
    delete m_server;
}

void tst_QPacketProtocol::maximumPacketSize()
{
    QPacketProtocol p(m_client);
    QCOMPARE(p.maximumPacketSize(), 0x7FFFFFFF);
}

void tst_QPacketProtocol::setMaximumPacketSize()
{
    QFETCH(qint32, size);
    QFETCH(qint32, expected);

    QPacketProtocol out(m_serverConn);
    QCOMPARE(out.setMaximumPacketSize(size), expected);
}

void tst_QPacketProtocol::setMaximumPacketSize_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("expected");

    QTest::newRow("invalid") << qint32(sizeof(qint32) - 1) << qint32(0x7FFFFFFF);
    QTest::newRow("still invalid") << qint32(sizeof(qint32)) << qint32(0x7FFFFFFF);
    QTest::newRow("valid") << qint32(sizeof(qint32) + 1) << qint32(sizeof(qint32) + 1);
}

void tst_QPacketProtocol::send()
{
    QFETCH(bool, useAutoSend);

    QPacketProtocol in(m_client);
    QPacketProtocol out(m_serverConn);

    QByteArray ba;
    int num;

    if (useAutoSend) {
        out.send() << "Hello world" << 123;
    } else {
        QPacket packet;
        packet << "Hello world" << 123;
        out.send(packet);
    }

    QVERIFY(QDeclarativeDebugTest::waitForSignal(&in, SIGNAL(readyRead())));

    QPacket p = in.read();
    p >> ba >> num;
    QCOMPARE(ba, QByteArray("Hello world") + '\0');
    QCOMPARE(num, 123);
}

void tst_QPacketProtocol::send_data()
{
    QTest::addColumn<bool>("useAutoSend");

    QTest::newRow("auto send") << true;
    QTest::newRow("no auto send") << false;
}

void tst_QPacketProtocol::packetsAvailable()
{
    QFETCH(int, packetCount);

    QPacketProtocol out(m_client);
    QPacketProtocol in(m_serverConn);

    QCOMPARE(out.packetsAvailable(), qint64(0));
    QCOMPARE(in.packetsAvailable(), qint64(0));

    for (int i=0; i<packetCount; i++)
        out.send() << "Hello";

    QVERIFY(QDeclarativeDebugTest::waitForSignal(&in, SIGNAL(readyRead())));
    QCOMPARE(in.packetsAvailable(), qint64(packetCount));
}

void tst_QPacketProtocol::packetsAvailable_data()
{
    QTest::addColumn<int>("packetCount");

    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("10") << 10;
}

void tst_QPacketProtocol::clear()
{
    QPacketProtocol in(m_client);
    QPacketProtocol out(m_serverConn);

    out.send() << 123;
    out.send() << 456;
    QVERIFY(QDeclarativeDebugTest::waitForSignal(&in, SIGNAL(readyRead())));

    in.clear();
    QVERIFY(in.read().isEmpty());
}

void tst_QPacketProtocol::read()
{
    QPacketProtocol in(m_client);
    QPacketProtocol out(m_serverConn);

    QVERIFY(in.read().isEmpty());

    out.send() << 123;
    out.send() << 456;
    QVERIFY(QDeclarativeDebugTest::waitForSignal(&in, SIGNAL(readyRead())));

    int num;

    QPacket p1 = in.read();
    QVERIFY(!p1.isEmpty());
    p1 >> num;
    QCOMPARE(num, 123);

    QPacket p2 = in.read();
    QVERIFY(!p2.isEmpty());
    p2 >> num;
    QCOMPARE(num, 456);

    QVERIFY(in.read().isEmpty());
}

void tst_QPacketProtocol::device()
{
    QPacketProtocol p(m_client);
    QVERIFY(p.device() == m_client);
}

void tst_QPacketProtocol::tst_QPacket_clear()
{
    QPacketProtocol protocol(m_client);

    QPacket packet;

    packet << "Hello world!" << 123;
    protocol.send(packet);

    packet.clear();
    QVERIFY(packet.isEmpty());
    packet << "Goodbyte world!" << 789;
    protocol.send(packet);

    QByteArray ba;
    int num;
    QPacketProtocol in(m_serverConn);
    QVERIFY(QDeclarativeDebugTest::waitForSignal(&in, SIGNAL(readyRead())));

    QPacket p1 = in.read();
    p1 >> ba >> num;
    QCOMPARE(ba, QByteArray("Hello world!") + '\0');
    QCOMPARE(num, 123);

    QPacket p2 = in.read();
    p2 >> ba >> num;
    QCOMPARE(ba, QByteArray("Goodbyte world!") + '\0');
    QCOMPARE(num, 789);
}

QTEST_MAIN(tst_QPacketProtocol)

#include "tst_qpacketprotocol.moc"
