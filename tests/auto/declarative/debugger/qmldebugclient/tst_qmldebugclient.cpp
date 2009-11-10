/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtDeclarative/qmlengine.h>

#include <private/qmldebug_p.h>
#include <private/qmlenginedebug_p.h>
#include <private/qmldebugclient_p.h>
#include <private/qmldebugservice_p.h>

#include "../debuggerutil_p.h"

class tst_QmlDebugClient : public QObject
{
    Q_OBJECT

public:
    tst_QmlDebugClient(QmlDebugConnection *conn, QmlEngine *engine)
        : m_conn(conn), m_engine(engine) {}

    QmlDebugConnection *m_conn;
    QmlEngine *m_engine;

private slots:
    void name();
    void isEnabled();
    void setEnabled();
    void isConnected();
    void sendMessage();
};

void tst_QmlDebugClient::name()
{
    QString name = "tst_QmlDebugClient::name()";

    QmlDebugClient client(name, m_conn);
    QCOMPARE(client.name(), name);
}

void tst_QmlDebugClient::isEnabled()
{
    QmlDebugClient client("tst_QmlDebugClient::isEnabled()", m_conn);
    QCOMPARE(client.isEnabled(), false);
}

void tst_QmlDebugClient::setEnabled()
{
    EchoService service("tst_QmlDebugClient::setEnabled()");
    MyQmlDebugClient client("tst_QmlDebugClient::setEnabled()", m_conn);

    QCOMPARE(service.isEnabled(), false);

    client.setEnabled(true);
    QCOMPARE(client.isEnabled(), true);
    QmlDebuggerTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), true);

    client.setEnabled(false);
    QCOMPARE(client.isEnabled(), false);
    QmlDebuggerTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), false);
}

void tst_QmlDebugClient::isConnected()
{
    QmlDebugClient client1("tst_QmlDebugClient::isConnected() A", m_conn);
    QCOMPARE(client1.isConnected(), true);

    QmlDebugConnection conn;
    QmlDebugClient client2("tst_QmlDebugClient::isConnected() B", &conn);
    QCOMPARE(client2.isConnected(), false);

    QmlDebugClient client3("tst_QmlDebugClient::isConnected() C", 0);
    QCOMPARE(client3.isConnected(), false);

    // duplicate plugin name
    QTest::ignoreMessage(QtWarningMsg, "QmlDebugClient: Conflicting plugin name \"tst_QmlDebugClient::isConnected() A\" ");
    QmlDebugClient client4("tst_QmlDebugClient::isConnected() A", m_conn);
    QCOMPARE(client4.isConnected(), false);
}

void tst_QmlDebugClient::sendMessage()
{
    EchoService service("tst_QmlDebugClient::sendMessage()");
    MyQmlDebugClient client("tst_QmlDebugClient::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
}



class tst_QmlDebugClient_Thread : public QThread
{
    Q_OBJECT
public:
    void run() {
        QTest::qWait(1000);
        connectToEngine();
    }

    QPointer<QmlEngine> m_engine;

signals:
    void testsFinished();

public slots:

    void connectToEngine()
    {
        QmlDebugConnection conn;
        conn.connectToHost("127.0.0.1", 3768);
        bool ok = conn.waitForConnected(5000);
        Q_ASSERT(ok);
        while (!m_engine)
            QTest::qWait(50);

        tst_QmlDebugClient test(&conn, m_engine);
        QTest::qExec(&test); 
        emit testsFinished();
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qputenv("QML_DEBUG_SERVER_PORT", "3768");

    tst_QmlDebugClient_Thread thread;
    QObject::connect(&thread, SIGNAL(testsFinished()), qApp, SLOT(quit()));
    thread.start();

    QmlEngine engine;  // blocks until client connects

    // start the test
    thread.m_engine = &engine;

    return app.exec();

}

#include "tst_qmldebugclient.moc"
