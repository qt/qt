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

#include "qmlstategroup_p.h"

#include "qmltransition_p.h"
#include "qmlstate_p_p.h"

#include <qmlbinding.h>
#include <qmlglobal_p.h>

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

class QmlStateGroupPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlStateGroup)
public:
    QmlStateGroupPrivate(QmlStateGroup *p)
    : nullState(0), componentComplete(true),
      ignoreTrans(false), applyingState(false) {}

    QString currentState;
    QmlState *nullState;

    static void append_state(QmlListProperty<QmlState> *list, QmlState *state);
    static int count_state(QmlListProperty<QmlState> *list);
    static QmlState *at_state(QmlListProperty<QmlState> *list, int index);

    QList<QmlState *> states;
    QList<QmlTransition *> transitions;

    bool componentComplete;
    bool ignoreTrans;
    bool applyingState;

    QmlTransition *findTransition(const QString &from, const QString &to);
    void setCurrentStateInternal(const QString &state, bool = false);
    bool updateAutoState();
};

/*!
   \qmlclass StateGroup QmlStateGroup
   \brief The StateGroup element provides state support for non-Item elements.

   Item (and all dervied elements) provides built in support for states and transitions
   via its state, states and transitions properties. StateGroup provides an easy way to
   use this support in other (non-Item-derived) elements.

   \qml
   MyCustomObject {
       StateGroup {
           id: myStateGroup
           states: State {
               name: "state1"
               ...
           }
           transitions: Transition {
               ...
           }
       }

       onSomethingHappened: myStateGroup.state = "state1";
   }
   \endqml

   \sa {qmlstate}{States} {state-transitions}{Transitions}
*/

QmlStateGroup::QmlStateGroup(QObject *parent)
    : QObject(*(new QmlStateGroupPrivate(this)), parent)
{
}

QmlStateGroup::~QmlStateGroup()
{
    Q_D(const QmlStateGroup);
    for (int i = 0; i < d->states.count(); ++i)
        d->states.at(i)->setStateGroup(0);
}

QList<QmlState *> QmlStateGroup::states() const
{
    Q_D(const QmlStateGroup);
    return d->states;
}

/*!
  \qmlproperty list<State> StateGroup::states
  This property holds a list of states defined by the state group.

  \qml
  StateGroup {
    states: [
      State { ... },
      State { ... }
      ...
    ]
  }
  \endqml

  \sa {qmlstate}{States}
*/
QmlListProperty<QmlState> QmlStateGroup::statesProperty()
{
    Q_D(QmlStateGroup);
    return QmlListProperty<QmlState>(this, &d->states, &QmlStateGroupPrivate::append_state,
                                                       &QmlStateGroupPrivate::count_state,
                                                       &QmlStateGroupPrivate::at_state);
}

void QmlStateGroupPrivate::append_state(QmlListProperty<QmlState> *list, QmlState *state)
{
    QmlStateGroup *_this = static_cast<QmlStateGroup *>(list->object);
    if (state) {
        _this->d_func()->states.append(state);
        state->setStateGroup(_this);
    }

}

int QmlStateGroupPrivate::count_state(QmlListProperty<QmlState> *list)
{
    QmlStateGroup *_this = static_cast<QmlStateGroup *>(list->object);
    return _this->d_func()->states.count();
}

QmlState *QmlStateGroupPrivate::at_state(QmlListProperty<QmlState> *list, int index)
{
    QmlStateGroup *_this = static_cast<QmlStateGroup *>(list->object);
    return _this->d_func()->states.at(index);
}

/*!
  \qmlproperty list<Transition> StateGroup::transitions
  This property holds a list of transitions defined by the state group.

  \qml
  StateGroup {
    transitions: [
      Transition { ... },
      Transition { ... }
      ...
    ]
  }
  \endqml

  \sa {state-transitions}{Transitions}
*/
QmlListProperty<QmlTransition> QmlStateGroup::transitionsProperty()
{
    Q_D(QmlStateGroup);
    return QmlListProperty<QmlTransition>(this, d->transitions);
}

/*!
  \qmlproperty string StateGroup::state

  This property holds the name of the current state of the state group.

  This property is often used in scripts to change between states. For
  example:

  \qml
    Script {
        function toggle() {
            if (button.state == 'On')
                button.state = 'Off';
            else
                button.state = 'On';
        }
    }
  \endqml

  If the state group is in its base state (i.e. no explicit state has been
  set), \c state will be a blank string. Likewise, you can return a
  state group to its base state by setting its current state to \c ''.

  \sa {qmlstates}{States}
*/
QString QmlStateGroup::state() const
{
    Q_D(const QmlStateGroup);
    return d->currentState;
}

void QmlStateGroup::setState(const QString &state)
{
    Q_D(QmlStateGroup);
    if (d->currentState == state)
        return;

    d->setCurrentStateInternal(state);
}

void QmlStateGroup::classBegin()
{
    Q_D(QmlStateGroup);
    d->componentComplete = false;
}

