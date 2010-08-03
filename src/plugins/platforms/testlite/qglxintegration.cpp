/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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
#include <QLibrary>
#include <QGLFormat>

#include "qtestlitewindow.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

#include "qglxintegration.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
#include <dlfcn.h>
#endif

QT_BEGIN_NAMESPACE

QMutex QGLXGLContext::m_defaultSharedContextMutex(QMutex::Recursive);

QVector<int> QGLXGLContext::buildSpec(const QPlatformWindowFormat &format)
{
    QVector<int> spec(48);
    int i = 0;
    int depthSize = 0;
    int stencilSize = 0;
    int sampleSize = 0;

    if (format.depth())
        depthSize = (format.depthBufferSize() == -1) ? 1 : format.depthBufferSize();
    if (format.stencil())
        stencilSize = (format.stencilBufferSize() == -1) ? 1 : format.stencilBufferSize();
    if (format.sampleBuffers())
        sampleSize = (format.samples() == -1) ? 1 : format.samples();

    spec[i++] = GLX_DRAWABLE_TYPE; spec[i++] = GLX_WINDOW_BIT;
    spec[i++] = GLX_RENDER_TYPE; spec[i++] = GLX_RGBA_BIT;
    spec[i++] = GLX_DOUBLEBUFFER; spec[i++] = format.doubleBuffer() ? True : False;
    spec[i++] = GLX_STEREO; spec[i++] =  format.stereo() ? True : False;

    spec[i++] = GLX_DEPTH_SIZE; spec[i++] = depthSize;
    spec[i++] = GLX_STENCIL_SIZE; spec[i++] =  stencilSize;
    spec[i++] = GLX_SAMPLE_BUFFERS_ARB; spec[i++] = sampleSize;

    spec[i++] = GLX_RED_SIZE; spec[i++] = (format.redBufferSize() == -1) ? 1 : format.redBufferSize();
    spec[i++] = GLX_GREEN_SIZE; spec[i++] =  (format.greenBufferSize() == -1) ? 1 : format.greenBufferSize();
    spec[i++] = GLX_BLUE_SIZE; spec[i++] = (format.blueBufferSize() == -1) ? 1 : format.blueBufferSize();
    spec[i++] = GLX_ALPHA_SIZE; spec[i++] = (format.alphaBufferSize() == -1) ? 0 : format.alphaBufferSize();

    spec[i++] = GLX_ACCUM_RED_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize();
    spec[i++] = GLX_ACCUM_GREEN_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize();
    spec[i++] = GLX_ACCUM_BLUE_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize();
    spec[i++] = GLX_ACCUM_ALPHA_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize();
    spec[i++] = XNone;
    return spec;
}

GLXFBConfig QGLXGLContext::findConfig(const GLXFBConfig *configs, int configCount, const QPlatformWindowFormat &format, const MyDisplay *xd)
{
    if (!configs)
        return 0;

    GLXFBConfig chosenConfig = 0;
    for (int i = 0; i < configCount; ++i) {
        chosenConfig = configs[i];

        // Make sure we try to get an ARGB visual if the format asked for an alpha:
        if (format.alpha()) {
            XVisualInfo* vi;
            vi = glXGetVisualFromFBConfig(xd->display, configs[i]);
            if (!vi)
                continue;

            XRenderPictFormat *pictFormat = XRenderFindVisualFormat(xd->display, vi->visual);
            XFree(vi);

            if (pictFormat && (pictFormat->type == PictTypeDirect) && pictFormat->direct.alphaMask) {
                // The pict format for the visual matching the FBConfig indicates ARGB
                break;
            }
        } else
            break; // Just choose the first in the list if there's no alpha requested
    }
    return chosenConfig;
}

