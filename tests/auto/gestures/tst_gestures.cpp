/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtGui>

#include <QtTest/QtTest>

#include "customgesturerecognizer.h"

//TESTED_CLASS=
//TESTED_FILES=

// color generator for syntax highlighting from QQ-26 by Helder Correia
QVector<QColor> highlight(const QColor &bg, const
                          QColor &fg, int noColors)
{
    QVector<QColor> colors;
    const int HUE_BASE = (bg.hue() == -1) ? 90 : bg.hue();
    int h, s, v;
    for (int i = 0; i < noColors; i++) {
        h = int(HUE_BASE + (360.0 / noColors * i)) % 360;
        s = 240;
        v = int(qMax(bg.value(), fg.value()) * 0.85);
        
        const int M = 35;
        if ((h < bg.hue() + M && h > bg.hue() - M)
            || (h < fg.hue() + M && h > fg.hue() - M))
        {
            h = ((bg.hue() + fg.hue()) / (i+1)) % 360;
            s = ((bg.saturation() + fg.saturation() + 2*i)
                 / 2) % 256;
            v = ((bg.value() + fg.value() + 2*i) / 2)
                % 256;
        }
        colors.append(QColor::fromHsv(h, s, v));
    }
    return colors;
}


// a hack to mark an event as spontaneous.
class QETWidget
{
public:
    static void setSpont(QEvent *event, bool spont) { event->spont = spont; }
};

struct GestureState
{
    int seenGestureEvent;
    struct LastGestureEvent
    {
        struct SingleshotGesture
        {
            bool delivered;
            QPoint offset;
        } singleshot;
        struct PinchGesture
        {
            bool delivered;
            TouchPoint startPoints[2];
            TouchPoint lastPoints[2];
            TouchPoint points[2];
            QPoint offset;
        } pinch;
        struct SecondFingerGesture
        {
            bool delivered;
            TouchPoint startPoint;
            TouchPoint lastPoint;
            TouchPoint point;
            QPoint offset;
        } secondfinger;
        struct PanGesture
        {
            bool delivered;
            TouchPoint startPoints[2];
            TouchPoint lastPoints[2];
            TouchPoint points[2];
            QPoint offset;
        } pan;
        QSet<QString> cancelled;
    } last;

    GestureState() { reset(); }
    void reset()
    {
        seenGestureEvent = 0;
        last.singleshot.delivered = false;
        last.singleshot.offset = QPoint();
        last.pinch.delivered = false;
        last.pinch.startPoints[0] = TouchPoint();
        last.pinch.startPoints[1] = TouchPoint();
        last.pinch.lastPoints[0] = TouchPoint();
        last.pinch.lastPoints[1] = TouchPoint();
        last.pinch.points[0] = TouchPoint();
        last.pinch.points[1] = TouchPoint();
        last.pinch.offset = QPoint();
        last.secondfinger.delivered = false;
        last.secondfinger.startPoint = TouchPoint();
        last.secondfinger.lastPoint = TouchPoint();
        last.secondfinger.point = TouchPoint();
        last.secondfinger.offset = QPoint();
        last.cancelled.clear();
    }
};

struct TouchState
{
    int seenTouchBeginEvent;
    int seenTouchUpdateEvent;
    int seenTouchEndEvent;

    TouchState() { reset(); }
    void reset()
    {
        seenTouchBeginEvent = seenTouchUpdateEvent = seenTouchEndEvent = 0;
    }
};

class GestureWidget : public QWidget
{
    Q_OBJECT
    static QVector<QColor> colors;
    static int numberOfWidgets;

public:
    enum Type { DoNotGrabGestures, GrabAllGestures, GrabSingleshot,
                GrabPinch, GrabSecondFinger, GrabPan };

    static const int LeftMargin = 10;
    static const int TopMargin  = 20;

