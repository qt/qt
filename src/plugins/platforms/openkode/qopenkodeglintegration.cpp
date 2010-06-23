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

#include "qopenkodeglintegration.h"
#include <QtCore/QDebug>
#include <QtGui/QPlatformWindow>
#include <QtGui/private/qapplication_p.h>

#include <QtOpenGL/private/qwindowsurface_gl_p.h>

#include <EGL/eglext.h>

void qt_eglproperties_set_glformat(QEglProperties& eglProperties, const QGLFormat& glFormat)
{
    int redSize     = glFormat.redBufferSize();
    int greenSize   = glFormat.greenBufferSize();
    int blueSize    = glFormat.blueBufferSize();
    int alphaSize   = glFormat.alphaBufferSize();
    int depthSize   = glFormat.depthBufferSize();
    int stencilSize = glFormat.stencilBufferSize();
    int sampleCount = glFormat.samples();

    // QGLFormat uses a magic value of -1 to indicate "don't care", even when a buffer of that
    // type has been requested. So we must check QGLFormat's booleans too if size is -1:
    if (glFormat.alpha() && alphaSize <= 0)
        alphaSize = 1;
    if (glFormat.depth() && depthSize <= 0)
        depthSize = 1;
    if (glFormat.stencil() && stencilSize <= 0)
        stencilSize = 1;
    if (glFormat.sampleBuffers() && sampleCount <= 0)
        sampleCount = 1;

    // We want to make sure 16-bit configs are chosen over 32-bit configs as they will provide
    // the best performance. The EGL config selection algorithm is a bit stange in this regard:
    // The selection criteria for EGL_BUFFER_SIZE is "AtLeast", so we can't use it to discard
    // 32-bit configs completely from the selection. So it then comes to the sorting algorithm.
    // The red/green/blue sizes have a sort priority of 3, so they are sorted by first. The sort
    // order is special and described as "by larger _total_ number of color bits.". So EGL will
    // put 32-bit configs in the list before the 16-bit configs. However, the spec also goes on
    // to say "If the requested number of bits in attrib_list for a particular component is 0,
    // then the number of bits for that component is not considered". This part of the spec also
    // seems to imply that setting the red/green/blue bits to zero means none of the components
    // are considered and EGL disregards the entire sorting rule. It then looks to the next
    // highest priority rule, which is EGL_BUFFER_SIZE. Despite the selection criteria being
    // "AtLeast" for EGL_BUFFER_SIZE, it's sort order is "smaller" meaning 16-bit configs are
    // put in the list before 32-bit configs. So, to make sure 16-bit is preffered over 32-bit,
    // we must set the red/green/blue sizes to zero. This has an unfortunate consequence that
    // if the application sets the red/green/blue size to 5/6/5 on the QGLFormat, they will
    // probably get a 32-bit config, even when there's an RGB565 config avaliable. Oh well.

    // Now normalize the values so -1 becomes 0
    redSize   = redSize   > 0 ? redSize   : 0;
    greenSize = greenSize > 0 ? greenSize : 0;
    blueSize  = blueSize  > 0 ? blueSize  : 0;
    alphaSize = alphaSize > 0 ? alphaSize : 0;
    depthSize = depthSize > 0 ? depthSize : 0;
    stencilSize = stencilSize > 0 ? stencilSize : 0;
    sampleCount = sampleCount > 0 ? sampleCount : 0;

    eglProperties.setValue(EGL_RED_SIZE,   redSize);
    eglProperties.setValue(EGL_GREEN_SIZE, greenSize);
    eglProperties.setValue(EGL_BLUE_SIZE,  blueSize);
    eglProperties.setValue(EGL_ALPHA_SIZE, alphaSize);
    eglProperties.setValue(EGL_DEPTH_SIZE, depthSize);
    eglProperties.setValue(EGL_STENCIL_SIZE, stencilSize);
    eglProperties.setValue(EGL_SAMPLES, sampleCount);
    eglProperties.setValue(EGL_SAMPLE_BUFFERS, sampleCount ? 1 : 0);
}

