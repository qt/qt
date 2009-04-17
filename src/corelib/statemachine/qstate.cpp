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

#include "qstate.h"
#include "qstate_p.h"
#include "qhistorystate.h"
#include "qhistorystate_p.h"
#include "qabstracttransition.h"
#include "qabstracttransition_p.h"
#include "qsignaltransition.h"
#include "qstatefinishedtransition.h"
#include "qstatemachine.h"
#include "qstatemachine_p.h"
#ifndef QT_NO_ANIMATION
#include "qanimationstate.h"
#endif

QT_BEGIN_NAMESPACE

/*!
  \class QState

  \brief The QState class provides a general-purpose state for QStateMachine.

  \ingroup statemachine

  QState objects can have child states, and can have transitions to other
  states. QState is part of \l{The State Machine Framework}.

  The addTransition() function adds a transition. The removeTransition()
  function removes a transition.

  \section1 States with Child States

  For non-parallel state groups, the setInitialState() function must be called
  to set the initial state. The child states are mutually exclusive states,
  and the state machine needs to know which child state to enter when the
  parent state is the target of a transition.

  The addHistoryState() function adds a history state.

  The setErrorState() sets the state's error state. The error state is the
  state that the state machine will transition to if an error is detected when
  attempting to enter the state (e.g. because no initial state has been set).
*/

/*!
  \enum QState::Type

  This enum specifies the type of a state.

  \value Normal A normal state. If the state has no child states, it is an
  atomic state; otherwise, the child states are mutually exclusive and an
  initial state must be set by calling QState::setInitialState().

  \value ParallelGroup The state is a parallel group state. When a parallel
  group state is entered, all its child states are entered in parallel.
*/

/*!
  \enum QState::HistoryType

  This enum specifies the type of history that a QHistoryState records.

  \value ShallowHistory Only the immediate child states of the parent state
  are recorded. In this case a transition with the history state as its
  target will end up in the immediate child state that the parent was in the
  last time it was exited. This is the default.

  \value DeepHistory Nested states are recorded. In this case a transition
  with the history state as its target will end up in the most deeply nested
  descendant state the parent was in the last time it was exited.
*/

QStatePrivate::QStatePrivate()
    : errorState(0), isParallelGroup(false), initialState(0)
{
}

QStatePrivate::~QStatePrivate()
{
}

QStatePrivate *QStatePrivate::get(QState *q)
{
    if (!q)
        return 0;
    return q->d_func();
}

const QStatePrivate *QStatePrivate::get(const QState *q)
{
    if (!q)
        return 0;
    return q->d_func();
}

/*!
  Constructs a new state with the given \a parent state.
*/
QState::QState(QState *parent)
    : QActionState(*new QStatePrivate, parent)
{
}

/*!
  Constructs a new state of the given \a type with the given \a parent state.
*/
QState::QState(Type type, QState *parent)
    : QActionState(*new QStatePrivate, parent)
{
    Q_D(QState);
    d->isParallelGroup = (type == ParallelGroup);
}

/*!
  \internal
*/
QState::QState(QStatePrivate &dd, QState *parent)
    : QActionState(dd, parent)
{
}

/*!
  Destroys this state.
*/
QState::~QState()
{
}

QList<QAbstractState*> QStatePrivate::childStates() const
{
    QList<QAbstractState*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QAbstractState *s = qobject_cast<QAbstractState*>(*it);
        if (!s || qobject_cast<QHistoryState*>(s))
            continue;
        result.append(s);
    }
    return result;
}

QList<QHistoryState*> QStatePrivate::historyStates() const
{
    QList<QHistoryState*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QHistoryState *h = qobject_cast<QHistoryState*>(*it);
        if (h)
            result.append(h);
    }
    return result;
}

QList<QAbstractTransition*> QStatePrivate::transitions() const
{
    QList<QAbstractTransition*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QAbstractTransition *t = qobject_cast<QAbstractTransition*>(*it);
        if (t)
            result.append(t);
    }
    return result;
}

