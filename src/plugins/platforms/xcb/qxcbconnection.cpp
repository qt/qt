/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qxcbconnection.h"
#include "qxcbscreen.h"
#include "qxcbwindow.h"

#include <QtAlgorithms>
#include <QSocketNotifier>

#include <stdio.h>

QXcbConnection::QXcbConnection(const char *displayName)
{
    int primaryScreen;

    m_connection = xcb_connect(displayName, &primaryScreen);

    m_setup = xcb_get_setup(m_connection);

    xcb_screen_iterator_t it = xcb_setup_roots_iterator(m_setup);

    while (it.rem) {
        m_screens << new QXcbScreen(this, it.data);
        xcb_screen_next(&it);
    }

    QSocketNotifier *socket = new QSocketNotifier(xcb_get_file_descriptor(m_connection), QSocketNotifier::Read, this);
    connect(socket, SIGNAL(activated(int)), this, SLOT(eventDispatcher()));
}

QXcbConnection::~QXcbConnection()
{
    qDeleteAll(m_screens);

    xcb_disconnect(m_connection);
}

QXcbWindow *platformWindowFromId(xcb_window_t id)
{
    QWidget *widget = QWidget::find(id);
    if (widget)
        return static_cast<QXcbWindow *>(widget->platformWindow());
    return 0;
}

#define HANDLE_PLATFORM_WINDOW_EVENT(event_t, window, handler) \
{ \
    event_t *e = (event_t *)event; \
    if (QXcbWindow *platformWindow = platformWindowFromId(e->window)) \
        platformWindow->handler(e); \
} \
break;

void QXcbConnection::eventDispatcher()
{
    while (xcb_generic_event_t *event = xcb_poll_for_event(m_connection)) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_expose_event_t, window, handleExposeEvent);
        case XCB_BUTTON_PRESS:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_button_press_event_t, event, handleButtonPressEvent);
        case XCB_BUTTON_RELEASE:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_button_release_event_t, event, handleButtonReleaseEvent);
        case XCB_MOTION_NOTIFY:
            HANDLE_PLATFORM_WINDOW_EVENT(xcb_motion_notify_event_t, event, handleMotionNotifyEvent);
        default:
            break;

        }
    }
}
