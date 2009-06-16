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

#include "qdirectfbwindowsurface.h"
#include "qdirectfbscreen.h"
#include "qdirectfbpaintengine.h"

#include <qwidget.h>
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
    onscreen = widget->testAttribute(Qt::WA_PaintOnScreen);
    if (onscreen)
        setSurfaceFlags(Opaque | RegionReserved);
    else
        setSurfaceFlags(Opaque | Buffered);
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
#warning QT_NO_DIRECTFB_LAYER requires QT_NO_DIRECTFB_WM
#else
    IDirectFBDisplayLayer *layer = screen->dfbDisplayLayer();
    if (!layer)
        qFatal("QDirectFBWindowSurface: Unable to get primary display layer!");

    DFBWindowDescription description;
    description.caps = DFBWindowCapabilities(DWCAPS_NODECORATION);
    description.flags = DFBWindowDescriptionFlags(DWDESC_CAPS
                                                  |DWDESC_SURFACE_CAPS
                                                  |DWDESC_PIXELFORMAT);

    description.surface_caps = DSCAPS_NONE;
    if (screen->directFBFlags() & QDirectFBScreen::VideoOnly)
        description.surface_caps = DFBSurfaceCapabilities(description.surface_caps|DSCAPS_VIDEOONLY);
    const QImage::Format format = screen->pixelFormat();
    description.pixelformat = QDirectFBScreen::getSurfacePixelFormat(format);
    if (QDirectFBScreen::isPremultiplied(format))
        description.surface_caps = DFBSurfaceCapabilities(DSCAPS_PREMULTIPLIED|description.caps);

    DFBResult result = layer->CreateWindow(layer, &description, &dfbWindow);
    if (result != DFB_OK)
        DirectFBErrorFatal("QDirectFBWindowSurface::createWindow", result);

    if (dfbSurface)
        dfbSurface->Release(dfbSurface);

    dfbWindow->GetSurface(dfbWindow, &dfbSurface);
#endif
}
#endif // QT_NO_DIRECTFB_WM

