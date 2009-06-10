/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmltransition.h"
#include "qmlstategroup.h"
#include "qmlstate_p.h"
#include "qmlbindablevalue.h"
#include "qmlstateoperations.h"
#include "qmlanimation.h"
#include "qmlanimation_p.h"
#include "qmlstate.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

Action::Action() : restore(true), bv(0), event(0), actionDone(false)
{
}

ActionEvent::~ActionEvent()
{
}

QString ActionEvent::name() const
{
    return QString();
}

void ActionEvent::execute()
{
}

/*!
    \internal
*/
QmlStateOperation::QmlStateOperation(QObjectPrivate &dd, QObject *parent)
    : QObject(dd, parent)
{
}

/*!
    \qmlclass State
    \brief The State element defines configurations of objects and properties.

    A state is specified as a set of batched changes from the default configuration.

    Note that setting the state of an object from within another state of the same object is
    inadvisible. Not only would this have the same effect as going directly to the second state
    it may cause the program to crash.

    \sa {states-transitions}{States and Transitions}
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

QML_DEFINE_TYPE(QmlState,State)
QmlState::QmlState(QObject *parent)
: QObject(*(new QmlStatePrivate), parent)
{
}

QmlState::~QmlState()
{
}

/*!
    \qmlproperty string State::name
    This property holds the name of the state

    Each state should have a unique name.
*/

/*!
    \property QmlState::name
    \brief the name of the state

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

/*!
    \property QmlState::when
    \brief when the state should be applied

    This should be set to an expression that evaluates to true when you want the state to
    be applied.
*/
QmlBindableValue *QmlState::when() const
{
    Q_D(const QmlState);
    return d->when;
}

void QmlState::setWhen(QmlBindableValue *when) 
{
    Q_D(QmlState);
    d->when = when;
    if (d->group)
        d->group->updateAutoState();
}

/*!
    \qmlproperty string State::extends
    This property holds the state that this state extends

    The state being extended is treated as the base state in regards to
    the changes specified by the extending state.
*/

/*!
    \property QmlState::extends
    \brief the state that this state extends

    The state being extended is treated as the base state in regards to
    the changes specified by the extending state.

    \sa operations
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
    \qmlproperty list<StateOperation> State::operations
    This property holds the changes to apply for this state
    \default

    By default these changes are applied against the default state. If the state
    extends another state, then the changes are applied against the state being
    extended.
*/

/*!
    \property QmlState::operations
    \brief the changes to apply for this state

    By default these changes are applied against the default state. If the state
    extends another state, then the changes are applied against the state being
    extended.
*/
QmlList<QmlStateOperation *> *QmlState::operations()
{
    Q_D(QmlState);
    return &d->operations;
}

QmlState &QmlState::operator<<(QmlStateOperation *op)
{
    Q_D(QmlState);
    d->operations.append(op);
    return *this;
}

#if 0
static void dump(const QmlStateOperation::ActionList &list)
{
    if (!QString(getenv("STATE_DEBUG")).isEmpty())
        return;

    for (int ii = 0; ii < list.count(); ++ii) {
        const Action &action = list.at(ii);
        qWarning() << action.property.object << action.property.name << action.toValue;
    }
}
#endif

void QmlStatePrivate::applyBindings()
{
    foreach(const Action &action, bindingsList) {
        if (action.bv && !action.toBinding.isEmpty()) {
            action.bv->setExpression(action.toBinding);
        }
    }
}

void QmlStatePrivate::complete()
{
    Q_Q(QmlState);
    //apply bindings (now that all transitions are complete)
    applyBindings();

    for (int ii = 0; ii < reverting.count(); ++ii) {
        for (int jj = 0; jj < revertList.count(); ++jj) {
            if (revertList.at(jj).property == reverting.at(ii)) {
                revertList.removeAt(jj);
                break;
            }
        }
    }
    reverting.clear();

    for (int ii = 0; ii < completeList.count(); ++ii) {
        const QmlMetaProperty &prop = completeList.at(ii).property;
        prop.write(completeList.at(ii).value);
    }

    completeList.clear();
    transition = 0;
    emit q->completed();
}

