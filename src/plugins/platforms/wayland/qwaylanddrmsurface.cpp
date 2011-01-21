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
#include <QtOpenGL/private/qglpaintdevice_p.h>
#include <QtGui/private/qpaintengine_p.h>

#include <wayland-client.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

QT_BEGIN_NAMESPACE

class QWaylandPaintDevice : public QGLPaintDevice
{
public:
    QWaylandPaintDevice(QWaylandDisplay *display, QWaylandDrmWindowSurface *windowSurface, QPlatformGLContext *context)
        : QGLPaintDevice()
        , mDisplay(display)
        , mPlatformGLContext(context)
        , mWindowSurface(windowSurface)
    {
    }

    QSize size() const { return mWindowSurface->size(); }
    QGLContext *context() const { return QGLContext::fromPlatformGLContext(mPlatformGLContext); }
    void beginPaint() { mWindowSurface->currentPaintBuffer()->bindToCurrentContext(); }
    void ensureActiveTarget() { }
    void endPaint() { }
    QPaintEngine *paintEngine() const { return qt_qgl_paint_engine(); }
    bool isFlipped()const {return true;}


private:
    QWaylandDisplay *mDisplay;
    QPlatformGLContext  *mPlatformGLContext;
    QWaylandDrmWindowSurface *mWindowSurface;

};

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

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, mFbo);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mImage);
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mImage);

    glGenRenderbuffers(1,&mDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER_EXT,mDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER_EXT,
        GL_DEPTH24_STENCIL8_EXT, size.width(), size.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, mDepthStencil);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT,
                                 GL_RENDERBUFFER_EXT, mDepthStencil);

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

QWaylandDrmBuffer::~QWaylandDrmBuffer(void)
{
    glDeleteFramebuffers(1, &mFbo);
    glDeleteTextures(1, &mTexture);
    eglDestroyImageKHR(mDisplay->eglDisplay(), mImage);
    wl_buffer_destroy(mBuffer);
}

void QWaylandDrmBuffer::bindToCurrentContext()
{
    Q_ASSERT(QPlatformGLContext::currentContext());
    glViewport(0, 0, size().width(), size().height());
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, mFbo);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, mTexture, 0);
    QT_CHECK_GLERROR();

}

QWaylandDrmWindowSurface::QWaylandDrmWindowSurface(QWidget *window,
						   QWaylandDisplay *display)
    : QWindowSurface(window)
    , mBufferList(1) // there is something wrong with the buffer flush. keeping it single buffered for now.
    , mCurrentPaintBuffer(0)
    , mDisplay(display)
    , mPaintDevice(new QWaylandPaintDevice(mDisplay, this,window->platformWindow()->glContext()))
{
    for (int i = 0; i < mBufferList.size(); i++) {
        mBufferList[i] = 0;
    }
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

    QVector<QRect> rects = region.rects();
    for (int i = 0; i < rects.size(); i++) {
        QRect r = rects.at(i);
        wl_surface_damage(ww->surface(),
                          r.x(), r.y(), r.width(), r.height());
    }

    mCurrentPaintBuffer = ++mCurrentPaintBuffer % mBufferList.size();
    ww->attach(currentPaintBuffer());
}

void QWaylandDrmWindowSurface::resize(const QSize &requestedSize)
{
    QWindowSurface::resize(requestedSize);
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QWaylandScreen *screen = (QWaylandScreen *)QApplicationPrivate::platformIntegration()->screens().first();
    QImage::Format format = screen->format();

    ww->glContext()->makeCurrent();

    for (int i = 0; i < mBufferList.size(); i++) {
        if (!mBufferList.at(i) || mBufferList.at(i)->size() != requestedSize) {
            delete mBufferList[i];
            mBufferList[i] = new QWaylandDrmBuffer(mDisplay, requestedSize, format);
        }
    }

    ww->attach(currentPaintBuffer());
}

QWaylandDrmBuffer * QWaylandDrmWindowSurface::currentPaintBuffer() const
{
    return mBufferList[mCurrentPaintBuffer];
}

QT_END_NAMESPACE
