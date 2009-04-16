/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdirectfbpaintengine.h"

#ifndef QT_NO_DIRECTFB

#include "qdirectfbsurface.h"
#include "qdirectfbscreen.h"
#include "qdirectfbpixmap.h"
#include <directfb.h>
#include <qtransform.h>
#include <qvarlengtharray.h>
#include <qcache.h>
#include <qmath.h>
#include <private/qpixmapdata_p.h>
#include <private/qpixmap_raster_p.h>

static inline uint ALPHA_MUL(uint x, uint a)
{
    uint t = x * a;
    t = ((t + (t >> 8) + 0x80) >> 8) & 0xff;
    return t;
}

class SurfaceCache
{
public:
    SurfaceCache();
    ~SurfaceCache();

    inline IDirectFBSurface *getSurface(const uint *buffer, int size);
    inline void clear();

private:
    IDirectFBSurface *surface;
    uint *buffer;
    int bufsize;
};

SurfaceCache::SurfaceCache()
    : surface(0), buffer(0), bufsize(0)
{
}

class CachedImage
{
public:
    CachedImage(const QImage &image);
    ~CachedImage();

    IDirectFBSurface *surface() { return s; }

private:
    IDirectFBSurface *s;
};

CachedImage::CachedImage(const QImage &image)
    : s(0)
{
    IDirectFBSurface *tmpSurface = 0;
    DFBSurfaceDescription description;
    description = QDirectFBScreen::getSurfaceDescription(image);
    QDirectFBScreen* screen = QDirectFBScreen::instance();

    tmpSurface = screen->createDFBSurface(&description, QDirectFBScreen::TrackSurface);
    if (!tmpSurface) {
        qWarning("CachedImage CreateSurface failed!");
        return;
    }

#ifndef QT_NO_DIRECTFB_PALETTE
    QDirectFBScreen::setSurfaceColorTable(tmpSurface, image);
#endif

    description.flags = DFBSurfaceDescriptionFlags(description.flags & ~DSDESC_PREALLOCATED);

    s = screen->createDFBSurface(&description, QDirectFBScreen::TrackSurface);
    if (!s)
        qWarning("QDirectFBPaintEngine failed caching image");

#ifndef QT_NO_DIRECTFB_PALETTE
    QDirectFBScreen::setSurfaceColorTable(s, image);
#endif

    if (s) {
        s->SetBlittingFlags(s, DSBLIT_NOFX);
        s->Blit(s, tmpSurface, 0, 0, 0);
        s->ReleaseSource(s);
    }
    if (tmpSurface)
        screen->releaseDFBSurface(tmpSurface);
}

CachedImage::~CachedImage()
{
    if (s && QDirectFBScreen::instance())
        QDirectFBScreen::instance()->releaseDFBSurface(s);
}

static QCache<qint64, CachedImage> imageCache(4*1024*1024); // 4 MB

IDirectFBSurface* SurfaceCache::getSurface(const uint *buf, int size)
{
    if (buffer == buf && bufsize == size)
        return surface;

    clear();

    DFBSurfaceDescription description;
    description = QDirectFBScreen::getSurfaceDescription(buf, size);

    surface = QDirectFBScreen::instance()->createDFBSurface(&description, QDirectFBScreen::TrackSurface);
    if (!surface)
        qWarning("QDirectFBPaintEngine: SurfaceCache: Unable to create surface");

    buffer = const_cast<uint*>(buf);
    bufsize = size;

    return surface;
}

void SurfaceCache::clear()
{
    if (surface)
        QDirectFBScreen::instance()->releaseDFBSurface(surface);
    surface = 0;
    buffer = 0;
    bufsize = 0;
}

SurfaceCache::~SurfaceCache()
{
    clear();
}

class QDirectFBPaintEnginePrivate : public QRasterPaintEnginePrivate
{
public:
    QDirectFBPaintEnginePrivate(QDirectFBPaintEngine *p);
    ~QDirectFBPaintEnginePrivate();

    IDirectFBSurface *surface;

