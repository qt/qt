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

#include <private/qdrawhelper_p.h>

#ifdef QT_HAVE_NEON

#include <private/qdrawhelper_neon_p.h>
#include <arm_neon.h>

QT_BEGIN_NAMESPACE

static inline uint16x8_t qvdiv_255_u16(uint16x8_t x, uint16x8_t half)
{
    // result = (x + (x >> 8) + 0x80) >> 8

    const uint16x8_t temp = vshrq_n_u16(x, 8); // x >> 8
    const uint16x8_t sum_part = vaddq_u16(x, half); // x + 0x80
    const uint16x8_t sum = vaddq_u16(temp, sum_part);

    return vshrq_n_u16(sum, 8);
}

static inline uint16x8_t qvbyte_mul_u16(uint16x8_t x, uint16x8_t alpha, uint16x8_t half)
{
    // t = qRound(x * alpha / 255.0)

    const uint16x8_t t = vmulq_u16(x, alpha); // t
    return qvdiv_255_u16(t, half);
}

static inline uint16x8_t qvinterpolate_pixel_255(uint16x8_t x, uint16x8_t a, uint16x8_t y, uint16x8_t b, uint16x8_t half)
{
    // t = x * a + y * b

    const uint16x8_t ta = vmulq_u16(x, a);
    const uint16x8_t tb = vmulq_u16(y, b);

    return qvdiv_255_u16(vaddq_u16(ta, tb), half);
}

static inline uint16x8_t qvsource_over_u16(uint16x8_t src16, uint16x8_t dst16, uint16x8_t half, uint16x8_t full)
{
    const uint16x4_t alpha16_high = vdup_lane_u16(vget_high_u16(src16), 3);
    const uint16x4_t alpha16_low = vdup_lane_u16(vget_low_u16(src16), 3);

    const uint16x8_t alpha16 = vsubq_u16(full, vcombine_u16(alpha16_low, alpha16_high));

    return vaddq_u16(src16, qvbyte_mul_u16(dst16, alpha16, half));
}

