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


#include <qabstracteventdispatcher.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qeventloop.h>
#include <qmutex.h>
#include <qthread.h>
#include <qtimer.h>
#include <qwaitcondition.h>
#include <QTcpServer>
#include <QTcpSocket>

//TESTED_CLASS=
//TESTED_FILES=

class EventLoopExiter : public QObject
{
    Q_OBJECT
    QEventLoop *eventLoop;
public:
    inline EventLoopExiter(QEventLoop *el)
        : eventLoop(el)
    { }
public slots:
    void exit();
    void exit1();
    void exit2();
};

void EventLoopExiter::exit()
{ eventLoop->exit(); }

void EventLoopExiter::exit1()
{ eventLoop->exit(1); }

void EventLoopExiter::exit2()
{ eventLoop->exit(2); }

class EventLoopThread : public QThread
{
    Q_OBJECT
signals:
    void checkPoint();
public:
    QEventLoop *eventLoop;
    void run();
};

void EventLoopThread::run()
{
    eventLoop = new QEventLoop;
    emit checkPoint();
    (void) eventLoop->exec();
    delete eventLoop;
    eventLoop = 0;
}

class MultipleExecThread : public QThread
{
    Q_OBJECT
signals:
    void checkPoint();
public:
    QMutex mutex;
    QWaitCondition cond;
    void run()
    {
        QMutexLocker locker(&mutex);
        // this exec should work

        cond.wakeOne();
        cond.wait(&mutex);

        QTimer timer;
        connect(&timer, SIGNAL(timeout()), SLOT(quit()), Qt::DirectConnection);
        timer.setInterval(1000);
        timer.start();
        (void) exec();

        // this should return immediately, since exit() has been called
        cond.wakeOne();
        cond.wait(&mutex);
        QEventLoop eventLoop;
        (void) eventLoop.exec();
    }
};

class StartStopEvent: public QEvent
{
public:
    StartStopEvent(int type, QEventLoop *loop = 0)
        : QEvent(Type(type)), el(loop)
    { }

    QEventLoop *el;
};

class EventLoopExecutor : public QObject
{
    Q_OBJECT
    QEventLoop *eventLoop;
public:
    int returnCode;
    EventLoopExecutor(QEventLoop *eventLoop)
        : QObject(), eventLoop(eventLoop), returnCode(-42)
    {
    }
public slots:
    void exec()
    {
        QTimer::singleShot(100, eventLoop, SLOT(quit()));
        // this should return immediately, and the timer event should be delivered to
        // tst_QEventLoop::exec() test, letting the test complete
        returnCode = eventLoop->exec();
    }
};

#ifndef QT_NO_EXCEPTIONS
class QEventLoopTestException { };

class ExceptionThrower : public QObject
{
    Q_OBJECT
public:
    ExceptionThrower() : QObject() { }
public slots:
    void throwException()
    {
        QEventLoopTestException e;
        throw e;
    }
};
#endif

class tst_QEventLoop : public QObject
{
    Q_OBJECT
public:
    tst_QEventLoop();
    ~tst_QEventLoop();
public slots:
    void init();
    void cleanup();
private slots:
    void processEvents();
    void exec();
    void exit();
    void wakeUp();
    void quit();
    void processEventsExcludeSocket();
    void processEventsExcludeTimers();

    // keep this test last:
    void nestedLoops();

protected:
    void customEvent(QEvent *e);
};

tst_QEventLoop::tst_QEventLoop()
{ }

tst_QEventLoop::~tst_QEventLoop()
{ }

void tst_QEventLoop::init()
{ }

void tst_QEventLoop::cleanup()
{ }

