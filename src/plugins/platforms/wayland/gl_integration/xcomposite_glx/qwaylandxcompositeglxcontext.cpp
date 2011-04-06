#include "qwaylandxcompositeglxcontext.h"

#include "qwaylandxcompositeglxwindow.h"
#include "qwaylandxcompositebuffer.h"

#include "wayland-xcomposite-client-protocol.h"
#include <QtCore/QDebug>

#include <X11/extensions/Xcomposite.h>

QWaylandXCompositeGLXContext::QWaylandXCompositeGLXContext(QWaylandXCompositeGLXIntegration *glxIntegration, QWaylandXCompositeGLXWindow *window)
    : QPlatformGLContext()
    , mGlxIntegration(glxIntegration)
    , mWindow(window)
    , mBuffer(0)
    , mXWindow(0)
    , mConfig(qglx_findConfig(glxIntegration->xDisplay(),glxIntegration->screen(),window->widget()->platformWindowFormat()))
{
    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(glxIntegration->xDisplay(),mConfig);
    mContext = glXCreateContext(glxIntegration->xDisplay(),visualInfo,0,TRUE);

    geometryChanged();
}

void QWaylandXCompositeGLXContext::makeCurrent()
{
    QPlatformGLContext::makeCurrent();
    glXMakeCurrent(mGlxIntegration->xDisplay(),mXWindow,mContext);
}

void QWaylandXCompositeGLXContext::doneCurrent()
{
    glXMakeCurrent(mGlxIntegration->xDisplay(),0,0);
    QPlatformGLContext::doneCurrent();
}

void QWaylandXCompositeGLXContext::swapBuffers()
{
    QSize size = mWindow->geometry().size();

    glXSwapBuffers(mGlxIntegration->xDisplay(),mXWindow);
    mWindow->damage(QRegion(QRect(QPoint(0,0),size)));
    mWindow->waitForFrameSync();
}

void * QWaylandXCompositeGLXContext::getProcAddress(const QString &procName)
{
    return (void *) glXGetProcAddress(reinterpret_cast<GLubyte *>(procName.toLatin1().data()));
}

QPlatformWindowFormat QWaylandXCompositeGLXContext::platformWindowFormat() const
{
    return qglx_platformWindowFromGLXFBConfig(mGlxIntegration->xDisplay(),mConfig,mContext);
}

void QWaylandXCompositeGLXContext::sync_function(void *data)
{
    QWaylandXCompositeGLXContext *that = static_cast<QWaylandXCompositeGLXContext *>(data);
    that->mWaitCondition.wakeAll();
}

void QWaylandXCompositeGLXContext::geometryChanged()
{
    QSize size(mWindow->geometry().size());
    if (size.isEmpty()) {
        //QGLWidget wants a context for a window without geometry
        size = QSize(1,1);
    }

    delete mBuffer;
    //XFreePixmap deletes the glxPixmap as well
    if (mXWindow) {
        XDestroyWindow(mGlxIntegration->xDisplay(),mXWindow);
    }

    XVisualInfo *visualInfo = glXGetVisualFromFBConfig(mGlxIntegration->xDisplay(),mConfig);
    Colormap cmap = XCreateColormap(mGlxIntegration->xDisplay(),mGlxIntegration->rootWindow(),visualInfo->visual,AllocNone);

    XSetWindowAttributes a;
    a.colormap = cmap;
    mXWindow = XCreateWindow(mGlxIntegration->xDisplay(), mGlxIntegration->rootWindow(),0, 0, size.width(), size.height(),
                             0, visualInfo->depth, InputOutput, visualInfo->visual,
                             CWColormap, &a);

    XCompositeRedirectWindow(mGlxIntegration->xDisplay(), mXWindow, CompositeRedirectManual);
    XMapWindow(mGlxIntegration->xDisplay(), mXWindow);

    XSync(mGlxIntegration->xDisplay(),False);
    mBuffer = new QWaylandXCompositeBuffer(mGlxIntegration->waylandXComposite(),
                                           (uint32_t)mXWindow,
                                           size,
                                           mGlxIntegration->waylandDisplay()->argbVisual());
    mWindow->attach(mBuffer);
    wl_display_sync_callback(mGlxIntegration->waylandDisplay()->wl_display(),
                             QWaylandXCompositeGLXContext::sync_function,
                             this);
    QMutex lock;
    lock.lock();
    wl_display_sync(mGlxIntegration->waylandDisplay()->wl_display(),0);
    mWaitCondition.wait(&lock);
}
