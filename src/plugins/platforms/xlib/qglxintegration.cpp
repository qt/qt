/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
#include "qxlibstatic.h"

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
#include "qglxintegration.h"

#include "qglxconvenience.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
#include <dlfcn.h>
#endif

QT_BEGIN_NAMESPACE

QGLXContext::QGLXContext(Window window, QXlibScreen *screen, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , m_screen(screen)
    , m_drawable((Drawable)window)
    , m_context(0)
{

    const QPlatformGLContext *sharePlatformContext;
    sharePlatformContext = format.sharedGLContext();
    GLXContext shareGlxContext = 0;
    if (sharePlatformContext)
        shareGlxContext = static_cast<const QGLXContext*>(sharePlatformContext)->glxContext();

    GLXFBConfig config = qglx_findConfig(screen->display()->nativeDisplay(),screen->xScreenNumber(),format);
    m_context = glXCreateNewContext(screen->display()->nativeDisplay(),config,GLX_RGBA_TYPE,shareGlxContext,TRUE);
    m_windowFormat = qglx_platformWindowFromGLXFBConfig(screen->display()->nativeDisplay(),config,m_context);

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