void tst_QEventLoop::processEvents()
{
    QSignalSpy spy1(QAbstractEventDispatcher::instance(), SIGNAL(aboutToBlock()));
    QSignalSpy spy2(QAbstractEventDispatcher::instance(), SIGNAL(awake()));

    QEventLoop eventLoop;

    QCoreApplication::postEvent(&eventLoop, new QEvent(QEvent::User));

    // process posted events, QEventLoop::processEvents() should return
    // true
    QVERIFY(eventLoop.processEvents());
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

    // allow any session manager to complete its handshake, so that
    // there are no pending events left.
    while (eventLoop.processEvents())
        ;

    // On mac we get application started events at this point,
    // so process events one more time just to be sure.
    eventLoop.processEvents();

    // no events to process, QEventLoop::processEvents() should return
    // false
    spy1.clear();
    spy2.clear();
    QVERIFY(!eventLoop.processEvents());
    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 1);

    // make sure the test doesn't block forever
    int timerId = startTimer(100);

    // wait for more events to process, QEventLoop::processEvents()
    // should return true
    spy1.clear();
    spy2.clear();
    QVERIFY(eventLoop.processEvents(QEventLoop::WaitForMoreEvents));

    // Verify that the eventloop has blocked and woken up. Some eventloops
    // may block and wake up multiple times.
    QVERIFY(spy1.count() > 0);
    QVERIFY(spy2.count() > 0);
    // We should get one awake for each aboutToBlock, plus one awake when
    // processEvents is entered.
    QVERIFY(spy2.count() >= spy1.count());

    killTimer(timerId);
}

void tst_QEventLoop::exec()
{
    {
        QEventLoop eventLoop;
        EventLoopExiter exiter(&eventLoop);
        int returnCode;

        QTimer::singleShot(100, &exiter, SLOT(exit()));
        returnCode = eventLoop.exec();
        QCOMPARE(returnCode, 0);

        QTimer::singleShot(100, &exiter, SLOT(exit1()));
        returnCode = eventLoop.exec();
        QCOMPARE(returnCode, 1);

        QTimer::singleShot(100, &exiter, SLOT(exit2()));
        returnCode = eventLoop.exec();
        QCOMPARE(returnCode, 2);
    }

    {
        // calling exec() after exit()/quit() should return immediately
        MultipleExecThread thread;

        // start thread and wait for checkpoint
        thread.mutex.lock();
        thread.start();
        thread.cond.wait(&thread.mutex);

        // make sure the eventloop runs
        QSignalSpy spy(QAbstractEventDispatcher::instance(&thread), SIGNAL(awake()));
        thread.cond.wakeOne();
        thread.cond.wait(&thread.mutex);
        QVERIFY(spy.count() > 0);

        // exec should return immediately
        spy.clear();
        thread.cond.wakeOne();
        thread.mutex.unlock();
        thread.wait();
        QCOMPARE(spy.count(), 0);
    }

    {
        // a single instance of QEventLoop should not be allowed to recurse into exec()
        QEventLoop eventLoop;
        EventLoopExecutor executor(&eventLoop);

        QTimer::singleShot(100, &executor, SLOT(exec()));
        int returnCode = eventLoop.exec();
        QCOMPARE(returnCode, 0);
        QCOMPARE(executor.returnCode, -1);
    }

#if !defined(QT_NO_EXCEPTIONS) && !defined(Q_OS_WINCE_WM)
    // Windows Mobile cannot handle cross library exceptions
    // qobject.cpp will try to rethrow the exception after handling
    // which causes gwes.exe to crash
    {
        // QEventLoop::exec() is exception safe
        QEventLoop eventLoop;
        int caughtExceptions = 0;

        try {
            ExceptionThrower exceptionThrower;
            QTimer::singleShot(100, &exceptionThrower, SLOT(throwException()));
            (void) eventLoop.exec();
        } catch (...) {
            ++caughtExceptions;
        }
        try {
            ExceptionThrower exceptionThrower;
            QTimer::singleShot(100, &exceptionThrower, SLOT(throwException()));
            (void) eventLoop.exec();
        } catch (...) {
            ++caughtExceptions;
        }
        QCOMPARE(caughtExceptions, 2);
    }
#endif
}

void tst_QEventLoop::exit()
{ DEPENDS_ON(exec()); }

void tst_QEventLoop::wakeUp()
{
    EventLoopThread thread;
    QEventLoop eventLoop;
    connect(&thread, SIGNAL(checkPoint()), &eventLoop, SLOT(quit()));
    connect(&thread, SIGNAL(finished()), &eventLoop, SLOT(quit()));

    thread.start();
    (void) eventLoop.exec();

    QSignalSpy spy(QAbstractEventDispatcher::instance(&thread), SIGNAL(awake()));
    thread.eventLoop->wakeUp();

    // give the thread time to wake up
    QTimer::singleShot(1000, &eventLoop, SLOT(quit()));
    (void) eventLoop.exec();

    QVERIFY(spy.count() > 0);

    thread.quit();
    (void) eventLoop.exec();
}

