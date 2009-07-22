/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdirectfbwindowsurface.h"
#include "qdirectfbscreen.h"
#include "qdirectfbpaintengine.h"

#include <qwidget.h>
#include <qwindowsystem_qws.h>
#include <qpaintdevice.h>
#include <qvarlengtharray.h>

//#define QT_DIRECTFB_DEBUG_SURFACES 1

QDirectFBWindowSurface::QDirectFBWindowSurface(DFBSurfaceFlipFlags flip, QDirectFBScreen *scr)
    : QDirectFBPaintDevice(scr)
#ifndef QT_NO_DIRECTFB_WM
    , dfbWindow(0)
#endif
    , engine(0)
    , flipFlags(flip)
    , boundingRectFlip(scr->directFBFlags() & QDirectFBScreen::BoundingRectFlip)
{
#ifdef QT_NO_DIRECTFB_WM
    mode = Offscreen;
#else
    mode = Window;
#endif
    setSurfaceFlags(Opaque | Buffered);
#ifdef QT_DIRECTFB_TIMING
    frames = 0;
    timer.start();
#endif
}

QDirectFBWindowSurface::QDirectFBWindowSurface(DFBSurfaceFlipFlags flip, QDirectFBScreen *scr, QWidget *widget)
    : QWSWindowSurface(widget), QDirectFBPaintDevice(scr)
#ifndef QT_NO_DIRECTFB_WM
    , dfbWindow(0)
#endif
    , engine(0)
    , flipFlags(flip)
    , boundingRectFlip(scr->directFBFlags() & QDirectFBScreen::BoundingRectFlip)
{
    if (widget && widget->testAttribute(Qt::WA_PaintOnScreen)) {
        setSurfaceFlags(Opaque | RegionReserved);
        mode = Primary;
    } else {
#ifdef QT_NO_DIRECTFB_WM
        mode = Offscreen;
#else
        mode = Window;
#endif
        setSurfaceFlags(Opaque | Buffered);
    }
#ifdef QT_DIRECTFB_TIMING
    frames = 0;
    timer.start();
#endif
}

QDirectFBWindowSurface::~QDirectFBWindowSurface()
{
}

bool QDirectFBWindowSurface::isValid() const
{
    return true;
}

#ifndef QT_NO_DIRECTFB_WM
void QDirectFBWindowSurface::createWindow()
{
#ifdef QT_NO_DIRECTFB_LAYER
#error QT_NO_DIRECTFB_LAYER requires QT_NO_DIRECTFB_WM
#else
    IDirectFBDisplayLayer *layer = screen->dfbDisplayLayer();
    if (!layer)
        qFatal("QDirectFBWindowSurface: Unable to get primary display layer!");

    DFBWindowDescription description;
    description.caps = DWCAPS_NODECORATION;
    description.flags = DWDESC_CAPS|DWDESC_SURFACE_CAPS|DWDESC_PIXELFORMAT;

    description.surface_caps = DSCAPS_NONE;
    if (screen->directFBFlags() & QDirectFBScreen::VideoOnly)
        description.surface_caps |= DSCAPS_VIDEOONLY;
    const QImage::Format format = screen->pixelFormat();
    description.pixelformat = QDirectFBScreen::getSurfacePixelFormat(format);
    if (QDirectFBScreen::isPremultiplied(format))
        description.surface_caps = DSCAPS_PREMULTIPLIED;

    DFBResult result = layer->CreateWindow(layer, &description, &dfbWindow);
    if (result != DFB_OK)
        DirectFBErrorFatal("QDirectFBWindowSurface::createWindow", result);

    if (dfbSurface)
        dfbSurface->Release(dfbSurface);

    dfbWindow->GetSurface(dfbWindow, &dfbSurface);
#endif
}
#endif // QT_NO_DIRECTFB_WM