    GestureWidget(Type type = GrabAllGestures)
    {
        if (colors.isEmpty()) {
            colors = highlight(palette().color(QPalette::Window), palette().color(QPalette::Text), 5);
        }
        QPalette p = palette();
        p.setColor(QPalette::Window, colors[numberOfWidgets % colors.size()]);
        setPalette(p);
        setAutoFillBackground(true);
        ++numberOfWidgets;

        QVBoxLayout *l = new QVBoxLayout(this);
        l->setSpacing(0);
        l->setContentsMargins(LeftMargin, TopMargin, LeftMargin, TopMargin);

        singleshotGestureId = -1;
        pinchGestureId = -1;
        secondFingerGestureId = -1;
        panGestureId = -1;
        if (type == GrabAllGestures || type == GrabSingleshot) {
            singleshotGestureId = grabGesture(SingleshotGestureRecognizer::Name);
        }
        if (type == GrabAllGestures || type == GrabPinch) {
            pinchGestureId = grabGesture(PinchGestureRecognizer::Name);
        }
        if (type == GrabAllGestures || type == GrabSecondFinger) {
            secondFingerGestureId = grabGesture(SecondFingerGestureRecognizer::Name);
        }
        if (type == GrabAllGestures || type == GrabPan) {
            panGestureId = grabGesture(PanGestureRecognizer::Name);
        }
        reset();
    }
    ~GestureWidget()
    {
        --numberOfWidgets;
        ungrabGestures();
    }

    void grabSingleshotGesture()
    {
        singleshotGestureId = grabGesture(SingleshotGestureRecognizer::Name);
    }
    void grabPinchGesture()
    {
        pinchGestureId = grabGesture(PinchGestureRecognizer::Name);
    }
    void grabSecondFingerGesture()
    {
        secondFingerGestureId = grabGesture(SecondFingerGestureRecognizer::Name);
    }
    void grabPanGesture()
    {
        panGestureId = grabGesture(PanGestureRecognizer::Name);
    }
    void ungrabGestures()
    {
        releaseGesture(singleshotGestureId);
        singleshotGestureId = -1;
        releaseGesture(pinchGestureId);
        pinchGestureId = -1;
        releaseGesture(secondFingerGestureId);
        secondFingerGestureId = -1;
        releaseGesture(panGestureId);
        panGestureId = -1;
    }

    int singleshotGestureId;
    int pinchGestureId;
    int secondFingerGestureId;
    int panGestureId;

    bool shouldAcceptSingleshotGesture;
    bool shouldAcceptPinchGesture;
    bool shouldAcceptSecondFingerGesture;
    bool shouldAcceptPanGesture;

    GestureState gesture;
    TouchState touch;

    void reset()
    {
        shouldAcceptSingleshotGesture = true;
        shouldAcceptPinchGesture = true;
        shouldAcceptSecondFingerGesture = true;
        shouldAcceptPanGesture = true;
        gesture.reset();
        touch.reset();
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::TouchBegin) {
            event->accept();
            ++touch.seenTouchBeginEvent;
            return true;
        } else if (event->type() == QEvent::TouchUpdate) {
            ++touch.seenTouchUpdateEvent;
        } else if (event->type() == QEvent::TouchEnd) {
            ++touch.seenTouchEndEvent;
        } else if (event->type() == QEvent::Gesture) {
            QGestureEvent *e = static_cast<QGestureEvent*>(event);
            ++gesture.seenGestureEvent;
            if (SingleshotGesture *g = (SingleshotGesture*)e->gesture(SingleshotGestureRecognizer::Name)) {
                gesture.last.singleshot.delivered = true;
                gesture.last.singleshot.offset = g->offset;
                if (shouldAcceptSingleshotGesture)
                    g->accept();
            }
            if (PinchGesture *g = (PinchGesture*)e->gesture(PinchGestureRecognizer::Name)) {
                gesture.last.pinch.delivered = true;
                gesture.last.pinch.startPoints[0] = g->startPoints[0];
                gesture.last.pinch.startPoints[1] = g->startPoints[1];
                gesture.last.pinch.lastPoints[0] = g->lastPoints[0];
                gesture.last.pinch.lastPoints[1] = g->lastPoints[1];
                gesture.last.pinch.points[0] = g->points[0];
                gesture.last.pinch.points[1] = g->points[1];
                gesture.last.pinch.offset = g->offset;
                if (shouldAcceptPinchGesture)
                    g->accept();
            }
            if (SecondFingerGesture *g = (SecondFingerGesture*)e->gesture(SecondFingerGestureRecognizer::Name)) {
                gesture.last.secondfinger.delivered = true;
                gesture.last.secondfinger.startPoint = g->startPoint;
                gesture.last.secondfinger.lastPoint = g->lastPoint;
                gesture.last.secondfinger.point = g->point;
                gesture.last.secondfinger.offset = g->offset;
                if (shouldAcceptSecondFingerGesture)
                    g->accept();
            }
            if (PanGesture *g = (PanGesture*)e->gesture(PanGestureRecognizer::Name)) {
                gesture.last.pan.delivered = true;
                gesture.last.pan.startPoints[0] = g->startPoints[0];
                gesture.last.pan.startPoints[1] = g->startPoints[1];
                gesture.last.pan.lastPoints[0] = g->lastPoints[0];
                gesture.last.pan.lastPoints[1] = g->lastPoints[1];
                gesture.last.pan.points[0] = g->points[0];
                gesture.last.pan.points[1] = g->points[1];
                gesture.last.pan.offset = g->offset;
                if (shouldAcceptPanGesture)
                    g->accept();
            }
            gesture.last.cancelled = e->cancelledGestures();
            return true;
        }
        return QWidget::event(event);
    }
};
QVector<QColor> GestureWidget::colors;
int GestureWidget::numberOfWidgets = 0;

