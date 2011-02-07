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

#include "qxcbwindow.h"

#include "qxcbconnection.h"
#include "qxcbscreen.h"

#include <private/qwindowsurface_p.h>

#include <QtGui/QWindowSystemInterface>

#include <stdio.h>

QXcbWindow::QXcbWindow(QWidget *tlw)
    : QPlatformWindow(tlw)
{
    m_screen = static_cast<QXcbScreen *>(QPlatformScreen::platformScreenForWidget(tlw));

    const quint32 mask = XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
    const quint32 values[] = {
        // XCB_CW_BACK_PIXMAP
        XCB_NONE,
        // XCB_CW_EVENT_MASK
        XCB_EVENT_MASK_EXPOSURE
        | XCB_EVENT_MASK_BUTTON_PRESS
        | XCB_EVENT_MASK_BUTTON_RELEASE
        | XCB_EVENT_MASK_BUTTON_MOTION
    };

    m_window = xcb_generate_id(connection());
    xcb_create_window(connection(),
                      XCB_COPY_FROM_PARENT,            // depth -- same as root
                      m_window,                        // window id
                      m_screen->root(),                // parent window id
                      tlw->x(),
                      tlw->y(),
                      tlw->width(),
                      tlw->height(),
                      0,                               // border width
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,   // window class
                      m_screen->screen()->root_visual, // visual
                      mask,                            // value mask
                      values);                         // value list

    printf("m_window: %d\n", m_window);
}

QXcbWindow::~QXcbWindow()
{
    xcb_destroy_window(connection(), m_window);
}

void QXcbWindow::setGeometry(const QRect &rect)
{
    QPlatformWindow::setGeometry(rect);
}

void QXcbWindow::setVisible(bool visible)
{
    if (visible)
        xcb_map_window(connection(), m_window);
    else
        xcb_unmap_window(connection(), m_window);
}

Qt::WindowFlags QXcbWindow::setWindowFlags(Qt::WindowFlags flags)
{
    return flags;
}

Qt::WindowFlags QXcbWindow::windowFlags() const
{
    return 0;
}

WId QXcbWindow::winId() const
{
    return m_window;
}

void QXcbWindow::setParent(const QPlatformWindow *)
{
}

void QXcbWindow::setWindowTitle(const QString &)
{
}

void QXcbWindow::raise()
{
}

void QXcbWindow::lower()
{
}

void QXcbWindow::requestActivateWindow()
{
}

xcb_connection_t *QXcbWindow::connection() const
{
    return m_screen->connection()->connection();
}

void QXcbWindow::handleExposeEvent(xcb_expose_event_t *event)
{
    if (event->count != 0)
        return;

    QWindowSurface *surface = widget()->windowSurface();
    if (surface)
        surface->flush(widget(), widget()->geometry(), QPoint());
}

static Qt::MouseButtons translateMouseButtons(int s)
{
    Qt::MouseButtons ret = 0;
    if (s & XCB_BUTTON_MASK_1)
        ret |= Qt::LeftButton;
    if (s & XCB_BUTTON_MASK_2)
        ret |= Qt::MidButton;
    if (s & XCB_BUTTON_MASK_3)
        ret |= Qt::RightButton;
    return ret;
}

static Qt::MouseButton translateMouseButton(xcb_button_t s)
{
    switch (s) {
    case 1:
        return Qt::LeftButton;
    case 2:
        return Qt::MidButton;
    case 3:
        return Qt::RightButton;
    default:
        return Qt::NoButton;
    }
}

void QXcbWindow::handleButtonPressEvent(xcb_button_press_event_t *event)
{
    QPoint local(event->event_x, event->event_y);
    QPoint global(event->root_x, event->root_y);

    Qt::KeyboardModifiers modifiers = Qt::NoModifier;

    if (event->detail >= 4 && event->detail <= 7) {
        //logic borrowed from qapplication_x11.cpp
        int delta = 120 * ((event->detail == 4 || event->detail == 6) ? 1 : -1);
        bool hor = (((event->detail == 4 || event->detail == 5)
                     && (modifiers & Qt::AltModifier))
                    || (event->detail == 6 || event->detail == 7));

        QWindowSystemInterface::handleWheelEvent(widget(), event->time,
                                                 local, global, delta, hor ? Qt::Horizontal : Qt::Vertical);
        return;
    }

    handleMouseEvent(event->detail, event->state, event->time, local, global);
}

void QXcbWindow::handleButtonReleaseEvent(xcb_button_release_event_t *event)
{
    QPoint local(event->event_x, event->event_y);
    QPoint global(event->root_x, event->root_y);

    handleMouseEvent(event->detail, event->state, event->time, local, global);
}

void QXcbWindow::handleMotionNotifyEvent(xcb_motion_notify_event_t *event)
{
    QPoint local(event->event_x, event->event_y);
    QPoint global(event->root_x, event->root_y);

    handleMouseEvent(event->detail, event->state, event->time, local, global);
}

void QXcbWindow::handleMouseEvent(xcb_button_t detail, uint16_t state, xcb_timestamp_t time, const QPoint &local, const QPoint &global)
{
    Qt::MouseButtons buttons = translateMouseButtons(state);
    Qt::MouseButton button = translateMouseButton(detail);

    buttons ^= button; // X event uses state *before*, Qt uses state *after*

    QWindowSystemInterface::handleMouseEvent(widget(), time, local, global, buttons);
}

