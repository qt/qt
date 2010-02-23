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
#include <private/qlistmodelinterface_p.h>
#include <qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlgraphicspositioners_p.h>
#include <private/qmltransition_p.h>
#include <qmlexpression.h>
#include "../../../shared/util.h"

class tst_QmlGraphicsPositioners : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsPositioners();

private slots:
    void test_horizontal();
    void test_horizontal_spacing();
    void test_horizontal_animated();
    void test_vertical();
    void test_vertical_spacing();
    void test_vertical_animated();
    void test_grid();
    void test_grid_spacing();
    void test_grid_animated();
    void test_propertychanges();
    void test_repeater();
private:
    QmlView *createView(const QString &filename);
};

tst_QmlGraphicsPositioners::tst_QmlGraphicsPositioners()
{
}

void tst_QmlGraphicsPositioners::test_horizontal()
{
    QmlView *canvas = createView(SRCDIR "/data/horizontal.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
}

void tst_QmlGraphicsPositioners::test_horizontal_spacing()
{
    QmlView *canvas = createView(SRCDIR "/data/horizontal-spacing.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 60.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 90.0);
    QCOMPARE(three->y(), 0.0);
}

void tst_QmlGraphicsPositioners::test_horizontal_animated()
{
    QmlView *canvas = createView(SRCDIR "/data/horizontal-animated.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    //Note that they animate in
    QCOMPARE(one->x(), -100.0);
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(three->x(), -100.0);

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
    QTest::qWait(0);//Let the animation start
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(three->x(), 50.0);

    QTRY_COMPARE(two->x(), 50.0);
    QTRY_COMPARE(three->x(), 100.0);
}

void tst_QmlGraphicsPositioners::test_vertical()
{
    QmlView *canvas = createView(SRCDIR "/data/vertical.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 50.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 60.0);
}

void tst_QmlGraphicsPositioners::test_vertical_spacing()
{
    QmlView *canvas = createView(SRCDIR "/data/vertical-spacing.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 60.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 80.0);
}

void tst_QmlGraphicsPositioners::test_vertical_animated()
{
    QmlView *canvas = createView(SRCDIR "/data/vertical-animated.qml");

    //Note that they animate in
    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);
    QCOMPARE(one->y(), -100.0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);
    QCOMPARE(two->y(), -100.0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);
    QCOMPARE(three->y(), -100.0);

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
    QTest::qWait(0);//Let the animation start
    QCOMPARE(two->y(), -100.0);
    QCOMPARE(three->y(), 50.0);

    QTRY_COMPARE(two->y(), 50.0);
    QTRY_COMPARE(three->y(), 100.0);

}

void tst_QmlGraphicsPositioners::test_grid()
{
    QmlView *canvas = createView("data/grid.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);
    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);
    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);
    QmlGraphicsRectangle *four = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("four");
    QVERIFY(four != 0);
    QmlGraphicsRectangle *five = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("five");
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
}

void tst_QmlGraphicsPositioners::test_grid_spacing()
{
    QmlView *canvas = createView("data/grid-spacing.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);
    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);
    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);
    QmlGraphicsRectangle *four = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("four");
    QVERIFY(four != 0);
    QmlGraphicsRectangle *five = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("five");
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
}

void tst_QmlGraphicsPositioners::test_grid_animated()
{
    QmlView *canvas = createView(SRCDIR "/data/grid-animated.qml");

    //Note that all animate in
    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);
    QCOMPARE(one->x(), -100.0);
    QCOMPARE(one->y(), -100.0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);
    QCOMPARE(two->x(), -100.0);
    QCOMPARE(two->y(), -100.0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);
    QCOMPARE(three->x(), -100.0);
    QCOMPARE(three->y(), -100.0);

    QmlGraphicsRectangle *four = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("four");
    QVERIFY(four != 0);
    QCOMPARE(four->x(), -100.0);
    QCOMPARE(four->y(), -100.0);

    QmlGraphicsRectangle *five = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("five");
    QVERIFY(five != 0);
    QCOMPARE(five->x(), -100.0);
    QCOMPARE(five->y(), -100.0);

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

}
void tst_QmlGraphicsPositioners::test_propertychanges()
{
    QmlView *canvas = createView("data/propertychanges.qml");

    QmlGraphicsGrid *grid = qobject_cast<QmlGraphicsGrid*>(canvas->rootObject());
    QmlTransition *rowTransition = canvas->rootObject()->findChild<QmlTransition*>("rowTransition");
    QmlTransition *columnTransition = canvas->rootObject()->findChild<QmlTransition*>("columnTransition");

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
}

void tst_QmlGraphicsPositioners::test_repeater()
{
    QmlView *canvas = createView("data/repeater.qml");

    QmlGraphicsRectangle *one = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("one");
    QVERIFY(one != 0);

    QmlGraphicsRectangle *two = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("two");
    QVERIFY(two != 0);

    QmlGraphicsRectangle *three = canvas->rootObject()->findChild<QmlGraphicsRectangle*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 100.0);
    QCOMPARE(three->y(), 0.0);
}

QmlView *tst_QmlGraphicsPositioners::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}


QTEST_MAIN(tst_QmlGraphicsPositioners)

#include "tst_qmlgraphicspositioners.moc"
