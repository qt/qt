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

#include "qwaylandxcompositeeglintegration.h"

#include "qwaylandxcompositeeglwindow.h"

#include <QtCore/QDebug>

#include "wayland-xcomposite-client-protocol.h"

QWaylandGLIntegration * QWaylandGLIntegration::createGLIntegration(QWaylandDisplay *waylandDisplay)
{
    return new QWaylandXCompositeEGLIntegration(waylandDisplay);
}

QWaylandXCompositeEGLIntegration::QWaylandXCompositeEGLIntegration(QWaylandDisplay * waylandDispaly)
    : QWaylandGLIntegration()
    , mWaylandDisplay(waylandDispaly)
{
    qDebug() << "Using XComposite-EGL";
    wl_display_add_global_listener(mWaylandDisplay->wl_display(), QWaylandXCompositeEGLIntegration::wlDisplayHandleGlobal,
                                   this);
}

QWaylandXCompositeEGLIntegration::~QWaylandXCompositeEGLIntegration()
{
    XCloseDisplay(mDisplay);
}

void QWaylandXCompositeEGLIntegration::initialize()
{
}

QWaylandWindow * QWaylandXCompositeEGLIntegration::createEglWindow(QWidget *widget)
{
    return new QWaylandXCompositeEGLWindow(widget,this);
}

Display * QWaylandXCompositeEGLIntegration::xDisplay() const
{
    return mDisplay;
}

EGLDisplay QWaylandXCompositeEGLIntegration::eglDisplay() const
{
    return mEglDisplay;
}

int QWaylandXCompositeEGLIntegration::screen() const
{
    return mScreen;
}

Window QWaylandXCompositeEGLIntegration::rootWindow() const
{
    return mRootWindow;
}

QWaylandDisplay * QWaylandXCompositeEGLIntegration::waylandDisplay() const
{
    return mWaylandDisplay;
}
wl_xcomposite * QWaylandXCompositeEGLIntegration::waylandXComposite() const
{
    return mWaylandComposite;
}

const struct wl_xcomposite_listener QWaylandXCompositeEGLIntegration::xcomposite_listener = {
    QWaylandXCompositeEGLIntegration::rootInformation
};

void QWaylandXCompositeEGLIntegration::wlDisplayHandleGlobal(wl_display *display, uint32_t id, const char *interface, uint32_t version, void *data)
{
    Q_UNUSED(version);
    if (strcmp(interface, "wl_xcomposite") == 0) {
        QWaylandXCompositeEGLIntegration *integration = static_cast<QWaylandXCompositeEGLIntegration *>(data);
        integration->mWaylandComposite = wl_xcomposite_create(display,id,1);
        wl_xcomposite_add_listener(integration->mWaylandComposite,&xcomposite_listener,integration);
    }

}

void QWaylandXCompositeEGLIntegration::rootInformation(void *data, wl_xcomposite *xcomposite, const char *display_name, uint32_t root_window)
{
    Q_UNUSED(xcomposite);
    QWaylandXCompositeEGLIntegration *integration = static_cast<QWaylandXCompositeEGLIntegration *>(data);

    integration->mDisplay = XOpenDisplay(display_name);
    integration->mRootWindow = (Window) root_window;
    integration->mScreen = XDefaultScreen(integration->mDisplay);
    integration->mEglDisplay = eglGetDisplay(integration->mDisplay);
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint minor,major;
    if (!eglInitialize(integration->mEglDisplay,&major,&minor)) {
        qFatal("Failed to initialize EGL");
    }
    eglSwapInterval(integration->eglDisplay(),0);
    qDebug() << "ROOT INFORMATION" << integration->mDisplay << integration->mRootWindow << integration->mScreen;
}

