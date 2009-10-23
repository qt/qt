/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qstandardgestures_p.h"
#include "qgesture.h"
#include "qgesture_p.h"
#include "qevent.h"
#include "qwidget.h"

QT_BEGIN_NAMESPACE

QPanGestureRecognizer::QPanGestureRecognizer()
{
}

QGesture *QPanGestureRecognizer::createGesture(QObject *target)
{
    if (target && target->isWidgetType()) {
        static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
    }
    return new QPanGesture;
}

QGestureRecognizer::Result QPanGestureRecognizer::filterEvent(QGesture *state, QObject *, QEvent *event)
{
    QPanGesture *q = static_cast<QPanGesture*>(state);
    QPanGesturePrivate *d = q->d_func();

    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);

    QGestureRecognizer::Result result;

    switch (event->type()) {
    case QEvent::TouchBegin: {
        result = QGestureRecognizer::MaybeGesture;
        QTouchEvent::TouchPoint p = ev->touchPoints().at(0);
        d->lastPosition = p.pos().toPoint();
        d->lastOffset = d->totalOffset = d->offset = QPointF();
        break;
    }
    case QEvent::TouchEnd: {
        if (q->state() != Qt::NoGesture) {
            if (ev->touchPoints().size() == 2) {
                QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
                QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);
                d->lastOffset = d->offset;
                d->offset =
                        QPointF(p1.pos().x() - p1.lastPos().x() + p2.pos().x() - p2.lastPos().x(),
                              p1.pos().y() - p1.lastPos().y() + p2.pos().y() - p2.lastPos().y()) / 2;
                d->totalOffset += d->offset;
            }
            result = QGestureRecognizer::GestureFinished;
        } else {
            result = QGestureRecognizer::NotGesture;
        }
        break;
    }
    case QEvent::TouchUpdate: {
        if (ev->touchPoints().size() >= 2) {
            QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
            QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);
            d->lastOffset = d->offset;
            d->offset =
                    QPointF(p1.pos().x() - p1.lastPos().x() + p2.pos().x() - p2.lastPos().x(),
                          p1.pos().y() - p1.lastPos().y() + p2.pos().y() - p2.lastPos().y()) / 2;
            d->totalOffset += d->offset;
            if (d->totalOffset.x() > 10  || d->totalOffset.y() > 10 ||
                d->totalOffset.x() < -10 || d->totalOffset.y() < -10) {
                result = QGestureRecognizer::GestureTriggered;
            } else {
                result = QGestureRecognizer::MaybeGesture;
            }
        }
        break;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        result = QGestureRecognizer::Ignore;
        break;
    default:
        result = QGestureRecognizer::Ignore;
        break;
    }
    return result;
}

void QPanGestureRecognizer::reset(QGesture *state)
{
    QPanGesture *pan = static_cast<QPanGesture*>(state);
    QPanGesturePrivate *d = pan->d_func();

    d->totalOffset = d->lastOffset = d->offset = QPointF();
    d->lastPosition = QPoint();
    d->acceleration = 0;

//#if defined(QT_MAC_USE_COCOA)
//    d->singleTouchPanTimer.stop();
//    d->prevMousePos = QPointF(0, 0);
//#endif

    QGestureRecognizer::reset(state);
}

/*! \internal */
/*
bool QPanGestureRecognizer::event(QEvent *event)
{
#if defined(QT_MAC_USE_COCOA)
    Q_D(QPanGesture);
    if (event->type() == QEvent::Timer) {
        const QTimerEvent *te = static_cast<QTimerEvent *>(event);
        if (te->timerId() == d->singleTouchPanTimer.timerId()) {
            d->singleTouchPanTimer.stop();
            updateState(Qt::GestureStarted);
        }
    }
#endif

    bool consume = false;

#if defined(Q_WS_WIN)
#elif defined(QT_MAC_USE_COCOA)
    // The following implements single touch
    // panning on Mac:
    const int panBeginDelay = 300;
    const int panBeginRadius = 3;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);

    switch (event->type()) {
    case QEvent::TouchBegin: {
        if (ev->touchPoints().size() == 1) {
            d->delayManager->setEnabled(true);
            consume = d->delayManager->append(d->gestureTarget, *event);
            d->lastPosition = QCursor::pos();
            d->singleTouchPanTimer.start(panBeginDelay, this);
        }
        break;}
    case QEvent::TouchEnd: {
        d->delayManager->setEnabled(false);
        if (state() != Qt::NoGesture) {
            updateState(Qt::GestureFinished);
            consume = true;
            d->delayManager->clear();
        } else {
            d->delayManager->replay();
        }
        reset();
        break;}
    case QEvent::TouchUpdate: {
        consume = d->delayManager->append(d->gestureTarget, *event);
        if (ev->touchPoints().size() == 1) {
            if (state() == Qt::NoGesture) {
                // INVARIANT: The singleTouchTimer has still not fired.
                // Lets check if the user moved his finger so much from
                // the starting point that it makes sense to cancel:
                const QPointF startPos = ev->touchPoints().at(0).startPos().toPoint();
                const QPointF p = ev->touchPoints().at(0).pos().toPoint();
                if ((startPos - p).manhattanLength() > panBeginRadius) {
                    d->delayManager->replay();
                    consume = false;
                    reset();
                } else {
                    d->lastPosition = QCursor::pos();
                }
            } else {
                d->delayManager->clear();
                QPointF mousePos = QCursor::pos();
                QPointF dist = mousePos - d->lastPosition;
                d->lastPosition = mousePos;
                d->lastOffset = d->offset;
                d->offset = QSizeF(dist.x(), dist.y());
                d->totalOffset += d->offset;
                updateState(Qt::GestureUpdated);
            }
        } else if (state() == Qt::NoGesture) {
            d->delayManager->replay();
            consume = false;
            reset();
        }
        break;}
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease:
        if (d->delayManager->isEnabled())
            consume = d->delayManager->append(d->gestureTarget, *event);
        break;
    default:
        return false;
    }
#else
    Q_UNUSED(event);
#endif
    return QGestureRecognizer::Ignore;
}
    */

QT_END_NAMESPACE