QPlatformWindowFormat QGLXGLContext::platformWindowFromGLXFBConfig(Display *display, GLXFBConfig config)
{
    QPlatformWindowFormat format;
    int redSize     = 0;
    int greenSize   = 0;
    int blueSize    = 0;
    int alphaSize   = 0;
    int depthSize   = 0;
    int stencilSize = 0;
    int sampleCount = 0;
    int level       = 0;
    int rgba        = 0;
    int stereo      = 0;
    int accumSizeA  = 0;
    int accumSizeR  = 0;
    int accumSizeG  = 0;
    int accumSizeB  = 0;

    glXGetFBConfigAttrib(display, config, GLX_RED_SIZE,     &redSize);
    glXGetFBConfigAttrib(display, config, GLX_GREEN_SIZE,   &greenSize);
    glXGetFBConfigAttrib(display, config, GLX_BLUE_SIZE,    &blueSize);
    glXGetFBConfigAttrib(display, config, GLX_ALPHA_SIZE,   &alphaSize);
    glXGetFBConfigAttrib(display, config, GLX_DEPTH_SIZE,   &depthSize);
    glXGetFBConfigAttrib(display, config, GLX_STENCIL_SIZE, &stencilSize);
    glXGetFBConfigAttrib(display, config, GLX_SAMPLES,      &sampleCount);
    glXGetFBConfigAttrib(display, config, GLX_LEVEL,        &level);
    glXGetFBConfigAttrib(display, config, GLX_RGBA,         &rgba);
    glXGetFBConfigAttrib(display, config, GLX_STEREO,       &stereo);
    glXGetFBConfigAttrib(display, config, GLX_ACCUM_ALPHA_SIZE, &accumSizeA);
    glXGetFBConfigAttrib(display, config, GLX_ACCUM_RED_SIZE, &accumSizeR);
    glXGetFBConfigAttrib(display, config, GLX_ACCUM_GREEN_SIZE, &accumSizeG);
    glXGetFBConfigAttrib(display, config, GLX_ACCUM_BLUE_SIZE, &accumSizeB);

    format.setRedBufferSize(redSize);
    format.setGreenBufferSize(greenSize);
    format.setBlueBufferSize(blueSize);
    format.setAlphaBufferSize(alphaSize);
    format.setDepthBufferSize(depthSize);
    format.setStencilBufferSize(stencilSize);
    format.setSamples(sampleCount);
    format.setDirectRendering(true); // We don't support anything else for now.
    format.setRgba(rgba);
    format.setStereo(stereo);
    format.setAccumBufferSize(accumSizeA + accumSizeB + accumSizeG + accumSizeR);

    return format;
}

