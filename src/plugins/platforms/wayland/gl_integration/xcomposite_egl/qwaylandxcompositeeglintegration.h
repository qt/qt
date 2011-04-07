#ifndef QWAYLANDXCOMPOSITEEGLINTEGRATION_H
#define QWAYLANDXCOMPOSITEEGLINTEGRATION_H

#include "gl_integration/qwaylandglintegration.h"
#include "wayland-client.h"

#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

#include <QWaitCondition>

#include <X11/Xlib.h>
#include <EGL/egl.h>

struct wl_xcomposite;

class QWaylandXCompositeEGLIntegration : public QWaylandGLIntegration
{
public:
    QWaylandXCompositeEGLIntegration(QWaylandDisplay * waylandDispaly);
    ~QWaylandXCompositeEGLIntegration();

    void initialize();

    QWaylandWindow *createEglWindow(QWidget *widget);

    QWaylandDisplay *waylandDisplay() const;
    struct wl_xcomposite *waylandXComposite() const;

    Display *xDisplay() const;
    EGLDisplay eglDisplay() const;
    int screen() const;
    Window rootWindow() const;

private:
    QWaylandDisplay *mWaylandDisplay;
    struct wl_xcomposite *mWaylandComposite;

    Display *mDisplay;
    EGLDisplay mEglDisplay;
    int mScreen;
    Window mRootWindow;

    static void wlDisplayHandleGlobal(struct wl_display *display, uint32_t id,
                             const char *interface, uint32_t version, void *data);

    static const struct wl_xcomposite_listener xcomposite_listener;
    static void rootInformation(void *data,
                 struct wl_xcomposite *xcomposite,
                 const char *display_name,
                 uint32_t root_window);
};

#endif // QWAYLANDXCOMPOSITEEGLINTEGRATION_H
