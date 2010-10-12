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


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qmutex.h>
#include <qthread.h>
#include <qwaitcondition.h>
#include <qthreadstorage.h>

#ifdef Q_OS_UNIX
#include <pthread.h>
#endif
#ifdef Q_OS_WIN
#ifndef Q_OS_WINCE
#include <process.h>
#endif
#include <windows.h>
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_QThreadStorage : public QObject
{
    Q_OBJECT

public:
    tst_QThreadStorage();

private slots:
    void hasLocalData();
    void localData();
    void localData_const();
    void setLocalData();
    void autoDelete();
    void adoptedThreads();
    void ensureCleanupOrder();
    void QTBUG13877_crashOnExit();
};

class Pointer
{
public:
    static int count;
    inline Pointer() { ++count; }
    inline ~Pointer() { --count; }
};
int Pointer::count = 0;

tst_QThreadStorage::tst_QThreadStorage()

{ }

void tst_QThreadStorage::hasLocalData()
{
    QThreadStorage<Pointer *> pointers;
    QVERIFY(!pointers.hasLocalData());
    pointers.setLocalData(new Pointer);
    QVERIFY(pointers.hasLocalData());
    pointers.setLocalData(0);
    QVERIFY(!pointers.hasLocalData());
}

void tst_QThreadStorage::localData()
{
    QThreadStorage<Pointer*> pointers;
    Pointer *p = new Pointer;
    QVERIFY(!pointers.hasLocalData());
    pointers.setLocalData(p);
    QVERIFY(pointers.hasLocalData());
    QCOMPARE(pointers.localData(), p);
    pointers.setLocalData(0);
    QCOMPARE(pointers.localData(), (Pointer *)0);
    QVERIFY(!pointers.hasLocalData());
}

void tst_QThreadStorage::localData_const()
{
    QThreadStorage<Pointer *> pointers;
    const QThreadStorage<Pointer *> &const_pointers = pointers;
    Pointer *p = new Pointer;
    QVERIFY(!pointers.hasLocalData());
    pointers.setLocalData(p);
    QVERIFY(pointers.hasLocalData());
    QCOMPARE(const_pointers.localData(), p);
    pointers.setLocalData(0);
    QCOMPARE(const_pointers.localData(), (Pointer *)0);
    QVERIFY(!pointers.hasLocalData());
}

void tst_QThreadStorage::setLocalData()
{
    QThreadStorage<Pointer *> pointers;
    QVERIFY(!pointers.hasLocalData());
    pointers.setLocalData(new Pointer);
    QVERIFY(pointers.hasLocalData());
    pointers.setLocalData(0);
    QVERIFY(!pointers.hasLocalData());
}

class Thread : public QThread
{
public:
    QThreadStorage<Pointer *> &pointers;

    QMutex mutex;
    QWaitCondition cond;

    Thread(QThreadStorage<Pointer *> &p)
        : pointers(p)
    { }

    void run()
    {
        pointers.setLocalData(new Pointer);

        QMutexLocker locker(&mutex);
        cond.wakeOne();
        cond.wait(&mutex);
    }
};

void tst_QThreadStorage::autoDelete()
{
    QThreadStorage<Pointer *> pointers;
    QVERIFY(!pointers.hasLocalData());

    Thread thread(pointers);
    int c = Pointer::count;
    {
        QMutexLocker locker(&thread.mutex);
        thread.start();
        thread.cond.wait(&thread.mutex);
        // QCOMPARE(Pointer::count, c + 1);
        thread.cond.wakeOne();
    }
    thread.wait();
    QCOMPARE(Pointer::count, c);
}

bool threadStorageOk;
void testAdoptedThreadStorageWin(void *p)
{
    QThreadStorage<Pointer *>  *pointers = reinterpret_cast<QThreadStorage<Pointer *> *>(p);
    if (pointers->hasLocalData()) {
        threadStorageOk = false;
        return;
    }

    Pointer *pointer = new Pointer();
    pointers->setLocalData(pointer);

    if (pointers->hasLocalData() == false) {
        threadStorageOk = false;
        return;
    }

    if (pointers->localData() != pointer) {
        threadStorageOk = false;
        return;
    }
    QObject::connect(QThread::currentThread(), SIGNAL(finished()), &QTestEventLoop::instance(), SLOT(exitLoop()));
}
void *testAdoptedThreadStorageUnix(void *pointers)
{
    testAdoptedThreadStorageWin(pointers);
    return 0;
}
void tst_QThreadStorage::adoptedThreads()
{
    QTestEventLoop::instance(); // Make sure the instance is created in this thread.
    QThreadStorage<Pointer *> pointers;
    int c = Pointer::count;
    threadStorageOk = true;
    {
#ifdef Q_OS_UNIX
        pthread_t thread;
        const int state = pthread_create(&thread, 0, testAdoptedThreadStorageUnix, &pointers);
        QCOMPARE(state, 0);
        pthread_join(thread, 0);
#elif defined Q_OS_WIN
        HANDLE thread;
#if defined(Q_OS_WINCE)
        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)testAdoptedThreadStorageWin, &pointers, 0, NULL);
#else
        thread = (HANDLE)_beginthread(testAdoptedThreadStorageWin, 0, &pointers);
#endif
        QVERIFY(thread);
        WaitForSingleObject(thread, INFINITE);
#endif
    }
    QVERIFY(threadStorageOk);

    QTestEventLoop::instance().enterLoop(2);
    QVERIFY(!QTestEventLoop::instance().timeout());

    QCOMPARE(Pointer::count, c);
}

QBasicAtomicInt cleanupOrder = Q_BASIC_ATOMIC_INITIALIZER(0);

class First
{
public:
    ~First()
    {
        order = cleanupOrder.fetchAndAddRelaxed(1);
    }
    static int order;
};
int First::order = -1;

class Second
{
public:
    ~Second()
    {
        order = cleanupOrder.fetchAndAddRelaxed(1);
    }
    static int order;
};
int Second::order = -1;

void tst_QThreadStorage::ensureCleanupOrder()
{
    class Thread : public QThread
    {
    public:
        QThreadStorage<First *> &first;
        QThreadStorage<Second *> &second;

        Thread(QThreadStorage<First *> &first,
               QThreadStorage<Second *> &second)
            : first(first), second(second)
        { }

        void run()
        {
            // set in reverse order, but shouldn't matter, the data
            // will be deleted in the order the thread storage objects
            // were created
            second.setLocalData(new Second);
            first.setLocalData(new First);
        }
    };

    QThreadStorage<Second *> second;
    QThreadStorage<First *> first;
    Thread thread(first, second);
    thread.start();
    thread.wait();

    QVERIFY(First::order < Second::order);
}

void tst_QThreadStorage::QTBUG13877_crashOnExit()
{
    QProcess process;
#ifdef Q_OS_WIN
# ifdef QT_NO_DEBUG
    process.start("release/crashOnExit");
# else
    process.start("debug/crashOnExit");
# endif
#else
    process.start("./crashOnExit");
#endif
    QVERIFY(process.waitForFinished());
    QVERIFY(process.exitStatus() != QProcess::CrashExit);
}

QTEST_MAIN(tst_QThreadStorage)
#include "tst_qthreadstorage.moc"
