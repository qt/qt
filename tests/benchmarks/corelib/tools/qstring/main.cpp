/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QStringList>
#include <QFile>
#include <QtTest/QtTest>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
// Application private dir is default serach path for files, so SRCDIR can be set to empty
#define SRCDIR ""
#endif

#ifdef Q_OS_UNIX
#include <sys/mman.h>
#include <unistd.h>
#endif

#include <private/qsimd_p.h>

#include "data.h"

class tst_QString: public QObject
{
    Q_OBJECT
public:
    tst_QString();
private slots:
    void equals() const;
    void equals_data() const;
    void equals2_data() const;
    void equals2() const;
    void ucstrncmp_data() const;
    void ucstrncmp() const;
    void fromUtf8() const;
};

void tst_QString::equals() const
{
    QFETCH(QString, a);
    QFETCH(QString, b);

    QBENCHMARK {
        a == b;
    }
}

tst_QString::tst_QString()
{
}

void tst_QString::equals_data() const
{
    static const struct {
        ushort data[80];
        int dummy;              // just to ensure 4-byte alignment
    } data = {
        {
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 16
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 32
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 48
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 64
            64, 64, 64, 64,  96, 96, 96, 96,
            64, 64, 96, 96,  96, 96, 96, 96  // 80
        }, 0
    };
    const QChar *ptr = reinterpret_cast<const QChar *>(data.data);

    QTest::addColumn<QString>("a");
    QTest::addColumn<QString>("b");
    QString base = QString::fromRawData(ptr, 64);

    QTest::newRow("different-length") << base << QString::fromRawData(ptr, 4);
    QTest::newRow("same-string") << base << base;
    QTest::newRow("same-data") << base << QString::fromRawData(ptr, 64);

    // try to avoid crossing a cache line (that is, at ptr[64])
    QTest::newRow("aligned-aligned-4n")
            << QString::fromRawData(ptr, 60) << QString::fromRawData(ptr + 2, 60);
    QTest::newRow("aligned-unaligned-4n")
            << QString::fromRawData(ptr, 60) << QString::fromRawData(ptr + 1, 60);
    QTest::newRow("unaligned-unaligned-4n")
            << QString::fromRawData(ptr + 1, 60) << QString::fromRawData(ptr + 3, 60);

    QTest::newRow("aligned-aligned-4n+1")
            << QString::fromRawData(ptr, 61) << QString::fromRawData(ptr + 2, 61);
    QTest::newRow("aligned-unaligned-4n+1")
            << QString::fromRawData(ptr, 61) << QString::fromRawData(ptr + 1, 61);
    QTest::newRow("unaligned-unaligned-4n+1")
            << QString::fromRawData(ptr + 1, 61) << QString::fromRawData(ptr + 3, 61);

    QTest::newRow("aligned-aligned-4n-1")
            << QString::fromRawData(ptr, 59) << QString::fromRawData(ptr + 2, 59);
    QTest::newRow("aligned-unaligned-4n-1")
            << QString::fromRawData(ptr, 59) << QString::fromRawData(ptr + 1, 59);
    QTest::newRow("unaligned-unaligned-4n-1")
            << QString::fromRawData(ptr + 1, 59) << QString::fromRawData(ptr + 3, 59);

    QTest::newRow("aligned-aligned-2n")
            << QString::fromRawData(ptr, 58) << QString::fromRawData(ptr + 2, 58);
    QTest::newRow("aligned-unaligned-2n")
            << QString::fromRawData(ptr, 58) << QString::fromRawData(ptr + 1, 58);
    QTest::newRow("unaligned-unaligned-2n")
            << QString::fromRawData(ptr + 1, 58) << QString::fromRawData(ptr + 3, 58);
}

static bool equals2_memcmp_call(const ushort *p1, const ushort *p2, int len)
{
    return memcmp(p1, p2, len * 2) == 0;
}

static bool equals2_bytewise(const ushort *p1, const ushort *p2, int len)
{
    if (p1 == p2 || !len)
        return true;
    uchar *b1 = (uchar *)p1;
    uchar *b2 = (uchar *)p2;
    len *= 2;
    while (len--)
        if (*b1++ != *b2++)
            return false;
    return true;
}

static bool equals2_shortwise(const ushort *p1, const ushort *p2, int len)
{
    if (p1 == p2 || !len)
        return true;
//    for (register int counter; counter < len; ++counter)
//        if (p1[counter] != p2[counter])
//            return false;
    while (len--) {
        if (p1[len] != p2[len])
            return false;
    }
    return true;
}

static bool equals2_intwise(const ushort *p1, const ushort *p2, int length)
{
    if (p1 == p2 || !length)
        return true;
    register union {
        const quint16 *w;
        const quint32 *d;
        quintptr value;
    } sa, sb;
    sa.w = p1;
    sb.w = p2;

    // check alignment
    if ((sa.value & 2) == (sb.value & 2)) {
        // both addresses have the same alignment
        if (sa.value & 2) {
            // both addresses are not aligned to 4-bytes boundaries
            // compare the first character
            if (*sa.w != *sb.w)
                return false;
            --length;
            ++sa.w;
            ++sb.w;

            // now both addresses are 4-bytes aligned
        }

        // both addresses are 4-bytes aligned
        // do a fast 32-bit comparison
        register const quint32 *e = sa.d + (length >> 1);
        for ( ; sa.d != e; ++sa.d, ++sb.d) {
            if (*sa.d != *sb.d)
                return false;
        }

        // do we have a tail?
        return (length & 1) ? *sa.w == *sb.w : true;
    } else {
        // one of the addresses isn't 4-byte aligned but the other is
        register const quint16 *e = sa.w + length;
        for ( ; sa.w != e; ++sa.w, ++sb.w) {
            if (*sa.w != *sb.w)
                return false;
        }
    }
    return true;
}

