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

#include "qgesturerecognizer.h"
#include "qgesture.h"

#include <private/qobject_p.h>
#include <private/qgesturerecognizer_p.h>

QT_BEGIN_NAMESPACE

QString qt_getStandardGestureTypeName(Qt::GestureType gestureType);

/*!
    \class QGestureRecognizer
    \since 4.6

    \brief The QGestureRecognizer class is the base class for
    implementing custom gestures.

    This is a base class, to create a custom gesture type, you should
    subclass it and implement its pure virtual functions.

    Usually gesture recognizer implements state machine, storing its
    state internally in the recognizer object. The recognizer receives
    input events through the \l{QGestureRecognizer::}{filterEvent()}
    virtual function and decides whether the parsed event should
    change the state of the recognizer - i.e. if the event starts or
    ends a gesture or if it isn't related to gesture at all.
*/

/*!
    \enum QGestureRecognizer::Result
    \since 4.6

    This enum type defines the state of the gesture recognizer.

    \value Ignore Gesture recognizer ignores the event.

    \value NotGesture Not a gesture.

    \value GestureStarted The continuous gesture has started. When the
    recognizer is in this state, a \l{QGestureEvent}{gesture event}
    containing QGesture objects returned by the
    \l{QGestureRecognizer::}{getGesture()} will be sent to a widget.

    \value GestureFinished The gesture has ended. A
    \l{QGestureEvent}{gesture event} will be sent to a widget.

    \value MaybeGesture Gesture recognizer hasn't decided yet if a
    gesture has started, but it might start soon after the following
    events are received by the recognizer. This means that gesture
    manager shouldn't reset() the internal state of the gesture
    recognizer.
*/

/*! \fn QGestureRecognizer::Result QGestureRecognizer::filterEvent(const QEvent *event)

    This is a pure virtual function that needs to be implemented in
    subclasses.

    Parses input \a event and returns the result, which specifies if
    the event sequence is a gesture or not.
*/

/*! \fn QGesture* QGestureRecognizer::getGesture()

    Returns a gesture object that will be send to the widget. This
    function is called when the gesture recognizer changed its state
    to QGestureRecognizer::GestureStarted or
    QGestureRecognizer::GestureFinished.

    The returned QGesture object must point to the same object in a
    single gesture sequence.

    The gesture object is owned by the recognizer itself.
*/

/*! \fn void QGestureRecognizer::reset()

    Resets the internal state of the gesture recognizer.
*/

/*! \fn void QGestureRecognizer::stateChanged(QGestureRecognizer::Result result)

    The gesture recognizer might emit the stateChanged() signal when
    the gesture state changes asynchronously, i.e. without any event
    being filtered through filterEvent().
*/

QGestureRecognizerPrivate::QGestureRecognizerPrivate()
    : gestureType(Qt::UnknownGesture)
{
}

/*!
    Creates a new gesture recognizer object that handles gestures of
    the specific \a gestureType as a child of \a parent.

    \sa QApplication::addGestureRecognizer(),
    QApplication::removeGestureRecognizer(),
*/
QGestureRecognizer::QGestureRecognizer(const QString &gestureType, QObject *parent)
    : QObject(*new QGestureRecognizerPrivate, parent)
{
    Q_D(QGestureRecognizer);
    d->customGestureType = gestureType;
}

/*!
    Returns the name of the gesture that is handled by the recognizer.
*/
QString QGestureRecognizer::gestureType() const
{
    Q_D(const QGestureRecognizer);
    if (d->gestureType == Qt::UnknownGesture)
        return d->customGestureType;
    return qt_getStandardGestureTypeName(d->gestureType);
}

QT_END_NAMESPACE
