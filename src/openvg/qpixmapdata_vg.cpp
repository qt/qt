/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#include "qpixmapdata_vg_p.h"
#include "qpaintengine_vg_p.h"
#include <QtGui/private/qdrawhelper_p.h>
#include "qvg_p.h"

QT_BEGIN_NAMESPACE

static int qt_vg_pixmap_serial = 0;

QVGPixmapData::QVGPixmapData(PixelType type)
    : QPixmapData(type, OpenVGClass)
{
    Q_ASSERT(type == QPixmapData::PixmapType);
    vgImage = VG_INVALID_HANDLE;
    vgImageOpacity = VG_INVALID_HANDLE;
    cachedOpacity = 1.0f;
    recreate = true;
#if !defined(QT_NO_EGL)
    context = 0;
#endif
    setSerialNumber(++qt_vg_pixmap_serial);
}

QVGPixmapData::~QVGPixmapData()
{
    if (vgImage != VG_INVALID_HANDLE) {
        // We need to have a context current to destroy the image.
#if !defined(QT_NO_EGL)
        if (context->isCurrent()) {
            vgDestroyImage(vgImage);
            if (vgImageOpacity != VG_INVALID_HANDLE)
                vgDestroyImage(vgImageOpacity);
        } else {
            // We don't currently have a widget surface active, but we
            // need a surface to make the context current.  So use the
            // shared pbuffer surface instead.
            qt_vg_make_current(context, qt_vg_shared_surface());
            vgDestroyImage(vgImage);
            if (vgImageOpacity != VG_INVALID_HANDLE)
                vgDestroyImage(vgImageOpacity);
            qt_vg_done_current(context);
            context->setSurface(EGL_NO_SURFACE);
        }
#else
        vgDestroyImage(vgImage);
        if (vgImageOpacity != VG_INVALID_HANDLE)
            vgDestroyImage(vgImageOpacity);
        } else {
#endif
    }
#if !defined(QT_NO_EGL)
    if (context)
        qt_vg_destroy_context(context);
#endif
}

bool QVGPixmapData::isValid() const
{
    return (w > 0 && h > 0);
}

void QVGPixmapData::resize(int wid, int ht)
{
    if (w == wid && h == ht)
        return;

    w = wid;
    h = ht;
    d = 32; // We always use ARGB_Premultiplied for VG pixmaps.
    is_null = (w <= 0 || h <= 0);
    source = QImage();
    recreate = true;

    setSerialNumber(++qt_vg_pixmap_serial);
}

void QVGPixmapData::fromImage
        (const QImage &image, Qt::ImageConversionFlags flags)
{
    if (image.size() == QSize(w, h))
        setSerialNumber(++qt_vg_pixmap_serial);
    else
        resize(image.width(), image.height());
    source = image.convertToFormat(sourceFormat(), flags);
    recreate = true;
}

void QVGPixmapData::fill(const QColor &color)
{
    if (!isValid())
        return;

    if (source.isNull())
        source = QImage(w, h, sourceFormat());

    if (source.depth() == 1) {
        // Pick the best approximate color in the image's colortable.
        int gray = qGray(color.rgba());
        if (qAbs(qGray(source.color(0)) - gray) < qAbs(qGray(source.color(1)) - gray))
            source.fill(0);
        else
            source.fill(1);
    } else {
        source.fill(PREMUL(color.rgba()));
    }

    // Re-upload the image to VG the next time toVGImage() is called.
    recreate = true;
}

bool QVGPixmapData::hasAlphaChannel() const
{
    if (!source.isNull())
        return source.hasAlphaChannel();
    else
        return isValid();
}

void QVGPixmapData::setAlphaChannel(const QPixmap &alphaChannel)
{
    forceToImage();
    source.setAlphaChannel(alphaChannel.toImage());
}

QImage QVGPixmapData::toImage() const
{
    if (!isValid())
        return QImage();

    if (source.isNull()) {
        source = QImage(w, h, sourceFormat());
        recreate = true;
    }

    return source;
}

QImage *QVGPixmapData::buffer()
{
    forceToImage();
    return &source;
}

