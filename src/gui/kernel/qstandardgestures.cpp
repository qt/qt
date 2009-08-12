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

#include "qstandardgestures.h"
#include "qstandardgestures_p.h"

#include <qabstractscrollarea.h>
#include <qscrollbar.h>
#include <private/qapplication_p.h>
#include <private/qevent_p.h>
#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

#ifdef Q_WS_WIN
QWidgetPrivate *qt_widget_private(QWidget *widget);
#endif

/*!
    \class QPanGesture
    \since 4.6

    \brief The QPanGesture class represents a Pan gesture,
    providing additional information related to panning.
*/

/*!
    Creates a new Pan gesture handler object and marks it as a child of \a
    parent.

    On some platform like Windows it's necessary to provide a non-null widget
    as \a parent to get native gesture support.
*/
QPanGesture::QPanGesture(QWidget *parent)
    : QGesture(*new QPanGesturePrivate, parent)
{
    if (parent) {
        QApplicationPrivate *qAppPriv = QApplicationPrivate::instance();
        qAppPriv->widgetGestures[parent].pan = this;
#ifdef Q_WS_WIN
        qt_widget_private(parent)->winSetupGestures();
#endif
    }
}

/*! \internal */
bool QPanGesture::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::ParentAboutToChange:
        if (QWidget *w = qobject_cast<QWidget*>(parent())) {
            QApplicationPrivate::instance()->widgetGestures[w].pan = 0;
#ifdef Q_WS_WIN
            qt_widget_private(w)->winSetupGestures();
#endif
        }
        break;
    case QEvent::ParentChange:
        if (QWidget *w = qobject_cast<QWidget*>(parent())) {
            QApplicationPrivate::instance()->widgetGestures[w].pan = this;
#ifdef Q_WS_WIN
            qt_widget_private(w)->winSetupGestures();
#endif
        }
        break;
    default:
        break;
    }

#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    Q_D(QPanGesture);
    if (event->type() == QEvent::Timer) {
        const QTimerEvent *te = static_cast<QTimerEvent *>(event);
        if (te->timerId() == d->panFinishedTimer) {
            killTimer(d->panFinishedTimer);
            d->panFinishedTimer = 0;
            d->lastOffset = QSize(0, 0);
            updateState(Qt::GestureFinished);
        }
    }
#endif

    return QObject::event(event);
}

bool QPanGesture::eventFilter(QObject *receiver, QEvent *event)
{
#ifdef Q_WS_WIN
    Q_D(QPanGesture);
    if (receiver->isWidgetType() && event->type() == QEvent::NativeGesture) {
        QNativeGestureEvent *ev = static_cast<QNativeGestureEvent*>(event);
        QApplicationPrivate *qAppPriv = QApplicationPrivate::instance();
        QApplicationPrivate::WidgetStandardGesturesMap::iterator it;
        it = qAppPriv->widgetGestures.find(static_cast<QWidget*>(receiver));
        if (it == qAppPriv->widgetGestures.end())
            return false;
        QPanGesture *gesture = it.value().pan;
        if (this != gesture)
            return false;
        Qt::GestureState nextState = Qt::NoGesture;
        switch(ev->gestureType) {
        case QNativeGestureEvent::GestureBegin:
            // next we might receive the first gesture update event, so we
            // prepare for it.
            d->state = Qt::NoGesture;
            return false;
        case QNativeGestureEvent::Pan:
            nextState = Qt::GestureUpdated;
            event->accept();
            break;
        case QNativeGestureEvent::GestureEnd:
            if (state() == Qt::NoGesture)
                return false; // some other gesture has ended
            nextState = Qt::GestureFinished;
            break;
        default:
            return false;
        }
        if (state() == Qt::NoGesture) {
            d->lastOffset = d->totalOffset = QSize();
        } else {
            d->lastOffset = QSize(ev->position.x() - d->lastPosition.x(),
                                  ev->position.y() - d->lastPosition.y());
            d->totalOffset += d->lastOffset;
        }
        d->lastPosition = ev->position;
        gesture->updateState(nextState);
        return true;
    }
#endif
    return QGesture::eventFilter(receiver, event);
}

