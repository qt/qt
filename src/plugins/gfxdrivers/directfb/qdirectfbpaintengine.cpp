/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdirectfbpaintengine.h"

#ifndef QT_NO_DIRECTFB

#include "qdirectfbwindowsurface.h"
#include "qdirectfbscreen.h"
#include "qdirectfbpixmap.h"
#include <directfb.h>
#include <qtransform.h>
#include <qvarlengtharray.h>
#include <qcache.h>
#include <qmath.h>
#include <private/qpixmapdata_p.h>
#include <private/qpixmap_raster_p.h>

#if defined QT_DIRECTFB_WARN_ON_RASTERFALLBACKS || defined QT_DIRECTFB_DISABLE_RASTERFALLBACKS
#define VOID_ARG() static_cast<bool>(false)
enum PaintOperation {
    DRAW_RECTS = 0x0001,
    DRAW_LINES = 0x0002,
    DRAW_IMAGE = 0x0004,
    DRAW_PIXMAP = 0x0008,
    DRAW_TILED_PIXMAP = 0x0010,
    STROKE_PATH = 0x0020,
    DRAW_PATH = 0x0040,
    DRAW_POINTS = 0x0080,
    DRAW_ELLIPSE = 0x0100,
    DRAW_POLYGON = 0x0200,
    DRAW_TEXT = 0x0400,
    FILL_PATH = 0x0800,
    FILL_RECT = 0x1000,
    DRAW_COLORSPANS = 0x2000,
    ALL = 0xffff
};
#endif

#ifdef QT_DIRECTFB_WARN_ON_RASTERFALLBACKS
template <typename T> inline const T *ptr(const T &t) { return &t; }
template <> inline const bool* ptr<bool>(const bool &) { return 0; }
template <typename device, typename T1, typename T2, typename T3>
static void rasterFallbackWarn(const char *msg, const char *func, const device *dev,
                               int scale, bool matrixRotShear, bool simplePen,
                               bool dfbHandledClip,
                               const char *nameOne, const T1 &one,
                               const char *nameTwo, const T2 &two,
                               const char *nameThree, const T3 &three)
{
    QString out;
    QDebug dbg(&out);
    dbg << msg << (QByteArray(func) + "()")  << "painting on";
    if (dev->devType() == QInternal::Widget) {
        dbg << static_cast<const QWidget*>(dev);
    } else {
        dbg << dev << "of type" << dev->devType();
    }

    dbg << "scale" << scale
        << "matrixRotShear" << matrixRotShear
        << "simplePen" << simplePen
        << "dfbHandledClip" << dfbHandledClip;

    const T1 *t1 = ptr(one);
    const T2 *t2 = ptr(two);
    const T3 *t3 = ptr(three);

    if (t1) {
        dbg << nameOne << *t1;
        if (t2) {
            dbg << nameTwo << *t2;
            if (t3) {
                dbg << nameThree << *t3;
            }
        }
    }
    qWarning("%s", qPrintable(out));
}
#endif

