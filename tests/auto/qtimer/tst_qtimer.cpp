/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include <qtimer.h>
#include <qthread.h>





#if defined Q_OS_UNIX
#include <unistd.h>
#endif


//TESTED_CLASS=
//TESTED_FILES=

class tst_QTimer : public QObject
{
    Q_OBJECT

public:
    tst_QTimer();
    virtual ~tst_QTimer();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void singleShotTimeout();
    void timeout();
    void livelock_data();
    void livelock();
    void timerInfiniteRecursion_data();
    void timerInfiniteRecursion();
    void recurringTimer_data();
    void recurringTimer();
    void deleteLaterOnQTimer(); // long name, don't want to shadow QObject::deleteLater()
    void moveToThread();
    void restartedTimerFiresTooSoon();
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
    void timeout();
};

void TimerHelper::timeout()
{
    ++count;
}

tst_QTimer::tst_QTimer()
{
}

tst_QTimer::~tst_QTimer()
{
}

void tst_QTimer::initTestCase()
{
}

void tst_QTimer::cleanupTestCase()
{
}

void tst_QTimer::init()
{
}

void tst_QTimer::cleanup()
{
}

void tst_QTimer::singleShotTimeout()
{
    TimerHelper helper;
    QTimer timer;
    timer.setSingleShot(true);

    connect(&timer, SIGNAL(timeout()), &helper, SLOT(timeout()));
    timer.start(100);

    QTest::qWait(500);
    QCOMPARE(helper.count, 1);
    QTest::qWait(500);
    QCOMPARE(helper.count, 1);
}

void tst_QTimer::timeout()
{
    TimerHelper helper;
    QTimer timer;

    connect(&timer, SIGNAL(timeout()), &helper, SLOT(timeout()));
    timer.start(100);

    QCOMPARE(helper.count, 0);

    QTest::qWait(200);
    QVERIFY(helper.count > 0);
    int oldCount = helper.count;

    QTest::qWait(200);
    QVERIFY(helper.count > oldCount);
}


void tst_QTimer::livelock_data()
{
    QTest::addColumn<int>("interval");
    QTest::newRow("zero timer") << 0;
    QTest::newRow("non-zero timer") << 1;
    QTest::newRow("longer than sleep") << 20;
}

/*!
 *
 * DO NOT "FIX" THIS TEST!  it is written like this for a reason, do
 * not *change it without first dicussing it with its maintainers.
 *
*/
class LiveLockTester : public QObject
{
public:
    LiveLockTester(int i)
        : interval(i),
          timeoutsForFirst(0), timeoutsForExtra(0), timeoutsForSecond(0),
          postEventAtRightTime(false)
    {
        firstTimerId = startTimer(interval);
        extraTimerId = startTimer(interval + 80);
        secondTimerId = -1; // started later
    }

    bool event(QEvent *e) {
        if (e->type() == 4002) {
            // got the posted event
            if (timeoutsForFirst == 1 && timeoutsForSecond == 0)
                postEventAtRightTime = true;
            return true;
        }
        return QObject::event(e);
    }

    void timerEvent(QTimerEvent *te) {
        if (te->timerId() == firstTimerId) {
            if (++timeoutsForFirst == 1) {
                killTimer(extraTimerId);
                extraTimerId = -1;
		QCoreApplication::postEvent(this, new QEvent(static_cast<QEvent::Type>(4002)));
                secondTimerId = startTimer(interval);
            }
        } else if (te->timerId() == secondTimerId) {
            ++timeoutsForSecond;
        } else if (te->timerId() == extraTimerId) {
            ++timeoutsForExtra;
        }

        // sleep for 2ms
        QTest::qSleep(2);

        killTimer(te->timerId());
    }

    const int interval;
    int firstTimerId;
    int secondTimerId;
    int extraTimerId;
    int timeoutsForFirst;
    int timeoutsForExtra;
    int timeoutsForSecond;
    bool postEventAtRightTime;
};

void tst_QTimer::livelock()
{
    /*
      New timers created in timer event handlers should not be sent
      until the next iteration of the eventloop.  Note: this test
      depends on the fact that we send posted events before timer
      events (since new posted events are not sent until the next
      iteration of the eventloop either).
    */
    QFETCH(int, interval);
    LiveLockTester tester(interval);
    QTest::qWait(180); // we have to use wait here, since we're testing timers with a non-zero timeout
    QCOMPARE(tester.timeoutsForFirst, 1);
    QCOMPARE(tester.timeoutsForExtra, 0);
    QCOMPARE(tester.timeoutsForSecond, 1);
#if defined(Q_OS_MAC)
    QEXPECT_FAIL("zero timer", "Posted events source are handled AFTER timers", Continue);
    QEXPECT_FAIL("non-zero timer", "Posted events source are handled AFTER timers", Continue);
#elif defined(Q_OS_UNIX)
    QEXPECT_FAIL("zero timer", "", Continue);
    QEXPECT_FAIL("non-zero timer", "", Continue);
#elif defined(Q_OS_WIN)
	if (QSysInfo::WindowsVersion < QSysInfo::WV_XP)
		QEXPECT_FAIL("non-zero timer", "Multimedia timers are not available on Win2K/9x", Continue);
#endif
    QVERIFY(tester.postEventAtRightTime);
}

class TimerInfiniteRecursionObject : public QObject
{
public:
    bool inTimerEvent;
    bool timerEventRecursed;
    int interval;

    TimerInfiniteRecursionObject(int interval)
        : inTimerEvent(false), timerEventRecursed(false), interval(interval)
    { }

