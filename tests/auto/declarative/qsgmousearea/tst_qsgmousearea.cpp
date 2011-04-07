/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QSignalSpy>
#include <private/qsgmousearea_p.h>
#include <private/qsgrectangle_p.h>
#include <private/qsgflickable_p.h>
#include <QtDeclarative/qsgview.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QSGMouseArea: public QObject
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
    void doubleClick();
    void clickTwice();
    void pressedOrdering();
    void preventStealing();
    void clickThrough();
    void testQtQuick11Attributes();
    void testQtQuick11Attributes_data();
    void hoverPosition();

private:
    QSGView *createView();
};

void tst_QSGMouseArea::dragProperties()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragproperties.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGMouseArea *mouseRegion = canvas->rootObject()->findChild<QSGMouseArea*>("mouseregion");
    QSGDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());
    QSGItem *rootItem = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(drag, SIGNAL(targetChanged()));
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);

    // axis
    QCOMPARE(drag->axis(), QSGDrag::XandYAxis);
    QSignalSpy axisSpy(drag, SIGNAL(axisChanged()));
    drag->setAxis(QSGDrag::XAxis);
    QCOMPARE(drag->axis(), QSGDrag::XAxis);
    QCOMPARE(axisSpy.count(),1);
    drag->setAxis(QSGDrag::XAxis);
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

void tst_QSGMouseArea::resetDrag()
{
    QSGView *canvas = createView();

    canvas->rootContext()->setContextProperty("haveTarget", QVariant(true));
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragreset.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGMouseArea *mouseRegion = canvas->rootObject()->findChild<QSGMouseArea*>("mouseregion");
    QSGDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());
    QSGItem *rootItem = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(drag, SIGNAL(targetChanged()));
    QVERIFY(drag->target() != 0);
    canvas->rootContext()->setContextProperty("haveTarget", QVariant(false));
    QCOMPARE(targetSpy.count(),1);
    QVERIFY(drag->target() == 0);

    delete canvas;
}


void tst_QSGMouseArea::dragging()
{
    QSGView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/dragging.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGMouseArea *mouseRegion = canvas->rootObject()->findChild<QSGMouseArea*>("mouseregion");
    QSGDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());

    QVERIFY(!drag->active());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QVERIFY(!drag->active());
    QCOMPARE(blackRect->x(), 50.0);
    QCOMPARE(blackRect->y(), 50.0);

    // First move event triggers drag, second is acted upon.
    // This is due to possibility of higher stacked area taking precedence.
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(106, 106), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);
    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(110, 110), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    QVERIFY(drag->active());
    QCOMPARE(blackRect->x(), 60.0);
    QCOMPARE(blackRect->y(), 60.0);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(110, 110), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QVERIFY(!drag->active());
    QCOMPARE(blackRect->x(), 60.0);
    QCOMPARE(blackRect->y(), 60.0);

    delete canvas;
}

QSGView *tst_QSGMouseArea::createView()
{
    QSGView *canvas = new QSGView(0);
    canvas->setFixedSize(240,320);

    return canvas;
}

void tst_QSGMouseArea::updateMouseAreaPosOnClick()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/updateMousePosOnClick.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGMouseArea *mouseRegion = canvas->rootObject()->findChild<QSGMouseArea*>("mouseregion");
    QVERIFY(mouseRegion != 0);

    QSGRectangle *rect = canvas->rootObject()->findChild<QSGRectangle*>("ball");
    QVERIFY(rect != 0);

    QCOMPARE(mouseRegion->mouseX(), rect->x());
    QCOMPARE(mouseRegion->mouseY(), rect->y());

    QMouseEvent event(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &event);

    QCOMPARE(mouseRegion->mouseX(), 100.0);
    QCOMPARE(mouseRegion->mouseY(), 100.0);

    QCOMPARE(mouseRegion->mouseX(), rect->x());
    QCOMPARE(mouseRegion->mouseY(), rect->y());

    delete canvas;
}

