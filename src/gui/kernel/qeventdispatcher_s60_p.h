/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#ifndef QEVENTDISPATCHER_S60_P_H
#define QEVENTDISPATCHER_S60_P_H

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

#include <private/qeventdispatcher_symbian_p.h>
#include "qt_s60_p.h"

QT_BEGIN_NAMESPACE

class Q_GUI_EXPORT QEventDispatcherS60 : public QEventDispatcherSymbian
{
    Q_OBJECT

public:
    QEventDispatcherS60(QObject *parent = 0);
    ~QEventDispatcherS60();

    bool processEvents ( QEventLoop::ProcessEventsFlags flags );
    bool hasPendingEvents();

    bool excludeUserInputEvents() { return m_noInputEvents; }

    void saveInputEvent(QSymbianControl *control, QWidget *widget, QInputEvent *event);

private:
    bool sendDeferredInputEvents();

private Q_SLOTS:
    void removeInputEventsForWidget(QObject *object);

private:
    bool m_noInputEvents;

    struct DeferredInputEvent
    {
        QSymbianControl *control;
        QWidget *widget;
        QInputEvent *event;
    };
    QList<DeferredInputEvent> m_deferredInputEvents;
};

QT_END_NAMESPACE

#endif // QEVENTDISPATCHER_S60_P_H