    QPen pen;
    QBrush brush;

    bool antialiased;
    bool forceRasterPrimitives;

    bool simplePen;
    bool simpleBrush;

    bool matrixRotShear;
    bool matrixScale;

    void setTransform(const QTransform &m);
    void setPen(const QPen &pen);
    void setBrush(const QBrush &brush);
    void setCompositionMode(QPainter::CompositionMode mode);
    void setOpacity(quint8 value);
    void setRenderHints(QPainter::RenderHints hints);

    inline void setDFBColor(const QColor &color);

    inline void lock();
    inline void unlock();

    inline bool dfbCanHandleClip(const QRect &rect) const;
    inline bool dfbCanHandleClip(const QRectF &rect) const;
    inline bool dfbCanHandleClip() const;

    void drawLines(const QLine *lines, int count) const;
    void drawLines(const QLineF *lines, int count) const;

    void fillRegion(const QRegion &r) const;
    void fillRects(const QRect *rects, int count) const;
    void drawRects(const QRect *rects, int count) const;
    void fillRects(const QRectF *rects, int count) const;
    void drawRects(const QRectF *rects, int count) const;

    void drawPixmap(const QRectF &dest,
                    const QPixmap &pixmap, const QRectF &src);
    void drawTiledPixmap(const QRectF &dest, const QPixmap &pixmap);
    void drawImage(const QRectF &dest, const QImage &image, const QRectF &src);

    inline void updateClip();
    inline void setClipDirty();
    void systemStateChanged();

    void begin(QPaintDevice *device);
    void end();

    void prepareForBlit(bool alpha);
    SurfaceCache *surfaceCache;
    QTransform transform;
    int lastLockedHeight;
private:
    IDirectFB *fb;
    DFBSurfaceDescription fbDescription;
    int fbWidth;
    int fbHeight;

    quint8 opacity;

    quint32 drawFlagsFromCompositionMode, blitFlagsFromCompositionMode;
    DFBSurfacePorterDuffRule porterDuffRule;

    bool dirtyClip;
    bool dfbHandledClip;
    QDirectFBPaintDevice *dfbDevice;

    QDirectFBPaintEngine *q;
};

QDirectFBPaintEnginePrivate::QDirectFBPaintEnginePrivate(QDirectFBPaintEngine *p)
    : surface(0), antialiased(false), forceRasterPrimitives(false), simplePen(false),
      simpleBrush(false), matrixRotShear(false), matrixScale(false), lastLockedHeight(-1),
      fbWidth(-1), fbHeight(-1), opacity(255), drawFlagsFromCompositionMode(0),
      blitFlagsFromCompositionMode(0), porterDuffRule(DSPD_SRC_OVER), dirtyClip(true),
      dfbHandledClip(false), dfbDevice(0), q(p)
{
    fb = QDirectFBScreen::instance()->dfb();
    surfaceCache = new SurfaceCache;
    static int cacheLimit = qgetenv("QT_DIRECTFB_IMAGECACHE").toInt();
    if (cacheLimit > 0)
        imageCache.setMaxCost(cacheLimit * 1024);
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

void QDirectFBPaintEnginePrivate::setClipDirty()
{
    dirtyClip = true;
}

void QDirectFBPaintEnginePrivate::lock()
{
    // We will potentially get a new pointer to the buffer after a
    // lock so we need to call the base implementation of prepare so
    // it updates its rasterBuffer to point to the new buffer address.
    lastLockedHeight = dfbDevice->height();

    Q_ASSERT(dfbDevice);
    prepare(dfbDevice);
}

void QDirectFBPaintEnginePrivate::unlock()
{
    Q_ASSERT(dfbDevice);
    dfbDevice->unlockDirectFB();
}

void QDirectFBPaintEnginePrivate::setTransform(const QTransform &m)
{
    transform = m;
    matrixRotShear = (transform.m12() != 0 || transform.m21() != 0);
    matrixScale = (transform.m11() != 1 || transform.m22() != 1);
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
    forceRasterPrimitives = dfbDevice->forceRasterPrimitives();

    surface->GetSize(surface, &fbWidth, &fbHeight);

    setTransform(QTransform());
    antialiased = false;
    opacity = 255;
    setCompositionMode(q->state()->compositionMode());
    dirtyClip = true;
    setPen(q->state()->pen);
    setDFBColor(pen.color());
}

void QDirectFBPaintEnginePrivate::end()
{
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
                 && (pen.widthF() <= 1 && !matrixScale));
}

