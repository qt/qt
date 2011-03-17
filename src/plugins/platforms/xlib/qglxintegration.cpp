/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qxlibwindow.h"
#include "qxlibscreen.h"
#include "qxlibdisplay.h"

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

#include "qglxintegration.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
#include <dlfcn.h>
#endif

QT_BEGIN_NAMESPACE

QMutex QGLXContext::m_defaultSharedContextMutex(QMutex::Recursive);

QVector<int> QGLXContext::buildSpec(const QPlatformWindowFormat &format)
{
    QVector<int> spec(48);
    int i = 0;

    spec[i++] = GLX_LEVEL;
    spec[i++] = 0;
    spec[i++] = GLX_DRAWABLE_TYPE; spec[i++] = GLX_WINDOW_BIT;

    if (format.rgba()) {
        spec[i++] = GLX_RENDER_TYPE; spec[i++] = GLX_RGBA_BIT;
        spec[i++] = GLX_RED_SIZE; spec[i++] = (format.redBufferSize() == -1) ? 1 : format.redBufferSize();
        spec[i++] = GLX_GREEN_SIZE; spec[i++] =  (format.greenBufferSize() == -1) ? 1 : format.greenBufferSize();
        spec[i++] = GLX_BLUE_SIZE; spec[i++] = (format.blueBufferSize() == -1) ? 1 : format.blueBufferSize();
        if (format.alpha()) {
                spec[i++] = GLX_ALPHA_SIZE; spec[i++] = (format.alphaBufferSize() == -1) ? 1 : format.alphaBufferSize();
            }

        spec[i++] = GLX_ACCUM_RED_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 1 : format.accumBufferSize();
        spec[i++] = GLX_ACCUM_GREEN_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 1 : format.accumBufferSize();
        spec[i++] = GLX_ACCUM_BLUE_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 1 : format.accumBufferSize();

        if (format.alpha()) {
            spec[i++] = GLX_ACCUM_ALPHA_SIZE; spec[i++] = (format.accumBufferSize() == -1) ? 1 : format.accumBufferSize();
        }

    } else {
        spec[i++] = GLX_RENDER_TYPE; spec[i++] = GLX_COLOR_INDEX_BIT; //I'm really not sure if this works....
        spec[i++] = GLX_BUFFER_SIZE; spec[i++] = 8;
    }

    spec[i++] = GLX_DOUBLEBUFFER; spec[i++] = format.doubleBuffer() ? True : False;
    spec[i++] = GLX_STEREO; spec[i++] =  format.stereo() ? True : False;

    if (format.depth()) {
        spec[i++] = GLX_DEPTH_SIZE; spec[i++] = (format.depthBufferSize() == -1) ? 1 : format.depthBufferSize();
    }

    if (format.stencil()) {
        spec[i++] = GLX_STENCIL_SIZE; spec[i++] =  (format.stencilBufferSize() == -1) ? 1 : format.stencilBufferSize();
    }
    if (format.sampleBuffers()) {
        spec[i++] = GLX_SAMPLE_BUFFERS_ARB;
        spec[i++] = 1;
        spec[i++] = GLX_SAMPLES_ARB;
        spec[i++] = format.samples() == -1 ? 4 : format.samples();
    }

    spec[i++] = XNone;
    return spec;
}

GLXFBConfig QGLXContext::findConfig(const QXlibScreen *screen, const QPlatformWindowFormat &format)
{
    bool reduced = true;
    GLXFBConfig chosenConfig = 0;
    QPlatformWindowFormat reducedFormat = format;
    while (!chosenConfig && reduced) {
        QVector<int> spec = buildSpec(reducedFormat);
        int confcount = 0;
        GLXFBConfig *configs;
        configs = glXChooseFBConfig(screen->display()->nativeDisplay(),screen->xScreenNumber(),spec.constData(),&confcount);
        if (confcount)
        {
            for (int i = 0; i < confcount; i++) {
                chosenConfig = configs[i];
                // Make sure we try to get an ARGB visual if the format asked for an alpha:
                if (reducedFormat.alpha()) {
                    int alphaSize;
                    glXGetFBConfigAttrib(screen->display()->nativeDisplay(),configs[i],GLX_ALPHA_SIZE,&alphaSize);
                    if (alphaSize > 0)
                        break;
                } else {
                    break; // Just choose the first in the list if there's no alpha requested
                }
            }

            XFree(configs);
        }
        reducedFormat = reducePlatformWindowFormat(reducedFormat,&reduced);
    }

    if (!chosenConfig)
        qWarning("Warning no context created");

    return chosenConfig;
}

