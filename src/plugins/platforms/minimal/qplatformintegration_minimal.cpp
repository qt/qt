#include "qplatformintegration_minimal.h"
#include "qwindowsurface_minimal.h"

#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>

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
void QMinimalIntegration::createWindowAndSurface(QPlatformWindow**window, QWindowSurface**surface, QWidget *widget, WId winId) const
{
    qDebug() << "createWindow";
    *surface = new QMinimalWindowSurface(widget);
    *window = new QPlatformWindow(widget);
}
