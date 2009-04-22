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

#include "qgesturemanager_p.h"
#include "qgesture.h"
#include "qevent.h"

#include "qapplication.h"
#include "qapplication_p.h"
#include "qwidget.h"
#include "qwidget_p.h"

#include "qgesturestandardrecognizers_p.h"

#include "qdebug.h"

// #define GESTURE_DEBUG
#ifndef GESTURE_DEBUG
# define DEBUG if (0) qDebug
#else
# define DEBUG qDebug
#endif

QT_BEGIN_NAMESPACE

bool qt_sendSpontaneousEvent(QObject *receiver, QEvent *event);

static const unsigned int MaximumGestureRecognitionTimeout = 2000;

QGestureManager::QGestureManager(QObject *parent)
    : QObject(parent), targetWidget(0), eventDeliveryDelayTimeout(300),
      delayedPressTimer(0), lastMousePressEvent(QEvent::None, QPoint(), Qt::NoButton, 0, 0),
      state(NotGesture)
{
    qRegisterMetaType<Qt::DirectionType>();
    qRegisterMetaType<Qt::GestureState>();

    recognizers << new QDoubleTapGestureRecognizer(this);
    recognizers << new QTapAndHoldGestureRecognizer(this);
    recognizers << new QGestureRecognizerPan(this);

    foreach(QGestureRecognizer *r, recognizers)
        connect(r, SIGNAL(stateChanged(QGestureRecognizer::Result)),
                this, SLOT(recognizerStateChanged(QGestureRecognizer::Result)));
}

void QGestureManager::addRecognizer(QGestureRecognizer *recognizer)
{
    recognizer->setParent(this);
    recognizers << recognizer;
}

void QGestureManager::removeRecognizer(QGestureRecognizer *recognizer)
{
    recognizers.remove(recognizer);
}

