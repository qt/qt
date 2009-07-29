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

#include "private/qobject_p.h"
#include "qmlstategroup.h"
#include "qmltransition.h"
#include <QtDeclarative/qmlbinding.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(stateChangeDebug, STATECHANGE_DEBUG);

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,StateGroup,QmlStateGroup)

class QmlStateGroupPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlStateGroup)
public:
    QmlStateGroupPrivate(QmlStateGroup *p)
    : nullState(0), states(p), componentComplete(true), ignoreTrans(false) {}

    QString currentState;
    QmlState *nullState;

    struct StateList : public QmlConcreteList<QmlState *>
    {
        StateList(QmlStateGroup *g)
            :group(g) {}
        void append(QmlState *s) {
            QmlConcreteList<QmlState *>::append(s);
            if (s) s->setStateGroup(group);
        }
    private:
        QmlStateGroup *group;
    };
    StateList states;

    QmlConcreteList<QmlTransition *> transitions;
    bool componentComplete;
    bool ignoreTrans;

    QmlTransition *findTransition(const QString &from, const QString &to);
    void setCurrentStateInternal(const QString &state, bool = false);
    void updateAutoState();
};

QmlStateGroup::QmlStateGroup(QObject *parent)
    : QObject(*(new QmlStateGroupPrivate(this)), parent)
{
}

QmlStateGroup::~QmlStateGroup()
{
}

QList<QmlState *> QmlStateGroup::states() const
{
    Q_D(const QmlStateGroup);
    return d->states;
}

QmlList<QmlState *>* QmlStateGroup::statesProperty()
{
    Q_D(QmlStateGroup);
    return &(d->states);
}

QmlList<QmlTransition *>* QmlStateGroup::transitionsProperty()
{
    Q_D(QmlStateGroup);
    return &(d->transitions);
}

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

    d->currentState = state;
    emit stateChanged(d->currentState);
}

void QmlStateGroup::classBegin()
{
    Q_D(QmlStateGroup);
    d->componentComplete = false;
}

void QmlStateGroup::updateAutoState()
{
    Q_D(QmlStateGroup);
    d->updateAutoState();
}

void QmlStateGroupPrivate::updateAutoState()
{
    Q_Q(QmlStateGroup);
    if (!componentComplete)
        return;

    bool revert = false;
    for (int ii = 0; ii < states.count(); ++ii) {
        QmlState *state = states.at(ii);
        if (state->isWhenKnown()) {
            if (!state->name().isEmpty()) {
                if (state->when() && state->when()->value().toBool()) {
                    if (stateChangeDebug()) 
                        qWarning() << "Setting auto state due to:" 
                                   << state->when()->expression();
                    q->setState(state->name());
                    return;
                } else if (state->name() == currentState) {
                    revert = true;
                }
            }
        }
    }
    if (revert)
        q->setState(QString());
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
    if (!componentComplete)
        return;

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

    QmlState *newState = 0;
    for (int ii = 0; ii < states.count(); ++ii) {
        if (states.at(ii)->name() == currentState) {
            newState = states.at(ii);
            break;
        }
    }

    if (oldState == 0 || newState == 0) {
        if (!nullState) { nullState = new QmlState(q); }
        if (!oldState) oldState = nullState;
        if (!newState) newState = nullState;
    }

    newState->apply(q, transition, oldState);
}

void QmlStateGroup::componentComplete()
{
    Q_D(QmlStateGroup);
    d->updateAutoState();
    d->componentComplete = true;
    if (!d->currentState.isEmpty()) {
        QString cs = d->currentState;
        d->currentState = QString();
        d->setCurrentStateInternal(cs, true);
    }
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

QT_END_NAMESPACE
