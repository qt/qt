/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <QtGui/qpaintdevice.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qwidget.h>
#include <QtCore/qdebug.h>

#include "qegl_p.h"

QT_BEGIN_NAMESPACE

static void noegl(const char *fn)
{
    qWarning() << fn << " called, but Qt configured without EGL" << endl;
}

#define NOEGL noegl(__FUNCTION__);

EGLDisplay QEglContext::dpy = 0;

QEglContext::QEglContext()
    : apiType(QEgl::OpenGL)
    , ctx(0)
    , cfg(0)
    , currentSurface(0)
    , current(false)
    , ownsContext(true)
    , sharing(false)
{
    NOEGL
}

QEglContext::~QEglContext()
{
    NOEGL
}

bool QEglContext::isValid() const
{
    NOEGL
    return false;
}

bool QEglContext::isCurrent() const
{
    NOEGL
    return false;
}

bool QEglContext::chooseConfig(const QEglProperties& properties, QEgl::PixelFormatMatch match)
{
    Q_UNUSED(properties)
    Q_UNUSED(match)
    NOEGL
    return false;
}

EGLSurface QEglContext::createSurface(QPaintDevice* device, const QEglProperties *properties)
{
    Q_UNUSED(device)
    Q_UNUSED(properties)
    NOEGL
    return 0;
}


// Create the EGLContext.
bool QEglContext::createContext(QEglContext *shareContext, const QEglProperties *properties)
{
    Q_UNUSED(shareContext)
    Q_UNUSED(properties)
    NOEGL
    return false;
}

// Destroy an EGL surface object.  If it was current on this context
// then call doneCurrent() for it first.
void QEglContext::destroySurface(EGLSurface surface)
{
    Q_UNUSED(surface)
    NOEGL
}

// Destroy the context.  Note: this does not destroy the surface.
void QEglContext::destroyContext()
{
    NOEGL
}

bool QEglContext::makeCurrent(EGLSurface surface)
{
    Q_UNUSED(surface)
    NOEGL
    return false;
}

bool QEglContext::doneCurrent()
{
    NOEGL
    return false;
}

// Act as though doneCurrent() was called, but keep the context
// and the surface active for the moment.  This allows makeCurrent()
// to skip a call to eglMakeCurrent() if we are using the same
// surface as the last set of painting operations.  We leave the
// currentContext() pointer as-is for now.
bool QEglContext::lazyDoneCurrent()
{
    NOEGL
    return false;
}

bool QEglContext::swapBuffers(EGLSurface surface)
{
    Q_UNUSED(surface)
    NOEGL
    return false;
}

bool QEglContext::configAttrib(int name, EGLint *value) const
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    NOEGL
    return false;
}

void QEglContext::clearError()
{
    NOEGL
    return;
}

EGLint QEglContext::error()
{
    NOEGL
    return 0;
}

EGLDisplay QEglContext::display()
{
    NOEGL
    return 0;
}

// Return the error string associated with a specific code.
QString QEglContext::errorString(EGLint code)
{
    Q_UNUSED(code)
    NOEGL
    return QString();
}

// Dump all of the EGL configurations supported by the system.
void QEglContext::dumpAllConfigs()
{
    NOEGL
}

QString QEglContext::extensions()
{
    NOEGL
    return QString();
}

bool QEglContext::hasExtension(const char* extensionName)
{
    Q_UNUSED(extensionName)
    NOEGL
    return false;
}

QEglContext *QEglContext::currentContext(QEgl::API api)
{
    Q_UNUSED(api)
    NOEGL
    return false;
}

void QEglContext::setCurrentContext(QEgl::API api, QEglContext *context)
{
    Q_UNUSED(api)
    Q_UNUSED(context)
    NOEGL
}

EGLNativeDisplayType QEglContext::nativeDisplay()
{
    NOEGL
    return 0;
}

void QEglContext::waitClient()
{
    NOEGL
}

void QEglContext::waitNative()
{
    NOEGL
}

QEglProperties QEglContext::configProperties(EGLConfig cfg) const
{
    Q_UNUSED(cfg)
    NOEGL
    return QEglProperties();
}

QT_END_NAMESPACE
