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

static inline int16x8_t qvdiv_255_s16(int16x8_t x, int16x8_t half)
{
    // result = (x + (x >> 8) + 0x80) >> 8

    const int16x8_t temp = vshrq_n_s16(x, 8); // x >> 8
    const int16x8_t sum_part = vaddq_s16(x, half); // x + 0x80
    const int16x8_t sum = vaddq_s16(temp, sum_part);

    return vreinterpretq_s16_u16(vshrq_n_u16(vreinterpretq_u16_s16(sum), 8));
}

static inline int16x8_t qvbyte_mul_s16(int16x8_t x, int16x8_t alpha, int16x8_t half)
{
    // t = qRound(x * alpha / 255.0)

    const int16x8_t t = vmulq_s16(x, alpha); // t
    return qvdiv_255_s16(t, half);
}

static inline int16x8_t qvinterpolate_pixel_255(int16x8_t x, int16x8_t a, int16x8_t y, int16x8_t b, int16x8_t half)
{
    // t = x * a + y * b

    const int16x8_t ta = vmulq_s16(x, a);
    const int16x8_t tb = vmulq_s16(y, b);

    return qvdiv_255_s16(vaddq_s16(ta, tb), half);
}

static inline int16x8_t qvsource_over_s16(int16x8_t src16, int16x8_t dst16, int16x8_t half, int16x8_t full)
{
    const int16x4_t alpha16_high = vdup_lane_s16(vget_high_s16(src16), 3);
    const int16x4_t alpha16_low = vdup_lane_s16(vget_low_s16(src16), 3);

    const int16x8_t alpha16 = vsubq_s16(full, vcombine_s16(alpha16_low, alpha16_high));

    return vaddq_s16(src16, qvbyte_mul_s16(dst16, alpha16, half));
}

void qt_blend_argb32_on_argb32_neon(uchar *destPixels, int dbpl,
                                    const uchar *srcPixels, int sbpl,
                                    int w, int h,
                                    int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;
    int16x8_t half = vdupq_n_s16(0x80);
    int16x8_t full = vdupq_n_s16(0xff);
    if (const_alpha == 256) {
        for (int y = 0; y < h; ++y) {
            int x = 0;
            for (; x < w-3; x += 4) {
                int32x4_t src32 = vld1q_s32((int32_t *)&src[x]);
                if ((src[x] & src[x+1] & src[x+2] & src[x+3]) >= 0xff000000) {
                    // all opaque
                    vst1q_s32((int32_t *)&dst[x], src32);
                } else if (src[x] | src[x+1] | src[x+2] | src[x+3]) {
                    int32x4_t dst32 = vld1q_s32((int32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_s32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_s32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const int16x8_t src16_low = vreinterpretq_s16_u16(vmovl_u8(src8_low));
                    const int16x8_t dst16_low = vreinterpretq_s16_u16(vmovl_u8(dst8_low));

                    const int16x8_t src16_high = vreinterpretq_s16_u16(vmovl_u8(src8_high));
                    const int16x8_t dst16_high = vreinterpretq_s16_u16(vmovl_u8(dst8_high));

                    const int16x8_t result16_low = qvsource_over_s16(src16_low, dst16_low, half, full);
                    const int16x8_t result16_high = qvsource_over_s16(src16_high, dst16_high, half, full);

                    const int32x2_t result32_low = vreinterpret_s32_s8(vmovn_s16(result16_low));
                    const int32x2_t result32_high = vreinterpret_s32_s8(vmovn_s16(result16_high));

                    vst1q_s32((int32_t *)&dst[x], vcombine_s32(result32_low, result32_high));
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
        int16x8_t const_alpha16 = vdupq_n_s16(const_alpha);
        for (int y = 0; y < h; ++y) {
            int x = 0;
            for (; x < w-3; x += 4) {
                if (src[x] | src[x+1] | src[x+2] | src[x+3]) {
                    int32x4_t src32 = vld1q_s32((int32_t *)&src[x]);
                    int32x4_t dst32 = vld1q_s32((int32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_s32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_s32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const int16x8_t src16_low = vreinterpretq_s16_u16(vmovl_u8(src8_low));
                    const int16x8_t dst16_low = vreinterpretq_s16_u16(vmovl_u8(dst8_low));

                    const int16x8_t src16_high = vreinterpretq_s16_u16(vmovl_u8(src8_high));
                    const int16x8_t dst16_high = vreinterpretq_s16_u16(vmovl_u8(dst8_high));

                    const int16x8_t srcalpha16_low = qvbyte_mul_s16(src16_low, const_alpha16, half);
                    const int16x8_t srcalpha16_high = qvbyte_mul_s16(src16_high, const_alpha16, half);

                    const int16x8_t result16_low = qvsource_over_s16(srcalpha16_low, dst16_low, half, full);
                    const int16x8_t result16_high = qvsource_over_s16(srcalpha16_high, dst16_high, half, full);

                    const int32x2_t result32_low = vreinterpret_s32_s8(vmovn_s16(result16_low));
                    const int32x2_t result32_high = vreinterpret_s32_s8(vmovn_s16(result16_high));

                    vst1q_s32((int32_t *)&dst[x], vcombine_s32(result32_low, result32_high));
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
            int16x8_t half = vdupq_n_s16(0x80);
            const_alpha = (const_alpha * 255) >> 8;
            int one_minus_const_alpha = 255 - const_alpha;
            int16x8_t const_alpha16 = vdupq_n_s16(const_alpha);
            int16x8_t one_minus_const_alpha16 = vdupq_n_s16(255 - const_alpha);
            for (int y = 0; y < h; ++y) {
                int x = 0;
                for (; x < w-3; x += 4) {
                    int32x4_t src32 = vld1q_s32((int32_t *)&src[x]);
                    int32x4_t dst32 = vld1q_s32((int32_t *)&dst[x]);

                    const uint8x16_t src8 = vreinterpretq_u8_s32(src32);
                    const uint8x16_t dst8 = vreinterpretq_u8_s32(dst32);

                    const uint8x8_t src8_low = vget_low_u8(src8);
                    const uint8x8_t dst8_low = vget_low_u8(dst8);

                    const uint8x8_t src8_high = vget_high_u8(src8);
                    const uint8x8_t dst8_high = vget_high_u8(dst8);

                    const int16x8_t src16_low = vreinterpretq_s16_u16(vmovl_u8(src8_low));
                    const int16x8_t dst16_low = vreinterpretq_s16_u16(vmovl_u8(dst8_low));

                    const int16x8_t src16_high = vreinterpretq_s16_u16(vmovl_u8(src8_high));
                    const int16x8_t dst16_high = vreinterpretq_s16_u16(vmovl_u8(dst8_high));

                    const int16x8_t result16_low = qvinterpolate_pixel_255(src16_low, const_alpha16, dst16_low, one_minus_const_alpha16, half);
                    const int16x8_t result16_high = qvinterpolate_pixel_255(src16_high, const_alpha16, dst16_high, one_minus_const_alpha16, half);

                    const int32x2_t result32_low = vreinterpret_s32_s8(vmovn_s16(result16_low));
                    const int32x2_t result32_high = vreinterpret_s32_s8(vmovn_s16(result16_high));

                    vst1q_s32((int32_t *)&dst[x], vcombine_s32(result32_low, result32_high));
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

