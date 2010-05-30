/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/QtTest>

#include <QtMultimedia/QVideoSurfaceFormat>

class tst_QVideoSurfaceFormat : public QObject
{
    Q_OBJECT
public:
    tst_QVideoSurfaceFormat();
    ~tst_QVideoSurfaceFormat();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void constructNull();
    void construct_data();
    void construct();
    void frameSize_data();
    void frameSize();
    void viewport_data();
    void viewport();
    void scanLineDirection_data();
    void scanLineDirection();
    void frameRate_data();
    void frameRate();
    void yCbCrColorSpace_data();
    void yCbCrColorSpace();
    void pixelAspectRatio_data();
    void pixelAspectRatio();
    void sizeHint_data();
    void sizeHint();
    void staticPropertyNames();
    void dynamicProperty();
    void compare();
    void copy();
    void assign();
};

tst_QVideoSurfaceFormat::tst_QVideoSurfaceFormat()
{
}

tst_QVideoSurfaceFormat::~tst_QVideoSurfaceFormat()
{
}

void tst_QVideoSurfaceFormat::initTestCase()
{
}

void tst_QVideoSurfaceFormat::cleanupTestCase()
{
}

void tst_QVideoSurfaceFormat::init()
{
}

void tst_QVideoSurfaceFormat::cleanup()
{
}

void tst_QVideoSurfaceFormat::constructNull()
{
    QVideoSurfaceFormat format;

    QVERIFY(!format.isValid());
    QCOMPARE(format.handleType(), QAbstractVideoBuffer::NoHandle);
    QCOMPARE(format.pixelFormat(), QVideoFrame::Format_Invalid);
    QCOMPARE(format.frameSize(), QSize());
    QCOMPARE(format.frameWidth(), -1);
    QCOMPARE(format.frameHeight(), -1);
    QCOMPARE(format.viewport(), QRect());
    QCOMPARE(format.scanLineDirection(), QVideoSurfaceFormat::TopToBottom);
    QCOMPARE(format.frameRate(), 0.0);
    QCOMPARE(format.pixelAspectRatio(), QSize(1, 1));
    QCOMPARE(format.yCbCrColorSpace(), QVideoSurfaceFormat::YCbCr_Undefined);
}

void tst_QVideoSurfaceFormat::construct_data()
{
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<QVideoFrame::PixelFormat>("pixelFormat");
    QTest::addColumn<QAbstractVideoBuffer::HandleType>("handleType");

    QTest::newRow("32x32 rgb32 no handle")
            << QSize(32, 32)
            << QVideoFrame::Format_RGB32
            << QAbstractVideoBuffer::NoHandle;

    QTest::newRow("1024x768 YUV444 GL texture")
            << QSize(32, 32)
            << QVideoFrame::Format_YUV444
            << QAbstractVideoBuffer::GLTextureHandle;
}

void tst_QVideoSurfaceFormat::construct()
{
    QFETCH(QSize, frameSize);
    QFETCH(QVideoFrame::PixelFormat, pixelFormat);
    QFETCH(QAbstractVideoBuffer::HandleType, handleType);

    QRect viewport(QPoint(0, 0), frameSize);

    QVideoSurfaceFormat format(frameSize, pixelFormat, handleType);

    QCOMPARE(format.handleType(), handleType);
    QCOMPARE(format.pixelFormat(), pixelFormat);
    QCOMPARE(format.frameSize(), frameSize);
    QCOMPARE(format.frameWidth(), frameSize.width());
    QCOMPARE(format.frameHeight(), frameSize.height());
    QCOMPARE(format.viewport(), viewport);
    QCOMPARE(format.scanLineDirection(), QVideoSurfaceFormat::TopToBottom);
    QCOMPARE(format.frameRate(), 0.0);
    QCOMPARE(format.pixelAspectRatio(), QSize(1, 1));
    QCOMPARE(format.yCbCrColorSpace(), QVideoSurfaceFormat::YCbCr_Undefined);
}

void tst_QVideoSurfaceFormat::frameSize_data()
{
    QTest::addColumn<QSize>("initialSize");
    QTest::addColumn<QSize>("newSize");

    QTest::newRow("grow")
            << QSize(64, 64)
            << QSize(1024, 1024);
    QTest::newRow("shrink")
            << QSize(1024, 1024)
            << QSize(64, 64);
    QTest::newRow("unchanged")
            << QSize(512, 512)
            << QSize(512, 512);
}

