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

GLXFBConfig qt_glx_integration_choose_config(MyDisplay* xd, QGLFormat& format, int drawableType)
{
    int depthSize = 0;
    int stencilSize = 0;
    int sampleSize = 0;

    if (format.depth())
        depthSize = (format.depthBufferSize() == -1) ? 1 : format.depthBufferSize();
    if (format.stencil())
        stencilSize = (format.stencilBufferSize() == -1) ? 1 : format.stencilBufferSize();
    if (format.sampleBuffers())
        sampleSize = (format.samples() == -1) ? 1 : format.samples();

    int configAttribs[] = {
        GLX_DRAWABLE_TYPE, drawableType,
        GLX_LEVEL, format.plane(),
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, format.doubleBuffer() ? True : False,
        GLX_STEREO, format.stereo() ? True : False,

        GLX_DEPTH_SIZE, depthSize,
        GLX_STENCIL_SIZE, stencilSize,
        GLX_SAMPLE_BUFFERS_ARB, sampleSize,

        GLX_RED_SIZE, (format.redBufferSize() == -1) ? 1 : format.redBufferSize(),
        GLX_GREEN_SIZE, (format.greenBufferSize() == -1) ? 1 : format.greenBufferSize(),
        GLX_BLUE_SIZE, (format.blueBufferSize() == -1) ? 1 : format.blueBufferSize(),
        GLX_ALPHA_SIZE, (format.alphaBufferSize() == -1) ? 0 : format.alphaBufferSize(),

        GLX_ACCUM_RED_SIZE, (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize(),
        GLX_ACCUM_GREEN_SIZE, (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize(),
        GLX_ACCUM_BLUE_SIZE, (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize(),
        GLX_ACCUM_ALPHA_SIZE, (format.accumBufferSize() == -1) ? 0 : format.accumBufferSize(),
        XNone
    };

    GLXFBConfig *configs;
    int configCount = 0;
    configs = glXChooseFBConfig(xd->display, xd->screen, configAttribs, &configCount);

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

    // TODO: Populate the QGLFormat with the values of the GLXFBConfig

    XFree(configs);
    return chosenConfig;
}

QGLXGLContext::QGLXGLContext(WId winId, MyDisplay *xd, QGLFormat& format, QPlatformGLContext* shareContext)
    : QPlatformGLContext()
    , m_xd(xd)
    , m_drawable((Drawable)winId)
    , m_config(0)
    , m_context(0)
{
    GLXContext shareGlxContext = 0;
    if (shareContext)
        shareGlxContext = static_cast<QGLXGLContext*>(shareContext)->glxContext();

    m_config = qt_glx_integration_choose_config(m_xd, format, GLX_WINDOW_BIT);

    m_context = glXCreateNewContext(m_xd->display, m_config, GLX_RGBA_TYPE, shareGlxContext, True);
#ifdef MYX11_DEBUG
    qDebug() << "QGLXGLContext::create context" << m_context;
#endif

}

QGLXGLContext::~QGLXGLContext()
{
    if (m_context) {
        qDebug("Destroying GLX context 0x%x", m_context);
        glXDestroyContext(m_xd->display, m_context);
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

QT_END_NAMESPACE
