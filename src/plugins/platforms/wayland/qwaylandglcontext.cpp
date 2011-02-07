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

#include "qwaylandglcontext.h"

#include "qwaylanddisplay.h"
#include "qwaylandwindow.h"
#include "qwaylanddrmsurface.h"

#include <QtGui/QPlatformGLContext>
#include <QtGui/QPlatformWindowFormat>

#include <unistd.h>
#include <fcntl.h>

extern "C" {
#include <xf86drm.h>
}

Q_GLOBAL_STATIC(QMutex,qt_defaultSharedContextMutex);

EGLint QWaylandGLContext::contextAttibutes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};

QWaylandGLContext::QWaylandGLContext(QWaylandDisplay *wd, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , mFormat(format)
    , mDisplay(wd)
{
    QPlatformGLContext *sharePlatformContext;
    if (format.useDefaultSharedContext()) {
        if (!QPlatformGLContext::defaultSharedContext()) {
            if (qt_defaultSharedContextMutex()->tryLock()){
                createDefaultSharedContex(wd);
                qt_defaultSharedContextMutex()->unlock();
            } else {
                qt_defaultSharedContextMutex()->lock(); //wait to the the shared context is created
                qt_defaultSharedContextMutex()->unlock();
            }
        }
        sharePlatformContext = QPlatformGLContext::defaultSharedContext();
    } else {
        sharePlatformContext = format.sharedGLContext();
    }
    mFormat.setSharedContext(sharePlatformContext);
    EGLContext shareEGLContext = EGL_NO_CONTEXT;
    if (sharePlatformContext)
        shareEGLContext = static_cast<const QWaylandGLContext*>(sharePlatformContext)->mContext;

    eglBindAPI(EGL_OPENGL_ES_API);
    mContext = eglCreateContext(mDisplay->eglDisplay(), NULL,
                                shareEGLContext, contextAttibutes);

    mFormat.setAccum(false);
    mFormat.setAlphaBufferSize(8);
    mFormat.setRedBufferSize(8);
    mFormat.setGreenBufferSize(8);
    mFormat.setBlueBufferSize(8);
    mFormat.setDepth(false);
//    mFormat.setDepthBufferSize(8);
    mFormat.setStencil(false);
//    mFormat.setStencilBufferSize(24);
//    mFormat.setSampleBuffers(false);

}

QWaylandGLContext::QWaylandGLContext()
    : QPlatformGLContext()
    , mDisplay(0)
    , mContext(EGL_NO_CONTEXT)
{ }

QWaylandGLContext::~QWaylandGLContext()
{
    eglDestroyContext(mDisplay->eglDisplay(),mContext);
}

void QWaylandGLContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();
    eglMakeCurrent(mDisplay->eglDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, mContext);
}

void QWaylandGLContext::doneCurrent()
{
    QPlatformGLContext::doneCurrent();
    eglMakeCurrent(mDisplay->eglDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

void QWaylandGLContext::swapBuffers()
{
}

void *QWaylandGLContext::getProcAddress(const QString &string)
{
    return (void *) eglGetProcAddress(string.toLatin1().data());
}

void QWaylandGLContext::createDefaultSharedContex(QWaylandDisplay *display)
{
    QWaylandGLContext *defaultSharedContext = new QWaylandGLContext;
    defaultSharedContext->mDisplay = display;
    defaultSharedContext->mContext = eglCreateContext(mDisplay->eglDisplay(), NULL,
                                                      EGL_NO_CONTEXT, contextAttibutes);
    QPlatformGLContext::setDefaultSharedContext(defaultSharedContext);
}