class GraphicsScene : public QGraphicsScene
{
public:
    GraphicsScene()
    {
        reset();
    }
    bool shouldAcceptSingleshotGesture;
    bool shouldAcceptPinchGesture;
    bool shouldAcceptSecondFingerGesture;
    GestureState gesture;

    void reset()
    {
        shouldAcceptSingleshotGesture = false;
        shouldAcceptPinchGesture = false;
        shouldAcceptSecondFingerGesture = false;
        gesture.reset();
    }
protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::GraphicsSceneGesture) {
            QGraphicsSceneGestureEvent *e = static_cast<QGraphicsSceneGestureEvent*>(event);
            ++gesture.seenGestureEvent;
            QGraphicsScene::event(event);
            if (SingleshotGesture *g = (SingleshotGesture*)e->gesture(SingleshotGestureRecognizer::Name)) {
                gesture.last.singleshot.delivered = true;
                gesture.last.singleshot.offset = g->offset;
                if (shouldAcceptSingleshotGesture)
                    g->accept();
            }
            if (PinchGesture *g = (PinchGesture*)e->gesture(PinchGestureRecognizer::Name)) {
                gesture.last.pinch.delivered = true;
                gesture.last.pinch.startPoints[0] = g->startPoints[0];
                gesture.last.pinch.startPoints[1] = g->startPoints[1];
                gesture.last.pinch.lastPoints[0] = g->lastPoints[0];
                gesture.last.pinch.lastPoints[1] = g->lastPoints[1];
                gesture.last.pinch.points[0] = g->points[0];
                gesture.last.pinch.points[1] = g->points[1];
                gesture.last.pinch.offset = g->offset;
                if (shouldAcceptPinchGesture)
                    g->accept();
            }
            if (SecondFingerGesture *g = (SecondFingerGesture*)e->gesture(SecondFingerGestureRecognizer::Name)) {
                gesture.last.secondfinger.delivered = true;
                gesture.last.secondfinger.startPoint = g->startPoint;
                gesture.last.secondfinger.lastPoint = g->lastPoint;
                gesture.last.secondfinger.point = g->point;
                gesture.last.secondfinger.offset = g->offset;
                if (shouldAcceptSecondFingerGesture)
                    g->accept();
            }
            gesture.last.cancelled = e->cancelledGestures();
            return true;
        }
        return QGraphicsScene::event(event);
    }
};

class GraphicsItem : public QGraphicsItem
{
public:
    GraphicsItem(int w = 100, int h = 100)
        : width(w), height(h)
    {
        reset();
    }

