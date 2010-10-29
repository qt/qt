/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpixmapdata_vg_p.h"
#include "qpaintengine_vg_p.h"
#include <QtGui/private/qdrawhelper_p.h>
#if !defined(QT_NO_EGL)
#include <QtGui/private/qegl_p.h>
#endif
#include "qvg_p.h"
#include "qvgimagepool_p.h"
#include <QBuffer>
#include <QImageReader>
#include <QtGui/private/qimage_p.h>

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
    inImagePool = false;
    inLRU = false;
    failedToAlloc = false;
#if !defined(QT_NO_EGL)
    context = 0;
    qt_vg_register_pixmap(this);
#endif
    setSerialNumber(++qt_vg_pixmap_serial);
}

QVGPixmapData::~QVGPixmapData()
{
    destroyImageAndContext();
#if !defined(QT_NO_EGL)
    qt_vg_unregister_pixmap(this);
#endif
}

void QVGPixmapData::destroyImages()
{
    if (inImagePool) {
        QVGImagePool *pool = QVGImagePool::instance();
        if (vgImage != VG_INVALID_HANDLE)
            pool->releaseImage(this, vgImage);
        if (vgImageOpacity != VG_INVALID_HANDLE)
            pool->releaseImage(this, vgImageOpacity);
    } else {
        if (vgImage != VG_INVALID_HANDLE)
            vgDestroyImage(vgImage);
        if (vgImageOpacity != VG_INVALID_HANDLE)
            vgDestroyImage(vgImageOpacity);
    }
    vgImage = VG_INVALID_HANDLE;
    vgImageOpacity = VG_INVALID_HANDLE;
    inImagePool = false;
}

void QVGPixmapData::destroyImageAndContext()
{
    if (vgImage != VG_INVALID_HANDLE) {
        // We need to have a context current to destroy the image.
#if !defined(QT_NO_EGL)
        if (context->isCurrent()) {
            destroyImages();
        } else {
            // We don't currently have a widget surface active, but we
            // need a surface to make the context current.  So use the
            // shared pbuffer surface instead.
            context->makeCurrent(qt_vg_shared_surface());
            destroyImages();
            context->lazyDoneCurrent();
        }
#else
        destroyImages();
#endif
    }
#if !defined(QT_NO_EGL)
    if (context) {
        qt_vg_destroy_context(context, QInternal::Pixmap);
        context = 0;
    }
#endif
    recreate = true;
}

QPixmapData *QVGPixmapData::createCompatiblePixmapData() const
{
    return new QVGPixmapData(pixelType());
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
    if(image.isNull())
        return;

    QImage img = image;
    createPixmapForImage(img, flags, false);
}

void QVGPixmapData::fromImageReader(QImageReader *imageReader,
                                 Qt::ImageConversionFlags flags)
{
    QImage image = imageReader->read();
    if (image.isNull())
        return;

    createPixmapForImage(image, flags, true);
}

bool QVGPixmapData::fromFile(const QString &filename, const char *format,
                          Qt::ImageConversionFlags flags)
{
    QImage image = QImageReader(filename, format).read();
    if (image.isNull())
        return false;

    createPixmapForImage(image, flags, true);

    return !isNull();
}

bool QVGPixmapData::fromData(const uchar *buffer, uint len, const char *format,
                      Qt::ImageConversionFlags flags)
{
    QByteArray a = QByteArray::fromRawData(reinterpret_cast<const char *>(buffer), len);
    QBuffer b(&a);
    b.open(QIODevice::ReadOnly);
    QImage image = QImageReader(&b, format).read();
    if (image.isNull())
        return false;

    createPixmapForImage(image, flags, true);

    return !isNull();
}

