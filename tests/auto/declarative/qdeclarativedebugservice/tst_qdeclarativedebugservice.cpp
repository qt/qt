/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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

#include <private/qdeclarativedebugclient_p.h>
#include <private/qdeclarativedebugservice_p.h>

#include "../../../shared/util.h"
#include "../shared/debugutil_p.h"


class tst_QDeclarativeDebugService : public QObject
{
    Q_OBJECT
private:
    QDeclarativeDebugConnection *m_conn;

private slots:
    void initTestCase();

    void name();
    void status();
    void sendMessage();
    void idForObject();
    void objectForId();
    void objectToString();
};

void tst_QDeclarativeDebugService::initTestCase()
{
    QTest::ignoreMessage(QtDebugMsg, "QDeclarativeDebugServer: Waiting for connection on port 13769...");
    new QDeclarativeEngine(this);

    m_conn = new QDeclarativeDebugConnection(this);
    m_conn->connectToHost("127.0.0.1", 13769);

    QTest::ignoreMessage(QtDebugMsg, "QDeclarativeDebugServer: Connection established");
    bool ok = m_conn->waitForConnected();
    QVERIFY(ok);

    QTRY_VERIFY(QDeclarativeDebugService::hasDebuggingClient());
}

void tst_QDeclarativeDebugService::name()
{
    QString name = "tst_QDeclarativeDebugService::name()";

    QDeclarativeDebugService service(name);
    QCOMPARE(service.name(), name);
}

void tst_QDeclarativeDebugService::status()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugService::status()");
    QCOMPARE(service.status(), QDeclarativeDebugService::Unavailable);

    {
        QDeclarativeDebugTestClient client("tst_QDeclarativeDebugService::status()", m_conn);
        QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Enabled);
        QTRY_COMPARE(service.status(), QDeclarativeDebugService::Enabled);
    }


    QTRY_COMPARE(service.status(), QDeclarativeDebugService::Unavailable);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugService: Conflicting plugin name \"tst_QDeclarativeDebugService::status()\" ");

    QDeclarativeDebugService duplicate("tst_QDeclarativeDebugService::status()");
    QCOMPARE(duplicate.status(), QDeclarativeDebugService::NotConnected);
}

void tst_QDeclarativeDebugService::sendMessage()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugService::sendMessage()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugService::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    QTRY_COMPARE(client.status(), QDeclarativeDebugClient::Enabled);
    QTRY_COMPARE(service.status(), QDeclarativeDebugService::Enabled);

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugService: Conflicting plugin name \"tst_QDeclarativeDebugService::sendMessage()\" ");
    QDeclarativeDebugService duplicate("tst_QDeclarativeDebugService::sendMessage()");
    duplicate.sendMessage("msg");
}

void tst_QDeclarativeDebugService::idForObject()
{
    QCOMPARE(QDeclarativeDebugService::idForObject(0), -1);

    QObject *objA = new QObject;

    int idA = QDeclarativeDebugService::idForObject(objA);
    QVERIFY(idA >= 0);
    QCOMPARE(QDeclarativeDebugService::objectForId(idA), objA);

    int idAA = QDeclarativeDebugService::idForObject(objA);
    QCOMPARE(idAA, idA);

    QObject *objB = new QObject;
    int idB = QDeclarativeDebugService::idForObject(objB);
    QVERIFY(idB != idA);
    QCOMPARE(QDeclarativeDebugService::objectForId(idB), objB);

    delete objA;
    delete objB;
}

void tst_QDeclarativeDebugService::objectForId()
{
    QCOMPARE(QDeclarativeDebugService::objectForId(-1), static_cast<QObject*>(0));
    QCOMPARE(QDeclarativeDebugService::objectForId(1), static_cast<QObject*>(0));

    QObject *obj = new QObject;
    int id = QDeclarativeDebugService::idForObject(obj);
    QCOMPARE(QDeclarativeDebugService::objectForId(id), obj);

    delete obj;
    QCOMPARE(QDeclarativeDebugService::objectForId(id), static_cast<QObject*>(0));
}

void tst_QDeclarativeDebugService::objectToString()
{
    QCOMPARE(QDeclarativeDebugService::objectToString(0), QString("NULL"));

    QObject *obj = new QObject;
    QCOMPARE(QDeclarativeDebugService::objectToString(obj), QString("QObject: <unnamed>"));

    obj->setObjectName("Hello");
    QCOMPARE(QDeclarativeDebugService::objectToString(obj), QString("QObject: Hello"));
    delete obj;
}


int main(int argc, char *argv[])
{
    int _argc = argc + 1;
    char **_argv = new char*[_argc];
    for (int i = 0; i < argc; ++i)
        _argv[i] = argv[i];
    _argv[_argc - 1] = "-qmljsdebugger=port:13769";

    QApplication app(_argc, _argv);
    tst_QDeclarativeDebugService tc;
    return QTest::qExec(&tc, _argc, _argv);
    delete _argv;
}

#include "tst_qdeclarativedebugservice.moc"