    QRectF boundingRect() const
    {
        return QRectF(0, 0, width, height);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
    {
        painter->setBrush(Qt::green);
        painter->drawRect(0, 0, width, height);
    }

    void grabSingleshotGesture()
    {
        singleshotGestureId = grabGesture(SingleshotGestureRecognizer::Name);
    }
    void grabPinchGesture()
    {
        pinchGestureId = grabGesture(PinchGestureRecognizer::Name);
    }
    void grabSecondFingerGesture()
    {
        secondFingerGestureId = grabGesture(SecondFingerGestureRecognizer::Name);
    }
    void ungrabGestures()
    {
        releaseGesture(singleshotGestureId);
        singleshotGestureId = -1;
        releaseGesture(pinchGestureId);
        pinchGestureId = -1;
        releaseGesture(secondFingerGestureId);
        secondFingerGestureId = -1;
    }

    int width;
    int height;

    int singleshotGestureId;
    int pinchGestureId;
    int secondFingerGestureId;

    bool shouldAcceptSingleshotGesture;
    bool shouldAcceptPinchGesture;
    bool shouldAcceptSecondFingerGesture;
    GestureState gesture;

    TouchState touch;

    void reset()
    {
        shouldAcceptSingleshotGesture = true;
        shouldAcceptPinchGesture = true;
        shouldAcceptSecondFingerGesture = true;
        gesture.reset();
    }
protected:
    bool sceneEvent(QEvent *event)
    {
        if (event->type() == QEvent::TouchBegin) {
            event->accept();
            ++touch.seenTouchBeginEvent;
            return true;
        } else if (event->type() == QEvent::TouchUpdate) {
            ++touch.seenTouchUpdateEvent;
        } else if (event->type() == QEvent::TouchEnd) {
            ++touch.seenTouchEndEvent;
        } else if (event->type() == QEvent::GraphicsSceneGesture) {
            QGraphicsSceneGestureEvent *e = static_cast<QGraphicsSceneGestureEvent*>(event);
            ++gesture.seenGestureEvent;
            if (SingleshotGesture *g = (SingleshotGesture*)e->gesture(SingleshotGestureRecognizer::Name)) {
                gesture.last.singleshot.delivered = true;
                gesture.last.singleshot.offset = g->offset;
                if (shouldAcceptSingleshotGesture)
                    g->accept();
            }
            if (PinchGesture *g = (PinchGesture*)e->gesture(PinchGestureRecognizer::Name)) {
                gesture.last.pinch.delivered = true;
                gesture.last.pinch.startPoints[0] = g->startPoints[0];
                gesture.last.pinch.startPoints[1] = g->startPoints[1];
                gesture.last.pinch.lastPoints[0] = g->lastPoints[0];
                gesture.last.pinch.lastPoints[1] = g->lastPoints[1];
                gesture.last.pinch.points[0] = g->points[0];
                gesture.last.pinch.points[1] = g->points[1];
                gesture.last.pinch.offset = g->offset;
                if (shouldAcceptPinchGesture)
                    g->accept();
            }
            if (SecondFingerGesture *g = (SecondFingerGesture*)e->gesture(SecondFingerGestureRecognizer::Name)) {
                gesture.last.secondfinger.delivered = true;
                gesture.last.secondfinger.startPoint = g->startPoint;
                gesture.last.secondfinger.lastPoint = g->lastPoint;
                gesture.last.secondfinger.point = g->point;
                gesture.last.secondfinger.offset = g->offset;
                if (shouldAcceptSecondFingerGesture)
                    g->accept();
            }
            gesture.last.cancelled = e->cancelledGestures();
            return true;
        }
        return QGraphicsItem::sceneEvent(event);
    }
};

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
    void singleshotGesture();
    void pinchGesture();

    void simplePropagation();
    void simplePropagation2();
    void acceptedGesturePropagation();

    void simpleGraphicsView();
    void simpleGraphicsItem();
    void overlappingGraphicsItems();

    void touch_widget();
    void touch_graphicsView();

    void panOnWidgets();

private:
    SingleshotGestureRecognizer *singleshotRecognizer;
    PinchGestureRecognizer *pinchRecognizer;
    SecondFingerGestureRecognizer *secondFingerRecognizer;
    PanGestureRecognizer *panGestureRecognizer;
    GestureWidget *mainWidget;

    void sendPinchEvents(QWidget *receiver, const QPoint &fromFinger1, const QPoint &fromFinger2);
};

tst_Gestures::tst_Gestures()
{
    singleshotRecognizer = new SingleshotGestureRecognizer;
    pinchRecognizer = new PinchGestureRecognizer;
    secondFingerRecognizer = new SecondFingerGestureRecognizer;
    panGestureRecognizer = new PanGestureRecognizer;
    qApp->addGestureRecognizer(singleshotRecognizer);
    qApp->addGestureRecognizer(pinchRecognizer);
    qApp->addGestureRecognizer(secondFingerRecognizer);
    qApp->addGestureRecognizer(panGestureRecognizer);
}

tst_Gestures::~tst_Gestures()
{
}


void tst_Gestures::initTestCase()
{
    mainWidget = new GestureWidget(GestureWidget::DoNotGrabGestures);
    mainWidget->setObjectName("MainGestureWidget");
    mainWidget->resize(500, 600);
    mainWidget->show();
}

