/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgl.h"
#include "qgl_egl_p.h"

#include <qglscreen_qws.h>
#include <qscreenproxy_qws.h>
#include <private/qglwindowsurface_qws_p.h>

#include <private/qbackingstore_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qstack.h>
#include <qdesktopwidget.h>
#include <qdebug.h>
#include <qvarlengtharray.h>

QT_BEGIN_NAMESPACE

static QGLScreen *glScreenForDevice(QPaintDevice *device)
{
    QScreen *screen = qt_screen;
    if (screen->classId() == QScreen::MultiClass) {
        int screenNumber;
        if (device && device->devType() == QInternal::Widget)
            screenNumber = qApp->desktop()->screenNumber(static_cast<QWidget *>(device));
        else
            screenNumber = 0;
        screen = screen->subScreens()[screenNumber];
    }
    while (screen->classId() == QScreen::ProxyClass) {
        screen = static_cast<QProxyScreen *>(screen)->screen();
    }
    if (screen->classId() == QScreen::GLClass)
        return static_cast<QGLScreen *>(screen);
    else
        return 0;
}

/*****************************************************************************
  QOpenGL debug facilities
 *****************************************************************************/
//#define DEBUG_OPENGL_REGION_UPDATE

bool QGLFormat::hasOpenGL()
{
    return true;
}


bool QGLFormat::hasOpenGLOverlays()
{
    QGLScreen *glScreen = glScreenForDevice(0);
    if (glScreen)
        return (glScreen->options() & QGLScreen::Overlays);
    else
        return false;
}

void qt_egl_add_platform_config(QEglProperties& props, QPaintDevice *device)
{
    // Find the QGLScreen for this paint device.
    QGLScreen *glScreen = glScreenForDevice(device);
    if (!glScreen) {
        qWarning("QGLContext::chooseContext(): The screen is not a QGLScreen");
        return;
    }
    int devType = device->devType();
    if (devType == QInternal::Image)
        props.setPixelFormat(static_cast<QImage *>(device)->format());
    else
        props.setPixelFormat(glScreen->pixelFormat());
}

bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);

    // Validate the device.
    if (!device())
        return false;
    int devType = device()->devType();
    if (devType != QInternal::Pixmap && devType != QInternal::Image && devType != QInternal::Widget) {
        qWarning("QGLContext::chooseContext(): Cannot create QGLContext's for paint device type %d", devType);
        return false;
    }

    // Get the display and initialize it.
    d->eglContext = new QEglContext();
    d->eglContext->setApi(QEglContext::OpenGL);
    if (!d->eglContext->openDisplay(device())) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

    // Construct the configuration we need for this surface.
    QEglProperties configProps;
    qt_egl_add_platform_config(configProps, device());
    qt_egl_set_format(configProps, devType, d->glFormat);
    configProps.setRenderableType(QEglContext::OpenGL);

    // Search for a matching configuration, reducing the complexity
    // each time until we get something that matches.
    if (!d->eglContext->chooseConfig(configProps)) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

    // Inform the higher layers about the actual format properties.
    qt_egl_update_format(*(d->eglContext), d->glFormat);

    // Create a new context for the configuration.
    if (!d->eglContext->createContext
            (shareContext ? shareContext->d_func()->eglContext : 0)) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

#if defined(EGL_VERSION_1_1)
    if (d->glFormat.swapInterval() != -1 && devType == QInternal::Widget)
        eglSwapInterval(d->eglContext->display(), d->glFormat.swapInterval());
#endif

    // Create the EGL surface to draw into.
    if (!d->eglContext->createSurface(device())) {
        delete d->eglContext;
        d->eglContext = 0;
        return false;
    }

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
        delete d->eglContext;
        d->eglContext = 0;
    }
    d->crWin = false;
    d->sharing = false;
    d->valid = false;
    d->transpColor = QColor();
    d->initDone = false;
    qgl_share_reg()->removeShare(this);
}

void QGLContext::makeCurrent()
{
    Q_D(QGLContext);
    if(!d->valid || !d->eglContext) {
        qWarning("QGLContext::makeCurrent(): Cannot make invalid context current");
        return;
    }

    if (d->eglContext->makeCurrent()) {
        if (!qgl_context_storage.hasLocalData() && QThread::currentThread())
            qgl_context_storage.setLocalData(new QGLThreadContext);
        if (qgl_context_storage.hasLocalData())
            qgl_context_storage.localData()->context = this;
        currentCtx = this;
    }
}

void QGLContext::doneCurrent()
{
    Q_D(QGLContext);
    if (d->eglContext)
        d->eglContext->doneCurrent();

    if (qgl_context_storage.hasLocalData())
        qgl_context_storage.localData()->context = 0;
    currentCtx = 0;
}


void QGLContext::swapBuffers() const
{
    Q_D(const QGLContext);
    if(!d->valid || !d->eglContext)
        return;

    d->eglContext->swapBuffers();
}

QColor QGLContext::overlayTransparentColor() const
{
    return QColor(0, 0, 0);                // Invalid color
}

uint QGLContext::colorIndex(const QColor &c) const
{
    //### color index doesn't work on egl
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

bool QGLWidget::event(QEvent *e)
{
    return QWidget::event(e);
}

void QGLWidget::setMouseTracking(bool enable)
{
    QWidget::setMouseTracking(enable);
}


void QGLWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QGLWidget);
    if (!isValid())
        return;
    makeCurrent();
    if (!d->glcx->initialized())
        glInit();
    resizeGL(width(), height());
    //handle overlay
}

const QGLContext* QGLWidget::overlayContext() const
{
    return 0;
}

void QGLWidget::makeOverlayCurrent()
{
    //handle overlay
}

void QGLWidget::updateOverlayGL()
{
    //handle overlay
}

void QGLWidget::setContext(QGLContext *context, const QGLContext* shareContext, bool deleteOldContext)
{
    Q_D(QGLWidget);
    if(context == 0) {
        qWarning("QGLWidget::setContext: Cannot set null context");
        return;
    }

    if(d->glcx)
        d->glcx->doneCurrent();
    QGLContext* oldcx = d->glcx;
    d->glcx = context;
    if(!d->glcx->isValid())
        d->glcx->create(shareContext ? shareContext : oldcx);
    if(deleteOldContext)
        delete oldcx;
}

void QGLWidgetPrivate::init(QGLContext *context, const QGLWidget* shareWidget)
{
    Q_Q(QGLWidget);

    QGLScreen *glScreen = glScreenForDevice(q);
    if (glScreen) {
        wsurf = static_cast<QWSGLWindowSurface*>(glScreen->createSurface(q));
        q->setWindowSurface(wsurf);
    }

    initContext(context, shareWidget);

    if(q->isValid() && glcx->format().hasOverlay()) {
        //no overlay
        qWarning("QtOpenGL ES doesn't currently support overlays");
    }
}

bool QGLWidgetPrivate::renderCxPm(QPixmap*)
{
    return false;
}

void QGLWidgetPrivate::cleanupColormaps()
{
}

const QGLColormap & QGLWidget::colormap() const
{
    return d_func()->cmap;
}

void QGLWidget::setColormap(const QGLColormap &)
{
}

void QGLExtensions::init()
{
    static bool init_done = false;

    if (init_done)
        return;
    init_done = true;
    init_extensions();
}

QT_END_NAMESPACE
