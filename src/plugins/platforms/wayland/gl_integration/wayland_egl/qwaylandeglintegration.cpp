#include "qwaylandeglintegration.h"

#include "gl_integration/qwaylandglintegration.h"

#include "qwaylandeglwindow.h"

#include <QtCore/QDebug>

QWaylandEglIntegration::QWaylandEglIntegration(struct wl_display *waylandDisplay)
    : mWaylandDisplay(waylandDisplay)
    , mNativeEglDisplay(wl_egl_display_create(mWaylandDisplay))
{
    qDebug() << "Using Wayland-EGL";
}


QWaylandEglIntegration::~QWaylandEglIntegration()
{
    eglTerminate(mEglDisplay);
}

void QWaylandEglIntegration::initialize()
{
    EGLint major,minor;
    mEglDisplay = eglGetDisplay((EGLNativeDisplayType)mNativeEglDisplay);
    if (mEglDisplay == NULL) {
        qWarning("EGL not available");
    } else {
        if (!eglInitialize(mEglDisplay, &major, &minor)) {
            qWarning("failed to initialize EGL display");
            return;
        }
    }
}

QWaylandWindow *QWaylandEglIntegration::createEglWindow(QWidget *window)
{
    return new QWaylandEglWindow(window);
}

EGLDisplay QWaylandEglIntegration::eglDisplay() const
{
    return mEglDisplay;
}

wl_egl_display * QWaylandEglIntegration::nativeDisplay() const
{
    return mNativeEglDisplay;
}

QWaylandGLIntegration *QWaylandGLIntegration::createGLIntegration(QWaylandDisplay *waylandDisplay)
{
    return new QWaylandEglIntegration(waylandDisplay->wl_display());
}
