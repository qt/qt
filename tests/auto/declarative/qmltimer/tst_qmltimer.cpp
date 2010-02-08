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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmltimer_p.h>
#include <QDebug>

class tst_qmltimer : public QObject
{
    Q_OBJECT
public:
    tst_qmltimer();

private slots:
    void notRepeating();
    void notRepeatingStart();
    void repeat();
    void noTriggerIfNotRunning();
    void triggeredOnStart();
    void triggeredOnStartRepeat();
    void changeDuration();
    void restart();
};

class TimerHelper : public QObject
{
    Q_OBJECT
public:
    TimerHelper() : QObject(), count(0)
    {
    }

    int count;

public slots:
    void timeout() {
        ++count;
    }
};

#if defined(Q_OS_SYMBIAN) && defined(Q_CC_NOKIAX86)
// Increase wait as emulator startup can cause unexpected delays
#define TIMEOUT_TIMEOUT 2000
#else
#define TIMEOUT_TIMEOUT 200
#endif

tst_qmltimer::tst_qmltimer()
{
}

void tst_qmltimer::notRepeating()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 100; running: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);
    QVERIFY(timer->isRunning());
    QVERIFY(!timer->isRepeating());
    QCOMPARE(timer->interval(), 100);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QVERIFY(timer->isRunning() == false);
}

void tst_qmltimer::notRepeatingStart()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 100 }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);
    QVERIFY(!timer->isRunning());

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 0);

    timer->start();
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QVERIFY(timer->isRunning() == false);

    delete timer;
}

void tst_qmltimer::repeat()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 100; repeat: true; running: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QCOMPARE(helper.count, 0);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > 0);
    int oldCount = helper.count;

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > oldCount);
    QVERIFY(timer->isRunning());

    oldCount = helper.count;
    timer->stop();

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count == oldCount);
    QVERIFY(timer->isRunning() == false);

    delete timer;
}

void tst_qmltimer::triggeredOnStart()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);
    QVERIFY(timer->triggeredOnStart());

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QTest::qWait(1);
    QCOMPARE(helper.count, 1);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
    QVERIFY(timer->isRunning() == false);

    delete timer;
}

void tst_qmltimer::triggeredOnStartRepeat()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true; repeat: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QTest::qWait(1);
    QCOMPARE(helper.count, 1);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > 1);
    int oldCount = helper.count;
    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > oldCount);
    QVERIFY(timer->isRunning());

    delete timer;
}

void tst_qmltimer::noTriggerIfNotRunning()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray(
        "import Qt 4.6\n"
        "Item { property bool ok: true\n"
            "Timer { id: t1; interval: 100; repeat: true; running: true; onTriggered: if (!running) ok=false }"
            "Timer { interval: 10; running: true; onTriggered: t1.running=false }"
        "}"
    ), QUrl::fromLocalFile(""));
    QObject *item = component.create();
    QVERIFY(item != 0);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(item->property("ok").toBool(), true);

    delete item;
}

void tst_qmltimer::changeDuration()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 200; repeat: true; running: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QCOMPARE(helper.count, 0);

    QTest::qWait(500);
    QCOMPARE(helper.count, 2);

    timer->setInterval(500);

    QTest::qWait(600);
    QCOMPARE(helper.count, 3);
    QVERIFY(timer->isRunning());

    delete timer;
}

void tst_qmltimer::restart()
{
    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\nTimer { interval: 500; repeat: true; running: true }"), QUrl::fromLocalFile(""));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QCOMPARE(helper.count, 0);

    QTest::qWait(600);
    QCOMPARE(helper.count, 1);

    QTest::qWait(300);

    timer->restart();

    QTest::qWait(700);

    QCOMPARE(helper.count, 2);
    QVERIFY(timer->isRunning());

    delete timer;
}

QTEST_MAIN(tst_qmltimer)

#include "tst_qmltimer.moc"
