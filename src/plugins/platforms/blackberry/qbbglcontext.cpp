/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBGLCONTEXT_DEBUG


#include "qbbglcontext.h"
#include "qbbscreen.h"
#include "qbbwindow.h"
#include "../eglconvenience/qeglconvenience.h"

#include <QtGui/QWidget>
#include <QDebug>

QT_BEGIN_NAMESPACE

EGLDisplay QBBGLContext::sEglDisplay = EGL_NO_DISPLAY;
EGLConfig QBBGLContext::sEglConfig = 0;
QPlatformWindowFormat QBBGLContext::sWindowFormat;

QBBGLContext::QBBGLContext(QBBWindow* platformWindow)
    : QPlatformGLContext(),
      mPlatformWindow(platformWindow),
      mEglSurface(EGL_NO_SURFACE)
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::QBBGLContext - w=" << mPlatformWindow->widget();
#endif

    // verify rendering API is correct
    QPlatformWindowFormat format = mPlatformWindow->widget()->platformWindowFormat();
    if (format.windowApi() != QPlatformWindowFormat::OpenGL) {
        qFatal("QBB: window API is not OpenGL");
    }

    // set current rendering API
    EGLBoolean eglResult = eglBindAPI(EGL_OPENGL_ES_API);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set EGL API, err=%d", eglGetError());
    }

    // get colour channel sizes from window format
    int alphaSize = format.alphaBufferSize();
    int redSize = format.redBufferSize();
    int greenSize = format.greenBufferSize();
    int blueSize = format.blueBufferSize();

    // check if all channels are don't care
    if (alphaSize == -1 && redSize == -1 && greenSize == -1 && blueSize == -1) {

        // set colour channels based on depth of window's screen
        int depth = platformWindow->screen()->depth();
        if (depth == 32) {
            // SCREEN_FORMAT_RGBA8888
            alphaSize = 8;
            redSize = 8;
            greenSize = 8;
            blueSize = 8;
        } else {
            // SCREEN_FORMAT_RGB565
            alphaSize = 0;
            redSize = 5;
            greenSize = 6;
            blueSize = 5;
        }

    } else {

        // choose best match based on supported pixel formats
        if (alphaSize <= 0 && redSize <= 5 && greenSize <= 6 && blueSize <= 5) {
            // SCREEN_FORMAT_RGB565
            alphaSize = 0;
            redSize = 5;
            greenSize = 6;
            blueSize = 5;
        } else {
            // SCREEN_FORMAT_RGBA8888
            alphaSize = 8;
            redSize = 8;
            greenSize = 8;
            blueSize = 8;
        }
    }

    // update colour channel sizes in window format
    format.setAlphaBufferSize(alphaSize);
    format.setRedBufferSize(redSize);
    format.setGreenBufferSize(greenSize);
    format.setBlueBufferSize(blueSize);

    // select EGL config based on requested window format
    sEglConfig = q_configFromQPlatformWindowFormat(sEglDisplay, format);
    if (sEglConfig == 0) {
        qFatal("QBB: failed to find EGL config");
    }

    mEglContext = eglCreateContext(sEglDisplay, sEglConfig, EGL_NO_CONTEXT, contextAttrs());
    if (mEglContext == EGL_NO_CONTEXT) {
        qFatal("QBB: failed to create EGL context, err=%d", eglGetError());
    }

    // query/cache window format of selected EGL config
    sWindowFormat = qt_qPlatformWindowFormatFromConfig(sEglDisplay, sEglConfig);
}

QBBGLContext::~QBBGLContext()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::~QBBGLContext - w=" << mPlatformWindow->widget();
#endif

    // cleanup EGL context if it exists
    if (mEglContext != EGL_NO_CONTEXT) {
        eglDestroyContext(sEglDisplay, mEglContext);
    }

    // cleanup EGL surface if it exists
    destroySurface();
}

void QBBGLContext::initialize()
{
    // initialize connection to EGL
    sEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (sEglDisplay == EGL_NO_DISPLAY) {
        qFatal("QBB: failed to obtain EGL display");
    }

    EGLBoolean eglResult = eglInitialize(sEglDisplay, NULL, NULL);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to initialize EGL display, err=%d", eglGetError());
    }

    // choose EGL settings based on OpenGL version
#if defined(QT_OPENGL_ES_2)
    EGLint renderableType = EGL_OPENGL_ES2_BIT;
#else
    EGLint renderableType = EGL_OPENGL_ES_BIT;
#endif

    // get EGL config compatible with window
    EGLint numConfig;
    EGLint configAttrs[] = { EGL_BUFFER_SIZE,       32,
                             EGL_ALPHA_SIZE,        8,
                             EGL_RED_SIZE,          8,
                             EGL_GREEN_SIZE,        8,
                             EGL_BLUE_SIZE,         8,
                             EGL_SURFACE_TYPE,      EGL_WINDOW_BIT,
                             EGL_RENDERABLE_TYPE,   renderableType,
                             EGL_NONE };
    eglResult = eglChooseConfig(sEglDisplay, configAttrs, &sEglConfig, 1, &numConfig);
    if (eglResult != EGL_TRUE || numConfig == 0) {
        qFatal("QBB: failed to find EGL config, err=%d, numConfig=%d", eglGetError(), numConfig);
    }

    // query/cache window format
    sWindowFormat = qt_qPlatformWindowFormatFromConfig(sEglDisplay, sEglConfig);
    sWindowFormat.setWindowApi(QPlatformWindowFormat::OpenGL);
}

