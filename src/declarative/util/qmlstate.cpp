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

#include "qmlstate_p_p.h"
#include "qmlstate_p.h"

#include "qmltransition_p.h"
#include "qmlstategroup_p.h"
#include "qmlstateoperations_p.h"
#include "qmlanimation_p.h"
#include "qmlanimation_p_p.h"

#include <qmlbinding.h>
#include <qmlglobal_p.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

QmlAction::QmlAction()
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false), fromBinding(0), toBinding(0), event(0),
  specifiedObject(0)
{
}

QmlAction::QmlAction(QObject *target, const QString &propertyName,
               const QVariant &value)
: restore(true), actionDone(false), reverseEvent(false), deletableToBinding(false), toValue(value), fromBinding(0),
  toBinding(0), event(0), specifiedObject(target),
  specifiedProperty(propertyName)
{
    property = QmlMetaProperty::createProperty(target, propertyName);
    if (property.isValid())
        fromValue = property.read();
}

QmlActionEvent::~QmlActionEvent()
{
}

QString QmlActionEvent::typeName() const
{
    return QString();
}

void QmlActionEvent::execute()
{
}

bool QmlActionEvent::isReversable()
{
    return false;
}

void QmlActionEvent::reverse()
{
}

QList<QmlAction> QmlActionEvent::extraActions()
{
    return QList<QmlAction>();
}

bool QmlActionEvent::changesBindings()
{
    return false;
}

void QmlActionEvent::clearForwardBindings()
{
}

void QmlActionEvent::clearReverseBindings()
{
}

bool QmlActionEvent::override(QmlActionEvent *other)
{
    Q_UNUSED(other);
    return false;
}

