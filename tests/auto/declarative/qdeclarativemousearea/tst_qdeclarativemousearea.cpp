/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include <private/qdeclarativemousearea_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativeflickable_p.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeproperty.h>

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
    void doubleClick();
    void clickTwice();
    void pressedOrdering();
    void preventStealing();
    void testQtQuick11Attributes();
    void testQtQuick11Attributes_data();
#ifndef QT_NO_CONTEXTMENU
    void preventContextMenu();
#endif // QT_NO_CONTEXTMENU

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

    // First move event triggers drag, second is acted upon.
    // This is due to possibility of higher stacked area taking precedence.
    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent.setScenePos(QPointF(106, 106));
    moveEvent.setButton(Qt::LeftButton);
    moveEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &moveEvent);

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

    delete canvas;
}

void tst_QDeclarativeMouseArea::onMousePressRejected()
{
    QDeclarativeView *canvas = createView();
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

    delete canvas;
}

void tst_QDeclarativeMouseArea::doubleClick()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/doubleclick.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(100, 100));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("released").toInt(), 1);

    QGraphicsSceneMouseEvent dblClickEvent(QEvent::GraphicsSceneMouseDoubleClick);
    dblClickEvent.setScenePos(QPointF(100, 100));
    dblClickEvent.setButton(Qt::LeftButton);
    dblClickEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &dblClickEvent);

    QApplication::sendEvent(scene, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("doubleClicked").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 2);

    delete canvas;
}

// QTBUG-14832
void tst_QDeclarativeMouseArea::clickTwice()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/clicktwice.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(100, 100));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("pressed").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 1);
    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 1);

    QGraphicsSceneMouseEvent dblClickEvent(QEvent::GraphicsSceneMouseDoubleClick);
    dblClickEvent.setScenePos(QPointF(100, 100));
    dblClickEvent.setButton(Qt::LeftButton);
    dblClickEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &dblClickEvent);

    QApplication::sendEvent(scene, &pressEvent);
    QApplication::sendEvent(scene, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("pressed").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("released").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("clicked").toInt(), 2);

    delete canvas;
}

void tst_QDeclarativeMouseArea::pressedOrdering()
{
    QDeclarativeView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pressedOrdering.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("base"));

    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    pressEvent.setScenePos(QPointF(100, 100));
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &pressEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("pressed"));

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(QPointF(100, 100));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &releaseEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("toggled"));

    QApplication::sendEvent(scene, &pressEvent);

    QCOMPARE(canvas->rootObject()->property("value").toString(), QLatin1String("pressed"));

    delete canvas;
}

void tst_QDeclarativeMouseArea::preventStealing()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/preventstealing.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(canvas->rootObject());
    QVERIFY(flickable != 0);

    QDeclarativeMouseArea *mouseArea = canvas->rootObject()->findChild<QDeclarativeMouseArea*>("mousearea");
    QVERIFY(mouseArea != 0);

    QSignalSpy mousePositionSpy(mouseArea, SIGNAL(positionChanged(QDeclarativeMouseEvent*)));

    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(80, 80)));

    // Without preventStealing, mouse movement over MouseArea would
    // cause the Flickable to steal mouse and trigger content movement.
    QGraphicsScene *scene = canvas->scene();
    QGraphicsSceneMouseEvent moveEvent(QEvent::GraphicsSceneMouseMove);
    moveEvent.setScenePos(QPointF(70, 70));
    moveEvent.setButton(Qt::LeftButton);
    moveEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &moveEvent);

    moveEvent.setScenePos(QPointF(60, 60));
    moveEvent.setButton(Qt::LeftButton);
    moveEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &moveEvent);

    moveEvent.setScenePos(QPointF(50, 50));
    moveEvent.setButton(Qt::LeftButton);
    moveEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(scene, &moveEvent);

    // We should have received all three move events
    QCOMPARE(mousePositionSpy.count(), 3);
    QVERIFY(mouseArea->pressed());

    // Flickable content should not have moved.
    QCOMPARE(flickable->contentX(), 0.);
    QCOMPARE(flickable->contentY(), 0.);

    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(50, 50)));

    // Now allow stealing and confirm Flickable does its thing.
    canvas->rootObject()->setProperty("stealing", false);

    QTest::mousePress(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(80, 80)));

    // Without preventStealing, mouse movement over MouseArea would
    // cause the Flickable to steal mouse and trigger content movement.
    moveEvent.setScenePos(QPointF(70, 70));
    QApplication::sendEvent(scene, &moveEvent);

    moveEvent.setScenePos(QPointF(60, 60));
    QApplication::sendEvent(scene, &moveEvent);

    moveEvent.setScenePos(QPointF(50, 50));
    QApplication::sendEvent(scene, &moveEvent);

    // We should only have received the first move event
    QCOMPARE(mousePositionSpy.count(), 4);
    // Our press should be taken away
    QVERIFY(!mouseArea->pressed());

    // Flickable content should have moved.
    QCOMPARE(flickable->contentX(), 10.);
    QCOMPARE(flickable->contentY(), 10.);

    QTest::mouseRelease(canvas->viewport(), Qt::LeftButton, 0, canvas->mapFromScene(QPoint(50, 50)));

    delete canvas;
}

