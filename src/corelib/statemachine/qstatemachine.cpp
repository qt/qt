/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qstatemachine.h"
#include "qstatemachine_p.h"
#include "qabstracttransition.h"
#include "qabstracttransition_p.h"
#include "qsignaltransition.h"
#include "qsignaltransition_p.h"
#include "qsignalevent.h"
#include "qsignaleventgenerator_p.h"
#include "qabstractstate.h"
#include "qabstractstate_p.h"
#include "qactionstate.h"
#include "qactionstate_p.h"
#include "qfinalstate.h"
#include "qhistorystate.h"
#include "qhistorystate_p.h"
#include "qstatefinishedevent.h"
#include "qstatefinishedtransition.h"
#include "qstate.h"
#include "qstate_p.h"
#include "qstateaction.h"
#include "qstateaction_p.h"
#ifndef QT_STATEMACHINE_SOLUTION
#include "private/qobject_p.h"
#include "private/qthread_p.h"
#endif

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
#include "qeventtransition.h"
#include "qeventtransition_p.h"
#include "qboundevent_p.h"
#endif

#ifndef QT_NO_ANIMATION
#include "qpropertyanimation.h"
#include "qanimationgroup.h"
# ifndef QT_STATEMACHINE_SOLUTION
# include <private/qvariantanimation_p.h>
# else
# include "qvariantanimation_p.h"
# endif
#endif

#include <QtCore/qmetaobject.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QStateMachine
  \reentrant

  \brief The QStateMachine class provides a hierarchical finite state machine.

  \ingroup statemachine

  The QStateMachine class provides a hierarchical finite state machine based
  on \l{Statecharts: A visual formalism for complex systems}{Statecharts}
  concepts and notation. QStateMachine is part of \l{The State Machine
  Framework}.

  A state machine manages a set of states (QAbstractState objects) and
  transitions (QAbstractTransition objects) between those states; the states
  and the transitions collectively define a state graph. Once a state graph
  has been defined, the state machine can execute it. QStateMachine's
  execution algorithm is based on the \l{State Chart XML: State Machine
  Notation for Control Abstraction}{State Chart XML (SCXML)} algorithm.

  The QState class provides a state that you can use to set properties and
  invoke methods on QObjects when the state is entered or exited. This is
  typically used in conjunction with \l{Signals and Slots}{signals}; the
  signals determine the flow of the state graph, whereas the states' property
  assigments and method invocations are the actions.

  Use the addState() function to add a state to the state machine;
  alternatively, pass the machine's rootState() to the state constructor.  Use
  the removeState() function to remove a state from the state machine.

  The following snippet shows a state machine that will finish when a button
  is clicked:

  \code
  QPushButton button;

  QStateMachine machine;
  QState *s1 = new QState();
  s1->assignProperty(&button, "text", "Click me");

  QFinalState *s2 = new QFinalState();
  s1->addTransition(&button, SIGNAL(clicked()), s2);

  machine.addState(s1);
  machine.addState(s2);
  machine.setInitialState(s1);
  machine.start();
  \endcode

  The setInitialState() function sets the state machine's initial state; this
  state is entered when the state machine is started.

  The start() function starts the state machine. The state machine executes
  asynchronously, i.e. you need to run an event loop in order for it to make
  progress. The started() signal is emitted when the state machine has entered
  the initial state.

  The state machine processes events and takes transitions until a top-level
  final state is entered; the state machine then emits the finished() signal.

  The stop() function stops the state machine. The stopped() signal is emitted
  when the state machine has stopped.

  The postEvent() function posts an event to the state machine. This is useful
  when you are using custom events to trigger transitions.

  The rootState() function returns the state machine's root state. All
  top-level states have the root state as their parent.

  \sa QAbstractState, QAbstractTransition
*/

/*!
    \property QStateMachine::rootState

    \brief the root state of this state machine
*/

/*!
    \property QStateMachine::initialState

    \brief the initial state of this state machine
*/

/*!
    \property QStateMachine::errorState

    \brief the error state of this state machine
*/

/*!
    \property QStateMachine::errorString

    \brief the error string of this state machine
*/

// #define QSTATEMACHINE_DEBUG

QStateMachinePrivate::QStateMachinePrivate()
{
    state = NotRunning;
    processing = false;
    processingScheduled = false;
    stop = false;
    error = QStateMachine::NoError;
    globalRestorePolicy = QAbstractState::DoNotRestoreProperties;
    rootState = 0;
    initialErrorStateForRoot = 0;
#ifndef QT_STATEMACHINE_SOLUTION
    signalEventGenerator = 0;
#endif
}

QStateMachinePrivate::~QStateMachinePrivate()
{
    qDeleteAll(internalEventQueue);
    qDeleteAll(externalEventQueue);
}

QStateMachinePrivate *QStateMachinePrivate::get(QStateMachine *q)
{
    if (q)
        return q->d_func();
    return 0;
}

static QEvent *cloneEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::None:
        return new QEvent(*e);
    case QEvent::Timer:
        return new QTimerEvent(*static_cast<QTimerEvent*>(e));
    default:
        Q_ASSERT_X(false, "cloneEvent()", "not implemented");
        break;
    }
    return 0;
}

const QStateMachinePrivate::Handler qt_kernel_statemachine_handler = {
    cloneEvent
};

const QStateMachinePrivate::Handler *QStateMachinePrivate::handler = &qt_kernel_statemachine_handler;

Q_CORE_EXPORT const QStateMachinePrivate::Handler *qcoreStateMachineHandler()
{
    return &qt_kernel_statemachine_handler;
}

bool QStateMachinePrivate::stateEntryLessThan(QAbstractState *s1, QAbstractState *s2)
{
    if (s1->parent() == s2->parent()) {
        return s1->children().indexOf(s1)
            < s2->children().indexOf(s2);
    } else if (isDescendantOf(s1, s2)) {
        return false;
    } else if (isDescendantOf(s2, s1)) {
        return true;
    } else {
        // ### fixme
        return s1 < s2;
    }
}

bool QStateMachinePrivate::stateExitLessThan(QAbstractState *s1, QAbstractState *s2)
{
    if (s1->parent() == s2->parent()) {
        return s1->children().indexOf(s1)
            < s2->children().indexOf(s2);
    } else if (isDescendantOf(s1, s2)) {
        return true;
    } else if (isDescendantOf(s2, s1)) {
        return false;
    } else {
        // ### fixme
        return s2 < s1;
    }
}

QState *QStateMachinePrivate::findLCA(const QList<QAbstractState*> &states)
{
    if (states.isEmpty())
        return 0;
    QList<QState*> ancestors = properAncestors(states.at(0), 0);
    for (int i = 0; i < ancestors.size(); ++i) {
        QState *anc = ancestors.at(i);
        bool ok = true;
        for (int j = states.size() - 1; (j > 0) && ok; --j) {
            const QAbstractState *s = states.at(j);
            if (!isDescendantOf(s, anc))
                ok = false;
        }
        if (ok)
            return anc;
    }
    return 0;
}

bool QStateMachinePrivate::isPreempted(const QAbstractState *s, const QSet<QAbstractTransition*> &transitions) const
{
    QSet<QAbstractTransition*>::const_iterator it;
    for (it = transitions.constBegin(); it != transitions.constEnd(); ++it) {
        QAbstractTransition *t = *it;
        QList<QAbstractState*> lst = t->targetStates();
        if (!lst.isEmpty()) {
            lst.prepend(t->sourceState());
            QAbstractState *lca = findLCA(lst);
            if (isDescendantOf(s, lca)) {
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q_func() << ":" << transitions << "preempts selection of a transition from"
                         << s << "because" << s << "is a descendant of" << lca;
#endif
                return true;
            }
        }
    }
    return false;
}