void tst_QVideoSurfaceFormat::frameSize()
{
    QFETCH(QSize, initialSize);
    QFETCH(QSize, newSize);

    QVideoSurfaceFormat format(initialSize, QVideoFrame::Format_RGB32);

    format.setFrameSize(newSize);

    QCOMPARE(format.frameSize(), newSize);
    QCOMPARE(format.property("frameSize").toSize(), newSize);
    QCOMPARE(format.frameWidth(), newSize.width());
    QCOMPARE(format.property("frameWidth").toInt(), newSize.width());
    QCOMPARE(format.frameHeight(), newSize.height());
    QCOMPARE(format.property("frameHeight").toInt(), newSize.height());
}

void tst_QVideoSurfaceFormat::viewport_data()
{
    QTest::addColumn<QSize>("initialSize");
    QTest::addColumn<QRect>("viewport");
    QTest::addColumn<QSize>("newSize");
    QTest::addColumn<QRect>("expectedViewport");

    QTest::newRow("grow reset")
            << QSize(64, 64)
            << QRect(8, 8, 48, 48)
            << QSize(1024, 1024)
            << QRect(0, 0, 1024, 1024);
    QTest::newRow("shrink reset")
            << QSize(1024, 1024)
            << QRect(8, 8, 1008, 1008)
            << QSize(64, 64)
            << QRect(0, 0, 64, 64);
    QTest::newRow("unchanged reset")
            << QSize(512, 512)
            << QRect(8, 8, 496, 496)
            << QSize(512, 512)
            << QRect(0, 0, 512, 512);
}

void tst_QVideoSurfaceFormat::viewport()
{
    QFETCH(QSize, initialSize);
    QFETCH(QRect, viewport);
    QFETCH(QSize, newSize);
    QFETCH(QRect, expectedViewport);

    {
        QRect initialViewport(QPoint(0, 0), initialSize);

        QVideoSurfaceFormat format(initialSize, QVideoFrame::Format_RGB32);

        format.setViewport(viewport);

        QCOMPARE(format.viewport(), viewport);
        QCOMPARE(format.property("viewport").toRect(), viewport);

        format.setFrameSize(newSize);

        QCOMPARE(format.viewport(), expectedViewport);
        QCOMPARE(format.property("viewport").toRect(), expectedViewport);
    }
    {
        QVideoSurfaceFormat format(initialSize, QVideoFrame::Format_RGB32);

        format.setProperty("viewport", viewport);

        QCOMPARE(format.viewport(), viewport);
        QCOMPARE(format.property("viewport").toRect(), viewport);
    }
}

void tst_QVideoSurfaceFormat::scanLineDirection_data()
{
    QTest::addColumn<QVideoSurfaceFormat::Direction>("direction");

    QTest::newRow("top to bottom")
            << QVideoSurfaceFormat::TopToBottom;

    QTest::newRow("bottom to top")
            << QVideoSurfaceFormat::BottomToTop;
}

void tst_QVideoSurfaceFormat::scanLineDirection()
{
    QFETCH(QVideoSurfaceFormat::Direction, direction);

    {
        QVideoSurfaceFormat format(QSize(16, 16), QVideoFrame::Format_RGB32);

        format.setScanLineDirection(direction);

        QCOMPARE(format.scanLineDirection(), direction);
        QCOMPARE(
                qvariant_cast<QVideoSurfaceFormat::Direction>(format.property("scanLineDirection")),
                direction);
    }
    {
        QVideoSurfaceFormat format(QSize(16, 16), QVideoFrame::Format_RGB32);

        format.setProperty("scanLineDirection", qVariantFromValue(direction));

        QCOMPARE(format.scanLineDirection(), direction);
        QCOMPARE(
                qvariant_cast<QVideoSurfaceFormat::Direction>(format.property("scanLineDirection")),
                direction);
    }
}

void tst_QVideoSurfaceFormat::frameRate_data()
{
    QTest::addColumn<qreal>("frameRate");

    QTest::newRow("null")
            << qreal(0.0);
    QTest::newRow("1/1")
            << qreal(1.0);
    QTest::newRow("24/1")
            << qreal(24.0);
    QTest::newRow("15/2")
            << qreal(7.5);
}