void tst_Gestures::cleanupTestCase()
{
    delete mainWidget; mainWidget = 0;
}

void tst_Gestures::init()
{
    // TODO: Add initialization code here.
    // This will be executed immediately before each test is run.
    mainWidget->reset();
}

void tst_Gestures::cleanup()
{
}

bool sendSpontaneousEvent(QWidget *receiver, QEvent *event)
{
    QETWidget::setSpont(event, true);
    return qApp->notify(receiver, event);
}

void tst_Gestures::singleshotGesture()
{
    mainWidget->grabSingleshotGesture();
    SingleshotEvent event;
    sendSpontaneousEvent(mainWidget, &event);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.last.singleshot.delivered);
    QVERIFY(mainWidget->gesture.last.cancelled.isEmpty());
}

void tst_Gestures::sendPinchEvents(QWidget *receiver, const QPoint &fromFinger1, const QPoint &fromFinger2)
{
    int x1 = fromFinger1.x();
    int y1 = fromFinger1.x();
    int x2 = fromFinger2.x();
    int y2 = fromFinger2.x();

    TouchEvent event;
    event.points[0] = TouchPoint(0,x1,y1, TouchPoint::Begin);
    event.points[1] = TouchPoint();
    sendSpontaneousEvent(receiver, &event);
    event.points[0] = TouchPoint(0, x1+=2,y1+=2, TouchPoint::Update);
    event.points[1] = TouchPoint();
    sendSpontaneousEvent(receiver, &event);
    event.points[0] = TouchPoint(0, x1,y1, TouchPoint::Update);
    event.points[1] = TouchPoint(1, x2,y2, TouchPoint::Begin);
    sendSpontaneousEvent(receiver, &event);
    event.points[0] = TouchPoint(0, x1+=5,y1+=10, TouchPoint::End);
    event.points[1] = TouchPoint(1, x2+=3,y2+=6, TouchPoint::Update);
    sendSpontaneousEvent(receiver, &event);
    event.points[0] = TouchPoint();
    event.points[1] = TouchPoint(1, x2+=10,y2+=15, TouchPoint::Update);
    sendSpontaneousEvent(receiver, &event);
    event.points[0] = TouchPoint();
    event.points[1] = TouchPoint(1, x2,y2, TouchPoint::End);
    sendSpontaneousEvent(receiver, &event);
}

void tst_Gestures::pinchGesture()
{
    mainWidget->grabPinchGesture();
    sendPinchEvents(mainWidget, QPoint(10,10), QPoint(20,20));

    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.last.singleshot.delivered);
    QVERIFY(mainWidget->gesture.last.cancelled.isEmpty());
    QVERIFY(mainWidget->gesture.last.pinch.delivered);
    QCOMPARE(mainWidget->gesture.last.pinch.startPoints[0].pt, QPoint(10,10));
    QCOMPARE(mainWidget->gesture.last.pinch.startPoints[1].pt, QPoint(20,20));
    QCOMPARE(mainWidget->gesture.last.pinch.offset, QPoint(0,0));
}

void tst_Gestures::simplePropagation()
{
    mainWidget->grabSingleshotGesture();
    GestureWidget offsetWidget(GestureWidget::DoNotGrabGestures);
    offsetWidget.setFixedSize(30, 30);
    mainWidget->layout()->addWidget(&offsetWidget);
    GestureWidget nonGestureWidget(GestureWidget::DoNotGrabGestures);
    mainWidget->layout()->addWidget(&nonGestureWidget);
    QApplication::processEvents();

    SingleshotEvent event;
    sendSpontaneousEvent(&nonGestureWidget, &event);
    QVERIFY(!offsetWidget.gesture.seenGestureEvent);
    QVERIFY(!nonGestureWidget.gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.last.cancelled.isEmpty());
    QVERIFY(mainWidget->gesture.last.singleshot.delivered);
    QCOMPARE(mainWidget->gesture.last.singleshot.offset, QPoint(GestureWidget::LeftMargin, 30 + GestureWidget::TopMargin));
}

void tst_Gestures::simplePropagation2()
{
    mainWidget->grabSingleshotGesture();
    mainWidget->grabPinchGesture();
    GestureWidget nonGestureMiddleWidget(GestureWidget::DoNotGrabGestures);
    GestureWidget secondGestureWidget(GestureWidget::GrabPinch);
    nonGestureMiddleWidget.layout()->addWidget(&secondGestureWidget);
    mainWidget->layout()->addWidget(&nonGestureMiddleWidget);
    QApplication::processEvents();

    SingleshotEvent event;
    sendSpontaneousEvent(&secondGestureWidget, &event);
    QVERIFY(!secondGestureWidget.gesture.seenGestureEvent);
    QVERIFY(!nonGestureMiddleWidget.gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.last.singleshot.delivered);
    QVERIFY(mainWidget->gesture.last.cancelled.isEmpty());
    QCOMPARE(mainWidget->gesture.last.singleshot.offset, QPoint(GestureWidget::LeftMargin*2,GestureWidget::TopMargin*2));

    mainWidget->reset();
    nonGestureMiddleWidget.reset();
    secondGestureWidget.reset();

    sendPinchEvents(&secondGestureWidget, QPoint(10,10), QPoint(20,20));
    QVERIFY(secondGestureWidget.gesture.seenGestureEvent);
    QVERIFY(!secondGestureWidget.gesture.last.singleshot.delivered);
    QVERIFY(secondGestureWidget.gesture.last.pinch.delivered);
    QCOMPARE(secondGestureWidget.gesture.last.pinch.startPoints[0].pt, QPoint(10,10));
    QCOMPARE(secondGestureWidget.gesture.last.pinch.startPoints[1].pt, QPoint(20,20));
    QCOMPARE(secondGestureWidget.gesture.last.pinch.offset, QPoint(0,0));
    QVERIFY(!nonGestureMiddleWidget.gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.seenGestureEvent);
}

void tst_Gestures::acceptedGesturePropagation()
{
    mainWidget->grabSingleshotGesture();
    mainWidget->grabPinchGesture();
    mainWidget->grabSecondFingerGesture();
    GestureWidget nonGestureMiddleWidget(GestureWidget::DoNotGrabGestures);
    nonGestureMiddleWidget.setObjectName("nonGestureMiddleWidget");
    GestureWidget secondGestureWidget(GestureWidget::GrabSecondFinger);
    secondGestureWidget.setObjectName("secondGestureWidget");
    nonGestureMiddleWidget.layout()->addWidget(&secondGestureWidget);
    mainWidget->layout()->addWidget(&nonGestureMiddleWidget);
    QApplication::processEvents();

    sendPinchEvents(&secondGestureWidget, QPoint(10, 10), QPoint(40, 40));
    QVERIFY(secondGestureWidget.gesture.seenGestureEvent);
    QVERIFY(!secondGestureWidget.gesture.last.singleshot.delivered);
    QVERIFY(!secondGestureWidget.gesture.last.pinch.delivered);
    QVERIFY(secondGestureWidget.gesture.last.secondfinger.delivered);
    QCOMPARE(secondGestureWidget.gesture.last.secondfinger.startPoint.pt, QPoint(40,40));
    QVERIFY(!nonGestureMiddleWidget.gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.last.singleshot.delivered);
    QVERIFY(!mainWidget->gesture.last.secondfinger.delivered);
    QVERIFY(mainWidget->gesture.last.pinch.delivered);
    QCOMPARE(mainWidget->gesture.last.pinch.startPoints[0].pt, QPoint(10,10));
    QCOMPARE(mainWidget->gesture.last.pinch.startPoints[1].pt, QPoint(40,40));

    mainWidget->reset();
    nonGestureMiddleWidget.reset();
    secondGestureWidget.reset();

    // don't accept it and make sure it propagates to parent
    secondGestureWidget.shouldAcceptSecondFingerGesture = false;
    sendPinchEvents(&secondGestureWidget, QPoint(10, 10), QPoint(40, 40));
    QVERIFY(secondGestureWidget.gesture.seenGestureEvent);
    QVERIFY(secondGestureWidget.gesture.last.secondfinger.delivered);
    QVERIFY(!nonGestureMiddleWidget.gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.last.singleshot.delivered);
    QVERIFY(mainWidget->gesture.last.secondfinger.delivered);
    QVERIFY(mainWidget->gesture.last.pinch.delivered);
}