    void timerEvent(QTimerEvent *timerEvent)
    {
        timerEventRecursed = inTimerEvent;
        if (timerEventRecursed) {
            // bug detected!
            return;
        }

        inTimerEvent = true;

        QEventLoop eventLoop;
        QTimer::singleShot(qMax(100, interval * 2), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        inTimerEvent = false;

        killTimer(timerEvent->timerId());
    }
};

void tst_QTimer::timerInfiniteRecursion_data()
{
    QTest::addColumn<int>("interval");
    QTest::newRow("zero timer") << 0;
    QTest::newRow("non-zero timer") << 1;
    QTest::newRow("10ms timer") << 10;
    QTest::newRow("11ms timer") << 11;
    QTest::newRow("100ms timer") << 100;
    QTest::newRow("1s timer") << 1000;
}


void tst_QTimer::timerInfiniteRecursion()
{
    QFETCH(int, interval);
    TimerInfiniteRecursionObject object(interval);
    (void) object.startTimer(interval);

    QEventLoop eventLoop;
    QTimer::singleShot(qMax(100, interval * 2), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QVERIFY(!object.timerEventRecursed);
}

class RecurringTimerObject : public QObject
{
Q_OBJECT
public:
    int times;
    int target;
    bool recurse;

    RecurringTimerObject(int target)
        : times(0), target(target), recurse(false)
    { }

    void timerEvent(QTimerEvent *timerEvent)
    {
        if (++times == target) {
            killTimer(timerEvent->timerId());
            emit done();
        } if (recurse) {
            QEventLoop eventLoop;
            QTimer::singleShot(100, &eventLoop, SLOT(quit()));
            eventLoop.exec();
        }
    }

signals:
    void done();
};

void tst_QTimer::recurringTimer_data()
{
    QTest::addColumn<int>("interval");
    QTest::newRow("zero timer") << 0;
    QTest::newRow("non-zero timer") << 1;
}

void tst_QTimer::recurringTimer()
{
    const int target = 5;
    QFETCH(int, interval);

    {
        RecurringTimerObject object(target);
        QObject::connect(&object, SIGNAL(done()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        (void) object.startTimer(interval);
        QTestEventLoop::instance().enterLoop(5);

        QCOMPARE(object.times, target);
    }

    {
        // make sure that eventloop recursion doesn't effect timer recurrance
        RecurringTimerObject object(target);
        object.recurse = true;

        QObject::connect(&object, SIGNAL(done()), &QTestEventLoop::instance(), SLOT(exitLoop()));
        (void) object.startTimer(interval);
        QTestEventLoop::instance().enterLoop(5);

        QCOMPARE(object.times, target);
    }
}

void tst_QTimer::deleteLaterOnQTimer()
{
    QTimer *timer = new QTimer;
    connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
    connect(timer, SIGNAL(destroyed()), &QTestEventLoop::instance(), SLOT(exitLoop()));
    timer->setInterval(1);
    timer->setSingleShot(true);
    timer->start();
    QPointer<QTimer> pointer = timer;
    QTestEventLoop::instance().enterLoop(5);
    QVERIFY(pointer.isNull());
}

void tst_QTimer::moveToThread()
{
    QTimer ti1;
    QTimer ti2;
    ti1.start(200);
    ti2.start(200);
    QVERIFY((ti1.timerId() & 0xffffff) != (ti2.timerId() & 0xffffff));
    QThread tr;
    ti1.moveToThread(&tr);
    connect(&ti1,SIGNAL(timeout()), &tr, SLOT(quit()));
    tr.start();
    QTimer ti3;
    ti3.start(200);
    QVERIFY((ti3.timerId() & 0xffffff) != (ti2.timerId() & 0xffffff));
    QVERIFY((ti3.timerId() & 0xffffff) != (ti1.timerId() & 0xffffff));
    QTest::qWait(300);
    QVERIFY(tr.wait());
    ti2.stop();
    QTimer ti4;
    ti4.start(200);
    ti3.stop();
    ti2.start(200);
    ti3.start(200);
    QVERIFY((ti4.timerId() & 0xffffff) != (ti2.timerId() & 0xffffff));
    QVERIFY((ti3.timerId() & 0xffffff) != (ti2.timerId() & 0xffffff));
    QVERIFY((ti3.timerId() & 0xffffff) != (ti1.timerId() & 0xffffff));
}

class RestartedTimerFiresTooSoonObject : public QObject
{
    Q_OBJECT

public:
    QBasicTimer m_timer;

    int m_interval;
    QTime m_startedTime;
    QEventLoop eventLoop;

    inline RestartedTimerFiresTooSoonObject()
        : QObject(), m_interval(0)
    { }

    void timerFired()
    {
        static int interval = 1000;

        m_interval = interval;
        m_startedTime.start();
        m_timer.start(interval, this);

        // alternate between single-shot and 1 sec
        interval = interval ? 0 : 1000;
    }

    void timerEvent(QTimerEvent* ev)
    {
        if (ev->timerId() != m_timer.timerId())
            return;

        m_timer.stop();

        QTime now = QTime::currentTime();
        int elapsed = m_startedTime.elapsed();

        if (elapsed < m_interval / 2) {
            // severely too early!
            m_timer.stop();
            eventLoop.exit(-1);
            return;
        }

        timerFired();

        // don't do this forever
        static int count = 0;
        if (count++ > 20) {
            m_timer.stop();
            eventLoop.quit();
            return;
        }
    }
};

void tst_QTimer::restartedTimerFiresTooSoon()
{
    RestartedTimerFiresTooSoonObject object;
    object.timerFired();
    QVERIFY(object.eventLoop.exec() == 0);
}

QTEST_MAIN(tst_QTimer)
#include "tst_qtimer.moc"
\
