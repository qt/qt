#ifndef QWAYLANDEGLINTEGRATION_H
#define QWAYLANDEGLINTEGRATION_H

#include "gl_integration/qwaylandglintegration.h"

#include "qwaylandeglinclude.h"

class QWaylandWindow;
class QWidget;

class QWaylandEglIntegration : public QWaylandGLIntegration
{
public:
    QWaylandEglIntegration(struct wl_display *waylandDisplay);
    ~QWaylandEglIntegration();

    void initialize();

    QWaylandWindow *createEglWindow(QWidget *window);

    EGLDisplay eglDisplay() const;
    struct wl_egl_display *nativeDisplay() const;
private:
    struct wl_display *mWaylandDisplay;

    EGLDisplay mEglDisplay;
    struct wl_egl_display *mNativeEglDisplay;


};

#endif // QWAYLANDEGLINTEGRATION_H