XVisualInfo *QGLXContext::findVisualInfo(const QXlibScreen *screen, const QPlatformWindowFormat &format)
{
    GLXFBConfig config = QGLXContext::findConfig(screen,format);
    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(screen->display()->nativeDisplay(),config);
    return visualInfo;
}

QPlatformWindowFormat QGLXContext::platformWindowFromGLXFBConfig(Display *display, GLXFBConfig config, GLXContext ctx)
{
    QPlatformWindowFormat format;
    int redSize     = 0;
    int greenSize   = 0;
    int blueSize    = 0;
    int alphaSize   = 0;
    int depthSize   = 0;
    int stencilSize = 0;
    int sampleBuffers = 0;
    int sampleCount = 0;
    int level       = 0;
    int rgba        = 0;
    int stereo      = 0;
    int accumSizeA  = 0;
    int accumSizeR  = 0;
    int accumSizeG  = 0;
    int accumSizeB  = 0;

    XVisualInfo *vi = glXGetVisualFromFBConfig(display,config);
    glXGetConfig(display,vi,GLX_RGBA,&rgba);
    XFree(vi);
    glXGetFBConfigAttrib(display, config, GLX_RED_SIZE,     &redSize);
    glXGetFBConfigAttrib(display, config, GLX_GREEN_SIZE,   &greenSize);
    glXGetFBConfigAttrib(display, config, GLX_BLUE_SIZE,    &blueSize);
    glXGetFBConfigAttrib(display, config, GLX_ALPHA_SIZE,   &alphaSize);
    glXGetFBConfigAttrib(display, config, GLX_DEPTH_SIZE,   &depthSize);
    glXGetFBConfigAttrib(display, config, GLX_STENCIL_SIZE, &stencilSize);
    glXGetFBConfigAttrib(display, config, GLX_SAMPLES,      &sampleBuffers);
    glXGetFBConfigAttrib(display, config, GLX_LEVEL,        &level);
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
    format.setSampleBuffers(sampleBuffers);
    if (format.sampleBuffers()) {
        glXGetFBConfigAttrib(display, config, GLX_SAMPLES_ARB, &sampleCount);
        format.setSamples(sampleCount);
    }

    format.setDirectRendering(glXIsDirect(display, ctx));
    format.setRgba(rgba);
    format.setStereo(stereo);
    format.setAccumBufferSize(accumSizeB);

    return format;
}

QPlatformWindowFormat QGLXContext::reducePlatformWindowFormat(const QPlatformWindowFormat &format, bool *reduced)
{
    QPlatformWindowFormat retFormat = format;
    *reduced = true;

    if (retFormat.sampleBuffers()) {
        retFormat.setSampleBuffers(false);
    } else if (retFormat.stereo()) {
        retFormat.setStereo(false);
    } else if (retFormat.accum()) {
        retFormat.setAccum(false);
    }else if (retFormat.stencil()) {
        retFormat.setStencil(false);
    }else if (retFormat.alpha()) {
        retFormat.setAlpha(false);
    }else if (retFormat.depth()) {
        retFormat.setDepth(false);
    }else if (retFormat.doubleBuffer()) {
        retFormat.setDoubleBuffer(false);
    }else{
        *reduced = false;
    }
    return retFormat;
}

