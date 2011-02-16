/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpixmapdata_vg_p.h"
#include "qvgfontglyphcache_p.h"
#include <private/qt_s60_p.h>

#include <fbs.h>
#include <bitdev.h>

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
#  include <sgresource/sgimage.h>
#  ifdef SYMBIAN_FBSERV_GLYPHDATA // defined in fbs.h
#    define QT_SYMBIAN_HARDWARE_GLYPH_CACHE
#    include <graphics/fbsglyphdataiterator.h>
#    include <private/qfontengine_s60_p.h>
#  endif
#endif

QT_BEGIN_NAMESPACE

typedef VGImage (*_vgCreateEGLImageTargetKHR)(VGeglImageKHR);
static _vgCreateEGLImageTargetKHR qt_vgCreateEGLImageTargetKHR = 0;

namespace QVG
{
    VGImage vgCreateEGLImageTargetKHR(VGeglImageKHR eglImage);
}

VGImage QVG::vgCreateEGLImageTargetKHR(VGeglImageKHR eglImage)
{
    if (!qt_vgCreateEGLImageTargetKHR && QEgl::hasExtension("EGL_KHR_image"))
        qt_vgCreateEGLImageTargetKHR = (_vgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

    return qt_vgCreateEGLImageTargetKHR ? qt_vgCreateEGLImageTargetKHR(eglImage) : 0;
}

extern int qt_vg_pixmap_serial;

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

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
static VGImage sgImageToVGImage(QEglContext *context, const RSgImage &sgImage)
{
    // when "0" used as argument then
    // default display, context are used
    if (!context)
        context = qt_vg_create_context(0, QInternal::Pixmap);

    VGImage vgImage = VG_INVALID_HANDLE;

    if (sgImage.IsNull())
        return vgImage;

    const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
    EGLImageKHR eglImage = QEgl::eglCreateImageKHR(QEgl::display(),
            EGL_NO_CONTEXT,
            EGL_NATIVE_PIXMAP_KHR,
            (EGLClientBuffer)&sgImage,
            (EGLint*)KEglImageAttribs);

    if (!eglImage)
        return vgImage;

    vgImage = QVG::vgCreateEGLImageTargetKHR(eglImage);

    QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
    return vgImage;
}
#endif

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

        VGImage vgImage = sgImageToVGImage(context, *sgImage);
        if (vgImage != VG_INVALID_HANDLE) {
            w = vgGetParameteri(vgImage, VG_IMAGE_WIDTH);
            h = vgGetParameteri(vgImage, VG_IMAGE_HEIGHT);
            d = 32; // We always use ARGB_Premultiplied for VG pixmaps.
        }

        is_null = (w <= 0 || h <= 0);
        source = QImage(); // vgGetImageSubData() some day?
        recreate = false;
        prevSize = QSize(w, h);
        //setSerialNumber(++qt_vg_pixmap_serial);
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
        int bytesPerLine = bitmap->ScanLineLength(size.iWidth, displayMode);

        bitmap->BeginDataAccess();
        uchar *bytes = (uchar*)bitmap->DataAddress();
        QImage img = QImage(bytes, size.iWidth, size.iHeight, bytesPerLine, format);
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

        QScopedPointer<RSgImage> sgImage(new RSgImage());
        err = sgImage->Create(sgInfo, NULL, NULL);
        if (err != KErrNone) {
            driver.Close();
            return 0;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = QEgl::eglCreateImageKHR(QEgl::display(),
                EGL_NO_CONTEXT,
                EGL_NATIVE_PIXMAP_KHR,
                (EGLClientBuffer)sgImage.data(),
                (EGLint*)KEglImageAttribs);
        if (!eglImage || eglGetError() != EGL_SUCCESS) {
            sgImage->Close();
            driver.Close();
            return 0;
        }

        VGImage dstVgImage = QVG::vgCreateEGLImageTargetKHR(eglImage);
        if (!dstVgImage || vgGetError() != VG_NO_ERROR) {
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
            sgImage.reset();
        }

        // release stuff
        vgDestroyImage(dstVgImage);
        QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
        driver.Close();
        return reinterpret_cast<void*>(sgImage.take());
#endif
    } else if (type == QPixmapData::FbsBitmap) {
                CFbsBitmap *bitmap = q_check_ptr(new CFbsBitmap);

        if (bitmap) {
            if (bitmap->Create(TSize(source.width(), source.height()),
                              EColor16MAP) == KErrNone) {
                const uchar *sptr = const_cast<const QImage&>(source).bits();
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

QSymbianVGFontGlyphCache::QSymbianVGFontGlyphCache() : QVGFontGlyphCache()
{
#ifdef QT_SYMBIAN_HARDWARE_GLYPH_CACHE
    invertedGlyphs = true;
#endif
}

void QSymbianVGFontGlyphCache::cacheGlyphs(QVGPaintEnginePrivate *d,
                                           QFontEngine *fontEngine,
                                           const glyph_t *g, int count)
{
#ifdef QT_SYMBIAN_HARDWARE_GLYPH_CACHE
    QFontEngineS60 *s60fontEngine = static_cast<QFontEngineS60*>(fontEngine);
    if (s60fontEngine->m_activeFont->TypeUid() != KCFbsFontUid)
            return QVGFontGlyphCache::cacheGlyphs(d, fontEngine, g, count);

    QVector<glyph_t> uncachedGlyphs;
    while (count-- > 0) {
        // Skip this glyph if we have already cached it before.
        glyph_t glyph = *g++;
        if (((glyph < 256) && ((cachedGlyphsMask[glyph / 32] & (1 << (glyph % 32))) != 0))
            || cachedGlyphs.contains(glyph))
            continue;
        if (!uncachedGlyphs.contains(glyph))
            uncachedGlyphs.append(glyph);
    }

    if (!uncachedGlyphs.isEmpty()) {
        CFbsFont *cfbsFont = static_cast<CFbsFont *>(s60fontEngine->m_activeFont);
        RFbsGlyphDataIterator iter;

        int err = iter.Open(*cfbsFont, (const unsigned int*)uncachedGlyphs.constData(), uncachedGlyphs.count());

        if (err == KErrNotSupported || err == KErrInUse) { // Fallback in possibly supported error cases
            iter.Close();
            qWarning("Falling back to default QVGFontGlyphCache");
            return QVGFontGlyphCache::cacheGlyphs(d, fontEngine, g, count);
        }

        for (; err == KErrNone; err = iter.Next()) {
            const unsigned int glyph = iter.GlyphCode();

            const RSgImage& image = iter.Image();
            const TOpenFontCharMetrics& metrics = iter.Metrics();

            TRect glyphBounds;
            metrics.GetHorizBounds(glyphBounds);
            VGImage vgImage = sgImageToVGImage(0, image);
            VGfloat origin[2];
            VGfloat escapement[2];
            origin[0] = -glyphBounds.iTl.iX;
            origin[1] = glyphBounds.iBr.iY;
            escapement[0] = 0;
            escapement[1] = 0;
            vgSetGlyphToImage(font, glyph, vgImage, origin, escapement);
            vgDestroyImage(vgImage);

            // Add to cache
            if (glyph < 256)
                cachedGlyphsMask[glyph / 32] |= (1 << (glyph % 32));
            else
                cachedGlyphs.insert(glyph);
        }
        iter.Close();

        if (err == KErrNoMemory || err == KErrNoGraphicsMemory)
            qWarning("Not enough memory to cache glyph");
        else if (err != KErrNotFound)
            qWarning("Received error %d from glyph cache", err);
    }
#else
    QVGFontGlyphCache::cacheGlyphs(d, fontEngine, g, count);
#endif
}

QT_END_NAMESPACE
