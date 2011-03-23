#include "qwaylandxpixmapeglintegration.h"

#include <QDebug>

#include "qwaylandxpixmapwindow.h"

QWaylandXPixmapEglIntegration::QWaylandXPixmapEglIntegration(QWaylandDisplay *display)
    : QWaylandGLIntegration()
    , mWaylandDisplay(display)
{
    char *display_name = getenv("DISPLAY");
    mDisplay = XOpenDisplay(display_name);
    mScreen = XDefaultScreen(mDisplay);
    mRootWindow = XDefaultRootWindow(mDisplay);
    XSync(mDisplay, False);
}

QWaylandXPixmapEglIntegration::~QWaylandXPixmapEglIntegration()
{
    XCloseDisplay(mDisplay);
}


QWaylandGLIntegration *QWaylandGLIntegration::createEglIntegration(QWaylandDisplay *waylandDisplay)
{
    return new QWaylandXPixmapEglIntegration(waylandDisplay);
}

void QWaylandXPixmapEglIntegration::initialize()
{
    eglBindAPI(EGL_OPENGL_ES_API);
    mEglDisplay = eglGetDisplay(mDisplay);
    EGLint major, minor;
    EGLBoolean initialized = eglInitialize(mEglDisplay,&major,&minor);
    if (initialized) {
        qDebug() << "EGL initialized successfully" << major << "," << minor;
    } else {
        qDebug() << "EGL could not initialized. All EGL and GL operations will fail";
    }
}

QWaylandWindow * QWaylandXPixmapEglIntegration::createEglWindow(QWidget *widget)
{
    return new QWaylandXPixmapWindow(widget,this);
}

EGLDisplay QWaylandXPixmapEglIntegration::eglDisplay()
{
    return mEglDisplay;
}

Window QWaylandXPixmapEglIntegration::rootWindow() const
{
    return mRootWindow;
}

int QWaylandXPixmapEglIntegration::depth() const
{
    return XDefaultDepth(mDisplay,mScreen);
}

Display * QWaylandXPixmapEglIntegration::xDisplay() const
{
    return mDisplay;
}

QWaylandDisplay * QWaylandXPixmapEglIntegration::waylandDisplay() const
{
    return mWaylandDisplay;
}
