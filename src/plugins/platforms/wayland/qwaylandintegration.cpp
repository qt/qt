#include "qfontconfigdatabase.h"

#include <QImageReader>
#include <QWindowSystemInterface>
#include <QPlatformCursor>
#include <QPaintEngine>

#include <QtGui/QPlatformGLContext>
#include <QtGui/QPlatformWindowFormat>

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

#include <private/qwindowsurface_gl_p.h>
#include <private/qpixmapdata_gl_p.h>

#include "qwaylandintegration.h"
#include "qwaylandwindowsurface.h"

#include <unistd.h>
#include <fcntl.h>

extern "C" {
#include <xf86drm.h>
}

class QWaylandCursor : QPlatformCursor {
public:
    QWaylandCursor(QWaylandDisplay *display,
		   QPlatformScreen *screen)
	: QPlatformCursor(screen)
	, mDisplay(display) { }

    void changeCursor(QCursor *cursor, QWidget *widget);
    QWaylandShmBuffer *mBuffer;
    QWaylandDisplay *mDisplay;
};

#define DATADIR "/usr/share"

static const struct pointer_image {
    const char *filename;
    int hotspot_x, hotspot_y;
} pointer_images[] = {
    /* FIXME: Half of these are wrong... */
    /* Qt::ArrowCursor */
    { DATADIR "/wayland/left_ptr.png",			10,  5 },
    /* Qt::UpArrowCursor */
    { DATADIR "/wayland/top_side.png",			18,  8 },
    /* Qt::CrossCursor */
    { DATADIR "/wayland/top_side.png",			18,  8 },
    /* Qt::WaitCursor */
    { DATADIR "/wayland/top_side.png",			18,  8 },
    /* Qt::IBeamCursor */
    { DATADIR "/wayland/xterm.png",			15, 15 },
    /* Qt::SizeVerCursor */
    { DATADIR "/wayland/top_side.png",			18,  8 },
    /* Qt::SizeHorCursor */
    { DATADIR "/wayland/bottom_left_corner.png",	 6, 30 },
    /* Qt::SizeBDiagCursor */
    { DATADIR "/wayland/bottom_right_corner.png",	28, 28 },
    /* Qt::SizeFDiagCursor */
    { DATADIR "/wayland/bottom_side.png",		16, 20 },
    /* Qt::SizeAllCursor */
    { DATADIR "/wayland/left_side.png",			10, 20 },
    /* Qt::BlankCursor */
    { DATADIR "/wayland/right_side.png",		30, 19 },
    /* Qt::SplitVCursor */
    { DATADIR "/wayland/sb_v_double_arrow.png",		15, 15 },
    /* Qt::SplitHCursor */
    { DATADIR "/wayland/sb_h_double_arrow.png",		15, 15 },
    /* Qt::PointingHandCursor */
    { DATADIR "/wayland/hand2.png",			14,  8 },
    /* Qt::ForbiddenCursor */
    { DATADIR "/wayland/top_right_corner.png",		26,  8 },
    /* Qt::WhatsThisCursor */
    { DATADIR "/wayland/top_right_corner.png",		26,  8 },
    /* Qt::BusyCursor */
    { DATADIR "/wayland/top_right_corner.png",		26,  8 },
    /* Qt::OpenHandCursor */
    { DATADIR "/wayland/hand1.png",			18, 11 },
    /* Qt::ClosedHandCursor */
    { DATADIR "/wayland/grabbing.png",			20, 17 },
    /* Qt::DragCopyCursor */
    { DATADIR "/wayland/dnd-copy.png",			13, 13 },
    /* Qt::DragMoveCursor */
    { DATADIR "/wayland/dnd-move.png",			13, 13 },
    /* Qt::DragLinkCursor */
    { DATADIR "/wayland/dnd-link.png",			13, 13 },
};

void QWaylandCursor::changeCursor(QCursor *cursor, QWidget *widget)
{
    const struct pointer_image *p;

    if (widget == NULL)
	return;

    p = NULL;

    switch (cursor->shape()) {
    case Qt::ArrowCursor:
	p = &pointer_images[cursor->shape()];
	break;
    case Qt::UpArrowCursor:
    case Qt::CrossCursor:
    case Qt::WaitCursor:
	break;
    case Qt::IBeamCursor:
	p = &pointer_images[cursor->shape()];
	break;
    case Qt::SizeVerCursor:	/* 5 */
    case Qt::SizeHorCursor:
    case Qt::SizeBDiagCursor:
    case Qt::SizeFDiagCursor:
    case Qt::SizeAllCursor:
    case Qt::BlankCursor:	/* 10 */
	break;
    case Qt::SplitVCursor:
    case Qt::SplitHCursor:
    case Qt::PointingHandCursor:
	p = &pointer_images[cursor->shape()];
	break;
    case Qt::ForbiddenCursor:
    case Qt::WhatsThisCursor:	/* 15 */
    case Qt::BusyCursor:
	break;
    case Qt::OpenHandCursor:
    case Qt::ClosedHandCursor:
    case Qt::DragCopyCursor:
    case Qt::DragMoveCursor: /* 20 */
    case Qt::DragLinkCursor:
	p = &pointer_images[cursor->shape()];
	break;

    default:
    case Qt::BitmapCursor:
	break;
    }

    if (!p) {
	p = &pointer_images[0];
	qWarning("unhandled cursor %d", cursor->shape());
    }

    QImageReader reader(p->filename);

    if (mBuffer == NULL || mBuffer->mImage.size() != reader.size()) {
	if (mBuffer)
	    delete mBuffer;

	mBuffer = new QWaylandShmBuffer(mDisplay, reader.size(),
					QImage::Format_ARGB32);
    }

    reader.read(&mBuffer->mImage);

    mDisplay->setCursor(mBuffer, p->hotspot_x, p->hotspot_y);
}

