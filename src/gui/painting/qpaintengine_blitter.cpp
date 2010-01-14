#include "private/qpaintengine_blitter_p.h"
#include "private/qpaintengine_raster_p.h"
#include "private/qpainter_p.h"
#include "private/qapplication_p.h"
#include "private/qpixmap_blitter_p.h"

#define STATE_XFORM_SCALE       0x00000001
#define STATE_XFORM_COMPLEX     0x00000002

#define STATE_BRUSH_PATTERN     0x00000010
#define STATE_BRUSH_ALPHA       0x00000020

#define STATE_PEN_ENABLED       0x00000100

#define STATE_ANTIALIASING      0x00001000
#define STATE_ALPHA             0x00002000
#define STATE_BLENDING_COMPLEX  0x00004000

#define STATE_CLIPSYS_COMPLEX   0x00010000
#define STATE_CLIP_COMPLEX      0x00020000

//#define RASTEROVERLAY

#ifdef RASTEROVERLAY
QColor rasterColor(255,0,0,100);
QBrush rasterBrush(rasterColor);
#endif

static inline void updateStateBits(uint *state, uint mask, bool on)
{
    *state = on ? (*state | mask) : (*state & ~mask);
}

class CapabilitiesToStateMask
{
public:
    CapabilitiesToStateMask(QBlittable::Capabilities capabilities)
            : m_capabilities(capabilities),
              fillRectMask(0), fillRectState(0),
              drawRectMask(0), drawRectState(0),
              drawPixmapMask(0), drawPixmapState(0)
    {
        if (capabilities & QBlittable::SolidRectCapability) {
            setFillRectMask();
        }
        if (capabilities & QBlittable::SourcePixmapCapability) {
           setSourcePixmapMask();
        }
        if (capabilities & QBlittable::SourceOverPixmapCapability) {
           setSourceOverPixmapMask();
        }
    }

    inline bool canBlitterFillRect() const
    {
       return !fillRectState || (fillRectState & fillRectMask);
    }

    inline bool canBlitterDrawRectMask() const
    {
        return drawRectState & drawRectMask;
    }

    bool canBlitterDrawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr) const
    {
        if ((!drawPixmapState) || drawPixmapState & drawPixmapMask) {
            if (m_capabilities & (QBlittable::SourceOverPixmapCapability
                                  | QBlittable::SourceOverScaledPixmapCapability)) {
                if (r.size() != sr.size()) {
                    return m_capabilities & QBlittable::SourceOverScaledPixmapCapability;
                } else {
                    return m_capabilities & QBlittable::SourceOverPixmapCapability;
                }
            }
            if ((m_capabilities & QBlittable::SourcePixmapCapability) && r.size() == sr.size() && !pm.hasAlphaChannel()) {
                return m_capabilities & QBlittable::SourcePixmapCapability;
            }
        }
        return false;
    }

    inline void updateFillRectBits(uint mask, bool on) {
        updateStateBits(&fillRectState,mask,on);
    }

    inline void updateDrawRectBits(uint mask, bool on) {
        updateStateBits(&drawRectState,mask,on);
    }

    inline void updateDrawPixmapBits(uint mask, bool on) {
        updateStateBits(&drawPixmapState,mask,on);
    }
    inline void updateAllWithBits(uint mask, bool on) {
        updateStateBits(&fillRectState,mask,on);
        updateStateBits(&drawRectState,mask,on);
        updateStateBits(&drawPixmapState,mask,on);
    }