bool QGestureManager::filterEvent(QEvent *event)
{
    QPoint currentPos;
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        currentPos = static_cast<QMouseEvent*>(event)->pos(); break;
    default: break;
    }

    const QMap<QString, int> &grabbedGestures = qApp->d_func()->grabbedGestures;

    bool ret = false;
    QSet<QGestureRecognizer*> startedGestures;
    QSet<QGestureRecognizer*> finishedGestures;
    QSet<QGestureRecognizer*> newMaybeGestures;
    QSet<QGestureRecognizer*> cancelledGestures;
    QSet<QGestureRecognizer*> notGestures;
    if (state == NotGesture || state == MaybeGesture) {
        DEBUG() << "QGestureManager: current event processing state: "
                << (state == NotGesture ? "NotGesture" : "MaybeGesture");

        Q_ASSERT(targetWidget != 0);
        QSet<QGestureRecognizer*> stillMaybeGestures;
        // try other recognizers.
        foreach(QGestureRecognizer *r, recognizers) {
            if (grabbedGestures.value(r->gestureType(), 0) <= 0)
                continue;
            QGestureRecognizer::Result result = r->filterEvent(event);
            if (result == QGestureRecognizer::GestureStarted) {
                DEBUG() << "QGestureManager: gesture started: " << r;
                startedGestures << r;
            } else if (result == QGestureRecognizer::GestureFinished) {
                DEBUG() << "QGestureManager: gesture finished: " << r;
                finishedGestures << r;
            } else if (result == QGestureRecognizer::MaybeGesture) {
                DEBUG() << "QGestureManager: maybe gesture: " << r;
                newMaybeGestures << r;
            } else {
                // if it was maybe gesture, but isn't a gesture anymore.
                DEBUG() << "QGestureManager: not gesture: " << r;
                notGestures << r;
            }
        }
        activeGestures -= newMaybeGestures;
        activeGestures += startedGestures;
        foreach(QGestureRecognizer *r, startedGestures+finishedGestures+notGestures) {
            QMap<QGestureRecognizer*, int>::iterator it = maybeGestures.find(r);
            if (it != maybeGestures.end()) {
                killTimer(it.value());
                maybeGestures.erase(it);
            }
        }
        foreach(QGestureRecognizer *r, newMaybeGestures) {
            if (!maybeGestures.contains(r)) {
                int timerId = startTimer(MaximumGestureRecognitionTimeout);
                if (!timerId)
                    qWarning("QGestureManager: couldn't start timer!");
                maybeGestures.insert(r, timerId);
            }
        }
        if (!finishedGestures.isEmpty() || !activeGestures.isEmpty()) {
            // gesture found!
            ret = true;
            DEBUG() << "QGestureManager: sending gesture event for: "
                    << activeGestures << " and " << finishedGestures;

            QList<QGesture*> gestures;
            foreach(QGestureRecognizer *r, finishedGestures) {
                if (QGesture *gesture = r->getGesture())
                    gestures << gesture;
            }
            foreach(QGestureRecognizer *r, activeGestures) {
                if (QGesture *gesture = r->getGesture())
                    gestures << gesture;
            }
            Q_ASSERT(!gestures.isEmpty());
            QGestureEvent event(gestures);
            ret = sendGestureEvent(targetWidget, &event);
            ret = ret && event.isAccepted();

            if (!activeGestures.isEmpty()) {
                DEBUG() << "QGestureManager: new state = Gesture";
                state = Gesture;
            } else if (!maybeGestures.isEmpty()) {
                DEBUG() << "QGestureManager: new state = Maybe";
                state = MaybeGesture;
            } else {
                DEBUG() << "QGestureManager: new state = NotGesture";
                state = NotGesture;
            }
        } else if (!maybeGestures.isEmpty()) {
            if (state != MaybeGesture) {
                // We got a new set of events that look like a start
                // of some gesture, so we switch to state MaybeGesture
                // and wait for more events.
                DEBUG() << "QGestureManager: new state = Maybe. Waiting for events";
                state = MaybeGesture;
                // start gesture timer
            } else {
                // we still not sure if it is a gesture or not.
            }
        } else if (state == MaybeGesture) {
            // last time we thought it looks like gesture, but now we
            // know for sure that it isn't.
            DEBUG() << "QGestureManager: new state = NotGesture";
            state = NotGesture;
        }
        foreach(QGestureRecognizer *r, finishedGestures)
            r->reset();
        foreach(QGestureRecognizer *r, cancelledGestures)
            r->reset();
        foreach(QGestureRecognizer *r, notGestures)
            r->reset();
    } else if (state == Gesture) {
        DEBUG() << "QGestureManager: current event processing state: Gesture";
        Q_ASSERT(!activeGestures.isEmpty());

        foreach(QGestureRecognizer *r, recognizers) {
            if (grabbedGestures.value(r->gestureType(), 0) <= 0)
                continue;
            QGestureRecognizer::Result result = r->filterEvent(event);
            if (result == QGestureRecognizer::GestureStarted) {
                DEBUG() << "QGestureManager: gesture started: " << r;
                startedGestures << r;
            } else if (result == QGestureRecognizer::GestureFinished) {
                DEBUG() << "QGestureManager: gesture finished: " << r;
                finishedGestures << r;
            } else if (result == QGestureRecognizer::MaybeGesture) {
                DEBUG() << "QGestureManager: maybe gesture: " << r;
                newMaybeGestures << r;
            } else {
                // if it was an active gesture, but isn't a gesture anymore.
                if (activeGestures.contains(r)) {
                    DEBUG() << "QGestureManager: cancelled gesture: " << r;
                    cancelledGestures << r;
                } else {
                    DEBUG() << "QGestureManager: not gesture: " << r;
                    notGestures << r;
                }
            }
        }

        activeGestures -= newMaybeGestures;
        activeGestures -= cancelledGestures;
        activeGestures -= finishedGestures;
        activeGestures += startedGestures;
        foreach(QGestureRecognizer *r, startedGestures+finishedGestures+notGestures) {
            QMap<QGestureRecognizer*, int>::iterator it = maybeGestures.find(r);
            if (it != maybeGestures.end()) {
                killTimer(it.value());
                maybeGestures.erase(it);
            }
        }
        foreach(QGestureRecognizer *r, newMaybeGestures) {
            if (!maybeGestures.contains(r)) {
                int timerId = startTimer(MaximumGestureRecognitionTimeout);
                if (!timerId)
                    qWarning("QGestureManager: couldn't start timer!");
                maybeGestures.insert(r, timerId);
            }
        }
        QList<QGesture*> gestures;
        if (!finishedGestures.isEmpty() || !activeGestures.isEmpty()) {
            // another gesture found!
            ret = true;
            DEBUG() << "QGestureManager: sending gesture event for: "
                    << activeGestures << " and " << finishedGestures;

            foreach(QGestureRecognizer *r, finishedGestures) {
                if (QGesture *gesture = r->getGesture())
                    gestures << gesture;
            }
            foreach(QGestureRecognizer *r, activeGestures) {
                if (QGesture *gesture = r->getGesture())
                    gestures << gesture;
            }
        }
        QSet<QString> cancelledGestureNames;
        foreach(QGestureRecognizer *r, cancelledGestures)
            cancelledGestureNames << r->gestureType();
        if(!gestures.isEmpty()) {
            QGestureEvent event(gestures, cancelledGestureNames);
            ret = sendGestureEvent(targetWidget, &event);
            ret = ret && event.isAccepted();
        }

        foreach(QGestureRecognizer *r, finishedGestures)
            r->reset();
        foreach(QGestureRecognizer *r, cancelledGestures)
            r->reset();
        foreach(QGestureRecognizer *r, notGestures)
            r->reset();
        if (!activeGestures.isEmpty()) {
            // nothing changed, we are still handling a gesture
        } else if (!maybeGestures.isEmpty()) {
            DEBUG() << "QGestureManager: new state = Maybe. Waiting for events: " << maybeGestures;
            state = MaybeGesture;
        } else {
            DEBUG() << "QGestureManager: new state = NotGesture";
            state = NotGesture;
        }
    }

    if (delayedPressTimer && state == Gesture) {
        DEBUG() << "QGestureManager: gesture started. Forgetting about postponed mouse press event";
        killTimer(delayedPressTimer);
        delayedPressTimer = 0;
    } else if (delayedPressTimer && (state == NotGesture ||
                                     event->type() == QEvent::MouseButtonRelease)) {
        // not a gesture or released button too fast, so replay press
        // event back.
        DEBUG() << "QGestureManager: replaying mouse press event";
        QMap<QGestureRecognizer*, int>::const_iterator it = maybeGestures.begin(),
                                                        e = maybeGestures.end();;
        for (; it != e; ++it) {
            it.key()->reset();
            killTimer(it.value());
        }
        maybeGestures.clear();
        state = NotGesture;

        Q_ASSERT(targetWidget != 0);
        QApplication::sendEvent(targetWidget, &lastMousePressEvent);
        if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            QMouseEvent move(QEvent::MouseMove, me->pos(), me->globalPos(), me->button(),
                             me->buttons(), me->modifiers());
            QApplication::sendEvent(targetWidget, &move);
            ret = false;
        }
        killTimer(delayedPressTimer);
        delayedPressTimer = 0;
    } else if (state == MaybeGesture && event->type() == QEvent::MouseButtonPress
               && eventDeliveryDelayTimeout) {
        // postpone the press event delivery until we know for
        // sure whether it is a gesture.
        DEBUG() << "QGestureManager: postponing mouse press event";
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        lastMousePressEvent = QMouseEvent(QEvent::MouseButtonPress, me->pos(),
                                          me->globalPos(), me->button(),
                                          me->buttons(), me->modifiers());
        Q_ASSERT(delayedPressTimer == 0);
        delayedPressTimer = startTimer(eventDeliveryDelayTimeout);
        if (!delayedPressTimer)
            qWarning("QGestureManager: couldn't start delayed press timer!");
    }
    // if we have postponed a mouse press event, postpone all
    // following event
    if (delayedPressTimer)
        ret = true;

    lastPos = currentPos;
    return ret;
}

