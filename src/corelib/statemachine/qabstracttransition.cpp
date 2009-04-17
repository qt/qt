/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qabstracttransition.h"
#include "qabstracttransition_p.h"
#include "qabstractstate.h"
#include "qstate.h"
#include "qstatemachine.h"

QT_BEGIN_NAMESPACE

/*!
  \class QAbstractTransition

  \brief The QAbstractTransition class is the base class of transitions between QAbstractState objects.

  \ingroup statemachine

  The QAbstractTransition class is the abstract base class of transitions
  between states (QAbstractState objects) of a
  QStateMachine. QAbstractTransition is part of \l{The State Machine
  Framework}.

  The QTransition class provides a default (action-based) implementation of
  the QAbstractTransition interface.

  The sourceState() function returns the source of the transition. The
  targetStates() function returns the targets of the transition.

  \section1 Subclassing

  The eventTest() function is called by the state machine to determine whether
  an event should trigger the transition. In your reimplementation you
  typically check the event type and cast the event object to the proper type,
  and check that one or more properties of the event meet your criteria.

  The onTransition() function is called when the transition is triggered;
  reimplement this function to perform custom processing for the transition.
*/

/*!
    \property QAbstractTransition::source

    \brief the source state (parent) of this transition
*/

/*!
    \property QAbstractTransition::target

    \brief the target state of this transition
*/

/*!
    \property QAbstractTransition::targets

    \brief the target states of this transition

    If multiple states are specified, all must be descendants of the same
    parallel group state.
*/

QAbstractTransitionPrivate::QAbstractTransitionPrivate()
{
}

QAbstractTransitionPrivate *QAbstractTransitionPrivate::get(QAbstractTransition *q)
{
    return q->d_func();
}

const QAbstractTransitionPrivate *QAbstractTransitionPrivate::get(const QAbstractTransition *q)
{
    return q->d_func();
}

QStateMachine *QAbstractTransitionPrivate::machine() const
{
    Q_Q(const QAbstractTransition);
    QObject *par = q->parent();
    while (par != 0) {
        if (QStateMachine *mach = qobject_cast<QStateMachine*>(par))
            return mach;
        par = par->parent();
    }
    return 0;
}

bool QAbstractTransitionPrivate::callEventTest(QEvent *e) const
{
    Q_Q(const QAbstractTransition);
    return q->eventTest(e);
}

void QAbstractTransitionPrivate::callOnTransition()
{
    Q_Q(QAbstractTransition);
    q->onTransition();
}

QState *QAbstractTransitionPrivate::sourceState() const
{
    Q_Q(const QAbstractTransition);
    return qobject_cast<QState*>(q->parent());
}

/*!
  Constructs a new QAbstractTransition object with the given \a sourceState.
*/
QAbstractTransition::QAbstractTransition(QState *sourceState)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QAbstractTransitionPrivate,
#endif
        sourceState)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QAbstractTransitionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  Constructs a new QAbstractTransition object with the given \a targets and \a
  sourceState.
*/
QAbstractTransition::QAbstractTransition(const QList<QAbstractState*> &targets,
                                         QState *sourceState)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QAbstractTransitionPrivate,
#endif
        sourceState)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QAbstractTransitionPrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QAbstractTransition);
    d->targetStates = targets;
}

/*!
  \internal
*/
QAbstractTransition::QAbstractTransition(QAbstractTransitionPrivate &dd,
                                         QState *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QAbstractTransition::QAbstractTransition(QAbstractTransitionPrivate &dd,
                                         const QList<QAbstractState*> &targets,
                                         QState *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
    Q_D(QAbstractTransition);
    d->targetStates = targets;
}

/*!
  Destroys this transition.
*/
QAbstractTransition::~QAbstractTransition()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  Returns the source state of this transition, or 0 if this transition has no
  source state.
*/
QState *QAbstractTransition::sourceState() const
{
    Q_D(const QAbstractTransition);
    return d->sourceState();
}

/*!
  Returns the target state of this transition, or 0 if the transition has no
  target.
*/
QAbstractState *QAbstractTransition::targetState() const
{
    Q_D(const QAbstractTransition);
    if (d->targetStates.isEmpty())
        return 0;
    return d->targetStates.first();
}

/*!
  Sets the \a target state of this transition.
*/
void QAbstractTransition::setTargetState(QAbstractState* target)
{
    Q_D(QAbstractTransition);
    if (!target)
        d->targetStates.clear();
    else
        d->targetStates = QList<QAbstractState*>() << target;
}

/*!
  Returns the target states of this transition, or an empty list if this
  transition has no target states.
*/
QList<QAbstractState*> QAbstractTransition::targetStates() const
{
    Q_D(const QAbstractTransition);
    return d->targetStates;
}

/*!
  Sets the target states of this transition to be the given \a targets.
*/
void QAbstractTransition::setTargetStates(const QList<QAbstractState*> &targets)
{
    Q_D(QAbstractTransition);
    d->targetStates = targets;
}

/*!
  \fn QAbstractTransition::eventTest(QEvent *event) const

  This function is called to determine whether the given \a event should cause
  this transition to trigger. Reimplement this function and return true if the
  event should trigger the transition, otherwise return false.
*/

/*!
  \fn QAbstractTransition::onTransition()

  This function is called when the transition is triggered.  Reimplement this
  function to perform custom processing when the transition is triggered.
*/

/*!
  \reimp
*/
bool QAbstractTransition::event(QEvent *e)
{
    return QObject::event(e);
}

QT_END_NAMESPACE
