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

#include <QDebug>

#include <QtGui/private/qt_x11_p.h>
#include <QtGui/private/qegl_p.h>
#include <QtGui/private/qeglproperties_p.h>
#include <QtGui/private/qeglcontext_p.h>

#if !defined(QT_OPENGL_ES_1)
#include <QtOpenGL/private/qpaintengineex_opengl2_p.h>
#endif

#ifndef QT_OPENGL_ES_2
#include <QtOpenGL/private/qpaintengine_opengl_p.h>
#endif

#include <QtOpenGL/private/qgl_p.h>
#include <QtOpenGL/private/qgl_egl_p.h>

#include "qpixmapdata_x11gl_p.h"

QT_BEGIN_NAMESPACE


// On 16bpp systems, RGB & ARGB pixmaps are different bit-depths and therefore need
// different contexts:

Q_GLOBAL_STATIC(QEglContext, qt_x11gl_rgbContext);
Q_GLOBAL_STATIC(QEglContext, qt_x11gl_argbContext)

QEglContext* QX11GLPixmapData::rgbContext = 0;
QEglContext* QX11GLPixmapData::argbContext = 0;


bool QX11GLPixmapData::hasX11GLPixmaps()
{
    static bool checkedForX11Pixmaps = false;
    static bool haveX11Pixmaps = false;

    if (checkedForX11Pixmaps)
        return haveX11Pixmaps;

    checkedForX11Pixmaps = true;

    EGLint rgbConfigId;
    EGLint argbConfigId;

    do {
        if (qgetenv("QT_USE_X11GL_PIXMAPS").isEmpty())
            break;

        EGLConfig rgbConfig = QEgl::defaultConfig(QInternal::Pixmap, QEgl::OpenGL, QEgl::Renderable);
        EGLConfig argbConfig = QEgl::defaultConfig(QInternal::Pixmap, QEgl::OpenGL,
                                                   QEgl::Renderable | QEgl::Translucent);

        eglGetConfigAttrib(QEgl::display(), rgbConfig, EGL_CONFIG_ID, &rgbConfigId);
        eglGetConfigAttrib(QEgl::display(), argbConfig, EGL_CONFIG_ID, &argbConfigId);

        if (!rgbContext) {
            rgbContext = qt_x11gl_rgbContext();
            rgbContext->setConfig(rgbConfig);
            rgbContext->createContext();
        }

        if (!rgbContext->isValid())
            break;

        // If the configs are the same, use the same egl contexts:
        if (rgbConfig == argbConfig)
            argbContext = rgbContext;

        if (!argbContext) {
            argbContext = qt_x11gl_argbContext();
            argbContext->setConfig(argbConfig);
            argbContext->createContext();
        }

        if (!argbContext->isValid())
            break;

        {
            QX11PixmapData *argbPixmapData = new QX11PixmapData(QPixmapData::PixmapType);
            argbPixmapData->resize(100, 100);
            argbPixmapData->fill(Qt::transparent); // Force ARGB
            QPixmap argbPixmap(argbPixmapData);
            EGLSurface argbPixmapSurface = QEgl::createSurface(&argbPixmap, argbConfig);
            haveX11Pixmaps = argbContext->makeCurrent(argbPixmapSurface);
            argbContext->doneCurrent();
            eglDestroySurface(QEgl::display(), argbPixmapSurface);
        }

        if (!haveX11Pixmaps) {
            qWarning() << "Unable to make pixmap surface current:" << QEgl::errorString();
            break;
        }

        // If the ARGB & RGB configs are different, check RGB too:
        if (argbConfig != rgbConfig) {
            QX11PixmapData *rgbPixmapData = new QX11PixmapData(QPixmapData::PixmapType);
            rgbPixmapData->resize(100, 100);
            rgbPixmapData->fill(Qt::red);

            QPixmap rgbPixmap(rgbPixmapData);
            EGLSurface rgbPixmapSurface = QEgl::createSurface(&rgbPixmap, rgbConfig);
            haveX11Pixmaps = rgbContext->makeCurrent(rgbPixmapSurface);
            rgbContext->doneCurrent();
            eglDestroySurface(QEgl::display(), rgbPixmapSurface);

            if (!haveX11Pixmaps) {
                qWarning() << "Unable to make pixmap config current:" << QEgl::errorString();
                break;
            }
        }

        // The pixmap surface destruction hooks are installed by QGLTextureCache, so we
        // must make sure this is instanciated:
        QGLTextureCache::instance();
    } while (0);

    if (!haveX11Pixmaps) {
        if (argbContext && (argbContext != rgbContext)) {
            delete argbContext;
            argbContext = 0;
        }
        if (rgbContext) {
            delete rgbContext;
            rgbContext = 0;
        }
    }

    if (haveX11Pixmaps)
        qDebug("Using QX11GLPixmapData with EGL config %d for ARGB and config %d for RGB", argbConfigId, rgbConfigId);
    else
        qDebug("QX11GLPixmapData is *NOT* being used");

    return haveX11Pixmaps;
}