void tst_Gestures::simpleGraphicsView()
{
    mainWidget->grabSingleshotGesture();
    GraphicsScene scene;
    QGraphicsView view(&scene);
    view.grabGesture(SingleshotGestureRecognizer::Name);
    mainWidget->layout()->addWidget(&view);
    QApplication::processEvents();

    scene.shouldAcceptSingleshotGesture = true;

    SingleshotEvent event;
    sendSpontaneousEvent(&view, &event);
    QVERIFY(!mainWidget->gesture.seenGestureEvent);
    QVERIFY(scene.gesture.seenGestureEvent);
    QVERIFY(scene.gesture.last.singleshot.delivered);
    QVERIFY(scene.gesture.last.cancelled.isEmpty());
}

void tst_Gestures::simpleGraphicsItem()
{
    mainWidget->grabSingleshotGesture();
    GraphicsScene scene;
    QGraphicsView view(&scene);
    mainWidget->layout()->addWidget(&view);
    GraphicsItem *item = new GraphicsItem;
    item->grabSingleshotGesture();
    item->setPos(30, 50);
    scene.addItem(item);
    QApplication::processEvents();

    SingleshotEvent event(50, 80);
    sendSpontaneousEvent(&view, &event);
    QVERIFY(item->gesture.seenGestureEvent);
    QVERIFY(scene.gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.seenGestureEvent);

    item->reset();
    scene.reset();
    mainWidget->reset();

    item->shouldAcceptSingleshotGesture = false;
    SingleshotEvent event2(20, 40);
    sendSpontaneousEvent(&view, &event2);
    QVERIFY(!item->gesture.seenGestureEvent);
    QVERIFY(scene.gesture.seenGestureEvent);
    QVERIFY(mainWidget->gesture.seenGestureEvent);
}

void tst_Gestures::overlappingGraphicsItems()
{
    mainWidget->grabSingleshotGesture();
    GraphicsScene scene;
    QGraphicsView view(&scene);
    mainWidget->layout()->addWidget(&view);

    GraphicsItem *item = new GraphicsItem(300, 100);
    item->setPos(30, 50);
    scene.addItem(item);
    GraphicsItem *subitem1 = new GraphicsItem(50, 70);
    subitem1->setPos(70, 70);
    scene.addItem(subitem1);
    GraphicsItem *subitem2 = new GraphicsItem(50, 70);
    subitem2->setPos(250, 70);
    scene.addItem(subitem2);
    QApplication::processEvents();

    item->grabSingleshotGesture();
    item->grabPinchGesture();
    item->grabSecondFingerGesture();
    subitem1->grabSingleshotGesture();
    subitem2->grabSecondFingerGesture();

    SingleshotEvent event(100, 100);
    sendSpontaneousEvent(&view, &event);
    QVERIFY(subitem1->gesture.seenGestureEvent);
    QVERIFY(!subitem2->gesture.seenGestureEvent);
    QVERIFY(!item->gesture.seenGestureEvent);
    QVERIFY(scene.gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.seenGestureEvent);
    QVERIFY(subitem1->gesture.last.singleshot.delivered);

    item->reset();
    subitem1->reset();
    subitem2->reset();
    scene.reset();
    mainWidget->reset();

    subitem1->shouldAcceptSingleshotGesture = false;
    SingleshotEvent event2(100, 100);
    sendSpontaneousEvent(&view, &event2);
    QVERIFY(subitem1->gesture.seenGestureEvent);
    QVERIFY(!subitem2->gesture.seenGestureEvent);
    QVERIFY(item->gesture.seenGestureEvent);
    QVERIFY(scene.gesture.seenGestureEvent);
    QVERIFY(!mainWidget->gesture.seenGestureEvent);
    QVERIFY(subitem1->gesture.last.singleshot.delivered);
    QVERIFY(item->gesture.last.singleshot.delivered);
}

