/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include "qegl_p.h"

#if defined(QT_OPENGL_ES) || defined(QT_OPENVG)

#include <windows.h>


QT_BEGIN_NAMESPACE

bool QEglContext::createSurface(QPaintDevice *device)
{
    // Create the native drawable for the paint device.
    int devType = device->devType();
    EGLNativePixmapType pixmapDrawable = 0;
    EGLNativeWindowType windowDrawable = 0;
    bool ok;
    if (devType == QInternal::Pixmap) {
        pixmapDrawable = 0;
        ok = (pixmapDrawable != 0);
    } else if (devType == QInternal::Widget) {
        windowDrawable = (EGLNativeWindowType)(static_cast<QWidget *>(device))->winId();
        ok = (windowDrawable != 0);
    } else {
        ok = false;
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
    EGLDisplay dpy = 0;
    HWND win = ((QWidget*)device)->winId(); 
    HDC myDc = GetDC(win); 
    if (!myDc) { 
        qWarning("QEglContext::defaultDisplay(): WinCE display is not open"); 
    } 
    dpy = eglGetDisplay(EGLNativeDisplayType(myDc)); 
    if (dpy == EGL_NO_DISPLAY) { 
        qWarning("QEglContext::defaultDisplay(): Falling back to EGL_DEFAULT_DISPLAY"); 
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY); 
    }
    return dpy;
}

QT_END_NAMESPACE

#endif // QT_OPENGL_ES || QT_OPENVG
