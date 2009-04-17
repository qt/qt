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

#ifndef QACTIONSTATE_H
#define QACTIONSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qabstractstate.h>
#else
#include "qabstractstate.h"
#endif

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QStateAction;

class QActionStatePrivate;
class Q_CORE_EXPORT QActionState : public QAbstractState
{
    Q_OBJECT
public:
    enum RestorePolicy {
        GlobalRestorePolicy,
        DoNotRestoreProperties,
        RestoreProperties
    };

    QActionState(QState *parent = 0);
    ~QActionState();

    void setPropertyOnEntry(QObject *object, const char *name,
                            const QVariant &value);
    void setPropertyOnExit(QObject *object, const char *name,
                           const QVariant &value);
    void invokeMethodOnEntry(QObject *object, const char *method,
                             const QList<QVariant> &args = QList<QVariant>());
    void invokeMethodOnExit(QObject *object, const char *method,
                            const QList<QVariant> &args = QList<QVariant>());

    void addEntryAction(QStateAction *action);
    void addExitAction(QStateAction *action);

    void removeEntryAction(QStateAction *action);
    void removeExitAction(QStateAction *action);

    QList<QStateAction*> entryActions() const;
    QList<QStateAction*> exitActions() const;

    void setRestorePolicy(RestorePolicy restorePolicy);
    RestorePolicy restorePolicy() const;

protected:
    void onEntry();
    void onExit();

    bool event(QEvent *e);

protected:
    QActionState(QActionStatePrivate &dd, QState *parent);

private:
    Q_DISABLE_COPY(QActionState)
    Q_DECLARE_PRIVATE(QActionState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