void QVGPixmapData::createPixmapForImage(QImage &image, Qt::ImageConversionFlags flags, bool inPlace)
{
    if (image.size() == QSize(w, h))
        setSerialNumber(++qt_vg_pixmap_serial);
    else
        resize(image.width(), image.height());

    QImage::Format format = sourceFormat();
    int d = image.depth();
    if (d == 1 || d == 16 || d == 24 || (d == 32 && !image.hasAlphaChannel()))
        format = QImage::Format_RGB32;
    else if (!(flags & Qt::NoOpaqueDetection) && const_cast<QImage &>(image).data_ptr()->checkForAlphaPixels())
        format = sourceFormat();
    else
        format = QImage::Format_RGB32;

    if (inPlace && image.data_ptr()->convertInPlace(format, flags))
        source = image;
    else
        source = image.convertToFormat(format);

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
    if (!isValid() || failedToAlloc)
        return VG_INVALID_HANDLE;

#if !defined(QT_NO_EGL)
    // Increase the reference count on the shared context.
    if (!context)
        context = qt_vg_create_context(0, QInternal::Pixmap);
#endif

    if (recreate && prevSize != QSize(w, h))
        destroyImages();
    else if (recreate)
        cachedOpacity = -1.0f;  // Force opacity image to be refreshed later.

    if (vgImage == VG_INVALID_HANDLE) {
        vgImage = QVGImagePool::instance()->createImageForPixmap
            (qt_vg_image_to_vg_format(source.format()), w, h, VG_IMAGE_QUALITY_FASTER, this);

        // Bail out if we run out of GPU memory - try again next time.
        if (vgImage == VG_INVALID_HANDLE) {
            failedToAlloc = true;
            return VG_INVALID_HANDLE;
        }

        inImagePool = true;
    } else if (inImagePool) {
        QVGImagePool::instance()->useImage(this);
    }

    if (!source.isNull() && recreate) {
        vgImageSubData
            (vgImage,
             source.constBits(), source.bytesPerLine(),
             qt_vg_image_to_vg_format(source.format()), 0, 0, w, h);
    }

    recreate = false;
    prevSize = QSize(w, h);

    return vgImage;
}

VGImage QVGPixmapData::toVGImage(qreal opacity)
{
#if !defined(QT_SHIVAVG)
    // Force the primary VG image to be recreated if necessary.
    if (toVGImage() == VG_INVALID_HANDLE)
        return VG_INVALID_HANDLE;

    if (opacity == 1.0f)
        return vgImage;

    // Create an alternative image for the selected opacity.
    if (vgImageOpacity == VG_INVALID_HANDLE || cachedOpacity != opacity) {
        if (vgImageOpacity == VG_INVALID_HANDLE) {
            if (inImagePool) {
                vgImageOpacity = QVGImagePool::instance()->createImageForPixmap
                    (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER, this);
            } else {
                vgImageOpacity = vgCreateImage
                    (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER);
            }

            // Bail out if we run out of GPU memory - try again next time.
            if (vgImageOpacity == VG_INVALID_HANDLE)
                return VG_INVALID_HANDLE;
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

void QVGPixmapData::detachImageFromPool()
{
    if (inImagePool) {
        QVGImagePool::instance()->detachImage(this);
        inImagePool = false;
    }
}

void QVGPixmapData::hibernate()
{
    // If the image was imported (e.g, from an SgImage under Symbian),
    // then we cannot copy it back to main memory for storage.
    if (vgImage != VG_INVALID_HANDLE && source.isNull())
        return;

    forceToImage();
    destroyImageAndContext();
}

void QVGPixmapData::reclaimImages()
{
    if (!inImagePool)
        return;
    forceToImage();
    destroyImages();
}

Q_DECL_IMPORT extern int qt_defaultDpiX();
Q_DECL_IMPORT extern int qt_defaultDpiY();

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

/*
    \internal

    Returns the VGImage that is storing the contents of \a pixmap.
    Returns VG_INVALID_HANDLE if \a pixmap is not owned by the OpenVG
    graphics system or \a pixmap is invalid.

    This function is typically used to access the backing store
    for a pixmap when executing raw OpenVG calls.  It must only
    be used when a QPainter is active and the OpenVG paint engine
    is in use by the QPainter.

    \sa {QtOpenVG Module}
*/
Q_OPENVG_EXPORT VGImage qPixmapToVGImage(const QPixmap& pixmap)
{
    QPixmapData *pd = pixmap.pixmapData();
    if (!pd)
        return VG_INVALID_HANDLE; // null QPixmap
    if (pd->classId() == QPixmapData::OpenVGClass) {
        QVGPixmapData *vgpd = static_cast<QVGPixmapData *>(pd);
        if (vgpd->isValid())
            return vgpd->toVGImage();
    }
    return VG_INVALID_HANDLE;
}

QT_END_NAMESPACE