#if defined QT_DIRECTFB_WARN_ON_RASTERFALLBACKS && defined QT_DIRECTFB_DISABLE_RASTERFALLBACKS
#define RASTERFALLBACK(op, one, two, three)                             \
    if (op & (QT_DIRECTFB_WARN_ON_RASTERFALLBACKS))                     \
        rasterFallbackWarn("Disabled raster engine operation",          \
                           __FUNCTION__, state()->painter->device(),    \
                           d_func()->scale, d_func()->matrixRotShear, \
                           d_func()->simplePen, d_func()->dfbCanHandleClip(), \
                           #one, one, #two, two, #three, three);        \
    if (op & (QT_DIRECTFB_DISABLE_RASTERFALLBACKS))                     \
        return;
#elif defined QT_DIRECTFB_DISABLE_RASTERFALLBACKS
#define RASTERFALLBACK(op, one, two, three)             \
    if (op & (QT_DIRECTFB_DISABLE_RASTERFALLBACKS))     \
        return;
#elif defined QT_DIRECTFB_WARN_ON_RASTERFALLBACKS
#define RASTERFALLBACK(op, one, two, three)                             \
    if (op & (QT_DIRECTFB_WARN_ON_RASTERFALLBACKS))                     \
        rasterFallbackWarn("Falling back to raster engine for",         \
                           __FUNCTION__, state()->painter->device(),    \
                           d_func()->scale, d_func()->matrixRotShear, \
                           d_func()->simplePen, d_func()->dfbCanHandleClip(), \
                           #one, one, #two, two, #three, three);
#else
#define RASTERFALLBACK(op, one, two, three)
#endif

static inline uint ALPHA_MUL(uint x, uint a)
{
    uint t = x * a;
    t = ((t + (t >> 8) + 0x80) >> 8) & 0xff;
    return t;
}

class SurfaceCache
{
public:
    SurfaceCache() : surface(0), buffer(0), bufsize(0) {}
    ~SurfaceCache() { clear(); }


    IDirectFBSurface *getSurface(const uint *buf, int size)
    {
        if (buffer == buf && bufsize == size)
            return surface;

        clear();

        const DFBSurfaceDescription description = QDirectFBScreen::getSurfaceDescription(buf, size);
        surface = QDirectFBScreen::instance()->createDFBSurface(description, QDirectFBScreen::TrackSurface);
        if (!surface)
            qWarning("QDirectFBPaintEngine: SurfaceCache: Unable to create surface");

        buffer = const_cast<uint*>(buf);
        bufsize = size;

        return surface;
    }

    void clear()
    {
        if (surface && QDirectFBScreen::instance())
            QDirectFBScreen::instance()->releaseDFBSurface(surface);
        surface = 0;
        buffer = 0;
        bufsize = 0;
    }
private:
    IDirectFBSurface *surface;
    uint *buffer;
    int bufsize;
};


#ifdef QT_DIRECTFB_IMAGECACHE
#include <private/qimage_p.h>
struct CachedImage
{
    IDirectFBSurface *surface;
    ~CachedImage()
    {
        if (surface && QDirectFBScreen::instance()) {
            QDirectFBScreen::instance()->releaseDFBSurface(surface);
        }
    }
};
static QCache<qint64, CachedImage> imageCache(4*1024*1024); // 4 MB
#endif

class QDirectFBPaintEnginePrivate : public QRasterPaintEnginePrivate
{
public:
    enum Scale { NoScale, Scaled, NegativeScale };

    QDirectFBPaintEnginePrivate(QDirectFBPaintEngine *p);
    ~QDirectFBPaintEnginePrivate();

    void setTransform(const QTransform &m);
    void setPen(const QPen &pen);
    inline void setCompositionMode(QPainter::CompositionMode mode);
    inline void setOpacity(quint8 value);
    void setRenderHints(QPainter::RenderHints hints);

    inline void setDFBColor(const QColor &color);

    inline void lock();
    inline void unlock();

    inline bool dfbCanHandleClip(const QRect &rect) const;
    inline bool dfbCanHandleClip(const QRectF &rect) const;
    inline bool dfbCanHandleClip() const;
    inline bool isSimpleBrush(const QBrush &brush) const;

    void drawLines(const QLine *lines, int count);
    void drawLines(const QLineF *lines, int count);

    void fillRegion(const QRegion &r);
    void fillRects(const QRect *rects, int count);
    void drawRects(const QRect *rects, int count);
    void fillRects(const QRectF *rects, int count);
    void drawRects(const QRectF *rects, int count);

    void drawTiledPixmap(const QRectF &dest, const QPixmap &pixmap);
    void blit(const QRectF &dest, IDirectFBSurface *surface, const QRectF &src);

    inline void updateClip();
    void systemStateChanged();

    void begin(QPaintDevice *device);
    void end();

    static IDirectFBSurface *getSurface(const QImage &img, bool *release);

#ifdef QT_DIRECTFB_IMAGECACHE
    static inline int cacheCost(const QImage &img) { return img.width() * img.height() * img.depth() / 8; }
#endif

    void prepareForBlit(bool alpha);
private:
    IDirectFBSurface *surface;

    QPen pen;

    bool antialiased;

    bool simplePen;

    bool matrixRotShear;
    Scale scale;

    SurfaceCache *surfaceCache;
    QTransform transform;
    int lastLockedHeight;

    IDirectFB *fb;
    int fbWidth;
    int fbHeight;

    quint8 opacity;

    bool dirtyClip;
    bool dfbHandledClip;
    bool ignoreSystemClip;
    QDirectFBPaintDevice *dfbDevice;
    void *lockedMemory;
    bool unsupportedCompositionMode;

    QDirectFBPaintEngine *q;
    friend class QDirectFBPaintEngine;
};

QDirectFBPaintEngine::QDirectFBPaintEngine(QPaintDevice *device)
    : QRasterPaintEngine(*(new QDirectFBPaintEnginePrivate(this)), device)
{
}

QDirectFBPaintEngine::~QDirectFBPaintEngine()
{
}

bool QDirectFBPaintEngine::begin(QPaintDevice *device)
{
    Q_D(QDirectFBPaintEngine);
    d->begin(device);
    const bool status = QRasterPaintEngine::begin(device);

    // XXX: QRasterPaintEngine::begin() resets the capabilities
    gccaps |= PorterDuff;

    return status;
}

bool QDirectFBPaintEngine::end()
{
    Q_D(QDirectFBPaintEngine);
    d->end();
    return QRasterPaintEngine::end();
}

void QDirectFBPaintEngine::clipEnabledChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->dirtyClip = true;
    QRasterPaintEngine::clipEnabledChanged();
}

void QDirectFBPaintEngine::penChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setPen(state()->pen);

    QRasterPaintEngine::penChanged();
}

