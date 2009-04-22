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

#ifndef QHISTORYSTATE_H
#define QHISTORYSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qstate.h>
#else
#include "qstate.h"
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QHistoryStatePrivate;
class Q_CORE_EXPORT QHistoryState : public QAbstractState
{
    Q_OBJECT
public:
    ~QHistoryState();

    QAbstractState *defaultState() const;
    void setDefaultState(QAbstractState *state);

protected:
    void onEntry();
    void onExit();

    bool event(QEvent *e);

private:
    QHistoryState(QState::HistoryType type,
                  QState *parent = 0);

private:
    Q_DISABLE_COPY(QHistoryState)
    Q_DECLARE_PRIVATE(QHistoryState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
