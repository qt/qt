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

#ifndef QTRANSITION_H
#define QTRANSITION_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qabstracttransition.h>
#else
#include "qabstracttransition.h"
#endif

#include <QtCore/qvariant.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QStateAction;

class QTransitionPrivate;
class Q_CORE_EXPORT QTransition : public QAbstractTransition
{
    Q_OBJECT
public:
    QTransition(QState *sourceState = 0);
    QTransition(const QList<QAbstractState*> &targets, QState *sourceState = 0);
    ~QTransition();

    void setPropertyOnTransition(QObject *object, const char *name,
                                 const QVariant &value);
    void invokeMethodOnTransition(QObject *object, const char *method,
                                  const QList<QVariant> &args = QList<QVariant>());

    void addAction(QStateAction *action);
    void removeAction(QStateAction *action);
    QList<QStateAction*> actions() const;

protected:
    virtual void onTransition();

    bool event(QEvent *e);

protected:
    QTransition(QTransitionPrivate &dd, QState *parent);
    QTransition(QTransitionPrivate &dd, const QList<QAbstractState*> &targets, QState *parent);

private:
    Q_DISABLE_COPY(QTransition)
    Q_DECLARE_PRIVATE(QTransition)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
