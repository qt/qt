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

#include "qdirectfbpixmap.h"

#include "qdirectfbscreen.h"
#include "qdirectfbpaintengine.h"

#include <QtGui/qbitmap.h>
#include <directfb.h>

static int global_ser_no = 0;

QDirectFBPixmapData::QDirectFBPixmapData(PixelType pixelType)
    : QPixmapData(pixelType, DirectFBClass),
      engine(0)
{
    setSerialNumber(0);
}

QDirectFBPixmapData::~QDirectFBPixmapData()
{
    unlockDirectFB();
    if (dfbSurface && QDirectFBScreen::instance())
        screen->releaseDFBSurface(dfbSurface);
    delete engine;
}

void QDirectFBPixmapData::resize(int width, int height)
{
    if (width <= 0 || height <= 0) {
        setSerialNumber(0);
        return;
    }

    DFBSurfaceDescription description;
    description.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH |
                                                   DSDESC_HEIGHT);
    description.width = width;
    description.height = height;

    dfbSurface = screen->createDFBSurface(&description);
    if (!dfbSurface)
        qCritical("QDirectFBPixmapData::resize(): Unable to allocate surface");

    setSerialNumber(++global_ser_no);
}

void QDirectFBPixmapData::fromImage(const QImage &img,
                                    Qt::ImageConversionFlags)
{
    QImage image;
    if (QDirectFBScreen::getSurfacePixelFormat(img) == DSPF_UNKNOWN)
        image = img.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    else
        image = img;

    DFBSurfaceDescription description;
    description = QDirectFBScreen::getSurfaceDescription(image);

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    IDirectFBSurface *imgSurface;
    imgSurface = screen->createDFBSurface(&description);
    if (!imgSurface) {
        qWarning("QDirectFBPixmapData::fromImage()");
        setSerialNumber(0);
        return;
    }
#ifndef QT_NO_DIRECTFB_PALETTE
    QDirectFBScreen::setSurfaceColorTable(imgSurface, image);
#endif
#endif // QT_NO_DIRECTFB_PREALLOCATED

    description.flags = DFBSurfaceDescriptionFlags(description.flags
                                                   & ~DSDESC_PREALLOCATED);
    dfbSurface = screen->createDFBSurface(&description);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::fromImage()");
        setSerialNumber(0);
        return;
    }

#ifndef QT_NO_DIRECTFB_PALETTE
    QDirectFBScreen::setSurfaceColorTable(dfbSurface, image);
#endif

#ifdef QT_NO_DIRECTFB_PREALLOCATED
    char *mem;
    surface->Lock(surface, DSLF_WRITE, (void**)&mem, &bpl);
    const int w = image.width() * image.depth() / 8;
    for (int i = 0; i < image.height(); ++i) {
        memcpy(mem, image.scanLine(i), w);
        mem += bpl;
    }
    surface->Unlock(surface);
#else
    DFBResult result;
    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    result = dfbSurface->Blit(dfbSurface, imgSurface, 0, 0, 0);
    if (result != DFB_OK)
        DirectFBError("QDirectFBPixmapData::fromImage()", result);
    dfbSurface->Flip(dfbSurface, 0, DSFLIP_NONE);
    dfbSurface->ReleaseSource(dfbSurface);
    screen->releaseDFBSurface(imgSurface);
#endif // QT_NO_DIRECTFB_PREALLOCATED

    setSerialNumber(++global_ser_no);
}

void QDirectFBPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    if (data->classId() != DirectFBClass) {
        QPixmapData::copy(data, rect);
        return;
    }

    IDirectFBSurface *src = static_cast<const QDirectFBPixmapData*>(data)->directFbSurface();

    DFBSurfaceDescription description;
    description.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH |
                                                   DSDESC_HEIGHT |
                                                   DSDESC_PIXELFORMAT);
    description.width = rect.width();
    description.height = rect.height();
    src->GetPixelFormat(src, &description.pixelformat);

    dfbSurface = screen->createDFBSurface(&description);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::copy()");
        setSerialNumber(0);
        return;
    }

    DFBResult result;
#ifndef QT_NO_DIRECTFB_PALETTE
    IDirectFBPalette *palette;
    result = src->GetPalette(src, &palette);
    if (result == DFB_OK) {
        dfbSurface->SetPalette(dfbSurface, palette);
        palette->Release(palette);
    }
#endif

    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    const DFBRectangle blitRect = { rect.x(), rect.y(),
                                    rect.width(), rect.height() };
    result = dfbSurface->Blit(dfbSurface, src, &blitRect, 0, 0);
    if (result != DFB_OK)
        DirectFBError("QDirectFBPixmapData::copy()", result);

    setSerialNumber(++global_ser_no);
}


