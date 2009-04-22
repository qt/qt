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

#include "qactiontransition.h"
#include "qactiontransition_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QActionTransition

  \brief The QActionTransition class provides an action-based transition.

  \since 4.6
  \ingroup statemachine

  QActionTransition provides an action-based transition; you add actions with
  the addAction() function. The transition executes the actions when the
  transition is triggered. QActionTransition is part of \l{The State Machine
  Framework}.

  The invokeMethodOnTransition() function is used for defining method
  invocations that should be performed when a transition is taken.

  \code
  QStateMachine machine;
  QState *s1 = new QState();
  machine.addState(s1);
  QActionTransition *t1 = new QActionTransition();
  QLabel label;
  t1->invokeMethodOnTransition(&label, "clear");
  QState *s2 = new QState();
  machine.addState(s2);
  t1->setTargetState(s2);
  s1->addTransition(t1);
  \endcode

  Actions are executed in the order in which they were added.

  \sa QState::addTransition(), QStateAction
*/

QActionTransitionPrivate::QActionTransitionPrivate()
{
}

QActionTransitionPrivate::~QActionTransitionPrivate()
{
}

QActionTransitionPrivate *QActionTransitionPrivate::get(QActionTransition *q)
{
    return q->d_func();
}

const QActionTransitionPrivate *QActionTransitionPrivate::get(const QActionTransition *q)
{
    return q->d_func();
}

QList<QStateAction*> QActionTransitionPrivate::actions() const
{
    QList<QStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QStateAction *s = qobject_cast<QStateAction*>(*it);
        if (s)
            result.append(s);
    }
    return result;
}

/*!
  Constructs a new QActionTransition object with the given \a sourceState.
*/
QActionTransition::QActionTransition(QState *sourceState)
    : QAbstractTransition(*new QActionTransitionPrivate, sourceState)
{
}

/*!
  Constructs a new QActionTransition object with the given \a targets and \a
  sourceState.
*/
QActionTransition::QActionTransition(const QList<QAbstractState*> &targets, QState *sourceState)
    : QAbstractTransition(*new QActionTransitionPrivate, targets, sourceState)
{
}

/*!
  \internal
*/
QActionTransition::QActionTransition(QActionTransitionPrivate &dd, QState *parent)
    : QAbstractTransition(dd, parent)
{
}

/*!
  \internal
*/
QActionTransition::QActionTransition(QActionTransitionPrivate &dd, const QList<QAbstractState*> &targets, QState *parent)
    : QAbstractTransition(dd, targets, parent)
{
}

/*!
  Destroys this transition.
*/
QActionTransition::~QActionTransition()
{
}

/*!
  Instructs this QActionTransition to invoke the given \a method of the given \a
  object with the given \a arguments when the transition is taken. This
  function will create a QStateInvokeMethodAction object and add it to the
  actions of the transition.
*/
void QActionTransition::invokeMethodOnTransition(QObject *object, const char *method,
                                           const QList<QVariant> &arguments)
{
    addAction(new QStateInvokeMethodAction(object, method, arguments));
}

/*!
  Adds the given \a action to this transition.
  The action will be executed when the transition is triggered.
  The transition takes ownership of the action.

  \sa removeAction()
*/
void QActionTransition::addAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionTransition::addAction: cannot add null action");
        return;
    }
    action->setParent(this);
}

/*!
  Removes the given \a action from this transition.
  The transition releases ownership of the action.

  \sa addAction()
*/
void QActionTransition::removeAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionTransition::removeAction: cannot remove null action");
        return;
    }
    action->setParent(0);
}

/*!
  Returns this transitions's actions, or an empty list if the transition has
  no actions.

  \sa addAction()
*/
QList<QStateAction*> QActionTransition::actions() const
{
    Q_D(const QActionTransition);
    return d->actions();
}

/*!
  \reimp
*/
void QActionTransition::onTransition()
{
    Q_D(QActionTransition);
    QList<QStateAction*> actions = d->actions();
    for (int i = 0; i < actions.size(); ++i)
        QStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
bool QActionTransition::event(QEvent *e)
{
    return QAbstractTransition::event(e);
}

QT_END_NAMESPACE
