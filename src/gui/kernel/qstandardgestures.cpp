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

QT_BEGIN_NAMESPACE

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
#ifdef Q_WS_WIN
    if (parent) {
        QApplicationPrivate* getQApplicationPrivateInternal();
        QApplicationPrivate *qAppPriv = getQApplicationPrivateInternal();
        qAppPriv->widgetGestures[parent].pan = this;
    }
#endif

#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    d_func()->panFinishedTimer = 0;
#endif
}

/*! \internal */
bool QPanGesture::event(QEvent *event)
{
#ifdef Q_WS_WIN
    QApplicationPrivate* getQApplicationPrivateInternal();
    switch (event->type()) {
    case QEvent::ParentAboutToChange:
        if (QWidget *w = qobject_cast<QWidget*>(parent()))
            getQApplicationPrivateInternal()->widgetGestures[w].pan = 0;
        break;
    case QEvent::ParentChange:
        if (QWidget *w = qobject_cast<QWidget*>(parent()))
            getQApplicationPrivateInternal()->widgetGestures[w].pan = this;
        break;
    default:
        break;
    }
#endif

#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    Q_D(QPanGesture);
    if (event->type() == QEvent::Timer) {
        const QTimerEvent *te = static_cast<QTimerEvent *>(event);
        if (te->timerId() == d->panFinishedTimer) {
            killTimer(d->panFinishedTimer);
            d->panFinishedTimer = 0;
            d->lastOffset = QSize(0, 0);
            setState(Qt::GestureFinished);
            emit triggered();
            setState(Qt::NoGesture);
        }
    }
#endif

    return QObject::event(event);
}

/*! \internal */
bool QPanGesture::filterEvent(QEvent *event)
{
    Q_D(QPanGesture);
    if (!event->spontaneous())
        return false;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);
    if (event->type() == QEvent::TouchBegin) {
        d->touchPoints = ev->touchPoints();
        const QPoint p = ev->touchPoints().at(0).pos().toPoint();
        setStartPos(p);
        setLastPos(p);
        setPos(p);
        return false;
    } else if (event->type() == QEvent::TouchEnd) {
        if (state() != Qt::NoGesture) {
            setState(Qt::GestureFinished);
            setLastPos(pos());
            setPos(ev->touchPoints().at(0).pos().toPoint());
            emit triggered();
            emit finished();
        }
        setState(Qt::NoGesture);
        reset();
    } else if (event->type() == QEvent::TouchUpdate) {
        d->touchPoints = ev->touchPoints();
        QPointF pt = d->touchPoints.at(0).pos() - d->touchPoints.at(0).startPos();
        setLastPos(pos());
        setPos(ev->touchPoints().at(0).pos().toPoint());
        if (pt.x() > 10 || pt.y() > 10 || pt.x() < -10 || pt.y() < -10) {
            if (state() == Qt::NoGesture)
                setState(Qt::GestureStarted);
            else
                setState(Qt::GestureUpdated);
            emit triggered();
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
            setState(Qt::GestureStarted);
            d->totalOffset = d->lastOffset;
        } else {
            setState(Qt::GestureUpdated);
            d->totalOffset += d->lastOffset;
        }

        killTimer(d->panFinishedTimer);
        d->panFinishedTimer = startTimer(200);
        emit triggered();
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
    d->touchPoints.clear();
}

/*!
    \property QPanGesture::totalOffset

    Specifies a total pan offset since the start of the gesture.
*/
QSize QPanGesture::totalOffset() const
{
#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    Q_D(const QPanGesture);
    return d->totalOffset;
#else
    QPoint pt = pos() - startPos();
    return QSize(pt.x(), pt.y());
#endif
}

/*!
    \property QPanGesture::lastOffset

    Specifies a pan offset since the last time the gesture was
    triggered.
*/
QSize QPanGesture::lastOffset() const
{
#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
    Q_D(const QPanGesture);
    return d->lastOffset;
#else
    QPoint pt = pos() - lastPos();
    return QSize(pt.x(), pt.y());
#endif
}

/*!
    \class QTapAndHoldGesture
    \since 4.6

    \brief The QTapAndHoldGesture class represents a Tap-and-Hold gesture,
    providing additional information.
*/

const int QTapAndHoldGesturePrivate::iterationCount = 40;
const int QTapAndHoldGesturePrivate::iterationTimeout = 50;

/*!
    Creates a new Tap and Hold gesture handler object and marks it as a child
    of \a parent.

    On some platforms like Windows there is a system-wide tap and hold gesture
    that cannot be overriden, hence the gesture might never trigger and default
    context menu will be shown instead.
*/
QTapAndHoldGesture::QTapAndHoldGesture(QWidget *parent)
    : QGesture(*new QTapAndHoldGesturePrivate, parent)
{
}

/*! \internal */
bool QTapAndHoldGesture::filterEvent(QEvent *event)
{
    Q_D(QTapAndHoldGesture);
    if (!event->spontaneous())
        return false;
    const QTouchEvent *ev = static_cast<const QTouchEvent*>(event);
    switch (event->type()) {
    case QEvent::TouchBegin: {
        if (d->timer.isActive())
            d->timer.stop();
        d->timer.start(QTapAndHoldGesturePrivate::iterationTimeout, this);
        const QPoint p = ev->touchPoints().at(0).pos().toPoint();
        setStartPos(p);
        setLastPos(p);
        setPos(p);
        break;
    }
    case QEvent::TouchUpdate:
        if (ev->touchPoints().size() != 1)
            reset();
        else if ((startPos() - ev->touchPoints().at(0).pos().toPoint()).manhattanLength() > 15)
            reset();
        break;
    case QEvent::TouchEnd:
        reset();
        break;
    default:
        break;
    }
    return false;
}

/*! \internal */
void QTapAndHoldGesture::timerEvent(QTimerEvent *event)
{
    Q_D(QTapAndHoldGesture);
    if (event->timerId() != d->timer.timerId())
        return;
    if (d->iteration == QTapAndHoldGesturePrivate::iterationCount) {
        d->timer.stop();
        setState(Qt::GestureFinished);
        emit triggered();
    } else {
        setState(Qt::GestureStarted);
        emit triggered();
    }
    ++d->iteration;
}

/*! \internal */
void QTapAndHoldGesture::reset()
{
    Q_D(QTapAndHoldGesture);
    if (state() != Qt::NoGesture)
        emit cancelled();
    setState(Qt::NoGesture);
    d->timer.stop();
    d->iteration = 0;
}

QT_END_NAMESPACE

#include "moc_qstandardgestures.cpp"

