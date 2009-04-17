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

#ifndef QABSTRACTSTATE_H
#define QABSTRACTSTATE_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QState;

class QAbstractStatePrivate;
class Q_CORE_EXPORT QAbstractState : public QObject
{
    Q_OBJECT
public:
    ~QAbstractState();

    QState *parentState() const;

protected:
    QAbstractState(QState *parent = 0);

    virtual void onEntry() = 0;
    virtual void onExit() = 0;

    bool event(QEvent *e);

protected:
#ifdef QT_STATEMACHINE_SOLUTION
     QAbstractStatePrivate *d_ptr;
#endif
    QAbstractState(QAbstractStatePrivate &dd, QState *parent);

private:
    Q_DISABLE_COPY(QAbstractState)
    Q_DECLARE_PRIVATE(QAbstractState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
