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
#include <QtGui/private/qapplication_p.h>
#include <QPixmap>
#include <QDebug>

#include <QtGui/private/qapplication_p.h>
#include <QtGui/QPlatformWindow>

#include "qgl.h"
#include "qgl_p.h"

QT_BEGIN_NAMESPACE

static QGLFormat qt_platformwindowformat_to_glformat(const QPlatformWindowFormat &format)
{
    QGLFormat retFormat;
    retFormat.setAccum(format.accum());
    if (format.accumBufferSize() >= 0)
        retFormat.setAccumBufferSize(format.accumBufferSize());
    retFormat.setAlpha(format.alpha());
    if (format.alphaBufferSize() >= 0)
        retFormat.setAlphaBufferSize(format.alphaBufferSize());
    if (format.blueBufferSize() >= 0)
        retFormat.setBlueBufferSize(format.blueBufferSize());
    retFormat.setDepth(format.depth());
    if (format.depthBufferSize() >= 0)
        retFormat.setDepthBufferSize(format.depthBufferSize());
    retFormat.setDirectRendering(format.directRendering());
    retFormat.setDoubleBuffer(format.doubleBuffer());
    if (format.greenBufferSize() >= 0)
        retFormat.setGreenBufferSize(format.greenBufferSize());
    if (format.redBufferSize() >= 0)
        retFormat.setRedBufferSize(format.redBufferSize());
    retFormat.setRgba(format.rgba());
    retFormat.setSampleBuffers(format.sampleBuffers());
    retFormat.setSamples(format.sampleBuffers());
    retFormat.setStencil(format.stencil());
    if (format.stencilBufferSize() >= 0)
        retFormat.setStencilBufferSize(format.stencilBufferSize());
    retFormat.setStereo(format.stereo());
    retFormat.setSwapInterval(format.swapInterval());
    return retFormat;
}

static QPlatformWindowFormat qt_glformat_to_platformwindowformat(const QGLFormat &format)
{
    QPlatformWindowFormat retFormat;
    retFormat.setAccum(format.accum());
    if (format.accumBufferSize() >= 0)
        retFormat.setAccumBufferSize(format.accumBufferSize());
    retFormat.setAlpha(format.alpha());
    if (format.alphaBufferSize() >= 0)
        retFormat.setAlphaBufferSize(format.alphaBufferSize());
    if (format.blueBufferSize() >= 0)
        retFormat.setBlueBufferSize(format.blueBufferSize());
    retFormat.setDepth(format.depth());
    if (format.depthBufferSize() >= 0)
        retFormat.setDepthBufferSize(format.depthBufferSize());
    retFormat.setDirectRendering(format.directRendering());
    retFormat.setDoubleBuffer(format.doubleBuffer());
    if (format.greenBufferSize() >= 0)
        retFormat.setGreenBufferSize(format.greenBufferSize());
    if (format.redBufferSize() >= 0)
        retFormat.setRedBufferSize(format.redBufferSize());
    retFormat.setRgba(format.rgba());
    retFormat.setSampleBuffers(format.sampleBuffers());
    if (format.samples() >= 0)
        retFormat.setSamples(format.samples());
    retFormat.setStencil(format.stencil());
    if (format.stencilBufferSize() >= 0)
        retFormat.setStencilBufferSize(format.stencilBufferSize());
    retFormat.setStereo(format.stereo());
    retFormat.setSwapInterval(format.swapInterval());
    return retFormat;
}

bool QGLFormat::hasOpenGL()
{
    return QApplicationPrivate::platformIntegration()->hasOpenGL();
}

bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);
    if (!d->paintDevice || d->paintDevice->devType() != QInternal::Widget) {
        d->valid = false;
    }else {
        QWidget *widget = static_cast<QWidget *>(d->paintDevice);
        if (!widget->platformWindow()){
            QGLFormat glformat = format();
            QPlatformWindowFormat winFormat = qt_glformat_to_platformwindowformat(glformat);
            if (shareContext) {
                winFormat.setSharedContext(shareContext->d_func()->platformContext);
            }
            winFormat.setWindowApi(QPlatformWindowFormat::OpenGL);
            widget->setPlatformWindowFormat(winFormat);
            widget->winId();//make window
        }
        d->platformContext = widget->platformWindow()->glContext();
        Q_ASSERT(d->platformContext);
        d->glFormat = qt_platformwindowformat_to_glformat(d->platformContext->platformWindowFormat());
        d->valid =(bool) d->platformContext;
    }

    return d->valid;
}

void QGLContext::reset()
{
    Q_D(QGLContext);
    if (!d->valid)
        return;
    d->cleanup();

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

    if (!d->workaroundsCached) {
        d->workaroundsCached = true;
        const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
        if (renderer && strstr(renderer, "Mali")) {
            d->workaround_brokenFBOReadBack = true;
        }
    }

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

    QGLContext* oldcx = d->glcx;
    d->glcx = context;

    if (!d->glcx->isValid())
        d->glcx->create(shareContext ? shareContext : oldcx);

    if (deleteOldContext)
        delete oldcx;
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
    QWidget *widget;
    QGLContext *context;
};

QGLTemporaryContext::QGLTemporaryContext(bool, QWidget *)
    : d(new QGLTemporaryContextPrivate)
{
    d->context = const_cast<QGLContext *>(QGLContext::currentContext());
    if (d->context)
        d->context->doneCurrent();
    d->widget = new QWidget;
    d->widget->setGeometry(0,0,3,3);
    QPlatformWindowFormat format = d->widget->platformWindowFormat();
    format.setWindowApi(QPlatformWindowFormat::OpenGL);
    d->widget->winId();


    d->widget->platformWindow()->glContext()->makeCurrent();
}

QGLTemporaryContext::~QGLTemporaryContext()
{
    d->widget->platformWindow()->glContext()->doneCurrent();
    if (d->context)
        d->context->makeCurrent();
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
    if (e->type() == QEvent::WinIdChange) {
        if (d->glcx->isValid()) {
            if (QGLContext::currentContext() == d->glcx)
                QGLContextPrivate::setCurrentContext(0); //Its not valid anymore
            setContext(new QGLContext(d->glcx->requestedFormat(), this));

        }
    }
    return QWidget::event(e);
}

void QGLWidget::resizeEvent(QResizeEvent *e)
{
    Q_D(QGLWidget);
    if (!isValid())
        return;
    makeCurrent();
    if (!d->glcx->initialized())
        glInit();
    resizeGL(width(), height());
    return QWidget::resizeEvent(e);
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