QSet<QAbstractTransition*> QStateMachinePrivate::selectTransitions(QEvent *event) const
{
    Q_Q(const QStateMachine);
    QSet<QAbstractTransition*> enabledTransitions;
    QSet<QAbstractState*>::const_iterator it;
    const_cast<QStateMachine*>(q)->beginSelectTransitions(event);
    for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
        QAbstractState *state = *it;
        if (!isAtomic(state))
            continue;
        if (isPreempted(state, enabledTransitions))
            continue;
        QList<QState*> lst = properAncestors(state, 0);
        if (QState *grp = qobject_cast<QState*>(state))
            lst.prepend(grp);
        bool found = false;
        for (int j = 0; (j < lst.size()) && !found; ++j) {
            QState *s = lst.at(j);
            QList<QAbstractTransition*> transitions = QStatePrivate::get(s)->transitions();
            for (int k = 0; k < transitions.size(); ++k) {
                QAbstractTransition *t = transitions.at(k);
                if (QAbstractTransitionPrivate::get(t)->callEventTest(event)) {
#ifdef QSTATEMACHINE_DEBUG
                    qDebug() << q << ": selecting transition" << t;
#endif
                    enabledTransitions.insert(t);
                    found = true;
                    break;
                }
            }
        }
    }
    const_cast<QStateMachine*>(q)->endSelectTransitions(event);
    return enabledTransitions;
}

void QStateMachinePrivate::microstep(const QList<QAbstractTransition*> &enabledTransitions)
{
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": begin microstep( enabledTransitions:" << enabledTransitions << ")";
    qDebug() << q_func() << ": configuration before exiting states:" << configuration;
#endif
    exitStates(enabledTransitions);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": configuration after exiting states:" << configuration;
#endif
    executeTransitionContent(enabledTransitions);
    QList<QAbstractState*> enteredStates = enterStates(enabledTransitions);
    applyProperties(enabledTransitions, enteredStates);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": configuration after entering states:" << configuration;
    qDebug() << q_func() << ": end microstep";
#endif
}

QList<QAbstractState*> QStateMachinePrivate::exitStates(const QList<QAbstractTransition*> &enabledTransitions)
{
//    qDebug() << "exitStates(" << enabledTransitions << ")";
    QSet<QAbstractState*> statesToExit;
//    QSet<QAbstractState*> statesToSnapshot;
    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QAbstractTransition *t = enabledTransitions.at(i);
        QList<QAbstractState*> lst = t->targetStates();
        if (lst.isEmpty())
            continue;
        lst.prepend(t->sourceState());
        QAbstractState *lca = findLCA(lst);
        {
            QSet<QAbstractState*>::const_iterator it;
            for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
                QAbstractState *s = *it;
                if (isDescendantOf(s, lca))
                    statesToExit.insert(s);
            }
        }
    }
    QList<QAbstractState*> statesToExit_sorted = statesToExit.toList();
    qSort(statesToExit_sorted.begin(), statesToExit_sorted.end(), stateExitLessThan);
    for (int i = 0; i < statesToExit_sorted.size(); ++i) {
        QAbstractState *s = statesToExit_sorted.at(i);
        if (QState *grp = qobject_cast<QState*>(s)) {
            QList<QHistoryState*> hlst = QStatePrivate::get(grp)->historyStates();
            for (int j = 0; j < hlst.size(); ++j) {
                QHistoryState *h = hlst.at(j);
                QHistoryStatePrivate::get(h)->configuration.clear();
                QSet<QAbstractState*>::const_iterator it;
                for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
                    QAbstractState *s0 = *it;
                    if (QHistoryStatePrivate::get(h)->historyType == QState::DeepHistory) {
                        if (isAtomic(s0) && isDescendantOf(s0, s))
                            QHistoryStatePrivate::get(h)->configuration.append(s0);
                    } else if (s0->parentState() == s) {
                        QHistoryStatePrivate::get(h)->configuration.append(s0);
                    }
                }
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q_func() << ": recorded" << ((QHistoryStatePrivate::get(h)->historyType == QState::DeepHistory) ? "deep" : "shallow")
                         << "history for" << s << "in" << h << ":" << QHistoryStatePrivate::get(h)->configuration;
#endif
            }
        }
    }
    for (int i = 0; i < statesToExit_sorted.size(); ++i) {
        QAbstractState *s = statesToExit_sorted.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q_func() << ": exiting" << s;
#endif
        QAbstractStatePrivate::get(s)->callOnExit();
        configuration.remove(s);
    }
    return statesToExit_sorted;
}

void QStateMachinePrivate::executeTransitionContent(const QList<QAbstractTransition*> &enabledTransitions)
{
    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QAbstractTransition *t = enabledTransitions.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q_func() << ": triggering" << t;
#endif
        QAbstractTransitionPrivate::get(t)->callOnTransition();
    }
}

QList<QAbstractState*> QStateMachinePrivate::enterStates(const QList<QAbstractTransition*> &enabledTransitions)
{
#ifdef QSTATEMACHINE_DEBUG
    Q_Q(QStateMachine);
#endif
//    qDebug() << "enterStates(" << enabledTransitions << ")";
    QSet<QAbstractState*> statesToEnter;
    QSet<QAbstractState*> statesForDefaultEntry;

    for (int i = 0; i < enabledTransitions.size(); ++i) {
        QAbstractTransition *t = enabledTransitions.at(i);
        QList<QAbstractState*> lst = t->targetStates();
        if (lst.isEmpty())
            continue;
        lst.prepend(t->sourceState());
        QState *lca = findLCA(lst);
        for (int j = 1; j < lst.size(); ++j) {
            QAbstractState *s = lst.at(j);
            if (QHistoryState *h = qobject_cast<QHistoryState*>(s)) {
                QList<QAbstractState*> hconf = QHistoryStatePrivate::get(h)->configuration;
                if (!hconf.isEmpty()) {
                    for (int k = 0; k < hconf.size(); ++k) {
                        QAbstractState *s0 = hconf.at(k);
                        addStatesToEnter(s0, lca, statesToEnter, statesForDefaultEntry);
                    }
#ifdef QSTATEMACHINE_DEBUG
                    qDebug() << q << ": restoring"
                            << ((QHistoryStatePrivate::get(h)->historyType == QState::DeepHistory) ? "deep" : "shallow")
                            << "history from" << s << ":" << hconf;
#endif
                } else {
                    QList<QAbstractState*> hlst;
                    if (QHistoryStatePrivate::get(h)->defaultState)
                        hlst.append(QHistoryStatePrivate::get(h)->defaultState);
                    if (hlst.isEmpty()) {
                        setError(QStateMachine::NoDefaultStateInHistoryState, h);
                    } else {
                        for (int k = 0; k < hlst.size(); ++k) {
                            QAbstractState *s0 = hlst.at(k);
                            addStatesToEnter(s0, lca, statesToEnter, statesForDefaultEntry);
                        }
#ifdef QSTATEMACHINE_DEBUG
                        qDebug() << q << ": initial history targets for" << s << ":" << hlst;                    
#endif
                    }
                }
            } else {
                addStatesToEnter(s, lca, statesToEnter, statesForDefaultEntry);
            }
        }
    }

    // Did an error occur while selecting transitions? Then we enter the error state.
    if (!pendingErrorStates.isEmpty()) {
        statesToEnter.clear();
        statesToEnter = pendingErrorStates;
        statesForDefaultEntry = pendingErrorStatesForDefaultEntry;
        pendingErrorStates.clear();
        pendingErrorStatesForDefaultEntry.clear();
    }

    QList<QAbstractState*> statesToEnter_sorted = statesToEnter.toList();
    qSort(statesToEnter_sorted.begin(), statesToEnter_sorted.end(), stateEntryLessThan);

    for (int i = 0; i < statesToEnter_sorted.size(); ++i) {
        QAbstractState *s = statesToEnter_sorted.at(i);
#ifdef QSTATEMACHINE_DEBUG
        qDebug() << q << ": entering" << s;
#endif
        configuration.insert(s);
        registerTransitions(s);
        QAbstractStatePrivate::get(s)->callOnEntry();
        if (statesForDefaultEntry.contains(s)) {
            // ### executeContent(s.initial.transition.children())
        }
        if (isFinal(s)) {
            QState *parent = s->parentState();
            if (parent) {
                QState *grandparent = parent->parentState();
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q << ": posting finished event for" << parent;
#endif
                internalEventQueue.append(new QStateFinishedEvent(parent));
                if (grandparent && isParallel(grandparent)) {
                    bool allChildStatesFinal = true;
                    QList<QAbstractState*> childStates = QStatePrivate::get(grandparent)->childStates();
                    for (int j = 0; j < childStates.size(); ++j) {
                        QAbstractState *cs = childStates.at(j);
                        if (!isInFinalState(cs)) {
                            allChildStatesFinal = false;
                            break;
                        }
                    }
                    if (allChildStatesFinal) {
#ifdef QSTATEMACHINE_DEBUG
                        qDebug() << q << ": posting finished event for" << grandparent;
#endif
                        internalEventQueue.append(new QStateFinishedEvent(grandparent));
                    }
                }
            }
        }
    }
    {
        QSet<QAbstractState*>::const_iterator it;
        for (it = configuration.constBegin(); it != configuration.constEnd(); ++it) {
            if (isFinal(*it) && (*it)->parentState() == rootState) {
                processing = false;
                stopProcessingReason = Finished;
                break;
            }
        }
    }
//    qDebug() << "configuration:" << configuration.toList();
    return statesToEnter_sorted;
}

