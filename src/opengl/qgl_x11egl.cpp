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
#include <private/qt_x11_p.h>
#include <private/qpixmap_x11_p.h>
#include <private/qimagepixmapcleanuphooks_p.h>
#include <private/qgl_p.h>
#include <private/qpaintengine_opengl_p.h>
#include "qgl_egl_p.h"
#include "qcolormap.h"
#include <QDebug>
#include <QPixmap>


QT_BEGIN_NAMESPACE


/*
    QGLTemporaryContext implementation
*/

class QGLTemporaryContextPrivate
{
public:
    bool initialized;
    Window window;
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
    int screen = 0;

    d->display = QEgl::display();

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

    XVisualInfo visualInfo;
    XVisualInfo *vi;
    int numVisuals;
    EGLint id = 0;

    visualInfo.visualid = QEgl::getCompatibleVisualId(config);
    vi = XGetVisualInfo(X11->display, VisualIDMask, &visualInfo, &numVisuals);
    if (!vi || numVisuals < 1) {
        qWarning("QGLTemporaryContext: Unable to get X11 visual info id.");
        return;
    }

    d->window = XCreateWindow(X11->display, RootWindow(X11->display, screen),
                              0, 0, 1, 1, 0,
                              vi->depth, InputOutput, vi->visual,
                              0, 0);

    d->surface = eglCreateWindowSurface(d->display, config, (EGLNativeWindowType) d->window, NULL);

    if (d->surface == EGL_NO_SURFACE) {
        qWarning("QGLTemporaryContext: Error creating EGL surface.");
        XFree(vi);
        XDestroyWindow(X11->display, d->window);
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
        eglDestroySurface(d->display, d->surface);
        XDestroyWindow(X11->display, d->window);
    }
    XFree(vi);
}