void QDirectFBPaintEngine::opacityChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setOpacity(quint8(state()->opacity * 255));
    QRasterPaintEngine::opacityChanged();
}

void QDirectFBPaintEngine::compositionModeChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setCompositionMode(state()->compositionMode());
    QRasterPaintEngine::compositionModeChanged();
}

void QDirectFBPaintEngine::renderHintsChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setRenderHints(state()->renderHints);
    QRasterPaintEngine::renderHintsChanged();
}

void QDirectFBPaintEngine::transformChanged()
{
    Q_D(QDirectFBPaintEngine);
    const QDirectFBPaintEnginePrivate::Scale old = d->scale;
    d->setTransform(state()->transform());
    if (d->scale != old) {
        d->setPen(state()->pen);
    }
    QRasterPaintEngine::transformChanged();
}

void QDirectFBPaintEngine::setState(QPainterState *s)
{
    Q_D(QDirectFBPaintEngine);
    QRasterPaintEngine::setState(s);
    d->dirtyClip = true;
    d->setPen(state()->pen);
    d->setOpacity(quint8(state()->opacity * 255));
    d->setCompositionMode(state()->compositionMode());
    d->setTransform(state()->transform());
}

void QDirectFBPaintEngine::clip(const QVectorPath &path, Qt::ClipOperation op)
{
    Q_D(QDirectFBPaintEngine);
    d->dirtyClip = true;
    const QPoint bottom = d->transform.map(QPoint(0, int(path.controlPointRect().y2)));
    if (bottom.y() >= d->lastLockedHeight)
        d->lock();
    QRasterPaintEngine::clip(path, op);
}

void QDirectFBPaintEngine::clip(const QRect &rect, Qt::ClipOperation op)
{
    Q_D(QDirectFBPaintEngine);
    d->dirtyClip = true;
    if (d->clip() && !d->clip()->hasRectClip && d->clip()->enabled) {
        const QPoint bottom = d->transform.map(QPoint(0, rect.bottom()));
        if (bottom.y() >= d->lastLockedHeight)
            d->lock();
    }

    QRasterPaintEngine::clip(rect, op);
}

void QDirectFBPaintEngine::drawRects(const QRect *rects, int rectCount)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    const QBrush &brush = state()->brush;
    if (d->unsupportedCompositionMode || !d->dfbCanHandleClip() || d->matrixRotShear
        || !d->simplePen || !d->isSimpleBrush(brush)) {
        RASTERFALLBACK(DRAW_RECTS, rectCount, VOID_ARG(), VOID_ARG());
        d->lock();
        QRasterPaintEngine::drawRects(rects, rectCount);
        return;
    }

    d->unlock();

    if (brush != Qt::NoBrush) {
        d->setDFBColor(brush.color());
        d->fillRects(rects, rectCount);
    }
    if (d->pen != Qt::NoPen) {
        d->setDFBColor(d->pen.color());
        d->drawRects(rects, rectCount);
    }
}

void QDirectFBPaintEngine::drawRects(const QRectF *rects, int rectCount)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    const QBrush &brush = state()->brush;
    if (d->unsupportedCompositionMode || !d->dfbCanHandleClip() || d->matrixRotShear
        || !d->simplePen || !d->isSimpleBrush(brush)) {
        RASTERFALLBACK(DRAW_RECTS, rectCount, VOID_ARG(), VOID_ARG());
        d->lock();
        QRasterPaintEngine::drawRects(rects, rectCount);
        return;
    }

    d->unlock();

    if (brush != Qt::NoBrush) {
        d->setDFBColor(brush.color());
        d->fillRects(rects, rectCount);
    }
    if (d->pen != Qt::NoPen) {
        d->setDFBColor(d->pen.color());
        d->drawRects(rects, rectCount);
    }
}

void QDirectFBPaintEngine::drawLines(const QLine *lines, int lineCount)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (d->unsupportedCompositionMode || !d->simplePen || !d->dfbCanHandleClip()) {
        RASTERFALLBACK(DRAW_LINES, lineCount, VOID_ARG(), VOID_ARG());
        d->lock();
        QRasterPaintEngine::drawLines(lines, lineCount);
        return;
    }

    if (d->pen != Qt::NoPen) {
        d->unlock();
        d->setDFBColor(d->pen.color());
        d->drawLines(lines, lineCount);
    }
}

void QDirectFBPaintEngine::drawLines(const QLineF *lines, int lineCount)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (d->unsupportedCompositionMode || !d->simplePen || !d->dfbCanHandleClip()) {
        RASTERFALLBACK(DRAW_LINES, lineCount, VOID_ARG(), VOID_ARG());
        d->lock();
        QRasterPaintEngine::drawLines(lines, lineCount);
        return;
    }

    if (d->pen != Qt::NoPen) {
        d->unlock();
        d->setDFBColor(d->pen.color());
        d->drawLines(lines, lineCount);
    }
}

