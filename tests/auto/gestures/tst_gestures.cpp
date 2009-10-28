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
#include "../../shared/util.h"

#include <qevent.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qgesture.h>
#include <qgesturerecognizer.h>
#include <qgraphicsitem.h>
#include <qgraphicsview.h>

#include <qdebug.h>

//TESTED_CLASS=
//TESTED_FILES=

static QPointF mapToGlobal(const QPointF &pt, QGraphicsItem *item, QGraphicsView *view)
{
    return view->mapToGlobal(view->mapFromScene(item->mapToScene(pt)));
}

class CustomGesture : public QGesture
{
    Q_OBJECT
public:
    static Qt::GestureType GestureType;

    CustomGesture(QObject *parent = 0)
        : QGesture(parent), serial(0)
    {
    }

    int serial;

    static const int SerialMaybeThreshold;
    static const int SerialStartedThreshold;
    static const int SerialFinishedThreshold;
};
Qt::GestureType CustomGesture::GestureType = Qt::CustomGesture;
const int CustomGesture::SerialMaybeThreshold = 1;
const int CustomGesture::SerialStartedThreshold = 3;
const int CustomGesture::SerialFinishedThreshold = 6;

class CustomEvent : public QEvent
{
public:
    static int EventType;

    CustomEvent(int serial_ = 0)
        : QEvent(QEvent::Type(CustomEvent::EventType)),
          serial(serial_), hasHotSpot(false)
    {
    }

    int serial;
    QPointF hotSpot;
    bool hasHotSpot;
};
int CustomEvent::EventType = 0;

class CustomGestureRecognizer : public QGestureRecognizer
{
public:
    CustomGestureRecognizer()
    {
        if (!CustomEvent::EventType)
            CustomEvent::EventType = QEvent::registerEventType();
    }

    QGesture* createGesture(QObject *)
    {
        return new CustomGesture;
    }

    QGestureRecognizer::Result filterEvent(QGesture *state, QObject*, QEvent *event)
    {
        if (event->type() == CustomEvent::EventType) {
            QGestureRecognizer::Result result = QGestureRecognizer::ConsumeEventHint;
            CustomGesture *g = static_cast<CustomGesture*>(state);
            CustomEvent *e = static_cast<CustomEvent*>(event);
            g->serial = e->serial;
            if (e->hasHotSpot)
                g->setHotSpot(e->hotSpot);
            if (g->serial >= CustomGesture::SerialFinishedThreshold)
                result |= QGestureRecognizer::GestureFinished;
            else if (g->serial >= CustomGesture::SerialStartedThreshold)
                result |= QGestureRecognizer::GestureTriggered;
            else if (g->serial >= CustomGesture::SerialMaybeThreshold)
                result |= QGestureRecognizer::MaybeGesture;
            else
                result = QGestureRecognizer::NotGesture;
            return result;
        }
        return QGestureRecognizer::Ignore;
    }

    void reset(QGesture *state)
    {
        CustomGesture *g = static_cast<CustomGesture*>(state);
        g->serial = 0;
        QGestureRecognizer::reset(state);
    }
};

// same as CustomGestureRecognizer but triggers early without the maybe state
class CustomContinuousGestureRecognizer : public QGestureRecognizer
{
public:
    CustomContinuousGestureRecognizer()
    {
        if (!CustomEvent::EventType)
            CustomEvent::EventType = QEvent::registerEventType();
    }

    QGesture* createGesture(QObject *)
    {
        return new CustomGesture;
    }

    QGestureRecognizer::Result filterEvent(QGesture *state, QObject*, QEvent *event)
    {
        if (event->type() == CustomEvent::EventType) {
            QGestureRecognizer::Result result = QGestureRecognizer::ConsumeEventHint;
            CustomGesture *g = static_cast<CustomGesture*>(state);
            CustomEvent *e = static_cast<CustomEvent*>(event);
            g->serial = e->serial;
            if (e->hasHotSpot)
                g->setHotSpot(e->hotSpot);
            if (g->serial >= CustomGesture::SerialFinishedThreshold)
                result |= QGestureRecognizer::GestureFinished;
            else if (g->serial >= CustomGesture::SerialMaybeThreshold)
                result |= QGestureRecognizer::GestureTriggered;
            else
                result = QGestureRecognizer::NotGesture;
            return result;
        }
        return QGestureRecognizer::Ignore;
    }

    void reset(QGesture *state)
    {
        CustomGesture *g = static_cast<CustomGesture*>(state);
        g->serial = 0;
        QGestureRecognizer::reset(state);
    }
};

class GestureWidget : public QWidget
{
    Q_OBJECT
public:
    GestureWidget(const char *name = 0, QWidget *parent = 0)
        : QWidget(parent)
    {
        if (name)
            setObjectName(QLatin1String(name));
        reset();
        acceptGestureOverride = false;
    }
    void reset()
    {
        customEventsReceived = 0;
        gestureEventsReceived = 0;
        gestureOverrideEventsReceived = 0;
        events.clear();
        overrideEvents.clear();
        ignoredGestures.clear();
    }

