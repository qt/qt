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


class tst_QDeclarativeDebugService : public QObject
{
    Q_OBJECT
private:
    QDeclarativeDebugConnection *m_conn;

private slots:
    void initTestCase();

    void name();
    void isEnabled();
    void enabledChanged();
    void sendMessage();
    void idForObject();
    void objectForId();
    void objectToString();
};

void tst_QDeclarativeDebugService::initTestCase()
{
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Waiting for connection on port 3769...");
    qputenv("QML_DEBUG_SERVER_PORT", "3769");
    new QDeclarativeEngine(this);

    m_conn = new QDeclarativeDebugConnection(this);
    m_conn->connectToHost("127.0.0.1", 3769);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Connection established");
    bool ok = m_conn->waitForConnected();
    Q_ASSERT(ok);

    QTRY_VERIFY(QDeclarativeDebugService::hasDebuggingClient());
}

void tst_QDeclarativeDebugService::name()
{
    QString name = "tst_QDeclarativeDebugService::name()";

    QDeclarativeDebugService service(name);
    QCOMPARE(service.name(), name);
}

void tst_QDeclarativeDebugService::isEnabled()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugService::isEnabled()", m_conn);
    QCOMPARE(service.isEnabled(), false);

    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugService::isEnabled()", m_conn);
    client.setEnabled(true);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), true);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugService: Conflicting plugin name \"tst_QDeclarativeDebugService::isEnabled()\" ");
    QDeclarativeDebugService duplicate("tst_QDeclarativeDebugService::isEnabled()", m_conn);
    QCOMPARE(duplicate.isEnabled(), false);
}

void tst_QDeclarativeDebugService::enabledChanged()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugService::enabledChanged()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugService::enabledChanged()", m_conn);

    QCOMPARE(service.enabled, false);

    client.setEnabled(true);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.enabled, true);
}

void tst_QDeclarativeDebugService::sendMessage()
{
    QDeclarativeDebugTestService service("tst_QDeclarativeDebugService::sendMessage()");
    QDeclarativeDebugTestClient client("tst_QDeclarativeDebugService::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
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

QTEST_MAIN(tst_QDeclarativeDebugService)

#include "tst_qdeclarativedebugservice.moc"
