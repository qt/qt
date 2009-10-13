/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgesture.h"
#include "private/qgesture_p.h"

QT_BEGIN_NAMESPACE

 /*!
    \class QGesture
    \since 4.6

    \brief The QGesture class represents a gesture, containing properties that
    describe the corresponding user input.

    Gesture objects are not constructed directly by developers. They are created by
    the QGestureRecognizer object that is registered with the application; see
    QApplication::registerGestureRecognizer().

    \section1 Gesture Properties

    The class has a list of properties that can be queried by the user to get
    some gesture-specific arguments. For example, the pinch gesture has a scale
    factor that is exposed as a property.

    Developers of custom gesture recognizers can add additional properties in
    order to provide additional information about a gesture. This can be done
    by adding new dynamic properties to a QGesture object, or by subclassing
    the QGesture class (or one of its subclasses).

    \section1 Lifecycle of a Gesture Object

    A QGesture instance is created when the application calls QWidget::grabGesture()
    or QGraphicsObject::grabGesture() to configure a widget or graphics object (the
    target object) for gesture input. One gesture object is created for each target
    object.

    The registered gesture recognizer monitors the input events for the target
    object via its \l{QGestureRecognizer::}{filterEvent()} function, updating the
    properties of the gesture object as required.

    The gesture object may be delivered to the target object in a QGestureEvent if
    the corresponding gesture is active or has just been canceled. Each event that
    is delivered contains a list of gesture objects, since support for more than
    one gesture may be enabled for the target object. Due to the way events are
    handled in Qt, gesture events may be filtered by other objects.

    \sa QGestureEvent, QGestureRecognizer
*/

/*!
    Constructs a new gesture object with the given \a parent.

    QGesture objects are created by gesture recognizers in the
    QGestureRecognizer::createGesture() function.
*/
QGesture::QGesture(QObject *parent)
    : QObject(*new QGesturePrivate, parent)
{
    d_func()->gestureType = Qt::CustomGesture;
}

