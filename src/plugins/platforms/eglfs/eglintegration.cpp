/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "eglintegration.h"

#include <QtGui/QPlatformWindow>
#include <QtOpenGL/private/qwindowsurface_gl_p.h>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#include <QtOpenGL/private/qpaintengine_opengl_p.h>

#include <stdio.h>
#include <EGL/egl.h>
#include <QDebug>
#include <QWindowSystemInterface>

#include "../eglconvenience/qeglconvenience.h"

//#define QEGL_EXTRA_DEBUG
#ifdef QEGL_EXTRA_DEBUG
struct AttrInfo { EGLint attr; const char *name; };
static struct AttrInfo attrs[] = {
    {EGL_BUFFER_SIZE, "EGL_BUFFER_SIZE"},
    {EGL_ALPHA_SIZE, "EGL_ALPHA_SIZE"},
    {EGL_BLUE_SIZE, "EGL_BLUE_SIZE"},
    {EGL_GREEN_SIZE, "EGL_GREEN_SIZE"},
    {EGL_RED_SIZE, "EGL_RED_SIZE"},
    {EGL_DEPTH_SIZE, "EGL_DEPTH_SIZE"},
    {EGL_STENCIL_SIZE, "EGL_STENCIL_SIZE"},
    {EGL_CONFIG_CAVEAT, "EGL_CONFIG_CAVEAT"},
    {EGL_CONFIG_ID, "EGL_CONFIG_ID"},
    {EGL_LEVEL, "EGL_LEVEL"},
    {EGL_MAX_PBUFFER_HEIGHT, "EGL_MAX_PBUFFER_HEIGHT"},
    {EGL_MAX_PBUFFER_PIXELS, "EGL_MAX_PBUFFER_PIXELS"},
    {EGL_MAX_PBUFFER_WIDTH, "EGL_MAX_PBUFFER_WIDTH"},
    {EGL_NATIVE_RENDERABLE, "EGL_NATIVE_RENDERABLE"},
    {EGL_NATIVE_VISUAL_ID, "EGL_NATIVE_VISUAL_ID"},
    {EGL_NATIVE_VISUAL_TYPE, "EGL_NATIVE_VISUAL_TYPE"},
    {EGL_SAMPLES, "EGL_SAMPLES"},
    {EGL_SAMPLE_BUFFERS, "EGL_SAMPLE_BUFFERS"},
    {EGL_SURFACE_TYPE, "EGL_SURFACE_TYPE"},
    {EGL_TRANSPARENT_TYPE, "EGL_TRANSPARENT_TYPE"},
    {EGL_TRANSPARENT_BLUE_VALUE, "EGL_TRANSPARENT_BLUE_VALUE"},
    {EGL_TRANSPARENT_GREEN_VALUE, "EGL_TRANSPARENT_GREEN_VALUE"},
    {EGL_TRANSPARENT_RED_VALUE, "EGL_TRANSPARENT_RED_VALUE"},
    {EGL_BIND_TO_TEXTURE_RGB, "EGL_BIND_TO_TEXTURE_RGB"},
    {EGL_BIND_TO_TEXTURE_RGBA, "EGL_BIND_TO_TEXTURE_RGBA"},
    {EGL_MIN_SWAP_INTERVAL, "EGL_MIN_SWAP_INTERVAL"},
    {EGL_MAX_SWAP_INTERVAL, "EGL_MAX_SWAP_INTERVAL"},
    {-1, 0}};
#endif //QEGL_EXTRA_DEBUG


class QEglScreen;

class QEglContext : public QPlatformGLContext
{
public:
    QEglContext(QGLFormat& format, QPlatformGLContext* shareContext = 0);
    ~QEglContext();

    virtual void makeCurrent();
    virtual void doneCurrent();
    virtual void swapBuffers();
    virtual void* getProcAddress(const QString& procName);

    EGLContext eglContext() {return m_context;}
    QSize size() const { return m_size; }

    QGLFormat format() const { return m_format; }
private:

    EGLContext  m_context;
    EGLDisplay m_dpy;
    EGLSurface m_surface;
    QSize m_size;
    static bool singleton_watch;
    QGLFormat m_format;
};

bool QEglContext::singleton_watch = false;

