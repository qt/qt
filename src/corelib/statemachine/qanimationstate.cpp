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

#ifndef QT_NO_ANIMATION

#include "qanimationstate.h"
#include "qparallelanimationgroup.h"
#include "qabstracttransition.h"
#include "qabstracttransition_p.h"
#include "qstatefinishedtransition.h"
#include "qsignaltransition.h"
#include "qpropertyanimation.h"
#include "qstatemachine.h"
#include "qstatemachine_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"
#include "qstate.h"
#include "qstate_p.h"
#include "qfinalstate.h"
#include "qsignaltransition_p.h"
#ifdef QT_STATEMACHINE_SOLUTION
#include "qvariantanimation_p.h"
#else
#include "private/qvariantanimation_p.h"
#endif
#include "qpauseanimation.h"

#include <QtCore/qhash.h>
#include <QtCore/qstack.h>
#include <QtCore/qlist.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtCore/qbitarray.h>

QT_BEGIN_NAMESPACE

/*!
  \class QAnimationState
  \brief The QAnimationState class provides a state that plays one or more animations.
  \ingroup statemachine
  \preliminary

  QAnimationState is part of \l{The State Machine Framework}.

  The addAnimation() function adds an animation to be played by the state.

  When the state is entered, it will call each of the animations' start() functions.
  When the animation is finished, a QStateFinishedEvent is posted; you can
  use the QStateFinishedTransition class to associate a transition with this
  event.

  \code
    QPushButton button;
    QPropertyAnimation animation(&button, "geometry");
    animation.setEndValue(QRect(100, 100, 400, 400));

    QStateMachine machine;
    QAnimationState *s1 = new QAnimationState(&animation, machine.rootState());
    QState *s2 = new QState(machine.rootState());
    s1->addFinishedTransition(s2);
  \endcode

  If the state is exited before the animation has finished, the animations will
  be stopped, and no event is generated.

  For convenience, the QState::addAnimatedTransition() functions can be used to set up the 
  animated transition between two states. 

  \section1 Initializing animations automatically
  QAnimationState will try to automatically initialize any QPropertyAnimation for which no 
  specific end value has been set. It will set the animation's end value based on actions in the 
  target state of the animation state's QStateFinishedTransition. 
  
  The only actions evaluated are the entry actions of the very first states entered after the 
  animation state has finished.
  
  QAnimationState will match its QPropertyAnimation objects with QStateSetPropertyAction objects 
  in the target states that manipulate the same property on the same object. The end values of the
  animations will be initialized to the values set by the actions.

  \code
    QPushButton button;
    QPropertyAnimation animation(&button, "geometry");

    QStateMachine machine;
    QAnimationState *s1 = new QAnimationState(&animation, machine.rootState());
    QState *s2 = new QState(machine.rootState());
    s2->setPropertyOnEntry(&button, "geometry", QRect(100, 100, 400, 400));

    s1->addFinishedTransition(s2);
  \endcode

  Specifically, QAnimationState will evaluate the actions set for the target state itself, and 
  also look in following locations:
  objects:
  \list
    \i If the state has children and is not parallel, actions set for its initial 
       state will be evaluated.
    \i If the state has children and is parallel, actions set for any of its children will be
       evaluated.
  \endlist

  Children of the target state will be evaluated recursively. 

  \section1 Animated restoring of properties 

  When a state has restore policy QActionState::RestoreProperties, any
  property that is set in the state using the QStateSetPropertyAction will
  potentially be restored to its original value later on. When regular,
  unanimated transitions are used, the properties will be restored when the
  state machine enters one or more states that do not explicitly set the
  property.

  When QAnimationState is used, it will restore the property with an
  animation. Rather than have the state machine restore the properties as it
  enters the target state, they will be restored by the QAnimationState in
  parallel to the regular animations that have been added to the state.

  If no animation has been added to the state, only the restore animations
  will be played.

  The animations used to restore the properties are QPropertyAnimations with
  with the default easing curve and duration.

  \sa QActionState::RestorePolicy, QPropertyAnimation, QStateSetPropertyAction, 
      QState::addAnimatedTransition()      

*/

