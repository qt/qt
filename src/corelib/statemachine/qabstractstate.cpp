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

#include "qabstractstate.h"
#include "qabstractstate_p.h"
#include "qstatemachine.h"
#include "qstatemachine_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QAbstractState

  \brief The QAbstractState class is the base class of states of a QStateMachine.

  \ingroup statemachine

  The QAbstractState class is the abstract base class of states that are part
  of a QStateMachine. It defines the interface that all state objects have in
  common. QAbstractState is part of \l{The State Machine Framework}.

  The parentState() function returns the state's parent state.

  \section1 Subclassing

  The onEntry() function is called when the state is entered; reimplement this
  function to perform custom processing when the state is entered.

  The onExit() function is called when the state is exited; reimplement this
  function to perform custom processing when the state is exited.
*/

QAbstractStatePrivate::QAbstractStatePrivate()
{
}

QAbstractStatePrivate *QAbstractStatePrivate::get(QAbstractState *q)
{
    return q->d_func();
}

const QAbstractStatePrivate *QAbstractStatePrivate::get(const QAbstractState *q)
{
    return q->d_func();
}

QStateMachine *QAbstractStatePrivate::machine() const
{
    Q_Q(const QAbstractState);
    QObject *par = q->parent();
    while (par != 0) {
        if (QStateMachine *mach = qobject_cast<QStateMachine*>(par))
            return mach;
        par = par->parent();
    }
    return 0;
}

void QAbstractStatePrivate::callOnEntry()
{
    Q_Q(QAbstractState);
    q->onEntry();
}

void QAbstractStatePrivate::callOnExit()
{
    Q_Q(QAbstractState);
    q->onExit();
}

/*!
  Constructs a new state with the given \a parent state.
*/
QAbstractState::QAbstractState(QState *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QAbstractStatePrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QAbstractStatePrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QAbstractState::QAbstractState(QAbstractStatePrivate &dd, QState *parent)
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
  Destroys this state.
*/
QAbstractState::~QAbstractState()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

/*!
  Returns this state's parent state, or 0 if the state has no parent state.
*/
QState *QAbstractState::parentState() const
{
    return qobject_cast<QState*>(parent());
}

/*!
  \fn QAbstractState::onExit()

  This function is called when the state is exited.  Reimplement this function
  to perform custom processing when the state is exited.
*/

/*!
  \fn QAbstractState::onEntry()

  This function is called when the state is entered. Reimplement this function
  to perform custom processing when the state is entered.
*/

/*!
  \reimp
*/
bool QAbstractState::event(QEvent *e)
{
    return QObject::event(e);
}

QT_END_NAMESPACE
