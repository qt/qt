#ifndef QPIXMAP_BLITTER_P_H
#define QPIXMAP_BLITTER_P_H

#include <private/qpixmapdata_p.h>
#include <private/qpaintengine_blitter_p.h>

#ifndef QT_NO_BLITTABLE
QT_BEGIN_NAMESPACE

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

    void markRasterOverlay(const QRectF &);
    void markRasterOverlay(const QPointF &, const QTextItem &);
    void markRasterOverlay(const QVectorPath &);
    void markRasterOverlay(const QRect *rects, int rectCount);
    void markRasterOverlay(const QRectF *rects, int rectCount);
    void unmarkRasterOverlay(const QRectF &);

#ifdef QT_BLITTER_RASTEROVERLAY
    void mergeOverlay();
    void unmergeOverlay();
    QImage *overlay();

#endif //QT_BLITTER_RASTEROVERLAY
protected:
    QBlitterPaintEngine *m_engine;
    QBlittable *m_blittable;

#ifdef QT_BLITTER_RASTEROVERLAY
    QImage *m_rasterOverlay;
    QImage *m_unmergedCopy;
    QColor m_overlayColor;

    void markRasterOverlayImpl(const QRectF &);
    void unmarkRasterOverlayImpl(const QRectF &);
    QRectF clipAndTransformRect(const QRectF &) const;
#endif //QT_BLITTER_RASTEROVERLAY

};

inline void QBlittablePixmapData::markRasterOverlay(const QRectF &rect)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    markRasterOverlayImpl(rect);
#else
   Q_UNUSED(rect)
#endif
}

inline void QBlittablePixmapData::markRasterOverlay(const QVectorPath &path)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    markRasterOverlayImpl(path.convertToPainterPath().boundingRect());
#else
    Q_UNUSED(path)
#endif
}

inline void QBlittablePixmapData::markRasterOverlay(const QPointF &pos, const QTextItem &ti)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    QFontMetricsF fm(ti.font());
    QRectF rect = fm.tightBoundingRect(ti.text());
    rect.moveBottomLeft(pos);
    markRasterOverlay(rect);
#else
    Q_UNUSED(pos)
    Q_UNUSED(ti)
#endif
}

inline void QBlittablePixmapData::markRasterOverlay(const QRect *rects, int rectCount)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    for (int i = 0; i < rectCount; i++) {
        markRasterOverlay(rects[i]);
    }
#else
    Q_UNUSED(rects)
    Q_UNUSED(rectCount)
#endif
}
inline void QBlittablePixmapData::markRasterOverlay(const QRectF *rects, int rectCount)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    for (int i = 0; i < rectCount; i++) {
        markRasterOverlay(rects[i]);
    }
#else
    Q_UNUSED(rects)
    Q_UNUSED(rectCount)
#endif
}

inline void QBlittablePixmapData::unmarkRasterOverlay(const QRectF &rect)
{
#ifdef QT_BLITTER_RASTEROVERLAY
    unmarkRasterOverlayImpl(rect);
#else
    Q_UNUSED(rect)
#endif
}

QT_END_NAMESPACE
#endif // QT_NO_BLITTABLE
#endif // QPIXMAP_BLITTER_P_H
