/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "customgesturerecognizer.h"
#include "qgesture.h"

const char* SingleshotGestureRecognizer::Name = "SingleshotGesture";
const char* PinchGestureRecognizer::Name = "PinchGesture";
const char* SecondFingerGestureRecognizer::Name = "SecondFingerGesture";
const char* PanGestureRecognizer::Name = "PanGesture";

SingleshotGestureRecognizer::SingleshotGestureRecognizer(QObject *parent)
    : QGestureRecognizer(QString(SingleshotGestureRecognizer::Name), parent)
{
    gesture = new SingleshotGesture(this, SingleshotGestureRecognizer::Name);
}

QGestureRecognizer::Result SingleshotGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() == SingleshotEvent::Type) {
        gesture->setHotSpot(static_cast<const SingleshotEvent*>(event)->point);
        return QGestureRecognizer::GestureFinished;
    }
    return QGestureRecognizer::NotGesture;
}

void SingleshotGestureRecognizer::reset()
{
    gesture->setHotSpot(QPoint());
    gesture->offset = QPoint();
}

PinchGestureRecognizer::PinchGestureRecognizer(QObject *parent)
    : QGestureRecognizer(PinchGestureRecognizer::Name, parent)
{
    gesture = new PinchGesture(this, PinchGestureRecognizer::Name);
}

QGestureRecognizer::Result PinchGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() != TouchEvent::Type)
        return QGestureRecognizer::Ignore;

    const TouchEvent *e = static_cast<const TouchEvent*>(event);
    if (e->points[0].state == TouchPoint::Begin)
        gesture->startPoints[0] = e->points[0];
    if (e->points[1].state == TouchPoint::Begin)
        gesture->startPoints[1] = e->points[1];
    gesture->lastPoints[0] = gesture->points[0];
    gesture->lastPoints[1] = gesture->points[1];
    gesture->points[0] = e->points[0];
    gesture->points[1] = e->points[1];
    if (((e->points[0].state == TouchPoint::Begin || e->points[0].state == TouchPoint::Update))) {
        if (e->points[1].state == TouchPoint::End || e->points[1].state == TouchPoint::None)
            return MaybeGesture;
        return GestureStarted;
    } else if (((e->points[1].state == TouchPoint::Begin || e->points[1].state == TouchPoint::Update))) {
        if (e->points[0].state == TouchPoint::End || e->points[0].state == TouchPoint::None)
            return MaybeGesture;
        return GestureStarted;
    } else if ((e->points[0].state == TouchPoint::End && e->points[1].state == TouchPoint::End) ||
               (e->points[0].state == TouchPoint::End && e->points[1].state == TouchPoint::None) ||
               (e->points[0].state == TouchPoint::None && e->points[1].state == TouchPoint::End)) {
        return QGestureRecognizer::NotGesture;
    }
    return QGestureRecognizer::NotGesture;
}

void PinchGestureRecognizer::reset()
{
    gesture->startPoints[0] = TouchPoint();
    gesture->startPoints[1] = TouchPoint();
    gesture->lastPoints[0] = TouchPoint();
    gesture->lastPoints[1] = TouchPoint();
    gesture->points[0] = TouchPoint();
    gesture->points[1] = TouchPoint();
    gesture->offset = QPoint();
}

SecondFingerGestureRecognizer::SecondFingerGestureRecognizer(QObject *parent)
    : QGestureRecognizer(SecondFingerGestureRecognizer::Name, parent)
{
    gesture = new SecondFingerGesture(this, SecondFingerGestureRecognizer::Name);
}

QGestureRecognizer::Result SecondFingerGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() != TouchEvent::Type)
        return QGestureRecognizer::Ignore;

    const TouchEvent *e = static_cast<const TouchEvent*>(event);
    if (e->points[1].state != TouchPoint::None) {
        if (e->points[1].state == TouchPoint::Begin)
            gesture->startPoint = e->points[1];
        gesture->lastPoint = gesture->point;
        gesture->point = e->points[1];
        if (e->points[1].state == TouchPoint::End)
            return QGestureRecognizer::GestureFinished;
        else if (e->points[1].state != TouchPoint::None)
            return QGestureRecognizer::GestureStarted;
    }
    return QGestureRecognizer::NotGesture;
}

void SecondFingerGestureRecognizer::reset()
{
    gesture->startPoint = TouchPoint();
    gesture->lastPoint = TouchPoint();
    gesture->point = TouchPoint();
    gesture->offset = QPoint();
}

PanGestureRecognizer::PanGestureRecognizer(QObject *parent)
    : QGestureRecognizer(PanGestureRecognizer::Name, parent)
{
    gesture = new PanGesture(this, PanGestureRecognizer::Name);
}

QGestureRecognizer::Result PanGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() != QEvent::TouchBegin &&
        event->type() != QEvent::TouchUpdate &&
        event->type() != QEvent::TouchEnd)
        return QGestureRecognizer::Ignore;

    const QTouchEvent *e = static_cast<const QTouchEvent*>(event);
    const QList<QTouchEvent::TouchPoint> &points = e->touchPoints();

    if (points.size() >= 1) {
        gesture->lastPoints[0] = gesture->points[0];
        gesture->points[0].id = points.at(0).id();
        gesture->points[0].pt = points.at(0).startPos().toPoint();
        gesture->points[0].state = (TouchPoint::State)points.at(0).state();
        if (points.at(0).state() == Qt::TouchPointPressed) {
            gesture->startPoints[0] = gesture->points[0];
            gesture->lastPoints[0] = gesture->points[0];
        }
    }
    if (points.size() >= 2) {
        gesture->lastPoints[1] = gesture->points[1];
        gesture->points[1].id = points.at(1).id();
        gesture->points[1].pt = points.at(1).startPos().toPoint();
        gesture->points[1].state = (TouchPoint::State)points.at(1).state();
        if (points.at(1).state() == Qt::TouchPointPressed) {
            gesture->startPoints[1] = gesture->points[1];
            gesture->lastPoints[1] = gesture->points[1];
        }
    }

    if (points.size() == 2)
        return QGestureRecognizer::GestureStarted;
    if (points.size() > 2)
        return QGestureRecognizer::MaybeGesture;
    if (points.at(0).state() == Qt::TouchPointPressed)
        return QGestureRecognizer::MaybeGesture;
    if (points.at(0).state() == Qt::TouchPointReleased)
        return QGestureRecognizer::GestureFinished;
    return QGestureRecognizer::GestureStarted;
}

void PanGestureRecognizer::reset()
{
    gesture->startPoints[0] = TouchPoint();
    gesture->startPoints[1] = TouchPoint();
    gesture->lastPoints[0] = TouchPoint();
    gesture->lastPoints[1] = TouchPoint();
    gesture->points[0] = TouchPoint();
    gesture->points[1] = TouchPoint();
    gesture->offset = QPoint();
}
