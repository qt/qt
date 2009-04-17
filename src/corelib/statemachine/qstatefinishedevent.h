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

#ifndef QSTATEFINISHEDEVENT_H
#define QSTATEFINISHEDEVENT_H

#include <QtCore/qcoreevent.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QState;

class Q_CORE_EXPORT QStateFinishedEvent : public QEvent
{
public:
    QStateFinishedEvent(QState *state);
    ~QStateFinishedEvent();

    QState *state() const;

private:
    QState *m_state;
    void *d;
};

QT_END_NAMESPACE

#endif
