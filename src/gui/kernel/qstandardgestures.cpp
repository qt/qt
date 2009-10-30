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
#include "qabstractscrollarea.h"

QT_BEGIN_NAMESPACE

QPanGestureRecognizer::QPanGestureRecognizer()
{
}

QGesture *QPanGestureRecognizer::createGesture(QObject *target)
{
    if (target && target->isWidgetType()) {
#if defined(Q_OS_WIN)
        // for scroll areas on Windows we want to use native gestures instead
        if (!qobject_cast<QAbstractScrollArea *>(target->parent()))
            static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
#else
        static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
#endif
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

    QGestureRecognizer::reset(state);
}


//
// QPinchGestureRecognizer
//

QPinchGestureRecognizer::QPinchGestureRecognizer()
{
}

QGesture *QPinchGestureRecognizer::createGesture(QObject *target)
{
    if (target && target->isWidgetType()) {
        static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
    }
    return new QPinchGesture;
}

QGestureRecognizer::Result QPinchGestureRecognizer::filterEvent(QGesture *state, QObject *, QEvent *event)
{
    QPinchGesture *q = static_cast<QPinchGesture *>(state);
    QPinchGesturePrivate *d = q->d_func();

    const QTouchEvent *ev = static_cast<const QTouchEvent *>(event);

    QGestureRecognizer::Result result;

    switch (event->type()) {
    case QEvent::TouchBegin: {
        result = QGestureRecognizer::MaybeGesture;
        break;
    }
    case QEvent::TouchEnd: {
        if (q->state() != Qt::NoGesture) {
            result = QGestureRecognizer::GestureFinished;
        } else {
            result = QGestureRecognizer::NotGesture;
        }
        break;
    }
    case QEvent::TouchUpdate: {
        d->whatChanged = 0;
        if (ev->touchPoints().size() == 2) {
            QTouchEvent::TouchPoint p1 = ev->touchPoints().at(0);
            QTouchEvent::TouchPoint p2 = ev->touchPoints().at(1);

            d->hotSpot = p1.screenPos();
            d->isHotSpotSet = true;

            if (d->isNewSequence) {
                d->startPosition[0] = p1.screenPos();
                d->startPosition[1] = p2.screenPos();
            }
            QLineF line(p1.screenPos(), p2.screenPos());
            QLineF tmp(line);
            tmp.setLength(line.length() / 2.);
            QPointF centerPoint = tmp.p2();

            d->lastCenterPoint = d->centerPoint;
            d->centerPoint = centerPoint;
            d->whatChanged |= QPinchGesture::CenterPointChanged;

            const qreal scaleFactor = QLineF(p1.pos(), p2.pos()).length()
                                      / QLineF(d->startPosition[0],  d->startPosition[1]).length();
            if (d->isNewSequence) {
                d->lastScaleFactor = scaleFactor;
            } else {
                d->lastScaleFactor = d->scaleFactor;
            }
            d->scaleFactor = scaleFactor;
            d->totalScaleFactor += d->scaleFactor - d->lastScaleFactor;
            d->whatChanged |= QPinchGesture::ScaleFactorChanged;

            const qreal rotationAngle = -line.angle();
            if (d->isNewSequence)
                d->lastRotationAngle = rotationAngle;
            else
                d->lastRotationAngle = d->rotationAngle;
            d->rotationAngle = rotationAngle;
            d->totalRotationAngle += d->rotationAngle - d->lastRotationAngle;
            d->whatChanged |= QPinchGesture::RotationAngleChanged;

            d->isNewSequence = false;
            result = QGestureRecognizer::GestureTriggered;
        } else {
            d->isNewSequence = true;
            result = QGestureRecognizer::MaybeGesture;
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

void QPinchGestureRecognizer::reset(QGesture *state)
{
    QPinchGesture *pinch = static_cast<QPinchGesture*>(state);
    QPinchGesturePrivate *d = pinch->d_func();

    d->whatChanged = 0;

    d->startCenterPoint = d->lastCenterPoint = d->centerPoint = QPointF();
    d->totalScaleFactor = d->lastScaleFactor = d->scaleFactor = 0;
    d->totalRotationAngle = d->lastRotationAngle = d->rotationAngle = 0;

    d->isNewSequence = true;

    QGestureRecognizer::reset(state);
}

QT_END_NAMESPACE