QEglContext::QEglContext(QGLFormat& format, QPlatformGLContext* shareContext)
{
    if (singleton_watch)
        qFatal("There can be only one");
    singleton_watch = true;

    Q_UNUSED(shareContext);
    Q_UNUSED(format);

    EGLint major, minor;
#ifdef QEGL_EXTRA_DEBUG
    EGLint index;
#endif
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        fprintf(stderr, "Could not bind GL_ES API\n");
        qFatal("EGL error");
    }

    m_dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_dpy == EGL_NO_DISPLAY) {
        fprintf(stderr, "Could not open egl display\n");
        qFatal("EGL error");
    }
    fprintf(stderr, "Opened display %p\n", m_dpy);

    if (!eglInitialize(m_dpy, &major, &minor)) {
        fprintf(stderr, "Could not initialize egl display\n");
        qFatal("EGL error");
    }

    fprintf(stderr, "Initialized display %d %d\n", major, minor);

    QPlatformWindowFormat platformFormat;
    platformFormat.setDepth(16);
    platformFormat.setWindowApi(QPlatformWindowFormat::OpenGL);
    platformFormat.setRedBufferSize(5);
    platformFormat.setGreenBufferSize(6);
    platformFormat.setBlueBufferSize(5);
    EGLConfig config = q_configFromQPlatformWindowFormat(m_dpy, platformFormat);

#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "Configuration %d matches requirements\n", (int)config);

    for (index = 0; attrs[index].attr != -1; ++index) {
        EGLint value;
        if (eglGetConfigAttrib(m_dpy, config, attrs[index].attr, &value)) {
            printf("\t%s: %d\n", attrs[index].name, (int)value);
        }
    }
    printf("\n");
#endif
    EGLint temp;
    EGLint attribList[32];

    temp = 0;

    attribList[temp++] = EGL_CONTEXT_CLIENT_VERSION;
    attribList[temp++] = 2; // GLES version 2
    attribList[temp++] = EGL_NONE;


    m_context = eglCreateContext(m_dpy, config, NULL, attribList);
    if (m_context == EGL_NO_CONTEXT) {
        fprintf(stderr, "Could not create the egl context\n");
        eglTerminate(m_dpy);
        qFatal("EGL error");
    }

    m_surface = eglCreateWindowSurface(m_dpy, config, 0, NULL);
    if (m_surface == EGL_NO_SURFACE) {
        fprintf(stderr, "Could not create the egl surface: error = 0x%x\n", eglGetError());
        eglTerminate(m_dpy);
        qFatal("EGL error");
    }

    EGLint w,h;

    eglQuerySurface(m_dpy, m_surface, EGL_WIDTH, &w);
    eglQuerySurface(m_dpy, m_surface, EGL_HEIGHT, &h);
    fprintf(stderr, "Created surface %dx%d\n", w, h);

    m_size = QSize(w,h);


}

QEglContext::~QEglContext()
{
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglContext::~QEglContext()";
#endif
}

void QEglContext::makeCurrent()
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglContext::makeCurrent\n");
#endif
    if (!eglMakeCurrent(m_dpy, m_surface, m_surface, m_context))
        fprintf(stderr, "Could not make the egl context current\n");

#ifdef QEGL_EXTRA_DEBUG
    static bool showDebug = true;
    if (showDebug) {
        showDebug = false;
        const char *str = (const char*)glGetString(GL_VENDOR);
        qDebug() << "Vendor" << str;
        printf("Vendor %s\n", str);
        str = (const char*)glGetString(GL_RENDERER);
        qDebug() << "Renderer" << str;
        printf("Renderer %s\n", str);
        str = (const char*)glGetString(GL_VERSION);
        qDebug() << "Version" << str;
        printf("Version %s\n", str);

        str = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        qDebug() << "Shader version" << str;

        str = (const char*)glGetString(GL_EXTENSIONS);
        qDebug() << "Extensions" << str;
        printf("Extensions %s\n", str);

    }
#endif
}

void QEglContext::doneCurrent()
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglContext::doneCurrent\n");
#endif
    if (!eglMakeCurrent(m_dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
        fprintf(stderr, "Could not release the egl context\n");
}

void QEglContext::swapBuffers()
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglContext::swapBuffers\n");
#endif
    eglSwapBuffers(m_dpy, m_surface);
}

void* QEglContext::getProcAddress(const QString& procName)
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglContext::getProcAddress\n");
#endif
    return (void*)eglGetProcAddress(qPrintable(procName));
}


class QEglPaintDevice;
class QEglScreen : public QPlatformScreen
{
public:
    QEglScreen();
    ~QEglScreen() {}

