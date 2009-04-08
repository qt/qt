/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include "qegl_p.h"

#if defined(QT_OPENGL_ES) || defined(QT_OPENVG)

#include <qglscreen_qws.h>
#include <qscreenproxy_qws.h>
#include <private/qglwindowsurface_qws_p.h>
#include <qapplication.h>
#include <qdesktopwidget.h>

QT_BEGIN_NAMESPACE

static QGLScreen *glScreenForDevice(QPaintDevice *device)
{
    QScreen *screen = qt_screen;
    if (screen->classId() == QScreen::MultiClass) {
        int screenNumber;
        if (device && device->devType() == QInternal::Widget)
            screenNumber = qApp->desktop()->screenNumber(static_cast<QWidget *>(device));
        else
            screenNumber = 0;
        screen = screen->subScreens()[screenNumber];
    }
    while (screen->classId() == QScreen::ProxyClass) {
        screen = static_cast<QProxyScreen *>(screen)->screen();
    }
    if (screen->classId() == QScreen::GLClass)
        return static_cast<QGLScreen *>(screen);
    else
        return 0;
}

// Create the surface for a QPixmap, QImage, or QWidget.
bool QEglContext::createSurface(QPaintDevice *device)
{
    // Get the screen surface functions, which are used to create native ids.
    QGLScreen *glScreen = glScreenForDevice(device);
    if (!glScreen)
        return false;
    QGLScreenSurfaceFunctions *funcs = glScreen->surfaceFunctions();
    if (!funcs)
        return false;

    // Create the native drawable for the paint device.
    int devType = device->devType();
    EGLNativePixmapType pixmapDrawable = 0;
    EGLNativeWindowType windowDrawable = 0;
    bool ok;
    if (devType == QInternal::Pixmap) {
        ok = funcs->createNativePixmap(static_cast<QPixmap *>(device), &pixmapDrawable);
    } else if (devType == QInternal::Image) {
        ok = funcs->createNativeImage(static_cast<QImage *>(device), &pixmapDrawable);
    } else {
        ok = funcs->createNativeWindow(static_cast<QWidget *>(device), &windowDrawable);
    }
    if (!ok) {
        qWarning("QEglContext::createSurface(): Cannot create the native EGL drawable");
        return false;
    }

    // Create the EGL surface to draw into, based on the native drawable.
    if (devType == QInternal::Widget)
        surf = eglCreateWindowSurface(dpy, cfg, windowDrawable, 0);
    else
        surf = eglCreatePixmapSurface(dpy, cfg, pixmapDrawable, 0);
    if (surf == EGL_NO_SURFACE) {
        qWarning("QEglContext::createSurface(): Unable to create EGL surface, error = 0x%x", eglGetError());
        return false;
    }
    return true;
}

EGLDisplay QEglContext::getDisplay(QPaintDevice *device)
{
    Q_UNUSED(device);
    return eglGetDisplay(EGLNativeDisplayType(EGL_DEFAULT_DISPLAY));
}

QT_END_NAMESPACE

#endif // QT_OPENGL_ES || QT_OPENVG
