#include "qplatformintegration_minimal.h"
#include "qwindowsurface_minimal.h"

#include <QtGui/private/qpixmap_raster_p.h>

QMinimalIntegration::QMinimalIntegration()
{
    QMinimalScreen *mPrimaryScreen = new QMinimalScreen();

    mPrimaryScreen->mGeometry = QRect(0, 0, 240, 320);
    mPrimaryScreen->mDepth = 16;
    mPrimaryScreen->mFormat = QImage::Format_RGB16;
    mPrimaryScreen->mPhysicalSize = QSize(40, 54);

    mScreens.append(mPrimaryScreen);
}

QPixmapData *QMinimalIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    return new QRasterPixmapData(type);
}
QWindowSurface *QMinimalIntegration::createWindowSurface(QWidget *widget) const
{
    return new QMinimalWindowSurface(widget);
}
