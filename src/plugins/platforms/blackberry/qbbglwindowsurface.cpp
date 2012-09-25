/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

//#define QBBGLWINDOWSURFACE_DEBUG


#include "qbbglwindowsurface.h"
#include "qbbglcontext.h"
#include "qbbwindow.h"

#include <QtGui/QWidget>
#include <QtOpenGL/private/qgl_p.h>
#include <QGLContext>
#include <QDebug>

#include <errno.h>

QT_BEGIN_NAMESPACE

QBBGLPaintDevice::QBBGLPaintDevice(QBBGLContext* platformGlContext)
    : mPlatformGlContext(platformGlContext)
{
    // create GL context from platform GL context
    mGlContext = QGLContext::fromPlatformGLContext(mPlatformGlContext);
}

QBBGLPaintDevice::~QBBGLPaintDevice()
{
    // cleanup GL context
    delete mGlContext;
}

QPaintEngine *QBBGLPaintDevice::paintEngine() const
{
    // select a paint engine based on configued OpenGL version
    return qt_qgl_paint_engine();
}

QSize QBBGLPaintDevice::size() const
{
    // get size of EGL surface
    return mPlatformGlContext->surfaceSize();
}

QBBGLWindowSurface::QBBGLWindowSurface(QWidget *window)
    : QWindowSurface(window)
{
#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::QBBGLWindowSurface - w=" << window;
#endif

    // extract GL context associated with window
    mPlatformGlContext = static_cast<QBBGLContext*>(window->platformWindow()->glContext());

    // create an OpenGL paint device
    mPaintDevice = new QBBGLPaintDevice(mPlatformGlContext);
}

QBBGLWindowSurface::~QBBGLWindowSurface()
{
#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::~QBBGLWindowSurface - w=" << window();
#endif

    // cleanup OpenGL paint device
    delete mPaintDevice;
}

void QBBGLWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::flush - w=" << window();
#endif

    // update the display with newly rendered content
    mPlatformGlContext->swapBuffers();
}

void QBBGLWindowSurface::resize(const QSize &size)
{
#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::resize - w=" << window() << ", s=" << size;
#endif

    // call parent method
    QWindowSurface::resize(size);

    // NOTE: defer resizing window buffers until next paint as
    // resize() can be called multiple times before a paint occurs
}

void QBBGLWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);

#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::beginPaint - w=" << window();
#endif

    // resize EGL surface if window surface resized
    QSize s = size();
    if (s != mPlatformGlContext->surfaceSize()) {
        mPlatformGlContext->resizeSurface(s);
    }
}

void QBBGLWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
#if defined(QBBGLWINDOWSURFACE_DEBUG)
    qDebug() << "QBBGLWindowSurface::endPaint - w=" << window();
#endif
}

QWindowSurface::WindowSurfaceFeatures QBBGLWindowSurface::features() const
{
    // force full frame updates on every paint
    return 0;
}

QT_END_NAMESPACE