void QStateMachinePrivate::addStatesToEnter(QAbstractState *s, QState *root,
                                            QSet<QAbstractState*> &statesToEnter,
                                            QSet<QAbstractState*> &statesForDefaultEntry)
{
    statesToEnter.insert(s);
    if (isParallel(s)) {
        QState *grp = qobject_cast<QState*>(s);
        QList<QAbstractState*> lst = QStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QAbstractState *child = lst.at(i);
            addStatesToEnter(child, grp, statesToEnter, statesForDefaultEntry);
        }
    } else if (isCompound(s)) {
        statesForDefaultEntry.insert(s);
        QState *grp = qobject_cast<QState*>(s);
        QAbstractState *initial = grp->initialState();
        if (initial != 0) {
            addStatesToEnter(initial, grp, statesToEnter, statesForDefaultEntry);
        } else {
            setError(QStateMachine::NoInitialStateError, grp);
            return;
        }
    }
    QList<QState*> ancs = properAncestors(s, root);
    for (int i = 0; i < ancs.size(); ++i) {
        QState *anc = ancs.at(i);
        if (!anc->parentState())
            continue;
        statesToEnter.insert(anc);
        if (isParallel(anc)) {
            QList<QAbstractState*> lst = QStatePrivate::get(anc)->childStates();
            for (int j = 0; j < lst.size(); ++j) {
                QAbstractState *child = lst.at(j);
                bool hasDescendantInList = false;
                QSet<QAbstractState*>::const_iterator it;
                for (it = statesToEnter.constBegin(); it != statesToEnter.constEnd(); ++it) {
                    if (isDescendantOf(*it, child)) {
                        hasDescendantInList = true;
                        break;
                    }
                }
                if (!hasDescendantInList)
                    addStatesToEnter(child, anc, statesToEnter, statesForDefaultEntry);
            }
        }
    }
}

void QStateMachinePrivate::applyProperties(const QList<QAbstractTransition*> &transitionList,
                                           const QList<QAbstractState*> &enteredStates)
{
#ifndef QT_NO_ANIMATION
    Q_Q(QStateMachine);
    // Gracefully terminate playing animations.
    for (int i = 0; i < playingAnimations.size(); ++i)
        playingAnimations.at(i)->stop();
    playingAnimations.clear();
    for (int i = 0; i < resetEndValues.size(); ++i)
        qobject_cast<QVariantAnimation*>(resetEndValues.at(i))->setEndValue(QVariant()); // ### generalize
    resetEndValues.clear();

    // Find the animations to use for the state change.
    QList<QAbstractAnimation*> selectedAnimations;
    for (int i = 0; i < transitionList.size(); ++i)
        selectedAnimations << transitionList.at(i)->animations();
#else
    Q_UNUSED(transitionList);
#endif

    // Process the SetProperty definitions of the entered states.
    QList<QPropertyAssignment> propertyAssignments;
    QHash<RestorableId, QVariant> pendingRestorables = registeredRestorables;
    for (int i = 0; i < enteredStates.size(); ++i) {
        QAbstractState *s = enteredStates.at(i);

        QAbstractState::RestorePolicy restorePolicy = s->restorePolicy();
        if (restorePolicy == QAbstractState::GlobalRestorePolicy)
            restorePolicy = globalRestorePolicy;

        QList<QPropertyAssignment> assignments = QAbstractStatePrivate::get(s)->propertyAssignments;
        for (int j = 0; j < assignments.size(); ++j) {
            const QPropertyAssignment &assn = assignments.at(j);
            if (restorePolicy == QAbstractState::RestoreProperties) {
                registerRestorable(assn.object, assn.propertyName);
            }
            pendingRestorables.remove(RestorableId(assn.object, assn.propertyName));
            propertyAssignments.append(assn);
        }
    }
    propertyAssignments << restorablesToPropertyList(pendingRestorables);

#ifndef QT_NO_ANIMATION
    // Set the animated properties that did not finish animating and that are not
    // set in the new state.
    for (int i = 0; i < propertiesForAnimations.size(); ++i) {
        QPropertyAssignment assn = propertiesForAnimations.at(i).second;
        bool found = false;
        for (int j = 0; j < propertyAssignments.size(); ++j) {
            if ((propertyAssignments.at(j).object == assn.object)
                && (propertyAssignments.at(j).propertyName == assn.propertyName)) {
                found = true;
                break;
            }
        }
        if (!found) {
            assn.object->setProperty(assn.propertyName, assn.value);
        }
    }

    // Initialize animations from SetProperty definitions.
    propertiesForAnimations.clear();
    for (int i = 0; i < selectedAnimations.size(); ++i) {
        QAbstractAnimation *anim = selectedAnimations.at(i);
        QList<QPropertyAssignment>::iterator it;
        for (it = propertyAssignments.begin(); it != propertyAssignments.end(); ) {
            QPair<QList<QAbstractAnimation*>, QList<QAbstractAnimation*> > ret;
            ret = initializeAnimation(anim, *it);
            QList<QAbstractAnimation*> handlers = ret.first;
            if (!handlers.isEmpty()) {
                for (int j = 0; j < handlers.size(); ++j)
                    propertiesForAnimations.append(qMakePair(handlers.at(j), *it));
                it = propertyAssignments.erase(it);
            } else {
                ++it;
            }
            resetEndValues << ret.second;
        }

        // We require that at least one animation is valid.
        // ### generalize
        QList<QVariantAnimation*> variantAnims = qFindChildren<QVariantAnimation*>(anim);
        if (QVariantAnimation *va = qobject_cast<QVariantAnimation*>(anim))
            variantAnims.append(va);
        bool hasValidEndValue = false;
        for (int j = 0; j < variantAnims.size(); ++j) {
            if (variantAnims.at(j)->endValue().isValid()) {
                hasValidEndValue = true;
                break;
            }
        }

        if (hasValidEndValue) {
            QObject::disconnect(anim, SIGNAL(finished()), q, SLOT(_q_animationFinished()));
            QObject::connect(anim, SIGNAL(finished()), q, SLOT(_q_animationFinished()));
            anim->start();
            playingAnimations.append(anim);
        }
    }
#endif // !QT_NO_ANIMATION

    // Immediately set the properties that are not animated.
    for (int i = 0; i < propertyAssignments.size(); ++i) {
        const QPropertyAssignment &assn = propertyAssignments.at(i);
        assn.object->setProperty(assn.propertyName, assn.value);
    }
}