void QDirectFBPaintEnginePrivate::setBrush(const QBrush &b)
{
    brush = b;
    simpleBrush = (brush.style() == Qt::NoBrush) ||
                  (brush.style() == Qt::SolidPattern && !antialiased);
}

void QDirectFBPaintEnginePrivate::setCompositionMode(QPainter::CompositionMode mode)
{
    blitFlagsFromCompositionMode = DSBLIT_NOFX;
    drawFlagsFromCompositionMode = DSDRAW_NOFX;

    bool blend = true;
    switch (mode) {
    case QPainter::CompositionMode_SourceOver:
        porterDuffRule = DSPD_SRC_OVER;
        break;
    case QPainter::CompositionMode_DestinationOver:
        porterDuffRule = DSPD_DST_OVER;
        break;
    case QPainter::CompositionMode_Clear:
        porterDuffRule = DSPD_CLEAR;
        blend = false;
        break;
    case QPainter::CompositionMode_Source:
        porterDuffRule = DSPD_SRC;
        blend = false;
        break;
    case QPainter::CompositionMode_Destination:
        porterDuffRule = DSPD_NONE; // ### need to double check this
        blend = false;
        return;
    case QPainter::CompositionMode_SourceIn:
        porterDuffRule = DSPD_SRC_IN;
        break;
    case QPainter::CompositionMode_DestinationIn:
        porterDuffRule = DSPD_DST_IN;
        break;
    case QPainter::CompositionMode_SourceOut:
        porterDuffRule = DSPD_SRC_OUT;
        break;
    case QPainter::CompositionMode_DestinationOut:
        porterDuffRule = DSPD_DST_OUT;
        break;
    case QPainter::CompositionMode_Xor:
        porterDuffRule = DSPD_XOR;
        blitFlagsFromCompositionMode |= DSBLIT_XOR;
        drawFlagsFromCompositionMode |= DSDRAW_XOR;
        break;
//     case QPainter::CompositionMode_Plus: // ???
//         porterDuffRule = DSPD_ADD;
//         break;
    default:
        qWarning("QDirectFBPaintEnginePrivate::setCompositionMode(): "
                 "mode %d not implemented", mode);
        return;
    }
    // intentially not comparing with current porterDuffRule. surface might have changed.
    if (blend) {
        blitFlagsFromCompositionMode |= DSBLIT_BLEND_ALPHACHANNEL;
        drawFlagsFromCompositionMode |= DSDRAW_BLEND;
    }
    if (opacity != 255) {
        setOpacity(opacity);
    }
}

void QDirectFBPaintEnginePrivate::setOpacity(quint8 op)
{
    opacity = op;
    if (opacity == 255) {
        blitFlagsFromCompositionMode &= ~DSBLIT_BLEND_COLORALPHA;
    } else {
        blitFlagsFromCompositionMode |= DSBLIT_BLEND_COLORALPHA;
    }
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
    quint32 blittingFlags = blitFlagsFromCompositionMode;
    if (alpha) {
        surface->SetPorterDuff(surface,
                               (blittingFlags & DSBLIT_BLEND_COLORALPHA)
                               ? DSPD_NONE
                               : porterDuffRule);
    } else {
        blittingFlags &= ~DSBLIT_BLEND_ALPHACHANNEL;
        surface->SetPorterDuff(surface, DSPD_NONE);
    }
    surface->SetColor(surface, 0xff, 0xff, 0xff, opacity);
    surface->SetBlittingFlags(surface, DFBSurfaceBlittingFlags(blittingFlags));
}