void tst_QSGMouseArea::updateMouseAreaPosOnResize()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/updateMousePosOnResize.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGMouseArea *mouseRegion = canvas->rootObject()->findChild<QSGMouseArea*>("mouseregion");
    QVERIFY(mouseRegion != 0);

    QSGRectangle *rect = canvas->rootObject()->findChild<QSGRectangle*>("brother");
    QVERIFY(rect != 0);

    QCOMPARE(mouseRegion->mouseX(), 0.0);
    QCOMPARE(mouseRegion->mouseY(), 0.0);

    QMouseEvent event(QEvent::MouseButtonPress, rect->pos().toPoint(), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &event);

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

void tst_QSGMouseArea::noOnClickedWithPressAndHold()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clickandhold.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QVERIFY(!canvas->rootObject()->property("clicked").toBool());
    QVERIFY(!canvas->rootObject()->property("held").toBool());

    QTest::qWait(1000);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QVERIFY(!canvas->rootObject()->property("clicked").toBool());
    QVERIFY(canvas->rootObject()->property("held").toBool());

    delete canvas;
}

void tst_QSGMouseArea::onMousePressRejected()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/rejectEvent.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);
    QVERIFY(canvas->rootObject()->property("enabled").toBool());

    QVERIFY(!canvas->rootObject()->property("mr1_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_canceled").toBool());

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QVERIFY(canvas->rootObject()->property("mr1_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(canvas->rootObject()->property("mr2_pressed").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());
    QVERIFY(canvas->rootObject()->property("mr2_canceled").toBool());

    QTest::qWait(200);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QVERIFY(canvas->rootObject()->property("mr1_released").toBool());
    QVERIFY(!canvas->rootObject()->property("mr1_canceled").toBool());
    QVERIFY(!canvas->rootObject()->property("mr2_released").toBool());

    delete canvas;
}

void tst_QSGMouseArea::doubleClick()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/doubleclick.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("released").toInt(), 1);

    pressEvent = QMouseEvent(QEvent::MouseButtonDblClick, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("doubleClicked").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 2);

    delete canvas;
}

// QTBUG-14832
void tst_QSGMouseArea::clickTwice()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clicktwice.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("pressed").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 1);

    pressEvent = QMouseEvent(QEvent::MouseButtonDblClick, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("pressed").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 2);

    delete canvas;
}

void tst_QSGMouseArea::pressedOrdering()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pressedOrdering.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("base"));

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("pressed"));

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("toggled"));

    QApplication::sendEvent(canvas, &pressEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("pressed"));

    delete canvas;
}

void tst_QSGMouseArea::preventStealing()
{
    QSGView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/preventstealing.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(canvas->rootObject());
    QVERIFY(flickable != 0);

    QSGMouseArea *mouseArea = canvas->rootObject()->findChild<QSGMouseArea*>("mousearea");
    QVERIFY(mouseArea != 0);

    QSignalSpy mousePositionSpy(mouseArea, SIGNAL(positionChanged(QSGMouseEvent*)));

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(80, 80));

    // Without preventStealing, mouse movement over MouseArea would
    // cause the Flickable to steal mouse and trigger content movement.

    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(70, 70), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(60, 60), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(50, 50), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    // We should have received all three move events
    QCOMPARE(mousePositionSpy.count(), 3);
    QVERIFY(mouseArea->pressed());

    // Flickable content should not have moved.
    QCOMPARE(flickable->contentX(), 0.);
    QCOMPARE(flickable->contentY(), 0.);

    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50, 50));

    // Now allow stealing and confirm Flickable does its thing.
    canvas->rootObject()->setProperty("stealing", false);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(80, 80));

    // Without preventStealing, mouse movement over MouseArea would
    // cause the Flickable to steal mouse and trigger content movement.
    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(70, 70), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(60, 60), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    moveEvent = QMouseEvent(QEvent::MouseMove, QPoint(50, 50), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    // We should only have received the first move event
    QCOMPARE(mousePositionSpy.count(), 4);
    // Our press should be taken away
    QVERIFY(!mouseArea->pressed());

    // Flickable content should have moved.
    QCOMPARE(flickable->contentX(), 10.);
    QCOMPARE(flickable->contentY(), 10.);

    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50, 50));

    delete canvas;
}

void tst_QSGMouseArea::clickThrough()
{
    //With no handlers defined click, doubleClick and PressAndHold should propagate to those with handlers
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clickThrough.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 1);

    QApplication::sendEvent(canvas, &pressEvent);
    QTest::qWait(1000);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 1);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);
    pressEvent = QMouseEvent(QEvent::MouseButtonDblClick, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("doubleClicks").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 1);

    delete canvas;

    //With handlers defined click, doubleClick and PressAndHold should propagate only when explicitly ignored
    canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clickThrough2.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    pressEvent = QMouseEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    releaseEvent = QMouseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 0);

    QApplication::sendEvent(canvas, &pressEvent);
    QTest::qWait(1000);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 0);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);

    pressEvent = QMouseEvent(QEvent::MouseButtonDblClick, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("doubleClicks").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 0);

    canvas->rootObject()->setProperty("letThrough", QVariant(true));

    pressEvent = QMouseEvent(QEvent::MouseButtonPress, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &pressEvent);

    releaseEvent = QMouseEvent(QEvent::MouseButtonRelease, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 1);

    QApplication::sendEvent(canvas, &pressEvent);
    QTest::qWait(1000);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 1);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);
    pressEvent = QMouseEvent(QEvent::MouseButtonDblClick, QPoint(100, 100), Qt::LeftButton, Qt::LeftButton, 0);

    QApplication::sendEvent(canvas, &pressEvent);
    QApplication::sendEvent(canvas, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("presses").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("clicks").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("doubleClicks").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("pressAndHolds").toInt(), 1);

    delete canvas;
}

void tst_QSGMouseArea::testQtQuick11Attributes()
{
    QFETCH(QString, code);
    QFETCH(QString, warning);
    QFETCH(QString, error);

    QDeclarativeEngine engine;
    QObject *obj;

    QDeclarativeComponent valid(&engine);
    valid.setData("import QtQuick 1.1; MouseArea { " + code.toUtf8() + " }", QUrl(""));
    obj = valid.create();
    QVERIFY(obj);
    QVERIFY(valid.errorString().isEmpty());
    delete obj;

    QDeclarativeComponent invalid(&engine);
    invalid.setData("import QtQuick 1.0; MouseArea { " + code.toUtf8() + " }", QUrl(""));
    QTest::ignoreMessage(QtWarningMsg, warning.toUtf8());
    obj = invalid.create();
    QCOMPARE(invalid.errorString(), error);
    delete obj;
}

void tst_QSGMouseArea::testQtQuick11Attributes_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("warning");
    QTest::addColumn<QString>("error");

    QTest::newRow("preventStealing") << "preventStealing: true"
        << "QDeclarativeComponent: Component is not ready"
        << ":1 \"MouseArea.preventStealing\" is not available in QtQuick 1.0.\n";
}

void tst_QSGMouseArea::hoverPosition()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/hoverPosition.qml"));

    QSGItem *root = canvas->rootObject();
    QVERIFY(root != 0);

    QCOMPARE(root->property("mouseX").toReal(), qreal(0));
    QCOMPARE(root->property("mouseY").toReal(), qreal(0));

    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(10, 32), Qt::NoButton, Qt::NoButton, 0);
    QApplication::sendEvent(canvas, &moveEvent);

    QCOMPARE(root->property("mouseX").toReal(), qreal(10));
    QCOMPARE(root->property("mouseY").toReal(), qreal(32));

    delete canvas;
}

QTEST_MAIN(tst_QSGMouseArea)

#include "tst_qsgmousearea.moc"
