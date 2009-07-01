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

#include "qgesture.h"
#include <private/qgesture_p.h>

QT_BEGIN_NAMESPACE

QString qt_getStandardGestureTypeName(Qt::GestureType type);

/*!
    \class QGesture
    \since 4.6

    \brief The QGesture class represents a gesture, containing all
    properties that describe a gesture.

    The widget receives a QGestureEvent with a list of QGesture
    objects that represent gestures that are occuring on it. The class
    has a list of properties that can be queried by the user to get
    some gesture-specific arguments (i.e. position of the tap in the
    DoubleTap gesture).

    When creating custom gesture recognizers, they might add new
    properties to the gesture object, or custom gesture developers
    might subclass the QGesture objects to provide some extended
    information. However, if the gesture developer wants to add a new
    property to the gesture object that describe coordinate (like a
    QPoint or QRect), it is required to subclass the QGesture and
    re-implement the \l{QGesture::}{translate()} function to make sure
    the coordinates are translated properly when the gesture event is
    propagated to parent widgets.

    \sa QGestureEvent, QGestureRecognizer
*/

/*!
    Creates a new gesture object of type \a type in a \a state and
    marks it as a child of \a parent.

    Usually QGesture objects should only be contructed by the
    QGestureRecognizer classes.
*/
QGesture::QGesture(QObject *parent, const QString &type, Qt::GestureState state)
    : QObject(*new QGesturePrivate, parent), m_accept(0)
{
    Q_D(QGesture);
    d->type = type;
    d->state = state;
}

/*!
    Creates a new gesture object of type \a type in a \a state and
    marks it as a child of \a parent.

    This constructor also fills some basic information about the
    gesture like a \a startPos which describes the start point of the
    gesture, \a lastPos - last point where the gesture happened, \a
    pos - a current point, \a rect - a bounding rect of the gesture,
    \a hotSpot - a center point of the gesture, \a startTime - a time
    when the gesture has started, \a duration - how long the gesture
    is going on.

    Usually QGesture objects should only be contructed by the
    QGestureRecognizer classes.
*/
QGesture::QGesture(QObject *parent, const QString &type, const QPoint &startPos,
                   const QPoint &lastPos, const QPoint &pos, const QRect &rect,
                   const QPoint &hotSpot, const QDateTime &startTime,
                   uint duration, Qt::GestureState state)
    : QObject(*new QGesturePrivate, parent)
{
    Q_D(QGesture);
    d->type = type;
    d->state = state;
    d->init(startPos, lastPos, pos, rect, hotSpot, startTime, duration);
}

/*! \internal
*/
QGesture::QGesture(QGesturePrivate &dd, QObject *parent, const QString &type,
                   Qt::GestureState state)
    : QObject(dd, parent)
{
    Q_D(QGesture);
    d->type = type;
    d->state = state;
}

/*!
    Destroys the gesture object.
*/
QGesture::~QGesture()
{
}

/*!
    \property QGesture::type

    \brief The type of the gesture.
*/
QString QGesture::type() const
{
    return d_func()->type;
}


/*!
    \property QGesture::state

    \brief The current state of the gesture.
*/
Qt::GestureState QGesture::state() const
{
    return d_func()->state;
}

/*!
    Translates the internal gesture properties that represent
    coordinates by \a offset.

    Custom gesture recognizer developer have to re-implement this
    function if they want to store custom properties that represent
    coordinates.
*/
void QGesture::translate(const QPoint &offset)
{
    Q_D(QGesture);
    d->rect.translate(offset);
    d->hotSpot += offset;
    d->startPos += offset;
    d->lastPos += offset;
    d->pos += offset;
}

/*!
    \property QGesture::rect

    \brief The bounding rect of a gesture.
*/
QRect QGesture::rect() const
{
    return d_func()->rect;
}

void QGesture::setRect(const QRect &rect)
{
    d_func()->rect = rect;
}

/*!
    \property QGesture::hotSpot

    \brief The center point of a gesture.
*/
QPoint QGesture::hotSpot() const
{
    return d_func()->hotSpot;
}

void QGesture::setHotSpot(const QPoint &point)
{
    d_func()->hotSpot = point;
}

/*!
    \property QGesture::startTime

    \brief The time when the gesture has started.
*/
QDateTime QGesture::startTime() const
{
    return d_func()->startTime;
}

/*!
    \property QGesture::duration

    \brief The duration time of a gesture.
*/
uint QGesture::duration() const
{
    return d_func()->duration;
}

/*!
    \property QGesture::startPos

    \brief The start position of the pointer.
*/
QPoint QGesture::startPos() const
{
    return d_func()->startPos;
}

void QGesture::setStartPos(const QPoint &point)
{
    d_func()->startPos = point;
}

/*!
    \property QGesture::lastPos

    \brief The last recorded position of the pointer.
*/
QPoint QGesture::lastPos() const
{
    return d_func()->lastPos;
}

void QGesture::setLastPos(const QPoint &point)
{
    d_func()->lastPos = point;
}

/*!
    \property QGesture::pos

    \brief The current position of the pointer.
*/
QPoint QGesture::pos() const
{
    return d_func()->pos;
}

void QGesture::setPos(const QPoint &point)
{
    d_func()->pos = point;
}

/*! \fn void QGesture::setAccepted(bool accepted)
  Marks the gesture with the value of \a accepted.
 */

/*! \fn bool QGesture::isAccepted() const
  Returns true if the gesture is marked accepted.
 */

/*! \fn void QGesture::accept()
  Marks the gesture accepted.
*/

/*! \fn void QGesture::ignore()
  Marks the gesture ignored.
*/

/*!
    \class QPanningGesture
    \since 4.6

    \brief The QPanningGesture class represents a Pan gesture,
    providing additional information related to panning.

    This class is provided for convenience, panning direction
    information is also contained in the QGesture object in it's
    properties.
*/

/*! \internal
*/
QPanningGesture::QPanningGesture(QObject *parent)
    : QGesture(*new QPanningGesturePrivate, parent,
               qt_getStandardGestureTypeName(Qt::PanGesture))
{
}

/*! \internal
*/
QPanningGesture::~QPanningGesture()
{
}

/*!
    \property QPanningGesture::lastDirection

    \brief The last recorded direction of panning.
*/
Qt::DirectionType QPanningGesture::lastDirection() const
{
    return d_func()->lastDirection;
}

/*!
    \property QPanningGesture::direction

    \brief The current direction of panning.
*/
Qt::DirectionType QPanningGesture::direction() const
{
    return d_func()->direction;
}

QT_END_NAMESPACE
