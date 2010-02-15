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

#include "../shared/debugutil_p.h"

class tst_QmlDebugService : public QObject
{
    Q_OBJECT

public:
    tst_QmlDebugService(QmlDebugTestData *data)
    {
        m_conn = data->conn;
    }

    QmlDebugConnection *m_conn;

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

    QmlDebugService service(name);
    QCOMPARE(service.name(), name);
}

void tst_QmlDebugService::isEnabled()
{
    QmlDebugTestService service("tst_QmlDebugService::isEnabled()", m_conn);
    QCOMPARE(service.isEnabled(), false);

    QmlDebugTestClient client("tst_QmlDebugService::isEnabled()", m_conn);
    client.setEnabled(true);
    QmlDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.isEnabled(), true);

    QTest::ignoreMessage(QtWarningMsg, "QmlDebugService: Conflicting plugin name \"tst_QmlDebugService::isEnabled()\" ");
    QmlDebugService duplicate("tst_QmlDebugService::isEnabled()", m_conn);
    QCOMPARE(duplicate.isEnabled(), false);
}

void tst_QmlDebugService::enabledChanged()
{
    QmlDebugTestService service("tst_QmlDebugService::enabledChanged()");
    QmlDebugTestClient client("tst_QmlDebugService::enabledChanged()", m_conn);

    QCOMPARE(service.enabled, false);

    client.setEnabled(true);
    QmlDebugTest::waitForSignal(&service, SIGNAL(enabledStateChanged()));
    QCOMPARE(service.enabled, true);
}

void tst_QmlDebugService::sendMessage()
{
    QmlDebugTestService service("tst_QmlDebugService::sendMessage()");
    QmlDebugTestClient client("tst_QmlDebugService::sendMessage()", m_conn);

    QByteArray msg = "hello!";

    client.sendMessage(msg);
    QByteArray resp = client.waitForResponse();
    QCOMPARE(resp, msg);
}

void tst_QmlDebugService::idForObject()
{
    QCOMPARE(QmlDebugService::idForObject(0), -1);

    QObject *objA = new QObject;

    int idA = QmlDebugService::idForObject(objA);
    QVERIFY(idA >= 0);
    QCOMPARE(QmlDebugService::objectForId(idA), objA);

    int idAA = QmlDebugService::idForObject(objA);
    QCOMPARE(idAA, idA);

    QObject *objB = new QObject;
    int idB = QmlDebugService::idForObject(objB);
    QVERIFY(idB != idA);
    QCOMPARE(QmlDebugService::objectForId(idB), objB);
 
    delete objA;
    delete objB;
}

void tst_QmlDebugService::objectForId()
{
    QCOMPARE(QmlDebugService::objectForId(-1), static_cast<QObject*>(0));
    QCOMPARE(QmlDebugService::objectForId(1), static_cast<QObject*>(0));

    QObject *obj = new QObject;
    int id = QmlDebugService::idForObject(obj);
    QCOMPARE(QmlDebugService::objectForId(id), obj);

    delete obj;
    QCOMPARE(QmlDebugService::objectForId(id), static_cast<QObject*>(0));
}

void tst_QmlDebugService::objectToString()
{
    QCOMPARE(QmlDebugService::objectToString(0), QString("NULL"));

    QObject *obj = new QObject;
    QCOMPARE(QmlDebugService::objectToString(obj), QString("QObject: <unnamed>"));

    obj->setObjectName("Hello");
    QCOMPARE(QmlDebugService::objectToString(obj), QString("QObject: Hello"));
    delete obj;
}


class tst_QmlDebugService_Factory : public QmlTestFactory
{
public:
    QObject *createTest(QmlDebugTestData *data) { return new tst_QmlDebugService(data); }
};

// This does not use QTEST_MAIN because the test has to be created and run
// in a separate thread.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    tst_QmlDebugService_Factory factory;
    return QmlDebugTest::runTests(&factory);
}

#include "tst_qmldebugservice.moc"
