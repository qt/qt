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

#ifndef QSTATEFINISHEDTRANSITION_H
#define QSTATEFINISHEDTRANSITION_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qtransition.h>
#else
#include "qtransition.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QState;

class QStateFinishedTransitionPrivate;
class Q_CORE_EXPORT QStateFinishedTransition : public QTransition
{
    Q_OBJECT
    Q_PROPERTY(QState* state READ state WRITE setState)
public:
    QStateFinishedTransition(QState *sourceState = 0);
    QStateFinishedTransition(QState *state, const QList<QAbstractState*> &targets,
                             QState *sourceState = 0);
    ~QStateFinishedTransition();

    QState *state() const; // ### name
    void setState(QState *state);

protected:
    bool eventTest(QEvent *event) const;
    void onTransition();

    bool event(QEvent *e);

private:
    Q_DISABLE_COPY(QStateFinishedTransition)
    Q_DECLARE_PRIVATE(QStateFinishedTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