void QDirectFBPaintEngine::drawImage(const QRectF &r, const QImage &image,
                                     const QRectF &sr,
                                     Qt::ImageConversionFlags flags)
{
    Q_D(QDirectFBPaintEngine);
    Q_UNUSED(flags);

    /*  This is hard to read. The way it works is like this:

    - If you do not have support for preallocated surfaces and do not use an
    image cache we always fall back to raster engine.

    - If it's rotated/sheared/mirrored (negative scale) or we can't
    clip it we fall back to raster engine.

    - If we don't cache the image, but we do have support for
    preallocated surfaces we fall back to the raster engine if the
    image is in a format DirectFB can't handle.

    - If we do cache the image but don't have support for preallocated
    images and the cost of caching the image (bytes used) is higher
    than the max image cache size we fall back to raster engine.
    */

    d->updateClip();
#if !defined QT_NO_DIRECTFB_PREALLOCATED || defined QT_DIRECTFB_IMAGECACHE
    if (d->unsupportedCompositionMode
        || d->matrixRotShear
        || d->scale == QDirectFBPaintEnginePrivate::NegativeScale
        || !d->dfbCanHandleClip(r)
#ifndef QT_DIRECTFB_IMAGECACHE
        || QDirectFBScreen::getSurfacePixelFormat(image.format()) == DSPF_UNKNOWN
#elif defined QT_NO_DIRECTFB_PREALLOCATED
        || QDirectFBPaintEnginePrivate::cacheCost(image) > imageCache.maxCost()
#endif
        )
#endif
    {
        RASTERFALLBACK(DRAW_IMAGE, r, image.size(), sr);
        d->lock();
        QRasterPaintEngine::drawImage(r, image, sr, flags);
        return;
    }
#if !defined QT_NO_DIRECTFB_PREALLOCATED || defined QT_DIRECTFB_IMAGECACHE
    d->unlock();
    bool release;
    IDirectFBSurface *imgSurface = d->getSurface(image, &release);
    d->prepareForBlit(QDirectFBScreen::hasAlpha(imgSurface));
    d->blit(r, imgSurface, sr);
    if (release) {
        imgSurface->ReleaseSource(imgSurface);
        imgSurface->Release(imgSurface);
    }
#endif
}

void QDirectFBPaintEngine::drawImage(const QPointF &p, const QImage &img)
{
    drawImage(QRectF(p, img.size()), img, img.rect());
}

void QDirectFBPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pixmap,
                                      const QRectF &sr)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();

    if (pixmap.pixmapData()->classId() != QPixmapData::DirectFBClass) {
        RASTERFALLBACK(DRAW_PIXMAP, r, pixmap.size(), sr);
        d->lock();
        QRasterPaintEngine::drawPixmap(r, pixmap, sr);
    } else if (d->unsupportedCompositionMode || !d->dfbCanHandleClip(r) || d->matrixRotShear
               || d->scale == QDirectFBPaintEnginePrivate::NegativeScale) {
        RASTERFALLBACK(DRAW_PIXMAP, r, pixmap.size(), sr);
        const QImage *img = static_cast<QDirectFBPixmapData*>(pixmap.pixmapData())->buffer(DSLF_READ);
        d->lock();
        QRasterPaintEngine::drawImage(r, *img, sr);
    } else {
        d->unlock();
        d->prepareForBlit(pixmap.hasAlphaChannel());
        QPixmapData *data = pixmap.pixmapData();
        Q_ASSERT(data->classId() == QPixmapData::DirectFBClass);
        QDirectFBPixmapData *dfbData = static_cast<QDirectFBPixmapData*>(data);
        IDirectFBSurface *s = dfbData->directFBSurface();
        d->blit(r, s, sr);
    }
}

void QDirectFBPaintEngine::drawPixmap(const QPointF &p, const QPixmap &pm)
{
    drawPixmap(QRectF(p, pm.size()), pm, pm.rect());
}

void QDirectFBPaintEngine::drawTiledPixmap(const QRectF &r,
                                           const QPixmap &pixmap,
                                           const QPointF &sp)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (pixmap.pixmapData()->classId() != QPixmapData::DirectFBClass) {
        RASTERFALLBACK(DRAW_TILED_PIXMAP, r, pixmap.size(), sp);
        d->lock();
        QRasterPaintEngine::drawTiledPixmap(r, pixmap, sp);
    } else if (d->unsupportedCompositionMode || !d->dfbCanHandleClip(r) || d->matrixRotShear || !sp.isNull()
               || d->scale == QDirectFBPaintEnginePrivate::NegativeScale) {
        RASTERFALLBACK(DRAW_TILED_PIXMAP, r, pixmap.size(), sp);
        const QImage *img = static_cast<QDirectFBPixmapData*>(pixmap.pixmapData())->buffer(DSLF_READ);
        d->lock();
        QRasterPixmapData *data = new QRasterPixmapData(QPixmapData::PixmapType);
        data->fromImage(*img, Qt::AutoColor);
        const QPixmap pix(data);
        QRasterPaintEngine::drawTiledPixmap(r, pix, sp);
    } else {
        d->unlock();
        d->drawTiledPixmap(r, pixmap);
    }
}


