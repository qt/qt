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

#include "qactionstate.h"
#include "qactionstate_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QActionState

  \brief The QActionState class provides an action-based state.

  \since 4.6
  \ingroup statemachine

  QActionState executes \l{QStateAction}{state actions} when the state is
  entered and exited. QActionState is part of \l{The State Machine Framework}.

  You can add actions to a state with the addEntryAction() and addExitAction()
  functions. The state executes the actions when the state is entered and
  exited, respectively.

  The invokeMethodOnEntry() and invokeMethodOnExit() functions are used for
  defining method invocations that should be performed when a state is entered
  and exited, respectively.

  \code
  QState *s2 = new QState();
  s2->invokeMethodOnEntry(&label, "showMaximized");
  machine.addState(s2);
  \endcode

  \sa QStateAction
*/

QActionStatePrivate::QActionStatePrivate()
{
}

QActionStatePrivate::~QActionStatePrivate()
{
}

QActionStatePrivate *QActionStatePrivate::get(QActionState *q)
{
    return q->d_func();
}

const QActionStatePrivate *QActionStatePrivate::get(const QActionState *q)
{
    return q->d_func();
}

QList<QStateAction*> QActionStatePrivate::entryActions() const
{
    QList<QStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QStateAction *act = qobject_cast<QStateAction*>(*it);
        if (act && (QStateActionPrivate::get(act)->when == QStateActionPrivate::ExecuteOnEntry))
            result.append(act);
    }
    return result;
}

QList<QStateAction*> QActionStatePrivate::exitActions() const
{
    QList<QStateAction*> result;
    QList<QObject*>::const_iterator it;
#ifdef QT_STATEMACHINE_SOLUTION
    const QObjectList &children = q_func()->children();
#endif
    for (it = children.constBegin(); it != children.constEnd(); ++it) {
        QStateAction *act = qobject_cast<QStateAction*>(*it);
        if (act && (QStateActionPrivate::get(act)->when == QStateActionPrivate::ExecuteOnExit))
            result.append(act);
    }
    return result;
}

/*!
  Constructs a new action state with the given \a parent state.
*/
QActionState::QActionState(QState *parent)
    : QAbstractState(*new QActionStatePrivate, parent)
{
}

/*!
  \internal
*/
QActionState::QActionState(QActionStatePrivate &dd,
                           QState *parent)
    : QAbstractState(dd, parent)
{
}

/*!
  Destroys this action state.
*/
QActionState::~QActionState()
{
}

/*!
  Instructs this state to invoke the given \a method of the given \a object
  with the given \a arguments when the state is entered. This function will
  create a QStateInvokeMethodAction object and add it to the entry actions of
  the state.

  \sa invokeMethodOnExit(), addEntryAction()
*/
void QActionState::invokeMethodOnEntry(QObject *object, const char *method,
                                       const QList<QVariant> &arguments)
{
    addEntryAction(new QStateInvokeMethodAction(object, method, arguments));
}

/*!
  Instructs this state to invoke the given \a method of the given \a object
  with the given \a arguments when the state is exited. This function will
  create a QStateInvokeMethodAction object and add it to the exit actions of
  the state.

  \sa invokeMethodOnEntry(), addExitAction()
*/
void QActionState::invokeMethodOnExit(QObject *object, const char *method,
                                      const QList<QVariant> &arguments)
{
    addExitAction(new QStateInvokeMethodAction(object, method, arguments));
}

/*!
  Adds the given \a action to this state. The action will be executed when
  this state is entered. The state takes ownership of the action.

  \sa addExitAction(), removeEntryAction()
*/
void QActionState::addEntryAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionState::addEntryAction: cannot add null action");
        return;
    }
    action->setParent(this);
    QStateActionPrivate::get(action)->when = QStateActionPrivate::ExecuteOnEntry;
}

/*!
  Adds the given \a action to this state. The action will be executed when
  this state is exited. The state takes ownership of the action.

  \sa addEntryAction(), removeExitAction()
*/
void QActionState::addExitAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionState::addExitAction: cannot add null action");
        return;
    }
    action->setParent(this);
    QStateActionPrivate::get(action)->when = QStateActionPrivate::ExecuteOnExit;
}

/*!
  Removes the given entry \a action from this state. The state releases
  ownership of the action.

  \sa addEntryAction()
*/
void QActionState::removeEntryAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionState::removeEntryAction: cannot remove null action");
        return;
    }
    if (action->parent() == this)
        action->setParent(0);
}

/*!
  Removes the given exit \a action from this state. The state releases
  ownership of the action.

  \sa addExitAction()
*/
void QActionState::removeExitAction(QStateAction *action)
{
    if (!action) {
        qWarning("QActionState::removeExitAction: cannot remove null action");
        return;
    }
    if (action->parent() == this)
        action->setParent(0);
}

/*!
  Returns this state's entry actions.

  \sa addEntryAction(), exitActions()
*/
QList<QStateAction*> QActionState::entryActions() const
{
    Q_D(const QActionState);
    return d->entryActions();
}

/*!
  Returns this state's exit actions.

  \sa addExitAction(), entryActions()
*/
QList<QStateAction*> QActionState::exitActions() const
{
    Q_D(const QActionState);
    return d->exitActions();
}

/*!
  \reimp
*/
void QActionState::onEntry()
{
    Q_D(QActionState);
    QList<QStateAction*> actions = d->entryActions();
    for (int i = 0; i < actions.size(); ++i)
        QStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
void QActionState::onExit()
{
    Q_D(QActionState);
    QList<QStateAction*> actions = d->exitActions();
    for (int i = 0; i < actions.size(); ++i)
        QStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
bool QActionState::event(QEvent *e)
{
    return QAbstractState::event(e);
}

QT_END_NAMESPACE
