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

#include "qgesture.h"
#include <private/qgesture_p.h>

QGesture::QGesture(const Qt::GestureType &type, Qt::GestureState state)
    : d(new QGesturePrivate), gestureType_(type), gestureState_(state)
{
}

QGesture::QGesture(const Qt::GestureType &type, const QPoint &startPos,
                   const QPoint &lastPos, const QPoint &pos, const QRect &rect,
                   const QPoint &hotSpot, const QDateTime &startTime,
                   uint duration, Qt::GestureState state)
    : d(new QGesturePrivate), gestureType_(type), gestureState_(state)
{
    d->init(startPos, lastPos, pos, rect, hotSpot, startTime, duration);
}

QGesture::QGesture(QGesturePrivate &dd, const Qt::GestureType &type,
                   Qt::GestureState state)
    : d(&dd), gestureType_(type), gestureState_(state)
{
}

QGesture::~QGesture()
{
    delete d; d = 0;
}

QRect QGesture::rect() const
{
    return d->rect;
}

QPoint QGesture::hotSpot() const
{
    return d->hotSpot;
}

QDateTime QGesture::startTime() const
{
    return d->startTime;
}

uint QGesture::duration() const
{
    return d->duration;
}

QPoint QGesture::startPos() const
{
    return d->startPos;
}

QPoint QGesture::lastPos() const
{
    return d->lastPos;
}

QPoint QGesture::pos() const
{
    return d->pos;
}

QPannableGesture::QPannableGesture(const Qt::GestureType &type, Qt::GestureState state)
    : QGesture(*new QPannableGesturePrivate, type, state)
{
}

QPannableGesture::QPannableGesture(const Qt::GestureType &type, const QPoint &startPos,
                   const QPoint &lastPos, const QPoint &pos, const QRect &rect,
                   const QPoint &hotSpot, const QDateTime &startTime,
                   uint duration, Qt::GestureState state)
    : QGesture(*new QPannableGesturePrivate, type, state)
{
    d->init(startPos, lastPos, pos, rect, hotSpot, startTime, duration);
    ((QPannableGesturePrivate*)d)->lastDirection = QPannableGesture::None;
    ((QPannableGesturePrivate*)d)->direction = QPannableGesture::None;
}

QPannableGesture::~QPannableGesture()
{
}

QPannableGesture::DirectionType QPannableGesture::lastDirection() const
{
    return ((QPannableGesturePrivate*)d)->lastDirection;
}

QPannableGesture::DirectionType QPannableGesture::direction() const
{
    return ((QPannableGesturePrivate*)d)->direction;
}
