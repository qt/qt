/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qgesturestandardrecognizers_p.h"
#include "qgesture_p.h"
#include "qgesturerecognizer_p.h"

// #define GESTURE_RECOGNIZER_DEBUG
#ifndef GESTURE_RECOGNIZER_DEBUG
# define DEBUG if (0) qDebug
#else
# define DEBUG qDebug
#endif

QT_BEGIN_NAMESPACE

QString qt_getStandardGestureTypeName(Qt::GestureType gestureType)
{
    switch (gestureType) {
    case Qt::TapGesture:
        return QLatin1String("__QTapGesture");
    case Qt::DoubleTapGesture:
        return QLatin1String("__QDoubleTapGesture");
    case Qt::TrippleTapGesture:
        return QLatin1String("__QTrippleTapGesture");
    case Qt::TapAndHoldGesture:
        return QLatin1String("__QTapAndHoldGesture");
    case Qt::PanGesture:
        return QLatin1String("__QPanGesture");
    case Qt::PinchGesture:
        return QLatin1String("__QPinchGesture");
    case Qt::UnknownGesture:
        break;
    }
    qFatal("QGestureRecognizer::gestureType: got an unhandled gesture type.");
    return QLatin1String("__unknown_gesture");
}

//
// QGestureRecognizerPan
//

QGestureRecognizerPan::QGestureRecognizerPan(QObject *parent)
    : QGestureRecognizer(QString(), parent),
      mousePressed(false), gestureState(Qt::NoGesture),
      lastDirection(Qt::NoDirection), currentDirection(Qt::NoDirection)
{
    Q_D(QGestureRecognizer);
    d->gestureType = Qt::PanGesture;
}

QGestureRecognizer::Result QGestureRecognizerPan::filterEvent(const QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (currentDirection != Qt::NoDirection) {
            DEBUG() << "Pan: MouseButtonPress: fail. another press during pan";
            reset();
            return QGestureRecognizer::NotGesture;
        }
        if (ev->button() != Qt::LeftButton) {
            return QGestureRecognizer::NotGesture;
        }
        DEBUG() << "Pan: MouseButtonPress: maybe gesture started";
        mousePressed = true;
        pressedPos = lastPos = currentPos = ev->pos();
        return QGestureRecognizer::MaybeGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (mousePressed && currentDirection != Qt::NoDirection
            && ev->button() == Qt::LeftButton) {
            DEBUG() << "Pan: MouseButtonRelease: pan detected";
            gestureState = Qt::GestureFinished;
            currentPos = ev->pos();
            internalReset();
            return QGestureRecognizer::GestureFinished;
        }
        DEBUG() << "Pan: MouseButtonRelease: some weird release detected, ignoring";
        reset();
        return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseMove) {
        if (!mousePressed)
            return QGestureRecognizer::NotGesture;
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        lastPos = currentPos;
        currentPos = ev->pos();
        Qt::DirectionType newDirection =
            simpleRecognizer.addPosition(ev->pos()).direction;
        DEBUG() << "Pan: MouseMove: simplerecognizer result = " << newDirection;
        QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
        if (currentDirection == Qt::NoDirection) {
            if (newDirection == Qt::NoDirection) {
                result = QGestureRecognizer::MaybeGesture;
            } else {
                result = QGestureRecognizer::GestureStarted;
                gestureState = Qt::GestureStarted;
            }
        } else {
            result = QGestureRecognizer::GestureStarted;
            gestureState = Qt::GestureUpdated;
        }
        if (newDirection != Qt::NoDirection) {
            if (currentDirection != newDirection)
                lastDirection = currentDirection;
            currentDirection = newDirection;
        }
        return result;
    }
    return QGestureRecognizer::Ignore;
}

QGesture* QGestureRecognizerPan::getGesture()
{
    if (currentDirection == Qt::NoDirection)
        return 0;
    QPanningGesturePrivate *d = gesture.d_func();
    d->startPos = pressedPos;
    d->lastPos = lastPos;
    d->pos = currentPos;
    d->hotSpot = pressedPos;
    d->state = gestureState;
    d->lastDirection = lastDirection;
    d->direction = currentDirection;
    
    return &gesture;
}

