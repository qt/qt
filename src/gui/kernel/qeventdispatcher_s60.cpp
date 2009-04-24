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

#include <qwidget.h>

#include "qeventdispatcher_s60_p.h"

QT_BEGIN_NAMESPACE

QEventDispatcherS60::QEventDispatcherS60(QObject *parent)
    : QEventDispatcherSymbian(parent),
      m_noInputEvents(false)
{
}

QEventDispatcherS60::~QEventDispatcherS60()
{
    for (int c = 0; c < m_deferredInputEvents.size(); ++c) {
        delete m_deferredInputEvents[c].event;
    }
}

bool QEventDispatcherS60::processEvents ( QEventLoop::ProcessEventsFlags flags )
{
    bool ret = false;

    bool oldNoInputEventsValue = m_noInputEvents;
    if (flags & QEventLoop::ExcludeUserInputEvents) {
        m_noInputEvents = true;
    } else {
        m_noInputEvents = false;
        ret = sendDeferredInputEvents() || ret;
    }

    ret = QEventDispatcherSymbian::processEvents(flags) || ret;

    m_noInputEvents = oldNoInputEventsValue;

    return ret;
}

bool QEventDispatcherS60::hasPendingEvents()
{
    return !m_deferredInputEvents.isEmpty() || QEventDispatcherSymbian::hasPendingEvents();
}

void QEventDispatcherS60::saveInputEvent(QSymbianControl *control, QWidget *widget, QInputEvent *event)
{
    DeferredInputEvent inputEvent = {control, widget, event};
    m_deferredInputEvents.append(inputEvent);
    connect(widget, SIGNAL(destroyed(QObject *)), SLOT(removeInputEventsForWidget(QObject *)));
}

bool QEventDispatcherS60::sendDeferredInputEvents()
{
    bool eventsSent = false;
    while (!m_deferredInputEvents.isEmpty()) {
        DeferredInputEvent inputEvent = m_deferredInputEvents.takeFirst();
#ifndef QT_NO_EXCEPTIONS
        try {
#endif
            inputEvent.control->sendInputEvent(inputEvent.widget, inputEvent.event);
#ifndef QT_NO_EXCEPTIONS
        } catch (...) {
            delete inputEvent.event;
            throw;
        }
#endif
        delete inputEvent.event;
        eventsSent = true;
    }

    return eventsSent;
}

void QEventDispatcherS60::removeInputEventsForWidget(QObject *object)
{
    for (int c = 0; c < m_deferredInputEvents.size(); ++c) {
        if (m_deferredInputEvents[c].widget == object) {
            delete m_deferredInputEvents[c].event;
            m_deferredInputEvents.removeAt(c--);
        }
    }
}

QT_END_NAMESPACE
