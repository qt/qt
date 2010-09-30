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
    void status();
    void sendMessage();
};

void tst_QDeclarativeDebugClient::initTestCase()
{
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Waiting for connection on port 3770...");

    new QDeclarativeEngine(this);

    m_conn = new QDeclarativeDebugConnection(this);

    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugClient::handshake()", m_conn);
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugClient::handshake()");

    m_conn->connectToHost("127.0.0.1", 3770);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Connection established");
    bool ok = m_conn->waitForConnected();
    Q_ASSERT(ok);

    QTRY_VERIFY(QDeclarativeDebugService::hasDebuggingClient());
    QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Enabled);
}

void tst_QDeclarativeDebugClient::name()
{
    QString name = "tst_QDeclarativeDebugClient::name()";

    QDeclarativeDebugClient client(name, m_conn);
    QCOMPARE(client.name(), name);
}

void tst_QDeclarativeDebugClient::status()
{
    {
        QDeclarativeDebugConnection dummyConn;
        QDeclarativeDebugClient client("tst_QDeclarativeDebugClient::status()", &dummyConn);
        QCOMPARE(client.status(), QDeclarativeDebugClient::NotConnected);
    }

    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugClient::status()", m_conn);
    QCOMPARE(client.status(), QDeclarativeDebugClient::Unavailable);

    {
        QDeclarativeDebugTestService service("tst_QDeclarativeDebugClient::status()");
        QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Enabled);
    }

    QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Unavailable);

    // duplicate plugin name
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugClient: Conflicting plugin name \"tst_QDeclarativeDebugClient::status()\" ");
    QDeclarativeDebugClient client2("tst_QDeclarativeDebugClient::status()", m_conn);
    QCOMPARE(client2.status(), QDeclarativeDebugClient::NotConnected);

    QDeclarativeDebugClient client3("tst_QDeclarativeDebugClient::status3()", 0);
    QCOMPARE(client3.status(), QDeclarativeDebugClient::NotConnected);
}

void tst_QDeclarativeDebugClient::sendMessage()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugClient::sendMessage()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugClient::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Enabled);

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
}

int main(int argc, char *argv[])
{
    int _argc = argc + 1;
    char **_argv = new char*[_argc];
    for (int i = 0; i < argc; ++i)
        _argv[i] = argv[i];
    _argv[_argc - 1] = "-qmljsdebugger=port:3770";

    QApplication app(_argc, _argv);
    tst_QDeclarativeDebugClient tc;
    return QTest::qExec(&tc, _argc, _argv);
    delete _argv;
}

#include "tst_qdeclarativedebugclient.moc"

