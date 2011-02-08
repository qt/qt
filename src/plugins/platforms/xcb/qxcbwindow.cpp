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

    setConnection(m_screen->connection());

    const quint32 mask = XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK;
    const quint32 values[] = {
        // XCB_CW_BACK_PIXMAP
        XCB_NONE,
        // XCB_CW_EVENT_MASK
        XCB_EVENT_MASK_EXPOSURE
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY
        | XCB_EVENT_MASK_KEY_PRESS
        | XCB_EVENT_MASK_KEY_RELEASE
        | XCB_EVENT_MASK_BUTTON_PRESS
        | XCB_EVENT_MASK_BUTTON_RELEASE
        | XCB_EVENT_MASK_BUTTON_MOTION
        | XCB_EVENT_MASK_ENTER_WINDOW
        | XCB_EVENT_MASK_LEAVE_WINDOW
        | XCB_EVENT_MASK_PROPERTY_CHANGE
        | XCB_EVENT_MASK_FOCUS_CHANGE
    };

    m_window = xcb_generate_id(xcb_connection());
    xcb_create_window(xcb_connection(),
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

    xcb_atom_t properties[] = {
        atom(QXcbAtom::WM_DELETE_WINDOW)
    };

    xcb_change_property(xcb_connection(),
                        XCB_PROP_MODE_REPLACE,
                        m_window,
                        atom(QXcbAtom::WM_PROTOCOLS),
                        4,
                        32,
                        sizeof(properties) / sizeof(xcb_atom_t),
                        properties);
}

QXcbWindow::~QXcbWindow()
{
    xcb_destroy_window(xcb_connection(), m_window);
}

void QXcbWindow::setGeometry(const QRect &rect)
{
    QPlatformWindow::setGeometry(rect);

    const quint32 mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
    const quint32 values[] = { rect.x(), rect.y(), rect.width(), rect.height() };

    xcb_configure_window(xcb_connection(), m_window, mask, values);
}

void QXcbWindow::setVisible(bool visible)
{
    if (visible)
        xcb_map_window(xcb_connection(), m_window);
    else
        xcb_unmap_window(xcb_connection(), m_window);
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

void QXcbWindow::setParent(const QPlatformWindow *parent)
{
    QPoint topLeft = geometry().topLeft();
    xcb_reparent_window(xcb_connection(), window(), static_cast<const QXcbWindow *>(parent)->window(), topLeft.x(), topLeft.y());
}

void QXcbWindow::setWindowTitle(const QString &title)
{
    QByteArray ba = title.toUtf8();
    xcb_change_property (xcb_connection(),
                         XCB_PROP_MODE_REPLACE,
                         m_window,
                         atom(QXcbAtom::_NET_WM_NAME),
                         atom(QXcbAtom::UTF8_STRING),
                         8,
                         ba.length(),
                         ba.constData());
}

void QXcbWindow::raise()
{
    const quint32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    const quint32 values[] = { XCB_STACK_MODE_ABOVE };
    xcb_configure_window(xcb_connection(), m_window, mask, values);
}

void QXcbWindow::lower()
{
    const quint32 mask = XCB_CONFIG_WINDOW_STACK_MODE;
    const quint32 values[] = { XCB_STACK_MODE_BELOW };
    xcb_configure_window(xcb_connection(), m_window, mask, values);
}

void QXcbWindow::requestActivateWindow()
{
}

void QXcbWindow::handleExposeEvent(const xcb_expose_event_t *event)
{
    QWindowSurface *surface = widget()->windowSurface();
    if (surface) {
        QRect rect(event->x, event->y, event->width, event->height);

        surface->flush(widget(), rect, QPoint());
    }
}

void QXcbWindow::handleClientMessageEvent(const xcb_client_message_event_t *event)
{
    if (event->format == 32 && event->type == atom(QXcbAtom::WM_PROTOCOLS)) {
        if (event->data.data32[0] == atom(QXcbAtom::WM_DELETE_WINDOW)) {
            QWindowSystemInterface::handleCloseEvent(widget());
        }
    }
}

void QXcbWindow::handleConfigureNotifyEvent(const xcb_configure_notify_event_t *event)
{
    int xpos = geometry().x();
    int ypos = geometry().y();

    if ((event->width == geometry().width() && event->height == geometry().height()) || event->x != 0 || event->y != 0) {
        xpos = event->x;
        ypos = event->y;
    }

    QWindowSystemInterface::handleGeometryChange(widget(), QRect(xpos, ypos, event->width, event->height));
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

void QXcbWindow::handleButtonPressEvent(const xcb_button_press_event_t *event)
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

void QXcbWindow::handleButtonReleaseEvent(const xcb_button_release_event_t *event)
{
    QPoint local(event->event_x, event->event_y);
    QPoint global(event->root_x, event->root_y);

    handleMouseEvent(event->detail, event->state, event->time, local, global);
}

void QXcbWindow::handleMotionNotifyEvent(const xcb_motion_notify_event_t *event)
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

void QXcbWindow::handleEnterNotifyEvent(const xcb_enter_notify_event_t *)
{
    QWindowSystemInterface::handleEnterEvent(widget());
}

void QXcbWindow::handleLeaveNotifyEvent(const xcb_leave_notify_event_t *)
{
    QWindowSystemInterface::handleLeaveEvent(widget());
}

void QXcbWindow::handleFocusInEvent(const xcb_focus_in_event_t *)
{
    QWindowSystemInterface::handleWindowActivated(widget());
}

void QXcbWindow::handleFocusOutEvent(const xcb_focus_out_event_t *)
{
    QWindowSystemInterface::handleWindowActivated(0);
}

