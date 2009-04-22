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

#ifndef QABSTRACTTRANSITION_H
#define QABSTRACTTRANSITION_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QEvent;
class QAbstractState;
class QState;

class QAbstractTransitionPrivate;
class Q_CORE_EXPORT QAbstractTransition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QState* source READ sourceState)
    Q_PROPERTY(QAbstractState* target READ targetState WRITE setTargetState)
    Q_PROPERTY(QList<QAbstractState*> targets READ targetStates WRITE setTargetStates)
public:
    QAbstractTransition(QState *sourceState = 0);
    QAbstractTransition(const QList<QAbstractState*> &targets, QState *sourceState = 0);
    virtual ~QAbstractTransition();

    QState *sourceState() const;
    QAbstractState *targetState() const;
    void setTargetState(QAbstractState* target);
    QList<QAbstractState*> targetStates() const;
    void setTargetStates(const QList<QAbstractState*> &targets);

protected:
    virtual bool eventTest(QEvent *event) const = 0;

    virtual void onTransition() = 0;

    bool event(QEvent *e);

protected:
#ifdef QT_STATEMACHINE_SOLUTION
    QAbstractTransitionPrivate *d_ptr;
#endif
    QAbstractTransition(QAbstractTransitionPrivate &dd, QState *parent);
    QAbstractTransition(QAbstractTransitionPrivate &dd,
                        const QList<QAbstractState*> &targets, QState *parent);

private:
    Q_DISABLE_COPY(QAbstractTransition)
    Q_DECLARE_PRIVATE(QAbstractTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