void qt_blend_argb32_on_argb32_neon(uchar *destPixels, int dbpl,
                                    const uchar *srcPixels, int sbpl,
                                    int w, int h,
                                    int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;
    uint16x8_t half = vdupq_n_u16(0x80);
    uint16x8_t full = vdupq_n_u16(0xff);
    if (const_alpha == 256) {
        for (int y = 0; y < h; ++y) {
            int x = 0;
            for (; x < w-3; x += 4) {
                uint32x4_t src32 = vld1q_u32((uint32_t *)&src[x]);
                if ((src[x] & src[x+1] & src[x+2] & src[x+3]) >= 0xff000000) {
                    // all opaque
                    vst1q_u32((uint32_t *)&dst[x], src32);
                } else if (src[x] | src[x+1] | src[x+2] | src[x+3]) {
                    uint32x4_t dst32 = vld1q_u32((uint32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_u32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_u32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const uint16x8_t src16_low = vmovl_u8(src8_low);
                    const uint16x8_t dst16_low = vmovl_u8(dst8_low);

                    const uint16x8_t src16_high = vmovl_u8(src8_high);
                    const uint16x8_t dst16_high = vmovl_u8(dst8_high);

                    const uint16x8_t result16_low = qvsource_over_u16(src16_low, dst16_low, half, full);
                    const uint16x8_t result16_high = qvsource_over_u16(src16_high, dst16_high, half, full);

                    const uint32x2_t result32_low = vreinterpret_u32_u8(vmovn_u16(result16_low));
                    const uint32x2_t result32_high = vreinterpret_u32_u8(vmovn_u16(result16_high));

                    vst1q_u32((uint32_t *)&dst[x], vcombine_u32(result32_low, result32_high));
                }
            }
            for (; x<w; ++x) {
                uint s = src[x];
                if (s >= 0xff000000)
                    dst[x] = s;
                else if (s != 0)
                    dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s));
            }
            dst = (quint32 *)(((uchar *) dst) + dbpl);
            src = (const quint32 *)(((const uchar *) src) + sbpl);
        }
    } else if (const_alpha != 0) {
        const_alpha = (const_alpha * 255) >> 8;
        uint16x8_t const_alpha16 = vdupq_n_u16(const_alpha);
        for (int y = 0; y < h; ++y) {
            int x = 0;
            for (; x < w-3; x += 4) {
                if (src[x] | src[x+1] | src[x+2] | src[x+3]) {
                    uint32x4_t src32 = vld1q_u32((uint32_t *)&src[x]);
                    uint32x4_t dst32 = vld1q_u32((uint32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_u32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_u32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const uint16x8_t src16_low = vmovl_u8(src8_low);
                    const uint16x8_t dst16_low = vmovl_u8(dst8_low);

                    const uint16x8_t src16_high = vmovl_u8(src8_high);
                    const uint16x8_t dst16_high = vmovl_u8(dst8_high);

                    const uint16x8_t srcalpha16_low = qvbyte_mul_u16(src16_low, const_alpha16, half);
                    const uint16x8_t srcalpha16_high = qvbyte_mul_u16(src16_high, const_alpha16, half);

                    const uint16x8_t result16_low = qvsource_over_u16(srcalpha16_low, dst16_low, half, full);
                    const uint16x8_t result16_high = qvsource_over_u16(srcalpha16_high, dst16_high, half, full);

                    const uint32x2_t result32_low = vreinterpret_u32_u8(vmovn_u16(result16_low));
                    const uint32x2_t result32_high = vreinterpret_u32_u8(vmovn_u16(result16_high));

                    vst1q_u32((uint32_t *)&dst[x], vcombine_u32(result32_low, result32_high));
                }
            }
            for (; x<w; ++x) {
                uint s = src[x];
                if (s != 0) {
                    s = BYTE_MUL(s, const_alpha);
                    dst[x] = s + BYTE_MUL(dst[x], qAlpha(~s));
                }
            }
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

void qt_blend_rgb32_on_rgb32_neon(uchar *destPixels, int dbpl,
                                  const uchar *srcPixels, int sbpl,
                                  int w, int h,
                                  int const_alpha)
{
    if (const_alpha != 256) {
        if (const_alpha != 0) {
            const uint *src = (const uint *) srcPixels;
            uint *dst = (uint *) destPixels;
            uint16x8_t half = vdupq_n_u16(0x80);
            const_alpha = (const_alpha * 255) >> 8;
            int one_minus_const_alpha = 255 - const_alpha;
            uint16x8_t const_alpha16 = vdupq_n_u16(const_alpha);
            uint16x8_t one_minus_const_alpha16 = vdupq_n_u16(255 - const_alpha);
            for (int y = 0; y < h; ++y) {
                int x = 0;
                for (; x < w-3; x += 4) {
                    uint32x4_t src32 = vld1q_u32((uint32_t *)&src[x]);
                    uint32x4_t dst32 = vld1q_u32((uint32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_u32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_u32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const uint16x8_t src16_low = vmovl_u8(src8_low);
                    const uint16x8_t dst16_low = vmovl_u8(dst8_low);

                    const uint16x8_t src16_high = vmovl_u8(src8_high);
                    const uint16x8_t dst16_high = vmovl_u8(dst8_high);

                    const uint16x8_t result16_low = qvinterpolate_pixel_255(src16_low, const_alpha16, dst16_low, one_minus_const_alpha16, half);
                    const uint16x8_t result16_high = qvinterpolate_pixel_255(src16_high, const_alpha16, dst16_high, one_minus_const_alpha16, half);

                    const uint32x2_t result32_low = vreinterpret_u32_u8(vmovn_u16(result16_low));
                    const uint32x2_t result32_high = vreinterpret_u32_u8(vmovn_u16(result16_high));

                    vst1q_u32((uint32_t *)&dst[x], vcombine_u32(result32_low, result32_high));
                }
                for (; x<w; ++x) {
                    uint s = src[x];
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

QT_END_NAMESPACE

#endif // QT_HAVE_NEON