QPaintEngine* QVGPixmapData::paintEngine() const
{
    // If the application wants to paint into the QPixmap, we first
    // force it to QImage format and then paint into that.
    // This is simpler than juggling multiple VG contexts.
    const_cast<QVGPixmapData *>(this)->forceToImage();
    return source.paintEngine();
}

VGImage QVGPixmapData::toVGImage()
{
    if (!isValid())
        return VG_INVALID_HANDLE;

#if !defined(QT_NO_EGL)
    // Increase the reference count on the shared context.
    if (!context)
        context = qt_vg_create_context(0);
#endif

    if (recreate) {
        if (vgImage != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImage);
            vgImage = VG_INVALID_HANDLE;
        }
        if (vgImageOpacity != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImageOpacity);
            vgImageOpacity = VG_INVALID_HANDLE;
        }
    }

    if (vgImage == VG_INVALID_HANDLE) {
        vgImage = vgCreateImage
            (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
    }

    if (!source.isNull() && recreate) {
        vgImageSubData
            (vgImage,
             source.bits(), source.bytesPerLine(),
             VG_sARGB_8888_PRE, 0, 0, w, h);
    }

    recreate = false;

    return vgImage;
}

VGImage QVGPixmapData::toVGImage(qreal opacity)
{
#if !defined(QT_SHIVAVG)
    if (!isValid())
        return VG_INVALID_HANDLE;

#if !defined(QT_NO_EGL)
    // Increase the reference count on the shared context.
    if (!context)
        context = qt_vg_create_context(0);
#endif

    if (recreate) {
        if (vgImage != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImage);
            vgImage = VG_INVALID_HANDLE;
        }
        if (vgImageOpacity != VG_INVALID_HANDLE) {
            vgDestroyImage(vgImageOpacity);
            vgImageOpacity = VG_INVALID_HANDLE;
        }
    }

    if (vgImage == VG_INVALID_HANDLE) {
        vgImage = vgCreateImage
            (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
    }

    if (!source.isNull() && recreate) {
        vgImageSubData
            (vgImage,
             source.bits(), source.bytesPerLine(),
             VG_sARGB_8888_PRE, 0, 0, w, h);
    }

    recreate = false;

    if (opacity == 1.0f)
        return vgImage;

    if (vgImageOpacity == VG_INVALID_HANDLE || cachedOpacity != opacity) {
        if (vgImageOpacity == VG_INVALID_HANDLE) {
            vgImageOpacity = vgCreateImage
                (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
        }
        VGfloat matrix[20] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, opacity,
            0.0f, 0.0f, 0.0f, 0.0f
        };
        vgColorMatrix(vgImageOpacity, vgImage, matrix);
        cachedOpacity = opacity;
    }

    return vgImageOpacity;
#else
    // vgColorMatrix() doesn't work with ShivaVG, so ignore the opacity.
    Q_UNUSED(opacity);
    return toVGImage();
#endif
}

extern int qt_defaultDpiX();
extern int qt_defaultDpiY();

int QVGPixmapData::metric(QPaintDevice::PaintDeviceMetric metric) const
{
    switch (metric) {
    case QPaintDevice::PdmWidth:
        return w;
    case QPaintDevice::PdmHeight:
        return h;
    case QPaintDevice::PdmNumColors:
        return 0;
    case QPaintDevice::PdmDepth:
        return d;
    case QPaintDevice::PdmWidthMM:
        return qRound(w * 25.4 / qt_defaultDpiX());
    case QPaintDevice::PdmHeightMM:
        return qRound(h * 25.4 / qt_defaultDpiY());
    case QPaintDevice::PdmDpiX:
    case QPaintDevice::PdmPhysicalDpiX:
        return qt_defaultDpiX();
    case QPaintDevice::PdmDpiY:
    case QPaintDevice::PdmPhysicalDpiY:
        return qt_defaultDpiY();
    default:
        qWarning("QVGPixmapData::metric(): Invalid metric");
        return 0;
    }
}

// Force the pixmap data to be in QImage format.
void QVGPixmapData::forceToImage()
{
    if (!isValid())
        return;

    if (source.isNull())
        source = QImage(w, h, sourceFormat());

    recreate = true;
}

QImage::Format QVGPixmapData::sourceFormat() const
{
    return QImage::Format_ARGB32_Premultiplied;
}

QT_END_NAMESPACE