void QWaylandDisplay::setCursor(QWaylandBuffer *buffer, int32_t x, int32_t y)
{
    /* Qt doesn't tell us which input device we should set the cursor
     * for, so set it for all devices. */
    for (int i = 0; i < mInputDevices.count(); i++) {
	QWaylandInputDevice *inputDevice = mInputDevices.at(i);
	inputDevice->attach(buffer, x, y);
    }
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

struct wl_buffer *QWaylandDisplay::createDrmBuffer(int name,
						   int width, int height,
						   uint32_t stride,
						   struct wl_visual *visual)
{
    return wl_drm_create_buffer(mDrm, name, width, height, stride, visual);
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
    drm_magic_t magic;

    qwd->mDeviceName = strdup(device);

    qwd->mFd = open(qwd->mDeviceName, O_RDWR);
    if (qwd->mFd < 0) {
	qWarning("drm open failed: %m");
	return;
    }

    if (drmGetMagic(qwd->mFd, &magic)) {
	qWarning("DRI2: failed to get drm magic");
	return;
    }

    wl_drm_authenticate(qwd->mDrm, magic);
}

void QWaylandDisplay::drmHandleAuthenticated(void *data, struct wl_drm *drm)
{
    Q_UNUSED(drm);
    QWaylandDisplay *qwd = (QWaylandDisplay *) data;
    EGLint major, minor;
    const char *extensions;

    qwd->mEglDisplay = eglGetDRMDisplayMESA(qwd->mFd);
    if (qwd->mEglDisplay == NULL) {
	qWarning("failed to create display");
	return;
    }

    if (!eglInitialize(qwd->mEglDisplay, &major, &minor)) {
	qWarning("failed to initialize display");
	qwd->mEglDisplay = NULL;
	return;
    }

    extensions = eglQueryString(qwd->mEglDisplay, EGL_EXTENSIONS);
    if (!strstr(extensions, "EGL_KHR_surfaceless_gles2")) {
	qWarning("EGL_KHR_surfaceless_opengles2 not available");
	eglTerminate(qwd->mEglDisplay);
	qwd->mEglDisplay = NULL;
	return;
    }
}

const struct wl_drm_listener QWaylandDisplay::drmListener = {
    QWaylandDisplay::drmHandleDevice,
    QWaylandDisplay::drmHandleAuthenticated
};

void QWaylandDisplay::shellHandleConfigure(void *data, struct wl_shell *shell,
					   uint32_t time, uint32_t edges,
					   struct wl_surface *surface,
					   int32_t x, int32_t y,
					   int32_t width, int32_t height)
{
    Q_UNUSED(data);
    Q_UNUSED(shell);
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QWaylandWindow *ww = (QWaylandWindow *) wl_surface_get_user_data(surface);

    ww->configure(time, edges, x, y, width, height);
}

const struct wl_shell_listener QWaylandDisplay::shellListener = {
    QWaylandDisplay::shellHandleConfigure,
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

    new QWaylandCursor(qwd, screen);

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
    } else if (strcmp(interface, "shell") == 0) {
	qwd->mShell = wl_shell_create(display, id);
	wl_shell_add_listener(qwd->mShell, &shellListener, qwd);
    } else if (strcmp(interface, "output") == 0) {
	struct wl_output *output = wl_output_create(display, id);
	wl_output_add_listener(output, &outputListener, qwd);
    } else if (strcmp(interface, "input_device") == 0) {
	QWaylandInputDevice *inputDevice =
	    new QWaylandInputDevice(display, id);
	qwd->mInputDevices.append(inputDevice);
    }
}

static void roundtripCallback(void *data)
{
    bool *done = (bool *) data;

    *done = true;
}

