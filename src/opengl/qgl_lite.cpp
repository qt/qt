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

#include <QApplication>
#include <QPixmap>
#include <QDebug>

#include <QtGui/private/qapplication_p.h>

#include "qgl.h"
#include "qgl_p.h"
#include "qglplatformintegration_lite.h"

#ifndef QT_NO_EGL
#include "qgl_egl_p.h"

void qt_eglproperties_set_glformat(QEglProperties& eglProperties, const QGLFormat& glFormat)
{
    int redSize     = glFormat.redBufferSize();
    int greenSize   = glFormat.greenBufferSize();
    int blueSize    = glFormat.blueBufferSize();
    int alphaSize   = glFormat.alphaBufferSize();
    int depthSize   = glFormat.depthBufferSize();
    int stencilSize = glFormat.stencilBufferSize();
    int sampleCount = glFormat.samples();

    // QGLFormat uses a magic value of -1 to indicate "don't care", even when a buffer of that
    // type has been requested. So we must check QGLFormat's booleans too if size is -1:
    if (glFormat.alpha() && alphaSize <= 0)
        alphaSize = 1;
    if (glFormat.depth() && depthSize <= 0)
        depthSize = 1;
    if (glFormat.stencil() && stencilSize <= 0)
        stencilSize = 1;
    if (glFormat.sampleBuffers() && sampleCount <= 0)
        sampleCount = 1;

    // We want to make sure 16-bit configs are chosen over 32-bit configs as they will provide
    // the best performance. The EGL config selection algorithm is a bit stange in this regard:
    // The selection criteria for EGL_BUFFER_SIZE is "AtLeast", so we can't use it to discard
    // 32-bit configs completely from the selection. So it then comes to the sorting algorithm.
    // The red/green/blue sizes have a sort priority of 3, so they are sorted by first. The sort
    // order is special and described as "by larger _total_ number of color bits.". So EGL will
    // put 32-bit configs in the list before the 16-bit configs. However, the spec also goes on
    // to say "If the requested number of bits in attrib_list for a particular component is 0,
    // then the number of bits for that component is not considered". This part of the spec also
    // seems to imply that setting the red/green/blue bits to zero means none of the components
    // are considered and EGL disregards the entire sorting rule. It then looks to the next
    // highest priority rule, which is EGL_BUFFER_SIZE. Despite the selection criteria being
    // "AtLeast" for EGL_BUFFER_SIZE, it's sort order is "smaller" meaning 16-bit configs are
    // put in the list before 32-bit configs. So, to make sure 16-bit is preffered over 32-bit,
    // we must set the red/green/blue sizes to zero. This has an unfortunate consequence that
    // if the application sets the red/green/blue size to 5/6/5 on the QGLFormat, they will
    // probably get a 32-bit config, even when there's an RGB565 config avaliable. Oh well.

    // Now normalize the values so -1 becomes 0
    redSize   = redSize   > 0 ? redSize   : 0;
    greenSize = greenSize > 0 ? greenSize : 0;
    blueSize  = blueSize  > 0 ? blueSize  : 0;
    alphaSize = alphaSize > 0 ? alphaSize : 0;
    depthSize = depthSize > 0 ? depthSize : 0;
    stencilSize = stencilSize > 0 ? stencilSize : 0;
    sampleCount = sampleCount > 0 ? sampleCount : 0;

    eglProperties.setValue(EGL_RED_SIZE,   redSize);
    eglProperties.setValue(EGL_GREEN_SIZE, greenSize);
    eglProperties.setValue(EGL_BLUE_SIZE,  blueSize);
    eglProperties.setValue(EGL_ALPHA_SIZE, alphaSize);
    eglProperties.setValue(EGL_DEPTH_SIZE, depthSize);
    eglProperties.setValue(EGL_STENCIL_SIZE, stencilSize);
    eglProperties.setValue(EGL_SAMPLES, sampleCount);
    eglProperties.setValue(EGL_SAMPLE_BUFFERS, sampleCount ? 1 : 0);
}

// Updates "format" with the parameters of the selected configuration.
void qt_glformat_from_eglconfig(QGLFormat& format, const EGLConfig config)
{
    EGLint redSize     = 0;
    EGLint greenSize   = 0;
    EGLint blueSize    = 0;
    EGLint alphaSize   = 0;
    EGLint depthSize   = 0;
    EGLint stencilSize = 0;
    EGLint sampleCount = 0;
    EGLint level       = 0;

    EGLDisplay display = QEgl::display();
    eglGetConfigAttrib(display, config, EGL_RED_SIZE,     &redSize);
    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE,   &greenSize);
    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE,    &blueSize);
    eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE,   &alphaSize);
    eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE,   &depthSize);
    eglGetConfigAttrib(display, config, EGL_STENCIL_SIZE, &stencilSize);
    eglGetConfigAttrib(display, config, EGL_SAMPLES,      &sampleCount);
    eglGetConfigAttrib(display, config, EGL_LEVEL,        &level);

    format.setRedBufferSize(redSize);
    format.setGreenBufferSize(greenSize);
    format.setBlueBufferSize(blueSize);
    format.setAlphaBufferSize(alphaSize);
    format.setDepthBufferSize(depthSize);
    format.setStencilBufferSize(stencilSize);
    format.setSamples(sampleCount);
    format.setPlane(level + 1);      // EGL calls level 0 "normal" whereas Qt calls 1 "normal"
    format.setDirectRendering(true); // All EGL contexts are direct-rendered
    format.setRgba(true);            // EGL doesn't support colour index rendering
    format.setStereo(false);         // EGL doesn't support stereo buffers
    format.setAccumBufferSize(0);    // EGL doesn't support accululation buffers

    // Clear the EGL error state because some of the above may
    // have errored out because the attribute is not applicable
    // to the surface type.  Such errors don't matter.
    eglGetError();
}
#endif