/*!
  Returns this state group's error state. 

  \sa QStateMachine::errorState(), QStateMachine::setErrorState()
*/
QAbstractState *QState::errorState() const
{
    Q_D(const QState);
    return d->errorState;
}

/*!
  Sets this state's error state to be the given \a state. If the error state
  is not set, or if it is set to 0, the state will inherit its parent's error
  state recursively.

  \sa QStateMachine::setErrorState(), QStateMachine::errorState()
*/
void QState::setErrorState(QAbstractState *state)
{
    Q_D(QState);
    if (state != 0 && QAbstractStatePrivate::get(state)->machine() != d->machine()) {
        qWarning("QState::setErrorState: error state cannot belong "
                 "to a different state machine");
        return;
    }

    d->errorState = state;
}

/*!
  Adds the given \a transition. The transition has this state as the source.
  This state takes ownership of the transition.
*/
void QState::addTransition(QAbstractTransition *transition)
{
    Q_D(QState);
    if (!transition) {
        qWarning("QState::addTransition: cannot add null transition");
        return;
    }
    const QList<QAbstractState*> &targets = QAbstractTransitionPrivate::get(transition)->targetStates;
    for (int i = 0; i < targets.size(); ++i) {
        QAbstractState *t = targets.at(i);
        if (!t) {
            qWarning("QState::addTransition: cannot add transition to null state");
            return;
        }
        if ((QAbstractStatePrivate::get(t)->machine() != d->machine())
            && QAbstractStatePrivate::get(t)->machine() && d->machine()) {
            qWarning("QState::addTransition: cannot add transition "
                     "to a state in a different state machine");
            return;
        }
    }
    transition->setParent(this);
}

/*!
  Adds a transition associated with the given \a signal of the given \a sender
  object. The transition has this state as the source, and the given \a target
  as the target state.
*/
void QState::addTransition(QObject *sender, const char *signal,
                           QAbstractState *target)
{
    if (!sender) {
        qWarning("QState::addTransition: sender cannot be null");
        return;
    }
    if (!signal) {
        qWarning("QState::addTransition: signal cannot be null");
        return;
    }
    addTransition(new QSignalTransition(sender, signal, QList<QAbstractState*>() << target));
}

/*!
  Adds a transition that's triggered by the finished event of this state, and
  that has the given \a target state.

  \sa QStateFinishedEvent
*/
void QState::addFinishedTransition(QAbstractState *target)
{
    addTransition(new QStateFinishedTransition(this, QList<QAbstractState*>() << target));
}

namespace {

class UnconditionalTransition : public QAbstractTransition
{
public:
    UnconditionalTransition(QAbstractState *target)
        : QAbstractTransition(QList<QAbstractState*>() << target) {}
protected:
    void onTransition() {}
    bool eventTest(QEvent *) const { return true; }
};

} // namespace

/*!
  Adds an unconditional transition from this state to the given \a target
  state.
*/
void QState::addTransition(QAbstractState *target)
{
    addTransition(new UnconditionalTransition(target));
}

/*!
  Removes the given \a transition from this state.  The state releases
  ownership of the transition.

  \sa addTransition()
*/
void QState::removeTransition(QAbstractTransition *transition)
{
    Q_D(QState);
    if (!transition) {
        qWarning("QState::removeTransition: cannot remove null transition");
        return;
    }
    if (transition->sourceState() != this) {
        qWarning("QState::removeTransition: transition %p's source state (%p)"
                 " is different from this state (%p)",
                 transition, transition->sourceState(), this);
        return;
    }
    QStateMachinePrivate *mach = QStateMachinePrivate::get(d->machine());
    if (mach)
        mach->unregisterTransition(transition);
    transition->setParent(0);
}

