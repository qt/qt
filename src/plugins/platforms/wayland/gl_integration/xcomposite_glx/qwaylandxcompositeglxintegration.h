#ifndef QWAYLANDXCOMPOSITEGLXINTEGRATION_H
#define QWAYLANDXCOMPOSITEGLXINTEGRATION_H

#include "gl_integration/qwaylandglintegration.h"
#include "wayland-client.h"

#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>
#include <QtGui/QWidget>

#include <X11/Xlib.h>

struct wl_xcomposite;

class QWaylandXCompositeGLXIntegration : public QWaylandGLIntegration
{
public:
    QWaylandXCompositeGLXIntegration(QWaylandDisplay * waylandDispaly);
    ~QWaylandXCompositeGLXIntegration();

    void initialize();

    QWaylandWindow *createEglWindow(QWidget *widget);

    QWaylandDisplay *waylandDisplay() const;
    struct wl_xcomposite *waylandXComposite() const;

    Display *xDisplay() const;
    int screen() const;
    Window rootWindow() const;

private:
    QWaylandDisplay *mWaylandDisplay;
    struct wl_xcomposite *mWaylandComposite;

    Display *mDisplay;
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

#endif // QWAYLANDXCOMPOSITEGLXINTEGRATION_H
