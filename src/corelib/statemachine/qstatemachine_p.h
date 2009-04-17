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

#ifndef QSTATEMACHINE_P_H
#define QSTATEMACHINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QT_STATEMACHINE_SOLUTION
#include <private/qobject_p.h>
#endif
#include <QtCore/qcoreevent.h>
#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qset.h>

#include "qstate.h"

QT_BEGIN_NAMESPACE

class QEvent;
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
class QEventTransition;
#endif
class QSignalEventGenerator;
class QSignalTransition;
class QAbstractState;
class QAbstractTransition;
class QState;

#ifndef QT_NO_ANIMATION
class QPropertyAnimation;
#endif

class QStateMachine;
class Q_CORE_EXPORT QStateMachinePrivate
#ifndef QT_STATEMACHINE_SOLUTION
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QStateMachine)
public:
    enum State {
        NotRunning,
        Starting,
        Running
    };
    enum StopProcessingReason {
        EventQueueEmpty,
        Finished,
        Stopped
    };

    QStateMachinePrivate();
    ~QStateMachinePrivate();

    static QStateMachinePrivate *get(QStateMachine *q);

    static QState *findLCA(const QList<QAbstractState*> &states);

    static bool stateEntryLessThan(QAbstractState *s1, QAbstractState *s2);
    static bool stateExitLessThan(QAbstractState *s1, QAbstractState *s2);

    QAbstractState *findErrorState(QAbstractState *context);
    void setError(QStateMachine::Error error, QAbstractState *currentContext);

    // private slots
    void _q_start();
    void _q_process();

    void microstep(const QList<QAbstractTransition*> &transitionList);
    bool isPreempted(const QAbstractState *s, const QSet<QAbstractTransition*> &transitions) const;
    QSet<QAbstractTransition*> selectTransitions(QEvent *event) const;
    void exitStates(const QList<QAbstractTransition*> &transitionList);
    void executeTransitionContent(const QList<QAbstractTransition*> &transitionList);
    void enterStates(const QList<QAbstractTransition*> &enabledTransitions);
    void addStatesToEnter(QAbstractState *s, QState *root,
                          QSet<QAbstractState*> &statesToEnter,
                          QSet<QAbstractState*> &statesForDefaultEntry);

    bool isInFinalState(QAbstractState *s) const;
    static bool isFinal(const QAbstractState *s);
    static bool isParallel(const QAbstractState *s);
    static bool isCompound(const QAbstractState *s);
    static bool isAtomic(const QAbstractState *s);
    static bool isDescendantOf(const QAbstractState *s, const QAbstractState *other);
    static QList<QState*> properAncestors(const QAbstractState *s, const QState *upperBound);

    void registerTransitions(QAbstractState *state);
    void registerSignalTransition(QSignalTransition *transition);
    void unregisterSignalTransition(QSignalTransition *transition);
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    void registerEventTransition(QEventTransition *transition);
    void unregisterEventTransition(QEventTransition *transition);
#endif
    void unregisterTransition(QAbstractTransition *transition);
    void handleTransitionSignal(const QObject *sender, int signalIndex,
                                void **args);    
    void scheduleProcess();
    
    typedef QPair<QObject *, QByteArray> RestorableId;
    QHash<RestorableId, QVariant> registeredRestorables;
    void registerRestorable(QObject *object, const QByteArray &propertyName);
    void unregisterRestorable(QObject *object, const QByteArray &propertyName);
    bool hasRestorable(QObject *object, const QByteArray &propertyName) const;
    QVariant restorableValue(QObject *object, const QByteArray &propertyName) const;

#ifndef QT_NO_ANIMATION
    void registerRestorable(QPropertyAnimation *animation);
        
    QPropertyAnimation *restorableAnimation(QObject *object, const QByteArray &propertyName);
    QList<QPropertyAnimation *> restorableAnimations() const;
    
    QHash<RestorableId, QPropertyAnimation*> registeredRestorableAnimations;    
#endif // QT_NO_ANIMATION

    State state;
    bool processing;
    bool processingScheduled;
    bool stop;
    StopProcessingReason stopProcessingReason;
    QState *rootState;
    QSet<QAbstractState*> configuration;
    QList<QEvent*> internalEventQueue;
    QList<QEvent*> externalEventQueue;
    
    QStateMachine::Error error;
    QActionState::RestorePolicy globalRestorePolicy;

    QString errorString;
    QSet<QAbstractState *> pendingErrorStates;
    QSet<QAbstractState *> pendingErrorStatesForDefaultEntry;
    QAbstractState *initialErrorStateForRoot;

#ifndef QT_STATEMACHINE_SOLUTION
    QSignalEventGenerator *signalEventGenerator;
#endif
    QHash<const QObject*, QList<int> > connections;
#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    QHash<QObject*, QSet<QEvent::Type> > qobjectEvents;
#endif
    QHash<int, QEvent*> delayedEvents;
  
    typedef QEvent* (*f_cloneEvent)(QEvent*);
    struct Handler {
        f_cloneEvent cloneEvent;
    };

    static const Handler *handler;

#ifdef QT_STATEMACHINE_SOLUTION
    QStateMachine *q_ptr;
#endif
};

QT_END_NAMESPACE

#endif