void tst_QEventLoop::quit()
{
    QEventLoop eventLoop;
    int returnCode;

    QTimer::singleShot(100, &eventLoop, SLOT(quit()));
    returnCode = eventLoop.exec();
    QCOMPARE(returnCode, 0);
}


void tst_QEventLoop::nestedLoops()
{
    QCoreApplication::postEvent(this, new StartStopEvent(QEvent::User));
    QCoreApplication::postEvent(this, new StartStopEvent(QEvent::User));
    QCoreApplication::postEvent(this, new StartStopEvent(QEvent::User));

    // without the fix, this will *wedge* and never return
    QTest::qWait(1000);
}

void tst_QEventLoop::customEvent(QEvent *e)
{
    if (e->type() == QEvent::User) {
        QEventLoop loop;
        QCoreApplication::postEvent(this, new StartStopEvent(int(QEvent::User) + 1, &loop));
        loop.exec();
    } else {
        static_cast<StartStopEvent *>(e)->el->exit();
    }
}

class SocketEventsTester: public QObject
{
    Q_OBJECT
public:
    SocketEventsTester()
    {
        socket = 0;
        server = 0;
        dataArrived = false;
        testResult = false;
    }
    ~SocketEventsTester()
    {
        delete socket;
        delete server;
    }
    bool init()
    {
        bool ret = false;
        server = new QTcpServer();
        socket = new QTcpSocket();
        connect(server, SIGNAL(newConnection()), this, SLOT(sendHello()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(sendAck()), Qt::DirectConnection);
        if((ret = server->listen(QHostAddress::LocalHost, 0))) {
            socket->connectToHost(server->serverAddress(), server->serverPort());
            socket->waitForConnected();
        }
        return ret;
    }

    QTcpSocket *socket;
    QTcpServer *server;
    bool dataArrived;
    bool testResult;
public slots:
    void sendAck()
    {
        dataArrived = true;
    }
    void sendHello()
    {
        char data[10] ="HELLO";
        qint64 size = sizeof(data);

        QTcpSocket *serverSocket = server->nextPendingConnection();
        serverSocket->write(data, size);
        serverSocket->flush();
        QCoreApplication::processEvents(QEventLoop::ExcludeSocketNotifiers);
        testResult = dataArrived;
        serverSocket->close();
        QThread::currentThread()->exit(0);
    }
};

class SocketTestThread : public QThread
{
    Q_OBJECT
public:
    SocketTestThread():QThread(0),testResult(false){};
    void run()
    {
        SocketEventsTester *tester = new SocketEventsTester();
        if (tester->init())
            exec();
        testResult = tester->testResult;
        delete tester;
    }
     bool testResult;
};

void tst_QEventLoop::processEventsExcludeSocket()
{
    SocketTestThread thread;
    thread.start();
    QVERIFY(thread.wait());
    QVERIFY(!thread.testResult);
}

class TimerReceiver : public QObject
{
public:
    int gotTimerEvent;

    TimerReceiver()
        : QObject(), gotTimerEvent(-1)
    { }

    void timerEvent(QTimerEvent *event)
    {
        gotTimerEvent = event->timerId();
    }
};

void tst_QEventLoop::processEventsExcludeTimers()
{
    TimerReceiver timerReceiver;
    int timerId = timerReceiver.startTimer(0);

    QEventLoop eventLoop;

    // normal process events will send timers
    eventLoop.processEvents();
    QCOMPARE(timerReceiver.gotTimerEvent, timerId);
    timerReceiver.gotTimerEvent = -1;

    // normal process events will send timers
    eventLoop.processEvents(QEventLoop::X11ExcludeTimers);
#ifndef Q_OS_UNIX
    QEXPECT_FAIL("", "X11ExcludeTimers only works on UN*X", Continue);
#endif
    QCOMPARE(timerReceiver.gotTimerEvent, -1);
    timerReceiver.gotTimerEvent = -1;

    // resume timer processing
    eventLoop.processEvents();
    QCOMPARE(timerReceiver.gotTimerEvent, timerId);
    timerReceiver.gotTimerEvent = -1;
}

QTEST_MAIN(tst_QEventLoop)
#include "tst_qeventloop.moc"































// previous test

#if 0

#include <qwidget.h>

#ifdef Q_WS_WIN
#include <windows.h>
#endif



//TESTED_CLASS=
//TESTED_FILES=

class EventHandlerWidget : public QWidget
{
public:
    EventHandlerWidget( QWidget* parent = 0, const char* name = 0 ) : QWidget( parent, name )
    {
        installEventFilter( this );
        recievedPaintEvent = FALSE;
        recievedMouseEvent = FALSE;
    }
    ~EventHandlerWidget() {}

