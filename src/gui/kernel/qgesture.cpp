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
#include "qgraphicsitem.h"

QT_BEGIN_NAMESPACE


class QEventFilterProxyGraphicsItem : public QGraphicsItem
{
public:
    QEventFilterProxyGraphicsItem(QGesture *g)
            : gesture(g)
    {
    }
    bool sceneEventFilter(QGraphicsItem *, QEvent *event)
    {
        return gesture ? gesture->filterEvent(event) : false;
    }
    QRectF boundingRect() const { return QRectF(); }
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) { }

private:
    QGesture *gesture;
};

/*!
    \class QGesture
    \since 4.6

    \brief The QGesture class is the base class for implementing custom
    gestures.

    This class represents both an object that recognizes a gesture out of a set
    of input events (a gesture recognizer), and a gesture object itself that
    can be used to get extended information about the triggered gesture.

    The class has a list of properties that can be queried by the user to get
    some gesture-specific parameters (for example, an offset of a Pan gesture).

    Usually gesture recognizer implements a state machine, storing its state
    internally in the recognizer object. The recognizer receives input events
    through the \l{QGesture::}{filterEvent()} virtual function and decides
    whether the event should change the state of the recognizer by emitting an
    appropriate signal.

    Input events should be either fed to the recognizer one by one with a
    filterEvent() function, or the gesture recognizer should be attached to an
    object it filters events for by specifying it as a parent object. The
    QGesture object installs itself as an event filter to the parent object
    automatically, the unsetObject() function should be used to remove an event
    filter from the parent object. To make a
    gesture that operates on a QGraphicsItem, both the appropriate QGraphicsView
    should be passed as a parent object and setGraphicsItem() functions should
    be used to attach a gesture to a graphics item.

    This is a base class, to create a custom gesture type, you should subclass
    it and implement its pure virtual functions.

    \sa QPanGesture
*/

/*! \fn bool QGesture::filterEvent(QEvent *event)

    Parses input \a event and emits a signal when detects a gesture.

    In your reimplementation of this function, if you want to filter the \a
    event out, i.e. stop it being handled further, return true; otherwise
    return false;

    This is a pure virtual function that needs to be implemented in subclasses.
*/

/*! \fn void QGesture::started()

    The signal is emitted when the gesture is started. Extended information
    about the gesture is contained in the signal sender object.

    In addition to started(), a triggered() signal should also be emitted.
*/

/*! \fn void QGesture::triggered()

    The signal is emitted when the gesture is detected. Extended information
    about the gesture is contained in the signal sender object.
*/

/*! \fn void QGesture::finished()

    The signal is emitted when the gesture is finished. Extended information
    about the gesture is contained in the signal sender object.
*/

/*! \fn void QGesture::cancelled()

    The signal is emitted when the gesture is cancelled, for example the reset()
    function is called while the gesture was in the process of emitting a
    triggered() signal.  Extended information about the gesture is contained in
    the sender object.
*/


/*!
    Creates a new gesture handler object and marks it as a child of \a parent.

    The \a parent object is also the default event source for the gesture,
    meaning that the gesture installs itself as an event filter for the \a
    parent.

    \sa setGraphicsItem()
*/
QGesture::QGesture(QObject *parent)
    : QObject(*new QGesturePrivate, parent)
{
    if (parent)
        parent->installEventFilter(this);
}

/*! \internal
 */
QGesture::QGesture(QGesturePrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
    if (parent)
        parent->installEventFilter(this);
}

/*!
    Destroys the gesture object.
*/
QGesture::~QGesture()
{
}

/*! \internal
 */
bool QGesture::eventFilter(QObject *receiver, QEvent *event)
{
    Q_D(QGesture);
    if (d->graphicsItem && receiver == parent())
        return false;
    return filterEvent(event);
}

/*!
    \property QGesture::state

    \brief The current state of the gesture.
*/

/*!
  Returns the gesture recognition state.
 */
Qt::GestureState QGesture::state() const
{
    return d_func()->state;
}

/*!
  Sets this gesture's recognition state to \a state and emits appropriate
  signals.

  This functions emits the signals according to the old state and the new
  \a state, and it should be called after all the internal properties have been
  initialized.

  \sa started(), triggered(), finished(), cancelled()
 */
void QGesture::updateState(Qt::GestureState state)
{
    Q_D(QGesture);
    if (d->state == state) {
        if (state == Qt::GestureUpdated)
            emit triggered();
        return;
    }
    const Qt::GestureState oldState = d->state;
    d->state = state;
    if (state != Qt::NoGesture && oldState > state) {
        // comparing the state as ints: state should only be changed from
        // started to (optionally) updated and to finished.
        qWarning("QGesture::updateState: incorrect new state");
        return;
    }
    if (oldState == Qt::NoGesture)
        emit started();
    if (state == Qt::GestureUpdated)
        emit triggered();
    else if (state == Qt::GestureFinished)
        emit finished();
    else if (state == Qt::NoGesture)
        emit cancelled();

    if (state == Qt::GestureFinished) {
        // gesture is finished, so we reset the internal state.
        d->state = Qt::NoGesture;
    }
}

/*!
    Sets the \a graphicsItem the gesture is filtering events for.

    The gesture will install an event filter to the \a graphicsItem and
    redirect them to the filterEvent() function.

    \sa graphicsItem()
*/
void QGesture::setGraphicsItem(QGraphicsItem *graphicsItem)
{
    Q_D(QGesture);
    if (d->graphicsItem && d->eventFilterProxyGraphicsItem)
        d->graphicsItem->removeSceneEventFilter(d->eventFilterProxyGraphicsItem);
    d->graphicsItem = graphicsItem;
    if (!d->eventFilterProxyGraphicsItem)
        d->eventFilterProxyGraphicsItem = new QEventFilterProxyGraphicsItem(this);
    if (graphicsItem)
        graphicsItem->installSceneEventFilter(d->eventFilterProxyGraphicsItem);
}

/*!
    Returns the graphics item the gesture is filtering events for.

    \sa setGraphicsItem()
*/
QGraphicsItem* QGesture::graphicsItem() const
{
    return d_func()->graphicsItem;
}

/*! \fn void QGesture::reset()

    Resets the internal state of the gesture. This function might be called by
    the filterEvent() implementation in a derived class, or by the user to
    cancel a gesture.  The base class implementation calls
    updateState(Qt::NoGesture) which emits the cancelled()
    signal if the state() of the gesture indicated it was active.
*/
void QGesture::reset()
{
    updateState(Qt::NoGesture);
}

QT_END_NAMESPACE