void QBBGLContext::shutdown()
{
    // close connection to EGL
    eglTerminate(sEglDisplay);
}

void QBBGLContext::makeCurrent()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::makeCurrent - w=" << mPlatformWindow->widget();
#endif

    if (!mPlatformWindow->hasBuffers()) {
        // NOTE: create a dummy EGL surface since Qt will call makeCurrent() before
        // setting the geometry on the associated window
        mSurfaceSize = QSize(1, 1);
        mPlatformWindow->setBufferSize(mSurfaceSize);
        createSurface();
    }

    // call the parent method
    QPlatformGLContext::makeCurrent();

    // set current rendering API
    EGLBoolean eglResult = eglBindAPI(EGL_OPENGL_ES_API);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set EGL API, err=%d", eglGetError());
    }

    // set current EGL context and bind with EGL surface
    eglResult = eglMakeCurrent(sEglDisplay, mEglSurface, mEglSurface, mEglContext);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set current EGL context, err=%d", eglGetError());
    }
}

void QBBGLContext::doneCurrent()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::doneCurrent - w=" << mPlatformWindow->widget();
#endif

    // call the parent method
    QPlatformGLContext::doneCurrent();

    // set current rendering API
    EGLBoolean eglResult = eglBindAPI(EGL_OPENGL_ES_API);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set EGL API, err=%d", eglGetError());
    }

    // clear curent EGL context and unbind EGL surface
    eglResult = eglMakeCurrent(sEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to clear current EGL context, err=%d", eglGetError());
    }
}

void QBBGLContext::swapBuffers()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::swapBuffers - w=" << mPlatformWindow->widget();
#endif

    // set current rendering API
    EGLBoolean eglResult = eglBindAPI(EGL_OPENGL_ES_API);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set EGL API, err=%d", eglGetError());
    }

    // post EGL surface to window
    eglResult = eglSwapBuffers(sEglDisplay, mEglSurface);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to swap EGL buffers, err=%d", eglGetError());
    }

    // resize surface if window was resized
    QSize s = mPlatformWindow->geometry().size();
    if (s != mSurfaceSize) {
        resizeSurface(s);
    }
}

void* QBBGLContext::getProcAddress(const QString& procName)
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::getProcAddress - w=" << mPlatformWindow->widget();
#endif

    // set current rendering API
    EGLBoolean eglResult = eglBindAPI(EGL_OPENGL_ES_API);
    if (eglResult != EGL_TRUE) {
        qFatal("QBB: failed to set EGL API, err=%d", eglGetError());
    }

    // lookup EGL extension function pointer
    return (void *)eglGetProcAddress( procName.toAscii().constData() );
}

void QBBGLContext::resizeSurface(const QSize &size)
{
    // need to destroy surface so make sure its not current
    bool restoreCurrent = false;
    if (isCurrent()) {
        doneCurrent();
        restoreCurrent = true;
    }

    // destroy old EGL surface
    destroySurface();

    // resize window's buffers
    mPlatformWindow->setBufferSize(size);

    // re-create EGL surface with new size
    mSurfaceSize = size;
    createSurface();

    // make context current again
    if (restoreCurrent) {
        makeCurrent();
    }
}

EGLint *QBBGLContext::contextAttrs()
{
    // choose EGL settings based on OpenGL version
#if defined(QT_OPENGL_ES_2)
    static EGLint attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    return attrs;
#else
    return NULL;
#endif
}

bool QBBGLContext::isCurrent() const
{
    return (eglGetCurrentContext() == mEglContext);
}

void QBBGLContext::createSurface()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::createSurface - w=" << mPlatformWindow->widget();
#endif

    // create EGL surface
    mEglSurface = eglCreateWindowSurface(sEglDisplay, sEglConfig, (EGLNativeWindowType)mPlatformWindow->winId(), NULL);
    if (mEglSurface == EGL_NO_SURFACE) {
        qFatal("QBB: failed to create EGL surface, err=%d", eglGetError());
    }
}

void QBBGLContext::destroySurface()
{
#if defined(QBBGLCONTEXT_DEBUG)
    qDebug() << "QBBGLContext::destroySurface - w=" << mPlatformWindow->widget();
#endif

    // destroy EGL surface if it exists
    if (mEglSurface != EGL_NO_SURFACE) {
        EGLBoolean eglResult = eglDestroySurface(sEglDisplay, mEglSurface);
        if (eglResult != EGL_TRUE) {
            qFatal("QBB: failed to destroy EGL surface, err=%d", eglGetError());
        }
    }
}

QT_END_NAMESPACE