bool QStateMachinePrivate::isFinal(const QAbstractState *s)
{
    return qobject_cast<const QFinalState*>(s) != 0;
}

bool QStateMachinePrivate::isParallel(const QAbstractState *s)
{
    const QState *ss = qobject_cast<const QState*>(s);
    return ss && QStatePrivate::get(ss)->isParallelGroup;
}

bool QStateMachinePrivate::isCompound(const QAbstractState *s)
{
    const QState *group = qobject_cast<const QState*>(s);
    if (!group)
        return false;
    return (!isParallel(group) && !QStatePrivate::get(group)->childStates().isEmpty())
        || (qobject_cast<QStateMachine*>(group->parent()) != 0);
}

bool QStateMachinePrivate::isAtomic(const QAbstractState *s)
{
    const QState *ss = qobject_cast<const QState*>(s);
    return (ss && !QStatePrivate::get(ss)->isParallelGroup
            && QStatePrivate::get(ss)->childStates().isEmpty())
        || isFinal(s);
}


bool QStateMachinePrivate::isDescendantOf(const QAbstractState *state, const QAbstractState *other)
{
    Q_ASSERT(state != 0);
    for (QAbstractState *s = state->parentState(); s != 0; s = s->parentState()) {
        if (s == other)
            return true;
    }
    return false;
}

QList<QState*> QStateMachinePrivate::properAncestors(const QAbstractState *state, const QState *upperBound)
{
    Q_ASSERT(state != 0);
    QList<QState*> result;
    for (QState *s = state->parentState(); s && s != upperBound; s = s->parentState()) {
        result.append(s);
    }
    return result;
}

bool QStateMachinePrivate::isInFinalState(QAbstractState* s) const
{
    if (isCompound(s)) {
        QState *grp = qobject_cast<QState*>(s);
        QList<QAbstractState*> lst = QStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QAbstractState *cs = lst.at(i);
            if (isFinal(cs) && configuration.contains(cs))
                return true;
        }
        return false;
    } else if (isParallel(s)) {
        QState *grp = qobject_cast<QState*>(s);
        QList<QAbstractState*> lst = QStatePrivate::get(grp)->childStates();
        for (int i = 0; i < lst.size(); ++i) {
            QAbstractState *cs = lst.at(i);
            if (!isInFinalState(cs))
                return false;
        }
        return true;
    }
    else
        return false;
}

void QStateMachinePrivate::registerRestorable(QObject *object, const QByteArray &propertyName)
{
    RestorableId id(object, propertyName);
    if (!registeredRestorables.contains(id))
        registeredRestorables.insert(id, object->property(propertyName));
}

QList<QPropertyAssignment> QStateMachinePrivate::restorablesToPropertyList(const QHash<RestorableId, QVariant> &restorables) const
{
    QList<QPropertyAssignment> result;
    QHash<RestorableId, QVariant>::const_iterator it;
    for (it = restorables.constBegin(); it != restorables.constEnd(); ++it) {
//        qDebug() << "restorable:" << it.key().first << it.key().second << it.value();
        result.append(QPropertyAssignment(it.key().first, it.key().second, it.value(), /*explicitlySet=*/false));
    }
    return result;
}

/*! 
   \internal
   Returns true if the variable with the given \a id has been registered for restoration.
*/
bool QStateMachinePrivate::hasRestorable(QObject *object, const QByteArray &propertyName) const
{
    return registeredRestorables.contains(RestorableId(object, propertyName));
}

QVariant QStateMachinePrivate::restorableValue(QObject *object, const QByteArray &propertyName) const
{
    return registeredRestorables.value(RestorableId(object, propertyName), QVariant());
}


/*!
   \internal
    Unregisters the variable identified by \a id
*/
void QStateMachinePrivate::unregisterRestorable(QObject *object, const QByteArray &propertyName)
{
//    qDebug() << "unregisterRestorable(" << object << propertyName << ")";
    RestorableId id(object, propertyName);
    registeredRestorables.remove(id);
}

QAbstractState *QStateMachinePrivate::findErrorState(QAbstractState *context)
{
    // If the user sets the root state's error state to 0, we return the initial error state
    if (context == 0)
        return initialErrorStateForRoot;

    // Find error state recursively in parent hierarchy if not set explicitly for context state
    QAbstractState *errorState = 0;
    QState *s = qobject_cast<QState*>(context);
    if (s) {
        errorState = s->errorState();
        if (!errorState)
            errorState = findErrorState(s->parentState());
        return errorState;
    }

    return errorState;
}

void QStateMachinePrivate::setError(QStateMachine::Error errorCode, QAbstractState *currentContext)
{
    error = errorCode;

    switch (errorCode) {
    case QStateMachine::NoInitialStateError:        
        Q_ASSERT(currentContext != 0);

        errorString = QStateMachine::tr("Missing initial state in compound state '%1'")
                        .arg(currentContext->objectName());

        break;
    case QStateMachine::NoDefaultStateInHistoryState:
        Q_ASSERT(currentContext != 0);

        errorString = QStateMachine::tr("Missing default state in history state '%1'")
                        .arg(currentContext->objectName());
        break;
    default:
        errorString = QStateMachine::tr("Unknown error");
    };

    pendingErrorStates.clear(); 
    pendingErrorStatesForDefaultEntry.clear();

    QAbstractState *currentErrorState = findErrorState(currentContext);

    // Avoid infinite loop if the error state itself has an error
    if (currentContext == currentErrorState) {
        Q_ASSERT(currentContext != initialErrorStateForRoot); // RootErrorState is broken
        currentErrorState = initialErrorStateForRoot;
    }

    if (currentErrorState) {
        QState *lca = findLCA(QList<QAbstractState*>() << currentErrorState << currentContext);
        addStatesToEnter(currentErrorState, lca, pendingErrorStates, pendingErrorStatesForDefaultEntry);
    }
}

#ifndef QT_NO_ANIMATION

