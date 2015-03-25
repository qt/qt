/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qxcbintegration.h"
#include "qxcbconnection.h"
#include "qxcbscreen.h"
#include "qxcbwindow.h"
#include "qxcbwindowsurface.h"
#include "qxcbnativeinterface.h"

#include <xcb/xcb.h>

#include <private/qpixmap_raster_p.h>

#include "qgenericunixfontdatabase.h"

#include <stdio.h>

#ifdef XCB_USE_EGL
#include <EGL/egl.h>
#endif

QXcbIntegration::QXcbIntegration()
    : m_connection(new QXcbConnection)
{
    foreach (QXcbScreen *screen, m_connection->screens())
        m_screens << screen;

    m_fontDatabase = new QGenericUnixFontDatabase();
    m_nativeInterface = new QXcbNativeInterface;
}

QXcbIntegration::~QXcbIntegration()
{
    delete m_connection;
}

bool QXcbIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    case OpenGL: return hasOpenGL();
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QXcbIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QXcbIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QXcbWindow(widget);
}

QWindowSurface *QXcbIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QXcbWindowSurface(widget);
}

QList<QPlatformScreen *> QXcbIntegration::screens() const
{
    return m_screens;
}

void QXcbIntegration::moveToScreen(QWidget *window, int screen)
{
    Q_UNUSED(window);
    Q_UNUSED(screen);
}

bool QXcbIntegration::isVirtualDesktop()
{
    return false;
}

QPlatformFontDatabase *QXcbIntegration::fontDatabase() const
{
    return m_fontDatabase;
}

QPixmap QXcbIntegration::grabWindow(WId window, int x, int y, int width, int height) const
{
    Q_UNUSED(window);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
    return QPixmap();
}


bool QXcbIntegration::hasOpenGL() const
{
#if defined(XCB_USE_GLX)
    return true;
#elif defined(XCB_USE_EGL)
    return m_connection->hasEgl();
#elif defined(XCB_USE_DRI2)
    if (m_connection->hasSupportForDri2()) {
        return true;
    }
#endif
    return false;
}

QPlatformNativeInterface * QXcbIntegration::nativeInterface() const
{
    return m_nativeInterface;
}