void QGestureManager::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == delayedPressTimer) {
        DEBUG() << "QGestureManager: replaying mouse press event due to timeout";
        // sanity checks
        Q_ASSERT(state != Gesture);

        QMap<QGestureRecognizer*, int>::const_iterator it = maybeGestures.begin(),
                                                        e = maybeGestures.end();;
        for (; it != e; ++it) {
            it.key()->reset();
            killTimer(it.value());
        }
        maybeGestures.clear();
        state = NotGesture;

        // we neither received a mouse release event nor gesture
        // started, so we replay stored mouse press event.
        QApplication::sendEvent(targetWidget, &lastMousePressEvent);

        killTimer(delayedPressTimer);
        delayedPressTimer = 0;
    } else {
        // sanity checks, remove later
        Q_ASSERT((state == Gesture && !activeGestures.isEmpty()) || (state != Gesture && activeGestures.isEmpty()));

        typedef QMap<QGestureRecognizer*, int> MaybeGestureMap;
        for (MaybeGestureMap::iterator it = maybeGestures.begin(), e = maybeGestures.end();
             it != e; ++it) {
            if (it.value() == event->timerId()) {
                DEBUG() << "QGestureManager: gesture timeout.";
                QGestureRecognizer *r = it.key();
                r->reset();
                maybeGestures.erase(it);
                killTimer(event->timerId());
                break;
            }
        }

        if (state == MaybeGesture && maybeGestures.isEmpty()) {
            DEBUG() << "QGestureManager: new state = NotGesture because of timeout";
            state = NotGesture;
        }
    }
}