void QDirectFBWindowSurface::setGeometry(const QRect &rect, const QRegion &mask)
{
    if (rect.isNull()) {
#ifndef QT_NO_DIRECTFB_WM
        if (dfbWindow) {
            dfbWindow->Release(dfbWindow);
            dfbWindow = 0;
        }
#endif
        if (dfbSurface && dfbSurface != screen->dfbSurface()) {
            dfbSurface->Release(dfbSurface);
            dfbSurface = 0;
        }
    } else if (rect != geometry()) {
        DFBResult result = DFB_OK;

        // If we're in a resize, the surface shouldn't be locked
        Q_ASSERT((lockedImage == 0) || (rect.size() == geometry().size()));

        if (onscreen) {
            IDirectFBSurface *primarySurface = screen->dfbSurface();
            Q_ASSERT(primarySurface);
            if (dfbSurface && dfbSurface != primarySurface)
                dfbSurface->Release(dfbSurface);

            if (rect == screen->region().boundingRect()) {
                dfbSurface = primarySurface;
            } else {
                const DFBRectangle r = { rect.x(), rect.y(),
                                         rect.width(), rect.height() };
                result = primarySurface->GetSubSurface(primarySurface, &r, &dfbSurface);
            }
        } else {
            const bool isResize = rect.size() != geometry().size();
#ifdef QT_NO_DIRECTFB_WM
            if (isResize) {
                if (dfbSurface)
                    dfbSurface->Release(dfbSurface);

                IDirectFB *dfb = screen->dfb();
                if (!dfb) {
                    qFatal("QDirectFBWindowSurface::setGeometry(): "
                           "Unable to get DirectFB handle!");
                }

                dfbSurface = screen->createDFBSurface(rect.size(), screen->pixelFormat(), QDirectFBScreen::DontTrackSurface);
            } else {
                Q_ASSERT(dfbSurface);
            }
#else
            const QRect oldRect = geometry();
            const bool isMove = oldRect.isEmpty() ||
                                rect.topLeft() != oldRect.topLeft();

            if (!dfbWindow)
                createWindow();

            if (isResize && isMove)
                result = dfbWindow->SetBounds(dfbWindow, rect.x(), rect.y(),
                                              rect.width(), rect.height());
            else if (isResize)
                result = dfbWindow->Resize(dfbWindow,
                                           rect.width(), rect.height());
            else if (isMove)
                result = dfbWindow->MoveTo(dfbWindow, rect.x(), rect.y());
#endif
        }

        if (result != DFB_OK)
            DirectFBErrorFatal("QDirectFBWindowSurface::setGeometry()", result);
    }

    QWSWindowSurface::setGeometry(rect, mask);
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

bool QDirectFBWindowSurface::scroll(const QRegion &region, int dx, int dy)
{
    if (!dfbSurface || !(flipFlags & DSFLIP_BLIT))
        return false;

    const QVector<QRect> rects = region.rects();
    const int n = rects.size();

    QVarLengthArray<DFBRectangle, 8> dfbRects(n);
    QVarLengthArray<DFBPoint, 8> dfbPoints(n);

    for (int i = 0; i < n; ++i) {
        const QRect r = rects.at(i);
        dfbRects[i].x = r.x();
        dfbRects[i].y = r.y();
        dfbRects[i].w = r.width();
        dfbRects[i].h = r.height();
        dfbPoints[i].x = r.x() + dx;
        dfbPoints[i].y = r.y() + dy;
    }

    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    dfbSurface->BatchBlit(dfbSurface, dfbSurface,
                          dfbRects.data(), dfbPoints.data(), n);
    dfbSurface->ReleaseSource(dfbSurface);
    return true;
}

bool QDirectFBWindowSurface::move(const QPoint &offset)
{
    QWSWindowSurface::move(offset);

#ifdef QT_NO_DIRECTFB_WM
    return true; // buffered
#else
    if (!dfbWindow)
        return false;

    DFBResult status = dfbWindow->Move(dfbWindow, offset.x(), offset.y());
    return (status == DFB_OK);
#endif
}

QRegion QDirectFBWindowSurface::move(const QPoint &offset, const QRegion &newClip)
{
#ifdef QT_NO_DIRECTFB_WM
    return QWSWindowSurface::move(offset, newClip);
#else
    Q_UNUSED(offset);
    Q_UNUSED(newClip);

    // DirectFB handles the entire move, so there's no need to blit.
    return QRegion();
#endif
}

QPaintEngine* QDirectFBWindowSurface::paintEngine() const
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
void QDirectFBWindowSurface::flush(QWidget *widget, const QRegion &region,
                                   const QPoint &offset)
{
    Q_UNUSED(widget);
#ifdef QT_NO_DIRECTFB_WM
    Q_UNUSED(region);
    Q_UNUSED(offset);
#endif

    QWidget *win = window();

    // hw: make sure opacity information is updated before compositing
    const bool opaque = isWidgetOpaque(win);
    if (opaque != isOpaque()) {
        SurfaceFlags flags = Buffered;
        if (opaque)
            flags |= Opaque;
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
    if (!(flipFlags & DSFLIP_BLIT)) {
        dfbSurface->Flip(dfbSurface, 0, flipFlags);
    } else {
        if (!boundingRectFlip && region.numRects() > 1) {
            const QVector<QRect> rects = region.rects();
            const DFBSurfaceFlipFlags nonWaitFlags = DFBSurfaceFlipFlags(flipFlags & ~DSFLIP_WAIT);
            for (int i=0; i<rects.size(); ++i) {
                const QRect &r = rects.at(i);
                const DFBRegion dfbReg = { r.x() + offset.x(), r.y() + offset.y(),
                                           r.x() + r.width() + offset.x(),
                                           r.y() + r.height() + offset.y() };
                dfbSurface->Flip(dfbSurface, &dfbReg, i + 1 < rects.size() ? nonWaitFlags : flipFlags);
            }
        } else {
            const QRect r = region.boundingRect();
            const DFBRegion dfbReg = { r.x() + offset.x(), r.y() + offset.y(),
                                       r.x() + r.width() + offset.x(),
                                       r.y() + r.height() + offset.y() };
            dfbSurface->Flip(dfbSurface, &dfbReg, flipFlags);
        }
    }
#endif
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