QPair<QList<QAbstractAnimation*>, QList<QAbstractAnimation*> >
QStateMachinePrivate::initializeAnimation(QAbstractAnimation *abstractAnimation, 
                                          const QPropertyAssignment &prop)
{
    QList<QAbstractAnimation*> handledAnimations;
    QList<QAbstractAnimation*> localResetEndValues;
    QAnimationGroup *group = qobject_cast<QAnimationGroup*>(abstractAnimation);
    if (group) {
        for (int i = 0; i < group->animationCount(); ++i) {
            QAbstractAnimation *animationChild = group->animationAt(i);
            QPair<QList<QAbstractAnimation*>, QList<QAbstractAnimation*> > ret;
            ret = initializeAnimation(animationChild, prop);
            handledAnimations << ret.first;
            localResetEndValues << ret.second;
        }
    } else { 
        QPropertyAnimation *animation = qobject_cast<QPropertyAnimation *>(abstractAnimation);
        if (animation != 0 
            && prop.object == animation->targetObject()
            && prop.propertyName == animation->propertyName()) {

            if (!animation->startValue().isValid()) {
                QByteArray propertyName = animation->propertyName();
                QVariant currentValue = animation->targetObject()->property(propertyName);

                QVariantAnimationPrivate::get(animation)->setDefaultStartValue(currentValue);
            }

            // Only change end value if it is undefined
            if (!animation->endValue().isValid()) {
                animation->setEndValue(prop.value);
                localResetEndValues.append(animation);
            }
            handledAnimations.append(animation);
        }
    }
    return qMakePair(handledAnimations, localResetEndValues);
}

static bool isAncestorOf(QObject *anc, QObject *o)
{
    for (o = o->parent() ; o != 0; o = o->parent()) {
        if (o == anc)
            return true;
    }
    return false;
}

void QStateMachinePrivate::_q_animationFinished()
{
    Q_Q(QStateMachine);
    QAbstractAnimation *animation = qobject_cast<QAbstractAnimation*>(q->sender());
    Q_ASSERT(animation != 0);
    QList<QPair<QAbstractAnimation*, QPropertyAssignment> >::iterator it;
    for (it = propertiesForAnimations.begin(); it != propertiesForAnimations.end(); ) {
        QAbstractAnimation *a = (*it).first;
        if (a == animation || isAncestorOf(animation, a)) {
            QPropertyAssignment assn = (*it).second;
            assn.object->setProperty(assn.propertyName, assn.value);
            if (!assn.explicitlySet)
                unregisterRestorable(assn.object, assn.propertyName);
            it = propertiesForAnimations.erase(it);
        } else {
            ++it;
        }
    }

    playingAnimations.removeOne(animation);
    if (playingAnimations.isEmpty())
        emit q->animationsFinished();
}

#endif // !QT_NO_ANIMATION

namespace {

class StartState : public QState
{
public:
    StartState(QState *parent)
        : QState(parent) {}
protected:
    void onEntry() {}
    void onExit() {}
};

class InitialTransition : public QAbstractTransition
{
public:
    InitialTransition(QAbstractState *target)
        : QAbstractTransition(QList<QAbstractState*>() << target) {}
protected:
    virtual bool eventTest(QEvent *) const { return true; }
    virtual void onTransition() {}
};

} // namespace

void QStateMachinePrivate::_q_start()
{
    Q_Q(QStateMachine);
    Q_ASSERT(state == Starting);
    if (!rootState) {
        state = NotRunning;
        return;
    }
    QAbstractState *initial = rootState->initialState();
    if (initial == 0)
        setError(QStateMachine::NoInitialStateError, rootState);    

    configuration.clear();
    qDeleteAll(internalEventQueue);
    internalEventQueue.clear();
    qDeleteAll(externalEventQueue);
    externalEventQueue.clear();

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": starting";
#endif
    state = Running;
    processingScheduled = true; // we call _q_process() below
    emit q->started();

    StartState *start = new StartState(rootState);
    QAbstractTransition *initialTransition = new InitialTransition(initial);
    start->addTransition(initialTransition);
    QList<QAbstractTransition*> transitions;
    transitions.append(initialTransition);
    executeTransitionContent(transitions);
    enterStates(transitions);
    applyProperties(transitions, QList<QAbstractState*>() << initial);
    delete start;

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": initial configuration:" << configuration;
#endif
    _q_process();
}

void QStateMachinePrivate::_q_process()
{
    Q_Q(QStateMachine);
    Q_ASSERT(state == Running);
    Q_ASSERT(!processing);
    processing = true;
    processingScheduled = false;
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": starting the event processing loop";
#endif
    while (processing) {
        if (stop) {
            stop = false;
            processing = false;
            stopProcessingReason = Stopped;
            break;
        }
        QSet<QAbstractTransition*> enabledTransitions;
        QEvent *e = new QEvent(QEvent::None);
        enabledTransitions = selectTransitions(e);
        if (enabledTransitions.isEmpty()) {
            delete e;
            e = 0;
        }
        if (enabledTransitions.isEmpty() && !internalEventQueue.isEmpty()) {
            e = internalEventQueue.takeFirst();
#ifdef QSTATEMACHINE_DEBUG
            qDebug() << q << ": dequeued internal event" << e << "of type" << e->type();
#endif
            enabledTransitions = selectTransitions(e);
            if (enabledTransitions.isEmpty()) {
                delete e;
                e = 0;
            }
        }
        if (enabledTransitions.isEmpty()) {
            if (externalEventQueue.isEmpty()) {
                if (internalEventQueue.isEmpty()) {
                    processing = false;
                    stopProcessingReason = EventQueueEmpty;
                }
            } else {
                e = externalEventQueue.takeFirst();
#ifdef QSTATEMACHINE_DEBUG
                qDebug() << q << ": dequeued external event" << e << "of type" << e->type();
#endif
                enabledTransitions = selectTransitions(e);
                if (enabledTransitions.isEmpty()) {
                    delete e;
                    e = 0;
                }
            }
        }
        if (!enabledTransitions.isEmpty()) {
            q->beginMicrostep(e);
            microstep(enabledTransitions.toList());
            q->endMicrostep(e);
        }
#ifdef QSTATEMACHINE_DEBUG
        else {
            qDebug() << q << ": no transitions enabled";
        }
#endif
        delete e;
    }
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": finished the event processing loop";
#endif
    switch (stopProcessingReason) {
    case EventQueueEmpty:
        break;
    case Finished:
        state = NotRunning;
        emit q->finished();
        break;
    case Stopped:
        state = NotRunning;
        emit q->stopped();
        break;
    }
}

void QStateMachinePrivate::scheduleProcess()
{
    if ((state != Running) || processing || processingScheduled)
        return;
    processingScheduled = true;
    QMetaObject::invokeMethod(q_func(), "_q_process", Qt::QueuedConnection);
}

void QStateMachinePrivate::registerTransitions(QAbstractState *state)
{
    QState *group = qobject_cast<QState*>(state);
    if (!group)
        return;
    QList<QAbstractTransition*> transitions = QStatePrivate::get(group)->transitions();
    for (int i = 0; i < transitions.size(); ++i) {
        QAbstractTransition *t = transitions.at(i);
        if (QSignalTransition *st = qobject_cast<QSignalTransition*>(t)) {
            registerSignalTransition(st);
        }
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
        else if (QEventTransition *oet = qobject_cast<QEventTransition*>(t)) {
            registerEventTransition(oet);
        }
#endif
    }
}

void QStateMachinePrivate::unregisterTransition(QAbstractTransition *transition)
{
    if (QSignalTransition *st = qobject_cast<QSignalTransition*>(transition)) {
        unregisterSignalTransition(st);
    }
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    else if (QEventTransition *oet = qobject_cast<QEventTransition*>(transition)) {
        unregisterEventTransition(oet);
    }
#endif
}

#ifndef QT_STATEMACHINE_SOLUTION

static int senderSignalIndex(const QObject *sender)
{
    QObjectPrivate *d = QObjectPrivate::get(const_cast<QObject*>(sender));
    QMutexLocker(&d->threadData->mutex);
    if (!d->currentSender)
        return -1;

    // Return -1 if d->currentSender isn't in d->senders
    bool found = false;
    for (int i = 0; !found && i < d->senders.count(); ++i)
        found = (d->senders.at(i).sender == d->currentSender->sender);
    if (!found)
        return -1;
    return d->currentSender->signal;
}

