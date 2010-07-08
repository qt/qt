#include "eglnullwswindowsurface.h"
#include "eglnullwsscreenplugin.h"

#include <QGLWidget>

static const QWSWindowSurface::SurfaceFlags Flags
    = QWSWindowSurface::RegionReserved | QWSWindowSurface::RegionReserved;

EGLNullWSWindowSurface::EGLNullWSWindowSurface(QWidget *w)
    :
    QWSGLWindowSurface(w),
    widget(w)
{
    setSurfaceFlags(Flags);
}

EGLNullWSWindowSurface::EGLNullWSWindowSurface()
    : widget(0)
{
    setSurfaceFlags(Flags);
}

EGLNullWSWindowSurface::~EGLNullWSWindowSurface() {}

QString EGLNullWSWindowSurface::key() const
{
    return QLatin1String(PluginName);
}

QPaintDevice *EGLNullWSWindowSurface::paintDevice()
{
    return widget;
}

bool EGLNullWSWindowSurface::isValid() const
{
    return qobject_cast<QGLWidget *>(window());
}

QImage EGLNullWSWindowSurface::image() const
{
    return QImage();
}
