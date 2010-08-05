/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativestate_p_p.h"
#include "private/qdeclarativestate_p.h"

#include "private/qdeclarativetransition_p.h"
#include "private/qdeclarativestategroup_p.h"
#include "private/qdeclarativestateoperations_p.h"
#include "private/qdeclarativeanimation_p.h"
#include "private/qdeclarativeanimation_p_p.h"

#include <qdeclarativebinding_p.h>
#include <qdeclarativeglobal_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

QDeclarativeAction::QDeclarativeAction()
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false), fromBinding(0), toBinding(0), event(0),
  specifiedObject(0)
{
}

QDeclarativeAction::QDeclarativeAction(QObject *target, const QString &propertyName,
               const QVariant &value)
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false), 
  property(target, propertyName), toValue(value), 
  fromBinding(0), toBinding(0), event(0), 
  specifiedObject(target), specifiedProperty(propertyName)
{
    if (property.isValid())
        fromValue = property.read();
}

QDeclarativeAction::QDeclarativeAction(QObject *target, const QString &propertyName,
               QDeclarativeContext *context, const QVariant &value)
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false),
  property(target, propertyName, context), toValue(value),
  fromBinding(0), toBinding(0), event(0),
  specifiedObject(target), specifiedProperty(propertyName)
{
    if (property.isValid())
        fromValue = property.read();
}


QDeclarativeActionEvent::~QDeclarativeActionEvent()
{
}

QString QDeclarativeActionEvent::typeName() const
{
    return QString();
}

void QDeclarativeActionEvent::execute(Reason)
{
}

bool QDeclarativeActionEvent::isReversable()
{
    return false;
}

void QDeclarativeActionEvent::reverse(Reason)
{
}

bool QDeclarativeActionEvent::changesBindings()
{
    return false;
}

void QDeclarativeActionEvent::clearBindings()
{
}

bool QDeclarativeActionEvent::override(QDeclarativeActionEvent *other)
{
    Q_UNUSED(other);
    return false;
}