void QDirectFBPaintEnginePrivate::setDFBColor(const QColor &color)
{
    Q_ASSERT(surface);
    const quint8 alpha = (opacity == 255 ?
                          color.alpha() : ALPHA_MUL(color.alpha(), opacity));
    surface->SetColor(surface,
                      color.red(), color.green(), color.blue(), alpha);
    quint32 drawingFlags = drawFlagsFromCompositionMode;
    if (alpha == 255) {
        drawingFlags &= ~DSDRAW_BLEND;
    }
    surface->SetPorterDuff(surface, DSPD_NONE);
    // PorterDuff messes up alpha values for primitives
    surface->SetDrawingFlags(surface, DFBSurfaceDrawingFlags(drawingFlags));
}

void QDirectFBPaintEnginePrivate::drawLines(const QLine *lines, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QLine l = transform.map(lines[i]);
        surface->DrawLine(surface, l.x1(), l.y1(), l.x2(), l.y2());
    }
}

void QDirectFBPaintEnginePrivate::drawLines(const QLineF *lines, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QLine l = transform.map(lines[i]).toLine();
        surface->DrawLine(surface, l.x1(), l.y1(), l.x2(), l.y2());
    }
}

void QDirectFBPaintEnginePrivate::fillRegion(const QRegion &region) const
{
    const QVector<QRect> rects = region.rects();
    const int n = rects.size();
    fillRects(rects.constData(), n);
}

void QDirectFBPaintEnginePrivate::fillRects(const QRect *rects, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]);
        surface->FillRectangle(surface, r.x(), r.y(),
                               r.width(), r.height());
    }
}

void QDirectFBPaintEnginePrivate::fillRects(const QRectF *rects, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]).toRect();
        surface->FillRectangle(surface, r.x(), r.y(),
                               r.width(), r.height());
    }
}

void QDirectFBPaintEnginePrivate::drawRects(const QRect *rects, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]);
        surface->DrawRectangle(surface, r.x(), r.y(),
                               r.width() + 1, r.height() + 1);
    }
}

void QDirectFBPaintEnginePrivate::drawRects(const QRectF *rects, int n) const
{
    for (int i = 0; i < n; ++i) {
        const QRect r = transform.mapRect(rects[i]).toRect();
        surface->DrawRectangle(surface, r.x(), r.y(),
                               r.width() + 1, r.height() + 1);
    }
}