// Updates "format" with the parameters of the selected configuration.
void qt_glformat_from_eglconfig(QGLFormat& format, const EGLConfig config)
{
    EGLint redSize     = 0;
    EGLint greenSize   = 0;
    EGLint blueSize    = 0;
    EGLint alphaSize   = 0;
    EGLint depthSize   = 0;
    EGLint stencilSize = 0;
    EGLint sampleCount = 0;
    EGLint level       = 0;

    EGLDisplay display = QEgl::display();
    eglGetConfigAttrib(display, config, EGL_RED_SIZE,     &redSize);
    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE,   &greenSize);
    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE,    &blueSize);
    eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE,   &alphaSize);
    eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE,   &depthSize);
    eglGetConfigAttrib(display, config, EGL_STENCIL_SIZE, &stencilSize);
    eglGetConfigAttrib(display, config, EGL_SAMPLES,      &sampleCount);
    eglGetConfigAttrib(display, config, EGL_LEVEL,        &level);

    format.setRedBufferSize(redSize);
    format.setGreenBufferSize(greenSize);
    format.setBlueBufferSize(blueSize);
    format.setAlphaBufferSize(alphaSize);
    format.setDepthBufferSize(depthSize);
    format.setStencilBufferSize(stencilSize);
    format.setSamples(sampleCount);
    format.setPlane(level + 1);      // EGL calls level 0 "normal" whereas Qt calls 1 "normal"
    format.setDirectRendering(true); // All EGL contexts are direct-rendered
    format.setRgba(true);            // EGL doesn't support colour index rendering
    format.setStereo(false);         // EGL doesn't support stereo buffers
    format.setAccumBufferSize(0);    // EGL doesn't support accululation buffers

    // Clear the EGL error state because some of the above may
    // have errored out because the attribute is not applicable
    // to the surface type.  Such errors don't matter.
    eglGetError();
}

QEGLPlatformContext::QEGLPlatformContext()
{
}

QEGLPlatformContext::~QEGLPlatformContext()
{
    if (m_eglSurface != EGL_NO_SURFACE) {
        doneCurrent();
        eglDestroySurface(QEgl::display(), m_eglSurface);
        m_eglSurface = EGL_NO_SURFACE;
    }

    if (m_context != EGL_NO_CONTEXT) {
        eglDestroyContext(QEgl::display(), m_context);
        m_context = EGL_NO_CONTEXT;
    }
}

bool QEGLPlatformContext::create(QPaintDevice* device, QGLFormat& format, QPlatformGLContext* shareContext)
{
    QEglProperties properties;
    properties.setValue(EGL_CONTEXT_CLIENT_VERSION, 2);
    //lets go with all defaults :) Seems like we get bad attributes for anything else
    //qt_eglproperties_set_glformat(properties,format);
    format.setDepthBufferSize(1);
    EGLConfig config = QEgl::defaultConfig(device->devType(), QEgl::OpenGL,QEgl::Renderable);
    QEGLPlatformContext *eglShareContext  = static_cast<QEGLPlatformContext *>(shareContext);
    if (shareContext && eglShareContext->m_context != EGL_NO_CONTEXT) {
        m_context = eglCreateContext(QEgl::display(), config, eglShareContext->m_context, properties.properties());
        if (m_context == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Could not share context:" << QEgl::errorString();
            shareContext = 0;
        }
    } else {
        m_context = eglCreateContext(QEgl::display(), config, EGL_NO_CONTEXT, properties.properties());
        if (m_context == EGL_NO_CONTEXT) {
            qWarning() << "QEglContext::createContext(): Unable to create EGL context:" << QEgl::errorString();
            return false;
        }
    }
    //Get/create the EGLSurface
    if (device && device->devType() == QInternal::Widget){
        QWidget* widget = static_cast<QWidget*>(device);
        QGLWidget* glWidget = qobject_cast<QGLWidget*>(widget);

        if (!widget->isTopLevel()) {
            qWarning("Creating a GL context is only supported on top-level QWidgets");
            return false;
        }
        EGLNativeWindowType winId = (EGLNativeWindowType) widget->window()->winId();

        m_eglSurface = eglCreateWindowSurface(QEgl::display(), config, winId, 0);
        makeCurrent();
        glClearColor(0.0, 0.0, 0.0, 1.0);
        swapBuffers();


    } else {
        qDebug() << "QEGLPlatformContext::create: didn't create surface!!!!!";
    }
    return true;

}

void QEGLPlatformContext::makeCurrent()
{
    eglBindAPI(EGL_OPENGL_API);
    bool ok = eglMakeCurrent(QEgl::display(), m_eglSurface, m_eglSurface, m_context);
    if (!ok)
        qWarning() << "QEGLPlatformContext::makeCurrent(" << m_eglSurface << "):" << QEgl::errorString();
}
void QEGLPlatformContext::doneCurrent()
{
    eglBindAPI(EGL_OPENGL_ES_API);
    bool ok = eglMakeCurrent(QEgl::display(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (!ok)
        qWarning() << "QEGLPlatformContext::doneCurrent():" << QEgl::errorString();
}
void QEGLPlatformContext::swapBuffers()
{
    bool ok = eglSwapBuffers(QEgl::display(), m_eglSurface);
    if (!ok)
        qWarning() << "QEGLPlatformContext::swapBuffers():" << QEgl::errorString();
}
void* QEGLPlatformContext::getProcAddress(const QString& procName)
{
    eglGetProcAddress(qPrintable(procName));
}