QX11GLPixmapData::QX11GLPixmapData()
    : QX11PixmapData(QPixmapData::PixmapType),
      ctx(0)
{
}

QX11GLPixmapData::~QX11GLPixmapData()
{
    if (ctx)
        delete ctx;
}


void QX11GLPixmapData::fill(const QColor &color)
{
    if (ctx) {
        ctx->makeCurrent();
        glFinish();
        eglWaitClient();
    }

    QX11PixmapData::fill(color);
    XSync(X11->display, False);

    if (ctx) {
        ctx->makeCurrent();
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    }
}

void QX11GLPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    if (ctx) {
        ctx->makeCurrent();
        glFinish();
        eglWaitClient();
    }

    QX11PixmapData::copy(data, rect);
    XSync(X11->display, False);

    if (ctx) {
        ctx->makeCurrent();
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    }
}

bool QX11GLPixmapData::scroll(int dx, int dy, const QRect &rect)
{
    if (ctx) {
        ctx->makeCurrent();
        glFinish();
        eglWaitClient();
    }

    QX11PixmapData::scroll(dx, dy, rect);
    XSync(X11->display, False);

    if (ctx) {
        ctx->makeCurrent();
        eglWaitNative(EGL_CORE_NATIVE_ENGINE);
    }
}

#if !defined(QT_OPENGL_ES_1)
Q_GLOBAL_STATIC(QGL2PaintEngineEx, qt_gl_pixmap_2_engine)
#endif

#ifndef QT_OPENGL_ES_2
Q_GLOBAL_STATIC(QOpenGLPaintEngine, qt_gl_pixmap_engine)
#endif


QPaintEngine* QX11GLPixmapData::paintEngine() const
{
    // We need to create the context before beginPaint - do it here:
    if (!ctx) {
        ctx = new QGLContext(glFormat());
        Q_ASSERT(ctx->d_func()->eglContext == 0);
        ctx->d_func()->eglContext = hasAlphaChannel() ? argbContext : rgbContext;
        // Update the glFormat for the QGLContext:
        qt_glformat_from_eglconfig(ctx->d_func()->glFormat, ctx->d_func()->eglContext->config());
    }

    QPaintEngine* engine;

#if defined(QT_OPENGL_ES_1)
    engine = qt_gl_pixmap_engine();
#elif defined(QT_OPENGL_ES_2)
    engine = qt_gl_pixmap_2_engine();
#else
    if (qt_gl_preferGL2Engine())
        engine = qt_gl_pixmap_2_engine();
    else
        engine = qt_gl_pixmap_engine();
#endif



    // Support multiple painters on multiple pixmaps simultaniously
    if (engine->isActive()) {
        qWarning("Pixmap paint engine already active");

#if defined(QT_OPENGL_ES_1)
        engine = new QOpenGLPaintEngine;
#elif defined(QT_OPENGL_ES_2)
        engine = new QGL2PaintEngineEx;
#else
        if (qt_gl_preferGL2Engine())
            engine = new QGL2PaintEngineEx;
        else
            engine = new QOpenGLPaintEngine;
#endif

        engine->setAutoDestruct(true);
        return engine;
    }

    return engine;
}

void QX11GLPixmapData::beginPaint()
{
//    qDebug("QX11GLPixmapData::beginPaint()");
    // TODO: Check to see if the surface is renderable
    if ((EGLSurface)gl_surface == EGL_NO_SURFACE) {
        QPixmap tmpPixmap(this);
        EGLConfig cfg = ctx->d_func()->eglContext->config();
        Q_ASSERT(cfg != QEGL_NO_CONFIG);

//        qDebug("QX11GLPixmapData - using EGL Config ID %d", ctx->d_func()->eglContext->configAttrib(EGL_CONFIG_ID));
        EGLSurface surface = QEgl::createSurface(&tmpPixmap, cfg);
        if (surface == EGL_NO_SURFACE) {
            qWarning() << "Error creating EGL surface for pixmap:" << QEgl::errorString();
            return;
        }
        gl_surface = (void*)surface;
        ctx->d_func()->eglSurface = surface;
        ctx->d_func()->valid = true;
    }
    QGLPaintDevice::beginPaint();
}

void QX11GLPixmapData::endPaint()
{
    glFinish();
    QGLPaintDevice::endPaint();
}

QGLContext* QX11GLPixmapData::context() const
{
    return ctx;
}

QSize QX11GLPixmapData::size() const
{
    return QSize(w, h);
}


QGLFormat QX11GLPixmapData::glFormat()
{
    return QGLFormat::defaultFormat(); //###
}

QT_END_NAMESPACE
