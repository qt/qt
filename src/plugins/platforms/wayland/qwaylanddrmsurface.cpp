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

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <QGLFramebufferObject>
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>
#include <QtOpenGL/private/qgl_p.h>
#include <QtOpenGL/private/qglpaintdevice_p.h>

#include "qwaylanddisplay.h"
#include "qwaylandwindowsurface.h"
#include "qwaylandwindow.h"
#include "qwaylandscreen.h"

#include <wayland-client.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

QT_BEGIN_NAMESPACE

class QWaylandPaintDevice : public QGLPaintDevice
{
public:
    QWaylandPaintDevice(QWaylandDisplay *display, QWidget *widget)
	: QGLPaintDevice(), mDisplay(display), mWidget(widget)
	{
	    QGLFormat format;
	    mContext = new QGLContext(format, widget);
	    mContext->create();
	}

    QSize size() const { return mWidget->size(); }
    QGLContext *context() const { return mContext; }
    QPaintEngine *paintEngine() const { return qt_qgl_paint_engine(); }

    void beginPaint();
    void endPaint();
private:
    QWaylandDisplay *mDisplay;
    QWidget *mWidget;
    QGLContext *mContext;
};

void QWaylandPaintDevice::beginPaint(void)
{
    QWaylandWindow *mWindow = (QWaylandWindow *)mWidget->platformWindow();
    QPlatformGLContext *ctx = mWindow->glContext();

    QGLPaintDevice::beginPaint();

    ctx->makeCurrent();
}

void QWaylandPaintDevice::endPaint(void)
{
    QWaylandWindow *mWindow = (QWaylandWindow *)mWidget->platformWindow();
    QPlatformGLContext *ctx = mWindow->glContext();
    QRect geometry = mWidget->geometry();

    ctx->doneCurrent();

    QGLPaintDevice::endPaint();
}

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
    static const EGLint contextAttribs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
    };
    EGLint imageAttribs[] = {
	EGL_WIDTH,			0,
	EGL_HEIGHT,			0,
	EGL_DRM_BUFFER_FORMAT_MESA,	EGL_DRM_BUFFER_FORMAT_ARGB32_MESA,
	EGL_DRM_BUFFER_USE_MESA,	EGL_DRM_BUFFER_USE_SCANOUT_MESA,
	EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);
    mContext = eglCreateContext(mDisplay->eglDisplay(), NULL,
				EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(mDisplay->eglDisplay(), 0, 0, mContext);

    imageAttribs[1] = size.width();
    imageAttribs[3] = size.height();
    mImage = eglCreateDRMImageMESA(mDisplay->eglDisplay(), imageAttribs);
    glGenFramebuffers(1, &mFbo);
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
    glDeleteFramebuffers(1, &mFbo);
    glDeleteTextures(1, &mTexture);
    eglDestroyImageKHR(mDisplay->eglDisplay(), mImage);
    wl_buffer_destroy(mBuffer);
    eglDestroyContext(mDisplay->eglDisplay(), mContext);
}


QWaylandDrmWindowSurface::QWaylandDrmWindowSurface(QWidget *window,
						   QWaylandDisplay *display)
    : QWindowSurface(window)
    , mBuffer(0)
    , mDisplay(display)
{
    QWaylandWindow *ww = (QWaylandWindow *) window->platformWindow();
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();

    mPaintDevice = new QWaylandPaintDevice(display, window);
    mBuffer = new QWaylandDrmBuffer(mDisplay, window->size(), format);
    ww->attach(mBuffer);
}

QWaylandDrmWindowSurface::~QWaylandDrmWindowSurface()
{
    delete mPaintDevice;
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
    const QRect *r;
    int i;

    for (i = 0; i < rects.size(); i++) {
	r = &rects.at(i);
	wl_surface_damage(ww->surface(),
			  r->x(), r->y(), r->width(), r->height());
    }
}

void QWaylandDrmWindowSurface::resize(const QSize &requestedSize)
{
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QWaylandScreen *screen = (QWaylandScreen *)QApplicationPrivate::platformIntegration()->screens().first();
    QImage::Format format = screen->format();
    QSize screenSize = screen->geometry().size();
    QSize size = requestedSize;

    if (mBuffer != NULL && mBuffer->mSize == size)
	return;

    if (mBuffer != NULL)
	delete mBuffer;

    /* Clamp to screen size */
    if (size.width() > screenSize.width())
	size.setWidth(screenSize.width());
    if (size.height() > screenSize.height())
	size.setHeight(screenSize.height());

    QWindowSurface::resize(size);

    mBuffer = new QWaylandDrmBuffer(mDisplay, size, format);

    ww->attach(mBuffer);
}

QT_END_NAMESPACE