void QmlStateGroup::componentComplete()
{
    Q_D(QmlStateGroup);
    d->componentComplete = true;

    if (d->updateAutoState()) {
        return;
    } else if (!d->currentState.isEmpty()) {
        QString cs = d->currentState;
        d->currentState = QString();
        d->setCurrentStateInternal(cs, true);
    }
}

/*!
    Returns true if the state was changed, otherwise false.
*/
bool QmlStateGroup::updateAutoState()
{
    Q_D(QmlStateGroup);
    return d->updateAutoState();
}

bool QmlStateGroupPrivate::updateAutoState()
{
    Q_Q(QmlStateGroup);
    if (!componentComplete)
        return false;

    bool revert = false;
    for (int ii = 0; ii < states.count(); ++ii) {
        QmlState *state = states.at(ii);
        if (state->isWhenKnown()) {
            if (!state->name().isEmpty()) {
                if (state->when() && state->when()->value().toBool()) {
                    if (stateChangeDebug()) 
                        qWarning() << "Setting auto state due to:" 
                                   << state->when()->expression();
                    if (currentState != state->name()) {
                        q->setState(state->name());
                        return true;
                    } else {
                        return false;
                    }
                } else if (state->name() == currentState) {
                    revert = true;
                }
            }
        }
    }
    if (revert) {
        bool rv = currentState != QString();
        q->setState(QString());
        return rv;
    } else {
        return false;
    }
}

QmlTransition *QmlStateGroupPrivate::findTransition(const QString &from, const QString &to)
{
    QmlTransition *highest = 0;
    int score = 0;
    bool reversed = false;
    bool done = false;

    for (int ii = 0; !done && ii < transitions.count(); ++ii) {
        QmlTransition *t = transitions.at(ii);
        for (int ii = 0; ii < 2; ++ii)
        {
            if (ii && (!t->reversible() ||
                      (t->fromState() == QLatin1String("*") && 
                       t->toState() == QLatin1String("*"))))
                break;
            QStringList fromState;
            QStringList toState;

            fromState = t->fromState().split(QLatin1Char(','));
            toState = t->toState().split(QLatin1Char(','));
            if (ii == 1)
                qSwap(fromState, toState);
            int tScore = 0;
            if (fromState.contains(from))
                tScore += 2;
            else if (fromState.contains(QLatin1String("*")))
                tScore += 1;
            else
                continue;

            if (toState.contains(to))
                tScore += 2;
            else if (toState.contains(QLatin1String("*")))
                tScore += 1;
            else
                continue;

            if (ii == 1)
                reversed = true;
            else
                reversed = false;

            if (tScore == 4) {
                highest = t;
                done = true;
                break;
            } else if (tScore > score) {
                score = tScore;
                highest = t;
            }
        }
    }

    if (highest)
        highest->setReversed(reversed);

    return highest;
}

void QmlStateGroupPrivate::setCurrentStateInternal(const QString &state, 
                                                   bool ignoreTrans)
{
    Q_Q(QmlStateGroup);
    if (!componentComplete) {
        currentState = state;
        return;
    }

    if (applyingState) {
        qWarning() << "Can't apply a state change as part of a state definition.";
        return;
    }

    applyingState = true;

    QmlTransition *transition = (ignoreTrans || ignoreTrans) ? 0 : findTransition(currentState, state);
    if (stateChangeDebug()) {
        qWarning() << this << "Changing state.  From" << currentState << ". To" << state;
        if (transition)
            qWarning() << "   using transition" << transition->fromState() 
                       << transition->toState();
    }

    QmlState *oldState = 0;
    if (!currentState.isEmpty()) {
        for (int ii = 0; ii < states.count(); ++ii) {
            if (states.at(ii)->name() == currentState) {
                oldState = states.at(ii);
                break;
            }
        }
    }

    currentState = state;
    emit q->stateChanged(currentState);

    QmlState *newState = 0;
    for (int ii = 0; ii < states.count(); ++ii) {
        if (states.at(ii)->name() == currentState) {
            newState = states.at(ii);
            break;
        }
    }

    if (oldState == 0 || newState == 0) {
        if (!nullState) { nullState = new QmlState; QmlGraphics_setParent_noEvent(nullState, q); }
        if (!oldState) oldState = nullState;
        if (!newState) newState = nullState;
    }

    newState->apply(q, transition, oldState);
    applyingState = false;
    if (!transition)
        static_cast<QmlStatePrivate*>(QObjectPrivate::get(newState))->complete();
}

QmlState *QmlStateGroup::findState(const QString &name) const
{
    Q_D(const QmlStateGroup);
    for (int i = 0; i < d->states.count(); ++i) {
        QmlState *state = d->states.at(i);
        if (state->name() == name)
            return state;
    }

    return 0;
}

void QmlStateGroup::removeState(QmlState *state)
{
    Q_D(QmlStateGroup);
    d->states.removeOne(state);
}

QT_END_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,StateGroup,QmlStateGroup)