void QDirectFBPaintEngine::stroke(const QVectorPath &path, const QPen &pen)
{
    RASTERFALLBACK(STROKE_PATH, path, VOID_ARG(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::stroke(path, pen);
}

void QDirectFBPaintEngine::drawPath(const QPainterPath &path)
{
    RASTERFALLBACK(DRAW_PATH, path, VOID_ARG(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPath(path);
}

void QDirectFBPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
    RASTERFALLBACK(DRAW_POINTS, pointCount, VOID_ARG(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPoints(points, pointCount);
}

void QDirectFBPaintEngine::drawPoints(const QPoint *points, int pointCount)
{
    RASTERFALLBACK(DRAW_POINTS, pointCount, VOID_ARG(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPoints(points, pointCount);
}

void QDirectFBPaintEngine::drawEllipse(const QRectF &rect)
{
    RASTERFALLBACK(DRAW_ELLIPSE, rect, VOID_ARG(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawEllipse(rect);
}

void QDirectFBPaintEngine::drawPolygon(const QPointF *points, int pointCount,
                                       PolygonDrawMode mode)
{
    RASTERFALLBACK(DRAW_POLYGON, pointCount, mode, VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPolygon(points, pointCount, mode);
}

void QDirectFBPaintEngine::drawPolygon(const QPoint *points, int pointCount,
                                       PolygonDrawMode mode)
{
    RASTERFALLBACK(DRAW_POLYGON, pointCount, mode, VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPolygon(points, pointCount, mode);
}

void QDirectFBPaintEngine::drawTextItem(const QPointF &p,
                                        const QTextItem &textItem)
{
    RASTERFALLBACK(DRAW_TEXT, p, textItem.text(), VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawTextItem(p, textItem);
}

void QDirectFBPaintEngine::fill(const QVectorPath &path, const QBrush &brush)
{
    RASTERFALLBACK(FILL_PATH, path, brush, VOID_ARG());
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::fill(path, brush);
}


void QDirectFBPaintEngine::fillRect(const QRectF &rect, const QBrush &brush)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (!d->unsupportedCompositionMode && d->dfbCanHandleClip(rect) && !d->matrixRotShear) {
        switch (brush.style()) {
        case Qt::SolidPattern: {
            d->unlock();
            d->setDFBColor(brush.color());
            const QRect r = d->transform.mapRect(rect).toRect();
            d->surface->FillRectangle(d->surface, r.x(), r.y(),
                                      r.width(), r.height());
            return; }
        case Qt::TexturePattern:
            if (state()->brushOrigin == QPointF() && brush.transform().isIdentity()) {
                //could handle certain types of brush.transform() E.g. scale
                d->unlock();
                d->drawTiledPixmap(rect, brush.texture());
                return;
            }
            break;
        default:
            break;
        }
    }
    RASTERFALLBACK(FILL_RECT, rect, brush, VOID_ARG());
    d->lock();
    QRasterPaintEngine::fillRect(rect, brush);
}

void QDirectFBPaintEngine::fillRect(const QRectF &rect, const QColor &color)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (d->unsupportedCompositionMode || !d->dfbCanHandleClip() || d->matrixRotShear) {
        RASTERFALLBACK(FILL_RECT, rect, color, VOID_ARG());
        d->lock();
        QRasterPaintEngine::fillRect(rect, color);
    } else {
        d->unlock();
        d->setDFBColor(color);
        const QRect r = d->transform.mapRect(rect).toRect();
        d->surface->FillRectangle(d->surface, r.x(), r.y(),
                                  r.width(), r.height());
    }
}

void QDirectFBPaintEngine::drawColorSpans(const QSpan *spans, int count,
                                          uint color)
{
    Q_D(QDirectFBPaintEngine);
    color = INV_PREMUL(color);

    QVarLengthArray<DFBRegion> lines(count);
    int j = 0;
    for (int i = 0; i < count; ++i) {
        if (spans[i].coverage == 255) {
            lines[j].x1 = spans[i].x;
            lines[j].y1 = spans[i].y;
            lines[j].x2 = spans[i].x + spans[i].len - 1;
            lines[j].y2 = spans[i].y;
            ++j;
        } else {
            DFBSpan span = { spans[i].x, spans[i].len };
            uint c = BYTE_MUL(color, spans[i].coverage);
            // ### how does this play with setDFBColor
            d->surface->SetColor(d->surface,
                                 qRed(c), qGreen(c), qBlue(c), qAlpha(c));
            d->surface->FillSpans(d->surface, spans[i].y, &span, 1);
        }
    }
    if (j > 0) {
        d->surface->SetColor(d->surface,
                             qRed(color), qGreen(color), qBlue(color),
                             qAlpha(color));
        d->surface->DrawLines(d->surface, lines.data(), j);
    }
}

void QDirectFBPaintEngine::drawBufferSpan(const uint *buffer, int bufsize,
                                          int x, int y, int length,
                                          uint const_alpha)
{
    Q_D(QDirectFBPaintEngine);
    IDirectFBSurface *src = d->surfaceCache->getSurface(buffer, bufsize);
    // ### how does this play with setDFBColor
    src->SetColor(src, 0, 0, 0, const_alpha);
    const DFBRectangle rect = { 0, 0, length, 1 };
    d->surface->Blit(d->surface, src, &rect, x, y);
}

#ifdef QT_DIRECTFB_IMAGECACHE
static void cachedImageCleanupHook(qint64 key)
{
    delete imageCache.take(key);
}
void QDirectFBPaintEngine::initImageCache(int size)
{
    Q_ASSERT(size >= 0);
    imageCache.setMaxCost(size);
    typedef void (*_qt_image_cleanup_hook_64)(qint64);
    extern Q_GUI_EXPORT _qt_image_cleanup_hook_64 qt_image_cleanup_hook_64;
    qt_image_cleanup_hook_64 = ::cachedImageCleanupHook;
}

#endif // QT_DIRECTFB_IMAGECACHE

// ---- QDirectFBPaintEnginePrivate ----


QDirectFBPaintEnginePrivate::QDirectFBPaintEnginePrivate(QDirectFBPaintEngine *p)
    : surface(0), antialiased(false), simplePen(false),
      matrixRotShear(false), scale(NoScale), lastLockedHeight(-1),
      fbWidth(-1), fbHeight(-1), opacity(255), dirtyClip(true),
      dfbHandledClip(false), dfbDevice(0), lockedMemory(0),
      unsupportedCompositionMode(false), q(p)
{
    fb = QDirectFBScreen::instance()->dfb();
    ignoreSystemClip = QDirectFBScreen::instance()->directFBFlags() & QDirectFBScreen::IgnoreSystemClip;
    surfaceCache = new SurfaceCache;
}

QDirectFBPaintEnginePrivate::~QDirectFBPaintEnginePrivate()
{
    delete surfaceCache;
}

bool QDirectFBPaintEnginePrivate::dfbCanHandleClip(const QRect &rect) const
{
    // TODO: Check to see if DirectFB can handle the clip for the given rect
    return dfbHandledClip;
}

bool QDirectFBPaintEnginePrivate::dfbCanHandleClip(const QRectF &rect) const
{
    // TODO: Check to see if DirectFB can handle the clip for the given rect
    return dfbHandledClip;
}

bool QDirectFBPaintEnginePrivate::dfbCanHandleClip() const
{
    return dfbHandledClip;
}

bool QDirectFBPaintEnginePrivate::isSimpleBrush(const QBrush &brush) const
{
    return (brush.style() == Qt::NoBrush) || (brush.style() == Qt::SolidPattern && !antialiased);
}

void QDirectFBPaintEnginePrivate::lock()
{
    // We will potentially get a new pointer to the buffer after a
    // lock so we need to call the base implementation of prepare so
    // it updates its rasterBuffer to point to the new buffer address.
    Q_ASSERT(dfbDevice);
    if (dfbDevice->lockFlags() != (DSLF_WRITE|DSLF_READ)
        || dfbDevice->height() != lastLockedHeight
        || dfbDevice->memory() != lockedMemory) {
        prepare(dfbDevice);
        lastLockedHeight = dfbDevice->height();
        lockedMemory = dfbDevice->memory();
    }
}

void QDirectFBPaintEnginePrivate::unlock()
{
    Q_ASSERT(dfbDevice);
    dfbDevice->unlockDirectFB();
    lockedMemory = 0;
}

void QDirectFBPaintEnginePrivate::setTransform(const QTransform &m)
{
    transform = m;
    matrixRotShear = (transform.m12() != 0 || transform.m21() != 0);
    if (qMin(transform.m11(), transform.m22()) < 0) {
        scale = NegativeScale;
    } else if (transform.m11() != 1 || transform.m22() != 1) {
        scale = Scaled;
    } else {
        scale = NoScale;
    }
}

void QDirectFBPaintEnginePrivate::begin(QPaintDevice *device)
{
    lastLockedHeight = -1;
    if (device->devType() == QInternal::CustomRaster)
        dfbDevice = static_cast<QDirectFBPaintDevice*>(device);
    else if (device->devType() == QInternal::Pixmap) {
        QPixmapData *data = static_cast<QPixmap*>(device)->pixmapData();
        Q_ASSERT(data->classId() == QPixmapData::DirectFBClass);
        QDirectFBPixmapData* dfbPixmapData = static_cast<QDirectFBPixmapData*>(data);
        dfbDevice = static_cast<QDirectFBPaintDevice*>(dfbPixmapData);
    }

    if (dfbDevice)
        surface = dfbDevice->directFBSurface();

    if (!surface) {
        qFatal("QDirectFBPaintEngine used on an invalid device: 0x%x",
               device->devType());
    }
    lockedMemory = 0;

    surface->GetSize(surface, &fbWidth, &fbHeight);

    setTransform(QTransform());
    antialiased = false;
    setOpacity(255);
    setCompositionMode(q->state()->compositionMode());
    dirtyClip = true;
    setPen(q->state()->pen);
    setDFBColor(pen.color());
}

void QDirectFBPaintEnginePrivate::end()
{
    lockedMemory = 0;
    dfbDevice = 0;
    surface->ReleaseSource(surface);
    surface->SetClip(surface, NULL);
    surface = 0;
}

void QDirectFBPaintEnginePrivate::setPen(const QPen &p)
{
    pen = p;
    simplePen = (pen.style() == Qt::NoPen) ||
                (pen.style() == Qt::SolidLine
                 && !antialiased
                 && (pen.brush().style() == Qt::SolidPattern)
                 && (pen.widthF() <= 1 && scale != NoScale));
}

void QDirectFBPaintEnginePrivate::setCompositionMode(QPainter::CompositionMode mode)
{
    unsupportedCompositionMode = (mode != QPainter::CompositionMode_SourceOver);
}


void QDirectFBPaintEnginePrivate::setOpacity(quint8 op)
{
    opacity = op;
}

void QDirectFBPaintEnginePrivate::setRenderHints(QPainter::RenderHints hints)
{
    const bool old = antialiased;
    antialiased = bool(hints & QPainter::Antialiasing);
    if (old != antialiased) {
        setPen(q->state()->pen);
    }
}

void QDirectFBPaintEnginePrivate::prepareForBlit(bool alpha)
{
    quint32 blittingFlags = alpha ? DSBLIT_BLEND_ALPHACHANNEL : DSBLIT_NOFX;
    if (opacity != 255) {
        blittingFlags |= DSBLIT_BLEND_COLORALPHA;
    }
    surface->SetColor(surface, 0xff, 0xff, 0xff, opacity);
    surface->SetBlittingFlags(surface, DFBSurfaceBlittingFlags(blittingFlags));
}

void QDirectFBPaintEnginePrivate::setDFBColor(const QColor &color)
{
    Q_ASSERT(surface);
    const quint8 alpha = (opacity == 255 ?
                          color.alpha() : ALPHA_MUL(color.alpha(), opacity));
    surface->SetColor(surface, color.red(), color.green(), color.blue(), alpha);
    surface->SetPorterDuff(surface, DSPD_NONE);
    surface->SetDrawingFlags(surface, alpha == 255 ? DSDRAW_NOFX : DSDRAW_BLEND);
}

void QDirectFBPaintEnginePrivate::drawLines(const QLine *lines, int n)
{
    for (int i = 0; i < n; ++i) {
        const QLine l = transform.map(lines[i]);
        surface->DrawLine(surface, l.x1(), l.y1(), l.x2(), l.y2());
    }
}

void QDirectFBPaintEnginePrivate::drawLines(const QLineF *lines, int n)
{
    for (int i = 0; i < n; ++i) {
        const QLine l = transform.map(lines[i]).toLine();
        surface->DrawLine(surface, l.x1(), l.y1(), l.x2(), l.y2());
    }
}

void QDirectFBPaintEnginePrivate::fillRegion(const QRegion &region)
{
    Q_ASSERT(isSimpleBrush(q->state()->brush));
    setDFBColor(q->state()->brush.color());
    const QVector<QRect> rects = region.rects();
    const int n = rects.size();
    fillRects(rects.constData(), n);
}

void QDirectFBPaintEnginePrivate::fillRects(const QRect *rects, int n)
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]);
        surface->FillRectangle(surface, r.x(), r.y(),
                               r.width(), r.height());
    }
}

void QDirectFBPaintEnginePrivate::fillRects(const QRectF *rects, int n)
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]).toRect();
        surface->FillRectangle(surface, r.x(), r.y(),
                               r.width(), r.height());
    }
}

void QDirectFBPaintEnginePrivate::drawRects(const QRect *rects, int n)
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]);
        surface->DrawRectangle(surface, r.x(), r.y(),
                               r.width() + 1, r.height() + 1);
    }
}

