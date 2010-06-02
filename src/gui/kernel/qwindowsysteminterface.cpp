/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qwindowsysteminterface.h"
#include "qapplication_p.h"
#include <QAbstractEventDispatcher>

QT_BEGIN_NAMESPACE


QTime QWindowSystemInterface::eventTime;

//------------------------------------------------------------
//
// Callback functions for plugins:
//

QList<QWindowSystemInterface::UserEvent *> QWindowSystemInterfacePrivate::userEventQueue;
QMutex QWindowSystemInterfacePrivate::queueMutex;

extern QPointer<QWidget> qt_last_mouse_receiver;
/*!

\a tlw == 0 means that \a ev is in global coords only


*/


void QWindowSystemInterface::handleEnterEvent(QWidget *tlw)
{
    if (tlw) {
        QApplicationPrivate::dispatchEnterLeave(tlw, 0);
        qt_last_mouse_receiver = tlw;
    }
}

void QWindowSystemInterface::handleLeaveEvent(QWidget *tlw)
{
    QApplicationPrivate::dispatchEnterLeave(0, qt_last_mouse_receiver);
    if (tlw && !tlw->isAncestorOf(qt_last_mouse_receiver)) //(???) this should not happen
        QApplicationPrivate::dispatchEnterLeave(0, tlw);
    qt_last_mouse_receiver = 0;
}

void QWindowSystemInterface::handleGeometryChange(QWidget *tlw, const QRect &newRect)
{
    if (tlw)
        QApplicationPrivate::processGeometryChange(tlw, newRect);
}


void QWindowSystemInterface::handleCloseEvent(QWidget *tlw)
{
    if (tlw)
        QApplicationPrivate::processCloseEvent(tlw);
}

void QWindowSystemInterface::handleMouseEvent(QWidget *tlw, ulong timestamp, const QPoint & local, const QPoint & global, Qt::MouseButtons b)
{
    MouseEvent * e = new MouseEvent(tlw, timestamp, local, global, b);
    QWindowSystemInterfacePrivate::queueUserEvent(e);
}

void QWindowSystemInterface::handleKeyEvent(QWidget *tlw, ulong timestamp, QEvent::Type t, int k, Qt::KeyboardModifiers mods, const QString & text, bool autorep, ushort count)
{
    KeyEvent * e = new KeyEvent(tlw, timestamp, t, k, mods, text, autorep, count);
    QWindowSystemInterfacePrivate::queueUserEvent(e);
}

void QWindowSystemInterface::handleWheelEvent(QWidget *tlw, ulong timestamp, const QPoint & local, const QPoint & global, int d, Qt::Orientation o)
{
    WheelEvent *e = new WheelEvent(tlw, timestamp, local, global, d, o);
    QWindowSystemInterfacePrivate::queueUserEvent(e);
}

QWindowSystemInterface::UserEvent * QWindowSystemInterfacePrivate::getUserEvent()
{
    queueMutex.lock();
    QWindowSystemInterface::UserEvent *ret;
    if (userEventQueue.isEmpty())
        ret = 0;
    else
        ret = userEventQueue.takeFirst();
    queueMutex.unlock();
    return ret;
}

void QWindowSystemInterfacePrivate::queueUserEvent(QWindowSystemInterface::UserEvent *ev)
{
    queueMutex.lock();
    userEventQueue.append(ev);
    queueMutex.unlock();

    QAbstractEventDispatcher *dispatcher = QApplicationPrivate::qt_lite_core_dispatcher();
    if (dispatcher)
        dispatcher->wakeUp();
}

void QWindowSystemInterface::handleTouchEvent(QWidget *tlw, ulong timestamp, QEvent::Type type, QTouchEvent::DeviceType devType, const QList<struct TouchPoint> &points)
{
    if (!points.size()) // Touch events must have at least one point
        return;

    QList<QTouchEvent::TouchPoint> touchPoints;
    Qt::TouchPointStates states;
    QTouchEvent::TouchPoint p;

    int primaryPoint = -1;
    QList<struct TouchPoint>::const_iterator point = points.constBegin();
    QList<struct TouchPoint>::const_iterator end = points.constEnd();
    while (point != end) {
        p.setId(point->id);
        p.setPressure(point->pressure);
        states |= point->state;
        Qt::TouchPointStates state = point->state;
        if (point->isPrimary) {
            state |= Qt::TouchPointPrimary;
            primaryPoint = point->id;
        }
        p.setState(state);
        p.setRect(point->area);
        p.setScreenPos(point->area.center());
        p.setNormalizedPos(point->normalPosition);

        touchPoints.append(p);
        ++point;
    }

    TouchEvent *e = new TouchEvent(tlw, timestamp, type, devType, touchPoints);
    QWindowSystemInterfacePrivate::queueUserEvent(e);
}

void QWindowSystemInterface::handleScreenGeometryChange(int screenIndex)
{
    QApplicationPrivate::reportGeometryChange(screenIndex);
}

void QWindowSystemInterface::handleScreenAvailableGeometryChange(int screenIndex)
{
    QApplicationPrivate::reportAvailableGeometryChange(screenIndex);
}

void QWindowSystemInterface::handleScreenCountChange(int count)
{
    QApplicationPrivate::reportScreenCount(count);
}

QT_END_NAMESPACE