/*!
    \internal
*/
QmlStateOperation::QmlStateOperation(QObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \qmlclass State QmlState
    \brief The State element defines configurations of objects and properties.

    A state is specified as a set of batched changes from the default configuration.

    Note that setting the state of an object from within another state of the same object is
    inadvisible. Not only would this have the same effect as going directly to the second state
    it may cause the program to crash.

    \sa {qmlstates}{States}, {state-transitions}{Transitions}
*/

/*!
    \internal
    \class QmlState
    \brief The QmlState class allows you to define configurations of objects and properties.

    \ingroup group_states

    QmlState allows you to specify a state as a set of batched changes from the default
    configuration.

    \sa {states-transitions}{States and Transitions}
*/


QmlState::QmlState(QObject *parent)
: QObject(*(new QmlStatePrivate), parent)
{
    Q_D(QmlState);
    d->transitionManager.setState(this);
}

QmlState::~QmlState()
{
    Q_D(QmlState);
    if (d->group)
        d->group->removeState(this);
}

/*!
    \qmlproperty string State::name
    This property holds the name of the state

    Each state should have a unique name.
*/
QString QmlState::name() const
{
    Q_D(const QmlState);
    return d->name;
}

void QmlState::setName(const QString &n)
{
    Q_D(QmlState);
    d->name = n;
}

bool QmlState::isWhenKnown() const
{
    Q_D(const QmlState);
    return d->when != 0;
}

/*!
    \qmlproperty bool State::when
    This property holds when the state should be applied

    This should be set to an expression that evaluates to true when you want the state to
    be applied.
*/
QmlBinding *QmlState::when() const
{
    Q_D(const QmlState);
    return d->when;
}

void QmlState::setWhen(QmlBinding *when)
{
    Q_D(QmlState);
    d->when = when;
    if (d->group)
        d->group->updateAutoState();
}

/*!
    \qmlproperty string State::extend
    This property holds the state that this state extends

    The state being extended is treated as the base state in regards to
    the changes specified by the extending state.
*/
QString QmlState::extends() const
{
    Q_D(const QmlState);
    return d->extends;
}

void QmlState::setExtends(const QString &extends)
{
    Q_D(QmlState);
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
QmlListProperty<QmlStateOperation> QmlState::changes()
{
    Q_D(QmlState);
    return QmlListProperty<QmlStateOperation>(this, &d->operations, QmlStatePrivate::operations_append,
                                              QmlStatePrivate::operations_count, QmlStatePrivate::operations_at,
                                              QmlStatePrivate::operations_clear);
}

int QmlState::operationCount() const
{
    Q_D(const QmlState);
    return d->operations.count();
}

QmlStateOperation *QmlState::operationAt(int index) const
{
    Q_D(const QmlState);
    return d->operations.at(index);
}

QmlState &QmlState::operator<<(QmlStateOperation *op)
{
    Q_D(QmlState);
    d->operations.append(QmlStatePrivate::OperationGuard(op, &d->operations));
    return *this;
}

void QmlStatePrivate::complete()
{
    Q_Q(QmlState);

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
QmlStateOperation::ActionList
QmlStatePrivate::generateActionList(QmlStateGroup *group) const
{
    QmlStateOperation::ActionList applyList;
    if (inState)
        return applyList;

    // Prevent "extends" recursion
    inState = true;

    if (!extends.isEmpty()) {
        QList<QmlState *> states = group->states();
        for (int ii = 0; ii < states.count(); ++ii)
            if (states.at(ii)->name() == extends)
                applyList = static_cast<QmlStatePrivate*>(states.at(ii)->d_func())->generateActionList(group);
    }

    foreach(QmlStateOperation *op, operations)
        applyList << op->actions();

    inState = false;
    return applyList;
}

QmlStateGroup *QmlState::stateGroup() const
{
    Q_D(const QmlState);
    return d->group;
}

void QmlState::setStateGroup(QmlStateGroup *group)
{
    Q_D(QmlState);
    d->group = group;
}

void QmlState::cancel()
{
    Q_D(QmlState);
    d->transitionManager.cancel();
}

void QmlAction::deleteFromBinding()
{
    if (fromBinding) {
        property.setBinding(0);
        fromBinding->destroy();
        fromBinding = 0;
    }
}

void QmlState::apply(QmlStateGroup *group, QmlTransition *trans, QmlState *revert)
{
    Q_D(QmlState);

    qmlExecuteDeferred(this);

    cancel();
    if (revert)
        revert->cancel();
    d->revertList.clear();
    d->reverting.clear();

    if (revert) {
        QmlStatePrivate *revertPrivate =
            static_cast<QmlStatePrivate*>(revert->d_func());
        d->revertList = revertPrivate->revertList;
        revertPrivate->revertList.clear();
    }

    // List of actions caused by this state
    QmlStateOperation::ActionList applyList = d->generateActionList(group);

    // List of actions that need to be reverted to roll back (just) this state
    QmlStatePrivate::SimpleActionList additionalReverts;
    // First add the reverse of all the applyList actions
    for (int ii = 0; ii < applyList.count(); ++ii) {
        QmlAction &action = applyList[ii];

        bool found = false;

        int jj;
        if (action.event) {
            if (!action.event->isReversable())
                continue;
            for (jj = 0; jj < d->revertList.count(); ++jj) {
                QmlActionEvent *event = d->revertList.at(jj).event;
                if (event && event->typeName() == action.event->typeName()) {
                    if (action.event->override(event)) {
                        found = true;
                        break;
                    }
                }
            }
            if (!found || action.event != d->revertList.at(jj).event)
                action.event->saveOriginals();
            else if (action.event->isRewindable())
                action.event->saveCurrentValues();
        } else {
            action.fromBinding = action.property.binding();

            for (jj = 0; jj < d->revertList.count(); ++jj) {
                if (d->revertList.at(jj).property == action.property) {
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            if (!action.restore) {
                action.deleteFromBinding();
            } else {
                // Only need to revert the applyList action if the previous
                // state doesn't have a higher priority revert already
                QmlSimpleAction r(action);
                additionalReverts << r;
            }
        } else if (d->revertList.at(jj).binding != action.fromBinding) {
            action.deleteFromBinding();
        }
    }

    // Any reverts from a previous state that aren't carried forth
    // into this state need to be translated into apply actions
    for (int ii = 0; ii < d->revertList.count(); ++ii) {
        bool found = false;
        if (d->revertList.at(ii).event) {
            QmlActionEvent *event = d->revertList.at(ii).event;
            if (!event->isReversable())
                continue;
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QmlAction &action = applyList.at(jj);
                if (action.event && action.event->typeName() == event->typeName()) {
                    if (action.event->override(event))
                        found = true;
                }
            }
        } else {
            for (int jj = 0; !found && jj < applyList.count(); ++jj) {
                const QmlAction &action = applyList.at(jj);
                if (action.property == d->revertList.at(ii).property)
                    found = true;
            }
        }
        if (!found) {
            QVariant cur = d->revertList.at(ii).property.read();
            QmlAbstractBinding *delBinding = d->revertList.at(ii).property.setBinding(0);
            if (delBinding)
                delBinding->destroy();

            QmlAction a;
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

    // Output for debugging
    if (stateChangeDebug()) {
        foreach(const QmlAction &action, applyList) {
            if (action.event)
                qWarning() << "    QmlAction event:" << action.event->typeName();
            else
                qWarning() << "    QmlAction:" << action.property.object()
                           << action.property.name() << "From:" << action.fromValue 
                           << "To:" << action.toValue;
        }
    }

    d->transitionManager.transition(applyList, trans);
}

QmlStateOperation::ActionList QmlStateOperation::actions()
{
    return ActionList();
}

QT_END_NAMESPACE
