/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include "qeglcontext_p.h"

#if !defined(QT_NO_EGL)

#include <QtGui/private/qgraphicssystem_p.h>
#include <QtGui/private/qapplication_p.h>
#include <QtGui/qdesktopwidget.h>

QT_BEGIN_NAMESPACE

EGLNativeDisplayType QEgl::nativeDisplay()
{
    return EGLNativeDisplayType(EGL_DEFAULT_DISPLAY);
}

EGLNativeWindowType QEgl::nativeWindow(QWidget* widget)
{
    return (EGLNativeWindowType)(widget->winId());
}

EGLNativePixmapType QEgl::nativePixmap(QPixmap* pixmap)
{
    Q_UNUSED(pixmap);
    return 0;
}

//EGLDisplay QEglContext::display()
//{
//    return eglGetDisplay(EGLNativeDisplayType(EGL_DEFAULT_DISPLAY));
//}

static QPlatformScreen *screenForDevice(QPaintDevice *device)
{
    QPlatformIntegration *pi = QApplicationPrivate::platformIntegration();

    QList<QPlatformScreen *> screens = pi->screens();

    int screenNumber;
    if (device && device->devType() == QInternal::Widget)
        screenNumber = qApp->desktop()->screenNumber(static_cast<QWidget *>(device));
    else
        screenNumber = 0;
    if (screenNumber < 0 || screenNumber >= screens.size())
        return 0;
    return screens[screenNumber];
}

// Set pixel format and other properties based on a paint device.
void QEglProperties::setPaintDeviceFormat(QPaintDevice *dev)
{
    if (!dev)
        return;

    // Find the QGLScreen for this paint device.
    QPlatformScreen *screen = screenForDevice(dev);
    if (!screen)
        return;
    int devType = dev->devType();
    if (devType == QInternal::Image)
        setPixelFormat(static_cast<QImage *>(dev)->format());
    else
        setPixelFormat(screen->format());
}

QT_END_NAMESPACE

#endif // !QT_NO_EGL
