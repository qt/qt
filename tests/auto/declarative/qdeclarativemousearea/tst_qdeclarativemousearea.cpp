/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QSignalSpy>
#include <private/qdeclarativemousearea_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativecontext.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QDeclarativeMouseArea: public QObject
{
    Q_OBJECT
private slots:
    void dragProperties();
    void resetDrag();
    void dragging();
    void updateMouseAreaPosOnClick();
    void updateMouseAreaPosOnResize();
    void noOnClickedWithPressAndHold();
    void onMousePressRejected();

private:
    QDeclarativeView *createView();
};

void tst_QDeclarativeMouseArea::dragProperties()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragproperties.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeMouseArea *mouseRegion = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mouseregion");
    QDeclarativeDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QDeclarativeItem *blackRect = canvas->rootObject()->findChild<QDeclarativeItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(drag, SIGNAL(targetChanged()));
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);

    // axis
    QCOMPARE(drag->axis(), QDeclarativeDrag::XandYAxis);
    QSignalSpy axisSpy(drag, SIGNAL(axisChanged()));
    drag->setAxis(QDeclarativeDrag::XAxis);
    QCOMPARE(drag->axis(), QDeclarativeDrag::XAxis);
    QCOMPARE(axisSpy.count(),1);
    drag->setAxis(QDeclarativeDrag::XAxis);
    QCOMPARE(axisSpy.count(),1);

    // minimum and maximum properties
    QSignalSpy xminSpy(drag, SIGNAL(minimumXChanged()));
    QSignalSpy xmaxSpy(drag, SIGNAL(maximumXChanged()));
    QSignalSpy yminSpy(drag, SIGNAL(minimumYChanged()));
    QSignalSpy ymaxSpy(drag, SIGNAL(maximumYChanged()));

    QCOMPARE(drag->xmin(), 0.0);
    QCOMPARE(drag->xmax(), rootItem->width()-blackRect->width());
    QCOMPARE(drag->ymin(), 0.0);
    QCOMPARE(drag->ymax(), rootItem->height()-blackRect->height());

    drag->setXmin(10);
    drag->setXmax(10);
    drag->setYmin(10);
    drag->setYmax(10);

    QCOMPARE(drag->xmin(), 10.0);
    QCOMPARE(drag->xmax(), 10.0);
    QCOMPARE(drag->ymin(), 10.0);
    QCOMPARE(drag->ymax(), 10.0);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);

    drag->setXmin(10);
    drag->setXmax(10);
    drag->setYmin(10);
    drag->setYmax(10);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);

    // filterChildren
    QSignalSpy filterChildrenSpy(drag, SIGNAL(filterChildrenChanged()));

    drag->setFilterChildren(true);

    QVERIFY(drag->filterChildren());
    QCOMPARE(filterChildrenSpy.count(), 1);

    drag->setFilterChildren(true);
    QCOMPARE(filterChildrenSpy.count(), 1);

    delete canvas;
}

void tst_QDeclarativeMouseArea::resetDrag()
{
    QDeclarativeView *canvas = createView();

    canvas->rootContext()->setContextProperty("haveTarget", QVariant(true));
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragreset.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeMouseArea *mouseRegion = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mouseregion");
    QDeclarativeDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QDeclarativeItem *blackRect = canvas->rootObject()->findChild<QDeclarativeItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());
    QDeclarativeItem *rootItem = qobject_cast<QDeclarativeItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(drag, SIGNAL(targetChanged()));
    QVERIFY(drag->target() != 0);
    canvas->rootContext()->setContextProperty("haveTarget", QVariant(false));
    QCOMPARE(targetSpy.count(),1);
    QVERIFY(drag->target() == 0);

    delete canvas;
}


