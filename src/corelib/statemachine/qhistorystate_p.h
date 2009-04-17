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

#ifndef QHISTORYSTATE_P_H
#define QHISTORYSTATE_P_H

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

#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

class QHistoryState;
class QHistoryStatePrivate : public QAbstractStatePrivate
{
    Q_DECLARE_PUBLIC(QHistoryState)

public:
    QHistoryStatePrivate();

    static QHistoryState *create(QState::HistoryType type,
                                 QState *parent);

    static QHistoryStatePrivate *get(QHistoryState *q);
    static const QHistoryStatePrivate *get(const QHistoryState *q);

    QAbstractState *defaultState;
    QState::HistoryType historyType;
    QList<QAbstractState*> configuration;
};

QT_END_NAMESPACE

#endif