void tst_QVideoSurfaceFormat::frameRate()
{
    QFETCH(qreal, frameRate);

    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);

        format.setFrameRate(frameRate);

        QCOMPARE(format.frameRate(), frameRate);
        QCOMPARE(qvariant_cast<qreal>(format.property("frameRate")), frameRate);
    }
    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);

        format.setFrameRate(frameRate);
        format.setProperty("frameRate", frameRate);

        QCOMPARE(format.frameRate(), frameRate);
        QCOMPARE(qvariant_cast<qreal>(format.property("frameRate")), frameRate);
    }
}

void tst_QVideoSurfaceFormat::yCbCrColorSpace_data()
{
    QTest::addColumn<QVideoSurfaceFormat::YCbCrColorSpace>("colorspace");

    QTest::newRow("undefined")
            << QVideoSurfaceFormat::YCbCr_Undefined;
    QTest::newRow("bt709")
            << QVideoSurfaceFormat::YCbCr_BT709;
    QTest::newRow("xvYCC601")
            << QVideoSurfaceFormat::YCbCr_xvYCC601;
    QTest::newRow("JPEG")
            << QVideoSurfaceFormat::YCbCr_JPEG;
}

void tst_QVideoSurfaceFormat::yCbCrColorSpace()
{
    QFETCH(QVideoSurfaceFormat::YCbCrColorSpace, colorspace);

    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);
        format.setYCbCrColorSpace(colorspace);

        QCOMPARE(format.yCbCrColorSpace(), colorspace);
        QCOMPARE(qvariant_cast<QVideoSurfaceFormat::YCbCrColorSpace>(format.property("yCbCrColorSpace")),
                colorspace);
    }
    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);
        format.setProperty("yCbCrColorSpace", qVariantFromValue(colorspace));

        QCOMPARE(format.yCbCrColorSpace(), colorspace);
        QCOMPARE(qvariant_cast<QVideoSurfaceFormat::YCbCrColorSpace>(format.property("yCbCrColorSpace")),
                colorspace);
    }
}

void tst_QVideoSurfaceFormat::pixelAspectRatio_data()
{
    QTest::addColumn<QSize>("aspectRatio");

    QTest::newRow("1:1")
            << QSize(1, 1);
    QTest::newRow("4:3")
            << QSize(4, 3);
    QTest::newRow("16:9")
            << QSize(16, 9);
}

void tst_QVideoSurfaceFormat::pixelAspectRatio()
{
    QFETCH(QSize, aspectRatio);

    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);
        format.setPixelAspectRatio(aspectRatio);

        QCOMPARE(format.pixelAspectRatio(), aspectRatio);
        QCOMPARE(format.property("pixelAspectRatio").toSize(), aspectRatio);
    }
    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);
        format.setPixelAspectRatio(aspectRatio.width(), aspectRatio.height());

        QCOMPARE(format.pixelAspectRatio(), aspectRatio);
        QCOMPARE(format.property("pixelAspectRatio").toSize(), aspectRatio);
    }
    {
        QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);
        format.setProperty("pixelAspectRatio", aspectRatio);

        QCOMPARE(format.pixelAspectRatio(), aspectRatio);
        QCOMPARE(format.property("pixelAspectRatio").toSize(), aspectRatio);
    }
}

void tst_QVideoSurfaceFormat::sizeHint_data()
{
    QTest::addColumn<QSize>("frameSize");
    QTest::addColumn<QRect>("viewport");
    QTest::addColumn<QSize>("aspectRatio");
    QTest::addColumn<QSize>("sizeHint");

    QTest::newRow("(0, 0, 1024x768), 1:1")
            << QSize(1024, 768)
            << QRect(0, 0, 1024, 768)
            << QSize(1, 1)
            << QSize(1024, 768);
    QTest::newRow("0, 0, 1024x768), 4:3")
            << QSize(1024, 768)
            << QRect(0, 0, 1024, 768)
            << QSize(4, 3)
            << QSize(1365, 768);
    QTest::newRow("(168, 84, 800x600), 1:1")
        << QSize(1024, 768)
        << QRect(168, 84, 800, 600)
        << QSize(1, 1)
        << QSize(800, 600);
    QTest::newRow("(168, 84, 800x600), 4:3")
        << QSize(1024, 768)
        << QRect(168, 84, 800, 600)
        << QSize(4, 3)
        << QSize(1066, 600);
}

void tst_QVideoSurfaceFormat::sizeHint()
{
    QFETCH(QSize, frameSize);
    QFETCH(QRect, viewport);
    QFETCH(QSize, aspectRatio);
    QFETCH(QSize, sizeHint);

    QVideoSurfaceFormat format(frameSize, QVideoFrame::Format_RGB32);
    format.setViewport(viewport);
    format.setPixelAspectRatio(aspectRatio);

    QCOMPARE(format.sizeHint(), sizeHint);
    QCOMPARE(format.property("sizeHint").toSize(), sizeHint);
}

