/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
#include "qwaylanddrmsurface.h"

#include "qwaylanddisplay.h"
#include "qwaylandwindow.h"
#include "qwaylandscreen.h"

#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>
#include <QtOpenGL/private/qgl_p.h>
#include <QtGui/private/qpaintengine_p.h>

#include <wayland-client.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

QT_BEGIN_NAMESPACE

/*
 * Shared DRM surface for GL based drawing
 */
QWaylandDrmBuffer::QWaylandDrmBuffer(QWaylandDisplay *display,
				     const QSize &size, QImage::Format format)
    : mDisplay(display)
    , mSize(size)
{
    struct wl_visual *visual;
    EGLint name, stride;
    EGLint imageAttribs[] = {
        EGL_WIDTH,			size.width(),
        EGL_HEIGHT,			size.height(),
	EGL_DRM_BUFFER_FORMAT_MESA,	EGL_DRM_BUFFER_FORMAT_ARGB32_MESA,
        EGL_DRM_BUFFER_USE_MESA,	EGL_DRM_BUFFER_USE_SCANOUT_MESA,
	EGL_NONE
    };

    mImage = eglCreateDRMImageMESA(mDisplay->eglDisplay(), imageAttribs);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mImage);
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mImage);

    eglExportDRMImageMESA(mDisplay->eglDisplay(),
                          mImage, &name, NULL, &stride);

    switch (format) {
    case QImage::Format_ARGB32:
	visual = display->argbVisual();
	break;
    case QImage::Format_ARGB32_Premultiplied:
	visual = display->argbPremultipliedVisual();
	break;
    default:
	qDebug("unsupported buffer format %d requested\n", format);
	visual = display->argbVisual();
	break;
    }

    mBuffer = display->createDrmBuffer(name, size.width(), size.height(),
				       stride, visual);
}

QWaylandDrmBuffer::~QWaylandDrmBuffer(void)
{
    glDeleteTextures(1, &mTexture);
    eglDestroyImageKHR(mDisplay->eglDisplay(), mImage);
    wl_buffer_destroy(mBuffer);
}

void QWaylandDrmBuffer::bindToCurrentFbo()
{
    Q_ASSERT(QPlatformGLContext::currentContext());
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, mTexture, 0);
    QT_CHECK_GLERROR();
}

QWaylandPaintDevice::QWaylandPaintDevice(QWaylandDisplay *display, QWindowSurface *windowSurface, QPlatformGLContext *context)
    : QGLPaintDevice()
    , mDisplay(display)
    , mPlatformGLContext(context)
    , mWindowSurface(windowSurface)
    , mBufferList(1)
    , mCurrentPaintBuffer(0)
    , mDepthStencil(0)
{
    for (int i = 0; i < mBufferList.size(); i++) {
        mBufferList[i] = 0;
    }

    mPlatformGLContext->makeCurrent();
    glGenFramebuffers(1, &m_thisFBO);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_thisFBO);

    if (windowSurface->size().isValid())
        resize(windowSurface->size());
}

QWaylandPaintDevice::~QWaylandPaintDevice()
{
    for(int i = 0; i < mBufferList.size(); i++) {
        delete mBufferList[i];
    }
    glDeleteRenderbuffers(1,&mDepthStencil);
    glDeleteFramebuffers(1,&m_thisFBO);
}

QSize QWaylandPaintDevice::size() const
{
    return mSize;
}
QGLContext *QWaylandPaintDevice::context() const
{
    return QGLContext::fromPlatformGLContext(mPlatformGLContext);
}
QPaintEngine *QWaylandPaintDevice::paintEngine() const
{
    return qt_qgl_paint_engine();
}

void QWaylandPaintDevice::beginPaint()
{
    QGLPaintDevice::beginPaint();
    currentDrmBuffer()->bindToCurrentFbo();
}

bool QWaylandPaintDevice::isFlipped()const
{
    return true;
}

void QWaylandPaintDevice::resize(const QSize &size)
{
    QImage::Format format = QPlatformScreen::platformScreenForWidget(mWindowSurface->window())->format();
    mPlatformGLContext->makeCurrent();
    mSize = size;
    for (int i = 0; i < mBufferList.size(); i++) {
        if (!mBufferList.at(i) || mBufferList.at(i)->size() != size) {
            delete mBufferList[i];
            mBufferList[i] = new QWaylandDrmBuffer(mDisplay, size, format);
        }
    }

    glDeleteRenderbuffers(1,&mDepthStencil);
    glGenRenderbuffers(1,&mDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT,mDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER_EXT,
                          GL_DEPTH24_STENCIL8_EXT, size.width(), size.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                              GL_RENDERBUFFER_EXT, mDepthStencil);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                              GL_RENDERBUFFER_EXT, mDepthStencil);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);

    switch(status) {
    case GL_NO_ERROR:
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        qDebug() << "fbo ok";
        break;
    default:
        qDebug() <<"QWaylandDrmBuffer error: "<< status;
        break;
    }
    QT_CHECK_GLERROR();
}

QWaylandDrmBuffer *QWaylandPaintDevice::currentDrmBuffer() const
{
    return mBufferList[mCurrentPaintBuffer];
}
QWaylandDrmBuffer *QWaylandPaintDevice::currentDrmBufferAndSwap()
{
    QWaylandDrmBuffer *currentDrmBuffer = mBufferList[mCurrentPaintBuffer];
    mCurrentPaintBuffer = (mCurrentPaintBuffer +1) % mBufferList.size();
    return currentDrmBuffer;
}

QWaylandDrmWindowSurface::QWaylandDrmWindowSurface(QWidget *window)
    : QWindowSurface(window)
    , mDisplay(QWaylandScreen::waylandScreenFromWidget(window)->display())
    , mPaintDevice(new QWaylandPaintDevice(mDisplay, this,window->platformWindow()->glContext()))
{

}

QWaylandDrmWindowSurface::~QWaylandDrmWindowSurface()
{
    delete mPaintDevice;
}

void QWaylandDrmWindowSurface::beginPaint(const QRegion &)
{
    window()->platformWindow()->glContext()->makeCurrent();
}

QPaintDevice *QWaylandDrmWindowSurface::paintDevice()
{
    return mPaintDevice;
}

void QWaylandDrmWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(offset);
    QWaylandWindow *ww = (QWaylandWindow *) widget->platformWindow();

    ww->attach(mPaintDevice->currentDrmBufferAndSwap());

    QVector<QRect> rects = region.rects();
    for (int i = 0; i < rects.size(); i++) {
        QRect r = rects.at(i);
        wl_surface_damage(ww->surface(),
                          r.x(), r.y(), r.width(), r.height());
    }
}

void QWaylandDrmWindowSurface::resize(const QSize &requestedSize)
{
    QWindowSurface::resize(requestedSize);
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();

    ww->glContext()->makeCurrent();

    mPaintDevice->resize(requestedSize);
    ww->attach(mPaintDevice->currentDrmBuffer());
}

QT_END_NAMESPACE
