/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglpixelbuffer.h>
#include "qgl_p.h"
#include "qgl_egl_p.h"
#include "qglpixelbuffer_p.h"

#ifdef Q_WS_X11
#include <QtGui/private/qpixmap_x11_p.h>
#endif

QT_BEGIN_NAMESPACE

void qt_eglproperties_set_glformat(QEglProperties& eglProperties, const QGLFormat& glFormat)
{
    // NOTE: QGLFormat uses a magic value of -1 to indicate "don't care", even when a buffer of that
    // type has been requested.
    if (glFormat.depth()) {
        int depthSize = glFormat.depthBufferSize();
        eglProperties.setValue(EGL_DEPTH_SIZE,  depthSize == -1 ? 1 : depthSize);
    }
    if (glFormat.stencil()) {
        int stencilSize = glFormat.stencilBufferSize();
        eglProperties.setValue(EGL_STENCIL_SIZE, stencilSize == -1 ? 1 : stencilSize);
    }
    if (glFormat.sampleBuffers()) {
        int sampleCount = glFormat.samples();
        eglProperties.setValue(EGL_SAMPLES, sampleCount == -1 ? 1 : sampleCount);
        eglProperties.setValue(EGL_SAMPLE_BUFFERS, 1);
    }
    if (glFormat.alpha()) {
        int alphaSize = glFormat.alphaBufferSize();
        eglProperties.setValue(EGL_ALPHA_SIZE, alphaSize == -1 ? 1 : alphaSize);
    }

    int redSize = glFormat.redBufferSize();
    int greenSize = glFormat.greenBufferSize();
    int blueSize = glFormat.blueBufferSize();
    int alphaSize = glFormat.alphaBufferSize();

    eglProperties.setValue(EGL_RED_SIZE,   redSize   > 0 ? redSize   : 1);
    eglProperties.setValue(EGL_GREEN_SIZE, greenSize > 0 ? greenSize : 1);
    eglProperties.setValue(EGL_BLUE_SIZE,  blueSize  > 0 ? blueSize  : 1);
    eglProperties.setValue(EGL_ALPHA_SIZE, alphaSize > 0 ? alphaSize : 0);
}


// Updates "format" with the parameters of the selected configuration.
void qt_egl_update_format(const QEglContext& context, QGLFormat& format)
{
    EGLint value = 0;

    if (context.configAttrib(EGL_RED_SIZE, &value))
        format.setRedBufferSize(value);
    if (context.configAttrib(EGL_GREEN_SIZE, &value))
        format.setGreenBufferSize(value);
    if (context.configAttrib(EGL_BLUE_SIZE, &value))
        format.setBlueBufferSize(value);
    if (context.configAttrib(EGL_ALPHA_SIZE, &value)) {
        format.setAlpha(value != 0);
        if (format.alpha())
            format.setAlphaBufferSize(value);
    }

    if (context.configAttrib(EGL_DEPTH_SIZE, &value)) {
        format.setDepth(value != 0);
        if (format.depth())
            format.setDepthBufferSize(value);
    }

    if (context.configAttrib(EGL_LEVEL, &value))
        format.setPlane(value);

    if (context.configAttrib(EGL_SAMPLE_BUFFERS, &value)) {
        format.setSampleBuffers(value != 0);
        if (format.sampleBuffers()) {
            context.configAttrib(EGL_SAMPLES, &value);
            format.setSamples(value);
        }
    }

    if (context.configAttrib(EGL_STENCIL_SIZE, &value)) {
        format.setStencil(value != 0);
        if (format.stencil())
            format.setStencilBufferSize(value);
    }

    // Clear the EGL error state because some of the above may
    // have errored out because the attribute is not applicable
    // to the surface type.  Such errors don't matter.
    QEgl::clearError();
}

bool QGLFormat::hasOpenGL()
{
    return true;
}

void QGLContext::reset()
{
    Q_D(QGLContext);
    if (!d->valid)
        return;
    d->cleanup();
    doneCurrent();
    if (d->eglContext) {
        d->destroyEglSurfaceForDevice();
        delete d->eglContext;
    }
    d->eglContext = 0;
    d->eglSurface = EGL_NO_SURFACE;
    d->crWin = false;
    d->sharing = false;
    d->valid = false;
    d->transpColor = QColor();
    d->initDone = false;
    QGLContextGroup::removeShare(this);
}

void QGLContext::makeCurrent()
{
    Q_D(QGLContext);
    if (!d->valid || !d->eglContext || d->eglSurfaceForDevice() == EGL_NO_SURFACE) {
        qWarning("QGLContext::makeCurrent(): Cannot make invalid context current");
        return;
    }

    if (d->eglContext->makeCurrent(d->eglSurfaceForDevice()))
        QGLContextPrivate::setCurrentContext(this);
}

void QGLContext::doneCurrent()
{
    Q_D(QGLContext);
    if (d->eglContext)
        d->eglContext->doneCurrent();

    QGLContextPrivate::setCurrentContext(0);
}


void QGLContext::swapBuffers() const
{
    Q_D(const QGLContext);
    if (!d->valid || !d->eglContext)
        return;

    d->eglContext->swapBuffers(d->eglSurfaceForDevice());
}

void QGLContextPrivate::destroyEglSurfaceForDevice()
{
    if (eglSurface != EGL_NO_SURFACE) {
#ifdef Q_WS_X11
        // Make sure we don't call eglDestroySurface on a surface which
        // was created for a different winId:
        if (paintDevice->devType() == QInternal::Widget) {
            QGLWidget* w = static_cast<QGLWidget*>(paintDevice);

            if (w->d_func()->eglSurfaceWindowId == w->winId())
                eglDestroySurface(eglContext->display(), eglSurface);
            else
                qWarning("WARNING: Potential EGL surface leak!");
        } else
#endif
            eglDestroySurface(eglContext->display(), eglSurface);
        eglSurface = EGL_NO_SURFACE;
    }
}

EGLSurface QGLContextPrivate::eglSurfaceForDevice() const
{
    // If a QPixmapData had to create the QGLContext, we don't have a paintDevice
    if (!paintDevice)
        return eglSurface;

#ifdef Q_WS_X11
    if (paintDevice->devType() == QInternal::Pixmap) {
        QPixmapData *pmd = static_cast<QPixmap*>(paintDevice)->data_ptr().data();
        if (pmd->classId() == QPixmapData::X11Class) {
            QX11PixmapData* x11PixmapData = static_cast<QX11PixmapData*>(pmd);
            return (EGLSurface)x11PixmapData->gl_surface;
        }
    }
#endif

    if (paintDevice->devType() == QInternal::Pbuffer) {
        QGLPixelBuffer* pbuf = static_cast<QGLPixelBuffer*>(paintDevice);
        return pbuf->d_func()->pbuf;
    }

    return eglSurface;
}

void QGLWidget::setMouseTracking(bool enable)
{
    QWidget::setMouseTracking(enable);
}

QColor QGLContext::overlayTransparentColor() const
{
    return d_func()->transpColor;
}

uint QGLContext::colorIndex(const QColor &c) const
{
    Q_UNUSED(c);
    return 0;
}

void QGLContext::generateFontDisplayLists(const QFont & fnt, int listBase)
{
    Q_UNUSED(fnt);
    Q_UNUSED(listBase);
}

void *QGLContext::getProcAddress(const QString &proc) const
{
    return (void*)eglGetProcAddress(reinterpret_cast<const char *>(proc.toLatin1().data()));
}

bool QGLWidgetPrivate::renderCxPm(QPixmap*)
{
    return false;
}

QT_END_NAMESPACE
