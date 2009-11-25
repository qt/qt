#ifndef QPIXMAP_BLITTER_P_H
#define QPIXMAP_BLITTER_P_H

#include <private/qpixmapdata_p.h>
#include <private/qpaintengine_blitter_p.h>

class Q_GUI_EXPORT  QBlittablePixmapData : public QPixmapData
{
//     Q_DECLARE_PRIVATE(QBlittablePixmapData);
public:
    QBlittablePixmapData(QPixmapData::PixelType type);
    ~QBlittablePixmapData();

    QBlittable *blittable() const;
    void setBlittable(QBlittable *blittable);

    void resize(int width, int height);
    int metric(QPaintDevice::PaintDeviceMetric metric) const;
    void fill(const QColor &color);
    QImage *buffer();
    QImage toImage() const;
    bool hasAlphaChannel() const;
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);

    QPaintEngine *paintEngine() const;
protected:
    QBlitterPaintEngine *m_engine;
    QBlittable *m_blittable;

};

#endif // QPIXMAP_BLITTER_P_H