/*!
  Returns the list of transitions from this state, or an empty list if there
  are no transitions from this state.

  \sa addTransition(), removeTransition()
*/
QList<QAbstractTransition*> QState::transitions() const
{
    Q_D(const QState);
    return d->transitions();
}

/*!
  Creates a history state of the given \a type for this state and returns the
  new state.  The history state becomes a child of this state.
*/
QHistoryState *QState::addHistoryState(HistoryType type)
{
    return QHistoryStatePrivate::create(type, this);
}

/*!
  \reimp
*/
void QState::onEntry()
{
    QActionState::onEntry();
}

/*!
  \reimp
*/
void QState::onExit()
{
    QActionState::onExit();
}

#ifndef QT_NO_ANIMATION

/*!
  \overload addAnimatedTransition()

  Adds an animated transition from the current state to \a targetState for \a animation. 

  This function creates a QSignalTransition for the \a sender and \a signal, and calls 
  addAnimatedTransition() with this transition object.
*/
QAnimationState *QState::addAnimatedTransition(QObject *sender, const char *signal,
                                               QAbstractState *targetState,
                                               QAbstractAnimation *animation)
{
    if (!targetState) {
        qWarning("QState::addAnimatedTransition: cannot add transition to null state");
        return 0;
    }
    return addAnimatedTransition(
        new QSignalTransition(sender, signal,
                              QList<QAbstractState*>() << targetState), animation);
}

/*!
  Adds an animated transition from the current state.
  
  The animated transition has an intermediate QAnimationState which plays \a
  animation before entering the target state(s). This QAnimationState will be
  entered when \a transition is taken by the state machine. When the animation
  has finished playing, the transition's target state(s) will be entered.

  The new QAnimationState object will become a child of this state's parent state.

  \code
    QPushButton button;
    QPropertyAnimation animation(&button, "geometry");
    animation.setEndValue(QRect(100, 100, 400, 400));

    QStateMachine machine;

    QState *s1 = new QState();
    QState *s2 = new QState();

    QTransition *transition = new QTransition(MyEventType);
    s1->addAnimatedTransition(transition, s2, &animation);
  \endcode

  The function returns the new QAnimationState. This state can be used if you want to add additional 
  transitions into or out from the animation state, and if you want to add additional animations.

  \sa QAnimationState
*/
QAnimationState *QState::addAnimatedTransition(QAbstractTransition *transition,
                                               QAbstractAnimation *animation)
{
    if (!transition) {
        qWarning("QState::addAnimatedTransition: cannot add null transition");
        return 0;
    }
    QList<QAbstractState*> targets = transition->targetStates();
    Q_ASSERT(!targets.isEmpty());
    if (!targets.at(0)->parentState()) {
        qWarning("QState::addAnimatedTransition: cannot add transition to target that doesn't have a parent state");
        return 0;
    }
    QAnimationState *animState = new QAnimationState(animation, targets.at(0)->parentState());
    animState->addTransition(new QStateFinishedTransition(animState, targets));
    transition->setTargetStates(QList<QAbstractState*>() << animState);
    addTransition(transition);
    return animState;
}

#endif

/*!
  Returns this state's initial state, or 0 if the state has no initial state.
*/
QAbstractState *QState::initialState() const
{
    Q_D(const QState);
    return d->initialState;
}

/*!
  Sets this state's initial state to be the given \a state.
  \a state has to be a child of this state.
*/
void QState::setInitialState(QAbstractState *state)
{
    Q_D(QState);
    if (d->isParallelGroup) {
        qWarning("QState::setInitialState: ignoring attempt to set initial state "
                 "of parallel state group %p", this);
        return;
    }
    if (state && (state->parentState() != this)) {
        qWarning("QState::setInitialState: state %p is not a child of this state (%p)",
                 state, this);
        return;
    }
    d->initialState = state;
}

/*!
  \reimp
*/
bool QState::event(QEvent *e)
{
    return QActionState::event(e);
}

QT_END_NAMESPACE