namespace {

class AnimatingState : public QState
{
public:
    AnimatingState(QState *parent)
        : QState(parent) {}
protected:
    void onEntry() {}
    void onExit() {}
};

class AnimationFinishedState : public QFinalState
{
public:
    AnimationFinishedState(QState *parent)
        : QFinalState(parent)
    {
    }   

protected:
    void onEntry() {}
    void onExit() {}
};

class AnimationFinishedTransition: public QSignalTransition
{
public:
    AnimationFinishedTransition(QAbstractAnimation *animation,
                                QAnimationStatePrivate *animationState_d,
                                QAbstractState *target)
        : QSignalTransition(animation, SIGNAL(finished()), QList<QAbstractState*>() << target),
          m_animationState_d(animationState_d)
    {
    }

    virtual bool eventTest(QEvent *) const;

private:
    QAnimationStatePrivate *m_animationState_d;
};

} // namespace

class QAnimationStatePrivate : public QStatePrivate
{
    Q_DECLARE_PUBLIC(QAnimationState)

public:
    typedef QStateMachinePrivate::RestorableId RestorableId;

    QAnimationStatePrivate();
    void init();
    QAbstractTransition *finishedTransition() const;

    void initializeAnimation(const QList<QStateAction*> &actions, QActionState::RestorePolicy restorePolicy);
    void initializeAnimation(const QList<QAbstractState*> &targets);
    void initializeAnimationFromAction(QAbstractAnimation *anim, 
                                       QStateAction *action,
                                       QActionState::RestorePolicy restorePolicy);

    void restoreAnimations();

    void addAnimation(QAbstractAnimation *animation, QList<QAbstractAnimation*> &list);
    void removeAnimation(QAbstractAnimation *animation, QList<QAbstractAnimation*> &list);

    QList<QAbstractAnimation *> animations;
    QList<QAbstractAnimation *> restorationAnimations;
    QList<QPropertyAnimation *> resetEndValues;
    QState *animatingState;
    QFinalState *finishedState;
    QTimer *timer;

    uint initializeAnimationFromTargetStates        :  1;
    uint initializeAnimationFromRestorableVariables :  1;
    uint reserved                                   : 30;       
    
    QHash<RestorableId, QVariant> pendingRestorables;
};

// implement here because it requires the definition of QAnimationStatePrivate.
namespace {
    bool AnimationFinishedTransition::eventTest(QEvent *e) const
    {    
        if (!QSignalTransition::eventTest(e))
            return false;

        QList<QAbstractAnimation *> animations = m_animationState_d->animations;
        QList<QAbstractAnimation *> restorationAnimations = m_animationState_d->restorationAnimations;

        for (int i=0; i<animations.size(); ++i) {
            if (animations.at(i)->state() != QAbstractAnimation::Stopped)
                return false;
        }

        for (int i=0; i<restorationAnimations.size(); ++i) {
            if (restorationAnimations.at(i)->state() != QAbstractAnimation::Stopped)
                return false;
        }

        return true;
    }
}

QAnimationStatePrivate::QAnimationStatePrivate()
{
}

void QAnimationStatePrivate::init()
{
    Q_Q(QAnimationState);

    // ### make it a configurable property, as it is highly magical
    initializeAnimationFromTargetStates = true;
    initializeAnimationFromRestorableVariables = true;

    animatingState = new AnimatingState(q);
    q->setInitialState(animatingState);
    finishedState = new AnimationFinishedState(q);    

    timer = 0;
}

void QAnimationStatePrivate::addAnimation(QAbstractAnimation *animation, 
                                          QList<QAbstractAnimation*> &list)
{
    if (animation != 0 && !list.contains(animation)) {
        list.append(animation);
        AnimationFinishedTransition *transition = new AnimationFinishedTransition(animation, this, finishedState);
        animatingState->addTransition(transition);
    }
}

void QAnimationStatePrivate::removeAnimation(QAbstractAnimation *animation,
                                             QList<QAbstractAnimation*> &list)
{
    if (animation != 0 && list.contains(animation)) {
        QStatePrivate *state_d = QStatePrivate::get(animatingState); 
        QList<QAbstractTransition *> transitions = state_d->transitions();
        Q_ASSERT(transitions.size() > 0);
        for (int i=0; i<transitions.size(); ++i) {
            QSignalTransition *transition = qobject_cast<QSignalTransition *>(transitions.at(i));

            if (transition != 0) {
                QSignalTransitionPrivate *transition_p = QSignalTransitionPrivate::get(transition);
                if (transition_p->sender == animation) {
                    delete transition;
                    break;
                }
            }            
        }

        list.removeAll(animation);
    }
}