void tst_Gestures::touch_widget()
{
    GestureWidget leftWidget(GestureWidget::DoNotGrabGestures);
    leftWidget.setObjectName("leftWidget");
    leftWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    GestureWidget rightWidget(GestureWidget::DoNotGrabGestures);
    rightWidget.setObjectName("rightWidget");
    rightWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    delete mainWidget->layout();
    (void)new QHBoxLayout(mainWidget);
    mainWidget->layout()->addWidget(&leftWidget);
    mainWidget->layout()->addWidget(&rightWidget);
    QApplication::processEvents();

    QTest::touchEvent()
        .press(0, QPoint(10, 10), &leftWidget);
    QTest::touchEvent()
        .move(0, QPoint(12, 30), &leftWidget);
    QTest::touchEvent()
        .stationary(0)
        .press(1, QPoint(15, 15), &rightWidget);
    QTest::touchEvent()
        .move(0, QPoint(10, 35), &leftWidget)
        .press(1, QPoint(15, 15), &rightWidget);
    QTest::touchEvent()
        .move(0, QPoint(10, 40), &leftWidget)
        .move(1, QPoint(20, 50), &rightWidget);
    QTest::touchEvent()
        .release(0, QPoint(10, 40), &leftWidget)
        .release(1, QPoint(20, 50), &rightWidget);
    QVERIFY(!mainWidget->touch.seenTouchBeginEvent);
    QVERIFY(leftWidget.touch.seenTouchBeginEvent);
    QVERIFY(leftWidget.touch.seenTouchUpdateEvent);
    QVERIFY(leftWidget.touch.seenTouchEndEvent);
    QVERIFY(rightWidget.touch.seenTouchBeginEvent);
    QVERIFY(rightWidget.touch.seenTouchUpdateEvent);
    QVERIFY(rightWidget.touch.seenTouchEndEvent);
}

void tst_Gestures::touch_graphicsView()
{
    mainWidget->setAttribute(Qt::WA_AcceptTouchEvents);
    GraphicsScene scene;
    QGraphicsView view(&scene);
    view.viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
    mainWidget->layout()->addWidget(&view);

    GraphicsItem *item = new GraphicsItem(300, 100);
    item->setAcceptTouchEvents(true);
    item->setPos(30, 50);
    scene.addItem(item);
    GraphicsItem *subitem1 = new GraphicsItem(50, 70);
    subitem1->setAcceptTouchEvents(true);
    subitem1->setPos(70, 70);
    scene.addItem(subitem1);
    GraphicsItem *subitem2 = new GraphicsItem(50, 70);
    subitem2->setAcceptTouchEvents(true);
    subitem2->setPos(250, 70);
    scene.addItem(subitem2);
    QApplication::processEvents();

    QRect itemRect = view.mapFromScene(item->mapRectToScene(item->boundingRect())).boundingRect();
    QPoint pt = itemRect.center();
    QTest::touchEvent(view.viewport())
        .press(0, pt)
        .press(1, pt);
    QTest::touchEvent(view.viewport())
        .move(0, pt + QPoint(20, 30))
        .move(1, QPoint(300, 300));
    QTest::touchEvent(view.viewport())
        .stationary(0)
        .move(1, QPoint(330, 330));
    QTest::touchEvent(view.viewport())
        .release(0, QPoint(120, 120))
        .release(1, QPoint(300, 300));

    QVERIFY(item->touch.seenTouchBeginEvent);
    QVERIFY(item->touch.seenTouchUpdateEvent);
    QVERIFY(item->touch.seenTouchEndEvent);
}

void tst_Gestures::panOnWidgets()
{
    GestureWidget leftWidget(GestureWidget::GrabPan);
    leftWidget.setObjectName("leftWidget");
    leftWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    GestureWidget rightWidget(GestureWidget::GrabPan);
    rightWidget.setObjectName("rightWidget");
    rightWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    delete mainWidget->layout();
    (void)new QHBoxLayout(mainWidget);
    mainWidget->layout()->addWidget(&leftWidget);
    mainWidget->layout()->addWidget(&rightWidget);
    QApplication::processEvents();

    QTest::touchEvent()
        .press(0, QPoint(10, 10), &leftWidget);
    QTest::touchEvent()
        .move(0, QPoint(12, 30), &leftWidget);
    QTest::touchEvent()
        .stationary(0)
        .press(1, QPoint(15, 15), &rightWidget);
    QTest::touchEvent()
        .move(0, QPoint(10, 35), &leftWidget)
        .press(1, QPoint(15, 15), &rightWidget);
    QTest::touchEvent()
        .move(0, QPoint(10, 40), &leftWidget)
        .move(1, QPoint(20, 50), &rightWidget);
    QTest::touchEvent()
        .release(0, QPoint(10, 40), &leftWidget)
        .release(1, QPoint(20, 50), &rightWidget);

    QVERIFY(leftWidget.gesture.last.pan.delivered);
    QVERIFY(rightWidget.gesture.last.pan.delivered);
}

QTEST_MAIN(tst_Gestures)
#include "tst_gestures.moc"
