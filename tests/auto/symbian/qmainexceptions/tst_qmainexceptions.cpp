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

#include <QtTest/QtTest>

#ifdef Q_OS_SYMBIAN

class tst_qmainexceptions : public QObject
{
    Q_OBJECT
public:
    tst_qmainexceptions(){};
    ~tst_qmainexceptions(){};
public slots:
    void initTestCase();
private slots:
    void trap();
    void cleanupstack();
    void leave();
};

class CDummy : public CBase
{
public:
    CDummy(){}
    ~CDummy(){}
};

void tst_qmainexceptions::initTestCase()
{
}

void tst_qmainexceptions::trap()
{
    TTrapHandler *th= User::TrapHandler();
    QVERIFY((int)th);
}

void tst_qmainexceptions::cleanupstack()
{
    __UHEAP_MARK;
    //fails if OOM
    CDummy* dummy1 = new (ELeave) CDummy;
    __UHEAP_CHECK(1);
    CleanupStack::PushL(dummy1);
    CleanupStack::PopAndDestroy(dummy1);
    __UHEAP_MARKEND;
}
void tst_qmainexceptions::leave()
{
    __UHEAP_MARK;
    CDummy* dummy1 = 0;
    TRAPD(err,{
        CDummy* csDummy = new (ELeave) CDummy;
        CleanupStack::PushL(csDummy);
        __UHEAP_FAILNEXT(1);
        dummy1 = new (ELeave) CDummy;        
        //CleanupStack::PopAndDestroy(csDummy); not executed as previous line throws
    });
    QCOMPARE(err,KErrNoMemory);
    QVERIFY(!((int)dummy1));    
    __UHEAP_MARKEND;
}


QTEST_MAIN(tst_qmainexceptions)
#include "tst_qmainexceptions.moc"
#else
QTEST_NOOP_MAIN
#endif