QGLXGLContext::QGLXGLContext(Window window, MyDisplay *xd, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , m_xd(xd)
    , m_drawable((Drawable)window)
    , m_context(0)
{
    if (!QPlatformGLContext::defaultSharedContext()) {
        if (m_defaultSharedContextMutex.tryLock()){
            createDefaultSharedContex(xd);
            m_defaultSharedContextMutex.unlock();
        } else {
            m_defaultSharedContextMutex.lock(); //wait to the the shared context is created
            m_defaultSharedContextMutex.unlock();
        }
    }

    QPlatformGLContext *sharePlatformContext;
    if (format.useDefaultSharedContext()) {
        sharePlatformContext = QPlatformGLContext::defaultSharedContext();
    } else {
        sharePlatformContext = format.sharedGLContext();
    }
    GLXContext shareGlxContext = 0;
    if (sharePlatformContext)
        shareGlxContext = static_cast<QGLXGLContext*>(sharePlatformContext)->glxContext();

    QVector<int> spec = buildSpec(format);
    int confcount = 0;
    GLXFBConfig *configs;
    configs = glXChooseFBConfig(xd->display,xd->screen,spec.constData(),&confcount);
    if (confcount)
    {
        GLXFBConfig config = findConfig(configs,confcount,format,xd);
        m_context = glXCreateNewContext(xd->display,config,GLX_RGBA_TYPE,shareGlxContext,TRUE);
        m_windowFormat = QGLXGLContext::platformWindowFromGLXFBConfig(xd->display,config);
        XFree(configs);
    } else {
        qFatal("Warning no context created");
    }

#ifdef MYX11_DEBUG
    qDebug() << "QGLXGLContext::create context" << m_context;
#endif
}

QGLXGLContext::QGLXGLContext(MyDisplay *display, Drawable drawable, GLXContext context)
    : QPlatformGLContext(), m_xd(display), m_drawable(drawable), m_context(context)
{

}

QGLXGLContext::~QGLXGLContext()
{
    if (m_context) {
        qDebug("Destroying GLX context 0x%p", m_context);
        glXDestroyContext(m_xd->display, m_context);
    }
}

void QGLXGLContext::createDefaultSharedContex(MyDisplay *xd)
{
    int x = 0;
    int y = 0;
    int w = 3;
    int h = 3;

    Window sharedWindow = XCreateSimpleWindow(xd->display, xd->rootWindow(),
                                   x, y, w, h, 0 /*border_width*/,
                                   xd->blackPixel(), xd->whitePixel());
    GLXContext context;
    QPlatformWindowFormat format;
    QVector<int> spec = buildSpec(format);
    int confcount = 0;
    GLXFBConfig *configs;
    configs = glXChooseFBConfig(xd->display,xd->screen,spec.constData(),&confcount);
    if (confcount)
    {
        GLXFBConfig config = findConfig(configs,confcount,format,xd);
        context = glXCreateNewContext(xd->display,config,GLX_RGBA_TYPE,0,TRUE);
        XFree(configs);
        QPlatformGLContext *sharedContext = new QGLXGLContext(xd,sharedWindow,context);
        QPlatformGLContext::setDefaultSharedContext(sharedContext);
    } else {
        qFatal("Warning no shared context created");
    }

}

void QGLXGLContext::makeCurrent()
{
#ifdef MYX11_DEBUG
    qDebug("QGLXGLContext::makeCurrent(window=0x%x, ctx=0x%x)", m_drawable, m_context);
#endif
    glXMakeCurrent(m_xd->display, m_drawable, m_context);
}

void QGLXGLContext::doneCurrent()
{
    glXMakeCurrent(m_xd->display, 0, 0);
}

void QGLXGLContext::swapBuffers()
{
    glXSwapBuffers(m_xd->display, m_drawable);
}

void* QGLXGLContext::getProcAddress(const QString& procName)
{
    typedef void *(*qt_glXGetProcAddressARB)(const GLubyte *);
    static qt_glXGetProcAddressARB glXGetProcAddressARB = 0;
    static bool resolved = false;

    if (resolved && !glXGetProcAddressARB)
        return 0;
    if (!glXGetProcAddressARB) {
        QList<QByteArray> glxExt = QByteArray(glXGetClientString(m_xd->display, GLX_EXTENSIONS)).split(' ');
        if (glxExt.contains("GLX_ARB_get_proc_address")) {
#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
            void *handle = dlopen(NULL, RTLD_LAZY);
            if (handle) {
                glXGetProcAddressARB = (qt_glXGetProcAddressARB) dlsym(handle, "glXGetProcAddressARB");
                dlclose(handle);
            }
            if (!glXGetProcAddressARB)
#endif
            {
                extern const QString qt_gl_library_name();
//                QLibrary lib(qt_gl_library_name());
                QLibrary lib(QLatin1String("GL"));
                glXGetProcAddressARB = (qt_glXGetProcAddressARB) lib.resolve("glXGetProcAddressARB");
            }
        }
        resolved = true;
    }
    if (!glXGetProcAddressARB)
        return 0;
    return glXGetProcAddressARB(reinterpret_cast<const GLubyte *>(procName.toLatin1().data()));
}

QPlatformWindowFormat QGLXGLContext::platformWindowFormat() const
{
    return m_windowFormat;
}

QT_END_NAMESPACE
