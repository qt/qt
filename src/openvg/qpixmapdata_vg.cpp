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

#if defined(Q_OS_SYMBIAN)
#include <private/qt_s60_p.h>
#include <fbs.h>
#endif
#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
#include <sgresource/sgimage.h>
typedef VGImage (*pfnVgCreateEGLImageTargetKHR)(VGeglImageKHR);
#endif // QT_SYMBIAN_SUPPORTS_SGIMAGE

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
        context = qt_vg_create_context(0, QInternal::Pixmap);
#endif

    if (recreate && prevSize != QSize(w, h))
        destroyImages();
    else if (recreate)
        cachedOpacity = -1.0f;  // Force opacity image to be refreshed later.

    if (vgImage == VG_INVALID_HANDLE) {
        vgImage = QVGImagePool::instance()->createImageForPixmap
            (VG_sARGB_8888_PRE, w, h, VG_IMAGE_QUALITY_FASTER, this);

        // Bail out if we run out of GPU memory - try again next time.
        if (vgImage == VG_INVALID_HANDLE)
            return VG_INVALID_HANDLE;

        inImagePool = true;
    } else if (inImagePool) {
        QVGImagePool::instance()->useImage(this);
    }

    if (!source.isNull() && recreate) {
        vgImageSubData
            (vgImage,
             source.constBits(), source.bytesPerLine(),
             VG_sARGB_8888_PRE, 0, 0, w, h);
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

#if defined(Q_OS_SYMBIAN)

static CFbsBitmap* createBlitCopy(CFbsBitmap* bitmap)
{
      CFbsBitmap *copy = q_check_ptr(new CFbsBitmap);
      if(!copy)
        return 0;

      if (copy->Create(bitmap->SizeInPixels(), bitmap->DisplayMode()) != KErrNone) {
          delete copy;
          copy = 0;

          return 0;
      }

      CFbsBitmapDevice* bitmapDevice = 0;
      CFbsBitGc *bitmapGc = 0;
      QT_TRAP_THROWING(bitmapDevice = CFbsBitmapDevice::NewL(copy));
      QT_TRAP_THROWING(bitmapGc = CFbsBitGc::NewL());
      bitmapGc->Activate(bitmapDevice);

      bitmapGc->BitBlt(TPoint(), bitmap);

      delete bitmapGc;
      delete bitmapDevice;

      return copy;
}

void QVGPixmapData::cleanup()
{
    is_null = w = h = 0;
    recreate = false;
    source = QImage();
}

void QVGPixmapData::fromNativeType(void* pixmap, NativeType type)
{
    if (type == QPixmapData::SgImage && pixmap) {
#if defined(QT_SYMBIAN_SUPPORTS_SGIMAGE) && !defined(QT_NO_EGL)
        RSgImage *sgImage = reinterpret_cast<RSgImage*>(pixmap);

        destroyImages();
        prevSize = QSize();

        TInt err = 0;

        RSgDriver driver;
        err = driver.Open();
        if (err != KErrNone) {
            cleanup();
            return;
        }

        if (sgImage->IsNull()) {
            cleanup();
            driver.Close();
            return;
        }

        TSgImageInfo sgImageInfo;
        err = sgImage->GetInfo(sgImageInfo);
        if (err != KErrNone) {
            cleanup();
            driver.Close();
            return;
        }

        pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

        if (eglGetError() != EGL_SUCCESS || !(QEgl::hasExtension("EGL_KHR_image") || QEgl::hasExtension("EGL_KHR_image_pixmap")) || !vgCreateEGLImageTargetKHR) {
            cleanup();
            driver.Close();
            return;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = QEgl::eglCreateImageKHR(QEgl::display(),
                EGL_NO_CONTEXT,
                EGL_NATIVE_PIXMAP_KHR,
                (EGLClientBuffer)sgImage,
                (EGLint*)KEglImageAttribs);

        if (eglGetError() != EGL_SUCCESS) {
            cleanup();
            driver.Close();
            return;
        }

        vgImage = vgCreateEGLImageTargetKHR(eglImage);
        if (vgGetError() != VG_NO_ERROR) {
            cleanup();
            QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
            driver.Close();
            return;
        }

        w = sgImageInfo.iSizeInPixels.iWidth;
        h = sgImageInfo.iSizeInPixels.iHeight;
        d = 32; // We always use ARGB_Premultiplied for VG pixmaps.
        is_null = (w <= 0 || h <= 0);
        source = QImage();
        recreate = false;
        prevSize = QSize(w, h);
        setSerialNumber(++qt_vg_pixmap_serial);
        // release stuff
        QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
        driver.Close();
#endif
    } else if (type == QPixmapData::FbsBitmap) {
        CFbsBitmap *bitmap = reinterpret_cast<CFbsBitmap*>(pixmap);

        bool deleteSourceBitmap = false;

#ifdef Q_SYMBIAN_HAS_EXTENDED_BITMAP_TYPE

        // Rasterize extended bitmaps

        TUid extendedBitmapType = bitmap->ExtendedBitmapType();
        if (extendedBitmapType != KNullUid) {
            bitmap = createBlitCopy(bitmap);
            deleteSourceBitmap = true;
        }
#endif

        if (bitmap->IsCompressedInRAM()) {
            bitmap = createBlitCopy(bitmap);
            deleteSourceBitmap = true;
        }

        TDisplayMode displayMode = bitmap->DisplayMode();
        QImage::Format format = qt_TDisplayMode2Format(displayMode);

        TSize size = bitmap->SizeInPixels();

        bitmap->BeginDataAccess();
        uchar *bytes = (uchar*)bitmap->DataAddress();
        QImage img = QImage(bytes, size.iWidth, size.iHeight, format);
        img = img.copy();
        bitmap->EndDataAccess();

        if(displayMode == EGray2) {
            //Symbian thinks set pixels are white/transparent, Qt thinks they are foreground/solid
            //So invert mono bitmaps so that masks work correctly.
            img.invertPixels();
        } else if(displayMode == EColor16M) {
            img = img.rgbSwapped(); // EColor16M is BGR
        }

        fromImage(img, Qt::AutoColor);

        if(deleteSourceBitmap)
            delete bitmap;
    }
}

void* QVGPixmapData::toNativeType(NativeType type)
{
    if (type == QPixmapData::SgImage) {
#if defined(QT_SYMBIAN_SUPPORTS_SGIMAGE) && !defined(QT_NO_EGL)
        toVGImage();

        if (!isValid() || vgImage == VG_INVALID_HANDLE)
            return 0;

        TInt err = 0;

        RSgDriver driver;
        err = driver.Open();
        if (err != KErrNone)
            return 0;

        TSgImageInfo sgInfo;
        sgInfo.iPixelFormat = EUidPixelFormatARGB_8888_PRE;
        sgInfo.iSizeInPixels.SetSize(w, h);
        sgInfo.iUsage = ESgUsageBitOpenVgImage | ESgUsageBitOpenVgSurface;

        RSgImage *sgImage = q_check_ptr(new RSgImage());
        err = sgImage->Create(sgInfo, NULL, NULL);
        if (err != KErrNone) {
            driver.Close();
            return 0;
        }

        pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

        if (eglGetError() != EGL_SUCCESS || !(QEgl::hasExtension("EGL_KHR_image") || QEgl::hasExtension("EGL_KHR_image_pixmap")) || !vgCreateEGLImageTargetKHR) {
            driver.Close();
            return 0;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = QEgl::eglCreateImageKHR(QEgl::display(),
                EGL_NO_CONTEXT,
                EGL_NATIVE_PIXMAP_KHR,
                (EGLClientBuffer)sgImage,
                (EGLint*)KEglImageAttribs);
        if (eglGetError() != EGL_SUCCESS) {
            sgImage->Close();
            driver.Close();
            return 0;
        }

        VGImage dstVgImage = vgCreateEGLImageTargetKHR(eglImage);
        if (vgGetError() != VG_NO_ERROR) {
            QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
            sgImage->Close();
            driver.Close();
            return 0;
        }

        vgCopyImage(dstVgImage, 0, 0,
                vgImage, 0, 0,
                w, h, VG_FALSE);

        if (vgGetError() != VG_NO_ERROR) {
            sgImage->Close();
            sgImage = 0;
        }
        // release stuff
        vgDestroyImage(dstVgImage);
        QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
        driver.Close();
        return reinterpret_cast<void*>(sgImage);
#endif
    } else if (type == QPixmapData::FbsBitmap) {
        CFbsBitmap *bitmap = q_check_ptr(new CFbsBitmap);

        if (bitmap) {
            if (bitmap->Create(TSize(source.width(), source.height()),
                              EColor16MAP) == KErrNone) {
                const uchar *sptr = source.constBits();
                bitmap->BeginDataAccess();

                uchar *dptr = (uchar*)bitmap->DataAddress();
                Mem::Copy(dptr, sptr, source.byteCount());

                bitmap->EndDataAccess();
            } else {
                delete bitmap;
                bitmap = 0;
            }
        }

        return reinterpret_cast<void*>(bitmap);
    }
    return 0;
}
#endif //Q_OS_SYMBIAN

QT_END_NAMESPACE
