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
#include <private/qlistmodelinterface_p.h>
#include <qdeclarativeview.h>
#include <qdeclarativeengine.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativepositioners_p.h>
#include <private/qdeclarativetransition_p.h>
#include <qdeclarativeexpression.h>
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QDeclarativePositioners : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativePositioners();

private slots:
    void test_horizontal();
    void test_horizontal_spacing();
    void test_horizontal_animated();
    void test_vertical();
    void test_vertical_spacing();
    void test_vertical_animated();
    void test_grid();
    void test_grid_topToBottom();
    void test_grid_spacing();
    void test_grid_animated();
    void test_grid_zero_columns();
    void test_propertychanges();
    void test_repeater();
    void test_flow();
    void test_flow_resize();
    void test_flow_implicit_resize();
    void test_conflictinganchors();
private:
    QDeclarativeView *createView(const QString &filename);
};

tst_QDeclarativePositioners::tst_QDeclarativePositioners()
{
}

void tst_QDeclarativePositioners::test_horizontal()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontal.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);

    QDeclarativeItem *row = canvas->rootObject()->findChild<QDeclarativeItem*>("row");
    QCOMPARE(row->width(), 110.0);
    QCOMPARE(row->height(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_horizontal_spacing()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontal-spacing.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 60.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 90.0);
    QCOMPARE(three->y(), 0.0);

    QDeclarativeItem *row = canvas->rootObject()->findChild<QDeclarativeItem*>("row");
    QCOMPARE(row->width(), 130.0);
    QCOMPARE(row->height(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_horizontal_animated()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/horizontal-animated.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    //Note that they animate in
    QCOMPARE(one->x(), -100.0);
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(three->x(), -100.0);

    QDeclarativeItem *row = canvas->rootObject()->findChild<QDeclarativeItem*>("row");
    QVERIFY(row);
    QCOMPARE(row->width(), 100.0);
    QCOMPARE(row->height(), 50.0);

    //QTRY_COMPARE used instead of waiting for the expected time of animation completion
    //Note that this means the duration of the animation is NOT tested

    QTRY_COMPARE(one->x(), 0.0);
    QTRY_COMPARE(one->y(), 0.0);
    QTRY_COMPARE(two->opacity(), 0.0);
    QTRY_COMPARE(two->x(), -100.0);//Not 'in' yet
    QTRY_COMPARE(two->y(), 0.0);
    QTRY_COMPARE(three->x(), 50.0);
    QTRY_COMPARE(three->y(), 0.0);

    //Add 'two'
    two->setOpacity(1.0);
    QCOMPARE(two->opacity(), 1.0);

    // New size should be immediate
    QCOMPARE(row->width(), 150.0);
    QCOMPARE(row->height(), 50.0);

    QTest::qWait(0);//Let the animation start
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(three->x(), 50.0);

    QTRY_COMPARE(two->x(), 50.0);
    QTRY_COMPARE(three->x(), 100.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_vertical()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/vertical.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 50.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 60.0);

    QDeclarativeItem *column = canvas->rootObject()->findChild<QDeclarativeItem*>("column");
    QVERIFY(column);
    QCOMPARE(column->height(), 80.0);
    QCOMPARE(column->width(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_vertical_spacing()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/vertical-spacing.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 60.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 80.0);

    QDeclarativeItem *column = canvas->rootObject()->findChild<QDeclarativeItem*>("column");
    QCOMPARE(column->height(), 100.0);
    QCOMPARE(column->width(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_vertical_animated()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/vertical-animated.qml");

    //Note that they animate in
    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QCOMPARE(one->y(), -100.0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QCOMPARE(two->y(), -100.0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QCOMPARE(three->y(), -100.0);

    QDeclarativeItem *column = canvas->rootObject()->findChild<QDeclarativeItem*>("column");
    QVERIFY(column);
    QCOMPARE(column->height(), 100.0);
    QCOMPARE(column->width(), 50.0);

    //QTRY_COMPARE used instead of waiting for the expected time of animation completion
    //Note that this means the duration of the animation is NOT tested

    QTRY_COMPARE(one->y(), 0.0);
    QTRY_COMPARE(one->x(), 0.0);
    QTRY_COMPARE(two->opacity(), 0.0);
    QTRY_COMPARE(two->y(), -100.0);//Not 'in' yet
    QTRY_COMPARE(two->x(), 0.0);
    QTRY_COMPARE(three->y(), 50.0);
    QTRY_COMPARE(three->x(), 0.0);

    //Add 'two'
    two->setOpacity(1.0);
    QTRY_COMPARE(two->opacity(), 1.0);
    QCOMPARE(column->height(), 150.0);
    QCOMPARE(column->width(), 50.0);
    QTest::qWait(0);//Let the animation start
    QCOMPARE(two->y(), -100.0);
    QCOMPARE(three->y(), 50.0);

    QTRY_COMPARE(two->y(), 50.0);
    QTRY_COMPARE(three->y(), 100.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_grid()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/gridtest.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 50.0);
    QCOMPARE(five->x(), 50.0);
    QCOMPARE(five->y(), 50.0);

    QDeclarativeItem *grid = canvas->rootObject()->findChild<QDeclarativeItem*>("grid");
    QCOMPARE(grid->width(), 120.0);
    QCOMPARE(grid->height(), 100.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_grid_topToBottom()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/grid-toptobottom.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 50.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 100.0);
    QCOMPARE(four->x(), 50.0);
    QCOMPARE(four->y(), 0.0);
    QCOMPARE(five->x(), 50.0);
    QCOMPARE(five->y(), 50.0);

    QDeclarativeItem *grid = canvas->rootObject()->findChild<QDeclarativeItem*>("grid");
    QCOMPARE(grid->width(), 100.0);
    QCOMPARE(grid->height(), 120.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_grid_spacing()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/grid-spacing.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 54.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 78.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 54.0);
    QCOMPARE(five->x(), 54.0);
    QCOMPARE(five->y(), 54.0);

    QDeclarativeItem *grid = canvas->rootObject()->findChild<QDeclarativeItem*>("grid");
    QCOMPARE(grid->width(), 128.0);
    QCOMPARE(grid->height(), 104.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_grid_animated()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/grid-animated.qml");

    //Note that all animate in
    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QCOMPARE(one->x(), -100.0);
    QCOMPARE(one->y(), -100.0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(two->y(), -100.0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QCOMPARE(three->x(), -100.0);
    QCOMPARE(three->y(), -100.0);

    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QCOMPARE(four->x(), -100.0);
    QCOMPARE(four->y(), -100.0);

    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);
    QCOMPARE(five->x(), -100.0);
    QCOMPARE(five->y(), -100.0);

    QDeclarativeItem *grid = canvas->rootObject()->findChild<QDeclarativeItem*>("grid");
    QVERIFY(grid);
    QCOMPARE(grid->width(), 150.0);
    QCOMPARE(grid->height(), 100.0);

    //QTRY_COMPARE used instead of waiting for the expected time of animation completion
    //Note that this means the duration of the animation is NOT tested

    QTRY_COMPARE(one->y(), 0.0);
    QTRY_COMPARE(one->x(), 0.0);
    QTRY_COMPARE(two->opacity(), 0.0);
    QTRY_COMPARE(two->y(), -100.0);
    QTRY_COMPARE(two->x(), -100.0);
    QTRY_COMPARE(three->y(), 0.0);
    QTRY_COMPARE(three->x(), 50.0);
    QTRY_COMPARE(four->y(), 0.0);
    QTRY_COMPARE(four->x(), 100.0);
    QTRY_COMPARE(five->y(), 50.0);
    QTRY_COMPARE(five->x(), 0.0);

    //Add 'two'
    two->setOpacity(1.0);
    QCOMPARE(two->opacity(), 1.0);
    QCOMPARE(grid->width(), 150.0);
    QCOMPARE(grid->height(), 100.0);
    QTest::qWait(0);//Let the animation start
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(two->y(), -100.0);
    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(three->x(), 50.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 100.0);
    QCOMPARE(four->y(), 0.0);
    QCOMPARE(five->x(), 0.0);
    QCOMPARE(five->y(), 50.0);
    //Let the animation complete
    QTRY_COMPARE(two->x(), 50.0);
    QTRY_COMPARE(two->y(), 0.0);
    QTRY_COMPARE(one->x(), 0.0);
    QTRY_COMPARE(one->y(), 0.0);
    QTRY_COMPARE(three->x(), 100.0);
    QTRY_COMPARE(three->y(), 0.0);
    QTRY_COMPARE(four->x(), 0.0);
    QTRY_COMPARE(four->y(), 50.0);
    QTRY_COMPARE(five->x(), 50.0);
    QTRY_COMPARE(five->y(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_grid_zero_columns()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/gridzerocolumns.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 120.0);
    QCOMPARE(four->y(), 0.0);
    QCOMPARE(five->x(), 0.0);
    QCOMPARE(five->y(), 50.0);

    QDeclarativeItem *grid = canvas->rootObject()->findChild<QDeclarativeItem*>("grid");
    QCOMPARE(grid->width(), 170.0);
    QCOMPARE(grid->height(), 60.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_propertychanges()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/propertychangestest.qml");

    QDeclarativeGrid *grid = qobject_cast<QDeclarativeGrid*>(canvas->rootObject());
    QVERIFY(grid != 0);
    QDeclarativeTransition *rowTransition = canvas->rootObject()->findChild<QDeclarativeTransition*>("rowTransition");
    QDeclarativeTransition *columnTransition = canvas->rootObject()->findChild<QDeclarativeTransition*>("columnTransition");

    QSignalSpy addSpy(grid, SIGNAL(addChanged()));
    QSignalSpy moveSpy(grid, SIGNAL(moveChanged()));
    QSignalSpy columnsSpy(grid, SIGNAL(columnsChanged()));
    QSignalSpy rowsSpy(grid, SIGNAL(rowsChanged()));

    QVERIFY(grid);
    QVERIFY(rowTransition);
    QVERIFY(columnTransition);
    QCOMPARE(grid->add(), columnTransition);
    QCOMPARE(grid->move(), columnTransition);
    QCOMPARE(grid->columns(), 4);
    QCOMPARE(grid->rows(), -1);

    grid->setAdd(rowTransition);
    grid->setMove(rowTransition);
    QCOMPARE(grid->add(), rowTransition);
    QCOMPARE(grid->move(), rowTransition);
    QCOMPARE(addSpy.count(),1);
    QCOMPARE(moveSpy.count(),1);

    grid->setAdd(rowTransition);
    grid->setMove(rowTransition);
    QCOMPARE(addSpy.count(),1);
    QCOMPARE(moveSpy.count(),1);

    grid->setAdd(0);
    grid->setMove(0);
    QCOMPARE(addSpy.count(),2);
    QCOMPARE(moveSpy.count(),2);

    grid->setColumns(-1);
    grid->setRows(3);
    QCOMPARE(grid->columns(), -1);
    QCOMPARE(grid->rows(), 3);
    QCOMPARE(columnsSpy.count(),1);
    QCOMPARE(rowsSpy.count(),1);

    grid->setColumns(-1);
    grid->setRows(3);
    QCOMPARE(columnsSpy.count(),1);
    QCOMPARE(rowsSpy.count(),1);

    grid->setColumns(2);
    grid->setRows(2);
    QCOMPARE(columnsSpy.count(),2);
    QCOMPARE(rowsSpy.count(),2);

    delete canvas;
}

void tst_QDeclarativePositioners::test_repeater()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/repeatertest.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);

    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);

    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 100.0);
    QCOMPARE(three->y(), 0.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_flow()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/flowtest.qml");

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 50.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 70.0);
    QCOMPARE(five->x(), 50.0);
    QCOMPARE(five->y(), 70.0);

    QDeclarativeItem *flow = canvas->rootObject()->findChild<QDeclarativeItem*>("flow");
    QVERIFY(flow);
    QCOMPARE(flow->width(), 90.0);
    QCOMPARE(flow->height(), 120.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_flow_resize()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/flowtest.qml");

    QDeclarativeItem *root = qobject_cast<QDeclarativeItem*>(canvas->rootObject());
    QVERIFY(root);
    root->setWidth(125);

    QDeclarativeRectangle *one = canvas->rootObject()->findChild<QDeclarativeRectangle*>("one");
    QVERIFY(one != 0);
    QDeclarativeRectangle *two = canvas->rootObject()->findChild<QDeclarativeRectangle*>("two");
    QVERIFY(two != 0);
    QDeclarativeRectangle *three = canvas->rootObject()->findChild<QDeclarativeRectangle*>("three");
    QVERIFY(three != 0);
    QDeclarativeRectangle *four = canvas->rootObject()->findChild<QDeclarativeRectangle*>("four");
    QVERIFY(four != 0);
    QDeclarativeRectangle *five = canvas->rootObject()->findChild<QDeclarativeRectangle*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 50.0);
    QCOMPARE(five->x(), 50.0);
    QCOMPARE(five->y(), 50.0);

    delete canvas;
}

void tst_QDeclarativePositioners::test_flow_implicit_resize()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/flow-testimplicitsize.qml");
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeFlow *flow = canvas->rootObject()->findChild<QDeclarativeFlow*>("flow");
    QVERIFY(flow != 0);

    QCOMPARE(flow->width(), 100.0);
    QCOMPARE(flow->height(), 120.0);

    canvas->rootObject()->setProperty("leftToRight", true);
    QCOMPARE(flow->width(), 220.0);
    QCOMPARE(flow->height(), 50.0);

    canvas->rootObject()->setProperty("leftToRight", false);
    QCOMPARE(flow->width(), 100.0);
    QCOMPARE(flow->height(), 120.0);

    delete canvas;
}

QString warningMessage;

void interceptWarnings(QtMsgType type, const char *msg)
{
    Q_UNUSED( type );
    warningMessage = msg;
}

void tst_QDeclarativePositioners::test_conflictinganchors()
{
    qInstallMsgHandler(interceptWarnings);
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);

    component.setData("import Qt 4.7\nColumn { Item {} }", QUrl::fromLocalFile(""));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());

    component.setData("import Qt 4.7\nRow { Item {} }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());

    component.setData("import Qt 4.7\nGrid { Item {} }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());

    component.setData("import Qt 4.7\nFlow { Item {} }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());

    component.setData("import Qt 4.7\nColumn { Item { anchors.top: parent.top } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Column: Cannot specify top, bottom, verticalCenter, fill or centerIn anchors for items inside Column"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nColumn { Item { anchors.centerIn: parent } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Column: Cannot specify top, bottom, verticalCenter, fill or centerIn anchors for items inside Column"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nColumn { Item { anchors.left: parent.left } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());
    warningMessage.clear();

    component.setData("import Qt 4.7\nRow { Item { anchors.left: parent.left } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Row: Cannot specify left, right, horizontalCenter, fill or centerIn anchors for items inside Row"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nRow { Item { anchors.fill: parent } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Row: Cannot specify left, right, horizontalCenter, fill or centerIn anchors for items inside Row"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nRow { Item { anchors.top: parent.top } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QVERIFY(warningMessage.isEmpty());
    warningMessage.clear();

    component.setData("import Qt 4.7\nGrid { Item { anchors.horizontalCenter: parent.horizontalCenter } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Grid: Cannot specify anchors for items inside Grid"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nGrid { Item { anchors.centerIn: parent } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Grid: Cannot specify anchors for items inside Grid"));
    warningMessage.clear();

    component.setData("import Qt 4.7\nFlow { Item { anchors.verticalCenter: parent.verticalCenter } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Flow: Cannot specify anchors for items inside Flow"));

    component.setData("import Qt 4.7\nFlow { Item { anchors.fill: parent } }", QUrl::fromLocalFile(""));
    item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(warningMessage, QString("file::2:1: QML Flow: Cannot specify anchors for items inside Flow"));
}

QDeclarativeView *tst_QDeclarativePositioners::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}


QTEST_MAIN(tst_QDeclarativePositioners)

#include "tst_qdeclarativepositioners.moc"
