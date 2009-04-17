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

#include "qhistorystate.h"
#include "qhistorystate_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QHistoryState

  \brief The QHistoryState class provides a means of returning to a previously active substate.

  \ingroup statemachine

  A history state is a pseudo-state that represents the child state that the
  parent state was in the last time the parent state was exited. A transition
  with a history state as its target is in fact a transition to one of the
  other child states of the parent state. QHistoryState is part of \l{The
  State Machine Framework}.

  Use QState::addHistoryState() to construct a history state. Use the
  setDefaultState() function to set the state that should be entered if the
  parent state has never been entered.  Example:

  \code
  QStateMachine machine;

  QState *s1 = new QState();
  QState *s11 = new QState(s1);
  QState *s12 = new QState(s1);

  QState *s1h = s1->addHistoryState();
  s1h->setDefaultState(s11);

  machine.addState(s1);

  QState *s2 = new QState();
  machine.addState(s2);

  QPushButton *button = new QPushButton();
  // Clicking the button will cause the state machine to enter the child state
  // that s1 was in the last time s1 was exited, or the history state's default
  // state if s1 has never been entered.
  s1->addTransition(button, SIGNAL(clicked()), s1h);
  \endcode
*/

QHistoryStatePrivate::QHistoryStatePrivate()
    : defaultState(0)
{
}

QHistoryState *QHistoryStatePrivate::create(QState::HistoryType type,
                                            QState *parent)
{
    return new QHistoryState(type, parent);
}

QHistoryStatePrivate *QHistoryStatePrivate::get(QHistoryState *q)
{
    return q->d_func();
}

const QHistoryStatePrivate *QHistoryStatePrivate::get(const QHistoryState *q)
{
    return q->d_func();
}

/*!
  \internal

  Constructs a new history state of the given \a type, with the given \a
  parent state.
*/
QHistoryState::QHistoryState(QState::HistoryType type, QState *parent)
    : QAbstractState(*new QHistoryStatePrivate, parent)
{
    Q_D(QHistoryState);
    d->historyType = type;
}

/*!
  Destroys this history state.
*/
QHistoryState::~QHistoryState()
{
}

/*!
  Returns this history state's default state.  The default state indicates the
  state to transition to if the parent state has never been entered before.
*/
QAbstractState *QHistoryState::defaultState() const
{
    Q_D(const QHistoryState);
    return d->defaultState;
}

/*!
  Sets this history state's default state to be the given \a state.
  \a state must be a sibling of this history state.
*/
void QHistoryState::setDefaultState(QAbstractState *state)
{
    Q_D(QHistoryState);
    if (state && state->parentState() != parentState()) {
        qWarning("QHistoryState::setDefaultState: state %p does not belong "
                 "to this history state's group (%p)", state, parentState());
        return;
    }
    d->defaultState = state;
}

/*!
  \reimp
*/
void QHistoryState::onEntry()
{
}

/*!
  \reimp
*/
void QHistoryState::onExit()
{
}

/*!
  \reimp
*/
bool QHistoryState::event(QEvent *e)
{
    return QAbstractState::event(e);
}

QT_END_NAMESPACE
