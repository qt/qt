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

#include "qgesturerecognizer.h"

#include "private/qgesture_p.h"

QT_BEGIN_NAMESPACE

/*!
   \class QGestureRecognizer
   \since 4.6

   \brief The QGestureRecognizer is a base class for implementing custom
   gestures.

   \sa QGesture
*/

/*!
    \enum QGestureRecognizer::ResultFlags

    This enum type describes the result of the current event filtering step in
    a gesture recognizer state machine.

    The result consists of a state value (one of Ignore, NotGesture,
    MaybeGesture, GestureTriggered, GestureFinished) and an optional hint
    (ConsumeEventHint).

    \value Ignore The event doesn't change the state of the recognizer.

    \value NotGesture The event made it clear that it is not a gesture. If the
    gesture recognizer was in GestureTriggered state before, then the gesture
    is canceled and the appropriate QGesture object will be delivered to the
    target as a part of a QGestureEvent.

    \value MaybeGesture The event changed the internal state of the recognizer,
    but it isn't clear yet if it is a gesture or not. The recognizer needs to
    filter more events to decide. Gesture recognizers in the MaybeGesture state
    may be reset automatically if it takes too long to recognizer a gesture.

    \value GestureTriggered The gesture has been triggered and the appropriate
    QGesture object will be delivered to the target as a part of a
    QGestureEvent.

    \value GestureFinished The gesture has been finished successfully and the
    appropriate QGesture object will be delivered to the target as a part of a
    QGestureEvent.

    \value ConsumeEventHint The hint specifies if the gesture framework should
    consume the filtered event and not deliver it to the receiver.

    \omitvalue ResultState_Mask
    \omitvalue ResultHint_Mask

    \sa QGestureRecognizer::filterEvent()
*/

/*!
    Constructs a new gesture recognizer object.
*/
QGestureRecognizer::QGestureRecognizer()
{
}

/*!
    Destroys the gesture recognizer object.
*/
QGestureRecognizer::~QGestureRecognizer()
{
}

/*!
    This function is called by Qt to creates a new QGesture object for the
    given \a target (QWidget or QGraphicsObject).

    Reimplement this function to create a custom QGesture-derived gesture
    object if necessary.
*/
QGesture *QGestureRecognizer::createGesture(QObject *target)
{
    Q_UNUSED(target);
    return new QGesture;
}

/*!
    This function is called by Qt to reset a \a gesture.

    Reimplement this function if a custom QGesture-derived gesture object is
    used which requires resetting additional properties.
*/
void QGestureRecognizer::reset(QGesture *gesture)
{
    if (gesture) {
        QGesturePrivate *d = gesture->d_func();
        d->state = Qt::NoGesture;
        d->hotSpot = QPointF();
        d->targetObject = 0;
    }
}

/*!
    \fn QGestureRecognizer::filterEvent(QGesture *gesture, QObject *watched, QEvent *event)

    This function is called by Qt to filter an \a event for a \a watched object
    (QWidget or QGraphicsObject).

    Returns the result of the current recognition step. The state of the \a
    gesture object is set depending on the result.

    \sa Qt::GestureState
*/

QT_END_NAMESPACE
