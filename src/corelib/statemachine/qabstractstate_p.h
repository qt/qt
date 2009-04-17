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

#ifndef QABSTRACTSTATE_P_H
#define QABSTRACTSTATE_P_H

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

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QAbstractTransition;
class QHistoryState;
class QStateMachine;

class QAbstractState;
class Q_CORE_EXPORT QAbstractStatePrivate
#ifndef QT_STATEMACHINE_SOLUTION
    : public QObjectPrivate
#endif
{
    Q_DECLARE_PUBLIC(QAbstractState)

public:
    QAbstractStatePrivate();

    static QAbstractStatePrivate *get(QAbstractState *q);
    static const QAbstractStatePrivate *get(const QAbstractState *q);

    QStateMachine *machine() const;

    void callOnEntry();
    void callOnExit();

#ifdef QT_STATEMACHINE_SOLUTION
    QAbstractState *q_ptr;
#endif
};

QT_END_NAMESPACE

#endif
