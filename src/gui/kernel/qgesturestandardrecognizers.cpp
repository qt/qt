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

QGestureRecognizerPan::QGestureRecognizerPan()
    : QGestureRecognizer(QString()), mousePressed(false), gestureFinished(false),
      lastDirection(Qt::NoDirection), currentDirection(Qt::NoDirection)
{
    Q_D(QGestureRecognizer);
    d->gestureType = Qt::PanGesture;

    qRegisterMetaType<Qt::DirectionType>();
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
        DEBUG() << "Pan: MouseButtonPress: maybe gesture started";
        mousePressed = true;
        pressedPos = lastPos = currentPos = ev->pos();
        return QGestureRecognizer::MaybeGesture;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        if (mousePressed && currentDirection != Qt::NoDirection) {
            DEBUG() << "Pan: MouseButtonRelease: pan detected";
            gestureFinished = true;
            const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
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
        Qt::DirectionType direction =
            simpleRecognizer.addPosition(ev->pos()).direction;
        DEBUG() << "Pan: MouseMove: simplerecognizer result = " << direction;
        QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
        if (currentDirection == Qt::NoDirection) {
            if (direction == Qt::NoDirection)
                result = QGestureRecognizer::MaybeGesture;
            else
                result = QGestureRecognizer::GestureStarted;
        } else {
            result = QGestureRecognizer::GestureStarted;
        }
        if (direction != Qt::NoDirection) {
            if (currentDirection != direction)
                lastDirection = currentDirection;
            currentDirection = direction;
        }
        return result;
    }
    return QGestureRecognizer::NotGesture;
}

QGesture* QGestureRecognizerPan::getGesture()
{
    if (currentDirection == Qt::NoDirection)
        return 0;
    QGesture *g = new QGesture(this, qt_getStandardGestureTypeName(Qt::PanGesture),
                               pressedPos, lastPos, currentPos,
                               QRect(), pressedPos, QDateTime(), 0,
                               gestureFinished ? Qt::GestureFinished : Qt::GestureStarted);
    g->setProperty("lastDirection", QVariant::fromValue(lastDirection));
    g->setProperty("direction", QVariant::fromValue(currentDirection));
    
    return g;
}

void QGestureRecognizerPan::reset()
{
    mousePressed = false;
    lastDirection = Qt::NoDirection;
    currentDirection = Qt::NoDirection;
    gestureFinished = false;
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
QDoubleTapGestureRecognizer::QDoubleTapGestureRecognizer()
    : QGestureRecognizer(QString())
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
    } else if (event->type() == QEvent::MouseButtonRelease) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        if (!pressedPosition.isNull() && (pressedPosition - ev->pos()).manhattanLength() < 10)
            return QGestureRecognizer::MaybeGesture;
    } else if (event->type() == QEvent::MouseButtonDblClick) {
        const QMouseEvent *ev = static_cast<const QMouseEvent*>(event);
        pressedPosition = ev->pos();
        return QGestureRecognizer::GestureFinished;
    }
    return QGestureRecognizer::NotGesture;
}

QGesture* QDoubleTapGestureRecognizer::getGesture()
{
    return new QGesture(this, qt_getStandardGestureTypeName(Qt::DoubleTapGesture),
                        pressedPosition, pressedPosition, pressedPosition,
                        QRect(), pressedPosition, QDateTime(), 0, Qt::GestureFinished);
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

QTapAndHoldGestureRecognizer::QTapAndHoldGestureRecognizer()
    : QGestureRecognizer(QString()), iteration(0)
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
    return QGestureRecognizer::NotGesture;
}

void QTapAndHoldGestureRecognizer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timer.timerId())
        return;
    if (++iteration == QTapAndHoldGestureRecognizer::iterationCount) {
        emit stateChanged(QGestureRecognizer::GestureFinished);
        timer.stop();
    } else {
        emit stateChanged(QGestureRecognizer::GestureStarted);
    }
}

QGesture* QTapAndHoldGestureRecognizer::getGesture()
{
    return new QGesture(this, qt_getStandardGestureTypeName(Qt::TapAndHoldGesture),
                        pressedPosition, pressedPosition, pressedPosition,
                        QRect(), pressedPosition, QDateTime(), 0,
                        iteration >= QTapAndHoldGestureRecognizer::iterationCount ?
                        Qt::GestureFinished :  Qt::GestureStarted);
}

void QTapAndHoldGestureRecognizer::reset()
{
    pressedPosition = QPoint();
    timer.stop();
    iteration = 0;
}

QT_END_NAMESPACE
