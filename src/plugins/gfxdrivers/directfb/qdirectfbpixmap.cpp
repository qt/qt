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

#include "qdirectfbpixmap.h"

#include "qdirectfbscreen.h"
#include "qdirectfbpaintengine.h"

#include <QtGui/qbitmap.h>
#include <directfb.h>

static int global_ser_no = 0;

QDirectFBPixmapData::QDirectFBPixmapData(PixelType pixelType)
    : QPixmapData(pixelType, DirectFBClass),
      engine(0), format(QImage::Format_Invalid), alpha(false)
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
        invalidate();
        return;
    }

    format = screen->pixelFormat();
    dfbSurface = QDirectFBScreen::instance()->createDFBSurface(QSize(width, height),
                                                               format,
                                                               QDirectFBScreen::TrackSurface);
    alpha = false;
    if (!dfbSurface) {
        invalidate();
        qWarning("QDirectFBPixmapData::resize(): Unable to allocate surface");
        return;
    }

    setSerialNumber(++global_ser_no);
}


// mostly duplicated from qimage.cpp (QImageData::checkForAlphaPixels)
static bool checkForAlphaPixels(const QImage &img)
{
    const uchar *bits = img.bits();
    const int bytes_per_line = img.bytesPerLine();
    const uchar *end_bits = bits + bytes_per_line;
    const int width = img.width();
    const int height = img.height();
    switch (img.format()) {
    case QImage::Format_Indexed8:
        return img.hasAlphaChannel();
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                if ((((uint *)bits)[x] & 0xff000000) != 0xff000000) {
                    return true;
                }
            }
            bits += bytes_per_line;
        }
        break;

    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_ARGB8565_Premultiplied:
        for (int y=0; y<height; ++y) {
            while (bits < end_bits) {
                if (bits[0] != 0) {
                    return true;
                }
                bits += 3;
            }
            bits = end_bits;
            end_bits += bytes_per_line;
        }
        break;

    case QImage::Format_ARGB6666_Premultiplied:
        for (int y=0; y<height; ++y) {
            while (bits < end_bits) {
                if ((bits[0] & 0xfc) != 0) {
                    return true;
                }
                bits += 3;
            }
            bits = end_bits;
            end_bits += bytes_per_line;
        }
        break;

    case QImage::Format_ARGB4444_Premultiplied:
        for (int y=0; y<height; ++y) {
            while (bits < end_bits) {
                if ((bits[0] & 0xf0) != 0) {
                    return true;
                }
                bits += 2;
            }
            bits = end_bits;
            end_bits += bytes_per_line;
        }
        break;

    default:
        break;
    }

    return false;
}

bool QDirectFBPixmapData::hasAlphaChannel(const QImage &img)
{
#ifndef QT_NO_DIRECTFB_OPAQUE_DETECTION
    return ::checkForAlphaPixels(img);
#else
    return img.hasAlphaChannel();
#endif
}


void QDirectFBPixmapData::fromImage(const QImage &i,
                                    Qt::ImageConversionFlags flags)
{
#ifdef QT_NO_DIRECTFB_OPAQUE_DETECTION
    Q_UNUSED(flags);
#endif
    const QImage img = (i.depth() == 1 ? i.convertToFormat(screen->alphaPixmapFormat()) : i);
    if (img.hasAlphaChannel()
#ifndef QT_NO_DIRECTFB_OPAQUE_DETECTION
        && (flags & Qt::NoOpaqueDetection || QDirectFBPixmapData::hasAlphaChannel(img))
#endif
        ) {
        alpha = true;
        format = screen->alphaPixmapFormat();
    } else {
        alpha = false;
        format = screen->pixelFormat();
    }
    dfbSurface = screen->copyToDFBSurface(img, format,
                                          QDirectFBScreen::TrackSurface);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::fromImage()");
        invalidate();
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
    alpha = data->hasAlphaChannel();
    format = (alpha
              ? QDirectFBScreen::instance()->alphaPixmapFormat()
              : QDirectFBScreen::instance()->pixelFormat());


    dfbSurface = screen->createDFBSurface(rect.size(), format,
                                          QDirectFBScreen::TrackSurface);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::copy()");
        invalidate();
        return;
    }

    if (alpha) {
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
        invalidate();
        return;
    }

    setSerialNumber(++global_ser_no);
}

static inline bool isOpaqueFormat(QImage::Format format)
{
    switch (format) {
    case QImage::Format_RGB32:
    case QImage::Format_RGB16:
    case QImage::Format_RGB666:
    case QImage::Format_RGB555:
    case QImage::Format_RGB888:
    case QImage::Format_RGB444:
        return true;
    default:
        break;
    }
    return false;
}

void QDirectFBPixmapData::fill(const QColor &color)
{
    if (!serialNumber())
        return;

    Q_ASSERT(dfbSurface);

    alpha = (color.alpha() < 255);

    if (alpha && ::isOpaqueFormat(format)) {
        QSize size;
        dfbSurface->GetSize(dfbSurface, &size.rwidth(), &size.rheight());
        screen->releaseDFBSurface(dfbSurface);
        format = screen->alphaPixmapFormat();
        dfbSurface = screen->createDFBSurface(size, screen->alphaPixmapFormat(), QDirectFBScreen::TrackSurface);
        setSerialNumber(++global_ser_no);
        if (!dfbSurface) {
            qWarning("QDirectFBPixmapData::fill()");
            invalidate();
            return;
        }
    }

    dfbSurface->Clear(dfbSurface, color.red(), color.green(), color.blue(), color.alpha());
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
    DFBSurfaceBlittingFlags flags = DSBLIT_NOFX;
    data->alpha = alpha;
    if (alpha) {
        flags = DSBLIT_BLEND_ALPHACHANNEL;
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

QPaintEngine *QDirectFBPixmapData::paintEngine() const
{
    if (!engine) {
        // QDirectFBPixmapData is also a QCustomRasterPaintDevice, so pass
        // that to the paint engine:
        QDirectFBPixmapData *that = const_cast<QDirectFBPixmapData*>(this);
        that->engine = new QDirectFBPaintEngine(that);
    }
    return engine;
}

QImage *QDirectFBPixmapData::buffer()
{
    lockDirectFB(DSLF_READ|DSLF_WRITE);
    return lockedImage;
}

QImage * QDirectFBPixmapData::buffer(uint lockFlags)
{
    lockDirectFB(lockFlags);
    return lockedImage;
}

void QDirectFBPixmapData::invalidate()
{
    setSerialNumber(0);
    alpha = false;
    format = QImage::Format_Invalid;
}

