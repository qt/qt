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

// Just to get Q_OS_SYMBIAN
#include <qglobal.h>

#include <QtTest/QtTest>


#include <qcoreapplication.h>
#include <qtcpsocket.h>
#include <qtcpserver.h>
#include <qhostaddress.h>
#include <qprocess.h>
#include <qstringlist.h>
#include <qplatformdefs.h>
#include <qhostinfo.h>

#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

#include <QNetworkProxy>
Q_DECLARE_METATYPE(QNetworkProxy)
Q_DECLARE_METATYPE(QList<QNetworkProxy>)

#include "../../../../auto/network-settings.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QTcpServer : public QObject
{
    Q_OBJECT

public:
    tst_QTcpServer();
    virtual ~tst_QTcpServer();


public slots:
    void initTestCase_data();
    void init();
    void initTestCase();
    void cleanup();
private slots:
    void ipv4LoopbackPerformanceTest();
    void ipv6LoopbackPerformanceTest();
    void ipv4PerformanceTest();
private:
#ifndef QT_NO_BEARERMANAGEMENT
    QNetworkConfigurationManager *netConfMan;
    QNetworkConfiguration networkConfiguration;
    QSharedPointer<QNetworkSession> networkSession;
#endif
};

tst_QTcpServer::tst_QTcpServer()
{
}

void tst_QTcpServer::initTestCase()
{
#ifndef QT_NO_BEARERMANAGEMENT
    netConfMan = new QNetworkConfigurationManager(this);
    netConfMan->updateConfigurations();
    connect(netConfMan, SIGNAL(updateCompleted()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QTestEventLoop::instance().enterLoop(10);
    networkConfiguration = netConfMan->defaultConfiguration();
    if (networkConfiguration.isValid()) {
        networkSession = QSharedPointer<QNetworkSession>(new QNetworkSession(networkConfiguration));
        if (!networkSession->isOpen()) {
            networkSession->open();
            QVERIFY(networkSession->waitForOpened(30000));
        }
    } else {
        QVERIFY(!(netConfMan->capabilities() & QNetworkConfigurationManager::NetworkSessionRequired));
    }
#endif
}

tst_QTcpServer::~tst_QTcpServer()
{
}

void tst_QTcpServer::initTestCase_data()
{
    QTest::addColumn<bool>("setProxy");
    QTest::addColumn<int>("proxyType");

    QTest::newRow("WithoutProxy") << false << 0;
    QTest::newRow("WithSocks5Proxy") << true << int(QNetworkProxy::Socks5Proxy);
    QTest::newRow("WithHttpProxy") << true << int(QNetworkProxy::HttpProxy);
}

void tst_QTcpServer::init()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy) {
        QFETCH_GLOBAL(int, proxyType);
        if (proxyType == QNetworkProxy::Socks5Proxy) {
            QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::Socks5Proxy, QtNetworkSettings::serverName(), 1080));
        }
    }
}

void tst_QTcpServer::cleanup()
{
    QNetworkProxy::setApplicationProxy(QNetworkProxy::DefaultProxy);
}

//----------------------------------------------------------------------------------
void tst_QTcpServer::ipv4LoopbackPerformanceTest()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;

    QTcpServer server;
    QVERIFY(server.listen(QHostAddress::LocalHost));

    QVERIFY(server.isListening());

    QTcpSocket clientA;
    clientA.connectToHost(QHostAddress::LocalHost, server.serverPort());
    QVERIFY(clientA.waitForConnected(5000));
    QVERIFY(clientA.state() == QAbstractSocket::ConnectedState);

    QVERIFY(server.waitForNewConnection());
    QTcpSocket *clientB = server.nextPendingConnection();
    QVERIFY(clientB);

    QByteArray buffer(16384, '@');
    QTime stopWatch;
    stopWatch.start();
    qlonglong totalWritten = 0;
    while (stopWatch.elapsed() < 5000) {
        QVERIFY(clientA.write(buffer.data(), buffer.size()) > 0);
        clientA.flush();
        totalWritten += buffer.size();
        while (clientB->waitForReadyRead(100)) {
            if (clientB->bytesAvailable() == 16384)
                break;
        }
        clientB->read(buffer.data(), buffer.size());
        clientB->write(buffer.data(), buffer.size());
        clientB->flush();
        totalWritten += buffer.size();
        while (clientA.waitForReadyRead(100)) {
            if (clientA.bytesAvailable() == 16384)
                break;
        }
        clientA.read(buffer.data(), buffer.size());
    }

    qDebug("\t\t%s: %.1fMB/%.1fs: %.1fMB/s",
           server.serverAddress().toString().toLatin1().constData(),
           totalWritten / (1024.0 * 1024.0),
           stopWatch.elapsed() / 1000.0,
           (totalWritten / (stopWatch.elapsed() / 1000.0)) / (1024 * 1024));

    delete clientB;
}

