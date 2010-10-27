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

#ifndef QWINDOWSURFACE_WAYLAND_H
#define QWINDOWSURFACE_WAYLAND_H

#include <QGLFramebufferObject>
#include <QtGui/private/qwindowsurface_p.h>

#include <QtGui/QPlatformWindow>

#define MESA_EGL_NO_X11_HEADERS
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

class QWaylandDisplay;

class QWaylandBuffer {
public:
    QWaylandBuffer() { }
    virtual ~QWaylandBuffer() { }
    struct wl_buffer *mBuffer;
};

class QWaylandShmBuffer : public QWaylandBuffer {
public:
    QWaylandShmBuffer(QWaylandDisplay *display,
		   const QSize &size, QImage::Format format);
    ~QWaylandShmBuffer();
    QImage mImage;
};

class QWaylandShmWindowSurface : public QWindowSurface
{
public:
    QWaylandShmWindowSurface(QWidget *window, QWaylandDisplay *display);
    ~QWaylandShmWindowSurface();

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

private:
    QWaylandShmBuffer *mBuffer;
    QWaylandDisplay *mDisplay;
};

class QWaylandDrmBuffer : public QWaylandBuffer {
public:
    QWaylandDrmBuffer(QWaylandDisplay *display,
		       const QSize &size, QImage::Format format);
    ~QWaylandDrmBuffer();
    EGLContext mContext;
    EGLImageKHR mImage;
    GLuint mTexture;
    QWaylandDisplay *mDisplay;
    QGLFramebufferObject *pdev;
    QSize mSize;
    GLuint mFbo;
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
    QPaintDevice *mPaintDevice;
};


QT_END_NAMESPACE

#endif
