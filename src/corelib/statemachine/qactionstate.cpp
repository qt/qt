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

#include "qactionstate.h"
#include "qactionstate_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QActionState

  \brief The QActionState class provides an action-based state.

  \ingroup statemachine

  QActionState executes \l{QStateAction}{state actions} when the state is
  entered and exited. QActionState is part of \l{The State Machine Framework}.

  You can add actions to a state with the addEntryAction() and addExitAction()
  functions. The state executes the actions when the state is entered and
  exited, respectively.

  Built-in actions are provided for setting properties and invoking methods of
  QObjects. The setPropertyOnEntry() and setPropertyOnExit() functions are
  used for defining property assignments that should be performed when a state
  is entered and exited, respectively.

  \code
  QLabel label;
  QStateMachine machine;
  QState *s1 = new QState();
  s1->setPropertyOnEntry(&label, "text", "Entered state s1");
  machine.addState(s1);
  \endcode

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

/*!
   \enum QActionState::RestorePolicy

   This enum specifies the restore policy type for a state. The restore policy takes effect when
   the machine enters a state which has entry actions of the type QStateSetPropertyAction. If the 
   restore policy of the state is set to RestoreProperties, the state machine will save the 
   value of the property before the QStateSetPropertyAction is executed. 

   Later, when the machine either enters a state which has its restore policy set to 
   DoNotRestoreProperties or when it enters a state which does not set a value for the given 
   property, the property will automatically be restored to its initial value. The state machine
   will only detect which properties are being set if they are being set using a
   QStateSetPropertyAction object set as entry action on a state.

   Special rules apply when using QAnimationState. If a QAnimationState registers that a property
   should be restored before entering the target state of its QStateFinishedTransition, it will
   restore this property using a QPropertyAnimation. 
   
   Only one initial value will be saved for any given property. If a value for a property has 
   already been saved by the state machine, it will not be overwritten until the property has been
   successfully restored. Once the property has been restored, the state machine will clear the 
   initial value until it enters a new state which sets the property and which has RestoreProperties
   as its restore policy.

   \value GlobalRestorePolicy The restore policy for the state should be retrieved using 
          QStateMachine::globalRestorePolicy()
   \value DoNotRestoreProperties The state machine should not save the initial values of properties 
          set in the state and restore them later.
   \value RestoreProperties The state machine should save the initial values of properties 
          set in the state and restore them later.


   \sa setRestorePolicy(), restorePolicy(), addEntryAction(), setPropertyOnEntry()
*/

QActionStatePrivate::QActionStatePrivate()
    : restorePolicy(QActionState::GlobalRestorePolicy)
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
  Instructs this state to set the property with the given \a name of the given
  \a object to the given \a value when the state is entered. This function
  will create a QStateSetPropertyAction object and add it to the entry actions
  of the state. If there is already an existing action associated with the
  property, the value of that action is updated.

  \sa setPropertyOnExit(), invokeMethodOnEntry(), addEntryAction()
*/
void QActionState::setPropertyOnEntry(QObject *object, const char *name,
                                      const QVariant &value)
{
    Q_D(QActionState);
    QList<QStateAction*> actions = d->entryActions();
    for (int i=0; i<actions.size(); ++i) {
        QStateAction *action = actions.at(i);           
        if (QStateSetPropertyAction *spa = qobject_cast<QStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }

    addEntryAction(new QStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this state to set the property with the given \a name of the given
  \a object to the given \a value when the state is exited. This function will
  create a QStateSetPropertyAction object and add it to the exit actions of
  the state. If there is already an existing action associated with the
  property, the value of that action is updated.

  \sa setPropertyOnEntry(), invokeMethodOnExit(), addExitAction()
*/
void QActionState::setPropertyOnExit(QObject *object, const char *name,
                                     const QVariant &value)
{
    Q_D(QActionState);
    QList<QStateAction*> actions = d->exitActions();
    for (int i=0; i<actions.size(); ++i) {
        QStateAction *action = actions.at(i);
        if (QStateSetPropertyAction *spa = qobject_cast<QStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }

    addExitAction(new QStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this state to invoke the given \a method of the given \a object
  with the given \a arguments when the state is entered. This function will
  create a QStateInvokeMethodAction object and add it to the entry actions of
  the state.

  \sa invokeMethodOnExit(), setPropertyOnEntry(), addEntryAction()
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

  \sa invokeMethodOnEntry(), setPropertyOnExit(), addExitAction()
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
  Sets the restore policy of this state to \a restorePolicy. 
  
  The default restore policy is QActionState::GlobalRestorePolicy.
*/
void QActionState::setRestorePolicy(RestorePolicy restorePolicy)
{
    Q_D(QActionState);
    d->restorePolicy = restorePolicy;
}

/*!
  Returns the restore policy for this state.
*/
QActionState::RestorePolicy QActionState::restorePolicy() const
{
    Q_D(const QActionState);
    return d->restorePolicy;
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
