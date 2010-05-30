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

#include <qapplication.h>
#include <qdebug.h>
#include <q3progressdialog.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ProgressDialog : public QObject
{
Q_OBJECT

public:
    tst_Q3ProgressDialog();
    virtual ~tst_Q3ProgressDialog();

private slots:
    void getSetCheck();
};

tst_Q3ProgressDialog::tst_Q3ProgressDialog()
{
}

tst_Q3ProgressDialog::~tst_Q3ProgressDialog()
{
}

// Testing get/set functions
void tst_Q3ProgressDialog::getSetCheck()
{
    Q3ProgressDialog obj1;
    // bool Q3ProgressDialog::autoReset()
    // void Q3ProgressDialog::setAutoReset(bool)
    obj1.setAutoReset(false);
    QVERIFY(!obj1.autoReset());
    obj1.setAutoReset(true);
    QVERIFY(obj1.autoReset());

    // bool Q3ProgressDialog::autoClose()
    // void Q3ProgressDialog::setAutoClose(bool)
    obj1.setAutoClose(false);
    QVERIFY(!obj1.autoClose());
    obj1.setAutoClose(true);
    QVERIFY(obj1.autoClose());

    // int Q3ProgressDialog::totalSteps()
    // void Q3ProgressDialog::setTotalSteps(int)
    obj1.setTotalSteps(0);
    QCOMPARE(obj1.totalSteps(), 0);
    obj1.setTotalSteps(INT_MIN);
    QCOMPARE(obj1.totalSteps(), INT_MIN);
    obj1.setTotalSteps(INT_MAX);
    QCOMPARE(obj1.totalSteps(), INT_MAX);

    // int Q3ProgressDialog::minimumDuration()
    // void Q3ProgressDialog::setMinimumDuration(int)
    obj1.setMinimumDuration(0);
    QCOMPARE(obj1.minimumDuration(), 0);
    obj1.setMinimumDuration(INT_MIN);
    // It makes no sense with negative duration, but we cannot change this
    // behavior in a Qt3Support classs.
    QCOMPARE(obj1.minimumDuration(), INT_MIN);
    obj1.setMinimumDuration(INT_MAX);
    QCOMPARE(obj1.minimumDuration(), INT_MAX);
}

QTEST_MAIN(tst_Q3ProgressDialog)
#include "tst_q3progressdialog.moc"