void tst_QDeclarativeMouseArea::dragging()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragging.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeMouseArea *mouseRegion = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mouseregion");
    QDeclarativeDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QDeclarativeItem *blackRect = canvas->rootObject()->findChild<QDeclarativeItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());

    QVERIFY(!drag->active());

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QVERIFY(!drag->active());
    QCOMPARE(blackRect->x(), 50.0);
    QCOMPARE(blackRect->y(), 50.0);

    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent.setScenePos(QPointF(110, 110));
    moveEvent.setButton(Qt::LeftButton);
    moveEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &moveEvent);

    QVERIFY(drag->active());
    QCOMPARE(blackRect->x(), 60.0);
    QCOMPARE(blackRect->y(), 60.0);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(110, 110));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QVERIFY(!drag->active());
    QCOMPARE(blackRect->x(), 60.0);
    QCOMPARE(blackRect->y(), 60.0);

    delete canvas;
}

QDeclarativeView *tst_QDeclarativeMouseArea::createView()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    return canvas;
}

void tst_QDeclarativeMouseArea::updateMouseAreaPosOnClick()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/updateMousePosOnClick.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeMouseArea *mouseRegion = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mouseregion");
    QVERIFY(mouseRegion != 0);

    QDeclarativeRectangle *rect = canvas->rootObject()->findChild<QDeclarativeRectangle*>("ball");
    QVERIFY(rect != 0);

    QCOMPARE(mouseRegion->mouseX(), rect->x());
    QCOMPARE(mouseRegion->mouseY(), rect->y());

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setScenePos(QPointF(100, 100));
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &event);

    QCOMPARE(mouseRegion->mouseX(), 100.0);
    QCOMPARE(mouseRegion->mouseY(), 100.0);

    QCOMPARE(mouseRegion->mouseX(), rect->x());
    QCOMPARE(mouseRegion->mouseY(), rect->y());

    delete canvas;
}

void tst_QDeclarativeMouseArea::updateMouseAreaPosOnResize()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/updateMousePosOnResize.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeMouseArea *mouseRegion = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mouseregion");
    QVERIFY(mouseRegion != 0);

    QDeclarativeRectangle *rect = canvas->rootObject()->findChild<QDeclarativeRectangle*>("brother");
    QVERIFY(rect != 0);

    QCOMPARE(mouseRegion->mouseX(), 0.0);
    QCOMPARE(mouseRegion->mouseY(), 0.0);

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setScenePos(rect->pos());
    event.setButton(Qt::LeftButton);
    event.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &event);

    QVERIFY(!mouseRegion->property("emitPositionChanged").toBool());
    QVERIFY(mouseRegion->property("mouseMatchesPos").toBool());

    QCOMPARE(mouseRegion->property("x1").toInt(), 0);
    QCOMPARE(mouseRegion->property("y1").toInt(), 0);

    // XXX: is it on purpose that mouseX is real and mouse.x is int?
    QCOMPARE(mouseRegion->property("x2").toInt(), (int) rect->x());
    QCOMPARE(mouseRegion->property("y2").toInt(), (int) rect->y());

    QCOMPARE(mouseRegion->mouseX(), rect->x());
    QCOMPARE(mouseRegion->mouseY(), rect->y());

    delete canvas;
}

void tst_QDeclarativeMouseArea::noOnClickedWithPressAndHold()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clickandhold.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QVERIFY(!canvas->rootObject()->property("clicked").toBool());
    QVERIFY(!canvas->rootObject()->property("held").toBool());

    QTest::qWait(1000);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(100, 100));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QVERIFY(!canvas->rootObject()->property("clicked").toBool());
    QVERIFY(canvas->rootObject()->property("held").toBool());
}

void tst_QDeclarativeMouseArea::onMousePressRejected()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/rejectEvent.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QVERIFY(!canvas->rootObject()->property("mr1_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_canceled").toBool());

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QVERIFY(canvas->rootObject()->property("mr1_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(canvas->rootObject()->property("mr2_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());
    QVERIFY(canvas->rootObject()->property("mr2_canceled").toBool());

    QTest::qWait(200);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(100, 100));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QVERIFY(canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());
}

QTEST_MAIN(tst_QDeclarativeMouseArea)

#include "tst_qdeclarativemousearea.moc"