    int customEventsReceived;
    int gestureEventsReceived;
    int gestureOverrideEventsReceived;
    struct Events
    {
        QList<Qt::GestureType> all;
        QList<Qt::GestureType> started;
        QList<Qt::GestureType> updated;
        QList<Qt::GestureType> finished;
        QList<Qt::GestureType> canceled;

        void clear()
        {
            all.clear();
            started.clear();
            updated.clear();
            finished.clear();
            canceled.clear();
        }
    } events, overrideEvents;

    bool acceptGestureOverride;
    QSet<Qt::GestureType> ignoredGestures;

protected:
    bool event(QEvent *event)
    {
        Events *eventsPtr = 0;
        if (event->type() == QEvent::Gesture) {
            QGestureEvent *e = static_cast<QGestureEvent*>(event);
            ++gestureEventsReceived;
            eventsPtr = &events;
            foreach(Qt::GestureType type, ignoredGestures)
                e->ignore(e->gesture(type));
        } else if (event->type() == QEvent::GestureOverride) {
            ++gestureOverrideEventsReceived;
            eventsPtr = &overrideEvents;
            if (acceptGestureOverride)
                event->accept();
        }
        if (eventsPtr) {
            QGestureEvent *e = static_cast<QGestureEvent*>(event);
            QList<QGesture*> gestures = e->allGestures();
            foreach(QGesture *g, gestures) {
                eventsPtr->all << g->gestureType();
                switch(g->state()) {
                case Qt::GestureStarted:
                    eventsPtr->started << g->gestureType();
                    break;
                case Qt::GestureUpdated:
                    eventsPtr->updated << g->gestureType();
                    break;
                case Qt::GestureFinished:
                    eventsPtr->finished << g->gestureType();
                    break;
                case Qt::GestureCanceled:
                    eventsPtr->canceled << g->gestureType();
                    break;
                default:
                    Q_ASSERT(false);
                }
            }
        } else if (event->type() == CustomEvent::EventType) {
            ++customEventsReceived;
        } else {
            return QWidget::event(event);
        }
        return true;
    }
};

static void sendCustomGesture(CustomEvent *event, QObject *object, QGraphicsScene *scene = 0)
{
    for (int i = CustomGesture::SerialMaybeThreshold;
         i <= CustomGesture::SerialFinishedThreshold; ++i) {
        event->serial = i;
        if (scene)
            scene->sendEvent(qobject_cast<QGraphicsObject *>(object), event);
        else
            QApplication::sendEvent(object, event);
    }
}

class tst_Gestures : public QObject
{
Q_OBJECT

public:
    tst_Gestures();
    virtual ~tst_Gestures();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void customGesture();
    void autoCancelingGestures();
    void gestureOverChild();
    void multipleWidgetOnlyGestureInTree();
    void conflictingGestures();
    void finishedWithoutStarted();
    void unknownGesture();
    void graphicsItemGesture();
    void graphicsItemTreeGesture();
    void explicitGraphicsObjectTarget();
    void gestureOverChildGraphicsItem();
    void twoGesturesOnDifferentLevel();
    void multipleGesturesInTree();
    void multipleGesturesInComplexTree();
    void testMapToScene();
};

tst_Gestures::tst_Gestures()
{
}

tst_Gestures::~tst_Gestures()
{
}

void tst_Gestures::initTestCase()
{
    CustomGesture::GestureType = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    QVERIFY(CustomGesture::GestureType != Qt::GestureType(0));
    QVERIFY(CustomGesture::GestureType != Qt::CustomGesture);
}

void tst_Gestures::cleanupTestCase()
{
}

void tst_Gestures::init()
{
}

void tst_Gestures::cleanup()
{
}

