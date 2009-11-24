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

protected:
    QImage *doLock()
    {
        return &image;
    }

    void doUnlock()
    {
    }

private:
    QImage image;
};

class Q_GUI_EXPORT  QBlittablePixmapData : public QPixmapData
{
//     Q_DECLARE_PRIVATE(QBlittablePixmapData);
public:
    QBlittablePixmapData(QPixmapData::PixelType type);
    ~QBlittablePixmapData();

    QBlittable *blittable();
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