void QDirectFBPaintEnginePrivate::drawRects(const QRectF *rects, int n)
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]).toRect();
        surface->DrawRectangle(surface, r.x(), r.y(),
                               r.width() + 1, r.height() + 1);
    }
}

IDirectFBSurface *QDirectFBPaintEnginePrivate::getSurface(const QImage &img, bool *release)
{
#ifndef QT_DIRECTFB_IMAGECACHE
    *release = true;
    return QDirectFBScreen::instance()->createDFBSurface(img, QDirectFBScreen::DontTrackSurface);
#else
    const qint64 key = img.cacheKey();
    *release = false;
    if (imageCache.contains(key)) {
        return imageCache[key]->surface;
    }

    const int cost = cacheCost(img);
    const bool cache = cost <= imageCache.maxCost();
    QDirectFBScreen *screen = QDirectFBScreen::instance();
    const QImage::Format format = (img.format() == screen->alphaPixmapFormat() || QDirectFBPixmapData::hasAlphaChannel(img)
                                   ? screen->alphaPixmapFormat() : screen->pixelFormat());

    IDirectFBSurface *surface = screen->copyToDFBSurface(img, format,
                                                         cache
                                                         ? QDirectFBScreen::TrackSurface
                                                         : QDirectFBScreen::DontTrackSurface);
    if (cache) {
        CachedImage *cachedImage = new CachedImage;
        const_cast<QImage&>(img).data_ptr()->is_cached = true;
        cachedImage->surface = surface;
        imageCache.insert(key, cachedImage, cost);
    } else {
        *release = true;
    }
    return surface;
#endif
}