void tst_Gestures::customGesture()
{
    GestureWidget widget;
    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    CustomEvent event;
    sendCustomGesture(&event, &widget);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;
    QCOMPARE(widget.customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(widget.gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(widget.gestureOverrideEventsReceived, 0);
    QCOMPARE(widget.events.all.size(), TotalGestureEventsCount);
    for(int i = 0; i < widget.events.all.size(); ++i)
        QCOMPARE(widget.events.all.at(i), CustomGesture::GestureType);
    QCOMPARE(widget.events.started.size(), 1);
    QCOMPARE(widget.events.updated.size(), TotalGestureEventsCount - 2);
    QCOMPARE(widget.events.finished.size(), 1);
    QCOMPARE(widget.events.canceled.size(), 0);
}

void tst_Gestures::autoCancelingGestures()
{
    GestureWidget widget;
    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    // send partial gesture. The gesture will be in the "maybe" state, but will
    // never get enough events to fire, so Qt will have to kill it.
    CustomEvent ev;
    for (int i = CustomGesture::SerialMaybeThreshold;
         i < CustomGesture::SerialStartedThreshold; ++i) {
        ev.serial = i;
        QApplication::sendEvent(&widget, &ev);
    }
    // wait long enough so the gesture manager will cancel the gesture
    QTest::qWait(5000);
    QCOMPARE(widget.customEventsReceived, CustomGesture::SerialStartedThreshold - CustomGesture::SerialMaybeThreshold);
    QCOMPARE(widget.gestureEventsReceived, 0);
    QCOMPARE(widget.gestureOverrideEventsReceived, 0);
    QCOMPARE(widget.events.all.size(), 0);
}

void tst_Gestures::gestureOverChild()
{
    GestureWidget widget("widget");
    QVBoxLayout *l = new QVBoxLayout(&widget);
    GestureWidget *child = new GestureWidget("child");
    l->addWidget(child);

    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);

    CustomEvent event;
    sendCustomGesture(&event, child);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;

    QCOMPARE(child->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(widget.customEventsReceived, 0);
    QCOMPARE(child->gestureEventsReceived, 0);
    QCOMPARE(child->gestureOverrideEventsReceived, 0);
    QCOMPARE(widget.gestureEventsReceived, 0);
    QCOMPARE(widget.gestureOverrideEventsReceived, 0);

    // enable gestures over the children
    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetWithChildrenGesture);

    widget.reset();
    child->reset();

    sendCustomGesture(&event, child);

    QCOMPARE(child->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(widget.customEventsReceived, 0);

    QCOMPARE(child->gestureEventsReceived, 0);
    QCOMPARE(child->gestureOverrideEventsReceived, 0);
    QCOMPARE(widget.gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(widget.gestureOverrideEventsReceived, 0);
    for(int i = 0; i < widget.events.all.size(); ++i)
        QCOMPARE(widget.events.all.at(i), CustomGesture::GestureType);
    QCOMPARE(widget.events.started.size(), 1);
    QCOMPARE(widget.events.updated.size(), TotalGestureEventsCount - 2);
    QCOMPARE(widget.events.finished.size(), 1);
    QCOMPARE(widget.events.canceled.size(), 0);
}

void tst_Gestures::multipleWidgetOnlyGestureInTree()
{
    GestureWidget parent("parent");
    QVBoxLayout *l = new QVBoxLayout(&parent);
    GestureWidget *child = new GestureWidget("child");
    l->addWidget(child);

    parent.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    child->grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;

    // sending events to the child and making sure there is no conflict
    CustomEvent event;
    sendCustomGesture(&event, child);

    QCOMPARE(child->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(parent.customEventsReceived, 0);
    QCOMPARE(child->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(child->gestureOverrideEventsReceived, 0);
    QCOMPARE(parent.gestureEventsReceived, 0);
    QCOMPARE(parent.gestureOverrideEventsReceived, 0);

    parent.reset();
    child->reset();

    // same for the parent widget
    sendCustomGesture(&event, &parent);

    QCOMPARE(child->customEventsReceived, 0);
    QCOMPARE(parent.customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(child->gestureEventsReceived, 0);
    QCOMPARE(child->gestureOverrideEventsReceived, 0);
    QCOMPARE(parent.gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(parent.gestureOverrideEventsReceived, 0);
}

void tst_Gestures::conflictingGestures()
{
    GestureWidget parent("parent");
    QVBoxLayout *l = new QVBoxLayout(&parent);
    GestureWidget *child = new GestureWidget("child");
    l->addWidget(child);

    parent.grabGesture(CustomGesture::GestureType, Qt::WidgetWithChildrenGesture);
    child->grabGesture(CustomGesture::GestureType, Qt::WidgetWithChildrenGesture);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    // child accepts the override, parent will not receive anything
    parent.acceptGestureOverride = false;
    child->acceptGestureOverride = true;

    // sending events to the child and making sure there is no conflict
    CustomEvent event;
    sendCustomGesture(&event, child);

    QCOMPARE(child->gestureOverrideEventsReceived, 1);
    QCOMPARE(child->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(parent.gestureOverrideEventsReceived, 0);
    QCOMPARE(parent.gestureEventsReceived, 0);

    parent.reset();
    child->reset();

    // parent accepts the override
    parent.acceptGestureOverride = true;
    child->acceptGestureOverride = false;

    // sending events to the child and making sure there is no conflict
    sendCustomGesture(&event, child);

    QCOMPARE(child->gestureOverrideEventsReceived, 1);
    QCOMPARE(child->gestureEventsReceived, 0);
    QCOMPARE(parent.gestureOverrideEventsReceived, 1);
    QCOMPARE(parent.gestureEventsReceived, TotalGestureEventsCount);

    parent.reset();
    child->reset();

    // nobody accepts the override, we will send normal events to the closest context (to the child)
    parent.acceptGestureOverride = false;
    child->acceptGestureOverride = false;
    child->ignoredGestures << CustomGesture::GestureType;

    // sending events to the child and making sure there is no conflict
    sendCustomGesture(&event, child);

    QCOMPARE(child->gestureOverrideEventsReceived, 1);
    QCOMPARE(child->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(parent.gestureOverrideEventsReceived, 1);
    QCOMPARE(parent.gestureEventsReceived, TotalGestureEventsCount);

    parent.reset();
    child->reset();

    Qt::GestureType ContinuousGesture = qApp->registerGestureRecognizer(new CustomContinuousGestureRecognizer);
    static const int ContinuousGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;
    child->grabGesture(ContinuousGesture);
    // child accepts override. And it also receives another custom gesture.
    parent.acceptGestureOverride = false;
    child->acceptGestureOverride = true;
    sendCustomGesture(&event, child);

    QCOMPARE(child->gestureOverrideEventsReceived, 1);
    QVERIFY(child->gestureEventsReceived > TotalGestureEventsCount);
    QCOMPARE(child->events.all.count(), TotalGestureEventsCount + ContinuousGestureEventsCount);
    QCOMPARE(parent.gestureOverrideEventsReceived, 0);
    QCOMPARE(parent.gestureEventsReceived, 0);
}

void tst_Gestures::finishedWithoutStarted()
{
    GestureWidget widget;
    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);

    // the gesture will claim it finished, but it was never started.
    CustomEvent ev;
    ev.serial = CustomGesture::SerialFinishedThreshold;
    QApplication::sendEvent(&widget, &ev);

    QCOMPARE(widget.customEventsReceived, 1);
    QCOMPARE(widget.gestureEventsReceived, 2);
    QCOMPARE(widget.gestureOverrideEventsReceived, 0);
    QCOMPARE(widget.events.all.size(), 2);
    QCOMPARE(widget.events.started.size(), 1);
    QCOMPARE(widget.events.updated.size(), 0);
    QCOMPARE(widget.events.finished.size(), 1);
    QCOMPARE(widget.events.canceled.size(), 0);
}

void tst_Gestures::unknownGesture()
{
    GestureWidget widget;
    widget.grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    widget.grabGesture(Qt::CustomGesture, Qt::WidgetGesture);
    widget.grabGesture(Qt::GestureType(Qt::PanGesture+512), Qt::WidgetGesture);

    CustomEvent event;
    sendCustomGesture(&event, &widget);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    QCOMPARE(widget.gestureEventsReceived, TotalGestureEventsCount);
}

static const QColor InstanceColors[] = {
    Qt::blue, Qt::red, Qt::green, Qt::gray, Qt::yellow
};

class GestureItem : public QGraphicsObject
{
    static int InstanceCount;

public:
    GestureItem(const char *name = 0)
    {
        instanceNumber = InstanceCount++;
        if (name)
            setObjectName(QLatin1String(name));
        size = QRectF(0, 0, 100, 100);
        customEventsReceived = 0;
        gestureEventsReceived = 0;
        gestureOverrideEventsReceived = 0;
        events.clear();
        overrideEvents.clear();
        acceptGestureOverride = false;
    }
    ~GestureItem()
    {
        --InstanceCount;
    }

    int customEventsReceived;
    int gestureEventsReceived;
    int gestureOverrideEventsReceived;
    struct Events
    {
        QList<Qt::GestureType> all;
        QList<Qt::GestureType> started;
        QList<Qt::GestureType> updated;
        QList<Qt::GestureType> finished;
        QList<Qt::GestureType> canceled;

        void clear()
        {
            all.clear();
            started.clear();
            updated.clear();
            finished.clear();
            canceled.clear();
        }
    } events, overrideEvents;

    bool acceptGestureOverride;
    QSet<Qt::GestureType> ignoredGestures;

    QRectF size;
    int instanceNumber;

    void reset()
    {
        customEventsReceived = 0;
        gestureEventsReceived = 0;
        gestureOverrideEventsReceived = 0;
        events.clear();
        overrideEvents.clear();
        ignoredGestures.clear();
    }

protected:
    QRectF boundingRect() const
    {
        return size;
    }
    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
    {
        QColor color = InstanceColors[instanceNumber % (sizeof(InstanceColors)/sizeof(InstanceColors[0]))];
        p->fillRect(boundingRect(), color);
    }

    bool event(QEvent *event)
    {
        Events *eventsPtr = 0;
        if (event->type() == QEvent::Gesture) {
            ++gestureEventsReceived;
            eventsPtr = &events;
            QGestureEvent *e = static_cast<QGestureEvent *>(event);
            foreach(Qt::GestureType type, ignoredGestures)
                e->ignore(e->gesture(type));
        } else if (event->type() == QEvent::GestureOverride) {
            ++gestureOverrideEventsReceived;
            eventsPtr = &overrideEvents;
            if (acceptGestureOverride)
                event->accept();
        }
        if (eventsPtr) {
            QGestureEvent *e = static_cast<QGestureEvent*>(event);
            QList<QGesture*> gestures = e->allGestures();
            foreach(QGesture *g, gestures) {
                eventsPtr->all << g->gestureType();
                switch(g->state()) {
                case Qt::GestureStarted:
                    eventsPtr->started << g->gestureType();
                    break;
                case Qt::GestureUpdated:
                    eventsPtr->updated << g->gestureType();
                    break;
                case Qt::GestureFinished:
                    eventsPtr->finished << g->gestureType();
                    break;
                case Qt::GestureCanceled:
                    eventsPtr->canceled << g->gestureType();
                    break;
                default:
                    Q_ASSERT(false);
                }
            }
        } else if (event->type() == CustomEvent::EventType) {
            ++customEventsReceived;
        } else {
            return QGraphicsObject::event(event);
        }
        return true;
    }
};
int GestureItem::InstanceCount = 0;

void tst_Gestures::graphicsItemGesture()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    GestureItem *item = new GestureItem("item");
    scene.addItem(item);
    item->setPos(100, 100);

    view.show();
    QTest::qWaitForWindowShown(&view);
    view.ensureVisible(scene.sceneRect());

    view.viewport()->grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    item->grabGesture(CustomGesture::GestureType);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;

    CustomEvent event;
    // gesture without hotspot should not be delivered to items in the view
    QTest::ignoreMessage(QtWarningMsg, "QGestureManager::deliverEvent: could not find the target for gesture");
    QTest::ignoreMessage(QtWarningMsg, "QGestureManager::deliverEvent: could not find the target for gesture");
    QTest::ignoreMessage(QtWarningMsg, "QGestureManager::deliverEvent: could not find the target for gesture");
    QTest::ignoreMessage(QtWarningMsg, "QGestureManager::deliverEvent: could not find the target for gesture");
    sendCustomGesture(&event, item, &scene);

    QCOMPARE(item->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(item->gestureEventsReceived, 0);
    QCOMPARE(item->gestureOverrideEventsReceived, 0);

    item->reset();

    // make sure the event is properly delivered if only the hotspot is set.
    event.hotSpot = mapToGlobal(QPointF(10, 10), item, &view);
    event.hasHotSpot = true;
    sendCustomGesture(&event, item, &scene);

    QCOMPARE(item->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(item->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item->gestureOverrideEventsReceived, 0);
    QCOMPARE(item->events.all.size(), TotalGestureEventsCount);
    for(int i = 0; i < item->events.all.size(); ++i)
        QCOMPARE(item->events.all.at(i), CustomGesture::GestureType);
    QCOMPARE(item->events.started.size(), 1);
    QCOMPARE(item->events.updated.size(), TotalGestureEventsCount - 2);
    QCOMPARE(item->events.finished.size(), 1);
    QCOMPARE(item->events.canceled.size(), 0);

    item->reset();

    // send gesture to the item which ignores it.
    item->ignoredGestures << CustomGesture::GestureType;

    event.hotSpot = mapToGlobal(QPointF(10, 10), item, &view);
    event.hasHotSpot = true;
    sendCustomGesture(&event, item, &scene);
    QCOMPARE(item->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(item->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item->gestureOverrideEventsReceived, 0);
}

void tst_Gestures::graphicsItemTreeGesture()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    GestureItem *item1 = new GestureItem("item1");
    item1->setPos(100, 100);
    item1->size = QRectF(0, 0, 350, 200);
    scene.addItem(item1);

    GestureItem *item1_child1 = new GestureItem("item1_child1");
    item1_child1->setPos(50, 50);
    item1_child1->size = QRectF(0, 0, 100, 100);
    item1_child1->setParentItem(item1);

    GestureItem *item1_child2 = new GestureItem("item1_child2");
    item1_child2->size = QRectF(0, 0, 100, 100);
    item1_child2->setPos(200, 50);
    item1_child2->setParentItem(item1);

    view.show();
    QTest::qWaitForWindowShown(&view);
    view.ensureVisible(scene.sceneRect());

    view.viewport()->grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    item1->grabGesture(CustomGesture::GestureType);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    CustomEvent event;
    event.hotSpot = mapToGlobal(QPointF(10, 10), item1_child1, &view);
    event.hasHotSpot = true;

    item1->ignoredGestures << CustomGesture::GestureType;
    sendCustomGesture(&event, item1_child1, &scene);
    QCOMPARE(item1_child1->gestureOverrideEventsReceived, 0);
    QCOMPARE(item1_child1->gestureEventsReceived, 0);
    QCOMPARE(item1_child2->gestureEventsReceived, 0);
    QCOMPARE(item1_child2->gestureOverrideEventsReceived, 0);
    QCOMPARE(item1->gestureOverrideEventsReceived, 0);
    QCOMPARE(item1->gestureEventsReceived, TotalGestureEventsCount);

    item1->reset(); item1_child1->reset(); item1_child2->reset();

    item1_child1->grabGesture(CustomGesture::GestureType);

    item1->ignoredGestures << CustomGesture::GestureType;
    item1_child1->ignoredGestures << CustomGesture::GestureType;
    sendCustomGesture(&event, item1_child1, &scene);
    QCOMPARE(item1_child1->gestureOverrideEventsReceived, 1);
    QCOMPARE(item1_child1->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item1_child2->gestureEventsReceived, 0);
    QCOMPARE(item1_child2->gestureOverrideEventsReceived, 0);
    QCOMPARE(item1->gestureOverrideEventsReceived, 1);
    QCOMPARE(item1->gestureEventsReceived, TotalGestureEventsCount);
}

void tst_Gestures::explicitGraphicsObjectTarget()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    GestureItem *item1 = new GestureItem("item1");
    scene.addItem(item1);
    item1->setPos(100, 100);
    item1->setZValue(1);

    GestureItem *item2 = new GestureItem("item2");
    scene.addItem(item2);
    item2->setPos(100, 100);
    item2->setZValue(5);

    GestureItem *item2_child1 = new GestureItem("item2_child1");
    scene.addItem(item2_child1);
    item2_child1->setParentItem(item2);
    item2_child1->setPos(10, 10);

    view.show();
    QTest::qWaitForWindowShown(&view);
    view.ensureVisible(scene.sceneRect());

    view.viewport()->grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    item1->grabGesture(CustomGesture::GestureType, Qt::ItemGesture);
    item2->grabGesture(CustomGesture::GestureType, Qt::ItemGesture);
    item2_child1->grabGesture(CustomGesture::GestureType, Qt::ItemGesture);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    // sending events to item1, but the hotSpot is set to item2
    CustomEvent event;
    event.hotSpot = mapToGlobal(QPointF(15, 15), item2, &view);
    event.hasHotSpot = true;

    sendCustomGesture(&event, item1, &scene);

    QCOMPARE(item1->gestureEventsReceived, 0);
    QCOMPARE(item1->gestureOverrideEventsReceived, 1);
    QCOMPARE(item2_child1->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item2_child1->gestureOverrideEventsReceived, 1);
    QCOMPARE(item2_child1->events.all.size(), TotalGestureEventsCount);
    for(int i = 0; i < item2_child1->events.all.size(); ++i)
        QCOMPARE(item2_child1->events.all.at(i), CustomGesture::GestureType);
    QCOMPARE(item2_child1->events.started.size(), 1);
    QCOMPARE(item2_child1->events.updated.size(), TotalGestureEventsCount - 2);
    QCOMPARE(item2_child1->events.finished.size(), 1);
    QCOMPARE(item2_child1->events.canceled.size(), 0);
    QCOMPARE(item2->gestureEventsReceived, 0);
    QCOMPARE(item2->gestureOverrideEventsReceived, 1);
}

void tst_Gestures::gestureOverChildGraphicsItem()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    GestureItem *item0 = new GestureItem("item0");
    scene.addItem(item0);
    item0->setPos(0, 0);
    item0->grabGesture(CustomGesture::GestureType);
    item0->setZValue(1);

    GestureItem *item1 = new GestureItem("item1");
    scene.addItem(item1);
    item1->setPos(100, 100);
    item1->setZValue(5);

    GestureItem *item2 = new GestureItem("item2");
    scene.addItem(item2);
    item2->setPos(100, 100);
    item2->setZValue(10);

    GestureItem *item2_child1 = new GestureItem("item2_child1");
    scene.addItem(item2_child1);
    item2_child1->setParentItem(item2);
    item2_child1->setPos(0, 0);

    view.show();
    QTest::qWaitForWindowShown(&view);
    view.ensureVisible(scene.sceneRect());

    view.viewport()->grabGesture(CustomGesture::GestureType, Qt::WidgetGesture);
    item1->grabGesture(CustomGesture::GestureType);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;

    CustomEvent event;
    event.hotSpot = mapToGlobal(QPointF(10, 10), item2_child1, &view);
    event.hasHotSpot = true;
    sendCustomGesture(&event, item0, &scene);

    QCOMPARE(item0->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(item2_child1->gestureEventsReceived, 0);
    QCOMPARE(item2_child1->gestureOverrideEventsReceived, 0);
    QCOMPARE(item2->gestureEventsReceived, 0);
    QCOMPARE(item2->gestureOverrideEventsReceived, 0);
    QCOMPARE(item1->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item1->gestureOverrideEventsReceived, 0);

    item0->reset(); item1->reset(); item2->reset(); item2_child1->reset();
    item2->grabGesture(CustomGesture::GestureType);
    item2->ignoredGestures << CustomGesture::GestureType;

    event.hotSpot = mapToGlobal(QPointF(10, 10), item2_child1, &view);
    event.hasHotSpot = true;
    sendCustomGesture(&event, item0, &scene);

    QCOMPARE(item0->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(item2_child1->gestureEventsReceived, 0);
    QCOMPARE(item2_child1->gestureOverrideEventsReceived, 0);
    QCOMPARE(item2->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item2->gestureOverrideEventsReceived, 1);
    QCOMPARE(item1->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(item1->gestureOverrideEventsReceived, 1);
}

void tst_Gestures::twoGesturesOnDifferentLevel()
{
    GestureWidget parent("parent");
    QVBoxLayout *l = new QVBoxLayout(&parent);
    GestureWidget *child = new GestureWidget("child");
    l->addWidget(child);

    Qt::GestureType SecondGesture = qApp->registerGestureRecognizer(new CustomGestureRecognizer);

    parent.grabGesture(CustomGesture::GestureType, Qt::WidgetWithChildrenGesture);
    child->grabGesture(SecondGesture, Qt::WidgetWithChildrenGesture);

    CustomEvent event;
    // sending events that form a gesture to one widget, but they will be
    // filtered by two different gesture recognizers and will generate two
    // QGesture objects. Check that those gesture objects are delivered to
    // different widgets properly.
    sendCustomGesture(&event, child);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;
    static const int TotalCustomEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialMaybeThreshold + 1;

    QCOMPARE(child->customEventsReceived, TotalCustomEventsCount);
    QCOMPARE(child->gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(child->gestureOverrideEventsReceived, 0);
    QCOMPARE(child->events.all.size(), TotalGestureEventsCount);
    for(int i = 0; i < child->events.all.size(); ++i)
        QCOMPARE(child->events.all.at(i), SecondGesture);

    QCOMPARE(parent.gestureEventsReceived, TotalGestureEventsCount);
    QCOMPARE(parent.gestureOverrideEventsReceived, 0);
    QCOMPARE(parent.events.all.size(), TotalGestureEventsCount);
    for(int i = 0; i < child->events.all.size(); ++i)
        QCOMPARE(parent.events.all.at(i), CustomGesture::GestureType);
}

void tst_Gestures::multipleGesturesInTree()
{
    GestureWidget a("A");
    GestureWidget *A = &a;
    GestureWidget *B = new GestureWidget("B", A);
    GestureWidget *C = new GestureWidget("C", B);
    GestureWidget *D = new GestureWidget("D", C);

    Qt::GestureType FirstGesture  = CustomGesture::GestureType;
    Qt::GestureType SecondGesture = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType ThirdGesture  = qApp->registerGestureRecognizer(new CustomGestureRecognizer);

    A->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);   // A [1   3]
    A->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);   // |
    B->grabGesture(SecondGesture, Qt::WidgetWithChildrenGesture);  // B [  2 3]
    B->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);   // |
    C->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);   // C [1 2 3]
    C->grabGesture(SecondGesture, Qt::WidgetWithChildrenGesture);  // |
    C->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);   // D [1   3]
    D->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);
    D->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);

    // make sure all widgets ignore events, so they get propagated.
    A->ignoredGestures << FirstGesture << ThirdGesture;
    B->ignoredGestures << SecondGesture << ThirdGesture;
    C->ignoredGestures << FirstGesture << SecondGesture << ThirdGesture;
    D->ignoredGestures << FirstGesture << ThirdGesture;

    CustomEvent event;
    sendCustomGesture(&event, D);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    // gesture override events
    QCOMPARE(D->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(D->overrideEvents.all.count(SecondGesture), 0);
    QCOMPARE(D->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(C->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(C->overrideEvents.all.count(SecondGesture), 1);
    QCOMPARE(C->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(B->overrideEvents.all.count(FirstGesture), 0);
    QCOMPARE(B->overrideEvents.all.count(SecondGesture), 1);
    QCOMPARE(B->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(A->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(A->overrideEvents.all.count(SecondGesture), 0);
    QCOMPARE(A->overrideEvents.all.count(ThirdGesture), 1);

    // normal gesture events
    QCOMPARE(D->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(D->events.all.count(SecondGesture), 0);
    QCOMPARE(D->events.all.count(ThirdGesture), TotalGestureEventsCount);

    QCOMPARE(C->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(SecondGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(ThirdGesture), TotalGestureEventsCount);

    QCOMPARE(B->events.all.count(FirstGesture), 0);
    QCOMPARE(B->events.all.count(SecondGesture), TotalGestureEventsCount);
    QCOMPARE(B->events.all.count(ThirdGesture), TotalGestureEventsCount);

    QCOMPARE(A->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(A->events.all.count(SecondGesture), 0);
    QCOMPARE(A->events.all.count(ThirdGesture), TotalGestureEventsCount);
}

void tst_Gestures::multipleGesturesInComplexTree()
{
    GestureWidget a("A");
    GestureWidget *A = &a;
    GestureWidget *B = new GestureWidget("B", A);
    GestureWidget *C = new GestureWidget("C", B);
    GestureWidget *D = new GestureWidget("D", C);

    Qt::GestureType FirstGesture   = CustomGesture::GestureType;
    Qt::GestureType SecondGesture  = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType ThirdGesture   = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType FourthGesture  = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType FifthGesture   = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType SixthGesture   = qApp->registerGestureRecognizer(new CustomGestureRecognizer);
    Qt::GestureType SeventhGesture = qApp->registerGestureRecognizer(new CustomGestureRecognizer);

    A->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);   // A [1,3,4]
    A->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);   // |
    A->grabGesture(FourthGesture, Qt::WidgetWithChildrenGesture);  // B [2,3,5]
    B->grabGesture(SecondGesture, Qt::WidgetWithChildrenGesture);  // |
    B->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);   // C [1,2,3,6]
    B->grabGesture(FifthGesture, Qt::WidgetWithChildrenGesture);   // |
    C->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);   // D [1,3,7]
    C->grabGesture(SecondGesture, Qt::WidgetWithChildrenGesture);
    C->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);
    C->grabGesture(SixthGesture, Qt::WidgetWithChildrenGesture);
    D->grabGesture(FirstGesture, Qt::WidgetWithChildrenGesture);
    D->grabGesture(ThirdGesture, Qt::WidgetWithChildrenGesture);
    D->grabGesture(SeventhGesture, Qt::WidgetWithChildrenGesture);

    // make sure all widgets ignore events, so they get propagated.
    QSet<Qt::GestureType> allGestureTypes;
    allGestureTypes << FirstGesture << SecondGesture << ThirdGesture
            << FourthGesture << FifthGesture << SixthGesture << SeventhGesture;
    A->ignoredGestures = B->ignoredGestures = allGestureTypes;
    C->ignoredGestures = D->ignoredGestures = allGestureTypes;

    CustomEvent event;
    sendCustomGesture(&event, D);

    static const int TotalGestureEventsCount = CustomGesture::SerialFinishedThreshold - CustomGesture::SerialStartedThreshold + 1;

    // gesture override events
    QCOMPARE(D->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(D->overrideEvents.all.count(SecondGesture), 0);
    QCOMPARE(D->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(C->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(C->overrideEvents.all.count(SecondGesture), 1);
    QCOMPARE(C->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(B->overrideEvents.all.count(FirstGesture), 0);
    QCOMPARE(B->overrideEvents.all.count(SecondGesture), 1);
    QCOMPARE(B->overrideEvents.all.count(ThirdGesture), 1);

    QCOMPARE(A->overrideEvents.all.count(FirstGesture), 1);
    QCOMPARE(A->overrideEvents.all.count(SecondGesture), 0);
    QCOMPARE(A->overrideEvents.all.count(ThirdGesture), 1);

    // normal gesture events
    QCOMPARE(D->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(D->events.all.count(SecondGesture), 0);
    QCOMPARE(D->events.all.count(ThirdGesture), TotalGestureEventsCount);
    QCOMPARE(D->events.all.count(FourthGesture), 0);
    QCOMPARE(D->events.all.count(FifthGesture), 0);
    QCOMPARE(D->events.all.count(SixthGesture), 0);
    QCOMPARE(D->events.all.count(SeventhGesture), TotalGestureEventsCount);

    QCOMPARE(C->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(SecondGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(ThirdGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(FourthGesture), 0);
    QCOMPARE(C->events.all.count(FifthGesture), 0);
    QCOMPARE(C->events.all.count(SixthGesture), TotalGestureEventsCount);
    QCOMPARE(C->events.all.count(SeventhGesture), 0);

    QCOMPARE(B->events.all.count(FirstGesture), 0);
    QCOMPARE(B->events.all.count(SecondGesture), TotalGestureEventsCount);
    QCOMPARE(B->events.all.count(ThirdGesture), TotalGestureEventsCount);
    QCOMPARE(B->events.all.count(FourthGesture), 0);
    QCOMPARE(B->events.all.count(FifthGesture), TotalGestureEventsCount);
    QCOMPARE(B->events.all.count(SixthGesture), 0);
    QCOMPARE(B->events.all.count(SeventhGesture), 0);

    QCOMPARE(A->events.all.count(FirstGesture), TotalGestureEventsCount);
    QCOMPARE(A->events.all.count(SecondGesture), 0);
    QCOMPARE(A->events.all.count(ThirdGesture), TotalGestureEventsCount);
    QCOMPARE(A->events.all.count(FourthGesture), TotalGestureEventsCount);
    QCOMPARE(A->events.all.count(FifthGesture), 0);
    QCOMPARE(A->events.all.count(SixthGesture), 0);
    QCOMPARE(A->events.all.count(SeventhGesture), 0);
}

void tst_Gestures::testMapToScene()
{
    QGesture gesture;
    QList<QGesture*> list;
    list << &gesture;
    QGestureEvent event(list);
    QCOMPARE(event.mapToScene(gesture.hotSpot()), QPointF()); // not set, can't do much

    QGraphicsScene scene;
    QGraphicsView view(&scene);

    GestureItem *item0 = new GestureItem;
    scene.addItem(item0);
    item0->setPos(14, 16);

    view.show(); // need to show to give it a global coordinate
    QTest::qWaitForWindowShown(&view);
    view.ensureVisible(scene.sceneRect());

    QPoint origin = view.mapToGlobal(QPoint());
    event.setWidget(view.viewport());

    QCOMPARE(event.mapToScene(origin + QPoint(100, 200)), view.mapToScene(QPoint(100, 200)));
}

QTEST_MAIN(tst_Gestures)
#include "tst_gestures.moc"
