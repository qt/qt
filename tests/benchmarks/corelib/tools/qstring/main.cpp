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

void tst_QString::equals2_data() const
{
    QTest::addColumn<int>("algorithm");
    QTest::newRow("selftest") << -1;
    QTest::newRow("memcmp_call") << 0;
    QTest::newRow("bytewise") << 1;
    QTest::newRow("shortwise") << 2;
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
