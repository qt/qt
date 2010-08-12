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

#include <private/qsimd_p.h>

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

static ushort databuffer[4096];

tst_QString::tst_QString()
{
    // populate databuffer with our seed, each byte 3 times in a row
    // include the NUL!
    static const char seed[] = "AAAAAAAAAEhlbGxvIFdvcmxkIAAAAAA=";
    static const int repeat = 3;
    int pos = 0;
    for (ushort *p = databuffer; p < databuffer + (sizeof(databuffer) / sizeof(databuffer[0])); p += repeat) {
        for (int j = 0; j < repeat; ++p, ++j)
            *p = seed[pos];
        pos = (pos + 1) % sizeof(seed);
    }
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

static bool equals2_memcmp_call(ushort *p1, ushort *p2, int len)
{
    return memcmp(p1, p2, len * 2) == 0;
}

static bool equals2_bytewise(ushort *p1, ushort *p2, int len)
{
    uchar *b1 = (uchar *)p1;
    uchar *b2 = (uchar *)p2;
    len *= 2;
    while (len--)
        if (*b1++ != *b2++)
            return false;
    return true;
}

static bool equals2_shortwise(ushort *p1, ushort *p2, int len)
{
    register ushort * const end = p1 + len;
    for ( ; p1 != end; ++p1, ++p2)
        if (*p1 != *p2)
            return false;
    return true;
}

static bool equals2_intwise(ushort *p1, ushort *p2, int length)
{
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

#ifdef __SSE2__
static bool equals2_sse2(ushort *p1, ushort *p2, int len)
{
    if (len > 8) {
        while (len > 8) {
            __m128i q1 = _mm_loadu_si128((__m128i *)p1);
            __m128i q2 = _mm_loadu_si128((__m128i *)p2);
            __m128i cmp = _mm_cmpeq_epi16(q1, q2);
            if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
                return false;

            len -= 8;
            p1 += 8;
            p2 += 8;
        }
    }

    return equals2_shortwise(p1, p2, len);
}

static inline
#ifdef Q_CC_GNU
__attribute__((always_inline))
#endif
bool prolog_align(ushort *&p1, ushort *&p2, int &len)
{
    const ushort *end = (ushort*) ((quintptr(p1) + 15) & ~15);
    if (end > p1 + len)
        end = p1 + len;
    for ( ; p1 != end; ++p1, ++p2, --len)
        if (*p1 != *p2)
            return false;
    return true;
}

static bool equals2_sse2_aligning(ushort *p1, ushort *p2, int len)
{
    if (len > 8) {
        if (!prolog_align(p1, p2, len))
            return false;
        while (len > 8) {
            __m128i q1 = _mm_load_si128((__m128i *)p1);
            __m128i q2 = _mm_loadu_si128((__m128i *)p2);
            __m128i cmp = _mm_cmpeq_epi16(q1, q2);
            if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
                return false;

            len -= 8;
            p1 += 8;
            p2 += 8;
        }
    }

    return equals2_shortwise(p1, p2, len);
}

template<int N> static inline bool equals2_ssse3_alignr(__m128i *m1, __m128i *m2, int len)
{
    __m128i lower = _mm_load_si128(m1);
    while (len > 8) {
        __m128i upper = _mm_load_si128(m1 + 1);
        __m128i correct;
        correct = _mm_alignr_epi8(upper, lower, N);

        __m128i q2 = _mm_loadu_si128(m2);
        __m128i cmp = _mm_cmpeq_epi16(correct, q2);
        if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
            return false;

        len -= 8;
        ++m2;
        ++m1;
        lower = upper;
    }

    // tail
    return len == 0 || equals2_shortwise((ushort *)m1 + N / 2, (ushort*)m2, len);
}

static inline bool equals2_ssse3_aligned(__m128i *m1, __m128i *m2, int len)
{
    while (len > 8) {
        __m128i q2 = _mm_loadu_si128(m2);
        __m128i cmp = _mm_cmpeq_epi16(*m1, q2);
        if (ushort(_mm_movemask_epi8(cmp)) != 0xffff)
            return false;

        len -= 8;
        ++m1;
        ++m2;
    }
    return len == 0 || equals2_shortwise((ushort *)m1, (ushort *)m2, len);
}

//#ifdef __SSSE3__
static bool equals2_ssse3(ushort *p1, ushort *p2, int len)
{
    // p1 & 0xf can be:
    //   0,  2,  4,  6,  8, 10, 12, 14
    // If it's 0, we're aligned
    // If it's not, then we're interested in the 16 - (p1 & 0xf) bytes only

    if (len > 8) {
        // find the last aligned position below the p1 memory
        __m128i *m1 = (__m128i *)(quintptr(p1) & ~0xf);
        __m128i *m2 = (__m128i *)p2;
        uchar diff = quintptr(p1) - quintptr(m1);

        // diff contains the number of extra bytes
        if (diff < 8) {
            if (diff < 4) {
                if (diff == 0)
                    return equals2_ssse3_aligned(m1, m2, len);
                else // diff == 2
                    return equals2_ssse3_alignr<2>(m1, m2, len);
            } else {
                if (diff == 4)
                    return equals2_ssse3_alignr<4>(m1, m2, len);
                else // diff == 6
                    return equals2_ssse3_alignr<6>(m1, m2, len);
            }
        } else {
            if (diff < 12) {
                if (diff == 8)
                    return equals2_ssse3_alignr<8>(m1, m2, len);
                else // diff == 10
                    return equals2_ssse3_alignr<10>(m1, m2, len);
            } else {
                if (diff == 12)
                    return equals2_ssse3_alignr<12>(m1, m2, len);
                else // diff == 14
                    return equals2_ssse3_alignr<14>(m1, m2, len);
            }
        }

//        switch (diff) {
//        case 0:
//            return equals2_ssse3_aligned(m1, m2, len);
//        case 2:
//            return equals2_ssse3_alignr<2>(m1, m2, len);
//        case 4:
//            return equals2_ssse3_alignr<4>(m1, m2, len);
//        case 6:
//            return equals2_ssse3_alignr<6>(m1, m2, len);
//        case 8:
//            return equals2_ssse3_alignr<8>(m1, m2, len);
//        case 10:
//            return equals2_ssse3_alignr<10>(m1, m2, len);
//        case 12:
//            return equals2_ssse3_alignr<12>(m1, m2, len);
//        case 14:
//            return equals2_ssse3_alignr<14>(m1, m2, len);
//        }
    }

    // tail
    return equals2_shortwise(p1, p2, len);
}
//#endif

//#ifdef __SSE4_1__
static bool equals2_sse4(ushort *p1, ushort *p2, int len)
{
    // We use the pcmpestrm instruction searching for differences (negative polarity)
    // it will reset CF if it's all equal
    // it will reset OF if the first char is equal
    // it will set ZF & SF if the length is less than 8 (which means we've done the last operation)
    // the three possible conditions are:
    //  difference found:         CF = 1
    //  all equal, not finished:  CF = ZF = SF = 0
    //  all equal, finished:      CF = 0, ZF = SF = 1
    bool result;
    asm (
        "movd       %%ebx, %%xmm1\n\t"
        "sub        %[p1], %[p2]\n\t"
        "mov        %[p1], %%ebx\n\t"
        "sub        $16, %%ebx\n\t"
        "add        $8, %[len]\n\t"
        "0:\n\t"
        "add        $16, %%ebx\n\t"
        "sub        $8, %[len]\n\t"
        "movdqu     (%%ebx), %%xmm0\n\t"
        "mov        %[len], %%edx\n\t"
        "pcmpestrm  %[mode], (%[p2],%%ebx), %%xmm0\n\t"
        "ja         0b\n\t"
        "1:\n\t"
        "mov        $0, %%eax\n\t"
        "setnc      %%al\n\t"
        "movd       %%xmm1, %%ebx\n\t"
        : [result] "=a" (result)
        : [len] "0" (len),
          [p1] "d" (p1),
          [p2] "r" (p2),
          [mode] "K" (_SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY)
        : "%xmm0", "%xmm1"
    );
    return result;
}
//#endif
#endif

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
#ifdef __SSSE3__
    QTest::newRow("ssse3") << 6;
#ifdef __SSE4_1__
    QTest::newRow("sse4.2") << 7;
#endif
#endif
#endif
}

void tst_QString::equals2() const
{
    static const short positions[] = {
        190, 1719, 2149, 1752,
        158, 244, 365, 1117,
        254, 265, 1047, 1785,
        1435, 552, 1476, 2030,
        // 16
        421, 1840, 2209, 232,
        1389, 907, 1500, 1479,
        1152, 541, 655, 1960,
        1642, 299, 740, 1995,
        // 32
        1946, 1407, 1272, 1946,
        1459, 1851, 1717, 1484,
        1761, 1630, 1377, 1675,
        629, 341, 661, 244
        // 48
    };
    // the length list must not contain 0
    static const int lens[] = {
        11, // 0
        40,
        28,
        38,
        9,
        52, // 5
        48,
        38,
        29,
        7,
        2,  // 10
        49,
        41,
        5,
        20,
        62  // 15
    };

    typedef bool (* FuncPtr)(ushort *, ushort *, int);
    static const FuncPtr func[] = {
        equals2_memcmp_call, // 0
        equals2_bytewise, // 1
        equals2_shortwise, // 1
        equals2_intwise, // 3
#ifdef __SSE2__
        equals2_sse2, // 4
        equals2_sse2_aligning, // 5
#ifdef __SSSE3__
        equals2_ssse3, // 6
#ifdef __SSE4_1__
        equals2_sse4, // 7
#endif
#endif
#endif
        0
    };

    QFETCH(int, algorithm);
    if (algorithm == -1) {
        for (uint pos1 = 0; pos1 < sizeof positions / sizeof positions[0]; ++pos1)
            for (uint pos2 = 0; pos2 < (sizeof positions / sizeof positions[0]) - 32; ++pos2)
                for (uint len = 0; len < sizeof lens / sizeof lens[0]; ++len) {
                    ushort *p1 = databuffer + positions[pos1];
                    ushort *p2 = databuffer + positions[pos2];
                    bool expected = memcmp(p1, p2, lens[len] * 2) == 0;

                    for (uint algo = 0; algo < -1 + (sizeof func / sizeof func[0]); ++algo) {
                        bool result = (func[algo])(p1, p2, lens[len]);
                        if (expected != result)
                            qWarning().nospace()
                                    << "algo=" << algo
                                    << " pos1=" << positions[pos1]
                                    << " pos2=" << positions[pos2]
                                    << " len=" << lens[len]
                                    << " failed (" << result << "!=" << expected
                                    << "); strings were "
                                    << QByteArray((char*)p1, lens[len]).toHex()
                                    << " and "
                                    << QByteArray((char*)p2, lens[len]).toHex();
                    }

                }
        return;
    }

    QBENCHMARK {
        for (uint pos1 = 0; pos1 < sizeof positions / sizeof positions[0]; ++pos1)
            for (uint pos2 = 0; pos2 < (sizeof positions / sizeof positions[0]) - 32; ++pos2)
                for (uint len = 0; len < sizeof lens / sizeof lens[0]; ++len) {
                    bool result = (func[algorithm])(databuffer + positions[pos1], databuffer + positions[pos2], lens[len]);
                    Q_UNUSED(result);
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
