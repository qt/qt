/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <private/qdrawhelper_x86_p.h>

#ifdef QT_HAVE_SSE2

#include <private/qpaintengine_raster_p.h>

#ifdef QT_LINUXBASE
// this is an evil hack - the posix_memalign declaration in LSB
// is wrong - see http://bugs.linuxbase.org/show_bug.cgi?id=2431
#  define posix_memalign _lsb_hack_posix_memalign
#  include <emmintrin.h>
#  undef posix_memalign
#else
#  include <emmintrin.h>
#endif

QT_BEGIN_NAMESPACE

/*
 * Multiply the components of pixelVector by alphaChannel
 * Each 32bits components of alphaChannel must be in the form 0x00AA00AA
 * colorMask must have 0x00ff00ff on each 32 bits component
 * half must have the value 128 (0x80) for each 32 bits compnent
 */
#define BYTE_MUL_SSE2(result, pixelVector, alphaChannel, colorMask, half) \
{ \
    /* 1. separate the colors in 2 vectors so each color is on 16 bits \
       (in order to be multiplied by the alpha \
       each 32 bit of dstVectorAG are in the form 0x00AA00GG \
       each 32 bit of dstVectorRB are in the form 0x00RR00BB */\
    __m128i pixelVectorAG = _mm_srli_epi16(pixelVector, 8); \
    __m128i pixelVectorRB = _mm_and_si128(pixelVector, colorMask); \
 \
    /* 2. multiply the vectors by the alpha channel */\
    pixelVectorAG = _mm_mullo_epi16(pixelVectorAG, alphaChannel); \
    pixelVectorRB = _mm_mullo_epi16(pixelVectorRB, alphaChannel); \
 \
    /* 3. devide by 255, that's the tricky part. \
       we do it like for BYTE_MUL(), with bit shift: X/255 ~= (X + X/256 + rounding)/256 */ \
    /** so first (X + X/256 + rounding) */\
    pixelVectorRB = _mm_add_epi16(pixelVectorRB, _mm_srli_epi16(pixelVectorRB, 8)); \
    pixelVectorRB = _mm_add_epi16(pixelVectorRB, half); \
    pixelVectorAG = _mm_add_epi16(pixelVectorAG, _mm_srli_epi16(pixelVectorAG, 8)); \
    pixelVectorAG = _mm_add_epi16(pixelVectorAG, half); \
 \
    /** second devide by 256 */\
    pixelVectorRB = _mm_srli_epi16(pixelVectorRB, 8); \
    /** for AG, we could >> 8 to divide followed by << 8 to put the \
        bytes in the correct position. By masking instead, we execute \
        only one instruction */\
    pixelVectorAG = _mm_andnot_si128(colorMask, pixelVectorAG); \
 \
    /* 4. combine the 2 pairs of colors */ \
    result = _mm_or_si128(pixelVectorAG, pixelVectorRB); \
}

/*
 * Each 32bits components of alphaChannel must be in the form 0x00AA00AA
 * oneMinusAlphaChannel must be 255 - alpha for each 32 bits component
 * colorMask must have 0x00ff00ff on each 32 bits component
 * half must have the value 128 (0x80) for each 32 bits compnent
 */
#define INTERPOLATE_PIXEL_255_SSE2(result, srcVector, dstVector, alphaChannel, oneMinusAlphaChannel, colorMask, half) { \
    /* interpolate AG */\
    __m128i srcVectorAG = _mm_srli_epi16(srcVector, 8); \
    __m128i dstVectorAG = _mm_srli_epi16(dstVector, 8); \
    __m128i srcVectorAGalpha = _mm_mullo_epi16(srcVectorAG, alphaChannel); \
    __m128i dstVectorAGoneMinusAlphalpha = _mm_mullo_epi16(dstVectorAG, oneMinusAlphaChannel); \
    __m128i finalAG = _mm_add_epi16(srcVectorAGalpha, dstVectorAGoneMinusAlphalpha); \
    finalAG = _mm_add_epi16(finalAG, _mm_srli_epi16(finalAG, 8)); \
    finalAG = _mm_add_epi16(finalAG, half); \
    finalAG = _mm_andnot_si128(colorMask, finalAG); \
 \
    /* interpolate RB */\
    __m128i srcVectorRB = _mm_and_si128(srcVector, colorMask); \
    __m128i dstVectorRB = _mm_and_si128(dstVector, colorMask); \
    __m128i srcVectorRBalpha = _mm_mullo_epi16(srcVectorRB, alphaChannel); \
    __m128i dstVectorRBoneMinusAlphalpha = _mm_mullo_epi16(dstVectorRB, oneMinusAlphaChannel); \
    __m128i finalRB = _mm_add_epi16(srcVectorRBalpha, dstVectorRBoneMinusAlphalpha); \
    finalRB = _mm_add_epi16(finalRB, _mm_srli_epi16(finalRB, 8)); \
    finalRB = _mm_add_epi16(finalRB, half); \
    finalRB = _mm_srli_epi16(finalRB, 8); \
 \
    /* combine */\
    result = _mm_or_si128(finalAG, finalRB); \
}