public:

    void setFillRectMask() {
        updateStateBits(&fillRectMask, STATE_XFORM_SCALE, false);
        updateStateBits(&fillRectMask, STATE_XFORM_COMPLEX, false);

        updateStateBits(&fillRectMask, STATE_BRUSH_PATTERN, false);
        updateStateBits(&fillRectMask, STATE_BRUSH_ALPHA, false);

        updateStateBits(&fillRectMask, STATE_PEN_ENABLED, false);

        updateStateBits(&fillRectMask, STATE_ANTIALIASING, false);
        updateStateBits(&fillRectMask, STATE_ALPHA, false);
        updateStateBits(&fillRectMask, STATE_BLENDING_COMPLEX, false);

        updateStateBits(&fillRectMask, STATE_CLIPSYS_COMPLEX, false);
        updateStateBits(&fillRectMask, STATE_CLIP_COMPLEX, false);
    }

    void setSourcePixmapMask() {
        updateStateBits(&drawPixmapMask, STATE_XFORM_SCALE, true);
        updateStateBits(&drawPixmapMask, STATE_XFORM_COMPLEX, false);

        updateStateBits(&drawPixmapMask, STATE_BRUSH_PATTERN, false);
        updateStateBits(&drawPixmapMask, STATE_BRUSH_ALPHA, false);

        updateStateBits(&drawPixmapMask, STATE_PEN_ENABLED, false);

        updateStateBits(&drawPixmapMask, STATE_ANTIALIASING, false);
        updateStateBits(&drawPixmapMask, STATE_ALPHA, false);
        updateStateBits(&drawPixmapMask, STATE_BLENDING_COMPLEX, false);

        updateStateBits(&drawPixmapMask, STATE_CLIPSYS_COMPLEX, false);
        updateStateBits(&drawPixmapMask, STATE_CLIP_COMPLEX, false);
    }

    void setSourceOverPixmapMask() {
        setSourcePixmapMask();
    }

    QBlittable::Capabilities m_capabilities;
    uint fillRectMask;
    uint fillRectState;
    uint drawRectMask;
    uint drawRectState;
    uint drawPixmapMask;
    uint drawPixmapState;
};

class QBlitterPaintEnginePrivate : public QPaintEngineExPrivate
{
public:
    QBlitterPaintEnginePrivate(QBlittablePixmapData *p)
            : QPaintEngineExPrivate(),
              isBlitterLocked(false),
              hasXForm(false)

    {
        blitter= p->blittable();
        raster = new QRasterPaintEngine(p->buffer());
        capabillities = new CapabilitiesToStateMask(blitter->capabilities());
    }

    inline void lock() {
        if (!isBlitterLocked) {
            raster->d_func()->rasterBuffer->prepare(blitter->lock());
            isBlitterLocked = true;
        }
    }

    inline void unlock() {
        if (isBlitterLocked) {
            blitter->unlock();
            isBlitterLocked = false;
        }
    }

    void fillRect(const QRectF &rect, const QColor &color) {
//        lock();
        QRectF targetRect = rect;
        if (hasXForm) {
            targetRect = state->matrix.mapRect(rect);
        }
        QClipData *clipData = state->clip;
        if (clipData) {
            if (clipData->hasRectClip) {
                unlock();
                blitter->fillRect(targetRect & clipData->clipRect, color);
            } else if (clipData->hasRegionClip) {
                QVector<QRect> rects = clipData->clipRegion.rects();
                for ( int i = 0; i < rects.size(); i++ ) {
                    QRect intersectRect = rects.at(i).intersected(targetRect.toRect());
                    if (!intersectRect.isEmpty()) {
                        unlock();
                        blitter->fillRect(intersectRect,color);
                    }
                }
            }
        } else {
            if (targetRect.x() >= 0 && targetRect.y() >= 0
                && targetRect.width() <= raster->paintDevice()->width()
                && targetRect.height() <= raster->paintDevice()->height()) {
                unlock();
                blitter->fillRect(targetRect,color);
            } else {
                QRectF deviceRect(0,0,raster->paintDevice()->width(), raster->paintDevice()->height());
                unlock();
                blitter->fillRect(deviceRect&targetRect,color);
            }
        }
    }

    void clipAndDrawPixmap(const QRectF &clip, const QRectF &target, const QPixmap &pm, const QRectF &sr) {
        QRectF intersectedRect = clip.intersected(target);
        if (intersectedRect.isEmpty())
            return;

        QRectF source = sr;
        if(intersectedRect.size() != target.size()) {
            qreal deltaTop = target.top() - intersectedRect.top();
            qreal deltaLeft = target.left() - intersectedRect.left();
            qreal deltaBottom = target.bottom() - intersectedRect.bottom();
            qreal deltaRight = target.right() - intersectedRect.right();
            source.adjust(-deltaLeft,-deltaTop,deltaRight,deltaBottom);
        }
        blitter->drawPixmap(intersectedRect, pm, source);
    }

    void updateClip() {
        const QClipData *clip = raster->d_func()->clip();
        bool complex = clip != 0 && !(clip->hasRectClip || clip->hasRegionClip);
        capabillities->updateAllWithBits(STATE_CLIP_COMPLEX, complex);
    }