QGLXContext::QGLXContext(Window window, QXlibScreen *screen, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , m_screen(screen)
    , m_drawable((Drawable)window)
    , m_context(0)
{

    const QPlatformGLContext *sharePlatformContext;
    if (format.useDefaultSharedContext()) {
        if (!QPlatformGLContext::defaultSharedContext()) {
            if (m_defaultSharedContextMutex.tryLock()){
                createDefaultSharedContex(screen);
                m_defaultSharedContextMutex.unlock();
            } else {
                m_defaultSharedContextMutex.lock(); //wait to the the shared context is created
                m_defaultSharedContextMutex.unlock();
            }
        }
        sharePlatformContext = QPlatformGLContext::defaultSharedContext();
    } else {
        sharePlatformContext = format.sharedGLContext();
    }
    GLXContext shareGlxContext = 0;
    if (sharePlatformContext)
        shareGlxContext = static_cast<const QGLXContext*>(sharePlatformContext)->glxContext();

    GLXFBConfig config = findConfig(screen,format);
    m_context = glXCreateNewContext(screen->display()->nativeDisplay(),config,GLX_RGBA_TYPE,shareGlxContext,TRUE);
    m_windowFormat = QGLXContext::platformWindowFromGLXFBConfig(screen->display()->nativeDisplay(),config,m_context);

#ifdef MYX11_DEBUG
    qDebug() << "QGLXGLContext::create context" << m_context;
#endif
}

QGLXContext::QGLXContext(QXlibScreen *screen, Drawable drawable, GLXContext context)
    : QPlatformGLContext(), m_screen(screen), m_drawable(drawable), m_context(context)
{

}

QGLXContext::~QGLXContext()
{
    if (m_context) {
        qDebug("Destroying GLX context 0x%p", m_context);
        glXDestroyContext(m_screen->display()->nativeDisplay(), m_context);
    }
}

void QGLXContext::createDefaultSharedContex(QXlibScreen *screen)
{
    int x = 0;
    int y = 0;
    int w = 3;
    int h = 3;

    QPlatformWindowFormat format = QPlatformWindowFormat::defaultFormat();
    GLXContext context;
    GLXFBConfig config = findConfig(screen,format);
    if (config) {
        XVisualInfo *visualInfo = glXGetVisualFromFBConfig(screen->display()->nativeDisplay(),config);
        Colormap cmap = XCreateColormap(screen->display()->nativeDisplay(),screen->rootWindow(),visualInfo->visual,AllocNone);
        XSetWindowAttributes a;
        a.colormap = cmap;
        Window sharedWindow = XCreateWindow(screen->display()->nativeDisplay(), screen->rootWindow(),x, y, w, h,
                                  0, visualInfo->depth, InputOutput, visualInfo->visual,
                                  CWColormap, &a);

        context = glXCreateNewContext(screen->display()->nativeDisplay(),config,GLX_RGBA_TYPE,0,TRUE);
        QPlatformGLContext *sharedContext = new QGLXContext(screen,sharedWindow,context);
        QPlatformGLContext::setDefaultSharedContext(sharedContext);
    } else {
        qWarning("Warning no shared context created");
    }
}

void QGLXContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();
#ifdef MYX11_DEBUG
    qDebug("QGLXGLContext::makeCurrent(window=0x%x, ctx=0x%x)", m_drawable, m_context);
#endif
    glXMakeCurrent(m_screen->display()->nativeDisplay(), m_drawable, m_context);
}

void QGLXContext::doneCurrent()
{
    QPlatformGLContext::doneCurrent();
    glXMakeCurrent(m_screen->display()->nativeDisplay(), 0, 0);
}

void QGLXContext::swapBuffers()
{
    glXSwapBuffers(m_screen->display()->nativeDisplay(), m_drawable);
}

void* QGLXContext::getProcAddress(const QString& procName)
{
    typedef void *(*qt_glXGetProcAddressARB)(const GLubyte *);
    static qt_glXGetProcAddressARB glXGetProcAddressARB = 0;
    static bool resolved = false;

    if (resolved && !glXGetProcAddressARB)
        return 0;
    if (!glXGetProcAddressARB) {
        QList<QByteArray> glxExt = QByteArray(glXGetClientString(m_screen->display()->nativeDisplay(), GLX_EXTENSIONS)).split(' ');
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

QPlatformWindowFormat QGLXContext::platformWindowFormat() const
{
    return m_windowFormat;
}

QT_END_NAMESPACE

#endif //!defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
