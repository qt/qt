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

#ifndef QSTATE_H
#define QSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qactionstate.h>
#else
#include "qactionstate.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QAbstractTransition;
class QHistoryState;
#ifndef QT_NO_ANIMATION
class QAbstractAnimation;
class QAnimationState;
#endif

class QStatePrivate;
class Q_CORE_EXPORT QState : public QActionState
{
    Q_OBJECT
public:
    enum Type {
        Normal,
        ParallelGroup
    };

    enum HistoryType {
        ShallowHistory,
        DeepHistory
    };

    QState(QState *parent = 0);
    QState(Type type, QState *parent = 0);
    ~QState();

    QAbstractState *errorState() const;
    void setErrorState(QAbstractState *state);

    void addTransition(QAbstractTransition *transition);
    void addTransition(QObject *sender, const char *signal, QAbstractState *target);
    void addTransition(QAbstractState *target);
    void addFinishedTransition(QAbstractState *target);
    void removeTransition(QAbstractTransition *transition);
    QList<QAbstractTransition*> transitions() const;

#ifndef QT_NO_ANIMATION
    QAnimationState *addAnimatedTransition(QObject *sender, const char *signal, 
                                           QAbstractState *targetState, 
                                           QAbstractAnimation *animation = 0);
    QAnimationState *addAnimatedTransition(QAbstractTransition *transition,
                                           QAbstractAnimation *animation = 0);
#endif

    QHistoryState *addHistoryState(HistoryType type = ShallowHistory);

    QAbstractState *initialState() const;
    void setInitialState(QAbstractState *state);

protected:
    void onEntry();
    void onExit();

    bool event(QEvent *e);

protected:
    QState(QStatePrivate &dd, QState *parent);

private:
    Q_DISABLE_COPY(QState)
    Q_DECLARE_PRIVATE(QState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
