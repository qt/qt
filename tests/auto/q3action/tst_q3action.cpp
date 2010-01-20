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

#include <q3action.h>

QT_USE_NAMESPACE

//TESTED_CLASS=
//TESTED_FILES=


class tst_Q3Action : public QObject
{
    Q_OBJECT

public:
    tst_Q3Action();
    virtual ~tst_Q3Action();

private slots:
    void getSetCheck();
    void setText_data();
    void setText();
    void toolTip();
};

// Testing get/set functions
void tst_Q3Action::getSetCheck()
{
    Q3ActionGroup obj1(0);
    // bool Q3ActionGroup::usesDropDown()
    // void Q3ActionGroup::setUsesDropDown(bool)
    obj1.setUsesDropDown(false);
    QCOMPARE(false, obj1.usesDropDown());
    obj1.setUsesDropDown(true);
    QCOMPARE(true, obj1.usesDropDown());
}



tst_Q3Action::tst_Q3Action()
{
}

tst_Q3Action::~tst_Q3Action()
{

}

void tst_Q3Action::setText_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("menuText");
    QTest::addColumn<QString>("toolTip");
    QTest::addColumn<QString>("statusTip");
    QTest::addColumn<QString>("whatsThis");

    //next we fill it with data
    QTest::newRow("Normal") << "Action" << "Action" << "Action" << "Action" << QString();
    QTest::newRow("Ampersand") << "Search & Destroy" << "Search && Destroy" << "Search & Destroy" << "Search & Destroy" << QString();
}

void tst_Q3Action::setText()
{
    QFETCH(QString,text);
    QFETCH(QString,menuText);
    QFETCH(QString,toolTip);
    QFETCH(QString,statusTip);
    QFETCH(QString,whatsThis);

    Q3Action action(0);
    action.setText(text);
    QCOMPARE(action.menuText(), menuText);
    QCOMPARE(action.toolTip(), toolTip);
    QCOMPARE(action.statusTip(), statusTip);
    QCOMPARE(action.whatsThis(), whatsThis);
}

void tst_Q3Action::toolTip()
{
    QWidget widget;
    Q3Action action(&widget);
    action.setAccel(QKeySequence(Qt::CTRL | Qt::Key_A));
#ifdef Q_WS_MAC
    // MacCtrl copied from qkeysequence.cpp
    const QString MacCtrl = QString(QChar(0x2318));
    QCOMPARE(action.toolTip(), QString(QString(" (") + MacCtrl + QString("A)")));
#else
    QCOMPARE(action.toolTip(), QString(" (Ctrl+A)"));
#endif

    
}

QTEST_MAIN(tst_Q3Action)
#include "tst_q3action.moc"

