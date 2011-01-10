#include "qwaylandintegration.h"

#include "qwaylanddisplay.h"
#include "qwaylandwindowsurface.h"
#include "qwaylandwindow.h"

#include "qfontconfigdatabase.h"

#include <QtGui/QWindowSystemInterface>
#include <QtGui/QPlatformCursor>
#include <QtGui/QPlatformWindowFormat>

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>

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
