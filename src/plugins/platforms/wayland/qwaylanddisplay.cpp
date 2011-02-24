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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

struct wl_surface *QWaylandDisplay::createSurface()
{
    return wl_compositor_create_surface(mCompositor);
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

struct wl_egl_display *QWaylandDisplay::nativeDisplay()
{
    return mNativeEglDisplay;
}

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

void QWaylandDisplay::outputHandleGeometry(void *data,
                                           struct wl_output *output,
                                           int32_t x, int32_t y,
                                           int32_t width, int32_t height)
{
    QWaylandDisplay *waylandDisplay = (QWaylandDisplay *) data;

    QRect outputRect = QRect(x, y, width, height);
    waylandDisplay->createNewScreen(output, outputRect);
}

const struct wl_output_listener QWaylandDisplay::outputListener = {
    QWaylandDisplay::outputHandleGeometry
};

void QWaylandDisplay::displayHandleGlobal(struct wl_display *display,
                                          uint32_t id,
                                          const char *interface,
                                          uint32_t version, void *data)
{
    Q_UNUSED(version);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    if (strcmp(interface, "compositor") == 0) {
        qwd->mCompositor = wl_compositor_create(display, id);
    } else if (strcmp(interface, "shm") == 0) {
        qwd->mShm = wl_shm_create(display, id);
    } else if (strcmp(interface, "shell") == 0) {
        qwd->mShell = wl_shell_create(display, id);
        wl_shell_add_listener(qwd->mShell, &shellListener, qwd);
    } else if (strcmp(interface, "output") == 0) {
        struct wl_output *output = wl_output_create(display, id);
        wl_output_add_listener(output, &outputListener, qwd);
    } else if (strcmp(interface, "input_device") == 0) {
        QWaylandInputDevice *inputDevice =
            new QWaylandInputDevice(display, id);
        qwd->mInputDevices.append(inputDevice);
    }
}

void QWaylandDisplay::eventDispatcher(void)
{
    wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);
}

int
QWaylandDisplay::sourceUpdate(uint32_t mask, void *data)
{
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    /* FIXME: We get a callback here when we ask wl_display for the
     * fd, but at that point we don't have the socket notifier as we
     * need the fd to create that.  We'll probably need to split that
     * API into get_fd and set_update_func functions. */
    if (qwd->mWriteNotifier == NULL)
        return 0;

    qwd->mWriteNotifier->setEnabled(mask & WL_DISPLAY_WRITABLE);

    return 0;
}

void QWaylandDisplay::flushRequests(void)
{
    wl_display_iterate(mDisplay, WL_DISPLAY_WRITABLE);
}

QWaylandDisplay::QWaylandDisplay(void)
    : mWriteNotifier(0)
{
#ifdef QT_WAYLAND_GL_SUPPORT
    EGLint major, minor;
#endif
    mDisplay = wl_display_connect(NULL);
    if (mDisplay == NULL) {
        fprintf(stderr, "failed to create display: %m\n");
        return;
    }

    wl_display_add_global_listener(mDisplay,
                                   QWaylandDisplay::displayHandleGlobal, this);

#ifdef QT_WAYLAND_GL_SUPPORT
    mNativeEglDisplay = wl_egl_display_create(mDisplay);

    mEglDisplay = eglGetDisplay((EGLNativeDisplayType)mNativeEglDisplay);
    if (mEglDisplay == NULL) {
        qWarning("EGL not available");
    } else {
	if (!eglInitialize(mEglDisplay, &major, &minor)) {
	    qWarning("failed to initialize EGL display");
	    return;
	}
    }
#else
    mNativeEglDisplay = 0;
    mEglDisplay = 0;
#endif

    eventDispatcher();

    int fd = wl_display_get_fd(mDisplay, sourceUpdate, this);
    mReadNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(mReadNotifier,
            SIGNAL(activated(int)), this, SLOT(eventDispatcher()));

    mWriteNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    connect(mWriteNotifier,
            SIGNAL(activated(int)), this, SLOT(flushRequests()));
    mWriteNotifier->setEnabled(false);
}

QWaylandDisplay::~QWaylandDisplay(void)
{
    close(mFd);
#ifdef QT_WAYLAND_GL_SUPPORT
    eglTerminate(mEglDisplay);
#endif
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