#endif

void QStateMachinePrivate::registerSignalTransition(QSignalTransition *transition)
{
    Q_Q(QStateMachine);
    if (QSignalTransitionPrivate::get(transition)->signalIndex != -1)
        return; // already registered
    QObject *sender = QSignalTransitionPrivate::get(transition)->sender;
    if (!sender)
        return;
    QByteArray signal = QSignalTransitionPrivate::get(transition)->signal;
    if (signal.startsWith('0'+QSIGNAL_CODE))
        signal.remove(0, 1);
    int signalIndex = sender->metaObject()->indexOfSignal(signal);
    if (signalIndex == -1) {
        qWarning("QSignalTransition: no such signal: %s::%s",
                 sender->metaObject()->className(), signal.constData());
        return;
    }
    QList<int> &connectedSignalIndexes = connections[sender];
    if (!connectedSignalIndexes.contains(signalIndex)) {
#ifndef QT_STATEMACHINE_SOLUTION
        if (!signalEventGenerator)
            signalEventGenerator = new QSignalEventGenerator(q);
#else
        QSignalEventGenerator *signalEventGenerator = new QSignalEventGenerator(signalIndex, q);
#endif
        bool ok = QMetaObject::connect(sender, signalIndex, signalEventGenerator,
                                       signalEventGenerator->metaObject()->methodOffset());
        if (!ok) {
#ifdef QSTATEMACHINE_DEBUG
            qDebug() << q << ": FAILED to add signal transition from" << transition->sourceState()
                     << ": ( sender =" << sender << ", signal =" << (signal.mid(1))
                     << ", targets =" << transition->targetStates() << ")";
#endif
            return;
        }
        connectedSignalIndexes.append(signalIndex);
    }
    QSignalTransitionPrivate::get(transition)->signalIndex = signalIndex;
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": added signal transition from" << transition->sourceState()
             << ": ( sender =" << sender << ", signal =" << (signal.mid(1))
             << ", targets =" << transition->targetStates() << ")";
#endif
}

void QStateMachinePrivate::unregisterSignalTransition(QSignalTransition *transition)
{
    int signalIndex = QSignalTransitionPrivate::get(transition)->signalIndex;
    if (signalIndex == -1)
        return; // not registered
#ifndef QT_STATEMACHINE_SOLUTION
    const QObject *sender = QSignalTransitionPrivate::get(transition)->sender;
    QList<int> &connectedSignalIndexes = connections[sender];
    Q_ASSERT(connectedSignalIndexes.contains(signalIndex));
    Q_ASSERT(signalEventGenerator != 0);
    bool ok = QMetaObject::disconnect(sender, signalIndex, signalEventGenerator,
                                      signalEventGenerator->metaObject()->methodOffset());
    if (ok) {
        connectedSignalIndexes.removeOne(signalIndex);
        if (connectedSignalIndexes.isEmpty())
            connections.remove(sender);
        QSignalTransitionPrivate::get(transition)->signalIndex = -1;
    }
#endif
}

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
void QStateMachinePrivate::registerEventTransition(QEventTransition *transition)
{
    Q_Q(QStateMachine);
    if (QEventTransitionPrivate::get(transition)->registered)
        return;
    if (transition->eventType() >= QEvent::User) {
        qWarning("QObject event transitions are not supported for custom types");
        return;
    }
    QObject *object = QEventTransitionPrivate::get(transition)->object;
    if (!object)
        return;
#ifndef QT_STATEMACHINE_SOLUTION
    QObjectPrivate *od = QObjectPrivate::get(object);
    if (!od->eventFilters.contains(q))
#endif
        object->installEventFilter(q);
    qobjectEvents[object].insert(transition->eventType());
    QEventTransitionPrivate::get(transition)->registered = true;
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q << ": added event transition from" << transition->sourceState()
             << ": ( object =" << object << ", event =" << transition->eventType()
             << ", targets =" << transition->targetStates() << ")";
#endif
}

void QStateMachinePrivate::unregisterEventTransition(QEventTransition *transition)
{
    Q_Q(QStateMachine);
    if (!QEventTransitionPrivate::get(transition)->registered)
        return;
    QObject *object = QEventTransitionPrivate::get(transition)->object;
    QSet<QEvent::Type> &events = qobjectEvents[object];
    events.remove(transition->eventType());
    if (events.isEmpty()) {
        qobjectEvents.remove(object);
        object->removeEventFilter(q);
    }
    QEventTransitionPrivate::get(transition)->registered = false;
}
#endif

void QStateMachinePrivate::handleTransitionSignal(const QObject *sender, int signalIndex,
                                                  void **argv)
{
    const QList<int> &connectedSignalIndexes = connections[sender];
    Q_ASSERT(connectedSignalIndexes.contains(signalIndex));
    const QMetaObject *meta = sender->metaObject();
    QMetaMethod method = meta->method(signalIndex);
    QList<QByteArray> parameterTypes = method.parameterTypes();
    int argc = parameterTypes.count();
    QList<QVariant> vargs;
    for (int i = 0; i < argc; ++i) {
        int type = QMetaType::type(parameterTypes.at(i));
        vargs.append(QVariant(type, argv[i+1]));
    }

#ifdef QSTATEMACHINE_DEBUG
    qDebug() << q_func() << ": sending signal event ( sender =" << sender
             << ", signal =" << sender->metaObject()->method(signalIndex).signature() << ")";
#endif
    internalEventQueue.append(new QSignalEvent(sender, signalIndex, vargs));
    scheduleProcess();
}

