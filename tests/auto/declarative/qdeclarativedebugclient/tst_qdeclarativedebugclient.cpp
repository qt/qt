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
#include <qtest.h>
#include <QSignalSpy>
#include <QTimer>
#include <QHostAddress>
#include <QDebug>
#include <QThread>

#include <QtDeclarative/qdeclarativeengine.h>

#include <private/qdeclarativedebug_p.h>
#include <private/qdeclarativeenginedebug_p.h>
#include <private/qdeclarativedebugclient_p.h>
#include <private/qdeclarativedebugservice_p.h>

#include "../../../shared/util.h"
#include "../shared/debugutil_p.h"

class tst_QDeclarativeDebugClient : public QObject
{
    Q_OBJECT

private:
    QDeclarativeDebugConnection *m_conn;

private slots:
    void initTestCase();

    void name();
    void isEnabled();
    void setEnabled();
    void isConnected();
    void sendMessage();
};

void tst_QDeclarativeDebugClient::initTestCase()
{
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Waiting for connection on port 3770...");

    qputenv("QML_DEBUG_SERVER_PORT", "3770");
    new QDeclarativeEngine(this);

    m_conn = new QDeclarativeDebugConnection(this);
    m_conn->connectToHost("127.0.0.1", 3770);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Connection established");
    bool ok = m_conn->waitForConnected();
    Q_ASSERT(ok);

    QTRY_VERIFY(QDeclarativeDebugService::hasDebuggingClient());
}

void tst_QDeclarativeDebugClient::name()
{
    QString name = "tst_QDeclarativeDebugClient::name()";

    QDeclarativeDebugClient client(name, m_conn);
    QCOMPARE(client.name(), name);
}

void tst_QDeclarativeDebugClient::isEnabled()
{
    QDeclarativeDebugClient client("tst_QDeclarativeDebugClient::isEnabled()", m_conn);
    QCOMPARE(client.isEnabled(), false);
}

void tst_QDeclarativeDebugClient::setEnabled()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugClient::setEnabled()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugClient::setEnabled()", m_conn);

    QCOMPARE(service.isEnabled(), false);

    client.setEnabled(true);
    QCOMPARE(client.isEnabled(), true);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), true);

    client.setEnabled(false);
    QCOMPARE(client.isEnabled(), false);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), false);
}

void tst_QDeclarativeDebugClient::isConnected()
{
    QDeclarativeDebugClient client1("tst_QDeclarativeDebugClient::isConnected() A", m_conn);
    QCOMPARE(client1.isConnected(), true);

    QDeclarativeDebugConnection conn;
    QDeclarativeDebugClient client2("tst_QDeclarativeDebugClient::isConnected() B", &conn);
    QCOMPARE(client2.isConnected(), false);

    QDeclarativeDebugClient client3("tst_QDeclarativeDebugClient::isConnected() C", 0);
    QCOMPARE(client3.isConnected(), false);

    // duplicate plugin name
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugClient: Conflicting plugin name \"tst_QDeclarativeDebugClient::isConnected() A\" ");
    QDeclarativeDebugClient client4("tst_QDeclarativeDebugClient::isConnected() A", m_conn);
    QCOMPARE(client4.isConnected(), false);
}

void tst_QDeclarativeDebugClient::sendMessage()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugClient::sendMessage()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugClient::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
}

QTEST_MAIN(tst_QDeclarativeDebugClient)

#include "tst_qdeclarativedebugclient.moc"

