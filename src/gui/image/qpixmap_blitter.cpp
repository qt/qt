#include "qpixmap_blitter_p.h"

#include <private/qapplication_p.h>
#include <private/qgraphicssystem_p.h>

QBlittablePixmapData::QBlittablePixmapData(PixelType type)
    : QPixmapData(type,BlitterClass), m_blittable(0), m_engine(0)
{
}

QBlittablePixmapData::~QBlittablePixmapData()
{
}

void QBlittablePixmapData::resize(int width, int height)
{
    delete m_blittable;
    m_blittable = QApplicationPrivate::graphicsSystem()->createBlittable(QRect(0,0,width,height));
    m_engine = 0;
}

int QBlittablePixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    QImage *image = m_blittable->lock();
    return image->metric(metric);
}

void QBlittablePixmapData::fill(const QColor &color)
{
    if (m_blittable->capabilities() & QBlittable::SolidRectCapability)
        m_blittable->fillRect(m_blittable->rect(),color);
    else
        m_blittable->lock()->fill(color.rgb());
}

QImage *QBlittablePixmapData::buffer()
{
    return m_blittable->lock();
}

QImage QBlittablePixmapData::toImage()
{
    return m_blittable->lock()->copy();
}

bool QBlittablePixmapData::hasAlphaChannel()
{
    return m_blittable->lock()->hasAlphaChannel();
}

void QBlittablePixmapData::fromImage(const QImage &image,
                                     Qt::ImageConversionFlags flags)
{
    m_blittable = new QImageBlitter(image);
    m_engine = 0;
}

QPaintEngine *QBlittablePixmapData::paintEngine() const
{
    if (!m_engine) {
        QBlittablePixmapData *that = const_cast<QBlittablePixmapData *>(this);
        that->m_engine = new QBlitterPaintEngine(that);
    }
    return m_engine;
}