#ifndef QT_NO_DIRECTFB_WM
static DFBResult setGeometry(IDirectFBWindow *dfbWindow, const QRect &old, const QRect &rect)
{
    DFBResult result = DFB_OK;
    const bool isMove = old.isEmpty() || rect.topLeft() != old.topLeft();
    const bool isResize = rect.size() != old.size();

#if (Q_DIRECTFB_VERSION >= 0x010000)
    if (isResize && isMove) {
        result = dfbWindow->SetBounds(dfbWindow, rect.x(), rect.y(),
                                      rect.width(), rect.height());
    } else if (isResize) {
        result = dfbWindow->Resize(dfbWindow,
                                   rect.width(), rect.height());
    } else if (isMove) {
        result = dfbWindow->MoveTo(dfbWindow, rect.x(), rect.y());
    }
#else
    if (isResize) {
        result = dfbWindow->Resize(dfbWindow,
                                   rect.width(), rect.height());
    }
    if (isMove) {
        result = dfbWindow->MoveTo(dfbWindow, rect.x(), rect.y());
    }
#endif
    return result;
}
#endif

void QDirectFBWindowSurface::setGeometry(const QRect &rect)
{
    IDirectFBSurface *primarySurface = screen->dfbSurface();
    Q_ASSERT(primarySurface);
    if (rect.isNull()) {
#ifndef QT_NO_DIRECTFB_WM
        if (dfbWindow) {
            dfbWindow->Release(dfbWindow);
            dfbWindow = 0;
        }
#endif
        if (dfbSurface) {
            if (dfbSurface != primarySurface) {
                dfbSurface->Release(dfbSurface);
            }
            dfbSurface = 0;
        }
    } else if (rect != geometry()) {
        const QRect oldRect = geometry();
        DFBResult result = DFB_OK;
        // If we're in a resize, the surface shouldn't be locked
        Q_ASSERT((lockedImage == 0) || (rect.size() == geometry().size()));
        switch (mode) {
        case Primary:
            if (dfbSurface && dfbSurface != primarySurface)
                dfbSurface->Release(dfbSurface);
            if (rect == screen->region().boundingRect()) {
                dfbSurface = primarySurface;
            } else {
                const DFBRectangle r = { rect.x(), rect.y(),
                                         rect.width(), rect.height() };
                result = primarySurface->GetSubSurface(primarySurface, &r, &dfbSurface);
            }
            break;
        case Window:
#ifndef QT_NO_DIRECTFB_WM
            if (!dfbWindow)
                createWindow();
            ::setGeometry(dfbWindow, oldRect, rect);
            // ### do I need to release and get the surface again here?
#endif
            break;
        case Offscreen: {
            if (!dfbSurface || oldRect.size() != rect.size()) {
                if (dfbSurface)
                    dfbSurface->Release(dfbSurface);
                dfbSurface = screen->createDFBSurface(rect.size(), screen->pixelFormat(), QDirectFBScreen::DontTrackSurface);
            }
            const QRegion region = QRegion(oldRect.isEmpty() ? screen->region() : QRegion(oldRect)).subtracted(rect);
            screen->erase(region);
            screen->flipSurface(primarySurface, flipFlags, region, QPoint());
            break; }
        }

        if (result != DFB_OK)
            DirectFBErrorFatal("QDirectFBWindowSurface::setGeometry()", result);
    }

    QWSWindowSurface::setGeometry(rect);
}

QByteArray QDirectFBWindowSurface::permanentState() const
{
    QByteArray array;
#ifdef QT_NO_DIRECTFB_WM
    array.resize(sizeof(SurfaceFlags) + sizeof(IDirectFBSurface*));
#else
    array.resize(sizeof(SurfaceFlags));
#endif
    char *ptr = array.data();

    *reinterpret_cast<SurfaceFlags*>(ptr) = surfaceFlags();
    ptr += sizeof(SurfaceFlags);

#ifdef QT_NO_DIRECTFB_WM
    *reinterpret_cast<IDirectFBSurface**>(ptr) = dfbSurface;
#endif
    return array;
}

