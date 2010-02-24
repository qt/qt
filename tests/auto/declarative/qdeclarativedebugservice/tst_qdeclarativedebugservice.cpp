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

#include "../shared/debugutil_p.h"

class tst_QmlDebugService : public QObject
{
    Q_OBJECT

public:
    tst_QmlDebugService(QDeclarativeDebugTestData *data)
    {
        m_conn = data->conn;
    }

    QDeclarativeDebugConnection *m_conn;

private slots:
    void name();
    void isEnabled();
    void enabledChanged();
    void sendMessage();
    void idForObject();
    void objectForId();
    void objectToString();
};

void tst_QmlDebugService::name()
{
    QString name = "tst_QmlDebugService::name()";

    QDeclarativeDebugService service(name);
    QCOMPARE(service.name(), name);
}

void tst_QmlDebugService::isEnabled()
{
    QDeclarativeDebugTestService service("tst_QmlDebugService::isEnabled()", m_conn);
    QCOMPARE(service.isEnabled(), false);

    QDeclarativeDebugTestClient client("tst_QmlDebugService::isEnabled()", m_conn);
    client.setEnabled(true);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), true);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugService: Conflicting plugin name \"tst_QmlDebugService::isEnabled()\" ");
    QDeclarativeDebugService duplicate("tst_QmlDebugService::isEnabled()", m_conn);
    QCOMPARE(duplicate.isEnabled(), false);
}

void tst_QmlDebugService::enabledChanged()
{
    QDeclarativeDebugTestService service("tst_QmlDebugService::enabledChanged()");
    QDeclarativeDebugTestClient client("tst_QmlDebugService::enabledChanged()", m_conn);

    QCOMPARE(service.enabled, false);

    client.setEnabled(true);
    QDeclarativeDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.enabled, true);
}

void tst_QmlDebugService::sendMessage()
{
    QDeclarativeDebugTestService service("tst_QmlDebugService::sendMessage()");
    QDeclarativeDebugTestClient client("tst_QmlDebugService::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
}

void tst_QmlDebugService::idForObject()
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

void tst_QmlDebugService::objectForId()
{
    QCOMPARE(QDeclarativeDebugService::objectForId(-1), static_cast<QObject*>(0));
    QCOMPARE(QDeclarativeDebugService::objectForId(1), static_cast<QObject*>(0));

    QObject *obj = new QObject;
    int id = QDeclarativeDebugService::idForObject(obj);
    QCOMPARE(QDeclarativeDebugService::objectForId(id), obj);

    delete obj;
    QCOMPARE(QDeclarativeDebugService::objectForId(id), static_cast<QObject*>(0));
}

void tst_QmlDebugService::objectToString()
{
    QCOMPARE(QDeclarativeDebugService::objectToString(0), QString("NULL"));

    QObject *obj = new QObject;
    QCOMPARE(QDeclarativeDebugService::objectToString(obj), QString("QObject: <unnamed>"));

    obj->setObjectName("Hello");
    QCOMPARE(QDeclarativeDebugService::objectToString(obj), QString("QObject: Hello"));
    delete obj;
}


class tst_QmlDebugService_Factory : public QDeclarativeTestFactory
{
public:
    QObject *createTest(QDeclarativeDebugTestData *data) { return new tst_QmlDebugService(data); }
};

// This does not use QTEST_MAIN because the test has to be created and run
// in a separate thread.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    tst_QmlDebugService_Factory factory;
    return QDeclarativeDebugTest::runTests(&factory);
}

#include "tst_qdeclarativedebugservice.moc"
