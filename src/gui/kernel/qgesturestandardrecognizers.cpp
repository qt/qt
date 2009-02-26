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

// #define GESTURE_RECOGNIZER_DEBUG
#ifndef GESTURE_RECOGNIZER_DEBUG
# define DEBUG if (0) qDebug
#else
# define DEBUG qDebug
#endif

QT_BEGIN_NAMESPACE
/*
QGestureRecognizerMouseTwoButtons::QGestureRecognizerMouseTwoButtons()
    : QGestureRecognizer(Qt::MouseTwoButtonClick)
{
    clear();
}

QGestureRecognizer::Result QGestureRecognizerMouseTwoButtons::recognize(const QList<QEvent*> &inputEvents)
{
    // get all mouse events
    QList<QMouseEvent*> events;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *event = inputEvents.at(i);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            events.push_back(static_cast<QMouseEvent*>(event));
        default:
            break;
        }
    }

    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < events.count(); ++i) {
        QMouseEvent *event = events.at(i);
        if (event->type() == QEvent::MouseButtonPress) {
            if (userEvents[3]) {
                // something wrong, we already has a gesture.
                clear();
            } else if (userEvents[2]) {
                // 1d, 2d, 1u, and user press another button. not gesture.
                DEBUG() << "1";
                result = QGestureRecognizer::NotGesture;
                clear();
                break;
            } else if (userEvents[1]) {
                // 1d, 2d, and user pressed third button. not gesture.
                DEBUG() << "2";
                result = QGestureRecognizer::NotGesture;
                clear();
                break;
            } else if (userEvents[0]) {
                // second button press.
                DEBUG() << "3";
                userEvents[1] = event;
                result = QGestureRecognizer::MaybeGesture;
            } else {
                // first button press.
                DEBUG() << "4";
                userEvents[0] = event;
                result = QGestureRecognizer::MaybeGesture;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            if (userEvents[3]) {
                // something wrong, we already has a gesture.
                clear();
            } else if (userEvents[2]) {
                // 1d, 2d, 1u, and button release
                DEBUG() << "5";
                if (userEvents[1]->button() != event->button()) {
                    // got weird buttonreleased event. doesn't look like gesture.
                    result = QGestureRecognizer::NotGesture;
                    clear();
                    break;
                }
                // gesture!
                userEvents[3] = event;
                result = QGestureRecognizer::GestureFinished;
                break;
            } else if (userEvents[1]) {
                // 1d, 2d, and button release
                DEBUG() << "6";
                if (userEvents[0]->button() != event->button()) {
                    // user released the wrong button. not gesture.
                    result = QGestureRecognizer::NotGesture;
                    clear();
                    break;
                }
                // user released the right button! looks like gesture
                userEvents[2] = event;
                result = QGestureRecognizer::MaybeGesture;
            } else if (userEvents[0]) {
                // 1d, and some button was released. not a gesture.
                DEBUG() << "7";
                result = QGestureRecognizer::NotGesture;
                clear();
                break;
            }
        }
    }
    return result;
}

QGesture* QGestureRecognizerMouseTwoButtons::makeEvent() const
{
    if (!userEvents[0] || !userEvents[1] || !userEvents[2] || !userEvents[3])
        return 0;
    QGesture::Direction direction =
        (userEvents[0]->button() < userEvents[1]->button() ?
         QGesture::Left : QGesture::Right);
    QList<QPoint> points;
    points.push_back(userEvents[0]->globalPos());
    points.push_back(userEvents[1]->globalPos());
    QGesture *gesture =
        new QGesture(Qt::MouseTwoButtonClick, points.at(0), points.at(1), points.at(1),
                     direction,
                     QRect(points.at(0), points.at(1)),
                     points.at(0));
    return gesture;
}

void QGestureRecognizerMouseTwoButtons::reset()
{
    clear();
}

void QGestureRecognizerMouseTwoButtons::clear()
{
    userEvents[0] = userEvents[1] = userEvents[2] = userEvents[3] = 0;
}
*/

//
// QGestureRecognizerPan
//

QGestureRecognizerPan::QGestureRecognizerPan()
    : QGestureRecognizer(Qt::Pan), mousePressed(false), gestureFinished(false),
      lastDirection(Direction::None)
{
}

