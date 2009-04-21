/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstatefinishedtransition.h"
#include "qstatefinishedevent.h"
#include "qactiontransition_p.h"

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

class QStateFinishedTransitionPrivate : public QActionTransitionPrivate
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
    : QActionTransition(*new QStateFinishedTransitionPrivate, sourceState)
{
}

/*!
  Constructs a new QStateFinishedTransition object associated with the given
  \a state, and that has the given \a targets and \a sourceState.
*/
QStateFinishedTransition::QStateFinishedTransition(
    QState *state, const QList<QAbstractState*> &targets, QState *sourceState)
    : QActionTransition(*new QStateFinishedTransitionPrivate, targets, sourceState)
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
bool QStateFinishedTransition::event(QEvent *e)
{
    return QActionTransition::event(e);
}

QT_END_NAMESPACE
