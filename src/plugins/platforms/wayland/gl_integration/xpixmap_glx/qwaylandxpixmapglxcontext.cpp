#include "qwaylandxpixmapglxcontext.h"

#include "qwaylandshmsurface.h"
#include "qwaylandxpixmapglxwindow.h"

#include <QtCore/QDebug>

QWaylandXPixmapGLXContext::QWaylandXPixmapGLXContext(QWaylandXPixmapGLXIntegration *glxIntegration, QWaylandXPixmapGLXWindow *window)
    : QPlatformGLContext()
    , mGlxIntegration(glxIntegration)
    , mWindow(window)
    , mBuffer(0)
    , mPixmap(0)
    , mConfig(findConfig(glxIntegration->xDisplay(),glxIntegration->screen(),window->widget()->platformWindowFormat()))
    , mGlxPixmap(0)
{
    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(glxIntegration->xDisplay(),mConfig);
    mContext = glXCreateContext(glxIntegration->xDisplay(),visualInfo,0,TRUE);

    geometryChanged();
}

void QWaylandXPixmapGLXContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();

    while(mWindow->waitingForFrameSync()) {
        mGlxIntegration->waylandDisplay()->iterate();
    }

    glXMakeCurrent(mGlxIntegration->xDisplay(),mGlxPixmap,mContext);
}

void QWaylandXPixmapGLXContext::doneCurrent()
{
    QPlatformGLContext::doneCurrent();
}

void QWaylandXPixmapGLXContext::swapBuffers()
{
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

void * QWaylandXPixmapGLXContext::getProcAddress(const QString &procName)
{
    return (void *) glXGetProcAddress(reinterpret_cast<GLubyte *>(procName.toLatin1().data()));
}

QPlatformWindowFormat QWaylandXPixmapGLXContext::platformWindowFormat() const
{
    return platformWindowFromGLXFBConfig(mGlxIntegration->xDisplay(),mConfig,mContext);
}

void QWaylandXPixmapGLXContext::geometryChanged()
{
    while (mWindow->waitingForFrameSync())
        mGlxIntegration->waylandDisplay()->iterate();

    QSize size(mWindow->geometry().size());
    delete mBuffer;
    if (mPixmap)
        XFreePixmap(mGlxIntegration->xDisplay(),mPixmap);
    if (mGlxPixmap)
        glXDestroyPixmap(mGlxIntegration->xDisplay(),mGlxPixmap);

    mBuffer = new QWaylandShmBuffer(mGlxIntegration->waylandDisplay(),size,QImage::Format_ARGB32);
    mWindow->attach(mBuffer);
    int depth = XDefaultDepth(mGlxIntegration->xDisplay(),mGlxIntegration->screen());
    mPixmap = XCreatePixmap(mGlxIntegration->xDisplay(),mGlxIntegration->rootWindow(),size.width(),size.height(),depth);
    XSync(mGlxIntegration->xDisplay(),False);

    mGlxPixmap = glXCreatePixmap(mGlxIntegration->xDisplay(),mConfig,mPixmap,0);

    if (!mGlxPixmap) {
        qDebug() << "Could not make egl surface out of pixmap :(";
    }
}
