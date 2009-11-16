#ifndef QPIXMAP_BLITTER_P_H
#define QPIXMAP_BLITTER_P_H

#include <private/qpixmapdata_p.h>
#include <private/qpaintengine_blitter_p.h>

class QImageBlitter : public QBlittable
{
public:
    QImageBlitter(const QImage &img)
        : QBlittable(img.rect(),0), image(img)
    {
    }

    void fillRect(const QRectF &, const QColor &)
    {
        //should never be called
    }
    void drawPixmap(const QRectF &, const QPixmap &, const QRectF &)
    {
        //should never be called
    }

    QImage *lock()
    {
        return &image;
    }

    void unlock()
    {
    }

private:
    QImage image;
};

class QBlittablePixmapData : public QPixmapData
{
public:
    QBlittablePixmapData(PixelType type);
    ~QBlittablePixmapData();

    QBlittable *blittable() const { return m_blittable; }

    void resize(int width, int height);
    int metric(QPaintDevice::PaintDeviceMetric metric) const;
    void fill(const QColor &color);
    QImage *buffer();
    QImage toImage();
    bool hasAlphaChannel();
    void fromImage(const QImage &image, Qt::ImageConversionFlags flags);

    QPaintEngine *paintEngine() const;

protected:
    QBlitterPaintEngine *m_engine;
    QBlittable *m_blittable;
};

#endif // QPIXMAP_BLITTER_P_H
