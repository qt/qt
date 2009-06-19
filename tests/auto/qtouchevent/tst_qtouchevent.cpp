/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtTest>

class tst_QTouchEventWidget : public QWidget
{
public:
    QList<QTouchEvent::TouchPoint> touchBeginPoints, touchUpdatePoints, touchEndPoints;
    bool seenTouchBegin, seenTouchUpdate, seenTouchEnd;
    bool acceptTouchBegin, acceptTouchUpdate, acceptTouchEnd;

    tst_QTouchEventWidget()
        : QWidget()
    {
        reset();
    }

    void reset()
    {
        touchBeginPoints.clear();
        touchUpdatePoints.clear();
        touchEndPoints.clear();
        seenTouchBegin = seenTouchUpdate = seenTouchEnd = false;
        acceptTouchBegin = acceptTouchUpdate = acceptTouchEnd = true;
    }

    bool event(QEvent *event)
    {
        switch (event->type()) {
        case QEvent::TouchBegin:
            if (seenTouchBegin) qWarning("TouchBegin: already seen a TouchBegin");
            if (seenTouchUpdate) qWarning("TouchBegin: TouchUpdate cannot happen before TouchBegin");
            if (seenTouchEnd) qWarning("TouchBegin: TouchEnd cannot happen before TouchBegin");
            seenTouchBegin = !seenTouchBegin && !seenTouchUpdate && !seenTouchEnd;
            touchBeginPoints = static_cast<QTouchEvent *>(event)->touchPoints();
            event->setAccepted(acceptTouchBegin);
            break;
        case QEvent::TouchUpdate:
            if (!seenTouchBegin) qWarning("TouchUpdate: have not seen TouchBegin");
            if (seenTouchEnd) qWarning("TouchUpdate: TouchEnd cannot happen before TouchUpdate");
            seenTouchUpdate = seenTouchBegin && !seenTouchEnd;
            touchUpdatePoints = static_cast<QTouchEvent *>(event)->touchPoints();
            event->setAccepted(acceptTouchUpdate);
            break;
        case QEvent::TouchEnd:
            if (!seenTouchBegin) qWarning("TouchEnd: have not seen TouchBegin");
            if (seenTouchEnd) qWarning("TouchEnd: already seen a TouchEnd");
            seenTouchEnd = seenTouchBegin && !seenTouchEnd;
            touchEndPoints = static_cast<QTouchEvent *>(event)->touchPoints();
            event->setAccepted(acceptTouchEnd);
            break;
        default:
            return QWidget::event(event);
        }
        return true;
    }
};

class tst_QTouchEvent : public QObject
{
    Q_OBJECT
public:
    tst_QTouchEvent() { }
    ~tst_QTouchEvent() { }

private slots:
    void touchDisabledByDefault();
    void touchEventAcceptedByDefault();
    void touchBeginPropagatesWhenIgnored();
    void touchUpdateAndEndNeverPropagate();
    void basicRawEventTranslation();
};

void tst_QTouchEvent::touchDisabledByDefault()
{
    // the widget attribute is not enabled by default
    QWidget widget;
    QVERIFY(!widget.testAttribute(Qt::WA_AcceptTouchEvents));

    // events should not be accepted since they are not enabled
    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchEvent(QEvent::TouchBegin,
                           Qt::NoModifier,
                           Qt::TouchPointPressed,
                           touchPoints);
    bool res = QApplication::sendEvent(&widget, &touchEvent);
    QVERIFY(!res);
    QVERIFY(!touchEvent.isAccepted());
}

void tst_QTouchEvent::touchEventAcceptedByDefault()
{
    // enabling touch events should automatically accept touch events
    QWidget widget;
    widget.setAttribute(Qt::WA_AcceptTouchEvents);

    // QWidget handles touch event by converting them into a mouse event, so the event is both
    // accepted and handled (res == true)
    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchEvent(QEvent::TouchBegin,
                           Qt::NoModifier,
                           Qt::TouchPointPressed,
                           touchPoints);
    bool res = QApplication::sendEvent(&widget, &touchEvent);
    QVERIFY(res);
    QVERIFY(touchEvent.isAccepted());

    // tst_QTouchEventWidget does handle, sending succeeds
    tst_QTouchEventWidget touchWidget;
    touchWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    touchEvent.ignore();
    res = QApplication::sendEvent(&touchWidget, &touchEvent);
    QVERIFY(res);
    QVERIFY(touchEvent.isAccepted());
}