    void systemStateChanged() {
        raster->d_func()->systemStateChanged();
    }

    QPixmap pixmap;
    QRasterPaintEngine *raster;
    QRasterPaintEngineState *state;

    QBlittable *blitter;
    bool isBlitterLocked;

    CapabilitiesToStateMask *capabillities;

    uint hasXForm;
};

QBlitterPaintEngine::QBlitterPaintEngine(QBlittablePixmapData *p)
    : QPaintEngineEx(*(new QBlitterPaintEnginePrivate(p)))
{
}

QBlitterPaintEngine::~QBlitterPaintEngine()
{
}

QPainterState *QBlitterPaintEngine::createState(QPainterState *orig) const
{
    Q_D(const QBlitterPaintEngine);
    return d->raster->createState(orig);
}

bool QBlitterPaintEngine::begin(QPaintDevice *pdev)
{
    Q_D(QBlitterPaintEngine);

    d->raster->setSystemRect(systemRect());
    d->raster->setSystemClip(systemClip());

    bool ok = d->raster->begin(pdev);
    return ok;
}


bool QBlitterPaintEngine::end()
{
    Q_D(QBlitterPaintEngine);

    d->raster->setPaintDevice(0);
    d->raster->setActive(false);
    return d->raster->end();
}


void QBlitterPaintEngine::fill(const QVectorPath &path, const QBrush &brush)
{
    Q_D(QBlitterPaintEngine);

    if (path.shape() == QVectorPath::RectangleHint) {
        QRectF rect(((QPointF *) path.points())[0], ((QPointF *) path.points())[2]);
        fillRect(rect, brush);
    } else {
        d->lock();
        d->raster->fill(path, brush);
#ifdef RASTEROVERLAY
        d->raster->fill(path,rasterBrush);
#endif
    }
}

void QBlitterPaintEngine::fillRect(const QRectF &rect, const QColor &color)
{
    Q_D(QBlitterPaintEngine);
    if (d->capabillities->canBlitterFillRect() && color.alpha() == 0xff) {
        d->fillRect(rect, color);
    } else {
        d->lock();
        d->raster->fillRect(rect, color);
#ifdef RASTEROVERLAY
        d->raster->fillRect(rect,rasterColor);
#endif
    }
}

void QBlitterPaintEngine::fillRect(const QRectF &rect, const QBrush &brush)
{
    if(rect.size().isEmpty())
        return;
    Q_D(QBlitterPaintEngine);
    if (qbrush_style(brush) == Qt::SolidPattern
        && qbrush_color(brush).alpha() == 0xff
        && d->capabillities->canBlitterFillRect())
    {
        d->fillRect(rect, qbrush_color(brush));
    }else if (brush.style() == Qt::TexturePattern
              && d->capabillities->canBlitterDrawPixmap(rect,brush.texture(),rect))
    {
        bool rectIsFilled = false;
        QRectF transformedRect = state()->matrix.mapRect(rect);
        qreal x = transformedRect.x();
        qreal y = transformedRect.y();
        QPixmap pm = brush.texture();
        d->unlock();
        int srcX = (int)(rect.x() - d->raster->state()->brushOrigin.x()) % pm.width();
        if (srcX < 0)
            srcX = pm.width() + srcX;
        const int startX = srcX;
        int srcY = (int)(rect.y() - d->raster->state()->brushOrigin.y())% pm.height();
        if (srcY < 0)
            srcY = pm.height() + srcY;
        while (!rectIsFilled) {
            qreal blitWidth = (pm.width() ) - srcX;
            qreal blitHeight = (pm.height() ) - srcY;
            if (x + blitWidth > transformedRect.right())
                blitWidth = transformedRect.right() -x;
            if (y + blitHeight > transformedRect.bottom())
                blitHeight = transformedRect.bottom() - y;
            const QClipData *clipData = d->raster->d_func()->clip();
            if (clipData->hasRectClip) {
                QRect targetRect = QRect(x,y,blitWidth,blitHeight).intersected(clipData->clipRect);
                if (targetRect.isValid()) {
                    int tmpSrcX  = srcX + (targetRect.x() - x);
                    int tmpSrcY = srcY + (targetRect.y() - y);
                    QRect srcRect(tmpSrcX,tmpSrcY,targetRect.width(),targetRect.height());
                    d->blitter->drawPixmap(targetRect,pm,srcRect);
                }
            } else if (clipData->hasRegionClip) {
                QVector<QRect> clipRects = clipData->clipRegion.rects();
                QRect unclippedTargetRect(x,y,blitWidth,blitHeight);
                QRegion intersectedRects = clipData->clipRegion.intersected(unclippedTargetRect);

                for ( int i = 0; i < intersectedRects.rects().size(); i++ ) {
                    QRect targetRect = intersectedRects.rects().at(i);
                    if (!targetRect.isValid() || targetRect.isEmpty())
                        continue;
                    int tmpSrcX = srcX + (targetRect.x() - x);
                    int tmpSrcY = srcY + (targetRect.y() - y);
                    QRect srcRect(tmpSrcX,tmpSrcY,targetRect.width(),targetRect.height());
                    d->blitter->drawPixmap(targetRect,pm,srcRect);
                }
            } else Q_ASSERT(false);//should never happen
            x+=blitWidth;
            if (x>=transformedRect.right()) {
                x = transformedRect.x();
                srcX = startX;
                srcY = 0;
                y+=blitHeight;
                if (y>=transformedRect.bottom())
                    rectIsFilled = true;
            } else
                srcX = 0;
        }

    } else {
        d->lock();
        d->raster->fillRect(rect, brush);
#ifdef RASTEROVERLAY
        d->raster->fillRect(rect,rasterBrush);
#endif
    }

}

