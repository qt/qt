/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtCore>
#include <QtTest/QtTest>

enum { OneMinute = 60 * 1000, TwoMinutes = OneMinute * 2 };

class tst_QObjectRace: public QObject
{
    Q_OBJECT
private slots:
    void moveToThreadRace();
};

class RaceObject : public QObject
{
    Q_OBJECT
    QList<QThread *> threads;
    int count;

public:
    RaceObject()
        : count(0)
    { }

    void addThread(QThread *thread)
    { threads.append(thread); }

public slots:
    void theSlot()
    {
        enum { step = 1000 };
        if ((++count % step) == 0) {
            QThread *nextThread = threads.at((count / step) % threads.size());
            moveToThread(nextThread);
        }
    }
};

class RaceThread : public QThread
{
    Q_OBJECT
    RaceObject *object;
    QTime stopWatch;

public:
    RaceThread()
        : object(0)
    { }

    void setObject(RaceObject *o)
    {
        object = o;
        object->addThread(this);
    }

    void start() {
        stopWatch.start();
        QThread::start();
    }

    void run() {
        QTimer zeroTimer;
        connect(&zeroTimer, SIGNAL(timeout()), object, SLOT(theSlot()));
        connect(&zeroTimer, SIGNAL(timeout()), this, SLOT(checkStopWatch()), Qt::DirectConnection);
        zeroTimer.start(0);
        (void) exec();
    }

signals:
    void theSignal();

private slots:
    void checkStopWatch()
    {
#ifndef Q_OS_WINCE
        if (stopWatch.elapsed() >= OneMinute)
#else
        if (stopWatch.elapsed() >= OneMinute / 2)
#endif
            quit();
    }
};

void tst_QObjectRace::moveToThreadRace()
{
    RaceObject *object = new RaceObject;

    enum { ThreadCount = 10 };
    RaceThread *threads[ThreadCount];
    for (int i = 0; i < ThreadCount; ++i) {
        threads[i] = new RaceThread;
        threads[i]->setObject(object);
    }

    object->moveToThread(threads[0]);

    for (int i = 0; i < ThreadCount; ++i)
        threads[i]->start();
    QVERIFY(threads[0]->wait(TwoMinutes));
    // the other threads should finish pretty quickly now
    for (int i = 1; i < ThreadCount; ++i)
        QVERIFY(threads[i]->wait(30000));

    for (int i = 0; i < ThreadCount; ++i)
        delete threads[i];
    delete object;
}

QTEST_MAIN(tst_QObjectRace)
#include "tst_qobjectrace.moc"