void tst_QTouchEvent::touchBeginPropagatesWhenIgnored()
{
    tst_QTouchEventWidget window, child, grandchild;
    child.setParent(&window);
    grandchild.setParent(&child);

    // all widgets accept touch events, grandchild ignores, so child sees the event, but not window
    window.setAttribute(Qt::WA_AcceptTouchEvents);
    child.setAttribute(Qt::WA_AcceptTouchEvents);
    grandchild.setAttribute(Qt::WA_AcceptTouchEvents);
    grandchild.acceptTouchBegin = false;

    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchEvent(QEvent::TouchBegin,
                           Qt::NoModifier,
                           Qt::TouchPointPressed,
                           touchPoints);
    bool res = QApplication::sendEvent(&grandchild, &touchEvent);
    QVERIFY(res);
    QVERIFY(touchEvent.isAccepted());
    QVERIFY(grandchild.seenTouchBegin);
    QVERIFY(child.seenTouchBegin);
    QVERIFY(!window.seenTouchBegin);

    // disable touch on grandchild. even though it doesn't accept it, child should still get the
    // TouchBegin
    grandchild.reset();
    child.reset();
    window.reset();
    grandchild.setAttribute(Qt::WA_AcceptTouchEvents, false);

    touchEvent.ignore();
    res = QApplication::sendEvent(&grandchild, &touchEvent);
    QVERIFY(res);
    QVERIFY(touchEvent.isAccepted());
    QVERIFY(!grandchild.seenTouchBegin);
    QVERIFY(child.seenTouchBegin);
    QVERIFY(!window.seenTouchBegin);
}

void tst_QTouchEvent::touchUpdateAndEndNeverPropagate()
{
    tst_QTouchEventWidget window, child;
    child.setParent(&window);

    window.setAttribute(Qt::WA_AcceptTouchEvents);
    child.setAttribute(Qt::WA_AcceptTouchEvents);
    child.acceptTouchUpdate = false;
    child.acceptTouchEnd = false;

    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchBeginEvent(QEvent::TouchBegin,
                                Qt::NoModifier,
                                Qt::TouchPointPressed,
                                touchPoints);
    bool res = QApplication::sendEvent(&child, &touchBeginEvent);
    QVERIFY(res);
    QVERIFY(touchBeginEvent.isAccepted());
    QVERIFY(child.seenTouchBegin);
    QVERIFY(!window.seenTouchBegin);

    // send the touch update to the child, but ignore it, it doesn't propagate
    QTouchEvent touchUpdateEvent(QEvent::TouchUpdate,
                                 Qt::NoModifier,
                                 Qt::TouchPointMoved,
                                 touchPoints);
    res = QApplication::sendEvent(&child, &touchUpdateEvent);
    QVERIFY(res);
    QVERIFY(!touchUpdateEvent.isAccepted());
    QVERIFY(child.seenTouchUpdate);
    QVERIFY(!window.seenTouchUpdate);

    // send the touch end, same thing should happen as with touch update
    QTouchEvent touchEndEvent(QEvent::TouchEnd,
                                 Qt::NoModifier,
                                 Qt::TouchPointReleased,
                                 touchPoints);
    res = QApplication::sendEvent(&child, &touchEndEvent);
    QVERIFY(res);
    QVERIFY(!touchEndEvent.isAccepted());
    QVERIFY(child.seenTouchEnd);
    QVERIFY(!window.seenTouchEnd);
}

