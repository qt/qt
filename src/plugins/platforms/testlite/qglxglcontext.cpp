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

#include "x11util.h"
#include "qglxglcontext.h"
#include <QtCore/QLibrary>

#include <GL/glx.h>

#if defined(Q_OS_LINUX) || defined(Q_OS_BSD4)
#include <dlfcn.h>
#endif


QT_BEGIN_NAMESPACE

QGLXGLContext::QGLXGLContext(Display *xdpy)
    : QPlatformGLContext()
    , m_display(xdpy)
    , m_context(0)
    , m_widget(0)
{
}

QGLXGLContext::~QGLXGLContext()
{
    if (m_context) {
        qDebug("Destroying GLX context 0x%x", m_context);
        glXDestroyContext(m_display, m_context);
    }
}

bool QGLXGLContext::create(QPaintDevice* device, const QGLFormat& format, QPlatformGLContext* shareContext)
{
    if (device->devType() != QInternal::Widget) {
        qWarning("Creating a GL context is only supported on QWidgets");
        return false;
    }

    m_widget = static_cast<QWidget*>(device);
    if (!m_widget->isTopLevel()) {
        qWarning("Creating a GL context is only supported on top-level QWidgets");
        return false;
    }

    // Get the XVisualInfo for the window:
//    XWindowAttributes windowAttribs;
//    XGetWindowAttributes(m_display, m_widget->winId(), &windowAttribs);
    XVisualInfo visualInfoTemplate;
    visualInfoTemplate.visualid = 33; //XVisualIDFromVisual(windowAttribs.visual);
    XVisualInfo *visualInfo;
    int matchingCount = 0;
    visualInfo = XGetVisualInfo(m_display, VisualIDMask, &visualInfoTemplate, &matchingCount);

    m_context = glXCreateContext(m_display, visualInfo, 0, True);

    qDebug("Created GLX context 0x%x for visual ID %d", m_context, visualInfoTemplate.visualid);

    return true;
}

void QGLXGLContext::makeCurrent()
{
    Window win = m_widget->winId();
    qDebug("QGLXGLContext::makeCurrent(window=0x%x, ctx=0x%x)", win, m_context);

    glXMakeCurrent(m_display, win, m_context);
}

void QGLXGLContext::doneCurrent()
{
    glXMakeCurrent(m_display, 0, 0);
}

void QGLXGLContext::swapBuffers()
{
    glXSwapBuffers(m_display, m_widget->winId());
}

void* QGLXGLContext::getProcAddress(const QString& procName)
{
    typedef void *(*qt_glXGetProcAddressARB)(const GLubyte *);
    static qt_glXGetProcAddressARB glXGetProcAddressARB = 0;
    static bool resolved = false;

    if (resolved && !glXGetProcAddressARB)
        return 0;
    if (!glXGetProcAddressARB) {
        QList<QByteArray> glxExt = QByteArray(glXGetClientString(m_display, GLX_EXTENSIONS)).split(' ');
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
                QLibrary lib(qt_gl_library_name());
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
