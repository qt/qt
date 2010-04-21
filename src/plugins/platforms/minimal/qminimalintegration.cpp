#include "qminimalintegration.h"
#include "qminimalwindowsurface.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

QMinimalIntegration::QMinimalIntegration()
{
    QMinimalScreen *mPrimaryScreen = new QMinimalScreen();

    mPrimaryScreen->mGeometry = QRect(0, 0, 240, 320);
    mPrimaryScreen->mDepth = 16;
    mPrimaryScreen->mFormat = QImage::Format_RGB16;

    mScreens.append(mPrimaryScreen);
}

QPixmapData *QMinimalIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}

QPlatformWindow *QMinimalIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QPlatformWindow(widget);
}

QWindowSurface *QMinimalIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    return new QMinimalWindowSurface(widget);
}