void QDirectFBPixmapData::fill(const QColor &color)
{
    if (!serialNumber())
        return;

    Q_ASSERT(dfbSurface);

    if (color.alpha() < 255 && !hasAlphaChannel()) {
        // convert to surface supporting alpha channel
        DFBSurfacePixelFormat format;
        dfbSurface->GetPixelFormat(dfbSurface, &format);
        switch (format) {
        case DSPF_YUY2:
        case DSPF_UYVY:
            format = DSPF_AYUV;
            break;
#if (Q_DIRECTFB_VERSION >= 0x010100)
        case DSPF_RGB444:
            format = DSPF_ARGB4444;
            break;
        case DSPF_RGB555:
#endif
        case DSPF_RGB18:
            format = DSPF_ARGB6666;
            break;
        default:
            format = DSPF_ARGB;
            break;
        }

        DFBSurfaceDescription description;
        description.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH |
                                                       DSDESC_HEIGHT |
                                                       DSDESC_PIXELFORMAT);
        dfbSurface->GetSize(dfbSurface, &description.width, &description.height);
        description.pixelformat = format;
        screen->releaseDFBSurface(dfbSurface); // release old surface

        dfbSurface = screen->createDFBSurface(&description);
        if (!dfbSurface) {
            qWarning("QDirectFBPixmapData::fill()");
            setSerialNumber(0);
            return;
        }
    }

    dfbSurface->Clear(dfbSurface, color.red(), color.green(), color.blue(),
                      color.alpha());
}

bool QDirectFBPixmapData::hasAlphaChannel() const
{
    if (!serialNumber())
        return false;

    DFBSurfacePixelFormat format;
    dfbSurface->GetPixelFormat(dfbSurface, &format);
    switch (format) {
    case DSPF_ARGB1555:
    case DSPF_ARGB:
    case DSPF_LUT8:
    case DSPF_AiRGB:
    case DSPF_A1:
    case DSPF_ARGB2554:
    case DSPF_ARGB4444:
    case DSPF_AYUV:
    case DSPF_A4:
    case DSPF_ARGB1666:
    case DSPF_ARGB6666:
    case DSPF_LUT2:
        return true;
    default:
        return false;
    }
}

QPixmap QDirectFBPixmapData::transformed(const QTransform &transform,
                                         Qt::TransformationMode mode) const
{
    if (!dfbSurface || transform.type() != QTransform::TxScale
        || mode != Qt::FastTransformation)
    {
        QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
        const QImage *image = that->buffer();
        if (image) { // avoid deep copy
            const QImage transformed = image->transformed(transform, mode);
            that->unlockDirectFB();
            QDirectFBPixmapData *data = new QDirectFBPixmapData(pixelType());
            data->fromImage(transformed, Qt::AutoColor);
            return QPixmap(data);
        }
        return QPixmapData::transformed(transform, mode);
    }

    int w, h;
    dfbSurface->GetSize(dfbSurface, &w, &h);

    const QSize size = transform.mapRect(QRect(0, 0, w, h)).size();
    if (size.isEmpty())
        return QPixmap();

    QDirectFBPixmapData *data = new QDirectFBPixmapData(pixelType());
    data->resize(size.width(), size.height());

    IDirectFBSurface *dest = data->dfbSurface;
    dest->SetBlittingFlags(dest, DSBLIT_NOFX);

    const DFBRectangle srcRect = { 0, 0, w, h };
    const DFBRectangle destRect = { 0, 0, size.width(), size.height() };
    dest->StretchBlit(dest, dfbSurface, &srcRect, &destRect);

    return QPixmap(data);
}

QImage QDirectFBPixmapData::toImage() const
{
    if (!dfbSurface)
        return QImage();

#ifdef QT_NO_DIRECTFB_PREALLOCATED
    QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
    const QImage *img = that->buffer();
    const QImage copied = img->copy();
    that->unlockDirectFB();
    return copied;
#else

    int w, h;
    dfbSurface->GetSize(dfbSurface, &w, &h);

    // Always convert to ARGB32:
    QImage image(w, h, QImage::Format_ARGB32);

    DFBSurfaceDescription description;
    description = QDirectFBScreen::getSurfaceDescription(image);

    IDirectFBSurface *imgSurface = screen->createDFBSurface(&description);
    if (!imgSurface) {
        qWarning("QDirectFBPixmapData::toImage()");
        return QImage();
    }

    imgSurface->SetBlittingFlags(imgSurface, DSBLIT_NOFX);
    DFBResult result = imgSurface->Blit(imgSurface, dfbSurface, 0, 0, 0);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBPixmapData::toImage() blit failed", result);
        return QImage();
    }
    screen->releaseDFBSurface(imgSurface);

    return image;
#endif // QT_NO_DIRECTFB_PREALLOCATED
}

QPaintEngine* QDirectFBPixmapData::paintEngine() const
{
    if (!engine) {
        // QDirectFBPixmapData is also a QCustomRasterPaintDevice, so pass
        // that to the paint engine:
        QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
        that->engine = new QDirectFBPaintEngine(that);
    }
    return engine;
}


QImage* QDirectFBPixmapData::buffer()
{
    lockDirectFB();
    return lockedImage;
}