//----------------------------------------------------------------------------------
void tst_QTcpServer::ipv6LoopbackPerformanceTest()
{
    QFETCH_GLOBAL(bool, setProxy);
    if (setProxy)
        return;
#if defined(Q_WS_WINCE_WM)
    QSKIP("WinCE WM: Not yet supported", SkipAll);
#endif

    QTcpServer server;
    if (!server.listen(QHostAddress::LocalHostIPv6, 0)) {
        QVERIFY(server.serverError() == QAbstractSocket::UnsupportedSocketOperationError);
    } else {
        QTcpSocket clientA;
        clientA.connectToHost(server.serverAddress(), server.serverPort());
        QVERIFY(clientA.waitForConnected(5000));

        QVERIFY(server.waitForNewConnection(5000));
        QTcpSocket *clientB = server.nextPendingConnection();
        QVERIFY(clientB);

        QByteArray buffer(16384, '@');
        QTime stopWatch;
        stopWatch.start();
        qlonglong totalWritten = 0;
        while (stopWatch.elapsed() < 5000) {
            clientA.write(buffer.data(), buffer.size());
            clientA.flush();
            totalWritten += buffer.size();
            while (clientB->waitForReadyRead(100)) {
                if (clientB->bytesAvailable() == 16384)
                    break;
            }
            clientB->read(buffer.data(), buffer.size());
            clientB->write(buffer.data(), buffer.size());
            clientB->flush();
            totalWritten += buffer.size();
            while (clientA.waitForReadyRead(100)) {
                if (clientA.bytesAvailable() == 16384)
                   break;
            }
            clientA.read(buffer.data(), buffer.size());
        }

        qDebug("\t\t%s: %.1fMB/%.1fs: %.1fMB/s",
               server.serverAddress().toString().toLatin1().constData(),
               totalWritten / (1024.0 * 1024.0),
               stopWatch.elapsed() / 1000.0,
               (totalWritten / (stopWatch.elapsed() / 1000.0)) / (1024 * 1024));
        delete clientB;
    }
}

//----------------------------------------------------------------------------------
void tst_QTcpServer::ipv4PerformanceTest()
{
    QTcpSocket probeSocket;
    probeSocket.connectToHost(QtNetworkSettings::serverName(), 143);
    QVERIFY(probeSocket.waitForConnected(5000));

    QTcpServer server;
    QVERIFY(server.listen(probeSocket.localAddress(), 0));

    QFETCH_GLOBAL(int, proxyType);
    //For http proxy, only the active connection can be proxied and not the server socket
    if (proxyType == QNetworkProxy::HttpProxy)
        QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::HttpProxy, QtNetworkSettings::serverName(), 3128));

    QTcpSocket clientA;
    clientA.connectToHost(server.serverAddress(), server.serverPort());
    QVERIFY(clientA.waitForConnected(5000));

    QVERIFY(server.waitForNewConnection(5000));
    QTcpSocket *clientB = server.nextPendingConnection();
    QVERIFY(clientB);

    QByteArray buffer(16384, '@');
    QTime stopWatch;
    stopWatch.start();
    qlonglong totalWritten = 0;
    while (stopWatch.elapsed() < 5000) {
        qlonglong writtenA = clientA.write(buffer.data(), buffer.size());
        clientA.flush();
        totalWritten += buffer.size();
        while (clientB->waitForReadyRead(100)) {
            if (clientB->bytesAvailable() == writtenA)
                break;
        }
        clientB->read(buffer.data(), buffer.size());
        qlonglong writtenB = clientB->write(buffer.data(), buffer.size());
        clientB->flush();
        totalWritten += buffer.size();
        while (clientA.waitForReadyRead(100)) {
            if (clientA.bytesAvailable() == writtenB)
               break;
        }
        clientA.read(buffer.data(), buffer.size());
    }

    qDebug("\t\t%s: %.1fMB/%.1fs: %.1fMB/s",
           probeSocket.localAddress().toString().toLatin1().constData(),
           totalWritten / (1024.0 * 1024.0),
           stopWatch.elapsed() / 1000.0,
           (totalWritten / (stopWatch.elapsed() / 1000.0)) / (1024 * 1024));

    delete clientB;
}

QTEST_MAIN(tst_QTcpServer)
#include "tst_qtcpserver.moc"