// Basically blend src over dst with the const alpha defined as constAlphaVector.
// nullVector, half, one, colorMask are constant accross the whole image/texture, and should be defined as:
//const __m128i nullVector = _mm_set1_epi32(0);
//const __m128i half = _mm_set1_epi16(0x80);
//const __m128i one = _mm_set1_epi16(0xff);
//const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
//const __m128i alphaMask = _mm_set1_epi32(0xff000000);
//
// The computation being done is:
// result = s + d * (1-alpha)
// with shortcuts if fully opaque or fully transparent.
#define BLEND_SOURCE_OVER_ARGB32_SSE2(dst, src, length, nullVector, half, one, colorMask, alphaMask) { \
    int x = 0; \
    for (; x < length-3; x += 4) { \
        const __m128i srcVector = _mm_loadu_si128((__m128i *)&src[x]); \
        const __m128i srcVectorAlpha = _mm_and_si128(srcVector, alphaMask); \
        if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVectorAlpha, alphaMask)) == 0xffff) { \
            /* all opaque */ \
            _mm_storeu_si128((__m128i *)&dst[x], srcVector); \
        } else if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVectorAlpha, nullVector)) != 0xffff) { \
            /* not fully transparent */ \
            /* extract the alpha channel on 2 x 16 bits */ \
            /* so we have room for the multiplication */ \
            /* each 32 bits will be in the form 0x00AA00AA */ \
            /* with A being the 1 - alpha */ \
            __m128i alphaChannel = _mm_srli_epi32(srcVector, 24); \
            alphaChannel = _mm_or_si128(alphaChannel, _mm_slli_epi32(alphaChannel, 16)); \
            alphaChannel = _mm_sub_epi16(one, alphaChannel); \
 \
            const __m128i dstVector = _mm_loadu_si128((__m128i *)&dst[x]); \
            __m128i destMultipliedByOneMinusAlpha; \
            BYTE_MUL_SSE2(destMultipliedByOneMinusAlpha, dstVector, alphaChannel, colorMask, half); \
 \
            /* result = s + d * (1-alpha) */\
            const __m128i result = _mm_add_epi8(srcVector, destMultipliedByOneMinusAlpha); \
            _mm_storeu_si128((__m128i *)&dst[x], result); \
        } \
    } \
    for (; x < length; ++x) { \
        uint s = src[x]; \
        if (s >= 0xff000000) \
            dst[x] = s; \
        else if (s != 0) \
            dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s)); \
    } \
}

