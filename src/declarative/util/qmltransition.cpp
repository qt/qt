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

#include "qmlstate.h"
#include "qmlstategroup.h"
#include "qmlstate_p.h"
#include "qmlstateoperations.h"
#include "qmlanimation.h"
#include "qmlanimation_p.h"
#include "qmltransitionmanager_p.h"
#include <QParallelAnimationGroup>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass Transition QmlTransition
    \brief The Transition element defines animated transitions that occur on state changes.

    \sa {states-transitions}{States and Transitions}
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
    ParallelAnimationWrapper(QObject *parent) : QParallelAnimationGroup(parent) {}
    QmlTransitionPrivate *trans;
protected:
    virtual void updateState(QAbstractAnimation::State oldState, QAbstractAnimation::State newState);
};

class QmlTransitionPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlTransition)
public:
    QmlTransitionPrivate() : fromState(QLatin1String("*")), toState(QLatin1String("*"))
    , reversed(false), reversible(false), group(0), endState(0)
    {
        operations.parent = this;
    }

    QString fromState;
    QString toState;
    bool reversed;
    bool reversible;
    ParallelAnimationWrapper *group;
    QmlTransitionManager *endState;

    void init()
    {
        Q_Q(QmlTransition);
        group = new ParallelAnimationWrapper(q);
        group->trans = this;
    }

    void complete()
    {
        endState->complete();
    }

    class AnimationList : public QmlConcreteList<QmlAbstractAnimation *>
    {
    public:
        AnimationList() : parent(0) {}
        virtual void append(QmlAbstractAnimation *a);
        virtual void clear() {  QmlConcreteList<QmlAbstractAnimation *>::clear(); } //XXX

        QmlTransitionPrivate *parent;
    };
    AnimationList operations;
};

void QmlTransitionPrivate::AnimationList::append(QmlAbstractAnimation *a)
{
    QmlConcreteList<QmlAbstractAnimation *>::append(a);
    parent->group->addAnimation(a->qtAnimation());
}

void ParallelAnimationWrapper::updateState(QAbstractAnimation::State oldState, QAbstractAnimation::State newState)
{
    QParallelAnimationGroup::updateState(oldState, newState);
    //XXX not 100% guaranteed to be at end (if there are many zero duration animations at the end)?
    if (newState == Stopped &&
        ((direction() == QAbstractAnimation::Forward && currentTime() == duration()) ||
         (direction() == QAbstractAnimation::Backward && currentTime() == 0)))
    {
        trans->complete();
    }
}


QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Transition,QmlTransition)
QmlTransition::QmlTransition(QObject *parent)
    : QObject(*(new QmlTransitionPrivate), parent)
{
    Q_D(QmlTransition);
    d->init();
}

QmlTransition::~QmlTransition()
{
}

void QmlTransition::stop()
{
    Q_D(QmlTransition);
    d->group->stop();
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

    if (d->reversed) {
        for (int ii = d->operations.count() - 1; ii >= 0; --ii) {
            d->operations.at(ii)->transition(actions, after, QmlAbstractAnimation::Backward);
        }
    } else {
        for (int ii = 0; ii < d->operations.count(); ++ii) {
            d->operations.at(ii)->transition(actions, after, QmlAbstractAnimation::Forward);
        }
    }

    d->endState = endState;
    d->group->setDirection(d->reversed ? QAbstractAnimation::Backward : QAbstractAnimation::Forward);
    d->group->start();
}

/*!
    \qmlproperty string Transition::fromState
    \qmlproperty string Transition::toState
    These properties are selectors indicating which state changes should trigger the transition.

    fromState is used in conjunction with toState to determine when a transition should
    be applied. By default fromState and toState are both "*" (any state). In the following example,
    the transition is applied when changing from state1 to state2.
    \code
    Transition {
        fromState: "state1"
        toState: "state2"
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
    \qmlproperty list<Animation> Transition::operations
    This property holds a list of the animations to be run for this transition.

    The top-level animations in operations are run in parallel.
    To run them sequentially, you can create a single SequentialAnimation
    which contains all the animations, and assign that to operations.
    \default
*/
QmlList<QmlAbstractAnimation *>* QmlTransition::operations()
{
    Q_D(QmlTransition);
    return &d->operations;
}

QT_END_NAMESPACE

#include "qmltransition.moc"