QT_BEGIN_NAMESPACE

QPlatformGLContext::QPlatformGLContext()
{
}

QPlatformGLContext::~QPlatformGLContext()
{
}

QPlatformGLWidgetSurface::QPlatformGLWidgetSurface()
{
}

QPlatformGLWidgetSurface::~QPlatformGLWidgetSurface()
{
}

bool QPlatformGLWidgetSurface::filterEvent(QEvent*)
{
    // By default, return false to allow the event to pass through
    return false;
}


bool QGLFormat::hasOpenGL()
{
    return QApplicationPrivate::platformIntegration()->hasOpenGL();
}

bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);
    d->platformContext = QApplicationPrivate::platformIntegration()->createGLContext();
    d->valid = d->platformContext->create(d->paintDevice, d->glFormat, shareContext ? shareContext->d_func()->platformContext : 0);

    return d->valid;
}

void QGLContext::reset()
{
    Q_D(QGLContext);
    if (!d->valid)
        return;
    d->cleanup();
    doneCurrent();

    if (d->platformContext) {
        delete d->platformContext;
        d->platformContext = 0;
    }

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
    d->platformContext->makeCurrent();
    QGLContextPrivate::setCurrentContext(this);
}

void QGLContext::doneCurrent()
{
    Q_D(QGLContext);
    d->platformContext->doneCurrent();
    QGLContextPrivate::setCurrentContext(0);
}

void QGLContext::swapBuffers() const
{
    Q_D(const QGLContext);
    d->platformContext->swapBuffers();
}

void *QGLContext::getProcAddress(const QString &procName) const
{
    Q_D(const QGLContext);
    return d->platformContext->getProcAddress(procName);
}

void QGLWidget::setContext(QGLContext *context,
                            const QGLContext* shareContext,
                            bool deleteOldContext)
{
    Q_D(QGLWidget);
    if (context == 0) {
        qWarning("QGLWidget::setContext: Cannot set null context");
        return;
    }
    if (!context->deviceIsPixmap() && context->device() != this) {
        qWarning("QGLWidget::setContext: Context must refer to this widget");
        return;
    }

    if (d->glcx)
        d->glcx->doneCurrent();
    QGLContext* oldcx = d->glcx;
    d->glcx = context;

    if (!d->wsurf) {
        // If the application has set WA_TranslucentBackground and not explicitly set
        // the alpha buffer size to zero, modify the format so it have an alpha channel
        QGLFormat format = d->glcx->d_func()->glFormat;
        if (testAttribute(Qt::WA_TranslucentBackground) && format.alphaBufferSize() == -1)
            format.setAlphaBufferSize(1);

        d->wsurf = QApplicationPrivate::platformIntegration()->createGLWidgetSurface();
        d->wsurf->create(this, format);
        d->glcx->d_func()->glFormat = format;
    }

    if (!d->glcx->isValid())
        d->glcx->create(shareContext ? shareContext : oldcx);

    if (deleteOldContext)
        delete oldcx;
}

QPlatformGLWidgetSurface* QGLWidget::platformSurface()
{
    Q_D(QGLWidget);
    return d->wsurf;
}

void QGLWidgetPrivate::init(QGLContext *context, const QGLWidget *shareWidget)
{
    initContext(context, shareWidget);
}

bool QGLFormat::hasOpenGLOverlays()
{
    return false;
}

QColor QGLContext::overlayTransparentColor() const
{
    return QColor(); // Invalid color
}

uint QGLContext::colorIndex(const QColor&) const
{
    return 0;
}

void QGLContext::generateFontDisplayLists(const QFont & fnt, int listBase)
{
    Q_UNUSED(fnt);
    Q_UNUSED(listBase);
}

/*
    QGLTemporaryContext implementation
*/
class QGLTemporaryContextPrivate
{
public:
    QGLWidget *widget;
};

QGLTemporaryContext::QGLTemporaryContext(bool, QWidget *)
    : d(new QGLTemporaryContextPrivate)
{
    d->widget = new QGLWidget;
    d->widget->makeCurrent();
}

QGLTemporaryContext::~QGLTemporaryContext()
{
    delete d->widget;
}


bool QGLWidgetPrivate::renderCxPm(QPixmap*)
{
    return false;
}

/*! \internal
  Free up any allocated colormaps. This fn is only called for
  top-level widgets.
*/
void QGLWidgetPrivate::cleanupColormaps()
{
}

void QGLWidget::setMouseTracking(bool enable)
{
    Q_UNUSED(enable);
}

bool QGLWidget::event(QEvent *e)
{
    Q_D(QGLWidget);

    if (d->wsurf) {
        bool eventFiltered = d->wsurf->filterEvent(e);
        if (eventFiltered)
            return true;
    }

    return QWidget::event(e);
}

void QGLWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QGLWidget);
    if (!isValid())
        return;

    if (!d->wsurf) {
        qWarning("QGLWidget::resizeEvent() - widget does not have a platform surface");
        return;
    }
    d->wsurf->setGeometry(geometry()); //### What about moveEvent?

    makeCurrent();
    if (!d->glcx->initialized())
        glInit();
    resizeGL(width(), height());
}


const QGLContext* QGLWidget::overlayContext() const
{
    return 0;
}

void QGLWidget::makeOverlayCurrent()
{
}


void QGLWidget::updateOverlayGL()
{
}

const QGLColormap & QGLWidget::colormap() const
{
    Q_D(const QGLWidget);
    return d->cmap;
}

void QGLWidget::setColormap(const QGLColormap & c)
{
    Q_UNUSED(c);
}

QT_END_NAMESPACE
