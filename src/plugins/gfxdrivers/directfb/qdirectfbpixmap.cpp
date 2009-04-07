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

    dfbSurface = screen->createDFBSurface(QSize(width, height),
                                          screen->pixelFormat(),
                                          QDirectFBScreen::TrackSurface);
    forceRaster = (screen->pixelFormat() == QImage::Format_RGB32);
    if (!dfbSurface) {
        setSerialNumber(0);
        qWarning("QDirectFBPixmapData::resize(): Unable to allocate surface");
        return;
    }

    setSerialNumber(++global_ser_no);
}

void QDirectFBPixmapData::fromImage(const QImage &img,
                                    Qt::ImageConversionFlags)
{
    const QImage::Format format = img.hasAlphaChannel() ?
                                  screen->alphaPixmapFormat()
                                  : screen->pixelFormat();
    dfbSurface = screen->copyToDFBSurface(img, format,
                                          QDirectFBScreen::TrackSurface);
    forceRaster = (format == QImage::Format_RGB32);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::fromImage()");
        setSerialNumber(0);
        return;
    }
    setSerialNumber(++global_ser_no);
}

void QDirectFBPixmapData::copy(const QPixmapData *data, const QRect &rect)
{
    if (data->classId() != DirectFBClass) {
        QPixmapData::copy(data, rect);
        return;
    }

    IDirectFBSurface *src = static_cast<const QDirectFBPixmapData*>(data)->directFBSurface();
    const bool hasAlpha = data->hasAlphaChannel();
    const QImage::Format format = (hasAlpha
                                   ? screen->alphaPixmapFormat()
                                   : screen->pixelFormat());

    dfbSurface = screen->createDFBSurface(rect.size(), format,
                                          QDirectFBScreen::TrackSurface);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::copy()");
        setSerialNumber(0);
        return;
    }
    forceRaster = (format == QImage::Format_RGB32);

    if (hasAlpha) {
        dfbSurface->Clear(dfbSurface, 0, 0, 0, 0);
        dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_BLEND_ALPHACHANNEL);
    } else {
        dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    }
    const DFBRectangle blitRect = { rect.x(), rect.y(),
                                    rect.width(), rect.height() };
    DFBResult result = dfbSurface->Blit(dfbSurface, src, &blitRect, 0, 0);
    dfbSurface->ReleaseSource(dfbSurface);
    if (result != DFB_OK) {
        DirectFBError("QDirectFBPixmapData::copy()", result);
        setSerialNumber(0);
        return;
    }

    setSerialNumber(++global_ser_no);
}


void QDirectFBPixmapData::fill(const QColor &color)
{
    if (!serialNumber())
        return;

    Q_ASSERT(dfbSurface);

    if (color.alpha() < 255 && !hasAlphaChannel()) {

        QSize size;
        dfbSurface->GetSize(dfbSurface, &size.rwidth(), &size.rheight());
        screen->releaseDFBSurface(dfbSurface);
        dfbSurface = screen->createDFBSurface(size, screen->alphaPixmapFormat(), QDirectFBScreen::TrackSurface);
        forceRaster = false;
        setSerialNumber(++global_ser_no);
        if (!dfbSurface) {
            qWarning("QDirectFBPixmapData::fill()");
            setSerialNumber(0);
            return;
        }
    }

    if (forceRaster) {
        // in DSPF_RGB32 all dfb drawing causes the Alpha byte to be
        // set to 0. This causes issues for the raster engine.
        char *mem;
        int bpl;
        const int h = QPixmapData::height();
        dfbSurface->Lock(dfbSurface, DSLF_WRITE, (void**)&mem, &bpl);
        const int c = color.rgba();
        for (int i = 0; i < h; ++i) {
            memset(mem, c, bpl);
            mem += bpl;
        }
        dfbSurface->Unlock(dfbSurface);
    } else {
        dfbSurface->Clear(dfbSurface, color.red(), color.green(), color.blue(),
                          color.alpha());
    }
}

bool QDirectFBPixmapData::hasAlphaChannel() const
{
    if (!serialNumber())
        return false;
    DFBSurfacePixelFormat format;
    dfbSurface->GetPixelFormat(dfbSurface, &format);
    return QDirectFBScreen::hasAlpha(format);

    // We don't need to ask DFB for this really. Can just keep track
    // of what image format this has. It should always have either
    // QDirectFBScreen::alphaPixmapFormat() or QScreen::pixelFormat()
}

QPixmap QDirectFBPixmapData::transformed(const QTransform &transform,
                                         Qt::TransformationMode mode) const
{
    if (!dfbSurface || transform.type() != QTransform::TxScale
        || mode != Qt::FastTransformation)
    {
        QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
        const QImage *image = that->buffer();
        Q_ASSERT(image);
        const QImage transformed = image->transformed(transform, mode);
        that->unlockDirectFB();
        QDirectFBPixmapData *data = new QDirectFBPixmapData(QPixmapData::PixmapType);
        data->fromImage(transformed, Qt::AutoColor);
        return QPixmap(data);
    }

    int w, h;
    dfbSurface->GetSize(dfbSurface, &w, &h);

    const QSize size = transform.mapRect(QRect(0, 0, w, h)).size();
    if (size.isEmpty())
        return QPixmap();

    QDirectFBPixmapData *data = new QDirectFBPixmapData(QPixmapData::PixmapType);
    QImage::Format format = screen->pixelFormat();
    DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
    if (hasAlphaChannel()) {
        flags = DSBLIT_BLEND_ALPHACHANNEL;
        format = screen->alphaPixmapFormat();
    }
    data->dfbSurface = screen->createDFBSurface(size,
                                                format,
                                                QDirectFBScreen::TrackSurface);
    if (flags & DSBLIT_BLEND_ALPHACHANNEL) {
        data->dfbSurface->Clear(data->dfbSurface, 0, 0, 0, 0);
    }
    data->dfbSurface->SetBlittingFlags(data->dfbSurface, flags);

    const DFBRectangle destRect = { 0, 0, size.width(), size.height() };
    data->dfbSurface->StretchBlit(data->dfbSurface, dfbSurface, 0, &destRect);
    data->dfbSurface->ReleaseSource(data->dfbSurface);

    return QPixmap(data);
}

QImage QDirectFBPixmapData::toImage() const
{
    if (!dfbSurface)
        return QImage();

#ifndef QT_NO_DIRECTFB_PREALLOCATED
    QImage ret(size(), QDirectFBScreen::getImageFormat(dfbSurface));
    if (IDirectFBSurface *imgSurface = screen->createDFBSurface(ret, QDirectFBScreen::DontTrackSurface)) {
        if (hasAlphaChannel()) {
            imgSurface->SetBlittingFlags(imgSurface, DSBLIT_BLEND_ALPHACHANNEL);
            imgSurface->Clear(imgSurface, 0, 0, 0, 0);
        } else {
            imgSurface->SetBlittingFlags(imgSurface, DSBLIT_NOFX);
        }
        imgSurface->Blit(imgSurface, dfbSurface, 0, 0, 0);
        imgSurface->ReleaseSource(imgSurface);
        imgSurface->Release(imgSurface);
        return ret;
    }
#endif

    QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
    const QImage *img = that->buffer();
    return img->copy();
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