/*!
    \internal
*/
QDeclarativeStateOperation::QDeclarativeStateOperation(QObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \qmlclass State QDeclarativeState
    \since 4.7
    \brief The State element defines configurations of objects and properties.

    A \e state is a set of batched changes from the default configuration.

    All items have a default state that defines the default configuration of objects
    and property values. New states can be defined by adding State items to the \l {Item::states}{states} property to
    allow items to switch between different configurations. These configurations
    can, for example, be used to apply different sets of property values or execute
    different scripts.

    The following example displays a single \l Rectangle. In the default state, the rectangle
    is colored black. In the "clicked" state, a PropertyChanges element changes the
    rectangle's color to red. Clicking within the MouseArea toggles the rectangle's state
    between the default state and the "clicked" state, thus toggling the color of the
    rectangle between black and red.

    \snippet doc/src/snippets/declarative/state.qml 0

    Notice the default state is referred to using an empty string ("").

    States are commonly used together with \l {Transitions} to provide
    animations when state changes occur.

    \note Setting the state of an object from within another state of the same object is
    not allowed.

    \sa {declarative/animation/states}{states example}, {qmlstates}{States}, {state-transitions}{Transitions}, QtDeclarative
*/

/*!
    \internal
    \class QDeclarativeState
    \brief The QDeclarativeState class allows you to define configurations of objects and properties.


    QDeclarativeState allows you to specify a state as a set of batched changes from the default
    configuration.

    \sa {states-transitions}{States and Transitions}
*/


QDeclarativeState::QDeclarativeState(QObject *parent)
: QObject(*(new QDeclarativeStatePrivate), parent)
{
    Q_D(QDeclarativeState);
    d->transitionManager.setState(this);
}

QDeclarativeState::~QDeclarativeState()
{
    Q_D(QDeclarativeState);
    if (d->group)
        d->group->removeState(this);
}

/*!
    \qmlproperty string State::name
    This property holds the name of the state.

    Each state should have a unique name within its item.
*/
QString QDeclarativeState::name() const
{
    Q_D(const QDeclarativeState);
    return d->name;
}

void QDeclarativeState::setName(const QString &n)
{
    Q_D(QDeclarativeState);
    d->name = n;
    d->named = true;
}

bool QDeclarativeState::isNamed() const
{
    Q_D(const QDeclarativeState);
    return d->named;
}

bool QDeclarativeState::isWhenKnown() const
{
    Q_D(const QDeclarativeState);
    return d->when != 0;
}

/*!
    \qmlproperty bool State::when
    This property holds when the state should be applied.

    This should be set to an expression that evaluates to \c true when you want the state to
    be applied. For example, the following \l Rectangle changes in and out of the "hidden"
    state when the \l MouseArea is pressed:

    \snippet doc/src/snippets/declarative/state-when.qml 0

    If multiple states in a group have \c when clauses that evaluate to \c true at the same time,
    the first matching state will be applied. For example, in the following snippet
    \c state1 will always be selected rather than \c state2 when sharedCondition becomes
    \c true.
    \qml
    states: [
        State { name: "state1"; when: sharedCondition },
        State { name: "state2"; when: sharedCondition }
    ]
    \endqml
*/
QDeclarativeBinding *QDeclarativeState::when() const
{
    Q_D(const QDeclarativeState);
    return d->when;
}

void QDeclarativeState::setWhen(QDeclarativeBinding *when)
{
    Q_D(QDeclarativeState);
    d->when = when;
    if (d->group)
        d->group->updateAutoState();
}

/*!
    \qmlproperty string State::extend
    This property holds the state that this state extends.

    When a state extends another state, it inherits all the changes of that state.

    The state being extended is treated as the base state in regards to
    the changes specified by the extending state.
*/
QString QDeclarativeState::extends() const
{
    Q_D(const QDeclarativeState);
    return d->extends;
}

void QDeclarativeState::setExtends(const QString &extends)
{
    Q_D(QDeclarativeState);
    d->extends = extends;
}

/*!
    \qmlproperty list<Change> State::changes
    This property holds the changes to apply for this state
    \default

    By default these changes are applied against the default state. If the state
    extends another state, then the changes are applied against the state being
    extended.
*/
QDeclarativeListProperty<QDeclarativeStateOperation> QDeclarativeState::changes()
{
    Q_D(QDeclarativeState);
    return QDeclarativeListProperty<QDeclarativeStateOperation>(this, &d->operations, QDeclarativeStatePrivate::operations_append,
                                              QDeclarativeStatePrivate::operations_count, QDeclarativeStatePrivate::operations_at,
                                              QDeclarativeStatePrivate::operations_clear);
}

int QDeclarativeState::operationCount() const
{
    Q_D(const QDeclarativeState);
    return d->operations.count();
}

QDeclarativeStateOperation *QDeclarativeState::operationAt(int index) const
{
    Q_D(const QDeclarativeState);
    return d->operations.at(index);
}

QDeclarativeState &QDeclarativeState::operator<<(QDeclarativeStateOperation *op)
{
    Q_D(QDeclarativeState);
    d->operations.append(QDeclarativeStatePrivate::OperationGuard(op, &d->operations));
    return *this;
}

void QDeclarativeStatePrivate::complete()
{
    Q_Q(QDeclarativeState);

    for (int ii = 0; ii < reverting.count(); ++ii) {
        for (int jj = 0; jj < revertList.count(); ++jj) {
            if (revertList.at(jj).property == reverting.at(ii)) {
                revertList.removeAt(jj);
                break;
            }
        }
    }
    reverting.clear();

    emit q->completed();
}

// Generate a list of actions for this state.  This includes coelescing state
// actions that this state "extends"
QDeclarativeStateOperation::ActionList
QDeclarativeStatePrivate::generateActionList(QDeclarativeStateGroup *group) const
{
    QDeclarativeStateOperation::ActionList applyList;
    if (inState)
        return applyList;

    // Prevent "extends" recursion
    inState = true;

    if (!extends.isEmpty()) {
        QList<QDeclarativeState *> states = group->states();
        for (int ii = 0; ii < states.count(); ++ii)
            if (states.at(ii)->name() == extends)
                applyList = static_cast<QDeclarativeStatePrivate*>(states.at(ii)->d_func())->generateActionList(group);
    }

    foreach(QDeclarativeStateOperation *op, operations)
        applyList << op->actions();

    inState = false;
    return applyList;
}

QDeclarativeStateGroup *QDeclarativeState::stateGroup() const
{
    Q_D(const QDeclarativeState);
    return d->group;
}

void QDeclarativeState::setStateGroup(QDeclarativeStateGroup *group)
{
    Q_D(QDeclarativeState);
    d->group = group;
}

void QDeclarativeState::cancel()
{
    Q_D(QDeclarativeState);
    d->transitionManager.cancel();
}

void QDeclarativeAction::deleteFromBinding()
{
    if (fromBinding) {
        QDeclarativePropertyPrivate::setBinding(property, 0);
        fromBinding->destroy();
        fromBinding = 0;
    }
}

void QDeclarativeState::apply(QDeclarativeStateGroup *group, QDeclarativeTransition *trans, QDeclarativeState *revert)
{
    Q_D(QDeclarativeState);

    qmlExecuteDeferred(this);

    cancel();
    if (revert)
        revert->cancel();
    d->revertList.clear();
    d->reverting.clear();

    if (revert) {
        QDeclarativeStatePrivate *revertPrivate =
            static_cast<QDeclarativeStatePrivate*>(revert->d_func());
        d->revertList = revertPrivate->revertList;
        revertPrivate->revertList.clear();
    }

    // List of actions caused by this state
    QDeclarativeStateOperation::ActionList applyList = d->generateActionList(group);

    // List of actions that need to be reverted to roll back (just) this state
    QDeclarativeStatePrivate::SimpleActionList additionalReverts;
    // First add the reverse of all the applyList actions
    for (int ii = 0; ii < applyList.count(); ++ii) {
        QDeclarativeAction &action = applyList[ii];

        if (action.event) {
            if (!action.event->isReversable())
                continue;
            bool found = false;
            for (int jj = 0; jj < d->revertList.count(); ++jj) {
                QDeclarativeActionEvent *event = d->revertList.at(jj).event;
                if (event && event->typeName() == action.event->typeName()) {
                    if (action.event->override(event)) {
                        found = true;

                        if (action.event != d->revertList.at(jj).event && action.event->needsCopy()) {
                            action.event->copyOriginals(d->revertList.at(jj).event);

                            QDeclarativeSimpleAction r(action);
                            additionalReverts << r;
                            d->revertList.removeAt(jj);
                            --jj;
                        } else if (action.event->isRewindable())    //###why needed?
                            action.event->saveCurrentValues();

                        break;
                    }
                }
            }
            if (!found) {
                action.event->saveOriginals();
                // Only need to revert the applyList action if the previous
                // state doesn't have a higher priority revert already
                QDeclarativeSimpleAction r(action);
                additionalReverts << r;
            }
        } else {
            bool found = false;
            action.fromBinding = QDeclarativePropertyPrivate::binding(action.property);

            for (int jj = 0; jj < d->revertList.count(); ++jj) {
                if (d->revertList.at(jj).property == action.property) {
                    found = true;
                    if (d->revertList.at(jj).binding != action.fromBinding) {
                        action.deleteFromBinding();
                    }
                    break;
                }
            }

            if (!found) {
                if (!action.restore) {
                    action.deleteFromBinding();
                } else {
                    // Only need to revert the applyList action if the previous
                    // state doesn't have a higher priority revert already
                    QDeclarativeSimpleAction r(action);
                    additionalReverts << r;
                }
            }
        }
    }

    // Any reverts from a previous state that aren't carried forth
    // into this state need to be translated into apply actions
    for (int ii = 0; ii < d->revertList.count(); ++ii) {
        bool found = false;
        if (d->revertList.at(ii).event) {
            QDeclarativeActionEvent *event = d->revertList.at(ii).event;
            if (!event->isReversable())
                continue;
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QDeclarativeAction &action = applyList.at(jj);
                if (action.event && action.event->typeName() == event->typeName()) {
                    if (action.event->override(event))
                        found = true;
                }
            }
        } else {
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QDeclarativeAction &action = applyList.at(jj);
                if (action.property == d->revertList.at(ii).property)
                    found = true;
            }
        }
        if (!found) {
            QVariant cur = d->revertList.at(ii).property.read();
            QDeclarativeAbstractBinding *delBinding = 
                QDeclarativePropertyPrivate::setBinding(d->revertList.at(ii).property, 0);
            if (delBinding)
                delBinding->destroy();

            QDeclarativeAction a;
            a.property = d->revertList.at(ii).property;
            a.fromValue = cur;
            a.toValue = d->revertList.at(ii).value;
            a.toBinding = d->revertList.at(ii).binding;
            a.specifiedObject = d->revertList.at(ii).specifiedObject;
            a.specifiedProperty = d->revertList.at(ii).specifiedProperty;
            a.event = d->revertList.at(ii).event;
            a.reverseEvent = d->revertList.at(ii).reverseEvent;
            if (a.event && a.event->isRewindable())
                a.event->saveCurrentValues();
            applyList << a;
            // Store these special reverts in the reverting list
            d->reverting << d->revertList.at(ii).property;
        }
    }
    // All the local reverts now become part of the ongoing revertList
    d->revertList << additionalReverts;

#ifndef QT_NO_DEBUG_STREAM
    // Output for debugging
    if (stateChangeDebug()) {
        foreach(const QDeclarativeAction &action, applyList) {
            if (action.event)
                qWarning() << "    QDeclarativeAction event:" << action.event->typeName();
            else
                qWarning() << "    QDeclarativeAction:" << action.property.object()
                           << action.property.name() << "From:" << action.fromValue 
                           << "To:" << action.toValue;
        }
    }
#endif

    d->transitionManager.transition(applyList, trans);
}

QDeclarativeStateOperation::ActionList QDeclarativeStateOperation::actions()
{
    return ActionList();
}

QT_END_NAMESPACE