void QDirectFBPaintEnginePrivate::blit(const QRectF &dest, IDirectFBSurface *s, const QRectF &src)
{
    const QRect sr = src.toRect();
    const QRect dr = transform.mapRect(dest).toRect();
    const DFBRectangle sRect = { sr.x(), sr.y(), sr.width(), sr.height() };
    DFBResult result;

    if (dr.size() == sr.size()) {
        result = surface->Blit(surface, s, &sRect, dr.x(), dr.y());
    } else {
        const DFBRectangle dRect = { dr.x(), dr.y(), dr.width(), dr.height() };
        result = surface->StretchBlit(surface, s, &sRect, &dRect);
    }
    if (result != DFB_OK)
        DirectFBError("QDirectFBPaintEngine::drawPixmap()", result);
}

void QDirectFBPaintEnginePrivate::drawTiledPixmap(const QRectF &dest,
                                                  const QPixmap &pixmap)
{
    prepareForBlit(pixmap.hasAlphaChannel());
    QPixmapData *data = pixmap.pixmapData();
    Q_ASSERT(data->classId() == QPixmapData::DirectFBClass);
    QDirectFBPixmapData *dfbData = static_cast<QDirectFBPixmapData*>(data);
    IDirectFBSurface *s = dfbData->directFBSurface();
    const QRect dr = transform.mapRect(dest).toRect();
    DFBResult result = DFB_OK;

    if (scale == NoScale && dr == QRect(0, 0, fbWidth, fbHeight)) {
        result = surface->TileBlit(surface, s, 0, 0, 0);
    } else if (scale == NoScale) {
        const int dx = pixmap.width();
        const int dy = pixmap.height();
        const DFBRectangle rect = { 0, 0, dx, dy };
        QVarLengthArray<DFBRectangle> rects;
        QVarLengthArray<DFBPoint> points;

        for (int y = dr.y(); y <= dr.bottom(); y += dy) {
            for (int x = dr.x(); x <= dr.right(); x += dx) {
                rects.append(rect);
                const DFBPoint point = { x, y };
                points.append(point);
            }
        }
        result = surface->BatchBlit(surface, s, rects.constData(),
                                    points.constData(), points.size());
    } else {
        const QRect sr = transform.mapRect(QRect(0, 0, pixmap.width(), pixmap.height()));
        const int dx = sr.width();
        const int dy = sr.height();
        const DFBRectangle sRect = { 0, 0, dx, dy };

        for (int y = dr.y(); y <= dr.bottom(); y += dy) {
            for (int x = dr.x(); x <= dr.right(); x += dx) {
                const DFBRectangle dRect = { x, y, dx, dy };
                result = surface->StretchBlit(surface, s, &sRect, &dRect);
                if (result != DFB_OK) {
                    y = dr.bottom() + 1;
                    break;
                }
            }
        }
    }

    if (result != DFB_OK)
        DirectFBError("QDirectFBPaintEngine::drawTiledPixmap()", result);
}

void QDirectFBPaintEnginePrivate::updateClip()
{
    if (!dirtyClip)
        return;

    const QClipData *clipData = clip();
    if (!clipData || !clipData->enabled) {
        surface->SetClip(surface, NULL);
        dfbHandledClip = true;
    } else if (clipData->hasRectClip) {
        const DFBRegion r = {
            clipData->clipRect.x(),
            clipData->clipRect.y(),
            clipData->clipRect.x() + clipData->clipRect.width(),
            clipData->clipRect.y() + clipData->clipRect.height()
        };
        surface->SetClip(surface, &r);
        dfbHandledClip = true;
    } else if (clipData->hasRegionClip && ignoreSystemClip && clipData->clipRegion == systemClip) {
        dfbHandledClip = true;
    } else {
        dfbHandledClip = false;
    }

    dirtyClip = false;
}

void QDirectFBPaintEnginePrivate::systemStateChanged()
{
    dirtyClip = true;
    QRasterPaintEnginePrivate::systemStateChanged();
}

#endif // QT_NO_DIRECTFB
