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

#ifndef QACTIONSTATE_P_H
#define QACTIONSTATE_P_H

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

#include "qabstractstate_p.h"
#include "qactionstate.h"

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QStateAction;

class QActionState;
class Q_CORE_EXPORT QActionStatePrivate : public QAbstractStatePrivate
{
    Q_DECLARE_PUBLIC(QActionState)

public:
    QActionStatePrivate();
    ~QActionStatePrivate();

    static QActionStatePrivate *get(QActionState *q);
    static const QActionStatePrivate *get(const QActionState *q);

    QList<QStateAction*> entryActions() const;
    QList<QStateAction*> exitActions() const;

    QActionState::RestorePolicy restorePolicy;
};

QT_END_NAMESPACE

#endif