/*!
  Constructs a new state machine with the given \a parent.
*/
QStateMachine::QStateMachine(QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        *new QStateMachinePrivate,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(new QStateMachinePrivate)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  \internal
*/
QStateMachine::QStateMachine(QStateMachinePrivate &dd, QObject *parent)
    : QObject(
#ifndef QT_STATEMACHINE_SOLUTION
        dd,
#endif
        parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , d_ptr(&dd)
#endif
{
#ifdef QT_STATEMACHINE_SOLUTION
    d_ptr->q_ptr = this;
#endif
}

/*!
  Destroys this state machine.
*/
QStateMachine::~QStateMachine()
{
#ifdef QT_STATEMACHINE_SOLUTION
    delete d_ptr;
#endif
}

namespace {

class RootErrorState : public QAbstractState 
{
public:
    RootErrorState(QState *parent) 
        : QAbstractState(parent) 
    {
        setObjectName(QString::fromLatin1("DefaultErrorState"));
    }

    void onEntry()
    {
        QAbstractStatePrivate *d = QAbstractStatePrivate::get(this);
        QStateMachine *machine = d->machine();

        qWarning("Unrecoverable error detected in running state machine: %s", 
                 qPrintable(machine->errorString()));
    }

    void onExit() {}
};

class RootState : public QState
{
public:
    RootState(QState *parent)
        : QState(parent)
    {                
    }

    void onEntry() {}
    void onExit() {}
};

} // namespace

/*!
  Returns this state machine's root state.
*/
QState *QStateMachine::rootState() const
{
    Q_D(const QStateMachine);
    if (!d->rootState) {
        const_cast<QStateMachinePrivate*>(d)->rootState = new RootState(0);
        const_cast<QStateMachinePrivate*>(d)->initialErrorStateForRoot = new RootErrorState(d->rootState);
        d->rootState->setParent(const_cast<QStateMachine*>(this));
        d->rootState->setErrorState(d->initialErrorStateForRoot);
    }
    return d->rootState;
}

/*!
  Returns the error state of the state machine's root state. 

  \sa QState::errorState()
*/
QAbstractState *QStateMachine::errorState() const
{
    return rootState()->errorState();
}

/*!
  Sets the error state of this state machine's root state to be \a state. When a running state 
  machine encounters an error which puts it in an undefined state, it will enter an error state 
  based on the context of the error that occurred. It will enter this state regardless of what 
  is currently in the event queue.
  
  If the erroneous state has an error state set, this will be entered by the machine. If no error
  state has been set, the state machine will search the parent hierarchy recursively for an 
  error state. The error state of the root state can thus be seen as a global error state that 
  applies for the states for which a more specific error state has not been set.
  
  Before entering the error state, the state machine will set the error code returned by error() and 
  error  message returned by errorString(). 

  The default error state will print a warning to the console containing the information returned by 
  errorString(). By setting a new error state on either the state machine itself, or on specific 
  states, you can fine tune error handling in the state machine. 

  If the root state's error state is set to 0, or if the error state selected by the machine itself
  contains an error, the default error state will be used.

  \sa QState::setErrorState(), rootState()
*/
void QStateMachine::setErrorState(QAbstractState *state)
{
    rootState()->setErrorState(state);
}

/*! \enum QStateMachine::Error 

    This enum type defines errors that can occur in the state machine at run time. When the state
    machine encounters an unrecoverable error at run time, it will set the error code returned 
    by error(), the error message returned by errorString(), and enter an error state based on 
    the context of the error.

    \value NoError No error has occurred.
    \value NoInitialStateError The machine has entered a QState with children which does not have an
           initial state set. The context of this error is the state which is missing an initial
           state.
    \value NoDefaultStateInHistoryState The machine has entered a QHistoryState which does not have 
           a default state set. The context of this error is the QHistoryState which is missing a
           default state.          

    \sa setErrorState()
*/

/*!
  Returns the error code of the last error that occurred in the state machine.
*/
QStateMachine::Error QStateMachine::error() const
{
    Q_D(const QStateMachine);
    return d->error;
}

/*!
  Returns the error string of the last error that occurred in the state machine.
*/
QString QStateMachine::errorString() const
{
    Q_D(const QStateMachine);
    return d->errorString;
}

/*!
  Clears the error string and error code of the state machine.
*/
void QStateMachine::clearError()
{
    Q_D(QStateMachine);
    d->errorString.clear();
    d->error = NoError;
}

/*!
   Returns the global restore policy of the state machine.

   \sa QActionState::restorePolicy()
*/
QActionState::RestorePolicy QStateMachine::globalRestorePolicy() const
{
    Q_D(const QStateMachine);
    return d->globalRestorePolicy;
}

/*!
   Sets the global restore policy of the state machine to \a restorePolicy. The default global 
   restore policy is QAbstractState::DoNotRestoreProperties.
   
   The global restore policy cannot be set to QAbstractState::GlobalRestorePolicy.

   \sa QAbstractState::setRestorePolicy()
*/
void QStateMachine::setGlobalRestorePolicy(QAbstractState::RestorePolicy restorePolicy) 
{
    Q_D(QStateMachine);
    if (restorePolicy == QState::GlobalRestorePolicy) {
        qWarning("QStateMachine::setGlobalRestorePolicy: Cannot set global restore policy to "
                 "GlobalRestorePolicy");
        return;
    }

    d->globalRestorePolicy = restorePolicy;
}

/*!
  Returns this state machine's initial state, or 0 if no initial state has
  been set.
*/
QAbstractState *QStateMachine::initialState() const
{
    Q_D(const QStateMachine);
    if (!d->rootState)
        return 0;
    return d->rootState->initialState();
}

/*!
  Sets this state machine's initial \a state.
*/
void QStateMachine::setInitialState(QAbstractState *state)
{
    Q_D(QStateMachine);
    if (!d->rootState) {
        if (!state)
            return;
        rootState()->setInitialState(state);
    }
    d->rootState->setInitialState(state);
}

/*!
  Adds the given \a state to this state machine. The state becomes a top-level
  state (i.e. a child of the rootState()).

  If the state is already in a different machine, it will first be removed
  from its old machine, and then added to this machine.

  \sa removeState(), rootState()
*/
void QStateMachine::addState(QAbstractState *state)
{
    if (!state) {
        qWarning("QStateMachine::addState: cannot add null state");
        return;
    }
    if (QAbstractStatePrivate::get(state)->machine() == this) {
        qWarning("QStateMachine::addState: state has already been added to this machine");
        return;
    }
    state->setParent(rootState());
}

/*!
  Removes the given \a state from this state machine.  The state machine
  releases ownership of the state.

  \sa addState()
*/
void QStateMachine::removeState(QAbstractState *state)
{
    if (!state) {
        qWarning("QStateMachine::removeState: cannot remove null state");
        return;
    }
    if (QAbstractStatePrivate::get(state)->machine() != this) {
        qWarning("QStateMachine::removeState: state %p's machine (%p)"
                 " is different from this machine (%p)",
                 state, QAbstractStatePrivate::get(state)->machine(), this);
        return;
    }
    state->setParent(0);
}

/*!
  Starts this state machine.
  The machine will reset its configuration and transition to the initial
  state.  When a final top-level state is entered, the machine will emit the
  finished() signal.

  \sa started(), finished(), stop(), initialState()
*/
void QStateMachine::start()
{
    Q_D(QStateMachine);

    if (rootState()->initialState() == 0) {
        qWarning("QStateMachine::start: No initial state set for machine. Refusing to start.");
        return;
    }

    switch (d->state) {
    case QStateMachinePrivate::NotRunning:
        d->state = QStateMachinePrivate::Starting;
        QMetaObject::invokeMethod(this, "_q_start", Qt::QueuedConnection);
        break;
    case QStateMachinePrivate::Starting:
        break;
    case QStateMachinePrivate::Running:
        qWarning("QStateMachine::start(): already running");
        break;
    }
}

/*!
  Stops this state machine.

  \sa stopped()
*/
void QStateMachine::stop()
{
    Q_D(QStateMachine);
    switch (d->state) {
    case QStateMachinePrivate::NotRunning:
        break;
    case QStateMachinePrivate::Starting:
        // the machine will exit as soon as it enters the event processing loop
        d->stop = true;
        break;
    case QStateMachinePrivate::Running:
        d->stop = true;
        d->scheduleProcess();
        break;
    }
}

/*!
  Posts the given \a event for processing by this state machine, with a delay
  of \a delay milliseconds.

  This function returns immediately. The event is added to the state machine's
  event queue. Events are processed in the order posted. The state machine
  takes ownership of the event and deletes it once it has been processed.

  You can only post events when the state machine is running.
*/
void QStateMachine::postEvent(QEvent *event, int delay)
{
    Q_D(QStateMachine);
    if (d->state != QStateMachinePrivate::Running) {
        qWarning("QStateMachine::postEvent: cannot post event when the state machine is not running");
        return;
    }
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << this << ": posting external event" << event << "with delay" << delay;
#endif
    if (delay) {
        int tid = startTimer(delay);
        d->delayedEvents[tid] = event;
    } else {
        d->externalEventQueue.append(event);
        d->scheduleProcess();
    }
}

/*!
  \internal

  Posts the given internal \a event for processing by this state machine.
*/
void QStateMachine::postInternalEvent(QEvent *event)
{
    Q_D(QStateMachine);
#ifdef QSTATEMACHINE_DEBUG
    qDebug() << this << ": posting internal event" << event;
#endif
    d->internalEventQueue.append(event);
    d->scheduleProcess();
}

/*!
  Returns the list of this state machine's states, or an empty list if the
  state machine has no states.
*/
QList<QAbstractState*> QStateMachine::states() const
{
    return QStatePrivate::get(rootState())->childStates();
}

/*!
  \internal

   Returns the maximal consistent set of states (including parallel and final
   states) that this state machine is currently in. If a state \c s is in the
   configuration, it is always the case that the parent of \c s is also in
   c. Note, however, that the rootState() is not an explicit member of the
   configuration.
*/
QSet<QAbstractState*> QStateMachine::configuration() const
{
    Q_D(const QStateMachine);
    return d->configuration;
}

/*!
  \fn QStateMachine::started()

  This signal is emitted when the state machine has entered its initial state.

  \sa QStateMachine::finished(), QStateMachine::start()
*/

/*!
  \fn QStateMachine::finished()

  This signal is emitted when the state machine has reached a top-level final
  state.

  \sa QStateMachine::started()
*/

/*!
  \fn QStateMachine::stopped()

  This signal is emitted when the state machine has stopped.

  \sa QStateMachine::stop()
*/

#ifndef QT_NO_ANIMATION

/*!
  \fn QStateMachine::animationsFinished()

  This signal is emitted when the state machine has finished playing all
  animations associated with the latest transition (i.e., all properties have
  reached their target values).
*/

#endif

/*!
  \reimp
*/
bool QStateMachine::event(QEvent *e)
{
    Q_D(QStateMachine);
    if (e->type() == QEvent::Timer) {
        QTimerEvent *te = static_cast<QTimerEvent*>(e);
        int tid = te->timerId();
        if (d->delayedEvents.contains(tid)) {
            killTimer(tid);
            QEvent *ee = d->delayedEvents.take(tid);
            d->externalEventQueue.append(ee);
            d->scheduleProcess();
            return true;
        }
    } else if (e->type() == QEvent::ChildAdded) {
        QChildEvent *ce = static_cast<QChildEvent*>(e);
        if (QAbstractState *state = qobject_cast<QAbstractState*>(ce->child())) {
            if (state != rootState()) {
                state->setParent(rootState());
                return true;
            }
        }
    }
    return QObject::event(e);
}

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
/*!
  \reimp
*/
bool QStateMachine::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QStateMachine);
    Q_ASSERT(d->qobjectEvents.contains(watched));
    if (d->qobjectEvents[watched].contains(event->type()))
        postEvent(new QBoundEvent(watched, d->handler->cloneEvent(event)));
    return false;
}
#endif

