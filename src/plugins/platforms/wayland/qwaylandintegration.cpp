#include <QWindowSystemInterface>
#include "qwaylandintegration.h"
#include "qwaylandwindowsurface.h"
#include "qfontconfigdatabase.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

#include <unistd.h>
#include <fcntl.h>

extern "C" {
#include <xf86drm.h>
}

struct wl_surface *QWaylandDisplay::createSurface()
{
    return wl_compositor_create_surface(mCompositor);
}

struct wl_buffer *QWaylandDisplay::createShmBuffer(int fd,
						   int width, int height,
						   uint32_t stride,
						   struct wl_visual *visual)
{
    return wl_shm_create_buffer(mShm, fd, width, height, stride, visual);
}

struct wl_visual *QWaylandDisplay::argbVisual()
{
    return wl_display_get_argb_visual(mDisplay);
}

void QWaylandDisplay::drmHandleDevice(void *data,
				      struct wl_drm *drm, const char *device)
{
    Q_UNUSED(drm);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    qwd->mDeviceName = strdup(device);
}

void QWaylandDisplay::drmHandleAuthenticated(void *data, struct wl_drm *drm)
{
    Q_UNUSED(drm);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    qwd->mAuthenticated = true;
}

const struct wl_drm_listener QWaylandDisplay::drmListener = {
    QWaylandDisplay::drmHandleDevice,
    QWaylandDisplay::drmHandleAuthenticated
};

void QWaylandDisplay::outputHandleGeometry(void *data,
					   struct wl_output *output,
					   int32_t width, int32_t height)
{
    Q_UNUSED(output);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;
    QWaylandScreen *screen;

    screen = new QWaylandScreen();
    screen->mGeometry = QRect(0, 0, width, height);
    screen->mDepth = 32;
    screen->mFormat = QImage::Format_ARGB32_Premultiplied;
    screen->mOutput = output;

    qwd->mScreens.append(screen);
}

const struct wl_output_listener QWaylandDisplay::outputListener = {
    QWaylandDisplay::outputHandleGeometry
};

void QWaylandDisplay::displayHandleGlobal(struct wl_display *display,
					  uint32_t id,
					  const char *interface,
					  uint32_t version, void *data)
{
    Q_UNUSED(version);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    if (strcmp(interface, "compositor") == 0) {
	qwd->mCompositor = wl_compositor_create(display, id);
    } else if (strcmp(interface, "drm") == 0) {
	qwd->mDrm = wl_drm_create(display, id);
	wl_drm_add_listener(qwd->mDrm, &drmListener, qwd);
    } else if (strcmp(interface, "shm") == 0) {
	qwd->mShm = wl_shm_create(display, id);
    } else if (strcmp(interface, "output") == 0) {
	struct wl_output *output = wl_output_create(display, id);
	wl_output_add_listener(output, &outputListener, qwd);
    } else if (strcmp(interface, "input_device") == 0) {
	QWaylandInputDevice *inputDevice =
	    waylandInputDeviceCreate(display, id);
	qwd->mInputDevices.append(inputDevice);
    }
}

static void initial_roundtrip(void *data)
{
    bool *done = (bool *) data;

    *done = true;
}

static const char socket_name[] = "\0wayland";

void QWaylandDisplay::eventDispatcher(void)
{
    wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);
}

int
QWaylandDisplay::sourceUpdate(uint32_t mask, void *data)
{
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;

    /* FIXME: We get a callback here when we ask wl_display for the
     * fd, but at that point we don't have the socket notifier as we
     * need the fd to create that.  We'll probably need to split that
     * API into get_fd and set_update_func functions. */
    if (qwd->mWriteNotifier == NULL)
	return 0;

    qwd->mWriteNotifier->setEnabled(mask & WL_DISPLAY_WRITABLE);

    return 0;
}

void QWaylandDisplay::flushRequests(void)
{
    wl_display_iterate(mDisplay, WL_DISPLAY_WRITABLE);
}

QWaylandDisplay::QWaylandDisplay(void)
    : mWriteNotifier(0)
{
    drm_magic_t magic;
    bool done;

    mDisplay = wl_display_create(socket_name, sizeof socket_name);
    if (mDisplay == NULL) {
	fprintf(stderr, "failed to create display: %m\n");
	return;
    }

    wl_display_add_global_listener(mDisplay,
				   QWaylandDisplay::displayHandleGlobal, this);

    /* Process connection events. */
    wl_display_sync_callback(mDisplay, initial_roundtrip, &done);
    wl_display_iterate(mDisplay, WL_DISPLAY_WRITABLE);
    done = false;
    while (!done)
	wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);

    mFd = open(mDeviceName, O_RDWR);
    if (mFd < 0) {
	qWarning("drm open failed: %m\n");
	return;
    }

    if (drmGetMagic(mFd, &magic)) {
	qWarning("DRI2: failed to get drm magic");
	return;
    }

    /* Wait for authenticated event */
    wl_drm_authenticate(mDrm, magic);
    wl_display_iterate(mDisplay, WL_DISPLAY_WRITABLE);
    while (!mAuthenticated)
	wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);

    int fd = wl_display_get_fd(mDisplay, sourceUpdate, this);
    mReadNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(mReadNotifier,
	    SIGNAL(activated(int)), this, SLOT(eventDispatcher()));

    mWriteNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    connect(mWriteNotifier,
	    SIGNAL(activated(int)), this, SLOT(flushRequests()));
    mWriteNotifier->setEnabled(false);
}

QWaylandDisplay::~QWaylandDisplay(void)
{
    close(mFd);
    wl_display_destroy(mDisplay);
}

QWaylandIntegration::QWaylandIntegration()
    : mFontDb(new QFontconfigDatabase())
    , mDisplay(new QWaylandDisplay())
{
}

QList<QPlatformScreen *>
QWaylandIntegration::screens() const
{
    return mDisplay->screens();
}

QPixmapData *QWaylandIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QWaylandWindow::QWaylandWindow(QWidget *window, QWaylandDisplay *display)
    : QPlatformWindow(window)
    , mSurface(0)
    , mDisplay(display)
{
    static WId id = 1;

    mWindowId = id++;
}

QWaylandWindow::~QWaylandWindow()
{
}

WId QWaylandWindow::winId() const
{
    return mWindowId;
}

void QWaylandWindow::setVisible(bool visible)
{
    QWaylandWindowSurface *wws =
	(QWaylandWindowSurface *) widget()->windowSurface();

    if (visible) {
	mSurface = mDisplay->createSurface();
	wl_surface_set_user_data(mSurface, this);
	wws->attach();
    } else {
	wl_surface_destroy(mSurface);
	mSurface = NULL;
    }
}

QPlatformWindow *QWaylandIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QWaylandWindow(widget, mDisplay);
}

QWindowSurface *QWaylandIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QWaylandWindowSurface(widget, mDisplay);
}

QPlatformFontDatabase *QWaylandIntegration::fontDatabase() const
{
    return mFontDb;
}
