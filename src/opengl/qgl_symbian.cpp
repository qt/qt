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


#include "qgl.h"
#include <coemain.h>
#include <coecntrl.h>
#include <w32std.h>
#include <private/qpixmap_s60_p.h>
#include <private/qimagepixmapcleanuphooks_p.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include <private/qwidget_p.h> // to access QWExtra
#include "qgl_egl_p.h"
#include "qcolormap.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

// Turn off "direct to window" rendering if EGL cannot support it.
#if !defined(EGL_RENDER_BUFFER) || !defined(EGL_SINGLE_BUFFER)
#if defined(QGL_DIRECT_TO_WINDOW)
#undef QGL_DIRECT_TO_WINDOW
#endif
#endif

// Determine if preserved window contents should be used.
#if !defined(EGL_SWAP_BEHAVIOR) || !defined(EGL_BUFFER_PRESERVED)
#if !defined(QGL_NO_PRESERVED_SWAP)
#define QGL_NO_PRESERVED_SWAP 1
#endif
#endif

/*
    QGLTemporaryContext implementation
*/


class QGLTemporaryContextPrivate
{
public:
    bool initialized;
    RWindow *window;
    EGLContext context;
    EGLSurface surface;
    EGLDisplay display;
};

QGLTemporaryContext::QGLTemporaryContext(bool, QWidget *)
    : d(new QGLTemporaryContextPrivate)
{
    d->initialized = false;
    d->window = 0;
    d->context = 0;
    d->surface = 0;

    d->display = d->display = QEgl::display();

    EGLConfig config;
    int numConfigs = 0;
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#ifdef QT_OPENGL_ES_2
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#endif
        EGL_NONE
    };

    eglChooseConfig(d->display, attribs, &config, 1, &numConfigs);
    if (!numConfigs) {
        qWarning("QGLTemporaryContext: No EGL configurations available.");
        return;
    }

    d->window = new RWindow(CCoeEnv::Static()->WsSession());
    d->window->Construct(CCoeEnv::Static()->RootWin(),(uint)this);

    d->surface = eglCreateWindowSurface(d->display, config, (EGLNativeWindowType) d->window, NULL);

    if (d->surface == EGL_NO_SURFACE) {
        qWarning("QGLTemporaryContext: Error creating EGL surface.");
        delete d->window;
        d->window = 0;
        return;
    }

    EGLint contextAttribs[] = {
#ifdef QT_OPENGL_ES_2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
        EGL_NONE
    };
    d->context = eglCreateContext(d->display, config, 0, contextAttribs);
    if (d->context != EGL_NO_CONTEXT
        && eglMakeCurrent(d->display, d->surface, d->surface, d->context))
    {
        d->initialized = true;
    } else {
        qWarning("QGLTemporaryContext: Error creating EGL context.");
        d->window = 0;
        return;
    }
}

