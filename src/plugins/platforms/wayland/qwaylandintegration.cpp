/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#include "qwaylandintegration.h"

#include "qwaylanddisplay.h"
#include "qwaylandshmsurface.h"
#include "qwaylandshmwindow.h"
#include "qwaylandnativeinterface.h"
#include "qwaylandclipboard.h"

#include "qgenericunixfontdatabase.h"

#include <QtGui/QWindowSystemInterface>
#include <QtGui/QPlatformCursor>
#include <QtGui/QPlatformWindowFormat>

#include <QtGui/private/qpixmap_raster_p.h>
#ifdef QT_WAYLAND_GL_SUPPORT
#include "gl_integration/qwaylandglintegration.h"
#include "gl_integration/qwaylandglwindowsurface.h"
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#endif

QWaylandIntegration::QWaylandIntegration(bool useOpenGL)
    : mFontDb(new QGenericUnixFontDatabase())
    , mDisplay(new QWaylandDisplay())
    , mUseOpenGL(useOpenGL)
    , mNativeInterface(new QWaylandNativeInterface)
    , mClipboard(0)
{
}

QPlatformNativeInterface * QWaylandIntegration::nativeInterface() const
{
    return mNativeInterface;
}

QList<QPlatformScreen *>
QWaylandIntegration::screens() const
{
    return mDisplay->screens();
}

bool QWaylandIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
    case OpenGL: return hasOpenGL();
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QWaylandIntegration::createPixmapData(QPixmapData::PixelType type) const
{
#ifdef QT_WAYLAND_GL_SUPPORT
    if (mUseOpenGL)
        return new QGLPixmapData(type);
#endif
    return new QRasterPixmapData(type);
}

QPlatformWindow *QWaylandIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
#ifdef QT_WAYLAND_GL_SUPPORT
    bool useOpenGL = mUseOpenGL || (widget->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL);
    if (useOpenGL)
        return mDisplay->eglIntegration()->createEglWindow(widget);
#endif
    return new QWaylandShmWindow(widget);
}

QWindowSurface *QWaylandIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    Q_UNUSED(winId);
#ifdef QT_WAYLAND_GL_SUPPORT
    bool useOpenGL = mUseOpenGL || (widget->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL);
    if (useOpenGL)
        return new QWaylandGLWindowSurface(widget);
#endif
    return new QWaylandShmWindowSurface(widget);
}

QPlatformFontDatabase *QWaylandIntegration::fontDatabase() const
{
    return mFontDb;
}

bool QWaylandIntegration::hasOpenGL() const
{
#ifdef QT_WAYLAND_GL_SUPPORT
    return true;
#else
    return false;
#endif
}

QPlatformClipboard *QWaylandIntegration::clipboard() const
{
    if (!mClipboard)
        mClipboard = new QWaylandClipboard(mDisplay);
    return mClipboard;
}
