/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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

    dfbSurface = QDirectFBScreen::instance()->createDFBSurface(QSize(width, height),
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
    const QImage::Format format = (data->hasAlphaChannel()
                                   ? QDirectFBScreen::instance()->alphaPixmapFormat()
                                   : QDirectFBScreen::instance()->pixelFormat());

    dfbSurface = screen->createDFBSurface(rect.size(), format,
                                          QDirectFBScreen::TrackSurface);
    if (!dfbSurface) {
        qWarning("QDirectFBPixmapData::copy()");
        setSerialNumber(0);
        return;
    }
    forceRaster = (format == QImage::Format_RGB32);

    dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
    const DFBRectangle blitRect = { rect.x(), rect.y(),
                                    rect.width(), rect.height() };
    DFBResult result = dfbSurface->Blit(dfbSurface, src, &blitRect, 0, 0);
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
        DFBSurfaceDescription description;
        description.flags = DFBSurfaceDescriptionFlags(DSDESC_WIDTH |
                                                       DSDESC_HEIGHT |
                                                       DSDESC_PIXELFORMAT);
        dfbSurface->GetSize(dfbSurface, &description.width, &description.height);
        QDirectFBScreen::initSurfaceDescriptionPixelFormat(&description, screen->alphaPixmapFormat());
        screen->releaseDFBSurface(dfbSurface); // release old surface

        dfbSurface = screen->createDFBSurface(&description, QDirectFBScreen::TrackSurface);
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

    // We don't need to ask DFB for this really. Can just keep track
    // of what image format this has. It should always have either
    // QDirectFBScreen::alphaPixmapFormat() or QScreen::pixelFormat()

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
    data->resize(size.width(), size.height());

    IDirectFBSurface *dest = data->dfbSurface;
    dest->SetBlittingFlags(dest, DSBLIT_NOFX);

    const DFBRectangle destRect = { 0, 0, size.width(), size.height() };
    dest->StretchBlit(dest, dfbSurface, 0, &destRect);

    return QPixmap(data);
}

QImage QDirectFBPixmapData::toImage() const
{
    if (!dfbSurface)
        return QImage();

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
