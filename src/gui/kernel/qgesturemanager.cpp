/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgesturemanager_p.h"
#include "qgesture.h"
#include "qgesture_p.h"
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
    : QObject(parent), eventDeliveryDelayTimeout(300),
      delayedPressTimer(0), lastMousePressReceiver(0), lastMousePressEvent(QEvent::None, QPoint(), Qt::NoButton, 0, 0),
      lastGestureId(0), state(NotGesture)
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

bool QGestureManager::filterEvent(QWidget *receiver, QEvent *event)
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
            } else if (result == QGestureRecognizer::NotGesture) {
                // if it was maybe gesture, but isn't a gesture anymore.
                DEBUG() << "QGestureManager: not gesture: " << r;
                notGestures << r;
            }
        }
        Q_ASSERT(activeGestures.isEmpty());
        activeGestures += startedGestures;
        for(QMap<QGestureRecognizer*, int>::iterator it = maybeGestures.begin();
            it != maybeGestures.end();) {
            QGestureRecognizer *r = it.key();
            if (startedGestures.contains(r) || finishedGestures.contains(r) ||
                notGestures.contains(r)) {
                killTimer(it.value());
                it = maybeGestures.erase(it);
            } else {
                ++it;
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
        if (!finishedGestures.isEmpty() || !startedGestures.isEmpty()) {
            // gesture found!
            ret = true;
            QSet<QGesture*> started;
            foreach(QGestureRecognizer *r, finishedGestures) {
                if (QGesture *gesture = r->getGesture()) {
                    started << gesture;
                    gesture->d_func()->singleshot = true;
                }
            }
            foreach(QGestureRecognizer *r, startedGestures) {
                if (QGesture *gesture = r->getGesture()) {
                    started << gesture;
                    gesture->d_func()->singleshot = false;
                }
            }

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

            Q_ASSERT(!started.isEmpty());
            ret = sendGestureEvent(receiver, started, QSet<QGesture*>());
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
            } else if (result == QGestureRecognizer::NotGesture) {
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

        for(QMap<QGestureRecognizer*, int>::iterator it = maybeGestures.begin();
            it != maybeGestures.end();) {
            QGestureRecognizer *r = it.key();
            if (startedGestures.contains(r) || finishedGestures.contains(r) ||
                notGestures.contains(r)) {
                killTimer(it.value());
                it = maybeGestures.erase(it);
            } else {
                ++it;
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
        QSet<QGesture*> started, updated;
        if (!finishedGestures.isEmpty() || !startedGestures.isEmpty()) {
            // another gesture found!
            ret = true;
            foreach(QGestureRecognizer *r, finishedGestures) {
                if (QGesture *gesture = r->getGesture()) {
                    gesture->d_func()->singleshot = !activeGestures.contains(r);
                    if (gesture->d_func()->singleshot)
                        started << gesture;
                    else
                        updated << gesture;
                }
            }
            foreach(QGestureRecognizer *r, startedGestures) {
                if (QGesture *gesture = r->getGesture()) {
                    gesture->d_func()->singleshot = !activeGestures.contains(r);
                    if (gesture->d_func()->singleshot)
                        started << gesture;
                    else
                        updated << gesture;
                }
            }
        }
        activeGestures -= newMaybeGestures;
        activeGestures -= cancelledGestures;
        activeGestures += startedGestures;
        activeGestures -= finishedGestures;
        QSet<QString> cancelledGestureNames;
        foreach(QGestureRecognizer *r, cancelledGestures)
            cancelledGestureNames << r->gestureType();
        ret = sendGestureEvent(receiver, started, updated, cancelledGestureNames);

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
        lastMousePressReceiver = 0;
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

        if (lastMousePressReceiver) {
            QApplication::sendEvent(lastMousePressReceiver, &lastMousePressEvent);
            if (event->type() == QEvent::MouseButtonRelease) {
                QMouseEvent *me = static_cast<QMouseEvent*>(event);
                QMouseEvent move(QEvent::MouseMove, me->pos(), me->globalPos(), me->button(),
                                 me->buttons(), me->modifiers());
                QApplication::sendEvent(lastMousePressReceiver, &move);
                ret = false;
            }
            lastMousePressReceiver = 0;
        }
        lastMousePressReceiver = 0;
        killTimer(delayedPressTimer);
        delayedPressTimer = 0;
    } else if (state == MaybeGesture && event->type() == QEvent::MouseButtonPress
               && eventDeliveryDelayTimeout) {
        // postpone the press event delivery until we know for
        // sure whether it is a gesture.
        DEBUG() << "QGestureManager: postponing mouse press event";
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        lastMousePressReceiver = receiver;
        lastMousePressEvent = QMouseEvent(QEvent::MouseButtonPress, me->pos(),
                                          me->globalPos(), me->button(),
                                          me->buttons(), me->modifiers());
        Q_ASSERT(delayedPressTimer == 0);
        delayedPressTimer = startTimer(eventDeliveryDelayTimeout);
        if (!delayedPressTimer)
            qWarning("QGestureManager: couldn't start delayed press timer!");
        ret = true;
    }
    if (delayedPressTimer && event->type() == QEvent::MouseMove) {
        // if we have postponed a mouse press event, postpone all
        // subsequent mouse move events as well.
        ret = true;
    }

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

        if (lastMousePressReceiver) {
            // we neither received a mouse release event nor gesture
            // started, so we replay stored mouse press event.
            QApplication::sendEvent(lastMousePressReceiver, &lastMousePressEvent);
            lastMousePressReceiver = 0;
        }

        lastMousePressReceiver = 0;
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
        QSet<QGesture*> gestures;
        if (QGesture *gesture = recognizer->getGesture())
            gestures << gesture;
        if(!gestures.isEmpty()) {
            //FIXME: sendGestureEvent(targetWidget, gestures);
        }
        if (result == QGestureRecognizer::GestureFinished)
            recognizer->reset();
    }
        break;
    case QGestureRecognizer::MaybeGesture: {
        DEBUG() << "QGestureManager: maybe gesture: " << recognizer;
        if (activeGestures.contains(recognizer)) {
            //FIXME: sendGestureEvent(targetWidget, QSet<QGesture*>(), QSet<QString>() << recognizer->gestureType());
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

bool QGestureManager::sendGestureEvent(QWidget *receiver,
                                       const QSet<QGesture*> &startedGestures,
                                       const QSet<QGesture*> &updatedGestures,
                                       const QSet<QString> &cancelled)
{
    DEBUG() << "QGestureManager::sendGestureEvent: sending to" << receiver
            << "gestures:" << startedGestures << "," << updatedGestures
            << "cancelled:" << cancelled;
    // grouping gesture objects by receiver widgets.
    typedef QMap<QWidget*, QSet<QGesture*> > WidgetGesturesMap;
    WidgetGesturesMap widgetGestures;
    for(QSet<QGesture*>::const_iterator it = startedGestures.begin(), e = startedGestures.end();
        it != e; ++it) {
        QGesture *g = *it;
        QGesturePrivate *gd = g->d_func();
        if (receiver) {
            // find the target widget
            gd->widget = 0;
            gd->graphicsItem = 0;
            QWidget *w = receiver;
            QPoint offset;
            const QString gestureType = g->type();
            while (w) {
                if (w->d_func()->hasGesture(gestureType))
                    break;
                if (w->isWindow()) {
                    w = 0;
                    break;
                }
                offset += w->pos();
                w = w->parentWidget();
            }
            if (w && w != gd->widget) {
                DEBUG() << "QGestureManager::sendGestureEvent:" << g << "propagating to widget" << w << "offset" << offset;
                g->translate(offset);
            }
            gd->widget = w;
        }
        if (!gd->widget) {
            DEBUG() << "QGestureManager: didn't find a widget to send gesture event ("
                    << g->type() << ") for tree:" << receiver;
            // TODO: maybe we should reset gesture recognizers when nobody interested in its gestures.
            continue;
        }
        widgetGestures[gd->widget].insert(g);
    }

    QSet<QGesture*> ignoredGestures;
    bool ret = false;
    for(WidgetGesturesMap::const_iterator it = widgetGestures.begin(), e = widgetGestures.end();
        it != e; ++it) {
        QWidget *receiver = it.key();
        Q_ASSERT(receiver != 0 /*should be taken care above*/);
        QSet<QGesture*> gestures = it.value();
        // mark all gestures as ignored by default
        for(QSet<QGesture*>::iterator it = gestures.begin(), e = gestures.end(); it != e; ++it)
            (*it)->ignore();
        // TODO: send cancelled gesture event to the widget that received the original gesture!
        QGestureEvent event(gestures, cancelled);
        DEBUG() << "QGestureManager::sendGestureEvent: sending now to" << receiver
                << "gestures" << gestures;
        bool processed = qt_sendSpontaneousEvent(receiver, &event);
        QSet<QGesture*> started = startedGestures & gestures;
        DEBUG() << "QGestureManager::sendGestureEvent:" <<
            (event.isAccepted() ? "" : "not") << "all gestures were accepted";
        if (!started.isEmpty() && !(processed && event.isAccepted())) {
            // there are started gestures events that weren't
            // accepted, so propagating each gesture independently.
            if (event.isAccepted()) {
                foreach(QGesture *g, started)
                    g->accept();
            }
            QSet<QGesture*>::const_iterator it = started.begin(),
                                             e = started.end();
            for(; it != e; ++it) {
                QGesture *g = *it;
                if (processed && g->isAccepted()) {
                    ret = true;
                    continue;
                }
                // if it wasn't accepted, find the first parent widget
                // that is subscribed to the gesture.
                QGesturePrivate *gd = g->d_func();
                QWidget *w = gd->widget;
                gd->widget = 0;

                if (w && !w->isWindow()) {
                    g->translate(w->pos());
                    w = w->parentWidget();
                    QPoint offset;
                    const QString gestureType = g->type();
                    while (w) {
                        if (w->d_func()->hasGesture(gestureType)) {
                            DEBUG() << "QGestureManager::sendGestureEvent:" << receiver
                                    << "didn't accept gesture" << g << "propagating to" << w;
                            ignoredGestures.insert(g);
                            gd->widget = w;
                            break;
                        }
                        if (w->isWindow()) {
                            w = 0;
                            break;
                        }
                        offset += w->pos();
                        w = w->parentWidget();
                    }
                    if (w) {
                        g->translate(offset);
                    } else {
                        DEBUG() << "QGestureManager::sendGestureEvent:" << receiver
                                << "didn't accept gesture" << g << "and nobody wants it";
                    }
                }
            }
        }
    }
    if (ignoredGestures.isEmpty())
        return ret;
    // try to send all gestures that were ignored to the next parent
    return sendGestureEvent(0, ignoredGestures, QSet<QGesture*>(), cancelled) || ret;
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

