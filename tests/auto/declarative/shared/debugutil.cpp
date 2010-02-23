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
#include <QSignalSpy>
#include <QEventLoop>
#include <QTimer>

#include <private/qmldebugclient_p.h>
#include <private/qmldebugservice_p.h>

#include "debugutil_p.h"

#include <iostream>

bool QmlDebugTest::waitForSignal(QObject *receiver, const char *member, int timeout) {
    QEventLoop loop;
    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(receiver, member, &loop, SLOT(quit()));
    timer.start(timeout);
    loop.exec();
    return timer.isActive();
}


QmlDebugTestData::QmlDebugTestData(QEventLoop *el)
    : exitCode(-1), loop(el)
{
}

QmlDebugTestData::~QmlDebugTestData()
{
    qDeleteAll(items);
}

void QmlDebugTestData::testsFinished(int code)
{
    exitCode = code;
    loop->quit();
}



QmlDebugTestService::QmlDebugTestService(const QString &s, QObject *parent)
    : QmlDebugService(s, parent), enabled(false)
{
}

void QmlDebugTestService::messageReceived(const QByteArray &ba)
{
    sendMessage(ba);
}

void QmlDebugTestService::enabledChanged(bool e)
{
    enabled = e;
    emit enabledStateChanged();
}


QmlDebugTestClient::QmlDebugTestClient(const QString &s, QmlDebugConnection *c)
    : QmlDebugClient(s, c)
{
}

QByteArray QmlDebugTestClient::waitForResponse()
{
    lastMsg.clear();
    QmlDebugTest::waitForSignal(this, SIGNAL(serverMessage(QByteArray)));
    if (lastMsg.isEmpty()) {
        qWarning() << "tst_QmlDebugClient: no response from server!";
        return QByteArray();
    }
    return lastMsg;
}

void QmlDebugTestClient::messageReceived(const QByteArray &ba)
{
    lastMsg = ba;
    emit serverMessage(ba);
}


tst_QmlDebug_Thread::tst_QmlDebug_Thread(QmlDebugTestData *data, QmlTestFactory *factory)
    : m_data(data), m_factory(factory)
{
}

void tst_QmlDebug_Thread::run()
{
    bool ok = false;

    QmlDebugConnection conn;
    conn.connectToHost("127.0.0.1", 3768);
    ok = conn.waitForConnected();
    Q_ASSERT(ok);

    QEventLoop loop;
    connect(m_data, SIGNAL(engineCreated()), &loop, SLOT(quit()));
    loop.exec();

    m_data->conn = &conn;

    Q_ASSERT(m_factory);
    QObject *test = m_factory->createTest(m_data);
    Q_ASSERT(test);
    int code = QTest::qExec(test, QCoreApplication::arguments()); 
    delete test;
    emit testsFinished(code);
}

int QmlDebugTest::runTests(QmlTestFactory *factory, const QList<QByteArray> &qml)
{
    qputenv("QML_DEBUG_SERVER_PORT", "3768");

    QEventLoop loop;
    QmlDebugTestData data(&loop);

    tst_QmlDebug_Thread thread(&data, factory);
    QObject::connect(&thread, SIGNAL(testsFinished(int)), &data, SLOT(testsFinished(int)));
    
    QmlDebugService::notifyOnServerStart(&thread, "start");

    QmlEngine engine;  // blocks until client connects

    foreach (const QByteArray &code, qml) {
        QmlComponent c(&engine);
        c.setData(code, QUrl::fromLocalFile(""));
        Q_ASSERT(c.isReady());  // fails if bad syntax
        data.items << qobject_cast<QmlGraphicsItem*>(c.create());
    }

    // start the test
    data.engine = &engine;
    emit data.engineCreated();

    loop.exec();
    thread.wait();

    return data.exitCode;
}