static void forceRoundtrip(struct wl_display *display)
{
    bool done;

    wl_display_sync_callback(display, roundtripCallback, &done);
    wl_display_iterate(display, WL_DISPLAY_WRITABLE);
    done = false;
    while (!done)
	wl_display_iterate(display, WL_DISPLAY_READABLE);
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
    mDisplay = wl_display_create(socket_name, sizeof socket_name);
    if (mDisplay == NULL) {
	fprintf(stderr, "failed to create display: %m\n");
	return;
    }

    wl_display_add_global_listener(mDisplay,
				   QWaylandDisplay::displayHandleGlobal, this);

    /* Process connection events. */
    wl_display_iterate(mDisplay, WL_DISPLAY_READABLE);
    if (!mShm || !mDeviceName)
	forceRoundtrip(mDisplay);

    /* Force a roundtrip to finish the drm authentication so we
     * initialize EGL before proceeding */
    forceRoundtrip(mDisplay);
    if (mEglDisplay == NULL)
	qDebug("EGL not available");
    else
	qDebug("EGL initialized");

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

QWaylandIntegration::QWaylandIntegration(bool useOpenGL)
    : mFontDb(new QFontconfigDatabase())
    , mDisplay(new QWaylandDisplay())
    , mUseOpenGL(useOpenGL)
{
}

QList<QPlatformScreen *>
QWaylandIntegration::screens() const
{
    return mDisplay->screens();
}

QPixmapData *QWaylandIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    if (mUseOpenGL)
	return new QGLPixmapData(type);
    return new QRasterPixmapData(type);
}

QWaylandWindow::QWaylandWindow(QWidget *window, QWaylandDisplay *display)
    : QPlatformWindow(window)
    , mSurface(0)
    , mDisplay(display)
    , mGLContext(0)
{
    static WId id = 1;

    mWindowId = id++;
}

QWaylandWindow::~QWaylandWindow()
{
    if (mGLContext)
	delete mGLContext;
}

WId QWaylandWindow::winId() const
{
    return mWindowId;
}

void QWaylandWindow::setVisible(bool visible)
{
    if (visible) {
	mSurface = mDisplay->createSurface();
	wl_surface_set_user_data(mSurface, this);
	attach(mBuffer);
    } else {
	wl_surface_destroy(mSurface);
	mSurface = NULL;
    }
}

void QWaylandWindow::attach(QWaylandBuffer *buffer)
{
    QRect geometry = widget()->geometry();

    mBuffer = buffer;
    if (mSurface) {
	wl_surface_attach(mSurface, mBuffer->mBuffer);
	wl_surface_map(mSurface, geometry.x(), geometry.y(),
		       geometry.width(), geometry.height());
    }
}

void QWaylandWindow::configure(uint32_t time, uint32_t edges,
			       int32_t x, int32_t y,
			       int32_t width, int32_t height)
{
    Q_UNUSED(time);
    Q_UNUSED(edges);
    QRect geometry = QRect(x, y, width, height);

    QWindowSystemInterface::handleGeometryChange(widget(), geometry);
}

class QWaylandGLContext : public QPlatformGLContext {
public:
    QWaylandGLContext(QWaylandDisplay *wd, const QPlatformWindowFormat &format);
    ~QWaylandGLContext();
    void makeCurrent();
    void doneCurrent();
    void swapBuffers();
    void* getProcAddress(const QString&);
    QPlatformWindowFormat platformWindowFormat() const { return mFormat; }

private:
    EGLContext mContext;
    QPlatformWindowFormat mFormat;
    QWaylandDisplay *mDisplay;
};

QWaylandGLContext::QWaylandGLContext(QWaylandDisplay *wd, const QPlatformWindowFormat &format)
    : QPlatformGLContext()
    , mContext(0)
    , mFormat(format)
    , mDisplay(wd)
{
    EGLDisplay eglDisplay;
    static const EGLint contextAttribs[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
    };

    eglBindAPI(EGL_OPENGL_ES_API);
    eglDisplay = mDisplay->eglDisplay();
    mContext = eglCreateContext(eglDisplay, NULL,
				EGL_NO_CONTEXT, contextAttribs);
    eglMakeCurrent(eglDisplay, NULL, NULL, mContext);
}

QWaylandGLContext::~QWaylandGLContext()
{
    if (mContext)
        eglDestroyContext(mDisplay->eglDisplay(), mContext);
}

void QWaylandGLContext::makeCurrent()
{
    eglMakeCurrent(mDisplay->eglDisplay(), 0, 0, mContext);
}

void QWaylandGLContext::doneCurrent()
{
    eglMakeCurrent(mDisplay->eglDisplay(), 0, 0, mContext);
}

void QWaylandGLContext::swapBuffers()
{
}

void *QWaylandGLContext::getProcAddress(const QString &string)
{
    return (void *) eglGetProcAddress(string.toLatin1().data());
}

QPlatformGLContext *QWaylandWindow::glContext() const
{
    if (!mGLContext) {
        QWaylandWindow *that = const_cast<QWaylandWindow *>(this);
        that->mGLContext = new QWaylandGLContext(mDisplay, widget()->platformWindowFormat());
    }

    return mGLContext;
}

QPlatformWindow *QWaylandIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QWaylandWindow(widget, mDisplay);
}

QWindowSurface *QWaylandIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    Q_UNUSED(winId);

    if (mUseOpenGL)
	return new QWaylandDrmWindowSurface(widget, mDisplay);
    return new QWaylandShmWindowSurface(widget, mDisplay);
}

QPlatformFontDatabase *QWaylandIntegration::fontDatabase() const
{
    return mFontDb;
}