void tst_QVideoSurfaceFormat::staticPropertyNames()
{
    QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);

    QList<QByteArray> propertyNames = format.propertyNames();

    QVERIFY(propertyNames.contains("handleType"));
    QVERIFY(propertyNames.contains("pixelFormat"));
    QVERIFY(propertyNames.contains("frameSize"));
    QVERIFY(propertyNames.contains("frameWidth"));
    QVERIFY(propertyNames.contains("viewport"));
    QVERIFY(propertyNames.contains("scanLineDirection"));
    QVERIFY(propertyNames.contains("frameRate"));
    QVERIFY(propertyNames.contains("pixelAspectRatio"));
    QVERIFY(propertyNames.contains("yCbCrColorSpace"));
    QVERIFY(propertyNames.contains("sizeHint"));
    QCOMPARE(propertyNames.count(), 10);
}

void tst_QVideoSurfaceFormat::dynamicProperty()
{
    QVideoSurfaceFormat format(QSize(64, 64), QVideoFrame::Format_RGB32);

    QCOMPARE(format.property("integer"), QVariant());
    QCOMPARE(format.property("size"), QVariant());
    QCOMPARE(format.property("string"), QVariant());
    QCOMPARE(format.property("null"), QVariant());

    QList<QByteArray> propertyNames = format.propertyNames();

    QCOMPARE(propertyNames.count(QByteArray("integer")), 0);
    QCOMPARE(propertyNames.count(QByteArray("string")), 0);
    QCOMPARE(propertyNames.count(QByteArray("size")), 0);
    QCOMPARE(propertyNames.count(QByteArray("null")), 0);

    format.setProperty("string", QString::fromLatin1("Hello"));
    format.setProperty("integer", 198);
    format.setProperty("size", QSize(43, 65));

    QCOMPARE(format.property("integer").toInt(), 198);
    QCOMPARE(format.property("size").toSize(), QSize(43, 65));
    QCOMPARE(format.property("string").toString(), QString::fromLatin1("Hello"));

    propertyNames = format.propertyNames();

    QCOMPARE(propertyNames.count(QByteArray("integer")), 1);
    QCOMPARE(propertyNames.count(QByteArray("string")), 1);
    QCOMPARE(propertyNames.count(QByteArray("size")), 1);

    format.setProperty("integer", 125423);
    format.setProperty("size", QSize(1, 986));

    QCOMPARE(format.property("integer").toInt(), 125423);
    QCOMPARE(format.property("size").toSize(), QSize(1, 986));
    QCOMPARE(format.property("string").toString(), QString::fromLatin1("Hello"));

    propertyNames = format.propertyNames();

    QCOMPARE(propertyNames.count(QByteArray("integer")), 1);
    QCOMPARE(propertyNames.count(QByteArray("string")), 1);
    QCOMPARE(propertyNames.count(QByteArray("size")), 1);

    format.setProperty("string", QVariant());
    format.setProperty("size", QVariant());
    format.setProperty("null", QVariant());

    QCOMPARE(format.property("integer").toInt(), 125423);
    QCOMPARE(format.property("size"), QVariant());
    QCOMPARE(format.property("string"), QVariant());
    QCOMPARE(format.property("null"), QVariant());

    propertyNames = format.propertyNames();

    QCOMPARE(propertyNames.count(QByteArray("integer")), 1);
    QCOMPARE(propertyNames.count(QByteArray("string")), 0);
    QCOMPARE(propertyNames.count(QByteArray("size")), 0);
    QCOMPARE(propertyNames.count(QByteArray("null")), 0);
}