void tst_QTouchEvent::basicRawEventTranslation()
{
    tst_QTouchEventWidget touchWidget;
    touchWidget.setAttribute(Qt::WA_AcceptTouchEvents);
    touchWidget.setGeometry(100, 100, 400, 300);

    QPointF pos = touchWidget.rect().center();
    QPointF screenPos = touchWidget.mapToGlobal(pos.toPoint());
    QPointF delta(10, 10);

    QTouchEvent::TouchPoint rawTouchPoint;
    rawTouchPoint.setId(0);

    // this should be translated to a TouchBegin
    rawTouchPoint.setState(Qt::TouchPointPressed);
    rawTouchPoint.setScreenPos(screenPos);
    qt_translateRawTouchEvent(QList<QTouchEvent::TouchPoint>() << rawTouchPoint, &touchWidget);
    QVERIFY(touchWidget.seenTouchBegin);
    QVERIFY(!touchWidget.seenTouchUpdate);
    QVERIFY(!touchWidget.seenTouchEnd);
    QCOMPARE(touchWidget.touchBeginPoints.count(), 1);
    QTouchEvent::TouchPoint touchBeginPoint = touchWidget.touchBeginPoints.first();
    QCOMPARE(touchBeginPoint.id(), rawTouchPoint.id());
    QCOMPARE(touchBeginPoint.state(), rawTouchPoint.state());
    QCOMPARE(touchBeginPoint.pos(), pos);
    QCOMPARE(touchBeginPoint.startPos(), pos);
    QCOMPARE(touchBeginPoint.lastPos(), pos);
    QCOMPARE(touchBeginPoint.scenePos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.startScenePos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.lastScenePos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.screenPos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.startScreenPos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.lastScreenPos(), rawTouchPoint.screenPos());
    QCOMPARE(touchBeginPoint.rect(), QRectF(pos, QSizeF(0, 0)));
    QCOMPARE(touchBeginPoint.screenRect(), QRectF(rawTouchPoint.screenPos(), QSizeF(0, 0)));
    QCOMPARE(touchBeginPoint.sceneRect(), touchBeginPoint.screenRect());
    QCOMPARE(touchBeginPoint.pressure(), qreal(-1.));

    // moving the point should translate to TouchUpdate
    rawTouchPoint.setState(Qt::TouchPointMoved);
    rawTouchPoint.setScreenPos(screenPos + delta);
    qt_translateRawTouchEvent(QList<QTouchEvent::TouchPoint>() << rawTouchPoint, &touchWidget);
    QVERIFY(touchWidget.seenTouchBegin);
    QVERIFY(touchWidget.seenTouchUpdate);
    QVERIFY(!touchWidget.seenTouchEnd);
    QCOMPARE(touchWidget.touchUpdatePoints.count(), 1);
    QTouchEvent::TouchPoint touchUpdatePoint = touchWidget.touchUpdatePoints.first();
    QCOMPARE(touchUpdatePoint.id(), rawTouchPoint.id());
    QCOMPARE(touchUpdatePoint.state(), rawTouchPoint.state());
    QCOMPARE(touchUpdatePoint.pos(), pos + delta);
    QCOMPARE(touchUpdatePoint.startPos(), pos);
    QCOMPARE(touchUpdatePoint.lastPos(), pos);
    QCOMPARE(touchUpdatePoint.scenePos(), rawTouchPoint.screenPos());
    QCOMPARE(touchUpdatePoint.startScenePos(), screenPos);
    QCOMPARE(touchUpdatePoint.lastScenePos(), screenPos);
    QCOMPARE(touchUpdatePoint.screenPos(), rawTouchPoint.screenPos());
    QCOMPARE(touchUpdatePoint.startScreenPos(), screenPos);
    QCOMPARE(touchUpdatePoint.lastScreenPos(), screenPos);
    QCOMPARE(touchUpdatePoint.rect(), QRectF(pos + delta, QSizeF(0, 0)));
    QCOMPARE(touchUpdatePoint.screenRect(), QRectF(rawTouchPoint.screenPos(), QSizeF(0, 0)));
    QCOMPARE(touchUpdatePoint.sceneRect(), touchUpdatePoint.screenRect());
    QCOMPARE(touchUpdatePoint.pressure(), qreal(-1.));

    // releasing the point translates to TouchEnd
    rawTouchPoint.setState(Qt::TouchPointReleased);
    rawTouchPoint.setScreenPos(screenPos + delta + delta);
    qt_translateRawTouchEvent(QList<QTouchEvent::TouchPoint>() << rawTouchPoint, &touchWidget);
    QVERIFY(touchWidget.seenTouchBegin);
    QVERIFY(touchWidget.seenTouchUpdate);
    QVERIFY(touchWidget.seenTouchEnd);
    QCOMPARE(touchWidget.touchEndPoints.count(), 1);
    QTouchEvent::TouchPoint touchEndPoint = touchWidget.touchEndPoints.first();
    QCOMPARE(touchEndPoint.id(), rawTouchPoint.id());
    QCOMPARE(touchEndPoint.state(), rawTouchPoint.state());
    QCOMPARE(touchEndPoint.pos(), pos + delta + delta);
    QCOMPARE(touchEndPoint.startPos(), pos);
    QCOMPARE(touchEndPoint.lastPos(), pos + delta);
    QCOMPARE(touchEndPoint.scenePos(), rawTouchPoint.screenPos());
    QCOMPARE(touchEndPoint.startScenePos(), screenPos);
    QCOMPARE(touchEndPoint.lastScenePos(), screenPos + delta);
    QCOMPARE(touchEndPoint.screenPos(), rawTouchPoint.screenPos());
    QCOMPARE(touchEndPoint.startScreenPos(), screenPos);
    QCOMPARE(touchEndPoint.lastScreenPos(), screenPos + delta);
    QCOMPARE(touchEndPoint.rect(), QRectF(pos + delta + delta, QSizeF(0, 0)));
    QCOMPARE(touchEndPoint.screenRect(), QRectF(rawTouchPoint.screenPos(), QSizeF(0, 0)));
    QCOMPARE(touchEndPoint.sceneRect(), touchEndPoint.screenRect());
    QCOMPARE(touchEndPoint.pressure(), qreal(-1.));
}

QTEST_MAIN(tst_QTouchEvent)

#include "tst_qtouchevent.moc"      
