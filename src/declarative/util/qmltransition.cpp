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

#include "qmlstate_p.h"
#include "qmlstategroup_p.h"
#include "qmlstate_p_p.h"
#include "qmlstateoperations_p.h"
#include "qmlanimation_p.h"
#include "qmlanimation_p_p.h"
#include "qmltransitionmanager_p_p.h"

#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Transition QmlTransition
    \brief The Transition element defines animated transitions that occur on state changes.

    \sa {qmlstates}{States}, {state-transitions}{Transitions}
*/

/*!
    \internal
    \class QmlTransition
    \brief The QmlTransition class allows you to define animated transitions that occur on state changes.

    \ingroup group_states
*/

//ParallelAnimationWrapperallows us to do a "callback" when the animation finishes, rather than connecting
//and disconnecting signals and slots frequently
class ParallelAnimationWrapper : public QParallelAnimationGroup
{
    Q_OBJECT
public:
    ParallelAnimationWrapper(QObject *parent = 0) : QParallelAnimationGroup(parent) {}
    QmlTransitionPrivate *trans;
protected:
    virtual void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState);
};

class QmlTransitionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlTransition)
public:
    QmlTransitionPrivate() 
    : fromState(QLatin1String("*")), toState(QLatin1String("*")), 
      reversed(false), reversible(false), endState(0)
    {
        group.trans = this;
    }

    QString fromState;
    QString toState;
    bool reversed;
    bool reversible;
    ParallelAnimationWrapper group;
    QmlTransitionManager *endState;

    void complete()
    {
        endState->complete();
    }
    static void append_animation(QmlListProperty<QmlAbstractAnimation> *list, QmlAbstractAnimation *a);
    QList<QmlAbstractAnimation *> animations;
};

void QmlTransitionPrivate::append_animation(QmlListProperty<QmlAbstractAnimation> *list, QmlAbstractAnimation *a)
{
    QmlTransition *q = static_cast<QmlTransition *>(list->object);
    q->d_func()->animations.append(a);
    q->d_func()->group.addAnimation(a->qtAnimation());
    a->setDisableUserControl();
}

void ParallelAnimationWrapper::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
    QParallelAnimationGroup::updateState(newState, oldState);
    if (newState == Stopped &&
        ((direction() == QAbstractAnimation::Forward && currentLoopTime() == duration()) ||
         (direction() == QAbstractAnimation::Backward && currentLoopTime() == 0)))
    {
        trans->complete();
    }
}



QmlTransition::QmlTransition(QObject *parent)
    : QObject(*(new QmlTransitionPrivate), parent)
{
}

QmlTransition::~QmlTransition()
{
}

void QmlTransition::stop()
{
    Q_D(QmlTransition);
    d->group.stop();
}

void QmlTransition::setReversed(bool r)
{
    Q_D(QmlTransition);
    d->reversed = r;
}

void QmlTransition::prepare(QmlStateOperation::ActionList &actions,
                            QList<QmlMetaProperty> &after,
                            QmlTransitionManager *endState)
{
    Q_D(QmlTransition);

    qmlExecuteDeferred(this);

    if (d->reversed) {
        for (int ii = d->animations.count() - 1; ii >= 0; --ii) {
            d->animations.at(ii)->transition(actions, after, QmlAbstractAnimation::Backward);
        }
    } else {
        for (int ii = 0; ii < d->animations.count(); ++ii) {
            d->animations.at(ii)->transition(actions, after, QmlAbstractAnimation::Forward);
        }
    }

    d->endState = endState;
    d->group.setDirection(d->reversed ? QAbstractAnimation::Backward : QAbstractAnimation::Forward);
    d->group.start();
}

/*!
    \qmlproperty string Transition::from
    \qmlproperty string Transition::to
    These properties are selectors indicating which state changes should trigger the transition.

    from is used in conjunction with to to determine when a transition should
    be applied. By default from and to are both "*" (any state). In the following example,
    the transition is applied when changing from state1 to state2.
    \code
    Transition {
        from: "state1"
        to: "state2"
        ...
    }
    \endcode
*/
QString QmlTransition::fromState() const
{
    Q_D(const QmlTransition);
    return d->fromState;
}

void QmlTransition::setFromState(const QString &f)
{
    Q_D(QmlTransition);
    d->fromState = f;
}

/*!
    \qmlproperty bool Transition::reversible
    This property holds whether the transition should be automatically reversed when the conditions that triggered this transition are reversed.

    The default value is false.
*/
bool QmlTransition::reversible() const
{
    Q_D(const QmlTransition);
    return d->reversible;
}

void QmlTransition::setReversible(bool r)
{
    Q_D(QmlTransition);
    d->reversible = r;
}

QString QmlTransition::toState() const
{
    Q_D(const QmlTransition);
    return d->toState;
}

void QmlTransition::setToState(const QString &t)
{
    Q_D(QmlTransition);
    d->toState = t;
}

/*!
    \qmlproperty list<Animation> Transition::animations
    \default
    This property holds a list of the animations to be run for this transition.

    The top-level animations are run in parallel. To run them sequentially,
    you can create a single SequentialAnimation which contains all the animations,
    and assign that to animations the animations property.
    \default
*/
QmlListProperty<QmlAbstractAnimation> QmlTransition::animations()
{
    Q_D(QmlTransition);
    return QmlListProperty<QmlAbstractAnimation>(this, &d->animations, QmlTransitionPrivate::append_animation);
}

QT_END_NAMESPACE

#include <qmltransition.moc>