/*!
    \internal
*/
QGesture::QGesture(QGesturePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    Destroys the gesture object.
*/
QGesture::~QGesture()
{
}

/*!
    \property QGesture::state
    \brief the current state of the gesture
*/

/*!
    \property QGesture::gestureType
    \brief the type of the gesture
*/

/*!
    \property QGesture::hotSpot

    \brief The point that is used to find the receiver for the gesture event.

    If the hot-spot is not set, the targetObject is used as the receiver of the
    gesture event.
*/

/*!
    \property QGesture::hasHotSpot
    \brief whether the gesture has a hot-spot
*/

/*!
    \property QGesture::targetObject
    \brief the target object which will receive the gesture event if the hotSpot is
    not set
*/

Qt::GestureType QGesture::gestureType() const
{
    return d_func()->gestureType;
}

Qt::GestureState QGesture::state() const
{
    return d_func()->state;
}

QObject *QGesture::targetObject() const
{
    return d_func()->targetObject;
}

void QGesture::setTargetObject(QObject *value)
{
    d_func()->targetObject = value;
}

QPointF QGesture::hotSpot() const
{
    return d_func()->hotSpot;
}

void QGesture::setHotSpot(const QPointF &value)
{
    Q_D(QGesture);
    d->hotSpot = value;
    d->isHotSpotSet = true;
}

bool QGesture::hasHotSpot() const
{
    return d_func()->isHotSpotSet;
}

void QGesture::unsetHotSpot()
{
    d_func()->isHotSpotSet = false;
}

// QPanGesture

QPanGesture::QPanGesture(QObject *parent)
    : QGesture(*new QPanGesturePrivate, parent)
{
    d_func()->gestureType = Qt::PanGesture;
}

QSizeF QPanGesture::totalOffset() const
{
    return d_func()->totalOffset;
}

QSizeF QPanGesture::lastOffset() const
{
    return d_func()->lastOffset;
}

QSizeF QPanGesture::offset() const
{
    return d_func()->offset;
}

qreal QPanGesture::acceleration() const
{
    return d_func()->acceleration;
}


void QPanGesture::setTotalOffset(const QSizeF &value)
{
    d_func()->totalOffset = value;
}

void QPanGesture::setLastOffset(const QSizeF &value)
{
    d_func()->lastOffset = value;
}

void QPanGesture::setOffset(const QSizeF &value)
{
    d_func()->offset = value;
}

void QPanGesture::setAcceleration(qreal value)
{
    d_func()->acceleration = value;
}

// QPinchGesture

QPinchGesture::QPinchGesture(QObject *parent)
    : QGesture(*new QPinchGesturePrivate, parent)
{
    d_func()->gestureType = Qt::PinchGesture;
}

QPinchGesture::WhatChanged QPinchGesture::whatChanged() const
{
    return d_func()->whatChanged;
}

void QPinchGesture::setWhatChanged(QPinchGesture::WhatChanged value)
{
    d_func()->whatChanged = value;
}


QPointF QPinchGesture::startCenterPoint() const
{
    return d_func()->startCenterPoint;
}

QPointF QPinchGesture::lastCenterPoint() const
{
    return d_func()->lastCenterPoint;
}

QPointF QPinchGesture::centerPoint() const
{
    return d_func()->centerPoint;
}

void QPinchGesture::setStartCenterPoint(const QPointF &value)
{
    d_func()->startCenterPoint = value;
}

void QPinchGesture::setLastCenterPoint(const QPointF &value)
{
    d_func()->lastCenterPoint = value;
}

void QPinchGesture::setCenterPoint(const QPointF &value)
{
    d_func()->centerPoint = value;
}


qreal QPinchGesture::totalScaleFactor() const
{
    return d_func()->totalScaleFactor;
}

qreal QPinchGesture::lastScaleFactor() const
{
    return d_func()->lastScaleFactor;
}

qreal QPinchGesture::scaleFactor() const
{
    return d_func()->scaleFactor;
}

void QPinchGesture::setTotalScaleFactor(qreal value)
{
    d_func()->totalScaleFactor = value;
}

void QPinchGesture::setLastScaleFactor(qreal value)
{
    d_func()->lastScaleFactor = value;
}

void QPinchGesture::setScaleFactor(qreal value)
{
    d_func()->scaleFactor = value;
}


qreal QPinchGesture::totalRotationAngle() const
{
    return d_func()->totalRotationAngle;
}

qreal QPinchGesture::lastRotationAngle() const
{
    return d_func()->lastRotationAngle;
}

qreal QPinchGesture::rotationAngle() const
{
    return d_func()->rotationAngle;
}

void QPinchGesture::setTotalRotationAngle(qreal value)
{
    d_func()->totalRotationAngle = value;
}

void QPinchGesture::setLastRotationAngle(qreal value)
{
    d_func()->lastRotationAngle = value;
}

void QPinchGesture::setRotationAngle(qreal value)
{
    d_func()->rotationAngle = value;
}

// QSwipeGesture

QSwipeGesture::QSwipeGesture(QObject *parent)
    : QGesture(*new QSwipeGesturePrivate, parent)
{
    d_func()->gestureType = Qt::SwipeGesture;
}

QSwipeGesture::SwipeDirection QSwipeGesture::horizontalDirection() const
{
    Q_D(const QSwipeGesture);
    if (d->swipeAngle < 0 || d->swipeAngle == 90 || d->swipeAngle == 270)
        return QSwipeGesture::NoDirection;
    else if (d->swipeAngle < 90 || d->swipeAngle > 270)
        return QSwipeGesture::Right;
    else
        return QSwipeGesture::Left;
}

QSwipeGesture::SwipeDirection QSwipeGesture::verticalDirection() const
{
    Q_D(const QSwipeGesture);
    if (d->swipeAngle <= 0 || d->swipeAngle == 180)
        return QSwipeGesture::NoDirection;
    else if (d->swipeAngle < 180)
        return QSwipeGesture::Up;
    else
        return QSwipeGesture::Down;
}

qreal QSwipeGesture::swipeAngle() const
{
    return d_func()->swipeAngle;
}

void QSwipeGesture::setSwipeAngle(qreal value)
{
    d_func()->swipeAngle = value;
}

QT_END_NAMESPACE