QmlStateOperation::ActionList QmlStatePrivate::generateActionList(QmlStateGroup *group) const
{
    QmlStateOperation::ActionList applyList;
    if (inState)
        return applyList;

    inState = true;

    if (!extends.isEmpty()) {
        QList<QmlState *> states = group->states();
        for (int ii = 0; ii < states.count(); ++ii)
            if (states.at(ii)->name() == extends)
                applyList = static_cast<QmlStatePrivate*>(states.at(ii)->d_ptr)->generateActionList(group);
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
    if (d->transition) {
        d->transition->stop();  //XXX this could potentially trigger a complete in rare circumstances
        d->transition = 0;
    }
}

void QmlState::apply(QmlStateGroup *group, QmlTransition *trans, QmlState *revert)
{
    Q_D(QmlState);

    cancel();
    if (revert)
        revert->cancel();
    d->revertList.clear();
    d->reverting.clear();
    d->bindingsList.clear();

    if (revert)
        d->revertList = static_cast<QmlStatePrivate*>(revert->d_ptr)->revertList;
    QmlStateOperation::RevertActionList additionalReverts;

    QmlStateOperation::ActionList applyList = d->generateActionList(group);

    for (int ii = 0; ii < applyList.count(); ++ii) {
        const Action &action = applyList.at(ii);
        if (action.event || !action.restore)
            continue;

        bool found = false;
        for (int jj = 0; !found && jj < d->revertList.count(); ++jj) {
            if (d->revertList.at(jj).property == action.property)
                found = true;
        }
        if (!found) {
            RevertAction r(action);
            additionalReverts << r;
        }
    }
    for (int ii = 0; ii < d->revertList.count(); ++ii) {
        bool found = false;
        for (int jj = 0; !found && jj < applyList.count(); ++jj) {
            const Action &action = applyList.at(jj);
            if (action.property == d->revertList.at(ii).property)
                found = true;
        }
        if (!found) {
            QVariant cur = d->revertList.at(ii).property.read();
            Action a;
            a.property = d->revertList.at(ii).property;
            a.fromValue = cur;
            a.toValue = d->revertList.at(ii).value;
            a.toBinding = d->revertList.at(ii).binding;
            if (!a.toBinding.isEmpty()) {
                a.fromBinding = d->revertList.at(ii).bv->expression();
                a.bv = d->revertList.at(ii).bv;
            }
            applyList << a;
            d->reverting << d->revertList.at(ii).property;
        }
    }
    d->revertList << additionalReverts;

    //apply all changes, and work out any ending positions for bindings
    //then rewind all changes and proceed as normal
    //### 4 foreach loops!
    ////////////////////////////////////////////////////////////////////
    foreach(const Action &action, applyList) {
        if (stateChangeDebug())
            qWarning() << "    Action:" << action.property.object() << action.property.name() << action.toValue;

        if (action.bv && !action.toBinding.isEmpty()) {
            d->bindingsList << action;
            action.bv->clearExpression();
        }
    }

    if (!d->bindingsList.isEmpty()) {
        foreach(const Action &action, applyList) {
            if (action.bv && !action.toBinding.isEmpty()) {
                action.bv->setExpression(action.toBinding);
            } else if (!action.event) {
                action.property.write(action.toValue);
            }
        }

        for (int ii = 0; ii < applyList.size(); ++ii) {
            Action *action = &applyList[ii];
            if (action->event)
                continue;

            const QmlMetaProperty &prop = action->property;
            if (action->bv && !action->toBinding.isEmpty()) {
                action->toValue = prop.read();
            }
        }

        foreach(const Action &action, applyList) {
            if (action.event)
                continue;

            if (action.bv && !action.toBinding.isEmpty())
                action.bv->clearExpression();
            action.property.write(action.fromValue);
        }
    }
    ////////////////////////////////////////////////////////////////////

    QmlStateOperation::ActionList modList = applyList;
    QList<QmlMetaProperty> touched;
    d->completeList.clear();
    if (trans) {
        d->transition = trans;
        trans->prepare(modList, touched, this);
        for (int ii = 0; ii < modList.count(); ++ii) {
            const Action &action = modList.at(ii);

            if (action.event) {
                if (action.actionDone) {
                    modList.removeAt(ii);
                    --ii;
                }
            } else {
                if (action.toValue != action.fromValue) {
                    d->completeList << RevertAction(action, false);
                }

                if (touched.contains(action.property)) {
                    modList.removeAt(ii);
                    --ii;
                }
            }
        }
    }

    foreach(const Action &action, modList) {
        if (action.event)
            action.event->execute();
        else
            action.property.write(action.toValue);
    }
    if (!trans)
        d->applyBindings(); //### merge into above foreach?
}

QML_DEFINE_TYPE(QmlStateOperation,StateOperation)
QmlStateOperation::ActionList QmlStateOperation::actions()
{
    return ActionList();
}

QT_END_NAMESPACE
