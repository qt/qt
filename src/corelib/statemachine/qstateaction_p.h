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

#ifndef QSTATEACTION_P_H
#define QSTATEACTION_P_H

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

QT_BEGIN_NAMESPACE

class QStateAction;
class QStateActionPrivate
#ifndef QT_STATEMACHINE_SOLUTION
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QStateAction)
public:
    QStateActionPrivate();
    ~QStateActionPrivate();

    static QStateActionPrivate *get(QStateAction *q);

    void callExecute();

    enum When {
        ExecuteOnEntry,
        ExecuteOnExit
    };

    When when;

#ifdef QT_STATEMACHINE_SOLUTION
    QStateAction *q_ptr;
#endif
};

class QStateSetPropertyAction;
class QStateSetPropertyActionPrivate : public QStateActionPrivate
{
    Q_DECLARE_PUBLIC(QStateSetPropertyAction)
public:
    QStateSetPropertyActionPrivate() {}
    ~QStateSetPropertyActionPrivate() {}

    static QStateSetPropertyActionPrivate *get(QStateSetPropertyAction *q);

    QObject *target;
    QByteArray propertyName;
    QVariant value;
};

class QStateInvokeMethodAction;
class QStateInvokeMethodActionPrivate : public QStateActionPrivate
{
    Q_DECLARE_PUBLIC(QStateInvokeMethodAction)
public:
    QStateInvokeMethodActionPrivate() {}
    ~QStateInvokeMethodActionPrivate() {}

    static QStateInvokeMethodActionPrivate *get(QStateInvokeMethodAction *q);

    QObject *target;
    QByteArray methodName;
    int methodIndex;
    QList<QVariant> args;
};

QT_END_NAMESPACE

#endif
