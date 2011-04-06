/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qwaylanddisplay.h"

#include "qwaylandwindow.h"
#include "qwaylandscreen.h"
#include "qwaylandcursor.h"
#include "qwaylandinputdevice.h"
#include "qwaylandeventthread.h"

#ifdef QT_WAYLAND_GL_SUPPORT
#include "gl_integration/qwaylandglintegration.h"
#endif

#include <QtGui/QApplication>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

struct wl_surface *QWaylandDisplay::createSurface(void *handle)
{
    struct wl_surface * surface = wl_compositor_create_surface(mCompositor);
    wl_surface_set_user_data(surface, handle);
    return surface;
}

struct wl_buffer *QWaylandDisplay::createShmBuffer(int fd,
                                                   int width, int height,
                                                   uint32_t stride,
                                                   struct wl_visual *visual)
{
    return wl_shm_create_buffer(mShm, fd, width, height, stride, visual);
}

struct wl_visual *QWaylandDisplay::rgbVisual()
{
    return wl_display_get_rgb_visual(mDisplay);
}

struct wl_visual *QWaylandDisplay::argbVisual()
{
    return wl_display_get_argb_visual(mDisplay);
}

struct wl_visual *QWaylandDisplay::argbPremultipliedVisual()
{
    return wl_display_get_premultiplied_argb_visual(mDisplay);
}

#ifdef QT_WAYLAND_GL_SUPPORT
QWaylandGLIntegration * QWaylandDisplay::eglIntegration()
{
    return mEglIntegration;
}
#endif

void QWaylandDisplay::shellHandleConfigure(void *data, struct wl_shell *shell,
                                           uint32_t time, uint32_t edges,
                                           struct wl_surface *surface,
                                           int32_t width, int32_t height)
{
    Q_UNUSED(data);
    Q_UNUSED(shell);
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QWaylandWindow *ww = (QWaylandWindow *) wl_surface_get_user_data(surface);

    ww->configure(time, edges, 0, 0, width, height);
}

const struct wl_shell_listener QWaylandDisplay::shellListener = {
    QWaylandDisplay::shellHandleConfigure,
};

void QWaylandDisplay::displayHandleGlobal(uint32_t id, QByteArray interface, uint32_t version)
{
    Q_UNUSED(version);

    if (interface == "compositor") {
        mCompositor = wl_compositor_create(mDisplay, id);
    } else if (interface == "shm") {
        mShm = wl_shm_create(mDisplay, id);
    } else if (interface == "shell"){
        mShell = wl_shell_create(mDisplay, id);
        wl_shell_add_listener(mShell, &shellListener, this);
    } else if (interface == "input_device") {
        QWaylandInputDevice *inputDevice =
            new QWaylandInputDevice(mDisplay, id);
        mInputDevices.append(inputDevice);
    }
}

QWaylandDisplay::QWaylandDisplay(void)
{
    mDisplay = wl_display_connect(NULL);
    if (mDisplay == NULL) {
        qErrnoWarning(errno, "Failed to create display");
        qFatal("No wayland connection available.");
    }
#ifdef QT_WAYLAND_GL_SUPPORT
    mEglIntegration = QWaylandGLIntegration::createGLIntegration(this);
#endif

    mEventThread = new QWaylandEventThread(this);


    mEventThread->waitForScreens();

#ifdef QT_WAYLAND_GL_SUPPORT
    mEglIntegration->initialize();
#endif


}

QWaylandDisplay::~QWaylandDisplay(void)
{
#ifdef QT_WAYLAND_GL_SUPPORT
    delete mEglIntegration;
#endif
    delete mEventThread;
    wl_display_destroy(mDisplay);
}

void QWaylandDisplay::createNewScreen(struct wl_output *output, QRect geometry)
{
    QWaylandScreen *waylandScreen = new QWaylandScreen(this,output,geometry);
    mScreens.append(waylandScreen);
}

void QWaylandDisplay::syncCallback(wl_display_sync_func_t func, void *data)
{
    wl_display_sync_callback(mDisplay, func, data);
}

void QWaylandDisplay::frameCallback(wl_display_frame_func_t func, void *data)
{
    wl_display_frame_callback(mDisplay, func, data);
}

