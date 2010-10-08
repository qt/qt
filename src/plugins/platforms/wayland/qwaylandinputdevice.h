#ifndef QWAYLANDINPUTDEVICE_H
#define QWAYLANDINPUTDEVICE_H

#include <QSocketNotifier>
#include <QObject>
#include <QtGui/QPlatformIntegration>
#include <QtGui/QPlatformScreen>

#include <wayland-client.h>
#include "qwaylandwindowsurface.h"

QT_BEGIN_NAMESPACE

class QWaylandWindow;

class QWaylandInputDevice {
public:
    QWaylandInputDevice(struct wl_display *display, uint32_t id);
    void attach(QWaylandBuffer *buffer, int x, int y);

private:
    struct wl_display *mDisplay;
    struct wl_input_device *mInputDevice;
    QWaylandWindow *mPointerFocus;
    QWaylandWindow *mKeyboardFocus;
    static const struct wl_input_device_listener inputDeviceListener;
    Qt::MouseButtons mButtons;
    QPoint mSurfacePos;
    QPoint mGlobalPos;
    struct xkb_desc *mXkb;
    Qt::KeyboardModifiers mModifiers;
    uint32_t mTime;

    static void inputHandleMotion(void *data,
				  struct wl_input_device *input_device,
				  uint32_t time,
				  int32_t x, int32_t y,
				  int32_t sx, int32_t sy);
    static void inputHandleButton(void *data,
				  struct wl_input_device *input_device,
				  uint32_t time, uint32_t button, uint32_t state);
    static void inputHandleKey(void *data,
			       struct wl_input_device *input_device,
			       uint32_t time, uint32_t key, uint32_t state);
    static void inputHandlePointerFocus(void *data,
					struct wl_input_device *input_device,
					uint32_t time, struct wl_surface *surface,
					int32_t x, int32_t y, int32_t sx, int32_t sy);
    static void inputHandleKeyboardFocus(void *data,
					 struct wl_input_device *input_device,
					 uint32_t time,
					 struct wl_surface *surface,
					 struct wl_array *keys);
};

QT_END_NAMESPACE

#endif
