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

#ifndef QBOUNDEVENT_P_H
#define QBOUNDEVENT_P_H

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

#include <QtCore/qcoreevent.h>

QT_BEGIN_NAMESPACE

class QBoundEvent : public QEvent
{
public:
    QBoundEvent(QObject *object, QEvent *event)
#ifdef QT_STATEMACHINE_SOLUTION
        : QEvent(QEvent::Type(QEvent::User-3)),
#else
        : QEvent(QEvent::Bound),
#endif
          m_object(object), m_event(event) {}
    ~QBoundEvent() {}

    inline QObject *object() const { return m_object; }
    inline QEvent *event() const { return m_event; }

private:
    QObject *m_object;
    QEvent *m_event;
};

QT_END_NAMESPACE

#endif
