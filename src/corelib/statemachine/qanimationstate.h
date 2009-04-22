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

#ifndef QANIMATIONSTATE_H
#define QANIMATIONSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qstate.h>
#else
#include "qstate.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_ANIMATION

class QAbstractAnimation;

class QAnimationStatePrivate;
class Q_CORE_EXPORT QAnimationState : public QState
{   
    Q_OBJECT
public:    

    QAnimationState(QAbstractAnimation *animation, QState *parent = 0);
    QAnimationState(QState *parent = 0);
    ~QAnimationState();

    int animationCount() const;
    QAbstractAnimation *animationAt(int i) const;
    void addAnimation(QAbstractAnimation *animation);
    void removeAnimation(QAbstractAnimation *animation);

protected:        
    virtual void onEntry();
    virtual void onExit();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QAnimationState)
    Q_DECLARE_PRIVATE(QAnimationState)    
};

#endif //QT_NO_ANIMATION

QT_END_NAMESPACE

QT_END_HEADER

#endif // QANIMATIONSTATE_H