// Basically blend src over dst with the const alpha defined as constAlphaVector.
// nullVector, half, one, colorMask are constant accross the whole image/texture, and should be defined as:
//const __m128i nullVector = _mm_set1_epi32(0);
//const __m128i half = _mm_set1_epi16(0x80);
//const __m128i one = _mm_set1_epi16(0xff);
//const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
//
// The computation being done is:
// dest = (s + d * sia) * ca + d * cia
//      = s * ca + d * (sia * ca + cia)
//      = s * ca + d * (1 - sa*ca)
#define BLEND_SOURCE_OVER_ARGB32_WITH_CONST_ALPHA_SSE2(dst, src, length, nullVector, half, one, colorMask, constAlphaVector) \
{ \
    int x = 0; \
    for (; x < length-3; x += 4) { \
        __m128i srcVector = _mm_loadu_si128((__m128i *)&src[x]); \
        if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVector, nullVector)) != 0xffff) { \
            BYTE_MUL_SSE2(srcVector, srcVector, constAlphaVector, colorMask, half); \
\
            __m128i alphaChannel = _mm_srli_epi32(srcVector, 24); \
            alphaChannel = _mm_or_si128(alphaChannel, _mm_slli_epi32(alphaChannel, 16)); \
            alphaChannel = _mm_sub_epi16(one, alphaChannel); \
 \
            const __m128i dstVector = _mm_loadu_si128((__m128i *)&dst[x]); \
            __m128i destMultipliedByOneMinusAlpha; \
            BYTE_MUL_SSE2(destMultipliedByOneMinusAlpha, dstVector, alphaChannel, colorMask, half); \
 \
            const __m128i result = _mm_add_epi8(srcVector, destMultipliedByOneMinusAlpha); \
            _mm_storeu_si128((__m128i *)&dst[x], result); \
        } \
    } \
    for (; x < length; ++x) { \
        quint32 s = src[x]; \
        if (s != 0) { \
            s = BYTE_MUL(s, const_alpha); \
            dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s)); \
        } \
    } \
}

