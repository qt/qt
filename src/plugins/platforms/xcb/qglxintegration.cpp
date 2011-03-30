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

#include "qxcbwindow.h"
#include "qxcbscreen.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

#include "qglxintegration.h"
#include "qglxconvenience.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
#include <dlfcn.h>
#endif

QMutex QGLXContext::m_defaultSharedContextMutex(QMutex::Recursive);

QGLXContext::QGLXContext(Window window, QXcbScreen *screen, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , m_screen(screen)
    , m_drawable((Drawable)window)
    , m_context(0)
{
    const QPlatformGLContext *sharePlatformContext;
    if (format.useDefaultSharedContext()) {
        if (!QPlatformGLContext::defaultSharedContext()) {
            if (m_defaultSharedContextMutex.tryLock()){
                createDefaultSharedContext(screen);
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

    GLXFBConfig config = qglx_findConfig(DISPLAY_FROM_XCB(screen),screen->screenNumber(),format);
    m_context = glXCreateNewContext(DISPLAY_FROM_XCB(screen), config, GLX_RGBA_TYPE, shareGlxContext, TRUE);
    m_windowFormat = qglx_platformWindowFromGLXFBConfig(DISPLAY_FROM_XCB(screen), config, m_context);
}

QGLXContext::QGLXContext(QXcbScreen *screen, Drawable drawable, GLXContext context)
    : QPlatformGLContext(), m_screen(screen), m_drawable(drawable), m_context(context)
{

}

QGLXContext::~QGLXContext()
{
    if (m_context)
        glXDestroyContext(DISPLAY_FROM_XCB(m_screen), m_context);
}

void QGLXContext::createDefaultSharedContext(QXcbScreen *screen)
{
    int x = 0;
    int y = 0;
    int w = 3;
    int h = 3;

    QPlatformWindowFormat format = QPlatformWindowFormat::defaultFormat();
    GLXContext context;
    GLXFBConfig config = qglx_findConfig(DISPLAY_FROM_XCB(screen),screen->screenNumber(),format);
    if (config) {
        XVisualInfo *visualInfo = glXGetVisualFromFBConfig(DISPLAY_FROM_XCB(screen), config);
        Colormap cmap = XCreateColormap(DISPLAY_FROM_XCB(screen), screen->root(), visualInfo->visual, AllocNone);
        XSetWindowAttributes a;
        a.colormap = cmap;
        Window sharedWindow = XCreateWindow(DISPLAY_FROM_XCB(screen), screen->root(), x, y, w, h,
                                  0, visualInfo->depth, InputOutput, visualInfo->visual,
                                  CWColormap, &a);

        context = glXCreateNewContext(DISPLAY_FROM_XCB(screen), config, GLX_RGBA_TYPE, 0, TRUE);
        QPlatformGLContext *sharedContext = new QGLXContext(screen, sharedWindow, context);
        QPlatformGLContext::setDefaultSharedContext(sharedContext);
    } else {
        qWarning("Warning no shared context created");
    }
}

void QGLXContext::makeCurrent()
{
    m_screen->connection()->setEventProcessingEnabled(false);
    QPlatformGLContext::makeCurrent();
    glXMakeCurrent(DISPLAY_FROM_XCB(m_screen), m_drawable, m_context);
}

void QGLXContext::doneCurrent()
{
    QPlatformGLContext::doneCurrent();
    glXMakeCurrent(DISPLAY_FROM_XCB(m_screen), 0, 0);
    m_screen->connection()->setEventProcessingEnabled(true);
}

void QGLXContext::swapBuffers()
{
    glXSwapBuffers(DISPLAY_FROM_XCB(m_screen), m_drawable);
    doneCurrent();
}

void* QGLXContext::getProcAddress(const QString& procName)
{
    typedef void *(*qt_glXGetProcAddressARB)(const GLubyte *);
    static qt_glXGetProcAddressARB glXGetProcAddressARB = 0;
    static bool resolved = false;

    if (resolved && !glXGetProcAddressARB)
        return 0;
    if (!glXGetProcAddressARB) {
        QList<QByteArray> glxExt = QByteArray(glXGetClientString(DISPLAY_FROM_XCB(m_screen), GLX_EXTENSIONS)).split(' ');
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
