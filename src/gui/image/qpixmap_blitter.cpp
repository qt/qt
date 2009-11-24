#include "qpixmap_blitter_p.h"

#include <private/qapplication_p.h>
#include <private/qgraphicssystem_p.h>

QBlittablePixmapData::QBlittablePixmapData(QPixmapData::PixelType type)
    : QPixmapData(type,BlitterClass), m_engine(0), m_blittable(0)
{
}

QBlittablePixmapData::~QBlittablePixmapData()
{
    delete m_blittable;
    delete m_engine;
}

QBlittable *QBlittablePixmapData::blittable()
{
    if (!m_blittable) {
        m_blittable = QApplicationPrivate::graphicsSystem()->createBlittable(QRect(0,0,w,h));
    }

    return m_blittable;
}

void QBlittablePixmapData::setBlittable(QBlittable *blittable)
{
    if (m_blittable)
        delete m_blittable;
    m_blittable = blittable;
}

void QBlittablePixmapData::resize(int width, int height)
{
    delete m_blittable;
    m_blittable = 0;
    delete m_engine;
    m_engine = 0;
    d = QApplicationPrivate::graphicsSystem()->screens().at(0)->depth();
    w = width;
    h = height;
    is_null = (w <= 0 || h <= 0);
}
extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QBlittablePixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmWidthMM:
        return qRound(w * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(h * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDepth:
        return 32;
    case QPaintDevice::PdmDpiX: // fall-through
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY: // fall-through
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QRasterPixmapData::metric(): Unhandled metric type %d", metric);
        break;
    }

    return 0;
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

QImage QBlittablePixmapData::toImage() const
{
    return m_blittable->lock()->copy();
}

bool QBlittablePixmapData::hasAlphaChannel() const
{
    return m_blittable->lock()->hasAlphaChannel();
}

void QBlittablePixmapData::fromImage(const QImage &image,
                                     Qt::ImageConversionFlags)
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
