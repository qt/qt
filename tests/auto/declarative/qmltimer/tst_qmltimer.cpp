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
#include <QtDeclarative/qmltimer.h>

class tst_qmltimer : public QObject
{
    Q_OBJECT
public:
    tst_qmltimer();

private slots:
    void notRepeating();
    void notRepeatingStart();
    void repeat();
    void triggeredOnStart();
    void triggeredOnStartRepeat();
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
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
}

void tst_qmltimer::notRepeatingStart()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100 }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 0);

    timer->start();
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
}

void tst_qmltimer::repeat()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; repeat: true; running: true }"), QUrl("file://"));
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
}

void tst_qmltimer::triggeredOnStart()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QTest::qWait(1);
    QCOMPARE(helper.count, 1);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
}

void tst_qmltimer::triggeredOnStartRepeat()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true; repeat: true }"), QUrl("file://"));
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
}

QTEST_MAIN(tst_qmltimer)

#include "tst_qmltimer.moc"