void qt_blend_argb32_on_argb32_sse2(uchar *destPixels, int dbpl,
                                    const uchar *srcPixels, int sbpl,
                                    int w, int h,
                                    int const_alpha)
{
    const quint32 *src = (const quint32 *) srcPixels;
    quint32 *dst = (quint32 *) destPixels;
    if (const_alpha == 256) {
        const __m128i alphaMask = _mm_set1_epi32(0xff000000);
        const __m128i nullVector = _mm_set1_epi32(0);
        const __m128i half = _mm_set1_epi16(0x80);
        const __m128i one = _mm_set1_epi16(0xff);
        const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
        for (int y = 0; y < h; ++y) {
            BLEND_SOURCE_OVER_ARGB32_SSE2(dst, src, w, nullVector, half, one, colorMask, alphaMask);
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    } else if (const_alpha != 0) {
        // dest = (s + d * sia) * ca + d * cia
        //      = s * ca + d * (sia * ca + cia)
        //      = s * ca + d * (1 - sa*ca)
        const_alpha = (const_alpha * 255) >> 8;
        const __m128i nullVector = _mm_set1_epi32(0);
        const __m128i half = _mm_set1_epi16(0x80);
        const __m128i one = _mm_set1_epi16(0xff);
        const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
        const __m128i constAlphaVector = _mm_set1_epi16(const_alpha);
        for (int y = 0; y < h; ++y) {
            BLEND_SOURCE_OVER_ARGB32_WITH_CONST_ALPHA_SSE2(dst, src, w, nullVector, half, one, colorMask, constAlphaVector)
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    }
}

// qblendfunctions.cpp
void qt_blend_rgb32_on_rgb32(uchar *destPixels, int dbpl,
                             const uchar *srcPixels, int sbpl,
                             int w, int h,
                             int const_alpha);

void qt_blend_rgb32_on_rgb32_sse2(uchar *destPixels, int dbpl,
                                 const uchar *srcPixels, int sbpl,
                                 int w, int h,
                                 int const_alpha)
{
    const quint32 *src = (const quint32 *) srcPixels;
    quint32 *dst = (quint32 *) destPixels;
    if (const_alpha != 256) {
        if (const_alpha != 0) {
            const __m128i nullVector = _mm_set1_epi32(0);
            const __m128i half = _mm_set1_epi16(0x80);
            const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);

            const_alpha = (const_alpha * 255) >> 8;
            int one_minus_const_alpha = 255 - const_alpha;
            const __m128i constAlphaVector = _mm_set1_epi16(const_alpha);
            const __m128i oneMinusConstAlpha =  _mm_set1_epi16(one_minus_const_alpha);
            for (int y = 0; y < h; ++y) {
                int x = 0;
                for (; x < w-3; x += 4) {
                    __m128i srcVector = _mm_loadu_si128((__m128i *)&src[x]);
                    if (_mm_movemask_epi8(_mm_cmpeq_epi32(srcVector, nullVector)) != 0xffff) {
                        const __m128i dstVector = _mm_loadu_si128((__m128i *)&dst[x]);
                        __m128i result;
                        INTERPOLATE_PIXEL_255_SSE2(result, srcVector, dstVector, constAlphaVector, oneMinusConstAlpha, colorMask, half);
                        _mm_storeu_si128((__m128i *)&dst[x], result);
                    }
                }
                for (; x<w; ++x) {
                    quint32 s = src[x];
                    s = BYTE_MUL(s, const_alpha);
                    dst[x] = INTERPOLATE_PIXEL_255(src[x], const_alpha, dst[x], one_minus_const_alpha);
                }
                dst = (quint32 *)(((uchar *) dst) + dbpl);
                src = (const quint32 *)(((const uchar *) src) + sbpl);
            }
        }
    } else {
        qt_blend_rgb32_on_rgb32(destPixels, dbpl, srcPixels, sbpl, w, h, const_alpha);
    }
}

void QT_FASTCALL comp_func_SourceOver_sse2(uint *destPixels, const uint *srcPixels, int length, uint const_alpha)
{
    Q_ASSERT(const_alpha >= 0);
    Q_ASSERT(const_alpha < 256);

    const quint32 *src = (const quint32 *) srcPixels;
    quint32 *dst = (quint32 *) destPixels;

    const __m128i nullVector = _mm_set1_epi32(0);
    const __m128i half = _mm_set1_epi16(0x80);
    const __m128i one = _mm_set1_epi16(0xff);
    const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
    if (const_alpha == 255) {
        const __m128i alphaMask = _mm_set1_epi32(0xff000000);
        BLEND_SOURCE_OVER_ARGB32_SSE2(dst, src, length, nullVector, half, one, colorMask, alphaMask);
    } else {
        const __m128i constAlphaVector = _mm_set1_epi16(const_alpha);
        BLEND_SOURCE_OVER_ARGB32_WITH_CONST_ALPHA_SSE2(dst, src, length, nullVector, half, one, colorMask, constAlphaVector);
    }
}

void qt_memfill32_sse2(quint32 *dest, quint32 value, int count)
{
    if (count < 7) {
        switch (count) {
        case 6: *dest++ = value;
        case 5: *dest++ = value;
        case 4: *dest++ = value;
        case 3: *dest++ = value;
        case 2: *dest++ = value;
        case 1: *dest   = value;
        }
        return;
    };

    const int align = (quintptr)(dest) & 0xf;
    switch (align) {
    case 4:  *dest++ = value; --count;
    case 8:  *dest++ = value; --count;
    case 12: *dest++ = value; --count;
    }

    int count128 = count / 4;
    __m128i *dst128 = reinterpret_cast<__m128i*>(dest);
    const __m128i value128 = _mm_set_epi32(value, value, value, value);

    int n = (count128 + 3) / 4;
    switch (count128 & 0x3) {
    case 0: do { _mm_store_si128(dst128++, value128);
    case 3:      _mm_store_si128(dst128++, value128);
    case 2:      _mm_store_si128(dst128++, value128);
    case 1:      _mm_store_si128(dst128++, value128);
    } while (--n > 0);
    }

    const int rest = count & 0x3;
    if (rest) {
        switch (rest) {
        case 3: dest[count - 3] = value;
        case 2: dest[count - 2] = value;
        case 1: dest[count - 1] = value;
        }
    }
}

void QT_FASTCALL comp_func_solid_SourceOver_sse2(uint *destPixels, int length, uint color, uint const_alpha)
{
    if ((const_alpha & qAlpha(color)) == 255) {
        qt_memfill32_sse2(destPixels, color, length);
    } else {
        if (const_alpha != 255)
            color = BYTE_MUL(color, const_alpha);

        const quint32 minusAlphaOfColor = qAlpha(~color);
        int x = 0;

        quint32 *dst = (quint32 *) destPixels;
        const __m128i colorVector = _mm_set1_epi32(color);
        const __m128i colorMask = _mm_set1_epi32(0x00ff00ff);
        const __m128i half = _mm_set1_epi16(0x80);
        const __m128i minusAlphaOfColorVector = _mm_set1_epi16(minusAlphaOfColor);

        for (; x < length-3; x += 4) {
            __m128i dstVector = _mm_loadu_si128((__m128i *)&dst[x]);
            BYTE_MUL_SSE2(dstVector, dstVector, minusAlphaOfColorVector, colorMask, half);
            dstVector = _mm_add_epi8(colorVector, dstVector);
            _mm_storeu_si128((__m128i *)&dst[x], dstVector);
        }
        for (;x < length; ++x)
            destPixels[x] = color + BYTE_MUL(destPixels[x], minusAlphaOfColor);
    }
}

void qt_memfill16_sse2(quint16 *dest, quint16 value, int count)
{
    if (count < 3) {
        switch (count) {
        case 2: *dest++ = value;
        case 1: *dest = value;
        }
        return;
    }

    const int align = (quintptr)(dest) & 0x3;
    switch (align) {
    case 2: *dest++ = value; --count;
    }

    const quint32 value32 = (value << 16) | value;
    qt_memfill32_sse2(reinterpret_cast<quint32*>(dest), value32, count / 2);

    if (count & 0x1)
        dest[count - 1] = value;
}

void qt_bitmapblit32_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride)
{
    quint32 *dest = reinterpret_cast<quint32*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint32);

    const __m128i c128 = _mm_set1_epi32(color);
    const __m128i maskmask1 = _mm_set_epi32(0x10101010, 0x20202020,
                                            0x40404040, 0x80808080);
    const __m128i maskadd1 = _mm_set_epi32(0x70707070, 0x60606060,
                                           0x40404040, 0x00000000);

    if (width > 4) {
        const __m128i maskmask2 = _mm_set_epi32(0x01010101, 0x02020202,
                                                0x04040404, 0x08080808);
        const __m128i maskadd2 = _mm_set_epi32(0x7f7f7f7f, 0x7e7e7e7e,
                                               0x7c7c7c7c, 0x78787878);
        while (height--) {
            for (int x = 0; x < width; x += 8) {
                const quint8 s = src[x >> 3];
                if (!s)
                    continue;
                __m128i mask1 = _mm_set1_epi8(s);
                __m128i mask2 = mask1;

                mask1 = _mm_and_si128(mask1, maskmask1);
                mask1 = _mm_add_epi8(mask1, maskadd1);
                _mm_maskmoveu_si128(c128, mask1, (char*)(dest + x));
                mask2 = _mm_and_si128(mask2, maskmask2);
                mask2 = _mm_add_epi8(mask2, maskadd2);
                _mm_maskmoveu_si128(c128, mask2, (char*)(dest + x + 4));
            }
            dest += destStride;
            src += stride;
        }
    } else {
        while (height--) {
            const quint8 s = *src;
            if (s) {
                __m128i mask1 = _mm_set1_epi8(s);
                mask1 = _mm_and_si128(mask1, maskmask1);
                mask1 = _mm_add_epi8(mask1, maskadd1);
                _mm_maskmoveu_si128(c128, mask1, (char*)(dest));
            }
            dest += destStride;
            src += stride;
        }
    }
}

