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

#include "qwaylandintegration.h"
#include "qwaylandwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

#include <wayland-client.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#define MESA_EGL_NO_X11_HEADERS

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

QT_BEGIN_NAMESPACE

QWaylandShmBuffer::QWaylandShmBuffer(QWaylandDisplay *display,
				     const QSize &size, QImage::Format format)
{
    int stride = size.width() * 4;
    int alloc = stride * size.height();
    char filename[] = "/tmp/wayland-shm-XXXXXX";
    int fd = mkstemp(filename);
    if (fd < 0)
	qWarning("open %s failed: %s", filename, strerror(errno));
    if (ftruncate(fd, alloc) < 0) {
	qWarning("ftruncate failed: %s", strerror(errno));
	close(fd);
	return;
    }
    uchar *data = (uchar *)
	mmap(NULL, alloc, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    unlink(filename);

    if (data == (uchar *) MAP_FAILED) {
	qWarning("mmap /dev/zero failed: %s", strerror(errno));
	close(fd);
	return;
    }

    mImage = QImage(data, size.width(), size.height(), stride, format);
    mBuffer = display->createShmBuffer(fd, size.width(), size.height(),
				       stride, display->argbVisual());
    close(fd);
}

QWaylandShmBuffer::~QWaylandShmBuffer(void)
{
    munmap((void *) mImage.constBits(), mImage.byteCount());
    wl_buffer_destroy(mBuffer);
}

QWaylandShmWindowSurface::QWaylandShmWindowSurface(QWidget *window,
						   QWaylandDisplay *display)
    : QWindowSurface(window)
    , mBuffer(0)
    , mDisplay(display)
{
}

QWaylandShmWindowSurface::~QWaylandShmWindowSurface()
{
}

QPaintDevice *QWaylandShmWindowSurface::paintDevice()
{
    return &mBuffer->mImage;
}

void QWaylandShmWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QVector<QRect> rects = region.rects();
    const QRect *r;
    int i;

    for (i = 0; i < rects.size(); i++) {
	r = &rects.at(i);
	wl_surface_damage(ww->surface(),
			  r->x(), r->y(), r->width(), r->height());
    }
}

void QWaylandShmWindowSurface::resize(const QSize &size)
{
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QWindowSurface::resize(size);
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();

    if (mBuffer != NULL && mBuffer->mImage.size() == size)
	return;

    if (mBuffer != NULL)
	delete mBuffer;

    mBuffer = new QWaylandShmBuffer(mDisplay, size, format);

    ww->attach(mBuffer);
}


class QWaylandDrmBuffer : public QWaylandBuffer {
public:
    QWaylandDrmBuffer(QWaylandDisplay *display,
		       const QSize &size, QImage::Format format);
    ~QWaylandDrmBuffer();
    EGLImageKHR mImage;
    GLuint mTexture;
    QWaylandDisplay *mDisplay;
};

class QWaylandDrmWindowSurface : public QWindowSurface
{
public:
    QWaylandDrmWindowSurface(QWidget *window, QWaylandDisplay *display);
    ~QWaylandDrmWindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

private:
    QWaylandDrmBuffer *mBuffer;
    QWaylandDisplay *mDisplay;
};

QWaylandDrmBuffer::QWaylandDrmBuffer(QWaylandDisplay *display,
				     const QSize &size, QImage::Format format)
    : mDisplay(display)
{
    EGLint name, stride;
    EGLint imageAttribs[] = {
	EGL_WIDTH,			0,
	EGL_HEIGHT,			0,
	EGL_DRM_BUFFER_FORMAT_MESA,	EGL_DRM_BUFFER_FORMAT_ARGB32_MESA,
	EGL_DRM_BUFFER_USE_MESA,	EGL_DRM_BUFFER_USE_SCANOUT_MESA,
	EGL_NONE
    };

    imageAttribs[1] = size.width();
    imageAttribs[3] = size.height();
    mImage = eglCreateDRMImageMESA(mDisplay->eglDisplay(), imageAttribs);
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, mImage);

    eglExportDRMImageMESA(mDisplay->eglDisplay(),
			  mImage, &name, NULL, &stride);

    mBuffer = display->createDrmBuffer(name, size.width(), size.height(),
				       stride, display->argbVisual());
}

QWaylandDrmBuffer::~QWaylandDrmBuffer(void)
{
    glDeleteTextures(1, &mTexture);
    eglDestroyImageKHR(mDisplay->eglDisplay(), mImage);
    wl_buffer_destroy(mBuffer);
}


QWaylandDrmWindowSurface::QWaylandDrmWindowSurface(QWidget *window,
						   QWaylandDisplay *display)
    : QWindowSurface(window)
    , mBuffer(0)
    , mDisplay(display)
{
}

QWaylandDrmWindowSurface::~QWaylandDrmWindowSurface()
{
}

QPaintDevice *QWaylandDrmWindowSurface::paintDevice()
{
    return NULL;
}

void QWaylandDrmWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
}

void QWaylandDrmWindowSurface::resize(const QSize &size)
{
}

QT_END_NAMESPACE