bool QGestureManager::inGestureMode()
{
    return state == Gesture;
}

void QGestureManager::setGestureTargetWidget(QWidget *widget)
{
    targetWidget = widget;
}

void QGestureManager::recognizerStateChanged(QGestureRecognizer::Result result)
{
    QGestureRecognizer *recognizer = qobject_cast<QGestureRecognizer*>(sender());
    if (!recognizer)
        return;
    if (qApp->d_func()->grabbedGestures.value(recognizer->gestureType(), 0) <= 0) {
        recognizer->reset();
        return;
    }

    switch (result) {
    case QGestureRecognizer::GestureStarted:
    case QGestureRecognizer::GestureFinished: {
        if (result == QGestureRecognizer::GestureStarted) {
            DEBUG() << "QGestureManager: gesture started: " << recognizer;
            activeGestures << recognizer;
            DEBUG() << "QGestureManager: new state = Gesture";
            state = Gesture;
        } else {
            DEBUG() << "QGestureManager: gesture finished: " << recognizer;
        }
        if (maybeGestures.contains(recognizer)) {
            killTimer(maybeGestures.value(recognizer));
            maybeGestures.remove(recognizer);
        }
        QList<QGesture*> gestures;
        if (QGesture *gesture = recognizer->getGesture())
            gestures << gesture;
        if(!gestures.isEmpty()) {
            QGestureEvent event(gestures);
            sendGestureEvent(targetWidget, &event);
        }
        if (result == QGestureRecognizer::GestureFinished)
            recognizer->reset();
    }
        break;
    case QGestureRecognizer::MaybeGesture: {
        DEBUG() << "QGestureManager: maybe gesture: " << recognizer;
        if (activeGestures.contains(recognizer)) {
            QGestureEvent event(QList<QGesture*>(),
                                QSet<QString>() << recognizer->gestureType());
            sendGestureEvent(targetWidget, &event);
        }
        if (!maybeGestures.contains(recognizer)) {
            int timerId = startTimer(MaximumGestureRecognitionTimeout);
            if (!timerId)
                qWarning("QGestureManager: couldn't start timer!");
            maybeGestures.insert(recognizer, timerId);
        }
    }
        break;
    case QGestureRecognizer::NotGesture:
        DEBUG() << "QGestureManager: not gesture: " << recognizer;
        if (maybeGestures.contains(recognizer)) {
            killTimer(maybeGestures.value(recognizer));
            maybeGestures.remove(recognizer);
        }
        recognizer->reset();
        break;
    default:
        Q_ASSERT(false);
    }

    if (delayedPressTimer && state == Gesture) {
        killTimer(delayedPressTimer);
        delayedPressTimer = 0;
    }
}

bool QGestureManager::sendGestureEvent(QWidget *receiver, QGestureEvent *event)
{
    QSet<QString> eventGestures;
    foreach(const QString &gesture, event->gestureTypes())
        eventGestures << gesture;

    QPoint offset;
    bool found = false;
    while (receiver) {
        QSet<int> widgetGestures = receiver->d_func()->gestures;
        foreach(int gestureId, widgetGestures) {
            if (eventGestures.contains(gestureNameFromId(gestureId))) {
                found = true;
                break;
            }
        }
        if (found)
            break;
        offset += receiver->pos();
        receiver = receiver->parentWidget();
    }
    foreach(QGesture *gesture, event->gestures())
        gesture->translate(offset);
    return receiver ? qt_sendSpontaneousEvent(receiver, event) : false;
}

int QGestureManager::eventDeliveryDelay() const
{
    return eventDeliveryDelayTimeout;
}

void QGestureManager::setEventDeliveryDelay(int ms)
{
    eventDeliveryDelayTimeout = ms;
}

int QGestureManager::makeGestureId(const QString &name)
{
    gestureIdMap[++lastGestureId] = name;
    return lastGestureId;
}

void QGestureManager::releaseGestureId(int gestureId)
{
    gestureIdMap.remove(gestureId);
}

QString QGestureManager::gestureNameFromId(int gestureId) const
{
    return gestureIdMap.value(gestureId);
}

QT_END_NAMESPACE

#include "moc_qgesturemanager_p.cpp"