QGLTemporaryContext::~QGLTemporaryContext()
{
    if (d->initialized) {
        eglMakeCurrent(d->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(d->display, d->context);
        eglDestroySurface(d->display, d->surface);
        delete d->window;
    }
}

bool QGLFormat::hasOpenGLOverlays()
{
    return false;
}

// Chooses the EGL config and creates the EGL context
bool QGLContext::chooseContext(const QGLContext* shareContext) // almost same as in qgl_x11egl.cpp
{
    Q_D(QGLContext);

    if (!device())
        return false;

    int devType = device()->devType();

    if ((devType != QInternal::Widget) && (devType != QInternal::Pbuffer)) {
        qWarning("WARNING: Creating a QGLContext not supported on device type %d", devType);
        return false;
    }

    // Get the display and initialize it.
    if (d->eglContext == 0) {
        d->eglContext = new QEglContext();
        d->ownsEglContext = true;
        d->eglContext->setApi(QEgl::OpenGL);

	    // If the device is a widget with WA_TranslucentBackground set, make sure the glFormat
        // has the alpha channel option set:
        if (devType == QInternal::Widget) {
            QWidget* widget = static_cast<QWidget*>(device());
            if (widget->testAttribute(Qt::WA_TranslucentBackground))
                d->glFormat.setAlpha(true);
        }

        // Construct the configuration we need for this surface.
        QEglProperties configProps;
        configProps.setDeviceType(devType);
        configProps.setPaintDeviceFormat(device());
        configProps.setRenderableType(QEgl::OpenGL);
        configProps.setValue(EGL_SURFACE_TYPE, EGL_WINDOW_BIT|EGL_SWAP_BEHAVIOR_PRESERVED_BIT);

        qt_eglproperties_set_glformat(configProps, d->glFormat);

        if (!d->eglContext->chooseConfig(configProps, QEgl::BestPixelFormat)) {
            delete d->eglContext;
            d->eglContext = 0;
            return false;
        }

        // Create a new context for the configuration.
        QEglContext* eglSharedContext = shareContext ? shareContext->d_func()->eglContext : 0;
        if (!d->eglContext->createContext(eglSharedContext)) {
            delete d->eglContext;
            d->eglContext = 0;
            return false;
        }
        d->sharing = d->eglContext->isSharing();
        if (d->sharing && shareContext)
            const_cast<QGLContext *>(shareContext)->d_func()->sharing = true;
	}

    // Inform the higher layers about the actual format properties
    qt_glformat_from_eglconfig(d->glFormat, d->eglContext->config());

    // Do don't create the EGLSurface for everything.
    //    QWidget - yes, create the EGLSurface and store it in QGLContextPrivate::eglSurface
    //    QGLWidget - yes, create the EGLSurface and store it in QGLContextPrivate::eglSurface
    //    QGLPixelBuffer - no, it creates the surface itself and stores it in QGLPixelBufferPrivate::pbuf

    if (devType == QInternal::Widget) {
        if (d->eglSurface != EGL_NO_SURFACE)
            eglDestroySurface(d->eglContext->display(), d->eglSurface);

        d->eglSurface = QEgl::createSurface(device(), d->eglContext->config());

#if !defined(QGL_NO_PRESERVED_SWAP)
        eglGetError();  // Clear error state first.
        eglSurfaceAttrib(QEgl::display(), d->eglSurface,
                EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
        if (eglGetError() != EGL_SUCCESS) {
            qWarning("QGLContext: could not enable preserved swap");
        }
#endif

        setWindowCreated(true);
    }

    return true;
}

void QGLWidget::resizeEvent(QResizeEvent *)
{
    Q_D(QGLWidget);
    if (!isValid())
        return;

    if (QGLContext::currentContext())
        doneCurrent();

    // Symbian needs to recreate the surface on resize.
    d->recreateEglSurface();

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

    bool createFailed = false;
    if (!d->glcx->isValid()) {
        // Create the QGLContext here, which in turn chooses the EGL config
        // and creates the EGL context:
        if (!d->glcx->create(shareContext ? shareContext : oldcx))
            createFailed = true;
    }
    if (createFailed) {
        if (deleteOldContext)
            delete oldcx;
        return;
    }

    d->eglSurfaceWindowId = winId(); // Remember the window id we created the surface for
}

void QGLWidgetPrivate::init(QGLContext *context, const QGLWidget* shareWidget)
{
    Q_Q(QGLWidget);

    initContext(context, shareWidget);

    if(q->isValid() && glcx->format().hasOverlay()) {
        //no overlay
        qWarning("QtOpenGL ES doesn't currently support overlays");
    }
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

void QGLWidgetPrivate::recreateEglSurface()
{
    Q_Q(QGLWidget);

    WId currentId = q->winId();

    if (glcx->d_func()->eglSurface != EGL_NO_SURFACE) {
        eglDestroySurface(glcx->d_func()->eglContext->display(),
                                                glcx->d_func()->eglSurface);
    }

    glcx->d_func()->eglSurface = QEgl::createSurface(glcx->device(),
                                       glcx->d_func()->eglContext->config());

#if !defined(QGL_NO_PRESERVED_SWAP)
        eglGetError();  // Clear error state first.
        eglSurfaceAttrib(QEgl::display(), glcx->d_func()->eglSurface,
                                    EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);
        if (eglGetError() != EGL_SUCCESS) {
            qWarning("QGLContext: could not enable preserved swap");
        }
#endif

    eglSurfaceWindowId = currentId;
}

QT_END_NAMESPACE