/*!
  \internal

  Returns a transition from this state that is triggered when this state is
  finished, or 0 if there is no such transition.
*/
QAbstractTransition *QAnimationStatePrivate::finishedTransition() const
{
    QList<QAbstractTransition*> trans = transitions();
    for (int i = 0; i < trans.size(); ++i) {
        QAbstractTransition *t = trans.at(i);
        if (QStateFinishedTransition *sft = qobject_cast<QStateFinishedTransition*>(t))
            return sft;
    }
    return 0;
}

void QAnimationStatePrivate::initializeAnimationFromAction(QAbstractAnimation *abstractAnimation, 
                                                           QStateAction *action,
                                                           QActionState::RestorePolicy restorePolicy)
{
    QAnimationGroup *group = qobject_cast<QAnimationGroup*>(abstractAnimation);
    if (group) {
        for (int i = 0; i < group->animationCount(); ++i) {
            QAbstractAnimation *animationChild = group->animationAt(i);
            initializeAnimationFromAction(animationChild, action, restorePolicy);
        }
    } else { 
        QPropertyAnimation *animation = qobject_cast<QPropertyAnimation *>(abstractAnimation);
        QStateSetPropertyAction *propertyAction = qobject_cast<QStateSetPropertyAction*>(action);
        if (propertyAction != 0 
            && animation != 0 
            && propertyAction->targetObject() == animation->targetObject()
            && propertyAction->propertyName() == animation->propertyName()) {

            if (!animation->startValue().isValid()) {                    
                QByteArray propertyName = animation->propertyName();
                QVariant currentValue = animation->targetObject()->property(propertyName);

                QVariantAnimationPrivate::get(animation)->setDefaultStartValue(currentValue);
            }

            // Only change end value if it is undefined
            if (!animation->endValue().isValid()) {
                QStateMachinePrivate *machine_d = QStateMachinePrivate::get(machine());
                if (restorePolicy == QActionState::RestoreProperties)
                    machine_d->registerRestorable(animation);          

                RestorableId id(animation->targetObject(), animation->propertyName());
                pendingRestorables.remove(id);

                animation->setEndValue(propertyAction->value());
                resetEndValues.append(animation);
            }
        }
    }
}

void QAnimationStatePrivate::initializeAnimation(const QList<QStateAction*> &actions,
                                                 QActionState::RestorePolicy restorePolicy)
{

    for (int i = 0; i < actions.size(); ++i) {
        QStateAction *act = actions.at(i);
        
        for (int j=0; j<animations.size(); ++j)
            initializeAnimationFromAction(animations.at(j), act, restorePolicy);
    }


}

void QAnimationStatePrivate::initializeAnimation(const QList<QAbstractState*> &targets)
{
    // ### consider resulting action order, and how to resolve conflicts (two actions that set the same property)
    for (int i = 0; i < targets.size(); ++i) {
        QActionState *s = qobject_cast<QActionState*>(targets.at(i));
        if (s != 0) {
            QActionState::RestorePolicy restorePolicy = s->restorePolicy();
            if (restorePolicy == QActionState::GlobalRestorePolicy)
                restorePolicy = machine()->globalRestorePolicy();
            initializeAnimation(QActionStatePrivate::get(s)->entryActions(), restorePolicy);
        }

        if (QStateMachinePrivate::isParallel(s)) {
            initializeAnimation(QStatePrivate::get(qobject_cast<QState*>(s))->childStates());
        } else if (QStateMachinePrivate::isCompound(s)) {
            initializeAnimation(QList<QAbstractState*>() << qobject_cast<QState*>(s)->initialState());
        }
    }
}

void QAnimationStatePrivate::restoreAnimations()
{
    QStateMachinePrivate *machine_d = QStateMachinePrivate::get(machine());

    QHash<RestorableId, QVariant>::const_iterator it;
    for (it=pendingRestorables.constBegin(); it != pendingRestorables.constEnd(); ++it) {
        QPropertyAnimation *animation = machine_d->registeredRestorableAnimations.value(it.key());
        if (animation == 0)
            continue;

        // ### Check if this works
        // animation->setDirection(QAbstractAnimation::Backward);

        QPropertyAnimation *clonedAnimation = new QPropertyAnimation(animation->targetObject(),
                                                                     animation->propertyName());
        clonedAnimation->setEasingCurve(animation->easingCurve());
        clonedAnimation->setEndValue(it.value());
        
        addAnimation(clonedAnimation, restorationAnimations);
    }

    pendingRestorables.clear();
}