void tst_QDeclarativeMouseArea::testQtQuick11Attributes()
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

void tst_QDeclarativeMouseArea::testQtQuick11Attributes_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("warning");
    QTest::addColumn<QString>("error");

    QTest::newRow("preventStealing") << "preventStealing: true"
        << "QDeclarativeComponent: Component is not ready"
        << ":1 \"MouseArea.preventStealing\" is not available in QtQuick 1.0.\n";
}

#ifndef QT_NO_CONTEXTMENU
class ContextMenuEventReceiver : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int eventCount READ eventCount NOTIFY eventCountChanged);
public:
    ContextMenuEventReceiver(QDeclarativeItem *parent = 0) : QDeclarativeItem(parent), m_eventCount(0) { }
    int eventCount() const { return m_eventCount; }
signals:
    void eventCountChanged(int);
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
        if (event->reason() == QGraphicsSceneContextMenuEvent::Mouse) {
            m_eventCount++;
            emit eventCountChanged(m_eventCount);
        }
    }
private:
    int m_eventCount;
};

void tst_QDeclarativeMouseArea::preventContextMenu()
{
    // A MouseArea accepting Left, Middle and Right buttons should prevent context menu
    // events with "Mouse" reason to hit the Item below.

    qmlRegisterType<ContextMenuEventReceiver>("Test", 1, 0, "ContextMenuEventReceiver");

    QDeclarativeView *view = createView();
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/preventContextMenu.qml"));
    view->show();
    QVERIFY(view->rootObject() != 0);

    QDeclarativeProperty mouseAreaEnabled(view->rootObject(), "mouseAreaEnabled");
    QVERIFY(mouseAreaEnabled.read().toBool());

    QDeclarativeProperty eventsReceived(view->rootObject(), "eventsReceived");
    QCOMPARE(eventsReceived.read().toInt(), 0);

    QPoint targetPoint = view->mapFromScene(QPoint(80, 80));

    QContextMenuEvent fakeEvent1(QContextMenuEvent::Mouse, targetPoint);
    QApplication::sendEvent(view->viewport(), &fakeEvent1);
    QCOMPARE(eventsReceived.read().toInt(), 0);

    mouseAreaEnabled.write(false);
    QVERIFY(!mouseAreaEnabled.read().toBool());
    QContextMenuEvent fakeEvent2(QContextMenuEvent::Mouse, targetPoint);
    QApplication::sendEvent(view->viewport(), &fakeEvent2);
    QCOMPARE(eventsReceived.read().toInt(), 1);

    mouseAreaEnabled.write(true);
    QVERIFY(mouseAreaEnabled.read().toBool());
    QContextMenuEvent fakeEvent3(QContextMenuEvent::Mouse, targetPoint);
    QApplication::sendEvent(view->viewport(), &fakeEvent3);
    QCOMPARE(eventsReceived.read().toInt(), 1);
}
#endif // QT_NO_CONTEXTMENU

QTEST_MAIN(tst_QDeclarativeMouseArea)

#include "tst_qdeclarativemousearea.moc"
