#ifndef QPAINTENGINE_BLITTER_P_H
#define QPAINTENGINE_BLITTER_P_H

#include "private/qpaintengineex_p.h"
#include "private/qpaintengine_raster_p.h"

#ifndef QT_NO_BLITTABLE
QT_BEGIN_NAMESPACE

class QBlitterPaintEnginePrivate;
class QBlittablePixmapData;
class QBlittable;

class Q_GUI_EXPORT QBlitterPaintEngine : public QPaintEngineEx
{
    Q_DECLARE_PRIVATE(QBlitterPaintEngine);
public:
    QBlitterPaintEngine(QBlittablePixmapData *p);
    ~QBlitterPaintEngine();

    virtual QPainterState *createState(QPainterState *orig) const;

    virtual QPaintEngine::Type type() const { return Blitter; }

    virtual bool begin(QPaintDevice *pdev);
    virtual bool end();

    virtual void fill(const QVectorPath &path, const QBrush &brush);
    virtual void stroke(const QVectorPath &path, const QPen &pen);

    virtual void clip(const QVectorPath &path, Qt::ClipOperation op);
    virtual void clip(const QRect &rect, Qt::ClipOperation op);
    virtual void clip(const QRegion &region, Qt::ClipOperation op);

    virtual void clipEnabledChanged();
    virtual void penChanged();
    virtual void brushChanged();
    virtual void brushOriginChanged();
    virtual void opacityChanged();
    virtual void compositionModeChanged();
    virtual void renderHintsChanged();
    virtual void transformChanged();

    virtual void fillRect(const QRectF &rect, const QBrush &brush);
    virtual void fillRect(const QRectF &rect, const QColor &color);

    virtual void drawRects(const QRect *rects, int rectCount);
    virtual void drawRects(const QRectF *rects, int rectCount);

    virtual void drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr);

    virtual void drawImage(const QRectF &r, const QImage &pm, const QRectF &sr, Qt::ImageConversionFlags flags);

    virtual void drawTextItem(const QPointF &pos, const QTextItem &ti);
    virtual void drawStaticTextItem(QStaticTextItem *);

    virtual void drawEllipse(const QRectF &r);

    virtual void setState(QPainterState *s);

    inline QPainterState *state() { return raster()->state(); }
    inline const QPainterState *state() const { const QPainterState *state = raster()->state(); return state;}
    inline const QClipData *clip(){return raster()->d_func()->clip();}

private:
    QRasterPaintEngine *raster() const;

};

QT_END_NAMESPACE
#endif //QT_NO_BLITTABLE
#endif // QPAINTENGINE_BLITTER_P_H

