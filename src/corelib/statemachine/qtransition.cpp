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

#include "qtransition.h"
#include "qtransition_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QTransition

  \brief The QTransition class provides an action-based transition.

  \ingroup statemachine

  QTransition provides an action-based transition; you add actions with the
  addAction() function. The transition executes the actions when the
  transition is triggered. QTransition is part of \l{The State Machine
  Framework}.

  Built-in actions are provided for setting properties and invoking methods of
  QObjects. The setPropertyOnTransition() function is used for defining
  property assignments that should be performed when a transition is taken.
  The invokeMethodOnTransition() function is used for defining method
  invocations that should be performed when a transition is taken.

  \code
  QStateMachine machine;
  QState *s1 = new QState();
  machine.addState(s1);
  QTransition *t1 = new QTransition();
  QLabel label;
  t1->setPropertyOnTransition(&label, "text", "Transition t1 was triggered");
  QState *s2 = new QState();
  machine.addState(s2);
  t1->setTargetState(s2);
  s1->addTransition(t1);
  \endcode

  Actions are executed in the order in which they were added.

  \sa QState::addTransition(), QStateAction
*/

QTransitionPrivate::QTransitionPrivate()
{
}

QTransitionPrivate::~QTransitionPrivate()
{
}

QTransitionPrivate *QTransitionPrivate::get(QTransition *q)
{
    return q->d_func();
}

const QTransitionPrivate *QTransitionPrivate::get(const QTransition *q)
{
    return q->d_func();
}

QList<QStateAction*> QTransitionPrivate::actions() const
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
  Constructs a new QTransition object with the given \a sourceState.
*/
QTransition::QTransition(QState *sourceState)
    : QAbstractTransition(*new QTransitionPrivate, sourceState)
{
}

/*!
  Constructs a new QTransition object with the given \a targets and \a
  sourceState.
*/
QTransition::QTransition(const QList<QAbstractState*> &targets, QState *sourceState)
    : QAbstractTransition(*new QTransitionPrivate, targets, sourceState)
{
}

/*!
  \internal
*/
QTransition::QTransition(QTransitionPrivate &dd, QState *parent)
    : QAbstractTransition(dd, parent)
{
}

/*!
  \internal
*/
QTransition::QTransition(QTransitionPrivate &dd, const QList<QAbstractState*> &targets, QState *parent)
    : QAbstractTransition(dd, targets, parent)
{
}

/*!
  Destroys this transition.
*/
QTransition::~QTransition()
{
}

/*!
  Instructs this QTransition to set the property with the given \a name of the
  given \a object to the given \a value when the transition is taken. This
  function will create a QStateSetPropertyAction object and add it to the
  actions of the transition. If there is already an existing action associated
  with the property, the value of that action is updated.

  \sa invokeMethodOnTransition()
*/
void QTransition::setPropertyOnTransition(QObject *object, const char *name,
                                          const QVariant &value)
{
    Q_D(QTransition);
    QList<QStateAction*> actions = d->actions();
    for (int i = 0; i < actions.size(); ++i) {
        QStateAction *action = actions.at(i);           
        if (QStateSetPropertyAction *spa = qobject_cast<QStateSetPropertyAction*>(action)) {
            if (spa->targetObject() == object && spa->propertyName() == name) {
                QStateSetPropertyActionPrivate::get(spa)->value = value;
                return;
            }
        }
    }
    addAction(new QStateSetPropertyAction(object, name, value));
}

/*!
  Instructs this QTransition to invoke the given \a method of the given \a
  object with the given \a arguments when the transition is taken. This
  function will create a QStateInvokeMethodAction object and add it to the
  actions of the transition.

  \sa setPropertyOnTransition()
*/
void QTransition::invokeMethodOnTransition(QObject *object, const char *method,
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
void QTransition::addAction(QStateAction *action)
{
    if (!action) {
        qWarning("QTransition::addAction: cannot add null action");
        return;
    }
    action->setParent(this);
}

/*!
  Removes the given \a action from this transition.
  The transition releases ownership of the action.

  \sa addAction()
*/
void QTransition::removeAction(QStateAction *action)
{
    if (!action) {
        qWarning("QTransition::removeAction: cannot remove null action");
        return;
    }
    action->setParent(0);
}

/*!
  Returns this transitions's actions, or an empty list if the transition has
  no actions.

  \sa addAction()
*/
QList<QStateAction*> QTransition::actions() const
{
    Q_D(const QTransition);
    return d->actions();
}

/*!
  \reimp
*/
void QTransition::onTransition()
{
    Q_D(QTransition);
    QList<QStateAction*> actions = d->actions();
    for (int i = 0; i < actions.size(); ++i)
        QStateActionPrivate::get(actions.at(i))->callExecute();
}

/*!
  \reimp
*/
bool QTransition::event(QEvent *e)
{
    return QAbstractTransition::event(e);
}

QT_END_NAMESPACE
