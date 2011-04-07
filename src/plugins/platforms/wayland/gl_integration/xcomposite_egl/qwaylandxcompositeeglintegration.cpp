#include "qwaylandxcompositeeglintegration.h"

#include "qwaylandxcompositeeglwindow.h"

#include <QtCore/QDebug>

#include "wayland-xcomposite-client-protocol.h"

QWaylandGLIntegration * QWaylandGLIntegration::createGLIntegration(QWaylandDisplay *waylandDisplay)
{
    return new QWaylandXCompositeEGLIntegration(waylandDisplay);
}

QWaylandXCompositeEGLIntegration::QWaylandXCompositeEGLIntegration(QWaylandDisplay * waylandDispaly)
    : QWaylandGLIntegration()
    , mWaylandDisplay(waylandDispaly)
{
    wl_display_add_global_listener(mWaylandDisplay->wl_display(), QWaylandXCompositeEGLIntegration::wlDisplayHandleGlobal,
                                   this);
}

QWaylandXCompositeEGLIntegration::~QWaylandXCompositeEGLIntegration()
{
    XCloseDisplay(mDisplay);
}

void QWaylandXCompositeEGLIntegration::initialize()
{
}

QWaylandWindow * QWaylandXCompositeEGLIntegration::createEglWindow(QWidget *widget)
{
    return new QWaylandXCompositeEGLWindow(widget,this);
}

Display * QWaylandXCompositeEGLIntegration::xDisplay() const
{
    return mDisplay;
}

EGLDisplay QWaylandXCompositeEGLIntegration::eglDisplay() const
{
    return mEglDisplay;
}

int QWaylandXCompositeEGLIntegration::screen() const
{
    return mScreen;
}

Window QWaylandXCompositeEGLIntegration::rootWindow() const
{
    return mRootWindow;
}

QWaylandDisplay * QWaylandXCompositeEGLIntegration::waylandDisplay() const
{
    return mWaylandDisplay;
}
wl_xcomposite * QWaylandXCompositeEGLIntegration::waylandXComposite() const
{
    return mWaylandComposite;
}

const struct wl_xcomposite_listener QWaylandXCompositeEGLIntegration::xcomposite_listener = {
    QWaylandXCompositeEGLIntegration::rootInformation
};

void QWaylandXCompositeEGLIntegration::wlDisplayHandleGlobal(wl_display *display, uint32_t id, const char *interface, uint32_t version, void *data)
{
    Q_UNUSED(version);
    if (strcmp(interface, "xcomposite") == 0) {
        QWaylandXCompositeEGLIntegration *integration = static_cast<QWaylandXCompositeEGLIntegration *>(data);
        integration->mWaylandComposite = wl_xcomposite_create(display,id);
        wl_xcomposite_add_listener(integration->mWaylandComposite,&xcomposite_listener,integration);
    }

}

void QWaylandXCompositeEGLIntegration::rootInformation(void *data, wl_xcomposite *xcomposite, const char *display_name, uint32_t root_window)
{
    Q_UNUSED(xcomposite);
    QWaylandXCompositeEGLIntegration *integration = static_cast<QWaylandXCompositeEGLIntegration *>(data);

    integration->mDisplay = XOpenDisplay(display_name);
    integration->mRootWindow = (Window) root_window;
    integration->mScreen = XDefaultScreen(integration->mDisplay);
    integration->mEglDisplay = eglGetDisplay(integration->mDisplay);
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint minor,major;
    if (!eglInitialize(integration->mEglDisplay,&major,&minor)) {
        qFatal("Failed to initialize EGL");
    }
    eglSwapInterval(integration->eglDisplay(),0);
    qDebug() << "ROOT INFORMATION" << integration->mDisplay << integration->mRootWindow << integration->mScreen;
}

