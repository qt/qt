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
#include <q3datetimeedit.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3DateTimeEdit : public QObject
{
Q_OBJECT

public:
    tst_Q3DateTimeEdit();
    virtual ~tst_Q3DateTimeEdit();

private slots:
    void getSetCheck();
};

tst_Q3DateTimeEdit::tst_Q3DateTimeEdit()
{
}

tst_Q3DateTimeEdit::~tst_Q3DateTimeEdit()
{
}

// Testing get/set functions
void tst_Q3DateTimeEdit::getSetCheck()
{
    Q3TimeEdit obj1;
    // uint Q3TimeEdit::display()
    // void Q3TimeEdit::setDisplay(uint)
    obj1.setDisplay(Q3TimeEdit::Hours);
    QCOMPARE(uint(Q3TimeEdit::Hours), obj1.display());
    obj1.setDisplay(Q3TimeEdit::Minutes);
    QCOMPARE(uint(Q3TimeEdit::Minutes), obj1.display());
    obj1.setDisplay(Q3TimeEdit::Seconds);
    QCOMPARE(uint(Q3TimeEdit::Seconds), obj1.display());
    obj1.setDisplay(Q3TimeEdit::AMPM);
    QCOMPARE(uint(Q3TimeEdit::AMPM), obj1.display());
}

QTEST_MAIN(tst_Q3DateTimeEdit)
#include "tst_q3datetimeedit.moc"
