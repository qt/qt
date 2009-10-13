/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <private/qgl_p.h>
#include <private/qegl_p.h>
#include <private/qeglproperties_p.h>
#include <private/qpaintengineex_opengl2_p.h>

#include "qpixmapdata_x11gl_p.h"

QT_BEGIN_NAMESPACE

extern EGLConfig qt_chooseEGLConfigForPixmap(bool hasAlpha, bool readOnly); // in qgl_x11egl.cpp
extern bool qt_createEGLSurfaceForPixmap(QPixmapData* pmd, bool readOnly); // in qgl_x11egl.cpp

static EGLContext qPixmapSharedEglContext = EGL_NO_CONTEXT;

void QX11GLPixmapData::createPixmapSharedContext(EGLConfig config)
{
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint contextAttribs[] = {
#if defined(QT_OPENGL_ES_2)
        EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
        EGL_NONE
    };
    qPixmapSharedEglContext = eglCreateContext(QEglContext::defaultDisplay(0), config, 0, contextAttribs);
//    qDebug("QX11GLPixmapData::createPixmapSharedContext() Created ctx 0x%x for config %d", qPixmapSharedEglContext, config);
}

bool QX11GLPixmapData::hasX11GLPixmaps()
{
    static bool checkedForX11Pixmaps = false;
    static bool haveX11Pixmaps = false;

    if (checkedForX11Pixmaps)
        return haveX11Pixmaps;

    checkedForX11Pixmaps = true;

    do {
        if (qgetenv("QT_USE_X11GL_PIXMAPS").isEmpty())
            break;

        // First, check we actually have an EGL config which supports pixmaps
        EGLConfig config = qt_chooseEGLConfigForPixmap(true, false);
        if (config == 0)
            break;

        // Now try to actually create an EGL pixmap surface
        QX11PixmapData *pd = new QX11PixmapData(QPixmapData::PixmapType);
        pd->resize(100, 100);
        bool success = qt_createEGLSurfaceForPixmap(pd, false);
        if (!success)
            break;

        createPixmapSharedContext(config);

        haveX11Pixmaps = eglMakeCurrent(QEglContext::defaultDisplay(0),
                                        (EGLSurface)pd->gl_surface, (EGLSurface)pd->gl_surface,
                                         qPixmapSharedEglContext);

        eglMakeCurrent(QEglContext::defaultDisplay(0),
                       EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        QGLContextPrivate::destroyGlSurfaceForPixmap(pd);
        delete pd;
    } while (0);

    if (haveX11Pixmaps)
        qDebug("QX11GLPixmapData is supported");

    return haveX11Pixmaps;
}

QX11GLPixmapData::QX11GLPixmapData()
    : QX11PixmapData(QPixmapData::PixmapType),
      ctx(0)
{
}

QX11GLPixmapData::~QX11GLPixmapData()
{
}

static QGL2PaintEngineEx* qt_gl2_engine_for_pixmaps = 0;

QPaintEngine* QX11GLPixmapData::paintEngine() const
{
    // We need to create the context before beginPaint - do it here:
    if (!ctx) {
        ctx = new QGLContext(glFormat());
        if (ctx->d_func()->eglContext == 0)
            ctx->d_func()->eglContext = new QEglContext();
        ctx->d_func()->eglContext->openDisplay(0); // ;-)
        ctx->d_func()->eglContext->setApi(QEgl::OpenGL);
        ctx->d_func()->eglContext->setContext(qPixmapSharedEglContext);
    }

    if (!qt_gl2_engine_for_pixmaps)
        qt_gl2_engine_for_pixmaps = new QGL2PaintEngineEx();

    // Support multiple painters on multiple pixmaps simultaniously
    if (qt_gl2_engine_for_pixmaps->isActive()) {
        QPaintEngine* engine = new QGL2PaintEngineEx();
        engine->setAutoDestruct(true);
        return engine;
    }

    return qt_gl2_engine_for_pixmaps;
}

void QX11GLPixmapData::beginPaint()
{
    if ((EGLSurface)gl_surface == EGL_NO_SURFACE) {
        qt_createEGLSurfaceForPixmap(this, false);
        ctx->d_func()->eglSurface = (EGLSurface)gl_surface;
        ctx->d_func()->valid = true; // ;-)
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