QGestureRecognizer::Result QGestureRecognizerPan::recognize(const QList<QEvent*> &inputEvents)
{
    // get all mouse events
    QList<QMouseEvent*> events;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *event = inputEvents.at(i);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            events.push_back(static_cast<QMouseEvent*>(event));
        default:
            break;
        }
    }

    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < events.count(); ++i) {
        QMouseEvent *event = events.at(i);
        if (event->type() == QEvent::MouseButtonPress) {
            if (lastDirection != Direction::None) {
                DEBUG() << "Pan: MouseButtonPress: fail. another press during pan";
                result = QGestureRecognizer::NotGesture;
                reset();
                break;
            }
            DEBUG() << "Pan: MouseButtonPress: maybe gesture started";
            result = QGestureRecognizer::MaybeGesture;
            mousePressed = true;
            pressedPos = lastPos = currentPos = event->pos();
        } else if (event->type() == QEvent::MouseButtonRelease) {
            if (mousePressed && lastDirection != Direction::None) {
                DEBUG() << "Pan: MouseButtonRelease: pan detected";
                result = QGestureRecognizer::GestureFinished;
                gestureFinished = true;
                currentPos = event->pos();
                internalReset();
                break;
            }
            DEBUG() << "Pan: MouseButtonRelease: some weird release detected, ignoring";
            result = QGestureRecognizer::NotGesture;
            reset();
            break;
        } else if (event->type() == QEvent::MouseMove) {
            if (!mousePressed)
                continue;
            lastPos = currentPos;
            currentPos = event->pos();
            Direction::DirectionType direction =
                simpleRecognizer.addPosition(event->pos()).direction;
            DEBUG() << "Pan: MouseMove: simplerecognizer result = " << direction;
            if (lastDirection == Direction::None) {
                if (direction == Direction::None)
                    result = QGestureRecognizer::MaybeGesture;
                else
                    result = QGestureRecognizer::GestureStarted;
            } else {
                result = QGestureRecognizer::GestureStarted;
            }
            if (direction != Direction::None)
                lastDirection = direction;
        }
    }
    return result;
}

QGesture* QGestureRecognizerPan::makeEvent() const
{
    QPannableGesture::DirectionType dir = QPannableGesture::None;
    if (lastDirection == Direction::Left)
        dir = QPannableGesture::Left;
    else if (lastDirection == Direction::Right)
        dir = QPannableGesture::Right;
    else if (lastDirection == Direction::Up)
        dir = QPannableGesture::Up;
    else if (lastDirection == Direction::Down)
        dir = QPannableGesture::Down;
    else
        return 0;
    QPannableGesture *g =
        new QPannableGesture(Qt::Pan, pressedPos, lastPos, currentPos,
                             QRect(), pressedPos, QDateTime(), 0,
                             gestureFinished ? Qt::GestureFinished : Qt::GestureStarted);
    QPannableGesturePrivate *d = (QPannableGesturePrivate*)g->d;
    d->lastDirection = dir; ///###
    d->direction = dir;
    
    return g;
}

void QGestureRecognizerPan::reset()
{
    mousePressed = false;
    lastDirection = Direction::None;
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
    : QGestureRecognizer(Qt::DoubleTap)
{
}

QGestureRecognizer::Result QDoubleTapGestureRecognizer::recognize(const QList<QEvent*> &inputEvents)
{
    // get all mouse events
    QList<QMouseEvent*> events;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *event = inputEvents.at(i);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
            events.push_back(static_cast<QMouseEvent*>(event));
        default:
            break;
        }
    }

    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < events.count(); ++i) {
        QMouseEvent *event = events.at(i);
        if (event->type() == QEvent::MouseButtonPress) {
            if (pressedPosition.isNull()) {
                result = QGestureRecognizer::MaybeGesture;
                pressedPosition = event->pos();
            } else if ((pressedPosition - event->pos()).manhattanLength() < 10) {
                result = QGestureRecognizer::GestureFinished;
            }
        } else if (event->type() == QEvent::MouseButtonRelease) {
            if (!pressedPosition.isNull() && (pressedPosition - event->pos()).manhattanLength() < 10)
                result = QGestureRecognizer::MaybeGesture;
        } else if (event->type() == QEvent::MouseButtonDblClick) {
            result = QGestureRecognizer::GestureFinished;
            pressedPosition = event->pos();
            break;
        }
    }
    return result;
}

QGesture* QDoubleTapGestureRecognizer::makeEvent() const
{
    return new QGesture(Qt::DoubleTap,
                        pressedPosition, pressedPosition, pressedPosition,
                        QRect(), pressedPosition, QDateTime(), 0, Qt::GestureFinished);
}

void QDoubleTapGestureRecognizer::reset()
{
    pressedPosition = QPoint();
}

//
// QLongTapGestureRecognizer
//
const int QLongTapGestureRecognizer::iterationCount = 40;
const int QLongTapGestureRecognizer::iterationTimeout = 50;

QLongTapGestureRecognizer::QLongTapGestureRecognizer()
    : QGestureRecognizer(Qt::LongTap), iteration(0)
{
}

QGestureRecognizer::Result QLongTapGestureRecognizer::recognize(const QList<QEvent*> &inputEvents)
{
    // get all mouse events
    QList<QMouseEvent*> events;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *event = inputEvents.at(i);
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
            events.push_back(static_cast<QMouseEvent*>(event));
        default:
            break;
        }
    }

    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < events.count(); ++i) {
        QMouseEvent *event = events.at(i);
        if (event->type() == QEvent::MouseButtonPress) {
            if (timer.isActive())
                timer.stop();
            timer.start(QLongTapGestureRecognizer::iterationTimeout, this);
            pressedPosition = event->pos();
            result = QGestureRecognizer::MaybeGesture;
        } else if (event->type() == QEvent::MouseMove) {
            if ((pressedPosition - event->pos()).manhattanLength() < 15)
                result = QGestureRecognizer::GestureStarted;
            else
                result = QGestureRecognizer::NotGesture;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            result = QGestureRecognizer::NotGesture;
            timer.stop();
            break;
        }
    }
    return result;
}