void QDirectFBPaintEnginePrivate::drawPixmap(const QRectF &dest,
                                             const QPixmap &pixmap,
                                             const QRectF &src)
{
    prepareForBlit(pixmap.hasAlphaChannel());
    QPixmapData *data = pixmap.pixmapData();
    Q_ASSERT(data->classId() == QPixmapData::DirectFBClass);
    QDirectFBPixmapData *dfbData = static_cast<QDirectFBPixmapData*>(data);
    IDirectFBSurface *s = dfbData->directFBSurface();
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

    if (!matrixScale && dr == QRect(0, 0, fbWidth, fbHeight)) {
        result = surface->TileBlit(surface, s, 0, 0, 0);
    } else if (!matrixScale) {
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

void QDirectFBPaintEnginePrivate::drawImage(const QRectF &dest,
                                            const QImage &image,
                                            const QRectF &src)
{
    Q_ASSERT(QDirectFBScreen::getSurfacePixelFormat(image.format()) != DSPF_UNKNOWN);
    CachedImage *img = imageCache[image.cacheKey()];
    IDirectFBSurface *imgSurface = 0;
    bool doRelease = false;

    if (img) {
        imgSurface = img->surface();
    } else {
        const int cost = image.width() * image.height() * image.depth() / 8;
        if (cost <= imageCache.maxCost()) {
            img = new CachedImage(image);
            imgSurface = img->surface();
            if (imgSurface) {
                imageCache.insert(image.cacheKey(), img, cost);
            } else {
                delete img;
                img = 0;
            }
        }

        if (!imgSurface) {
            DFBSurfaceDescription description;

            description = QDirectFBScreen::getSurfaceDescription(image);
            imgSurface = QDirectFBScreen::instance()->createDFBSurface(&description,
                                                                       QDirectFBScreen::DontTrackSurface);
            if (!imgSurface) {
                qWarning("QDirectFBPaintEnginePrivate::drawImage");
                return;
            }

#ifndef QT_NO_DIRECTFB_PALETTE
            QDirectFBScreen::setSurfaceColorTable(surface, image);
#endif
            doRelease = (imgSurface != 0);
        }
    }

    const QRect sr = src.toRect();
    const QRect dr = transform.mapRect(dest).toRect();
    const DFBRectangle sRect = { sr.x(), sr.y(), sr.width(), sr.height() };

    prepareForBlit(image.hasAlphaChannel());
    if (dr.size() == sr.size()) {
        surface->Blit(surface, imgSurface, &sRect, dr.x(), dr.y());
    } else {
        const DFBRectangle dRect = { dr.x(), dr.y(),
                                     dr.width(), dr.height() };
        surface->StretchBlit(surface, imgSurface, &sRect, &dRect);
    }
    if (doRelease) {
        surface->ReleaseSource(surface);
        imgSurface->Release(imgSurface);
    }
}

void QDirectFBPaintEnginePrivate::updateClip()
{
    if (!dirtyClip)
        return;

    if (!clip() || !clip()->enabled) {
        surface->SetClip(surface, NULL);
        dfbHandledClip = true;
    }
    else if (clip()->hasRectClip) {
        const DFBRegion r = {
            clip()->clipRect.x(),
            clip()->clipRect.y(),
            clip()->clipRect.x() + clip()->clipRect.width(),
            clip()->clipRect.y() + clip()->clipRect.height()
        };
        surface->SetClip(surface, &r);

        dfbHandledClip = true;
    }
    else
        dfbHandledClip = false;

    dirtyClip = false;
}

void QDirectFBPaintEnginePrivate::systemStateChanged()
{
    setClipDirty();
    QRasterPaintEnginePrivate::systemStateChanged();
}

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
    d->setClipDirty();
    QRasterPaintEngine::clipEnabledChanged();
}

void QDirectFBPaintEngine::penChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setPen(state()->pen);

    QRasterPaintEngine::penChanged();
}

void QDirectFBPaintEngine::brushChanged()
{
    Q_D(QDirectFBPaintEngine);
    d->setBrush(state()->brush);

    QRasterPaintEngine::brushChanged();
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
    const bool old = d->matrixScale;
    d->setTransform(state()->transform());
    if (d->matrixScale != old) {
        d->setPen(state()->pen);
    }
    QRasterPaintEngine::transformChanged();
}

void QDirectFBPaintEngine::setState(QPainterState *s)
{
    Q_D(QDirectFBPaintEngine);
    QRasterPaintEngine::setState(s);
    d->setClipDirty();
    d->setPen(state()->pen);
    d->setBrush(state()->brush);
    d->setOpacity(quint8(state()->opacity * 255));
    d->setCompositionMode(state()->compositionMode());
    d->setTransform(state()->transform());
}

void QDirectFBPaintEngine::clip(const QVectorPath &path, Qt::ClipOperation op)
{
    Q_D(QDirectFBPaintEngine);
    d->setClipDirty();
    const QPoint bottom = d->transform.map(QPoint(0, path.controlPointRect().y2));
    if (bottom.y() >= d->lastLockedHeight)
        d->lock();
    QRasterPaintEngine::clip(path, op);
}

