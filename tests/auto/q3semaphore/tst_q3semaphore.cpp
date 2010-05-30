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


#include <QtTest/QtTest>
#include <qthread.h>
#include <q3semaphore.h>
#include "q3semaphore.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3Semaphore : public QObject
{
    Q_OBJECT

public:
    tst_Q3Semaphore();
    virtual ~tst_Q3Semaphore();

public slots:
    void init();
    void cleanup();
private slots:
    void incrementOne();
    void incrementN();
};

Q3Semaphore *semaphore = 0;

tst_Q3Semaphore::tst_Q3Semaphore()
{

}

tst_Q3Semaphore::~tst_Q3Semaphore()
{

}

// init() will be executed immediately before each testfunction is run.
void tst_Q3Semaphore::init()
{
}

// cleanup() will be executed immediately after each testfunction is run.
void tst_Q3Semaphore::cleanup()
{
}

class ThreadOne : public QThread
{
public:
    ThreadOne() {}

protected:
    void run()
    {
	int i = 0;
	while ( i < 100 ) {
	    (*semaphore)++;
	    i++;
	    (*semaphore)--;
	}
    }
};

void tst_Q3Semaphore::incrementOne()
{
    QVERIFY(!semaphore);
    semaphore = new Q3Semaphore(1);

    ThreadOne t1;
    ThreadOne t2;

    t1.start();
    t2.start();

    QVERIFY(t1.wait(4000));
    QVERIFY(t2.wait(4000));

    delete semaphore;
    semaphore = 0;
}

class ThreadN : public QThread
{
    int N;

 public:
    ThreadN(int n) :N(n) { }

protected:
    void run()
    {
	int i = 0;
	while ( i < 100 ) {
	    (*semaphore)+=N;
	    i++;
	    (*semaphore)-=N;
	}
    }
};

void tst_Q3Semaphore::incrementN()
{
    QVERIFY(!semaphore);
    semaphore = new Q3Semaphore(4);

    ThreadN t1(2);
    ThreadN t2(3);

    t1.start();
    t2.start();

    QVERIFY(t1.wait(4000));
    QVERIFY(t2.wait(4000));

    delete semaphore;
    semaphore = 0;
}

QTEST_MAIN(tst_Q3Semaphore)
#include "tst_q3semaphore.moc"