void QDirectFBWindowSurface::setPermanentState(const QByteArray &state)
{
    SurfaceFlags flags;
    const char *ptr = state.constData();

    flags = *reinterpret_cast<const SurfaceFlags*>(ptr);
    setSurfaceFlags(flags);

#ifdef QT_NO_DIRECTFB_WM
    ptr += sizeof(SurfaceFlags);
    dfbSurface = *reinterpret_cast<IDirectFBSurface* const*>(ptr);
#endif
}

static inline void scrollSurface(IDirectFBSurface *surface, const QRect &r, int dx, int dy)
{
    const DFBRectangle rect = { r.x(), r.y(), r.width(), r.height() };
    surface->Blit(surface, surface, &rect, r.x() + dx, r.y() + dy);
}

bool QDirectFBWindowSurface::scroll(const QRegion &region, int dx, int dy)
{
    if (!dfbSurface || !(flipFlags & DSFLIP_BLIT) || region.isEmpty())
        return false;
    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    if (region.numRects() == 1) {
        ::scrollSurface(dfbSurface, region.boundingRect(), dx, dy);
    } else {
        const QVector<QRect> rects = region.rects();
        const int n = rects.size();
        for (int i=0; i<n; ++i) {
            ::scrollSurface(dfbSurface, rects.at(i), dx, dy);
        }
    }
    return true;
}

bool QDirectFBWindowSurface::move(const QPoint &moveBy)
{
    setGeometry(geometry().translated(moveBy));
    return true;
}

QPaintEngine *QDirectFBWindowSurface::paintEngine() const
{
    if (!engine) {
        QDirectFBWindowSurface *that = const_cast<QDirectFBWindowSurface*>(this);
        that->engine = new QDirectFBPaintEngine(that);
        return that->engine;
    }
    return engine;
}

// hw: XXX: copied from QWidgetPrivate::isOpaque()
inline bool isWidgetOpaque(const QWidget *w)
{
    if (w->testAttribute(Qt::WA_OpaquePaintEvent)
        || w->testAttribute(Qt::WA_PaintOnScreen))
        return true;

    const QPalette &pal = w->palette();

    if (w->autoFillBackground()) {
        const QBrush &autoFillBrush = pal.brush(w->backgroundRole());
        if (autoFillBrush.style() != Qt::NoBrush && autoFillBrush.isOpaque())
            return true;
    }

    if (!w->testAttribute(Qt::WA_NoSystemBackground)) {
        const QBrush &windowBrush = w->palette().brush(QPalette::Window);
        if (windowBrush.style() != Qt::NoBrush && windowBrush.isOpaque())
            return true;
    }

    return false;
}

