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

#ifndef QSTATEMACHINE_H
#define QSTATEMACHINE_H

#ifndef QT_STATEMACHINE_SOLUTION
#  include <QtCore/qactionstate.h>
#else
#  include "qactionstate.h"
#endif

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qset.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEvent;
class QAbstractState;
class QState;

class QStateMachinePrivate;
class Q_CORE_EXPORT QStateMachine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QState* rootState READ rootState)
    Q_PROPERTY(QAbstractState* initialState READ initialState WRITE setInitialState)
    Q_PROPERTY(QAbstractState* errorState READ errorState WRITE setErrorState)
    Q_PROPERTY(QString errorString READ errorString)
public:

    enum Error {
        NoError, 
        NoInitialStateError,
        NoDefaultStateInHistoryState,
    };

    QStateMachine(QObject *parent = 0);
    ~QStateMachine();

    void addState(QAbstractState *state);
    void removeState(QAbstractState *state);

    QState *rootState() const;

    QAbstractState *initialState() const;
    void setInitialState(QAbstractState *state);

    QAbstractState *errorState() const;
    void setErrorState(QAbstractState *state);

    Error error() const;
    QString errorString() const;
    void clearError();

    QActionState::RestorePolicy globalRestorePolicy() const;
    void setGlobalRestorePolicy(QActionState::RestorePolicy restorePolicy);

    void postEvent(QEvent *event, int delay = 0);

    QList<QAbstractState*> states() const;
    QSet<QAbstractState*> configuration() const;

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    bool eventFilter(QObject *watched, QEvent *event);
#endif

public Q_SLOTS:
    void start();
    void stop();

Q_SIGNALS:
    void started();
    void stopped();
    void finished();

protected:
    void postInternalEvent(QEvent *event);

    virtual void beginSelectTransitions(QEvent *event);
    virtual void endSelectTransitions(QEvent *event);

    virtual void beginMicrostep(QEvent *event);
    virtual void endMicrostep(QEvent *event);

    bool event(QEvent *e);

#ifdef QT_STATEMACHINE_SOLUTION
     QStateMachinePrivate *d_ptr;
#endif

private:
    Q_DISABLE_COPY(QStateMachine)
    Q_DECLARE_PRIVATE(QStateMachine)
    Q_PRIVATE_SLOT(d_func(), void _q_start())
    Q_PRIVATE_SLOT(d_func(), void _q_process())
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