void QBlitterPaintEngine::stroke(const QVectorPath &path, const QPen &pen)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->stroke(path, pen);
#ifdef RASTEROVERLAY
    d->raster->stroke(path,QPen(rasterColor));
#endif
}

void QBlitterPaintEngine::clip(const QVectorPath &path, Qt::ClipOperation op)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->clip(path, op);
    d->updateClip();
}
void QBlitterPaintEngine::clip(const QRect &rect, Qt::ClipOperation op){
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->clip(rect,op);
    d->updateClip();
}
void QBlitterPaintEngine::clip(const QRegion &region, Qt::ClipOperation op)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->clip(region,op);
    d->updateClip();
}

void QBlitterPaintEngine::clipEnabledChanged()
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->clipEnabledChanged();
}

void QBlitterPaintEngine::penChanged()
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->penChanged();
    d->capabillities->updateDrawRectBits(STATE_PEN_ENABLED,qpen_style(d->state->pen) != Qt::NoPen);
}

void QBlitterPaintEngine::brushChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->brushChanged();

    bool solid = qbrush_style(d->state->brush) == Qt::SolidPattern;

    d->capabillities->updateDrawRectBits(STATE_BRUSH_PATTERN, !solid);
    d->capabillities->updateDrawRectBits(STATE_BRUSH_ALPHA,
                                        qbrush_color(d->state->brush).alpha() < 255);
}

void QBlitterPaintEngine::brushOriginChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->brushOriginChanged();
}

void QBlitterPaintEngine::opacityChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->opacityChanged();

    bool translucent = d->state->opacity < 1;
    d->capabillities->updateAllWithBits(STATE_ALPHA,translucent);
}

void QBlitterPaintEngine::compositionModeChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->compositionModeChanged();

    bool nonTrivial = d->state->composition_mode != QPainter::CompositionMode_SourceOver
                      && d->state->composition_mode != QPainter::CompositionMode_Source;

    d->capabillities->updateAllWithBits(STATE_BLENDING_COMPLEX,nonTrivial);
}

void QBlitterPaintEngine::renderHintsChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->renderHintsChanged();

    bool aa = d->state->renderHints & QPainter::Antialiasing;
    d->capabillities->updateFillRectBits(STATE_ANTIALIASING, aa);
    d->capabillities->updateDrawRectBits(STATE_ANTIALIASING, aa);

}

void QBlitterPaintEngine::transformChanged()
{
    Q_D(QBlitterPaintEngine);
    d->raster->transformChanged();

    QTransform::TransformationType type = d->state->matrix.type();

    d->capabillities->updateAllWithBits(STATE_XFORM_COMPLEX, type > QTransform::TxScale);

    d->hasXForm = type >= QTransform::TxTranslate;

}

