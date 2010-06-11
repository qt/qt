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


bool QGLFormat::hasOpenGL()
{
    return QApplicationPrivate::platformIntegration()->hasOpenGL();
}

bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);
    if (!d->paintDevice && d->paintDevice->devType() != QInternal::Widget) {
        d->valid = false;
    }else {
        QWidget *widget = static_cast<QWidget *>(d->paintDevice);
        if (!widget->platformWindow()){
            widget->winId();//make window
        }
        d->platformContext = widget->platformWindow()->glContext();
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
    return QWidget::event(e);
}

void QGLWidget::resizeEvent(QResizeEvent *e)
{
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