    QRect geometry() const { return m_geometry; }
    int depth() const { return m_depth; }
    QImage::Format format() const { return m_format; }
    //QSize physicalSize() const { return m_physicalSize; } //###

public:
    QRect m_geometry;
    int m_depth;
    QImage::Format m_format;
    //QSize m_physicalSize;
    QEglContext * m_platformContext;
};


class QEglWindow : public QPlatformWindow
{
public:
    QEglWindow(QWidget *w, QEglScreen *screen);
    QPlatformGLContext *glContext();

    void setGeometry(const QRect &);
    WId winId() const;

    QGLContext *context() { return m_context; }
    QEglPaintDevice *paintDevice() {  return m_pd; }
private:
    QEglScreen *m_screen;
    QEglPaintDevice *m_pd;
    QGLContext *m_context;
    WId m_winid;
};

class QEglPaintDevice : public QGLPaintDevice
{
public:
    QEglPaintDevice(QEglScreen *screen, QEglWindow *window);

    QSize size() const { return m_screen->geometry().size(); }
    QGLContext* context() const { return m_window->context();}

    QPaintEngine *paintEngine() const { return qt_qgl_paint_engine(); }

    void  beginPaint(){
        QGLPaintDevice::beginPaint();
    }
private:
    QEglScreen *m_screen;
    QEglWindow * m_window;
};




QEglScreen::QEglScreen()
    : m_depth(16), m_format(QImage::Format_RGB16), m_platformContext(0)
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglScreen %p\n", this);
#endif
    QGLFormat format = QGLFormat::defaultFormat();
    m_platformContext = new QEglContext(format);


    m_geometry = QRect(QPoint(), m_platformContext->size());

}




QEglPaintDevice::QEglPaintDevice(QEglScreen *screen, QEglWindow *window)
    :QGLPaintDevice(), m_screen(screen), m_window(window)
{
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglPaintDevice" << this << screen << window;
#endif
}



QEglWindow::QEglWindow(QWidget *w, QEglScreen *screen)
    : QPlatformWindow(w), m_screen(screen), m_pd(0), m_context(0)
{
    static int serialNo = 0;
    m_winid  = ++serialNo;
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglWindow %p: %p %p 0x%x\n", this, w, screen, uint(m_winid));
#endif
    m_pd = new QEglPaintDevice(screen, this);
    m_context = new QGLContext( screen->m_platformContext->format(), w);
    m_context->create();
}


void QEglWindow::setGeometry(const QRect &)
{
    // We only support full-screen windows
    QRect rect(m_screen->availableGeometry());
    QWindowSystemInterface::handleGeometryChange(this->widget(), rect);

    QPlatformWindow::setGeometry(rect);
}

WId QEglWindow::winId() const
{
    return m_winid;
}



QPlatformGLContext *QEglWindow::glContext()
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglWindow::glContext %p\n", m_screen->m_platformContext);
#endif
    Q_ASSERT(m_screen);
     return m_screen->m_platformContext;
}

QEglIntegration::QEglIntegration()
{
    m_primaryScreen = new QEglScreen();

    mScreens.append(m_primaryScreen);
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglIntegration\n");
#endif
}

QPixmapData *QEglIntegration::createPixmapData(QPixmapData::PixelType type) const
{
#ifdef QEGL_EXTRA_DEBUG
    fprintf(stderr, "QEglIntegration::createPixmapData %d\n", type);
#endif
    return new QGLPixmapData(type);
}

QPlatformWindow *QEglIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglIntegration::createPlatformWindow" <<  widget;
#endif
    return new QEglWindow(widget, m_primaryScreen);
}


class QEglWindowSurface : public QWindowSurface
{
public:
    QEglWindowSurface(QWidget *window, QEglScreen *screen);
    ~QEglWindowSurface() {}

    QPaintDevice *paintDevice() { return m_window->paintDevice(); }
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size) {}
private:
    QEglScreen *m_screen;
    QEglWindow *m_window;
};

QEglWindowSurface::QEglWindowSurface(QWidget *window, QEglScreen *screen)
    :QWindowSurface(window), m_screen(screen), m_window(0)
{
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglWindowSurface" << window << screen;
#endif
    m_window = static_cast<QEglWindow*>(window->platformWindow());
}


void QEglWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);
#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglWindowSurface::flush";
#endif
    m_screen->m_platformContext->swapBuffers();
}

QWindowSurface *QEglIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);

#ifdef QEGL_EXTRA_DEBUG
    qDebug() << "QEglIntegration::createWindowSurface" << widget;
#endif
    return new QEglWindowSurface(widget, m_primaryScreen);
}