    bool recievedPaintEvent;
    bool recievedMouseEvent;

protected:
    bool eventFilter( QObject* o, QEvent* e )
    {
        if ( e->type() == QEvent::Paint )
            recievedPaintEvent = TRUE;
        else if ( e->type() == QEvent::MouseButtonPress )
            recievedMouseEvent = TRUE;
        return QWidget::eventFilter( o, e );
    }
};

class InBetweenObject : public QObject
{
    Q_OBJECT
public:
    InBetweenObject(QObject *parent, QObject *child)
    : QObject(parent), childObject(child)
    {
        childObject->setParent(this);
        ++instanceCounter;
    }

    ~InBetweenObject()
    {
        --instanceCounter;
    }

    static int instanceCounter;

protected:
    void childEvent(QChildEvent *e)
    {
        if (e->removed() && e->child() == childObject) {
            deleteLater();
        }
    }

private:
    QObject *childObject;
};

class ObjectContainer : public QObject
{
public:
    ObjectContainer(QObject *parent = 0)
    : QObject(parent)
    {
    }

protected:
    void childEvent(QChildEvent *e)
    {
        if (e->inserted() && !::qobject_cast<InBetweenObject*>(e->child())) {
            InBetweenObject *inBetween = new InBetweenObject(this, e->child());
        }
    }
};

class tst_QEventLoop : public QObject
{
    Q_OBJECT
public:
    tst_QEventLoop();
    ~tst_QEventLoop();
public slots:
    void init();
    void cleanup();
private slots:
    void processEvents();
    void eventHandlerPostsEvent();
};

tst_QEventLoop::tst_QEventLoop()
{
}

tst_QEventLoop::~tst_QEventLoop()
{
}

void tst_QEventLoop::init()
{
}

void tst_QEventLoop::cleanup()
{
}


void tst_QEventLoop::processEvents()
{
    EventHandlerWidget *mainWidget = new EventHandlerWidget( 0 );
    mainWidget->show();
    qApp->setMainWidget( mainWidget );

#ifdef Q_WS_WIN
    QEventLoop* eventLoop = qApp->eventLoop();
    eventLoop->processEvents( QEventLoop::AllEvents );
    QVERIFY( !eventLoop->hasPendingEvents() );

    // Make sure the flag is cleared first
    mainWidget->recievedPaintEvent = FALSE;
#ifdef Q_WS_WIN
    InvalidateRect( mainWidget->winId(), 0, TRUE );
#endif
    QVERIFY( !mainWidget->recievedPaintEvent );
    eventLoop->processEvents( QEventLoop::AllEvents );
    QVERIFY( mainWidget->recievedPaintEvent );

#ifdef Q_WS_WIN
    // TODO: Hardcoded for now...
    LPARAM lParam = MAKELPARAM( 10, 10 );
    PostMessage( mainWidget->winId(), WM_LBUTTONDOWN, 0, lParam );
#endif

    mainWidget->recievedMouseEvent = FALSE;
    eventLoop->processEvents( QEventLoop::ExcludeUserInput );
    QVERIFY( !mainWidget->recievedMouseEvent );
#else
    QSKIP( "QEventLoop test is not implememented on X11 yet", SkipAll);
#endif
}

int InBetweenObject::instanceCounter = 0;

void tst_QEventLoop::eventHandlerPostsEvent()
{
    ObjectContainer container;

    QObject *object = new QObject(&container);
    qApp->processEvents();
    qApp->processEvents();

    QCOMPARE(InBetweenObject::instanceCounter, 1);

    object->deleteLater();

    qApp->processEvents();
    QCOMPARE(InBetweenObject::instanceCounter, 0);
};

QTEST_MAIN(tst_QEventLoop)
#include "tst_qeventloop.moc"

#endif
