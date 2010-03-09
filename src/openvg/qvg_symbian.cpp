/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#include "qvgfontglyphcache_p.h"

#ifdef QT_SYMBIAN_SUPPORTS_SGIMAGE
#include <private/qt_s60_p.h>
#include <fbs.h>
#include <gdi.h>
#include <sgresource/sgimage.h>
typedef EGLImageKHR (*pfnEglCreateImageKHR)(EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, EGLint*);
typedef EGLBoolean (*pfnEglDestroyImageKHR)(EGLDisplay, EGLImageKHR);
typedef VGImage (*pfnVgCreateEGLImageTargetKHR)(VGeglImageKHR);
#ifdef SYMBIAN_GDI_GLYPHDATA // defined in gdi.h
#define QT_SYMBIAN_HARDWARE_GLYPH_CACHE
#include <graphics/glyphdataiterator.h>
#include <private/qfontengine_s60_p.h>
#endif
#endif

QT_BEGIN_NAMESPACE

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

static VGImage sgImageToVGImage(QEglContext *context, const RSgImage &sgImage)
{
    // when "0" used as argument then
    // default display, context are used
    if (!context)
        context = qt_vg_create_context(0, QInternal::Pixmap);

    VGImage vgImage = VG_INVALID_HANDLE;

    TInt err = 0;

    RSgDriver driver;
    err = driver.Open();
    if (err != KErrNone) {
        return vgImage;
    }

    if (sgImage.IsNull()) {
        driver.Close();
        return vgImage;
    }

    TSgImageInfo sgImageInfo;
    err = sgImage.GetInfo(sgImageInfo);
    if (err != KErrNone) {
        driver.Close();
        return vgImage;
    }

    pfnEglCreateImageKHR eglCreateImageKHR = (pfnEglCreateImageKHR) eglGetProcAddress("eglCreateImageKHR");
    pfnEglDestroyImageKHR eglDestroyImageKHR = (pfnEglDestroyImageKHR) eglGetProcAddress("eglDestroyImageKHR");
    pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

    if (eglGetError() != EGL_SUCCESS || !eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateEGLImageTargetKHR) {
        driver.Close();
        return vgImage;
    }

    const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
    EGLImageKHR eglImage = eglCreateImageKHR(context->display(),
            EGL_NO_CONTEXT,
            EGL_NATIVE_PIXMAP_KHR,
            (EGLClientBuffer)&sgImage,
            (EGLint*)KEglImageAttribs);

    if (eglGetError() != EGL_SUCCESS) {
        driver.Close();
        return vgImage;
    }

    vgImage = vgCreateEGLImageTargetKHR(eglImage);
    if (vgGetError() != VG_NO_ERROR) {
        eglDestroyImageKHR(context->display(), eglImage);
        driver.Close();
        return vgImage;
    }

    //setSerialNumber(++qt_vg_pixmap_serial);
    // release stuff
    eglDestroyImageKHR(context->display(), eglImage);
    driver.Close();
    return vgImage;
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

        pfnEglCreateImageKHR eglCreateImageKHR = (pfnEglCreateImageKHR) eglGetProcAddress("eglCreateImageKHR");
        pfnEglDestroyImageKHR eglDestroyImageKHR = (pfnEglDestroyImageKHR) eglGetProcAddress("eglDestroyImageKHR");
        pfnVgCreateEGLImageTargetKHR vgCreateEGLImageTargetKHR = (pfnVgCreateEGLImageTargetKHR) eglGetProcAddress("vgCreateEGLImageTargetKHR");

        if (eglGetError() != EGL_SUCCESS || !eglCreateImageKHR || !eglDestroyImageKHR || !vgCreateEGLImageTargetKHR) {
            driver.Close();
            return 0;
        }

        const EGLint KEglImageAttribs[] = {EGL_IMAGE_PRESERVED_SYMBIAN, EGL_TRUE, EGL_NONE};
        EGLImageKHR eglImage = eglCreateImageKHR(context->display(),
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
            eglDestroyImageKHR(context->display(), eglImage);
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
        eglDestroyImageKHR(context->display(), eglImage);
        driver.Close();
        return reinterpret_cast<void*>(sgImage);
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
                                           const QTextItemInt &ti,
                                           const QVarLengthArray<glyph_t> &glyphs)
{
#ifdef QT_SYMBIAN_HARDWARE_GLYPH_CACHE
    QFontEngineS60 *fontEngine = static_cast<QFontEngineS60*>(ti.fontEngine);
    CFont *cfont = fontEngine->m_activeFont;

//    QVarLengthArray<glyph_t, 256> adjustedGlyphs(glyphs);
//    for (int i = 0; i < glyphs.count(); ++i)
//        adjustedGlyphs[i] |= 0x80000000;

    RGlyphDataIterator iter;
    int err = iter.Open(*cfont, (const unsigned int*)glyphs.constData(), glyphs.count());

    if (err == KErrNotSupported || err == KErrInUse) { // Fallback in possibly supported error cases
        iter.Close();
        qWarning("Falling back to default QVGFontGlyphCache");
        return QVGFontGlyphCache::cacheGlyphs(d, ti, glyphs);
    }

    for (; err == KErrNone; err = iter.Next()) {
        // Skip this glyph if we have already cached it before.
        const unsigned int glyph = iter.GlyphCode();
        if (((glyph < 256) && ((cachedGlyphsMask[glyph / 32] & (1 << (glyph % 32))) != 0))
            || cachedGlyphs.contains(glyph))
                continue;

        const RSgImage& image = iter.Image();
        const TOpenFontCharMetrics& metrics = iter.Metrics();

        TRect glyphBounds;
        metrics.GetHorizBounds(glyphBounds);
        VGImage vgImage = sgImageToVGImage(0, image);
        VGfloat origin[2];
        VGfloat escapement[2];
        origin[0] = -glyphBounds.iTl.iX + 0.5f;
        origin[1] = glyphBounds.iBr.iY + 0.5f;
        escapement[0] = metrics.HorizAdvance();
        escapement[1] = 0;
        vgSetGlyphToImage(font, iter.GlyphCode(), vgImage, origin, escapement);
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
#else
    QVGFontGlyphCache::cacheGlyphs(d, ti, glyphs);
#endif
}

QT_END_NAMESPACE