void tst_QVideoSurfaceFormat::compare()
{
    QVideoSurfaceFormat format1(
            QSize(16, 16), QVideoFrame::Format_RGB32, QAbstractVideoBuffer::GLTextureHandle);
    QVideoSurfaceFormat format2(
            QSize(16, 16), QVideoFrame::Format_RGB32, QAbstractVideoBuffer::GLTextureHandle);
    QVideoSurfaceFormat format3(
            QSize(32, 32), QVideoFrame::Format_YUV444, QAbstractVideoBuffer::GLTextureHandle);
    QVideoSurfaceFormat format4(
            QSize(16, 16), QVideoFrame::Format_RGB32, QAbstractVideoBuffer::UserHandle);

    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);
    QCOMPARE(format1 == format3, false);
    QCOMPARE(format1 != format3, true);
    QCOMPARE(format1 == format4, false);
    QCOMPARE(format1 != format4, true);

    format2.setFrameSize(1024, 768);

    // Not equal, frame size differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format1.setFrameSize(1024, 768);

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format1.setViewport(QRect(0, 0, 800, 600));
    format2.setViewport(QRect(112, 84, 800, 600));

    // Not equal, viewports differ.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format1.setViewport(QRect(112, 84, 800, 600));

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format2.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);

    // Not equal scan line direction differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format1.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format1.setFrameRate(7.5);

    // Not equal frame rate differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format2.setFrameRate(qreal(7.50001));

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format2.setPixelAspectRatio(4, 3);

    // Not equal pixel aspect ratio differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format1.setPixelAspectRatio(QSize(4, 3));

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format2.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_xvYCC601);

    // Not equal yuv color space differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format1.setYCbCrColorSpace(QVideoSurfaceFormat::YCbCr_xvYCC601);

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format1.setProperty("integer", 12);

    // Not equal, property mismatch.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format2.setProperty("integer", 45);

    // Not equal, integer differs.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format2.setProperty("integer", 12);

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format1.setProperty("string", QString::fromLatin1("Hello"));
    format2.setProperty("size", QSize(12, 54));

    // Not equal, property mismatch.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);

    format2.setProperty("string", QString::fromLatin1("Hello"));
    format1.setProperty("size", QSize(12, 54));

    // Equal.
    QCOMPARE(format1 == format2, true);
    QCOMPARE(format1 != format2, false);

    format1.setProperty("string", QVariant());

    // Not equal, property mismatch.
    QCOMPARE(format1 == format2, false);
    QCOMPARE(format1 != format2, true);
}


void tst_QVideoSurfaceFormat::copy()
{
    QVideoSurfaceFormat original(
            QSize(1024, 768), QVideoFrame::Format_ARGB32, QAbstractVideoBuffer::GLTextureHandle);
    original.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);

    QVideoSurfaceFormat copy(original);

    QCOMPARE(copy.handleType(), QAbstractVideoBuffer::GLTextureHandle);
    QCOMPARE(copy.pixelFormat(), QVideoFrame::Format_ARGB32);
    QCOMPARE(copy.frameSize(), QSize(1024, 768));
    QCOMPARE(copy.scanLineDirection(), QVideoSurfaceFormat::BottomToTop);

    QCOMPARE(original == copy, true);
    QCOMPARE(original != copy, false);

    copy.setScanLineDirection(QVideoSurfaceFormat::TopToBottom);

    QCOMPARE(copy.scanLineDirection(), QVideoSurfaceFormat::TopToBottom);

    QCOMPARE(original.scanLineDirection(), QVideoSurfaceFormat::BottomToTop);

    QCOMPARE(original == copy, false);
    QCOMPARE(original != copy, true);
}

void tst_QVideoSurfaceFormat::assign()
{
    QVideoSurfaceFormat copy(
            QSize(64, 64), QVideoFrame::Format_AYUV444, QAbstractVideoBuffer::UserHandle);

    QVideoSurfaceFormat original(
            QSize(1024, 768), QVideoFrame::Format_ARGB32, QAbstractVideoBuffer::GLTextureHandle);
    original.setScanLineDirection(QVideoSurfaceFormat::BottomToTop);

    copy = original;

    QCOMPARE(copy.handleType(), QAbstractVideoBuffer::GLTextureHandle);
    QCOMPARE(copy.pixelFormat(), QVideoFrame::Format_ARGB32);
    QCOMPARE(copy.frameSize(), QSize(1024, 768));
    QCOMPARE(copy.scanLineDirection(), QVideoSurfaceFormat::BottomToTop);

    QCOMPARE(original == copy, true);
    QCOMPARE(original != copy, false);

    copy.setScanLineDirection(QVideoSurfaceFormat::TopToBottom);

    QCOMPARE(copy.scanLineDirection(), QVideoSurfaceFormat::TopToBottom);

    QCOMPARE(original.scanLineDirection(), QVideoSurfaceFormat::BottomToTop);

    QCOMPARE(original == copy, false);
    QCOMPARE(original != copy, true);
}

QTEST_MAIN(tst_QVideoSurfaceFormat)

#include "tst_qvideosurfaceformat.moc"