/*! \internal */
bool QPanGesture::filterEvent(QEvent *event)
{
    Q_D(QPanGesture);
    if (!event->spontaneous())
        return false;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);
    if (event->type() == QEvent::TouchBegin) {
        QTouchEvent::TouchPoint p = ev->touchPoints().at(0);
        d->lastPosition = p.pos().toPoint();
        d->lastOffset = d->totalOffset = QSize();
    } else if (event->type() == QEvent::TouchEnd) {
        if (state() != Qt::NoGesture) {
            if (!ev->touchPoints().isEmpty()) {
                QTouchEvent::TouchPoint p = ev->touchPoints().at(0);
                const QPoint pos = p.pos().toPoint();
                const QPoint lastPos = p.lastPos().toPoint();
                const QPoint startPos = p.startPos().toPoint();
                d->lastOffset = QSize(pos.x() - lastPos.x(), pos.y() - lastPos.y());
                d->totalOffset = QSize(pos.x() - startPos.x(), pos.y() - startPos.y());
            }
            updateState(Qt::GestureFinished);
        }
        reset();
    } else if (event->type() == QEvent::TouchUpdate) {
        QTouchEvent::TouchPoint p = ev->touchPoints().at(0);
        const QPoint pos = p.pos().toPoint();
        const QPoint lastPos = p.lastPos().toPoint();
        const QPoint startPos = p.startPos().toPoint();
        d->lastOffset = QSize(pos.x() - lastPos.x(), pos.y() - lastPos.y());
        d->totalOffset = QSize(pos.x() - startPos.x(), pos.y() - startPos.y());
        if (d->totalOffset.width() > 10  || d->totalOffset.height() > 10 ||
            d->totalOffset.width() < -10 || d->totalOffset.height() < -10) {
            updateState(Qt::GestureUpdated);
        }
    }
#ifdef Q_OS_MAC
    else if (event->type() == QEvent::Wheel) {
        // On Mac, there is really no native panning gesture. Instead, a two 
        // finger pan is delivered as mouse wheel events. Otoh, on Windows, you
        // either get mouse wheel events or pan events. We have decided to make this
        // the Qt behaviour as well, meaning that on Mac, wheel
        // events will be masked away when listening for pan events.
#ifndef QT_MAC_USE_COCOA
        // In Carbon we receive neither touch-, nor pan gesture events.
        // So we create pan gestures by converting wheel events. After all, this
        // is how things are supposed to work on mac in the first place.
        const QWheelEvent *wev = static_cast<const QWheelEvent*>(event);
        int offset = wev->delta() / -120;
        d->lastOffset = wev->orientation() == Qt::Horizontal ? QSize(offset, 0) : QSize(0, offset);

        if (state() == Qt::NoGesture) {
            d->totalOffset = d->lastOffset;
        } else {
            d->totalOffset += d->lastOffset;
        }

        killTimer(d->panFinishedTimer);
        d->panFinishedTimer = startTimer(200);
        updateState(Qt::GestureUpdated);
#endif
        return true;
    }
#endif
    return false;
}

/*! \internal */
void QPanGesture::reset()
{
    Q_D(QPanGesture);
    d->lastOffset = d->totalOffset = QSize();
    d->lastPosition = QPoint();
#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    if (d->panFinishedTimer) {
        killTimer(d->panFinishedTimer);
        d->panFinishedTimer = 0;
    }
#endif
}

/*!
    \property QPanGesture::totalOffset

    Specifies a total pan offset since the start of the gesture.
*/
QSize QPanGesture::totalOffset() const
{
    Q_D(const QPanGesture);
    return d->totalOffset;
}

/*!
    \property QPanGesture::lastOffset

    Specifies a pan offset since the last time the gesture was
    triggered.
*/
QSize QPanGesture::lastOffset() const
{
    Q_D(const QPanGesture);
    return d->lastOffset;
}

QT_END_NAMESPACE

#include "moc_qstandardgestures.cpp"

