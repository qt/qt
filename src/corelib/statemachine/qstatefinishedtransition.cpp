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

#include "qstatefinishedtransition.h"
#include "qstatefinishedevent.h"
#include "qtransition_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QStateFinishedTransition

  \brief The QStateFinishedTransition class provides a transition that triggers when a state is finished.

  \ingroup statemachine

  A state is finished when one of its final child states (a QFinalState) is
  entered; this will cause a QStateFinishedEvent to be generated. The
  QStateFinishedTransition class provides a way of associating a transition
  with such an event. QStateFinishedTransition is part of \l{The State Machine
  Framework}.

  \code
    QStateMachine machine;
    QState *s1 = new QState(machine.rootState());
    QState *s11 = new QState(s1);
    QFinalState *s12 = new QFinalState(s1);
    s11->addTransition(s12);

    QState *s2 = new QState(machine.rootState());
    QStateFinishedTransition *finishedTransition = new QStateFinishedTransition(s1);
    finishedTransition->setTargetState(s2);
    s1->addTransition(finishedTransition);
  \endcode

  \sa QState::addFinishedTransition(), QStateFinishedEvent
*/

/*!
    \property QStateFinishedTransition::state

    \brief the state whose QStateFinishedEvent this transition is associated with
*/

class QStateFinishedTransitionPrivate : public QTransitionPrivate
{
    Q_DECLARE_PUBLIC(QStateFinishedTransition)
public:
    QStateFinishedTransitionPrivate();

    static QStateFinishedTransitionPrivate *get(QStateFinishedTransition *q);

    QState *state;
};

QStateFinishedTransitionPrivate::QStateFinishedTransitionPrivate()
{
    state = 0;
}

QStateFinishedTransitionPrivate *QStateFinishedTransitionPrivate::get(QStateFinishedTransition *q)
{
    return q->d_func();
}

/*!
  Constructs a new QStateFinishedTransition object that has the given \a
  sourceState.
*/
QStateFinishedTransition::QStateFinishedTransition(QState *sourceState)
    : QTransition(*new QStateFinishedTransitionPrivate, sourceState)
{
}

/*!
  Constructs a new QStateFinishedTransition object associated with the given
  \a state, and that has the given \a targets and \a sourceState.
*/
QStateFinishedTransition::QStateFinishedTransition(
    QState *state, const QList<QAbstractState*> &targets, QState *sourceState)
    : QTransition(*new QStateFinishedTransitionPrivate, targets, sourceState)
{
    Q_D(QStateFinishedTransition);
    d->state = state;
}

/*!
  Destroys this QStateFinishedTransition.
*/
QStateFinishedTransition::~QStateFinishedTransition()
{
}

/*!
  Returns the state associated with this QStateFinishedTransition.
*/
QState *QStateFinishedTransition::state() const
{
    Q_D(const QStateFinishedTransition);
    return d->state;
}

/*!
  Sets the \a state associated with this QStateFinishedTransition.
*/
void QStateFinishedTransition::setState(QState *state)
{
    Q_D(QStateFinishedTransition);
    d->state = state;
}

/*!
  \reimp
*/
bool QStateFinishedTransition::eventTest(QEvent *event) const
{
    Q_D(const QStateFinishedTransition);
#ifndef QT_STATEMACHINE_SOLUTION
    if (event->type() == QEvent::StateFinished) {
#else
    if (event->type() == QEvent::Type(QEvent::User-2)) {
#endif
        QStateFinishedEvent *sfe = static_cast<QStateFinishedEvent*>(event);
        return (sfe->state() == d->state);
    }
    return false;
}

/*!
  \reimp
*/
void QStateFinishedTransition::onTransition()
{
}

/*!
  \reimp
*/
bool QStateFinishedTransition::event(QEvent *e)
{
    return QTransition::event(e);
}

QT_END_NAMESPACE