void QDirectFBWindowSurface::flush(QWidget *, const QRegion &region,
                                   const QPoint &offset)
{
    // hw: make sure opacity information is updated before compositing
    if (QWidget *win = window()) {

        const bool opaque = isWidgetOpaque(win);
        if (opaque != isOpaque()) {
            SurfaceFlags flags = surfaceFlags();
            if (opaque) {
                flags |= Opaque;
            } else {
                flags &= ~Opaque;
            }
            setSurfaceFlags(flags);
        }

#ifndef QT_NO_DIRECTFB_WM
        const quint8 winOpacity = quint8(win->windowOpacity() * 255);
        quint8 opacity;

        if (dfbWindow) {
            dfbWindow->GetOpacity(dfbWindow, &opacity);
            if (winOpacity != opacity)
                dfbWindow->SetOpacity(dfbWindow, winOpacity);
        }
#endif
    }

    const QRect windowGeometry = QDirectFBWindowSurface::geometry();
    IDirectFBSurface *primarySurface = screen->dfbSurface();
    if (mode == Offscreen) {
        primarySurface->SetBlittingFlags(primarySurface, DSBLIT_NOFX);
        const QRect windowRect(0, 0, windowGeometry.width(), windowGeometry.height());
        const int n = region.numRects();
        if (n == 1 || boundingRectFlip ) {
            const QRect regionBoundingRect = region.boundingRect().translated(offset);
            const QRect source = windowRect & regionBoundingRect;
            const DFBRectangle rect = {
                source.x(), source.y(), source.width(), source.height()
            };
            primarySurface->Blit(primarySurface, dfbSurface, &rect,
                                 windowGeometry.x() + source.x(),
                                 windowGeometry.y() + source.y());
        } else {
            const QVector<QRect> rects = region.rects();
            QVarLengthArray<DFBRectangle, 16> dfbRectangles(n);
            QVarLengthArray<DFBPoint, 16> dfbPoints(n);

            for (int i=0; i<n; ++i) {
                const QRect &r = rects.at(i).translated(offset);
                const QRect source = windowRect & r;
                DFBRectangle &rect = dfbRectangles[i];
                rect.x = source.x();
                rect.y = source.y();
                rect.w = source.width();
                rect.h = source.height();
                dfbPoints[i].x = (windowGeometry.x() + source.x());
                dfbPoints[i].y = (windowGeometry.y() + source.y());
            }
            primarySurface->BatchBlit(primarySurface, dfbSurface, dfbRectangles.constData(),
                                      dfbPoints.constData(), n);
        }
    }

    if (QScreenCursor *cursor = QScreenCursor::instance()) {
        const QRect cursorRectangle = cursor->boundingRect();
        if (cursor->isVisible() && !cursor->isAccelerated()
            && region.intersects(cursorRectangle.translated(-(offset + windowGeometry.topLeft())))) {
            const QImage image = cursor->image();

            IDirectFBSurface *surface = screen->createDFBSurface(image, QDirectFBScreen::DontTrackSurface);
            primarySurface->SetBlittingFlags(primarySurface, DSBLIT_BLEND_ALPHACHANNEL);
            primarySurface->Blit(primarySurface, surface, 0, cursorRectangle.x(), cursorRectangle.y());
            surface->Release(surface);
#if (Q_DIRECTFB_VERSION >= 0x010000)
            primarySurface->ReleaseSource(primarySurface);
#endif
        }
    }
    if (mode == Offscreen) {
        screen->flipSurface(primarySurface, flipFlags, region, offset + windowGeometry.topLeft());
    } else {
        screen->flipSurface(dfbSurface, flipFlags, region, offset);
    }

#ifdef QT_DIRECTFB_TIMING
    enum { Secs = 3 };
    ++frames;
    if (timer.elapsed() >= Secs * 1000) {
        qDebug("%d fps", int(double(frames) / double(Secs)));
        frames = 0;
        timer.restart();
    }
#endif
}


void QDirectFBWindowSurface::beginPaint(const QRegion &)
{
}

void QDirectFBWindowSurface::endPaint(const QRegion &)
{
#ifdef QT_DIRECTFB_DEBUG_SURFACES
    if (bufferImages.count()) {
        qDebug("QDirectFBWindowSurface::endPaint() this=%p", this);

        foreach(QImage* bufferImg, bufferImages)
            qDebug("   Deleting buffer image %p", bufferImg);
    }
#endif

    qDeleteAll(bufferImages);
    bufferImages.clear();
    unlockDirectFB();
}


QImage *QDirectFBWindowSurface::buffer(const QWidget *widget)
{
    if (!lockedImage)
        return 0;

    const QRect rect = QRect(offset(widget), widget->size())
                       & lockedImage->rect();
    if (rect.isEmpty())
        return 0;

    QImage *img = new QImage(lockedImage->scanLine(rect.y())
                             + rect.x() * (lockedImage->depth() / 8),
                             rect.width(), rect.height(),
                             lockedImage->bytesPerLine(),
                             lockedImage->format());
    bufferImages.append(img);

#ifdef QT_DIRECTFB_DEBUG_SURFACES
    qDebug("QDirectFBWindowSurface::buffer() Created & returned %p", img);
#endif

    return img;
}