void QGestureRecognizerPan::reset()
{
    mousePressed = false;
    lastDirection = Qt::NoDirection;
    currentDirection = Qt::NoDirection;
    gestureState = Qt::NoGesture;
    diagonalRecognizer.reset();
    simpleRecognizer.reset();
}

void QGestureRecognizerPan::internalReset()
{
    mousePressed = false;
    diagonalRecognizer.reset();
    simpleRecognizer.reset();
}


//
// QDoubleTapGestureRecognizer
//
QDoubleTapGestureRecognizer::QDoubleTapGestureRecognizer(QObject *parent)
    : QGestureRecognizer(QString(), parent),
      gesture(0, qt_getStandardGestureTypeName(Qt::DoubleTapGesture))
{
    Q_D(QGestureRecognizer);
    d->gestureType = Qt::DoubleTapGesture;
}

QGestureRecognizer::Result QDoubleTapGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (pressedPosition.isNull()) {
            pressedPosition = ev->pos();
            return QGestureRecognizer::MaybeGesture;
        } else if ((pressedPosition - ev->pos()).manhattanLength() < 10) {
            return QGestureRecognizer::GestureFinished;
        }
        return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (!pressedPosition.isNull() && (pressedPosition - ev->pos()).manhattanLength() < 10)
            return QGestureRecognizer::MaybeGesture;
        return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        pressedPosition = ev->pos();
        return QGestureRecognizer::GestureFinished;
    }
    return QGestureRecognizer::NotGesture;
}

QGesture* QDoubleTapGestureRecognizer::getGesture()
{
    QGesturePrivate *d = gesture.d_func();
    d->startPos = pressedPosition;
    d->lastPos = pressedPosition;
    d->pos = pressedPosition;
    d->hotSpot = pressedPosition;
    d->state = Qt::GestureFinished;
    return &gesture;
}

void QDoubleTapGestureRecognizer::reset()
{
    pressedPosition = QPoint();
}

//
// QTapAndHoldGestureRecognizer
//
const int QTapAndHoldGestureRecognizer::iterationCount = 40;
const int QTapAndHoldGestureRecognizer::iterationTimeout = 50;

QTapAndHoldGestureRecognizer::QTapAndHoldGestureRecognizer(QObject *parent)
    : QGestureRecognizer(QString(), parent),
      gesture(0, qt_getStandardGestureTypeName(Qt::TapAndHoldGesture)),
      iteration(0)
{
    Q_D(QGestureRecognizer);
    d->gestureType = Qt::TapAndHoldGesture;
}

QGestureRecognizer::Result QTapAndHoldGestureRecognizer::filterEvent(const QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (timer.isActive())
            timer.stop();
        timer.start(QTapAndHoldGestureRecognizer::iterationTimeout, this);
        pressedPosition = ev->pos();
        return QGestureRecognizer::MaybeGesture;
    } else if (event->type() == QEvent::MouseMove) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if ((pressedPosition - ev->pos()).manhattanLength() < 15)
            return QGestureRecognizer::GestureStarted;
        else
            return QGestureRecognizer::NotGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        timer.stop();
        return QGestureRecognizer::NotGesture;
    }
    return QGestureRecognizer::Ignore;
}

void QTapAndHoldGestureRecognizer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timer.timerId())
        return;
    if (iteration == QTapAndHoldGestureRecognizer::iterationCount) {
        timer.stop();
        emit stateChanged(QGestureRecognizer::GestureFinished);
    } else {
        emit stateChanged(QGestureRecognizer::GestureStarted);
    }
    ++iteration;
}

QGesture* QTapAndHoldGestureRecognizer::getGesture()
{
    QGesturePrivate *d = gesture.d_func();
    d->startPos = pressedPosition;
    d->lastPos = pressedPosition;
    d->pos = pressedPosition;
    d->hotSpot = pressedPosition;
    if (iteration >= QTapAndHoldGestureRecognizer::iterationCount)
        d->state = Qt::GestureFinished;
    else
        d->state = iteration == 0 ? Qt::GestureStarted : Qt::GestureUpdated;
    return &gesture;
}

void QTapAndHoldGestureRecognizer::reset()
{
    pressedPosition = QPoint();
    timer.stop();
    iteration = 0;
}

QT_END_NAMESPACE