void QLongTapGestureRecognizer::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timer.timerId())
        return;
    if (++iteration == QLongTapGestureRecognizer::iterationCount) {
        emit triggered(QGestureRecognizer::GestureFinished);
        timer.stop();
    } else {
        emit triggered(QGestureRecognizer::GestureStarted);
    }
}

QGesture* QLongTapGestureRecognizer::makeEvent() const
{
    return new QGesture(Qt::LongTap,
                        pressedPosition, pressedPosition, pressedPosition,
                        QRect(), pressedPosition, QDateTime(), 0,
                        iteration >= QLongTapGestureRecognizer::iterationCount ?
                        Qt::GestureFinished :  Qt::GestureStarted);
}

void QLongTapGestureRecognizer::reset()
{
    pressedPosition = QPoint();
    timer.stop();
    iteration = 0;
}

//
// QMultiTouchGestureRecognizer
//

/*
QMultiTouchGestureRecognizer::QMultiTouchGestureRecognizer()
    : QGestureRecognizer(Qt::Pinch)
{
}

QGestureRecognizer::Result QMultiTouchGestureRecognizer::recognize(const QList<QEvent*> &inputEvents)
{
    QGestureRecognizer::Result result = QGestureRecognizer::NotGesture;
    for(int i = 0; i < inputEvents.count(); ++i) {
        QEvent *inputEvent = inputEvents.at(i);
        if (inputEvent->type() != QEvent::Pointer) {
            if (touches.size() == 2)
                result = QGestureRecognizer::GestureStarted;
            else if (touches.size() == 1)
                result = QGestureRecognizer::MaybeGesture;
            continue;
        }
        QPointerEvent *event = static_cast<QPointerEvent*>(inputEvent);
        Q_ASSERT(event->pointerCount() > 0);
        for (int idx = 0; idx < event->pointerCount(); ++idx) {
            switch (event->state(idx)) {
            case QPointerEvent::Begin:
                qDebug() << "Begin" << idx;
                if (touches.size() == 2) {
                    // too many touches
                    qDebug() << "too many touches";
                    result = QGestureRecognizer::MaybeGesture;
                    continue;
                }
                touches[event->pointerId(idx)].startPosition = event->pos(idx);
                if (touches.size() == 1) {
                    result = QGestureRecognizer::MaybeGesture;
                } else if (touches.size() == 2) {
                    result = QGestureRecognizer::GestureStarted;
                } else {
                    result = QGestureRecognizer::NotGesture;
                }
                break;

            case QPointerEvent::Move:
                qDebug() << "Move" << idx;
                touches[event->pointerId(idx)].lastPosition = touches[event->pointerId(idx)].currentPosition;
                touches[event->pointerId(idx)].currentPosition = event->pos(idx);
                if (touches.size() == 1)
                    result = QGestureRecognizer::MaybeGesture;
                else if (touches.size() == 2)
                    result = QGestureRecognizer::GestureStarted;
                else
                    result = QGestureRecognizer::NotGesture;
                break;

            case QPointerEvent::End:
                qDebug() << "End" << idx;
                touches.remove(event->pointerId(idx));
                if (touches.size() == 1) {
                    result = QGestureRecognizer::MaybeGesture;
                } else if (touches.size() == 0) {
                    result = QGestureRecognizer::NotGesture;
                } else {
                    qDebug() << "unexpected touch end event";
                    return QGestureRecognizer::NotGesture;
                }
                break;

            case QPointerEvent::Stationary:
                qDebug() << "Stationary" << idx;
                if (touches.size() == 2)
                    result = QGestureRecognizer::GestureStarted;
                else if (touches.size() == 1)
                    result = QGestureRecognizer::MaybeGesture;
                break;

            default:
                break;
            }
        }
    }
    return result;
}

QGesture* QMultiTouchGestureRecognizer::makeEvent() const
{
    if (touches.size() != 2)
        return 0;

    QGesture *g = new QGesture(Qt::Pinch);
    QGesture::Data data;

    TapMap::const_iterator it = touches.begin();
    data.startPos = it->startPosition.toPoint();
    data.lastPos = it->lastPosition.toPoint();
    data.currentPos = it->currentPosition.toPoint();
    g->datas.push_back(data);

    ++it;
    data.startPos = it->startPosition.toPoint();
    data.lastPos = it->lastPosition.toPoint();
    data.currentPos = it->currentPosition.toPoint();
    g->datas.push_back(data);

    return g;
}

void QMultiTouchGestureRecognizer::reset()
{
    touches.clear();
}
*/

QT_END_NAMESPACE