static inline bool equals2_short_tail(const ushort *p1, const ushort *p2, int len)
{
    if (len) {
        if (*p1 != *p2)
            return false;
        if (--len) {
            if (p1[1] != p2[1])
                return false;
            if (--len) {
                if (p1[2] != p2[2])
                    return false;
                if (--len) {
                    if (p1[3] != p2[3])
                        return false;
                    if (--len) {
                        if (p1[4] != p2[4])
                            return false;
                        if (--len) {
                            if (p1[5] != p2[5])
                                return false;
                            if (--len) {
                                if (p1[6] != p2[6])
                                    return false;
                                return p1[7] == p2[7];
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

//#pragma GCC optimize("no-unroll-loops")
#ifdef __SSE2__
static bool equals2_sse2_aligned(const ushort *p1, const ushort *p2, int len)
{
    if (len >= 8) {
        qptrdiff counter = 0;
        while (len > 8) {
            __m128i q1 = _mm_load_si128((__m128i *)(p1 + counter));
            __m128i q2 = _mm_load_si128((__m128i *)(p2 + counter));
            __m128i cmp = _mm_cmpeq_epi16(q1, q2);
            if (ushort(_mm_movemask_epi8(cmp)) != ushort(0xffff))
                return false;

            len -= 8;
            counter += 8;
        }
        p1 += counter;
        p2 += counter;
    }

    return equals2_short_tail(p1, p2, len);
}

static bool __attribute__((optimize("no-unroll-loops"))) equals2_sse2(const ushort *p1, const ushort *p2, int len)
{
    if (p1 == p2 || !len)
        return true;

    if (len >= 8) {
        qptrdiff counter = 0;
        while (len >= 8) {
            __m128i q1 = _mm_loadu_si128((__m128i *)(p1 + counter));
            __m128i q2 = _mm_loadu_si128((__m128i *)(p2 + counter));
            __m128i cmp = _mm_cmpeq_epi16(q1, q2);
            if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
                return false;

            len -= 8;
            counter += 8;
        }
        p1 += counter;
        p2 += counter;
    }

    return equals2_short_tail(p1, p2, len);
}

//static bool equals2_sse2(const ushort *p1, const ushort *p2, int len)
//{
//    register int val1 = quintptr(p1) & 0xf;
//    register int val2 = quintptr(p2) & 0xf;
//    if (false && val1 + val2 == 0)
//        return equals2_sse2_aligned(p1, p2, len);
//    else
//        return equals2_sse2_unaligned(p1, p2, len);
//}

static bool equals2_sse2_aligning(const ushort *p1, const ushort *p2, int len)
{
    if (len < 8)
        return equals2_short_tail(p1, p2, len);

    qptrdiff counter = 0;

    // which one is easier to align, p1 or p2 ?
    register int val1 = quintptr(p1) & 0xf;
    register int val2 = quintptr(p2) & 0xf;
    if (val1 && val2) {
#if 0
        // we'll align the one which requires the least number of steps
        if (val1 > val2) {
            qSwap(p1, p2);
            val1 = val2;
        }

        // val1 contains the number of bytes past the 16-aligned mark
        // we must read 16-val1 bytes to align
        val1 = 16 - val1;
        if (val1 & 0x2) {
            if (*p1 != *p2)
                return false;
            --len;
            ++counter;
        }
        while (val1 & 12) {
            if (*(uint*)p1 != *(uint*)p2)
                return false;
            --len;
            counter += 2;
            val1 -= 4;
        }
#else
        // we'll align the one closest to the 16-byte mark
        if (val1 > val2) {
            qSwap(p1, p2);
            val1 = val2;
        }

        // we're reading val1 bytes too many
        __m128i q2 = _mm_loadu_si128((__m128i *)(p2 - val1/2));
        __m128i cmp = _mm_cmpeq_epi16(*(__m128i *)(p1 - val1/2), q2);
        if (short(_mm_movemask_epi8(cmp)) >> val1 != short(-1))
            return false;

        counter = 8 - val1/2;
        len -= 8 - val1/2;
#endif
    } else if (!val2) {
        // p2 is already aligned
        qSwap(p1, p2);
    }

    // p1 is aligned

    while (len >= 8) {
        __m128i q1 = _mm_load_si128((__m128i *)(p1 + counter));
        __m128i q2 = _mm_loadu_si128((__m128i *)(p2 + counter));
        __m128i cmp = _mm_cmpeq_epi16(q1, q2);
        if (ushort(_mm_movemask_epi8(cmp)) != ushort(0xffff))
            return false;

        len -= 8;
        counter += 8;
    }

    // tail
    return equals2_short_tail(p1 + counter, p2 + counter, len);
}

#ifdef __SSE3__
static bool __attribute__((optimize("no-unroll-loops"))) equals2_sse3(const ushort *p1, const ushort *p2, int len)
{
    if (p1 == p2 || !len)
        return true;

    if (len >= 8) {
        qptrdiff counter = 0;
        while (len >= 8) {
            __m128i q1 = _mm_lddqu_si128((__m128i *)(p1 + counter));
            __m128i q2 = _mm_lddqu_si128((__m128i *)(p2 + counter));
            __m128i cmp = _mm_cmpeq_epi16(q1, q2);
            if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
                return false;

            len -= 8;
            counter += 8;
        }
        p1 += counter;
        p2 += counter;
    }

    return equals2_short_tail(p1, p2, len);
}

#ifdef __SSSE3__
template<int N> static __attribute__((optimize("unroll-loops"))) inline bool equals2_ssse3_alignr(__m128i *m1, __m128i *m2, int len)
{
    __m128i lower = _mm_load_si128(m1);
    while (len >= 8) {
        __m128i upper = _mm_load_si128(m1 + 1);
        __m128i correct;
        correct = _mm_alignr_epi8(upper, lower, N);

        __m128i q2 = _mm_lddqu_si128(m2);
        __m128i cmp = _mm_cmpeq_epi16(correct, q2);
        if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
            return false;

        len -= 8;
        ++m2;
        ++m1;
        lower = upper;
    }

    // tail
    return len == 0 || equals2_short_tail((const ushort *)m1 + N / 2, (const ushort*)m2, len);
}

static inline __attribute__((optimize("unroll-loops"))) bool equals2_ssse3_aligned(__m128i *m1, __m128i *m2, int len)
{
    while (len >= 8) {
        __m128i q2 = _mm_lddqu_si128(m2);
        __m128i cmp = _mm_cmpeq_epi16(*m1, q2);
        if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
            return false;

        len -= 8;
        ++m1;
        ++m2;
    }
    return len == 0 || equals2_short_tail((const ushort *)m1, (const ushort *)m2, len);
}

static bool equals2_ssse3(const ushort *p1, const ushort *p2, int len)
{
    // p1 & 0xf can be:
    //   0,  2,  4,  6,  8, 10, 12, 14
    // If it's 0, we're aligned
    // If it's not, then we're interested in the 16 - (p1 & 0xf) bytes only

    if (len >= 8) {
        // find the last aligned position below the p1 memory
        __m128i *m1 = (__m128i *)(quintptr(p1) & ~0xf);
        __m128i *m2 = (__m128i *)p2;
        qptrdiff diff = quintptr(p1) - quintptr(m1);

        // diff contains the number of extra bytes
        if (diff == 10)
            return equals2_ssse3_alignr<10>(m1, m2, len);
        else if (diff == 2)
            return equals2_ssse3_alignr<2>(m1, m2, len);
        if (diff < 8) {
            if (diff < 4) {
                return equals2_ssse3_aligned(m1, m2, len);
            } else {
                if (diff == 4)
                    return equals2_ssse3_alignr<4>(m1, m2, len);
                else // diff == 6
                    return equals2_ssse3_alignr<6>(m1, m2, len);
            }
        } else {
            if (diff < 12) {
                return equals2_ssse3_alignr<8>(m1, m2, len);
            } else {
                if (diff == 12)
                    return equals2_ssse3_alignr<12>(m1, m2, len);
                else // diff == 14
                    return equals2_ssse3_alignr<14>(m1, m2, len);
            }
        }
    }

    // tail
    return equals2_short_tail(p1, p2, len);
}

template<int N> static inline bool equals2_ssse3_aligning_alignr(__m128i *m1, __m128i *m2, int len)
{
    __m128i lower = _mm_load_si128(m1);
    while (len >= 8) {
        __m128i upper = _mm_load_si128(m1 + 1);
        __m128i correct;
        correct = _mm_alignr_epi8(upper, lower, N);

        __m128i cmp = _mm_cmpeq_epi16(correct, *m2);
        if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
            return false;

        len -= 8;
        ++m2;
        ++m1;
        lower = upper;
    }

    // tail
    return len == 0 || equals2_short_tail((const ushort *)m1 + N / 2, (const ushort*)m2, len);
}

static bool equals2_ssse3_aligning(const ushort *p1, const ushort *p2, int len)
{
    if (len < 8)
        return equals2_short_tail(p1, p2, len);
    qptrdiff counter = 0;

    // which one is easier to align, p1 or p2 ?
    {
        register int val1 = quintptr(p1) & 0xf;
        register int val2 = quintptr(p2) & 0xf;
        if (val1 && val2) {
            // we'll align the one closest to the 16-byte mark
            if (val1 < val2) {
                qSwap(p1, p2);
                val2 = val1;
            }

            // we're reading val1 bytes too many
            __m128i q1 = _mm_lddqu_si128((__m128i *)(p1 - val2/2));
            __m128i cmp = _mm_cmpeq_epi16(q1, *(__m128i *)(p2 - val2/2));
            if (short(_mm_movemask_epi8(cmp)) >> val1 != short(-1))
                return false;

            counter = 8 - val2/2;
            len -= 8 - val2/2;
        } else if (!val1) {
            // p1 is already aligned
            qSwap(p1, p2);
        }
    }

    // p2 is aligned now
    // we want to use palignr in the mis-alignment of p1
    __m128i *m1 = (__m128i *)(quintptr(p1 + counter) & ~0xf);
    __m128i *m2 = (__m128i *)(p2 + counter);
    register int val1 = quintptr(p1 + counter) - quintptr(m1);

    // val1 contains the number of extra bytes
    if (val1 == 8)
        return equals2_ssse3_aligning_alignr<8>(m1, m2, len);
    if (val1 == 0)
        return equals2_sse2_aligned(p1 + counter, p2 + counter, len);
    if (val1 < 8) {
        if (val1 < 4) {
            return equals2_ssse3_aligning_alignr<2>(m1, m2, len);
        } else {
            if (val1 == 4)
                return equals2_ssse3_aligning_alignr<4>(m1, m2, len);
            else // diff == 6
                return equals2_ssse3_aligning_alignr<6>(m1, m2, len);
        }
    } else {
        if (val1 < 12) {
            return equals2_ssse3_aligning_alignr<10>(m1, m2, len);
        } else {
            if (val1 == 12)
                return equals2_ssse3_aligning_alignr<12>(m1, m2, len);
            else // diff == 14
                return equals2_ssse3_aligning_alignr<14>(m1, m2, len);
        }
    }
}

#ifdef __SSE4_1__
static bool equals2_sse4(const ushort *p1, const ushort *p2, int len)
{
    // We use the pcmpestrm instruction searching for differences (negative polarity)
    // it will reset CF if it's all equal
    // it will reset OF if the first char is equal
    // it will set ZF & SF if the length is less than 8 (which means we've done the last operation)
    // the three possible conditions are:
    //  difference found:         CF = 1
    //  all equal, not finished:  CF = ZF = SF = 0
    //  all equal, finished:      CF = 0, ZF = SF = 1
    // We use the JA instruction that jumps if ZF = 0 and CF = 0
    if (p1 == p2 || !len)
        return true;

    // This function may read some bytes past the end of p1 or p2
    // It is safe to do that, as long as those extra bytes (beyond p1+len and p2+len)
    // are on the same page as the last valid byte.
    // If len is a multiple of 8, we'll never load invalid bytes.
    if (len & 7) {
        // The last load would load (len & ~7) valid bytes and (8 - (len & ~7)) invalid bytes.
        // So we can't do the last load if any of those bytes is in a different
        // page. That is, if:
        //    pX + len      is on a different page from     pX + (len & ~7) + 8
        //
        // that is, if second-to-last load ended up less than 16 bytes from the page end:
        //    pX + (len & ~7)  is the last ushort read in the second-to-last load
        if (len < 8)
            return equals2_short_tail(p1, p2, len);
        if ((quintptr(p1 + (len & ~7)) & 0xfff) > 0xff0 ||
                (quintptr(p2 + (len & ~7)) & 0xfff) > 0xff0) {

            // yes, so we mustn't do the final 128-bit load
            bool result;
            asm (
            "sub        %[p1], %[p2]\n\t"
            "sub        $16, %[p1]\n\t"
            "add        $8, %[len]\n\t"

            // main loop:
            "0:\n\t"
            "add        $16, %[p1]\n\t"
            "sub        $8, %[len]\n\t"
            "jz         1f\n\t"
            "lddqu      (%[p1]), %%xmm0\n\t"
            "mov        %[len], %%edx\n\t"
            "pcmpestri  %[mode], (%[p2],%[p1]), %%xmm0\n\t"

            "jna        1f\n\t"
            "add        $16, %[p1]\n\t"
            "sub        $8, %[len]\n\t"
            "jz         1f\n\t"
            "lddqu      (%[p1]), %%xmm0\n\t"
            "mov        %[len], %%edx\n\t"
            "pcmpestri  %[mode], (%[p2],%[p1]), %%xmm0\n\t"

            "ja         0b\n\t"
            "1:\n\t"
            "setnc      %[result]\n\t"
            : [result] "=a" (result),
              [p1] "+r" (p1),
              [p2] "+r" (p2)
            : [len] "0" (len & ~7),
              [mode] "i" (_SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY)
            : "%edx", "%ecx", "%xmm0"
            );
            return result && equals2_short_tail(p1, (const ushort *)(quintptr(p1) + quintptr(p2)), len & 7);
        }
    }

//    const qptrdiff disp = p2 - p1;
//    p1 -= 8;
//    len += 8;
//    while (true) {
//        enum { Mode = _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY };

//        p1 += 8;
//        len -= 8;
//        if (!len)
//            return true;

//        __m128i q1 = _mm_lddqu_si128((__m128i *)(p1 + disp));
//        __m128i *m2 = (__m128i *)p1;

//        bool cmp_a = _mm_cmpestra(q1, len, *m2, len, Mode);
//        if (cmp_a)
//            continue;
//        return !_mm_cmpestrc(q1, len, *m2, len, Mode);
//    }
//    return true;
    bool result;
    asm (
        "sub        %[p1], %[p2]\n\t"
        "sub        $16, %[p1]\n\t"
        "add        $8, %[len]\n\t"

    "0:\n\t"
        "add        $16, %[p1]\n\t"
        "sub        $8, %[len]\n\t"
        "jz         1f\n\t"
        "lddqu      (%[p2],%[p1]), %%xmm0\n\t"
        "mov        %[len], %%edx\n\t"
        "pcmpestri  %[mode], (%[p1]), %%xmm0\n\t"

        "jna        1f\n\t"
        "add        $16, %[p1]\n\t"
        "sub        $8, %[len]\n\t"
        "jz         1f\n\t"
        "lddqu      (%[p2],%[p1]), %%xmm0\n\t"
        "mov        %[len], %%edx\n\t"
        "pcmpestri  %[mode], (%[p1]), %%xmm0\n\t"

        "ja         0b\n\t"

    "1:\n\t"
        "setnc      %[result]\n\t"
        : [result] "=a" (result)
        : [len] "0" (len),
          [p1] "r" (p1),
          [p2] "r" (p2),
          [mode] "i" (_SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY)
        : "%edx", "%ecx", "%xmm0"
    );
    return result;
}

#endif
#endif
#endif
#endif

typedef bool (* FuncPtr)(const ushort *, const ushort *, int);
static const FuncPtr func[] = {
    equals2_memcmp_call, // 0
    equals2_bytewise, // 1
    equals2_shortwise, // 1
    equals2_intwise, // 3
#ifdef __SSE2__
    equals2_sse2, // 4
    equals2_sse2_aligning, // 5
#ifdef __SSE3__
    equals2_sse3, // 6
#ifdef __SSSE3__
    equals2_ssse3, // 7
    equals2_ssse3, // 8
#ifdef __SSE4_1__
    equals2_sse4, // 9
#endif
#endif
#endif
#endif
    0
};
static const int functionCount = sizeof(func)/sizeof(func[0]) - 1;

void tst_QString::equals2_data() const
{
    QTest::addColumn<int>("algorithm");
    QTest::newRow("selftest") << -1;
    QTest::newRow("memcmp_call") << 0;
    QTest::newRow("bytewise") << 1;
    QTest::newRow("shortwise") << 2;
    QTest::newRow("intwise") << 3;
#ifdef __SSE2__
    QTest::newRow("sse2") << 4;
    QTest::newRow("sse2_aligning") << 5;
#ifdef __SSE3__
    QTest::newRow("sse3") << 6;
#ifdef __SSSE3__
    QTest::newRow("ssse3") << 7;
    QTest::newRow("ssse3_aligning") << 8;
#ifdef __SSE4_1__
    QTest::newRow("sse4.2") << 9;
#endif
#endif
#endif
#endif
}

static void __attribute__((noinline)) equals2_selftest()
{
#ifdef Q_OS_UNIX
    const long pagesize = sysconf(_SC_PAGESIZE);
    void *page1, *page3;
    ushort *page2;
    page1 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    page2 = (ushort *)mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
    page3 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    Q_ASSERT(quintptr(page2) == quintptr(page1) + pagesize || quintptr(page2) == quintptr(page1) - pagesize);
    Q_ASSERT(quintptr(page3) == quintptr(page2) + pagesize || quintptr(page3) == quintptr(page2) - pagesize);
    munmap(page1, pagesize);
    munmap(page3, pagesize);

    // populate our page
    for (uint i = 0; i < pagesize / sizeof(long long); ++i)
        ((long long *)page2)[i] = Q_INT64_C(0x0041004100410041);

    // the following should crash:
    //page2[-1] = 0xdead;
    //page2[pagesize / sizeof(ushort) + 1] = 0xbeef;

    static const ushort needle[] = {
        0x41, 0x41, 0x41, 0x41,   0x41, 0x41, 0x41, 0x41,
        0x41, 0x41, 0x41, 0x41,   0x41, 0x41, 0x41, 0x41,
        0x41
    };

    for (int algo = 0; algo < functionCount; ++algo) {
        // boundary condition test:
        for (int i = 0; i < 8; ++i) {
            (func[algo])(page2 + i, needle, sizeof needle / 2);
            (func[algo])(page2 - i - 1 - sizeof(needle)/2 + pagesize/2, needle, sizeof needle/2);
        }
    }

    munmap(page2, pagesize);
#endif

    for (int algo = 0; algo < functionCount; ++algo) {
        for (int i = 0; i < stringCollectionCount; ++i) {
            const ushort *p1 = stringCollectionData + stringCollection[i].offset1;
            const ushort *p2 = stringCollectionData + stringCollection[i].offset2;
            bool expected = memcmp(p1, p2, stringCollection[i].len * 2) == 0;

            bool result = (func[algo])(p1, p2, stringCollection[i].len);
            if (expected != result)
                qWarning().nospace()
                        << "algo=" << algo
                        << " i=" << i
                        << " failed (" << result << "!=" << expected
                        << "); strings were "
                        << QByteArray((char*)p1, stringCollection[i].len).toHex()
                        << " and "
                        << QByteArray((char*)p2, stringCollection[i].len).toHex();
        }
    }
}

void tst_QString::equals2() const
{
    QFETCH(int, algorithm);
    if (algorithm == -1) {
        equals2_selftest();
        return;
    }

    QBENCHMARK {
        for (int i = 0; i < stringCollectionCount; ++i) {
            const ushort *p1 = stringCollectionData + stringCollection[i].offset1;
            const ushort *p2 = stringCollectionData + stringCollection[i].offset2;
            bool result = (func[algorithm])(p1, p2, stringCollection[i].len);
            Q_UNUSED(result);
        }
    }
}

static int ucstrncmp_shortwise(const ushort *a, const ushort *b, int l)
{
    while (l-- && *a == *b)
        a++,b++;
    if (l==-1)
        return 0;
    return *a - *b;
}

static int ucstrncmp_intwise(const ushort *a, const ushort *b, int len)
{
    // do both strings have the same alignment?
    if ((quintptr(a) & 2) == (quintptr(b) & 2)) {
        // are we aligned to 4 bytes?
        if (quintptr(a) & 2) {
            if (*a != *b)
                return *a - *b;
            ++a;
            ++b;
            --len;
        }

        const uint *p1 = (const uint *)a;
        const uint *p2 = (const uint *)b;
        quintptr counter = 0;
        for ( ; len > 1 ; len -= 2, ++counter) {
            if (p1[counter] != p2[counter]) {
                // which ushort isn't equal?
                int diff = a[2*counter] - b[2*counter];
                return diff ? diff : a[2*counter + 1] - b[2*counter + 1];
            }
        }

        return len ? a[2*counter] - b[2*counter] : 0;
    } else {
        while (len-- && *a == *b)
            a++,b++;
        if (len==-1)
            return 0;
        return *a - *b;
    }
}

#ifdef __SSE2__
static inline int ucstrncmp_short_tail(const ushort *p1, const ushort *p2, int len)
{
    if (len) {
        if (*p1 != *p2)
            return *p1 - *p2;
        if (--len) {
            if (p1[1] != p2[1])
                return p1[1] - p2[1];
            if (--len) {
                if (p1[2] != p2[2])
                    return p1[2] - p2[2];
                if (--len) {
                    if (p1[3] != p2[3])
                        return p1[3] - p2[3];
                    if (--len) {
                        if (p1[4] != p2[4])
                            return p1[4] - p2[4];
                        if (--len) {
                            if (p1[5] != p2[5])
                                return p1[5] - p2[5];
                            if (--len) {
                                if (p1[6] != p2[6])
                                    return p1[6] - p2[6];
                                return p1[7] - p2[7];
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

static inline int bsf_nonzero(register long val)
{
    int result;
# ifdef Q_CC_GNU
    // returns the first non-zero bit on a non-zero reg
    asm ("bsf   %1, %0" : "=r" (result) : "r" (val));
    return result;
# elif defined(Q_CC_MSVC)
    _BitScanForward(&result, val);
    return result;
# endif
}

static __attribute__((optimize("no-unroll-loops"))) int ucstrncmp_sse2(const ushort *a, const ushort *b, int len)
{
    qptrdiff counter = 0;
    while (len >= 8) {
        __m128i m1 = _mm_loadu_si128((__m128i *)(a + counter));
        __m128i m2 = _mm_loadu_si128((__m128i *)(b + counter));
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }

        counter += 8;
        len -= 8;
    }
    return ucstrncmp_short_tail(a + counter, b + counter, len);
}

static __attribute__((optimize("no-unroll-loops"))) int ucstrncmp_sse2_aligning(const ushort *a, const ushort *b, int len)
{
    if (len >= 8) {
        __m128i m1 = _mm_loadu_si128((__m128i *)a);
        __m128i m2 = _mm_loadu_si128((__m128i *)b);
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            int counter = bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }


        // now align to do 16-byte loads
        int diff = 8 - (quintptr(a) & 0xf)/2;
        len -= diff;
        a += diff;
        b += diff;
    }

    qptrdiff counter = 0;
    while (len >= 8) {
        __m128i m1 = _mm_load_si128((__m128i *)(a + counter));
        __m128i m2 = _mm_loadu_si128((__m128i *)(b + counter));
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }

        counter += 8;
        len -= 8;
    }
    return ucstrncmp_short_tail(a + counter, b + counter, len);
}

static inline __attribute__((optimize("no-unroll-loops"))) int ucstrncmp_sse2_aligned(const ushort *a, const ushort *b, int len)
{
    quintptr counter = 0;
    while (len >= 8) {
        __m128i m1 = _mm_load_si128((__m128i *)(a + counter));
        __m128i m2 = _mm_load_si128((__m128i *)(b + counter));
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }

        counter += 8;
        len -= 8;
    }
    return ucstrncmp_short_tail(a + counter, b + counter, len);
}

static inline __attribute__((optimize("no-unroll-loops"))) int ucstrncmp_ssse3_alignr_aligned(const ushort *a, const ushort *b, int len)
{
    quintptr counter = 0;
    while (len >= 8) {
        __m128i m1 = _mm_load_si128((__m128i *)(a + counter));
        __m128i m2 = _mm_lddqu_si128((__m128i *)(b + counter));
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }

        counter += 8;
        len -= 8;
    }
    return ucstrncmp_short_tail(a + counter, b + counter, len);
}


typedef __m128i (* MMLoadFunction)(const __m128i *);
template<int N, MMLoadFunction LoadFunction>
static inline __attribute__((optimize("no-unroll-loops"))) int ucstrncmp_ssse3_alignr(const ushort *a, const ushort *b, int len)
{
    qptrdiff counter = 0;
    __m128i lower, upper;
    upper = _mm_load_si128((__m128i *)a);

    do {
        lower = upper;
        upper = _mm_load_si128((__m128i *)(a + counter) + 1);
        __m128i merged = _mm_alignr_epi8(upper, lower, N);

        __m128i m2 = LoadFunction((__m128i *)(b + counter));
        __m128i cmp = _mm_cmpeq_epi16(merged, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter + N/2] - b[counter];
        }

        counter += 8;
        len -= 8;
    } while (len >= 8);

    return ucstrncmp_short_tail(a + counter + N/2, b + counter, len);
}

static int ucstrncmp_ssse3(const ushort *a, const ushort *b, int len)
{
    if (len >= 8) {
        int val = quintptr(a) & 0xf;
        a -= val/2;

        if (val == 10)
            return ucstrncmp_ssse3_alignr<10, _mm_lddqu_si128>(a, b, len);
        else if (val == 2)
            return ucstrncmp_ssse3_alignr<2, _mm_lddqu_si128>(a, b, len);
        if (val < 8) {
            if (val < 4)
                return ucstrncmp_ssse3_alignr_aligned(a, b, len);
            else if (val == 4)
                    return ucstrncmp_ssse3_alignr<4, _mm_lddqu_si128>(a, b, len);
            else
                    return ucstrncmp_ssse3_alignr<6, _mm_lddqu_si128>(a, b, len);
        } else {
            if (val < 12)
                return ucstrncmp_ssse3_alignr<8, _mm_lddqu_si128>(a, b, len);
            else if (val == 12)
                return ucstrncmp_ssse3_alignr<12, _mm_lddqu_si128>(a, b, len);
            else
                return ucstrncmp_ssse3_alignr<14, _mm_lddqu_si128>(a, b, len);
        }
    }
    return ucstrncmp_short_tail(a, b, len);
}

static int ucstrncmp_ssse3_aligning(const ushort *a, const ushort *b, int len)
{
    if (len >= 8) {
        __m128i m1 = _mm_loadu_si128((__m128i *)a);
        __m128i m2 = _mm_loadu_si128((__m128i *)b);
        __m128i cmp = _mm_cmpeq_epi16(m1, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            int counter = bsf_nonzero(mask)/2;
            return a[counter] - b[counter];
        }


        // now 'b' align to do 16-byte loads
        int diff = 8 - (quintptr(b) & 0xf)/2;
        len -= diff;
        a += diff;
        b += diff;
    }

    if (len < 8)
        return ucstrncmp_short_tail(a, b, len);

    // 'b' is aligned
    int val = quintptr(a) & 0xf;
    a -= val/2;

    if (val == 8)
        return ucstrncmp_ssse3_alignr<8, _mm_load_si128>(a, b, len);
    else if (val == 0)
        return ucstrncmp_sse2_aligned(a, b, len);
    if (val < 8) {
        if (val < 4)
            return ucstrncmp_ssse3_alignr<2, _mm_load_si128>(a, b, len);
        else if (val == 4)
            return ucstrncmp_ssse3_alignr<4, _mm_load_si128>(a, b, len);
        else
            return ucstrncmp_ssse3_alignr<6, _mm_load_si128>(a, b, len);
    } else {
        if (val < 12)
            return ucstrncmp_ssse3_alignr<10, _mm_load_si128>(a, b, len);
        else if (val == 12)
            return ucstrncmp_ssse3_alignr<12, _mm_load_si128>(a, b, len);
        else
            return ucstrncmp_ssse3_alignr<14, _mm_load_si128>(a, b, len);
    }
}

static inline __attribute__((optimize("no-unroll-loops")))
int ucstrncmp_ssse3_aligning2_aligned(const ushort *a, const ushort *b, int len, int garbage)
{
    // len >= 8
    __m128i m1 = _mm_load_si128((const __m128i *)a);
    __m128i m2 = _mm_load_si128((const __m128i *)b);
    __m128i cmp = _mm_cmpeq_epi16(m1, m2);
    int mask = short(_mm_movemask_epi8(cmp)); // force sign extension
    mask >>= garbage;
    if (~mask) {
        // which ushort isn't equal?
        uint counter = (garbage + bsf_nonzero(~mask));
        return a[counter/2] - b[counter/2];
    }

    // the first 16-garbage bytes (8-garbage/2 ushorts) were equal
    len -= 8 - garbage/2;
    return ucstrncmp_sse2_aligned(a + 8, b + 8, len);
}

template<int N> static inline __attribute__((optimize("no-unroll-loops"),always_inline))
int ucstrncmp_ssse3_aligning2_alignr(const ushort *a, const ushort *b, int len, int garbage)
{
    // len >= 8
    __m128i lower, upper, merged;
    lower = _mm_load_si128((const __m128i*)a);
    upper = _mm_load_si128((const __m128i*)(a + 8));
    merged = _mm_alignr_epi8(upper, lower, N);

    __m128i m2 = _mm_load_si128((const __m128i*)b);
    __m128i cmp = _mm_cmpeq_epi16(merged, m2);
    int mask = short(_mm_movemask_epi8(cmp)); // force sign extension
    mask >>= garbage;
    if (~mask) {
        // which ushort isn't equal?
        uint counter = (garbage + bsf_nonzero(~mask));
        return a[counter/2 + N/2] - b[counter/2];
    }

    // the first 16-garbage bytes (8-garbage/2 ushorts) were equal
    quintptr counter = 8;
    len -= 8 - garbage/2;
    while (len >= 8) {
        lower = upper;
        upper = _mm_load_si128((__m128i *)(a + counter) + 1);
        merged = _mm_alignr_epi8(upper, lower, N);

        m2 = _mm_load_si128((__m128i *)(b + counter));
        cmp = _mm_cmpeq_epi16(merged, m2);
        ushort mask = ~uint(_mm_movemask_epi8(cmp));
        if (mask) {
            // which ushort isn't equal?
            counter += bsf_nonzero(mask)/2;
            return a[counter + N/2] - b[counter];
        }

        counter += 8;
        len -= 8;
    }

    return ucstrncmp_short_tail(a + counter + N/2, b + counter, len);
}

static inline int conditional_invert(int result, bool invert)
{
    if (invert)
        return -result;
    return result;
}

static int ucstrncmp_ssse3_aligning2(const ushort *a, const ushort *b, int len)
{
    // Different strategy from above: instead of doing two unaligned loads
    // when trying to align, we'll only do aligned loads and round down the
    // addresses of a and b. This means the first load will contain garbage
    // in the beginning of the string, which we'll shift out of the way
    // (after _mm_movemask_epi8)

    if (len < 8)
        return ucstrncmp_intwise(a, b, len);

    // both a and b are misaligned
    // we'll call the alignr function with the alignment *difference* between the two
    int offset = (quintptr(a) & 0xf) - (quintptr(b) & 0xf);
    if (offset >= 0) {
        // from this point on, b has the shortest alignment
        // and align(a) = align(b) + offset
        // round down the alignment so align(b) == align(a) == 0
        int garbage = (quintptr(b) & 0xf);
        a = (const ushort*)(quintptr(a) & ~0xf);
        b = (const ushort*)(quintptr(b) & ~0xf);

        // now the first load of b will load 'garbage' extra bytes
        // and the first load of a will load 'garbage + offset' extra bytes
        if (offset == 8)
            return ucstrncmp_ssse3_aligning2_alignr<8>(a, b, len, garbage);
        if (offset == 0)
            return ucstrncmp_ssse3_aligning2_aligned(a, b, len, garbage);
        if (offset < 8) {
            if (offset < 4)
                return ucstrncmp_ssse3_aligning2_alignr<2>(a, b, len, garbage);
            else if (offset == 4)
                return ucstrncmp_ssse3_aligning2_alignr<4>(a, b, len, garbage);
            else
                return ucstrncmp_ssse3_aligning2_alignr<6>(a, b, len, garbage);
        } else {
            if (offset < 12)
                return ucstrncmp_ssse3_aligning2_alignr<10>(a, b, len, garbage);
            else if (offset == 12)
                return ucstrncmp_ssse3_aligning2_alignr<12>(a, b, len, garbage);
            else
                return ucstrncmp_ssse3_aligning2_alignr<14>(a, b, len, garbage);
        }
    } else {
        // same as above but inverted
        int garbage = (quintptr(a) & 0xf);
        a = (const ushort*)(quintptr(a) & ~0xf);
        b = (const ushort*)(quintptr(b) & ~0xf);

        offset = -offset;
        if (offset == 8)
            return -ucstrncmp_ssse3_aligning2_alignr<8>(b, a, len, garbage);
        if (offset < 8) {
            if (offset < 4)
                return -ucstrncmp_ssse3_aligning2_alignr<2>(b, a, len, garbage);
            else if (offset == 4)
                return -ucstrncmp_ssse3_aligning2_alignr<4>(b, a, len, garbage);
            else
                return -ucstrncmp_ssse3_aligning2_alignr<6>(b, a, len, garbage);
        } else {
            if (offset < 12)
                return -ucstrncmp_ssse3_aligning2_alignr<10>(b, a, len, garbage);
            else if (offset == 12)
                return -ucstrncmp_ssse3_aligning2_alignr<12>(b, a, len, garbage);
            else
                return -ucstrncmp_ssse3_aligning2_alignr<14>(b, a, len, garbage);
        }
    }
}

#endif

typedef int (* UcstrncmpFunction)(const ushort *, const ushort *, int);
Q_DECLARE_METATYPE(UcstrncmpFunction)

void tst_QString::ucstrncmp_data() const
{
    QTest::addColumn<UcstrncmpFunction>("function");
    QTest::newRow("selftest") << UcstrncmpFunction(0);
    QTest::newRow("shortwise") << &ucstrncmp_shortwise;
    QTest::newRow("intwise") << &ucstrncmp_intwise;
#ifdef __SSE2__
    QTest::newRow("sse2") << &ucstrncmp_sse2;
    QTest::newRow("sse2_aligning") << &ucstrncmp_sse2_aligning;
#ifdef __SSSE3__
    QTest::newRow("ssse3") << &ucstrncmp_ssse3;
    QTest::newRow("ssse3_aligning") << &ucstrncmp_ssse3_aligning;
    QTest::newRow("ssse3_aligning2") << &ucstrncmp_ssse3_aligning2;
#endif
#endif
}

void tst_QString::ucstrncmp() const
{
    QFETCH(UcstrncmpFunction, function);
    if (!function) {
        static const UcstrncmpFunction func[] = {
            &ucstrncmp_shortwise,
            &ucstrncmp_intwise,
#ifdef __SSE2__
            &ucstrncmp_sse2,
            &ucstrncmp_sse2_aligning,
#ifdef __SSSE3__
            &ucstrncmp_ssse3,
            &ucstrncmp_ssse3_aligning,
            &ucstrncmp_ssse3_aligning2
#endif
#endif
        };
        static const int functionCount = sizeof func / sizeof func[0];

#ifdef Q_OS_UNIX
        const long pagesize = sysconf(_SC_PAGESIZE);
        void *page1, *page3;
        ushort *page2;
        page1 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        page2 = (ushort *)mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
        page3 = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        Q_ASSERT(quintptr(page2) == quintptr(page1) + pagesize || quintptr(page2) == quintptr(page1) - pagesize);
        Q_ASSERT(quintptr(page3) == quintptr(page2) + pagesize || quintptr(page3) == quintptr(page2) - pagesize);
        munmap(page1, pagesize);
        munmap(page3, pagesize);

        // populate our page
        for (uint i = 0; i < pagesize / sizeof(long long); ++i)
            ((long long *)page2)[i] = Q_INT64_C(0x0041004100410041);

        // the following should crash:
        //page2[-1] = 0xdead;
        //page2[pagesize / sizeof(ushort) + 1] = 0xbeef;

        static const ushort needle[] = {
            0x41, 0x41, 0x41, 0x41,   0x41, 0x41, 0x41, 0x41,
            0x41, 0x41, 0x41, 0x41,   0x41, 0x41, 0x41, 0x41,
            0x41
        };

        for (int algo = 0; algo < functionCount; ++algo) {
            // boundary condition test:
            for (int i = 0; i < 8; ++i) {
                (func[algo])(page2 + i, needle, sizeof needle / 2);
                (func[algo])(page2 - i - 1 - sizeof(needle)/2 + pagesize/2, needle, sizeof needle/2);
            }
        }

        munmap(page2, pagesize);
#endif

        for (int algo = 0; algo < functionCount; ++algo) {
            for (int i = 0; i < stringCollectionCount; ++i) {
                const ushort *p1 = stringCollectionData + stringCollection[i].offset1;
                const ushort *p2 = stringCollectionData + stringCollection[i].offset2;
                int expected = ucstrncmp_shortwise(p1, p2, stringCollection[i].len);
                expected = qBound(-1, expected, 1);

                int result = (func[algo])(p1, p2, stringCollection[i].len);
                result = qBound(-1, result, 1);
                if (expected != result)
                    qWarning().nospace()
                        << "algo=" << algo
                        << " i=" << i
                        << " failed (" << result << "!=" << expected
                        << "); strings were "
                        << QByteArray((char*)p1, stringCollection[i].len).toHex()
                        << " and "
                        << QByteArray((char*)p2, stringCollection[i].len).toHex();
            }
        }
        return;
    }

    QBENCHMARK {
        for (int i = 0; i < stringCollectionCount; ++i) {
            const ushort *p1 = stringCollectionData + stringCollection[i].offset1;
            const ushort *p2 = stringCollectionData + stringCollection[i].offset2;
            (function)(p1, p2, stringCollection[i].len);
        }
    }
}

void tst_QString::fromUtf8() const
{
    QFile file(SRCDIR "utf-8.txt");
    if (!file.open(QFile::ReadOnly)) {
        qFatal("Cannot open input file");
        return;
    }
    QByteArray data = file.readAll();
    const char *d = data.constData();
    int size = data.size();

    QBENCHMARK {
        QString::fromUtf8(d, size);
    }
}

QTEST_MAIN(tst_QString)

#include "main.moc"