/*!
  \internal

  This function is called when the state machine is about to select
  transitions based on the given \a event.

  The default implementation does nothing.
*/
void QStateMachine::beginSelectTransitions(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine has finished selecting
  transitions based on the given \a event.

  The default implementation does nothing.
*/
void QStateMachine::endSelectTransitions(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine is about to do a microstep.

  The default implementation does nothing.
*/
void QStateMachine::beginMicrostep(QEvent *event)
{
    Q_UNUSED(event);
}

/*!
  \internal

  This function is called when the state machine has finished doing a
  microstep.

  The default implementation does nothing.
*/
void QStateMachine::endMicrostep(QEvent *event)
{
    Q_UNUSED(event);
}

static const uint qt_meta_data_QSignalEventGenerator[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QSignalEventGenerator[] = {
    "QSignalEventGenerator\0\0execute()\0"
};

const QMetaObject QSignalEventGenerator::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QSignalEventGenerator,
      qt_meta_data_QSignalEventGenerator, 0 }
};

const QMetaObject *QSignalEventGenerator::metaObject() const
{
    return &staticMetaObject;
}

void *QSignalEventGenerator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSignalEventGenerator))
        return static_cast<void*>(const_cast< QSignalEventGenerator*>(this));
    return QObject::qt_metacast(_clname);
}

int QSignalEventGenerator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: {
#ifndef QT_STATEMACHINE_SOLUTION
// ### in Qt 4.6 we can use QObject::senderSignalIndex()
            int signalIndex = senderSignalIndex(this);
            Q_ASSERT(signalIndex != -1);
#endif
            QStateMachine *machine = qobject_cast<QStateMachine*>(parent());
            QStateMachinePrivate::get(machine)->handleTransitionSignal(sender(), signalIndex, _a);
            break;
        }
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

QSignalEventGenerator::QSignalEventGenerator(
#ifdef QT_STATEMACHINE_SOLUTION
    int sigIdx,
#endif
    QStateMachine *parent)
    : QObject(parent)
#ifdef QT_STATEMACHINE_SOLUTION
    , signalIndex(sigIdx)
#endif
{
}

/*!
  \class QSignalEvent

  \brief The QSignalEvent class represents a Qt signal event.

  \ingroup statemachine

  A signal event is generated by a QStateMachine in response to a Qt
  signal. The QSignalTransition class provides a transition associated with a
  signal event. QSignalEvent is part of \l{The State Machine Framework}.

  The sender() function returns the object that generated the signal. The
  signalIndex() function returns the index of the signal. The arguments()
  function returns the arguments of the signal.

  \sa QSignalTransition
*/

/*!
  \internal

  Constructs a new QSignalEvent object with the given \a sender, \a
  signalIndex and \a arguments.
*/
QSignalEvent::QSignalEvent(const QObject *sender, int signalIndex,
                           const QList<QVariant> &arguments)
    :
#ifndef QT_STATEMACHINE_SOLUTION
    QEvent(QEvent::Signal)
#else
    QEvent(QEvent::Type(QEvent::User-1))
#endif
    , m_sender(sender), m_signalIndex(signalIndex), m_arguments(arguments)
{
}

/*!
  Destroys this QSignalEvent.
*/
QSignalEvent::~QSignalEvent()
{
}

/*!
  \fn QSignalEvent::sender() const

  Returns the object that emitted the signal.

  \sa QObject::sender()
*/

/*!
  \fn QSignalEvent::signalIndex() const

  Returns the index of the signal.

  \sa QMetaObject::indexOfSignal()
*/

/*!
  \fn QSignalEvent::arguments() const

  Returns the arguments of the signal.
*/

/*!
  \class QStateFinishedEvent

  \brief The QStateFinishedEvent class contains parameters that describe a state that has finished.

  \ingroup statemachine

  A state is finished when one of its final child states (a QFinalState) is
  entered; this will cause a QStateFinishedEvent to be generated by the state
  machine. QStateFinishedEvent is part of \l{The State Machine Framework}.

  Typically you do not create QStateFinishedEvent objects yourself, but rather
  use QStateFinishedTransition to create a transition that's triggered by a
  state's finished event.

  \sa QStateFinishedTransition
*/

/*!
  \internal

  Constructs a new QStateFinishedEvent object associated with the given \a state.
*/
QStateFinishedEvent::QStateFinishedEvent(QState *state)
    :
#ifndef QT_STATEMACHINE_SOLUTION
    QEvent(StateFinished)
#else
    QEvent(QEvent::Type(QEvent::User-2))
#endif
    , m_state(state)
{
}

/*!
  Destroys this QStateFinishedEvent.
*/
QStateFinishedEvent::~QStateFinishedEvent()
{
}

/*!
  Returns the state associated with this QStateFinishedEvent.
*/
QState *QStateFinishedEvent::state() const
{
    return m_state;
}

QT_END_NAMESPACE

#include "moc_qstatemachine.cpp"
