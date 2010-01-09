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


#include <QApplication>
#include <QGroupBox>
#include <Q3GroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QtTest/QtTest>

class tst_q3groupbox : public QObject
{
Q_OBJECT
private slots:
    void getSetCheck();
    void groupBoxHeight();
};

// Testing get/set functions
void tst_q3groupbox::getSetCheck()
{
    Q3GroupBox obj1;
    // int Q3GroupBox::insideMargin()
    // void Q3GroupBox::setInsideMargin(int)
    obj1.setInsideMargin(0);
    QCOMPARE(0, obj1.insideMargin());
    obj1.setInsideMargin(INT_MIN);
    QCOMPARE(INT_MIN, obj1.insideMargin());
    obj1.setInsideMargin(INT_MAX);
    QCOMPARE(INT_MAX, obj1.insideMargin());

    // int Q3GroupBox::insideSpacing()
    // void Q3GroupBox::setInsideSpacing(int)
    obj1.setInsideSpacing(0);
    QCOMPARE(0, obj1.insideSpacing());
    obj1.setInsideSpacing(INT_MIN);
    QCOMPARE(INT_MIN, obj1.insideSpacing());
    obj1.setInsideSpacing(INT_MAX);
    QCOMPARE(INT_MAX, obj1.insideSpacing());
}

/*
    Test that a Q3GroupBox has a reasonable height compared to a QGroupBox.
*/
void tst_q3groupbox::groupBoxHeight()
{
    QWidget w;

    // Create group boxes.
    Q3GroupBox * const g3 = new Q3GroupBox(1000, Qt::Vertical, "Q3 Group Box", &w);
    new QLabel("Row 1", g3);

    QGroupBox * const g4 = new QGroupBox(&w, "QGroupBox");
    g4->setTitle("QGroupBox");
    QVBoxLayout * const g4Layout = new QVBoxLayout(g4);
    g4Layout->addWidget(new QLabel("QT4 Row 1"));

    // Add them to a layout.
    QVBoxLayout * const layout = new QVBoxLayout(&w, 5, 5);
    layout->addWidget(g3);
    layout->addWidget(g4);
    layout->addWidget(new QLabel("Label at Bottom"));
    w.show();

    // Measure height and test.
    const int q3height = g3->height();
    const int q4height = g4->height();

    const double withinReason = 0.5; // Up to 50% off is OK.
    const int minimum = int(q4height * (1.0 - withinReason));
    const int maximum = int(q4height * (1.0 + withinReason));

    QVERIFY(q3height > minimum);
    QVERIFY(q3height < maximum);
}

QTEST_MAIN(tst_q3groupbox)
#include "tst_q3groupbox.moc"
