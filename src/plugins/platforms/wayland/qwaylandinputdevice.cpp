#include <QWindowSystemInterface>
#include "qwaylandintegration.h"
#include "qwaylandwindowsurface.h"
#include "qfontconfigdatabase.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

#include <unistd.h>
#include <fcntl.h>

class QWaylandInputDevice {
public:
    QWaylandInputDevice(struct wl_display *display, uint32_t id);

private:
    struct wl_display *mDisplay;
    struct wl_input_device *mInputDevice;
    QWaylandWindow *mPointerFocus;
    QWaylandWindow *mKeyboardFocus;
    static const struct wl_input_device_listener inputDeviceListener;
    Qt::MouseButtons mButtons;
    QPoint mSurfacePos;
    QPoint mGlobalPos;

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

QWaylandInputDevice::QWaylandInputDevice(struct wl_display *display,
					 uint32_t id)
    : mDisplay(display)
    , mInputDevice(wl_input_device_create(display, id))
    , mPointerFocus(NULL)
    , mKeyboardFocus(NULL)
    , mButtons(0)
{
    wl_input_device_add_listener(mInputDevice,
				 &inputDeviceListener,
				 this);
    wl_input_device_set_user_data(mInputDevice, this);
}

void QWaylandInputDevice::inputHandleMotion(void *data,
					    struct wl_input_device *input_device,
					    uint32_t time,
					    int32_t x, int32_t y,
					    int32_t surface_x, int32_t surface_y)
{
    Q_UNUSED(input_device);
    QWaylandInputDevice *inputDevice = (QWaylandInputDevice *) data;
    QWaylandWindow *window = inputDevice->mPointerFocus;

    inputDevice->mSurfacePos = QPoint(surface_x, surface_y);
    inputDevice->mGlobalPos = QPoint(x, y);
    QWindowSystemInterface::handleMouseEvent(window->widget(),
					     time,
					     inputDevice->mSurfacePos,
					     inputDevice->mGlobalPos,
					     inputDevice->mButtons);
}

void QWaylandInputDevice::inputHandleButton(void *data,
					    struct wl_input_device *input_device,
					    uint32_t time, uint32_t button, uint32_t state)
{
    Q_UNUSED(input_device);
    QWaylandInputDevice *inputDevice = (QWaylandInputDevice *) data;
    QWaylandWindow *window = inputDevice->mPointerFocus;
    Qt::MouseButton qt_button;

    switch (button) {
    case 272:
	qt_button = Qt::LeftButton;
	break;
    case 273:
	qt_button = Qt::RightButton;
	break;
    case 274:
	qt_button = Qt::MidButton;
	break;
    default:
	return;
    }

    if (state)
	inputDevice->mButtons |= qt_button;
    else
	inputDevice->mButtons &= ~qt_button;

    QWindowSystemInterface::handleMouseEvent(window->widget(),
					     time,
					     inputDevice->mSurfacePos,
					     inputDevice->mGlobalPos,
					     inputDevice->mButtons);
}

void QWaylandInputDevice::inputHandleKey(void *data,
					 struct wl_input_device *input_device,
					 uint32_t time, uint32_t key, uint32_t state)
{
    Q_UNUSED(input_device);
    QWaylandInputDevice *inputDevice = (QWaylandInputDevice *) data;
    QWaylandWindow *window = inputDevice->mKeyboardFocus;
}

void QWaylandInputDevice::inputHandlePointerFocus(void *data,
						  struct wl_input_device *input_device,
						  uint32_t time, struct wl_surface *surface,
						  int32_t x, int32_t y, int32_t sx, int32_t sy)
{
    Q_UNUSED(input_device);
    QWaylandInputDevice *inputDevice = (QWaylandInputDevice *) data;
    QWaylandWindow *window;

    qWarning("pointer focus %p", surface);

    if (inputDevice->mPointerFocus) {
	window = inputDevice->mPointerFocus;
	QWindowSystemInterface::handleLeaveEvent(window->widget());
	inputDevice->mPointerFocus = NULL;
    }

    if (surface) {
	window = (QWaylandWindow *) wl_surface_get_user_data(surface);
	QWindowSystemInterface::handleEnterEvent(window->widget());
	inputDevice->mPointerFocus = window;
    }
}

void QWaylandInputDevice::inputHandleKeyboardFocus(void *data,
						   struct wl_input_device *input_device,
						   uint32_t time,
						   struct wl_surface *surface,
						   struct wl_array *keys)
{
    Q_UNUSED(input_device);
    QWaylandInputDevice *inputDevice = (QWaylandInputDevice *) data;
    QWaylandWindow *window;

    if (inputDevice->mKeyboardFocus) {
	window = inputDevice->mKeyboardFocus;
	inputDevice->mKeyboardFocus = NULL;
    }

    if (surface) {
	window = (QWaylandWindow *) wl_surface_get_user_data(surface);
	inputDevice->mKeyboardFocus = window;
    }
}

const struct wl_input_device_listener QWaylandInputDevice::inputDeviceListener = {
    QWaylandInputDevice::inputHandleMotion,
    QWaylandInputDevice::inputHandleButton,
    QWaylandInputDevice::inputHandleKey,
    QWaylandInputDevice::inputHandlePointerFocus,
    QWaylandInputDevice::inputHandleKeyboardFocus,
};


QWaylandInputDevice *
waylandInputDeviceCreate(struct wl_display *display, uint32_t id)
{
    return new QWaylandInputDevice(display, id);
}
