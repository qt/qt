#include "qwaylandxpixmapeglcontext.h"

#include "../../../eglconvenience/qeglconvenience.h"

#include <QtOpenGL/QGLContext>

#include "qwaylandshmsurface.h"

#include <QtCore/QDebug>

QXPixmapReadbackGLContext::QXPixmapReadbackGLContext(QWaylandXPixmapEglIntegration *eglIntegration, QWaylandXPixmapWindow *window)
    : mEglIntegration(eglIntegration)
    , mWindow(window)
    , mBuffer(0)
    , mPixmap(0)
    , mConfig(q_configFromQPlatformWindowFormat(eglIntegration->eglDisplay(),window->widget()->platformWindowFormat(),true,EGL_PIXMAP_BIT))
    , mPixmapSurface(EGL_NO_SURFACE)
{
    QVector<EGLint> eglContextAttrs;
    eglContextAttrs.append(EGL_CONTEXT_CLIENT_VERSION);
    eglContextAttrs.append(2);
    eglContextAttrs.append(EGL_NONE);

    mContext = eglCreateContext(eglIntegration->eglDisplay(),mConfig,0,eglContextAttrs.constData());

    geometryChanged();
}

QXPixmapReadbackGLContext::~QXPixmapReadbackGLContext()
{
    eglDestroyContext(mEglIntegration->eglDisplay(),mContext);
}

void QXPixmapReadbackGLContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();

    while(mWindow->waitingForFrameSync()) {
        mEglIntegration->waylandDisplay()->iterate();
    }

    eglMakeCurrent(mEglIntegration->eglDisplay(),mPixmapSurface,mPixmapSurface,mContext);
}

void QXPixmapReadbackGLContext::doneCurrent()
{
    QPlatformGLContext::doneCurrent();
    eglMakeCurrent(mEglIntegration->eglDisplay(),EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
}

void QXPixmapReadbackGLContext::swapBuffers()
{
    eglSwapBuffers(mEglIntegration->eglDisplay(),mPixmapSurface);

    if (QPlatformGLContext::currentContext() != this) {
        makeCurrent();
    }

    QSize size = mWindow->geometry().size();

    QImage img(size,QImage::Format_ARGB32);
    const uchar *constBits = img.bits();
    void *pixels = const_cast<uchar *>(constBits);

    glReadPixels(0,0, size.width(), size.height(), GL_RGBA,GL_UNSIGNED_BYTE, pixels);

    img = img.mirrored();
    constBits = img.bits();

    const uchar *constDstBits = mBuffer->image()->bits();
    uchar *dstBits = const_cast<uchar *>(constDstBits);
    memcpy(dstBits,constBits,(img.width()*4) * img.height());


    mWindow->damage(QRegion(QRect(QPoint(0,0),size)));
}

void * QXPixmapReadbackGLContext::getProcAddress(const QString &procName)
{
    return (void *) eglGetProcAddress(procName.toLatin1().data());
}

QPlatformWindowFormat QXPixmapReadbackGLContext::platformWindowFormat() const
{
    return qt_qPlatformWindowFormatFromConfig(mEglIntegration->eglDisplay(),mConfig);
}

void QXPixmapReadbackGLContext::geometryChanged()
{
    while (mWindow->waitingForFrameSync())
        mEglIntegration->waylandDisplay()->iterate();

    QSize size(mWindow->geometry().size());
    delete mBuffer;
    if (mPixmap)
        XFreePixmap(mEglIntegration->xDisplay(),mPixmap);
    if (mPixmapSurface != EGL_NO_SURFACE)
        eglDestroySurface(mEglIntegration->eglDisplay(),mPixmapSurface);

    mBuffer = new QWaylandShmBuffer(mEglIntegration->waylandDisplay(),size,QImage::Format_ARGB32);
    mWindow->attach(mBuffer);
    mPixmap = XCreatePixmap(mEglIntegration->xDisplay(),mEglIntegration->rootWindow(),size.width(),size.height(),mEglIntegration->depth());
    XSync(mEglIntegration->xDisplay(),False);

    mPixmapSurface = eglCreatePixmapSurface(mEglIntegration->eglDisplay(),mConfig,mPixmap,0);
    if (mPixmapSurface == EGL_NO_SURFACE) {
        qDebug() << "Could not make egl surface out of pixmap :(";
    }
}