void QDirectFBPaintEngine::clip(const QRect &rect, Qt::ClipOperation op)
{
    Q_D(QDirectFBPaintEngine);
    d->setClipDirty();
    if (!d->clip()->hasRectClip && d->clip()->enabled) {
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
    if (!d->dfbCanHandleClip() || d->matrixRotShear || !d->simpleBrush
        || !d->simplePen || d->forceRasterPrimitives) {
        d->lock();
        QRasterPaintEngine::drawRects(rects, rectCount);
        return;
    }

    d->unlock();

    if (d->brush != Qt::NoBrush) {
        d->setDFBColor(d->brush.color());
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
    if (!d->dfbCanHandleClip() || d->matrixRotShear || !d->simpleBrush
        || !d->simplePen || d->forceRasterPrimitives) {
        d->lock();
        QRasterPaintEngine::drawRects(rects, rectCount);
        return;
    }

    d->unlock();

    if (d->brush != Qt::NoBrush) {
        d->setDFBColor(d->brush.color());
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
    if (!d->simplePen || !d->dfbCanHandleClip() || d->forceRasterPrimitives) {
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
    if (!d->simplePen || !d->dfbCanHandleClip() || d->forceRasterPrimitives) {
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
    Q_UNUSED(flags); // XXX

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    d->updateClip();
    if (!d->dfbCanHandleClip(r) || d->matrixRotShear
        || QDirectFBScreen::getSurfacePixelFormat(image.format()) == DSPF_UNKNOWN)
#endif
    {
        d->lock();
        QRasterPaintEngine::drawImage(r, image, sr, flags);
        return;
    }

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    d->unlock();
    d->drawImage(r, image, sr);
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
        d->lock();
        QRasterPaintEngine::drawPixmap(r, pixmap, sr);
    } else if (!d->dfbCanHandleClip(r) || d->matrixRotShear) {
        const QImage *img = static_cast<QDirectFBPixmapData*>(pixmap.pixmapData())->buffer();
        d->lock();
        QRasterPaintEngine::drawImage(r, *img, sr);
    } else {
        d->unlock();
        d->drawPixmap(r, pixmap, sr);
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
        d->lock();
        QRasterPaintEngine::drawTiledPixmap(r, pixmap, sp);
    } else if (!d->dfbCanHandleClip(r) || d->matrixRotShear || !sp.isNull()) {
        const QImage *img = static_cast<QDirectFBPixmapData*>(pixmap.pixmapData())->buffer();
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
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::stroke(path, pen);
}

void QDirectFBPaintEngine::drawPath(const QPainterPath &path)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPath(path);
}

void QDirectFBPaintEngine::drawPoints(const QPointF *points, int pointCount)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPoints(points, pointCount);
}

void QDirectFBPaintEngine::drawPoints(const QPoint *points, int pointCount)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPoints(points, pointCount);
}

void QDirectFBPaintEngine::drawEllipse(const QRectF &rect)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawEllipse(rect);
}

void QDirectFBPaintEngine::drawPolygon(const QPointF *points, int pointCount,
                                       PolygonDrawMode mode)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPolygon(points, pointCount, mode);
}

void QDirectFBPaintEngine::drawPolygon(const QPoint *points, int pointCount,
                                       PolygonDrawMode mode)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawPolygon(points, pointCount, mode);
}

void QDirectFBPaintEngine::drawTextItem(const QPointF &p,
                                        const QTextItem &textItem)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::drawTextItem(p, textItem);
}

void QDirectFBPaintEngine::fill(const QVectorPath &path, const QBrush &brush)
{
    Q_D(QDirectFBPaintEngine);
    d->lock();
    QRasterPaintEngine::fill(path, brush);
}


void QDirectFBPaintEngine::fillRect(const QRectF &rect, const QBrush &brush)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (d->dfbCanHandleClip(rect) && !d->matrixRotShear) {
        switch (brush.style()) {
        case Qt::SolidPattern: {
            if (d->forceRasterPrimitives)
                break;
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
    d->lock();
    QRasterPaintEngine::fillRect(rect, brush);
}

void QDirectFBPaintEngine::fillRect(const QRectF &rect, const QColor &color)
{
    Q_D(QDirectFBPaintEngine);
    d->updateClip();
    if (!d->dfbCanHandleClip() || d->matrixRotShear || d->forceRasterPrimitives) {
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
    if (d->forceRasterPrimitives) {
        d->lock();
        QRasterPaintEngine::drawColorSpans(spans, count, color);
    } else {
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

#endif // QT_NO_DIRECTFB
