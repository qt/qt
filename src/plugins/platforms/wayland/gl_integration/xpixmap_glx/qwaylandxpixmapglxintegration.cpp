#include "qwaylandxpixmapglxintegration.h"

#include "qwaylandxpixmapglxwindow.h"

#include <QtCore/QDebug>

QWaylandXPixmapGLXIntegration::QWaylandXPixmapGLXIntegration(QWaylandDisplay * waylandDispaly)
    : QWaylandGLIntegration()
    , mWaylandDisplay(waylandDispaly)
{
    qDebug() << "Using Wayland XPixmap-GLX";
    char *display_name = getenv("DISPLAY");
    mDisplay = XOpenDisplay(display_name);
    mScreen = XDefaultScreen(mDisplay);
    mRootWindow = XDefaultRootWindow(mDisplay);
    XSync(mDisplay, False);
}

QWaylandXPixmapGLXIntegration::~QWaylandXPixmapGLXIntegration()
{
    XCloseDisplay(mDisplay);
}

void QWaylandXPixmapGLXIntegration::initialize()
{
}

QWaylandWindow * QWaylandXPixmapGLXIntegration::createEglWindow(QWidget *widget)
{
    return new QWaylandXPixmapGLXWindow(widget,this);
}

QWaylandGLIntegration * QWaylandGLIntegration::createGLIntegration(QWaylandDisplay *waylandDisplay)
{
    return new QWaylandXPixmapGLXIntegration(waylandDisplay);
}

Display * QWaylandXPixmapGLXIntegration::xDisplay() const
{
    return mDisplay;
}

int QWaylandXPixmapGLXIntegration::screen() const
{
    return mScreen;
}

Window QWaylandXPixmapGLXIntegration::rootWindow() const
{
    return mRootWindow;
}

QWaylandDisplay * QWaylandXPixmapGLXIntegration::waylandDisplay() const
{
    return mWaylandDisplay;
}
