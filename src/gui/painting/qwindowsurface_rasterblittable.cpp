#include "qwindowsurface_rasterblittable_p.h"

#include <private/qpaintengine_blitter_p.h>
#include <private/qpixmap_blitter_p.h>

QRasterBlittableWindowSurface::QRasterBlittableWindowSurface(QWidget *widget)
    : QRasterWindowSurface(widget),
        m_currentImage(0),
        m_blittable(0),
        m_pmData(new QBlittablePixmapData(QPixmapData::PixmapType)),
        m_pixmap(new QPixmap(m_pmData))
{
}

QPaintDevice *QRasterBlittableWindowSurface::paintDevice()
{
    QPaintDevice *device = QRasterWindowSurface::paintDevice();
    if (m_currentImage != device) {
        if (device->devType() == QInternal::Image) {
            m_currentImage = static_cast<QImage *>(device);
            m_blittable = createBlittable(m_currentImage);
            m_pmData->setBlittable(m_blittable);
        }
    }
    return m_pixmap;
}
