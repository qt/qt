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

#ifndef QFINALSTATE_H
#define QFINALSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qactionstate.h>
#else
#include "qactionstate.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QFinalStatePrivate;
class Q_CORE_EXPORT QFinalState : public QActionState
{
    Q_OBJECT
public:
    QFinalState(QState *parent = 0);
    ~QFinalState();

protected:
    void onEntry();
    void onExit();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QFinalState)
    Q_DECLARE_PRIVATE(QFinalState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