/*!
  Constructs a new QAnimationState object with the given \a animation and \a
  parent state 
*/
QAnimationState::QAnimationState(QAbstractAnimation *animation, QState *parent)
    : QState(*new QAnimationStatePrivate, parent)
{
    Q_D(QAnimationState);
    d->init();

    if (animation != 0)
        addAnimation(animation);
}

/*!
  Constructs a new QAnimationState object with the given \a parent state.
*/
QAnimationState::QAnimationState(QState *parent)
    : QState(*new QAnimationStatePrivate, parent)
{
    Q_D(QAnimationState);
    d->init();
}

/*!
  Destroys this QAnimationState.
*/
QAnimationState::~QAnimationState()
{
}

/*!
  Returns the number of animations added to this QAnimationState.
*/
int QAnimationState::animationCount() const
{
    Q_D(const QAnimationState);
    return d->animations.size();
}

/*!
  Returns the animation associated with this QAnimationState at index \a i.
*/
QAbstractAnimation *QAnimationState::animationAt(int i) const
{
    Q_D(const QAnimationState);
    return d->animations.at(i);
}

/*!
  Adds \a animation to this QAnimationState.
*/
void QAnimationState::addAnimation(QAbstractAnimation *animation)
{    
    Q_D(QAnimationState);
    if (animation == 0) {
        qWarning("QAnimationState::addAnimation: Cannot add null animation");
        return;
    }

    d->addAnimation(animation, d->animations);
}

/*!
    Removes \a animation from this QAnimationState.
*/
void QAnimationState::removeAnimation(QAbstractAnimation *animation)
{
    Q_D(QAnimationState);
    d->removeAnimation(animation, d->animations);
}

/*!
  \reimp
*/
void QAnimationState::onEntry()
{
    Q_D(QAnimationState);

    {
        QStateMachinePrivate *machine_d = QStateMachinePrivate::get(d->machine());
        d->pendingRestorables = machine_d->registeredRestorables;
    }

    if (d->initializeAnimationFromTargetStates) {            
        if (QAbstractTransition *t = d->finishedTransition())
            d->initializeAnimation(t->targetStates());
    }

    if (d->initializeAnimationFromRestorableVariables)
        d->restoreAnimations();    

    for (int i=0; i<d->animations.size(); ++i) 
        d->animations.at(i)->start();

    for (int i=0; i<d->restorationAnimations.size(); ++i)
        d->restorationAnimations.at(i)->start();

    // If there are no animations playing, we use a 0 timer to trigger the transition
    // to the final state
    if (d->animations.size()+d->restorationAnimations.size() == 0) {
        if (d->timer == 0) {
            d->timer = new QTimer(this);
            d->timer->setInterval(0);
            d->timer->setSingleShot(true);

            d->animatingState->addTransition(d->timer, SIGNAL(timeout()), d->finishedState);
        }

        d->timer->start();
    }

}

/*!
  \reimp
*/
void QAnimationState::onExit()
{
    Q_D(QAnimationState);

    for (int i=0; i<d->animations.size(); ++i) {
        if (d->animations.at(i)->state() != QAbstractAnimation::Stopped)
            d->animations.at(i)->stop();    
    }

    QList<QAbstractAnimation *> restorationAnimations = d->restorationAnimations;
    for (int i=0; i<restorationAnimations.size(); ++i) {
        QAbstractAnimation *restorationAnimation = restorationAnimations.at(i);
        if (restorationAnimation->state() != QAbstractAnimation::Stopped) {
            restorationAnimation->stop();
            d->removeAnimation(restorationAnimation, d->restorationAnimations);

            // ###
            delete restorationAnimation; 
        } else {
            QPropertyAnimation *propertyAnimation = qobject_cast<QPropertyAnimation*>(restorationAnimation);
            if (propertyAnimation != 0) {
                QStateMachinePrivate *machine_d = QStateMachinePrivate::get(d->machine());
                machine_d->unregisterRestorable(propertyAnimation->targetObject(), 
                                                propertyAnimation->propertyName());
            }
        }
    }

    for (int i=0; i<d->resetEndValues.size(); ++i) 
        d->resetEndValues.at(i)->setEndValue(QVariant());

    if (d->timer != 0)
        d->timer->stop();
}

/*!
  \reimp
*/
bool QAnimationState::event(QEvent *e)
{
    return QState::event(e);
}

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION
