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

#include "qfinalstate.h"
#include "qactionstate_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QFinalState

  \brief The QFinalState class provides a final state.

  \ingroup statemachine

  A final state is used to communicate that (part of) a QStateMachine has
  finished its work. When a final top-level state is entered, the state
  machine's \l{QStateMachine::finished()}{finished}() signal is emitted. In
  general, when a final substate (a child of a QState) is entered, a
  QStateFinishedEvent is generated for the final state's parent
  state. QFinalState is part of \l{The State Machine Framework}.

  To use a final state, you create a QFinalState object and add a transition
  to it from another state. Example:

  \code
  QPushButton button;

  QStateMachine machine;
  QState *s1 = new QState();
  QFinalState *s2 = new QFinalState();
  s1->addTransition(&button, SIGNAL(clicked()), s2);
  machine.addState(s1);
  machine.addState(s2);

  QObject::connect(&machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
  machine.setInitialState(s1);
  machine.start();
  \endcode

  \sa QStateFinishedTransition
*/

class QFinalStatePrivate : public QActionStatePrivate
{
    Q_DECLARE_PUBLIC(QFinalState)

public:
    QFinalStatePrivate();
};

QFinalStatePrivate::QFinalStatePrivate()
{
}

/*!
  Constructs a new QFinalState object with the given \a parent state.
*/
QFinalState::QFinalState(QState *parent)
    : QActionState(*new QFinalStatePrivate, parent)
{
}

/*!
  Destroys this final state.
*/
QFinalState::~QFinalState()
{
}

/*!
  \reimp
*/
void QFinalState::onEntry()
{
    QActionState::onEntry();
}

/*!
  \reimp
*/
void QFinalState::onExit()
{
    QActionState::onExit();
}

/*!
  \reimp
*/
bool QFinalState::event(QEvent *e)
{
    return QActionState::event(e);
}

QT_END_NAMESPACE
