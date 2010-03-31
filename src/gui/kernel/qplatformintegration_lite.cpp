#include "qplatformintegration_lite.h"

QT_BEGIN_NAMESPACE

QBlittable *QPlatformIntegration::createBlittable(const QSize &) const
{ return 0; }

QPixmap QPlatformIntegration::grabWindow(WId window, int x, int y, int width, int height) const
{
    Q_UNUSED(window);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
    return QPixmap();
}

QT_END_NAMESPACE