QGLTemporaryContext::~QGLTemporaryContext()
{
    if (d->initialized) {
        eglMakeCurrent(d->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(d->display, d->context);
        eglDestroySurface(d->display, d->surface);
        XDestroyWindow(X11->display, d->window);
    }
}

bool QGLFormat::hasOpenGLOverlays()
{
    return false;
}

// Chooses the EGL config and creates the EGL context
bool QGLContext::chooseContext(const QGLContext* shareContext)
{
    Q_D(QGLContext);

    if (!device())
        return false;

    int devType = device()->devType();

    QX11PixmapData *x11PixmapData = 0;
    if (devType == QInternal::Pixmap) {
        QPixmapData *pmd = static_cast<QPixmap*>(device())->data_ptr().data();
        if (pmd->classId() == QPixmapData::X11Class)
            x11PixmapData = static_cast<QX11PixmapData*>(pmd);
        else {
            // TODO: Replace the pixmap's data with a new QX11PixmapData
            qWarning("WARNING: Creating a QGLContext on a QPixmap is only supported for X11 pixmap backend");
            return false;
        }
    } else if ((devType != QInternal::Widget) && (devType != QInternal::Pbuffer)) {
        qWarning("WARNING: Creating a QGLContext not supported on device type %d", devType);
        return false;
    }

    // Only create the eglContext if we don't already have one:
    if (d->eglContext == 0) {
        d->eglContext = new QEglContext();
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
        configProps.setRenderableType(QEgl::OpenGL);
        qt_eglproperties_set_glformat(configProps, d->glFormat);

        // Use EGL_BUFFER_SIZE to make sure we prefer a 16-bit config over a 32-bit config
        if (device()->depth() == 16 && !d->glFormat.alpha())
            configProps.setValue(EGL_BUFFER_SIZE, 16);

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

    // Inform the higher layers about the actual format properties.
    qt_egl_update_format(*(d->eglContext), d->glFormat);


    // Do don't create the EGLSurface for everything.
    //    QWidget - yes, create the EGLSurface and store it in QGLContextPrivate::eglSurface
    //    QGLWidget - yes, create the EGLSurface and store it in QGLContextPrivate::eglSurface
    //    QPixmap - yes, create the EGLSurface but store it in QX11PixmapData::gl_surface
    //    QGLPixelBuffer - no, it creates the surface itself

    if (devType == QInternal::Widget) {
        if (d->eglSurface != EGL_NO_SURFACE)
            eglDestroySurface(d->eglContext->display(), d->eglSurface);
        d->eglSurface = QEgl::createSurface(device(), d->eglContext->config());
        XFlush(X11->display);
        setWindowCreated(true);
    }

    if (x11PixmapData) {
        // TODO: Actually check to see if the existing surface can be re-used
        if (x11PixmapData->gl_surface)
            eglDestroySurface(d->eglContext->display(), (EGLSurface)x11PixmapData->gl_surface);

        x11PixmapData->gl_surface = (Qt::HANDLE)QEgl::createSurface(device(), d->eglContext->config());
    }

    return true;
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

    if (q->isValid() && glcx->format().hasOverlay()) {
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

// Re-creates the EGL surface if the window ID has changed or if force is true
void QGLWidgetPrivate::recreateEglSurface(bool force)
{
    Q_Q(QGLWidget);

    Window currentId = q->winId();

    if ( force || (currentId != eglSurfaceWindowId) ) {
        // The window id has changed so we need to re-create the EGL surface
        QEglContext *ctx = glcx->d_func()->eglContext;
        EGLSurface surface = glcx->d_func()->eglSurface;
        if (surface != EGL_NO_SURFACE)
            ctx->destroySurface(surface); // Will force doneCurrent() if nec.
        surface = ctx->createSurface(q);
        if (surface == EGL_NO_SURFACE)
            qWarning("Error creating EGL window surface: 0x%x", eglGetError());
        glcx->d_func()->eglSurface = surface;

        eglSurfaceWindowId = currentId;
    }
}

// Selects which configs should be used
EGLConfig Q_OPENGL_EXPORT qt_chooseEGLConfigForPixmap(bool hasAlpha, bool readOnly)
{
    // Cache the configs we select as they wont change:
    static EGLConfig roPixmapRGBConfig = 0;
    static EGLConfig roPixmapRGBAConfig = 0;
    static EGLConfig rwPixmapRGBConfig = 0;
    static EGLConfig rwPixmapRGBAConfig = 0;

    EGLConfig* targetConfig;

    if (hasAlpha) {
        if (readOnly)
            targetConfig = &roPixmapRGBAConfig;
        else
            targetConfig = &rwPixmapRGBAConfig;
    }
    else {
        if (readOnly)
            targetConfig = &roPixmapRGBConfig;
        else
            targetConfig = &rwPixmapRGBConfig;
    }

    if (*targetConfig == 0) {
        QEglProperties configAttribs;
        configAttribs.setValue(EGL_SURFACE_TYPE, EGL_PIXMAP_BIT);
        configAttribs.setRenderableType(QEgl::OpenGL);
        if (hasAlpha)
            configAttribs.setValue(EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE);
        else
            configAttribs.setValue(EGL_BIND_TO_TEXTURE_RGB, EGL_TRUE);

        // If this is going to be a render target, it needs to have a depth, stencil & sample buffer
        if (!readOnly) {
            configAttribs.setValue(EGL_DEPTH_SIZE, 1);
            configAttribs.setValue(EGL_STENCIL_SIZE, 1);
            configAttribs.setValue(EGL_SAMPLE_BUFFERS, 1);
        }

        EGLint configCount = 0;
        do {
            eglChooseConfig(QEgl::display(), configAttribs.properties(), targetConfig, 1, &configCount);
            if (configCount > 0) {
                // Got one
                qDebug() << "Found an" << (hasAlpha ? "ARGB" : "RGB") << (readOnly ? "readonly" : "target" )
                         << "config (" << int(*targetConfig) << ") to create a pixmap surface:";

//                QEglProperties configProps(*targetConfig);
//                qDebug() << configProps.toString();
                break;
            }
            qWarning("choosePixmapConfig() - No suitible config found, reducing requirements");
        } while (configAttribs.reduceConfiguration());
    }

    if (*targetConfig == 0)
        qWarning("choosePixmapConfig() - Couldn't find a suitable config");

    return *targetConfig;
}

bool Q_OPENGL_EXPORT qt_createEGLSurfaceForPixmap(QPixmapData* pmd, bool readOnly)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData* pixmapData = static_cast<QX11PixmapData*>(pmd);

    bool hasAlpha = pixmapData->hasAlphaChannel();

    EGLConfig pixmapConfig = qt_chooseEGLConfigForPixmap(hasAlpha, readOnly);

    QEglProperties pixmapAttribs;

    // If the pixmap can't be bound to a texture, it's pretty useless
    pixmapAttribs.setValue(EGL_TEXTURE_TARGET, EGL_TEXTURE_2D);
    if (hasAlpha)
        pixmapAttribs.setValue(EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA);
    else
        pixmapAttribs.setValue(EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB);

    EGLSurface pixmapSurface;
    pixmapSurface = eglCreatePixmapSurface(QEgl::display(),
                                           pixmapConfig,
                                           (EGLNativePixmapType) pixmapData->handle(),
                                           pixmapAttribs.properties());
//    qDebug("qt_createEGLSurfaceForPixmap() created surface 0x%x for pixmap 0x%x",
//           pixmapSurface, pixmapData->handle());
    if (pixmapSurface == EGL_NO_SURFACE) {
        qWarning() << "Failed to create a pixmap surface using config" << (int)pixmapConfig
                   << ":" << QEgl::errorString();
        return false;
    }

    static bool doneOnce = false;
    if (!doneOnce) {
        // Make sure QGLTextureCache is instanciated so it can install cleanup hooks
        // which cleanup the EGL surface.
        QGLTextureCache::instance();
        doneOnce = true;
    }

    Q_ASSERT(sizeof(Qt::HANDLE) >= sizeof(EGLSurface)); // Just to make totally sure!
    pixmapData->gl_surface = (Qt::HANDLE)pixmapSurface;
    QImagePixmapCleanupHooks::enableCleanupHooks(pixmapData); // Make sure the cleanup hook gets called

    return true;
}


QGLTexture *QGLContextPrivate::bindTextureFromNativePixmap(QPixmapData* pd, const qint64 key,
                                                           QGLContext::BindOptions options)
{
    Q_Q(QGLContext);

    // The EGL texture_from_pixmap has no facility to invert the y coordinate
    if (!(options & QGLContext::CanFlipNativePixmapBindOption))
        return 0;

    Q_ASSERT(pd->classId() == QPixmapData::X11Class);

    static bool checkedForTFP = false;
    static bool haveTFP = false;

    if (!checkedForTFP) {
        // Check for texture_from_pixmap egl extension
        checkedForTFP = true;
        if (QEgl::hasExtension("EGL_NOKIA_texture_from_pixmap") ||
            QEgl::hasExtension("EGL_EXT_texture_from_pixmap"))
        {
            qDebug("Found texture_from_pixmap EGL extension!");
            haveTFP = true;
        }
    }

    if (!haveTFP)
        return 0;

    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pd);

    bool hasAlpha = pixmapData->hasAlphaChannel();

    // Check to see if the surface is still valid
    if (pixmapData->gl_surface &&
        hasAlpha != (pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha))
    {
        // Surface is invalid!
        destroyGlSurfaceForPixmap(pixmapData);
    }

    if (pixmapData->gl_surface == 0) {
        EGLConfig config = QEgl::defaultConfig(QInternal::Pixmap,
                                               QEgl::OpenGL,
                                               hasAlpha ? QEgl::Translucent : QEgl::NoOptions);

        QPixmap tmpPixmap(pixmapData); //###
        pixmapData->gl_surface = (Qt::HANDLE)QEgl::createSurface(&tmpPixmap, config);
        if (pixmapData->gl_surface == (Qt::HANDLE)EGL_NO_SURFACE) {
            haveTFP = false;
            return 0;
        }
    }

    Q_ASSERT(pixmapData->gl_surface);

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // bind the egl pixmap surface to a texture
    EGLBoolean success;
    success = eglBindTexImage(eglContext->display(), (EGLSurface)pixmapData->gl_surface, EGL_BACK_BUFFER);
    if (success == EGL_FALSE) {
        qWarning() << "eglBindTexImage() failed:" << QEgl::errorString();
        eglDestroySurface(eglContext->display(), (EGLSurface)pixmapData->gl_surface);
        pixmapData->gl_surface = (Qt::HANDLE)EGL_NO_SURFACE;
        haveTFP = false;
        return 0;
    }

    QGLTexture *texture = new QGLTexture(q, textureId, GL_TEXTURE_2D, options);
    pixmapData->flags |= QX11PixmapData::InvertedWhenBoundToTexture;

    // We assume the cost of bound pixmaps is zero
    QGLTextureCache::instance()->insert(q, key, texture, 0);

    glBindTexture(GL_TEXTURE_2D, textureId);
    return texture;
}

void QGLContextPrivate::destroyGlSurfaceForPixmap(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        EGLBoolean success;
        success = eglDestroySurface(QEgl::display(), (EGLSurface)pixmapData->gl_surface);
        if (success == EGL_FALSE) {
            qWarning() << "destroyGlSurfaceForPixmap() - Error deleting surface: "
                       << QEgl::errorString();
        }
        pixmapData->gl_surface = 0;
    }
}

void QGLContextPrivate::unbindPixmapFromTexture(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        EGLBoolean success;
        success = eglReleaseTexImage(QEgl::display(),
                                     (EGLSurface)pixmapData->gl_surface,
                                     EGL_BACK_BUFFER);
        if (success == EGL_FALSE) {
            qWarning() << "unbindPixmapFromTexture() - Unable to release bound texture: "
                       << QEgl::errorString();
        }
    }
}

QT_END_NAMESPACE
