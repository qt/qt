/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qwaylandxcompositeglxcontext.h"

#include "qwaylandxcompositeglxwindow.h"
#include "qwaylandxcompositebuffer.h"

#include "wayland-xcomposite-client-protocol.h"
#include <QtCore/QDebug>

#include <X11/extensions/Xcomposite.h>

QWaylandXCompositeGLXContext::QWaylandXCompositeGLXContext(QWaylandXCompositeGLXIntegration *glxIntegration, QWaylandXCompositeGLXWindow *window)
    : QPlatformGLContext()
    , mGlxIntegration(glxIntegration)
    , mWindow(window)
    , mBuffer(0)
    , mXWindow(0)
    , mConfig(qglx_findConfig(glxIntegration->xDisplay(),glxIntegration->screen(),window->widget()->platformWindowFormat()))
    , mWaitingForSyncCallback(false)
{
    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(glxIntegration->xDisplay(),mConfig);
    mContext = glXCreateContext(glxIntegration->xDisplay(),visualInfo,0,TRUE);

    geometryChanged();
}

void QWaylandXCompositeGLXContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();
    glXMakeCurrent(mGlxIntegration->xDisplay(),mXWindow,mContext);
}

void QWaylandXCompositeGLXContext::doneCurrent()
{
    glXMakeCurrent(mGlxIntegration->xDisplay(),0,0);
    QPlatformGLContext::doneCurrent();
}

void QWaylandXCompositeGLXContext::swapBuffers()
{
    QSize size = mWindow->geometry().size();

    glXSwapBuffers(mGlxIntegration->xDisplay(),mXWindow);
    mWindow->damage(QRect(QPoint(0,0),size));
    mWindow->waitForFrameSync();
}

void * QWaylandXCompositeGLXContext::getProcAddress(const QString &procName)
{
    return (void *) glXGetProcAddress(reinterpret_cast<GLubyte *>(procName.toLatin1().data()));
}

QPlatformWindowFormat QWaylandXCompositeGLXContext::platformWindowFormat() const
{
    return qglx_platformWindowFromGLXFBConfig(mGlxIntegration->xDisplay(),mConfig,mContext);
}

void QWaylandXCompositeGLXContext::sync_function(void *data)
{
    QWaylandXCompositeGLXContext *that = static_cast<QWaylandXCompositeGLXContext *>(data);
    that->mWaitingForSyncCallback = false;
}

void QWaylandXCompositeGLXContext::waitForSync()
{
    wl_display_sync_callback(mGlxIntegration->waylandDisplay()->wl_display(),
                             QWaylandXCompositeGLXContext::sync_function,
                             this);
    mWaitingForSyncCallback = true;
    wl_display_sync(mGlxIntegration->waylandDisplay()->wl_display(),0);
    mGlxIntegration->waylandDisplay()->flushRequests();
    while (mWaitingForSyncCallback) {
        mGlxIntegration->waylandDisplay()->readEvents();
    }
}

void QWaylandXCompositeGLXContext::geometryChanged()
{
    QSize size(mWindow->geometry().size());
    if (size.isEmpty()) {
        //QGLWidget wants a context for a window without geometry
        size = QSize(1,1);
    }

    delete mBuffer;
    //XFreePixmap deletes the glxPixmap as well
    if (mXWindow) {
        XDestroyWindow(mGlxIntegration->xDisplay(),mXWindow);
    }

    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(mGlxIntegration->xDisplay(),mConfig);
    Colormap cmap = XCreateColormap(mGlxIntegration->xDisplay(),mGlxIntegration->rootWindow(),visualInfo->visual,AllocNone);

    XSetWindowAttributes a;
    a.background_pixel = WhitePixel(mGlxIntegration->xDisplay(), mGlxIntegration->screen());
    a.border_pixel = BlackPixel(mGlxIntegration->xDisplay(), mGlxIntegration->screen());
    a.colormap = cmap;
    mXWindow = XCreateWindow(mGlxIntegration->xDisplay(), mGlxIntegration->rootWindow(),0, 0, size.width(), size.height(),
                             0, visualInfo->depth, InputOutput, visualInfo->visual,
                             CWBackPixel|CWBorderPixel|CWColormap, &a);

    XCompositeRedirectWindow(mGlxIntegration->xDisplay(), mXWindow, CompositeRedirectManual);
    XMapWindow(mGlxIntegration->xDisplay(), mXWindow);

    XSync(mGlxIntegration->xDisplay(),False);
    mBuffer = new QWaylandXCompositeBuffer(mGlxIntegration->waylandXComposite(),
                                           (uint32_t)mXWindow,
                                           size,
                                           mGlxIntegration->waylandDisplay()->argbVisual());
    mWindow->attach(mBuffer);
    waitForSync();
}