void QBlitterPaintEngine::drawRects(const QRect *rects, int rectCount)
{
    Q_D(QBlitterPaintEngine);
    if (d->capabillities->canBlitterDrawRectMask()) {
        for (int i=0; i<rectCount; ++i) {
            d->fillRect(rects[i], qbrush_color(d->state->brush));
        }
    } else {
        QPaintEngineEx::drawRects(rects, rectCount);
#ifdef RASTEROVERLAY
        d->raster->drawRects(rects,rectCount);
#endif
    }
}

void QBlitterPaintEngine::drawRects(const QRectF *rects, int rectCount)
{
    Q_D(QBlitterPaintEngine);
    if (d->capabillities->canBlitterDrawRectMask()) {
        for (int i=0; i<rectCount; ++i) {
            d->fillRect(rects[i], qbrush_color(d->state->brush));
        }
    } else {
        QPaintEngineEx::drawRects(rects, rectCount);
#ifdef RASTEROVERLAY
        d->raster->drawRects(rects,rectCount);
#endif
    }
}

void QBlitterPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
    Q_D(QBlitterPaintEngine);
    if (d->capabillities->canBlitterDrawPixmap(r,pm,sr)) {

        d->unlock();
        QRectF targetRect = r;
        if (d->hasXForm) {
            targetRect = state()->matrix.mapRect(r);
        }
        const QClipData *clipData = d->raster->d_func()->clip();
        if (clipData) {
            if (clipData->hasRectClip) {
                d->clipAndDrawPixmap(clipData->clipRect,targetRect,pm,sr);
            }else if (clipData->hasRegionClip) {
                QVector<QRect>rects = clipData->clipRegion.rects();
                for (int i = 0; i<rects.size(); i++) {
                    d->clipAndDrawPixmap(rects.at(i),targetRect,pm,sr);
                }
            }
        } else {
            QRectF deviceRect(0,0,d->raster->paintDevice()->width(), d->raster->paintDevice()->height());
            d->clipAndDrawPixmap(deviceRect,targetRect,pm,sr);
        }
    }else {
        d->lock();
        d->raster->drawPixmap(r, pm, sr);
#ifdef RASTEROVERLAY
        d->raster->fillRect(r,rasterColor);
#endif
    }
}

void QBlitterPaintEngine::drawImage(const QRectF &r, const QImage &pm, const QRectF &sr,
                                    Qt::ImageConversionFlags flags)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->drawImage(r, pm, sr, flags);
#ifdef RASTEROVERLAY
    d->raster->fillRect(r,rasterColor);
#endif
}


void QBlitterPaintEngine::drawTextItem(const QPointF &pos, const QTextItem &ti)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    d->raster->drawTextItem(pos, ti);
#ifdef RASTEROVERLAY
    QFontMetricsF fm(ti.font());
    QRectF rect = fm.tightBoundingRect(ti.text());
    rect.moveBottomLeft(pos);
    d->raster->fillRect(rect, rasterColor);
#endif
}


void QBlitterPaintEngine::setState(QPainterState *s)
{
    Q_D(QBlitterPaintEngine);
    d->lock();
    QPaintEngineEx::setState(s);
    d->raster->setState(s);
    d->state = (QRasterPaintEngineState *) s;

    clipEnabledChanged();
    penChanged();
    brushChanged();
    brushOriginChanged();
    opacityChanged();
    compositionModeChanged();
    renderHintsChanged();
    transformChanged();

    d->updateClip();
}

class QBlittablePrivate
{
public:
    QBlittablePrivate(const QRect &rect, QBlittable::Capabilities caps)
        : caps(caps), m_rect(rect), locked(false), cachedImg(0)
    {}
    QBlittable::Capabilities caps;
    QRect m_rect;
    bool locked;
    QImage *cachedImg;
};


QBlittable::QBlittable(const QRect &rect, Capabilities caps)
    : d_ptr(new QBlittablePrivate(rect,caps))
{
}

QBlittable::~QBlittable()
{
    delete d_ptr;
}


QBlittable::Capabilities QBlittable::capabilities() const
{
    Q_D(const QBlittable);
    return d->caps;
}

QRect QBlittable::rect() const
{
    Q_D(const QBlittable);
    return d->m_rect;
}

QImage *QBlittable::lock()
{
    Q_D(QBlittable);
    if (!d->locked) {
        d->cachedImg = doLock();
        d->locked = true;
    }

    return d->cachedImg;
}

void QBlittable::unlock()
{
    Q_D(QBlittable);
    if (d->locked) {
        doUnlock();
        d->locked = false;
    }
}