void qt_bitmapblit16_sse2(QRasterBuffer *rasterBuffer, int x, int y,
                          quint32 color,
                          const uchar *src, int width, int height, int stride)
{
    const quint16 c = qt_colorConvert<quint16, quint32>(color, 0);
    quint16 *dest = reinterpret_cast<quint16*>(rasterBuffer->scanLine(y)) + x;
    const int destStride = rasterBuffer->bytesPerLine() / sizeof(quint16);

    const __m128i c128 = _mm_set1_epi16(c);
#if defined(Q_CC_MSVC)
#  pragma warning(disable: 4309) // truncation of constant value
#endif
    const __m128i maskmask = _mm_set_epi16(0x0101, 0x0202, 0x0404, 0x0808,
                                           0x1010, 0x2020, 0x4040, 0x8080);
    const __m128i maskadd = _mm_set_epi16(0x7f7f, 0x7e7e, 0x7c7c, 0x7878,
                                          0x7070, 0x6060, 0x4040, 0x0000);

    while (height--) {
        for (int x = 0; x < width; x += 8) {
            const quint8 s = src[x >> 3];
            if (!s)
                continue;
            __m128i mask = _mm_set1_epi8(s);
            mask = _mm_and_si128(mask, maskmask);
            mask = _mm_add_epi8(mask, maskadd);
            _mm_maskmoveu_si128(c128, mask, (char*)(dest + x));
        }
        dest += destStride;
        src += stride;
    }
}

QT_END_NAMESPACE

#endif // QT_HAVE_SSE2
