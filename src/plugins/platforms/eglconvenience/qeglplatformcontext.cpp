/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qeglplatformcontext.h"

#include <EGL/egl.h>

#include <QtCore/QDebug>
#include <QtGui/QPlatformWindow>

QEGLPlatformContext::QEGLPlatformContext(EGLDisplay display, EGLConfig config, EGLint contextAttrs[], EGLSurface surface, EGLenum eglApi)
    : m_eglDisplay(display)
    , m_eglSurface(surface)
    , m_eglApi(eglApi)
{
    if (m_eglSurface == EGL_NO_SURFACE) {
        qWarning("Createing QEGLPlatformContext with no surface");
    }

    eglBindAPI(m_eglApi);
    m_eglContext = eglCreateContext(m_eglDisplay,config, 0,contextAttrs);
    if (!m_eglContext) {
        qErrnoWarning("QEGLPlatformContext could not create eglContext");
    }
}

QEGLPlatformContext::~QEGLPlatformContext()
{
    if (m_eglSurface != EGL_NO_SURFACE) {
        doneCurrent();
        eglDestroySurface(m_eglDisplay, m_eglSurface);
        m_eglSurface = EGL_NO_SURFACE;
    }

    if (m_eglContext != EGL_NO_CONTEXT) {
        eglDestroyContext(m_eglDisplay, m_eglContext);
        m_eglContext = EGL_NO_CONTEXT;
    }
}

void QEGLPlatformContext::makeCurrent()
{
    eglBindAPI(m_eglApi);
    bool ok = eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
    if (!ok)
        qWarning() << "QEGLPlatformContext::makeCurrent(" << m_eglSurface << "):" << eglGetError();
}
void QEGLPlatformContext::doneCurrent()
{
    eglBindAPI(m_eglApi);
    bool ok = eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (!ok)
        qWarning() << "QEGLPlatformContext::doneCurrent():" << eglGetError();
}
void QEGLPlatformContext::swapBuffers()
{
    eglBindAPI(m_eglApi);
    bool ok = eglSwapBuffers(m_eglDisplay, m_eglSurface);
    if (!ok)
        qWarning() << "QEGLPlatformContext::swapBuffers():" << eglGetError();
}
void* QEGLPlatformContext::getProcAddress(const QString& procName)
{
    eglBindAPI(m_eglApi);
    return (void *)eglGetProcAddress(qPrintable(procName));
}
