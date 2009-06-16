/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QPixmap>
#include <QSet>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTimer>

typedef QMap<QString, QString> QStringMap;
typedef QList<int> QIntList;
Q_DECLARE_METATYPE(QImage)
Q_DECLARE_METATYPE(QRect)
Q_DECLARE_METATYPE(QSize)
Q_DECLARE_METATYPE(QColor)
Q_DECLARE_METATYPE(QStringMap)
Q_DECLARE_METATYPE(QIntList)
Q_DECLARE_METATYPE(QIODevice *)
Q_DECLARE_METATYPE(QImage::Format)

//TESTED_FILES=

class tst_QImageReader : public QObject
{
    Q_OBJECT

public:
    tst_QImageReader();
    virtual ~tst_QImageReader();

public slots:
    void init();
    void cleanup();

private slots:
    void getSetCheck();
    void readImage_data();
    void readImage();
    void jpegRgbCmyk();

    void setScaledSize_data();
    void setScaledSize();

    void setClipRect_data();
    void setClipRect();

    void setScaledClipRect_data();
    void setScaledClipRect();

    void imageFormat_data();
    void imageFormat();

    void blackXPM();
    void transparentXPM();
    void multiWordNamedColorXPM();

    void supportedFormats();

    void readFromDevice_data();
    void readFromDevice();

    void readFromFileAfterJunk_data();
    void readFromFileAfterJunk();

    void setBackgroundColor_data();
    void setBackgroundColor();

    void supportsAnimation_data();
    void supportsAnimation();

    void description_data();
    void description();

    void readFromResources_data();
    void readFromResources();

    void dotsPerMeter_data();
    void dotsPerMeter();

    void physicalDpi_data();
    void physicalDpi();

    void sizeBeforeRead_data();
    void sizeBeforeRead();

    void imageFormatBeforeRead_data();
    void imageFormatBeforeRead();

#if defined QTEST_HAVE_GIF
    void gifHandlerBugs();
#endif

    void readCorruptImage_data();
    void readCorruptImage();
    void readCorruptBmp();

    void supportsOption_data();
    void supportsOption();

#if defined QTEST_HAVE_TIFF
    void tiffCompression_data();
    void tiffCompression();
    void tiffEndianness();
#endif

    void autoDetectImageFormat();
    void fileNameProbing();
};

// Testing get/set functions
void tst_QImageReader::getSetCheck()
{
    QImageReader obj1;
    // QIODevice * QImageReader::device()
    // void QImageReader::setDevice(QIODevice *)
    QFile *var1 = new QFile;
    obj1.setDevice(var1);

    //A bit strange but that's the only way to compile under windows.
    QCOMPARE((QIODevice *) var1, obj1.device());
    obj1.setDevice((QIODevice *)0);
    QCOMPARE((QIODevice *) 0,
             obj1.device());
    delete var1;
}

tst_QImageReader::tst_QImageReader()
{
}

tst_QImageReader::~tst_QImageReader()
{

}

void tst_QImageReader::init()
{
}

void tst_QImageReader::cleanup()
{
}

void tst_QImageReader::readImage_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("success");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("empty") << QString() << false << QByteArray();
    QTest::newRow("BMP: colorful") << QString("colorful.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: font") << QString("font.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: signed char") << QString("crash-signed-char.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: 4bpp RLE") << QString("4bpp-rle.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: 4bpp uncompressed") << QString("tst7.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: 16bpp") << QString("16bpp.bmp") << true << QByteArray("bmp");
    QTest::newRow("BMP: negative height") << QString("negativeheight.bmp") << true << QByteArray("bmp");
    QTest::newRow("XPM: marble") << QString("marble.xpm") << true << QByteArray("xpm");
    QTest::newRow("PNG: kollada") << QString("kollada.png") << true << QByteArray("png");
    QTest::newRow("PPM: teapot") << QString("teapot.ppm") << true << QByteArray("ppm");
    QTest::newRow("PPM: runners") << QString("runners.ppm") << true << QByteArray("ppm");
    QTest::newRow("PPM: test") << QString("test.ppm") << true << QByteArray("ppm");
    QTest::newRow("XBM: gnus") << QString("gnus.xbm") << true << QByteArray("xbm");
#if defined QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << QString("beavis.jpg") << true << QByteArray("jpeg");
#endif
#if defined QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << QString("earth.gif") << true << QByteArray("gif");
    QTest::newRow("GIF: trolltech") << QString("trolltech.gif") << true << QByteArray("gif");
#endif
#if defined QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << QString("ball.mng") << true << QByteArray("mng");
    QTest::newRow("MNG: fire") << QString("fire.mng") << true << QByteArray("mng");
#endif
}

void tst_QImageReader::readImage()
{
    QFETCH(QString, fileName);
    QFETCH(bool, success);
    QFETCH(QByteArray, format);

    for (int i = 0; i < 2; ++i) {
        QImageReader io("images/" + fileName, i ? QByteArray() : format);
        if (success) {
            if (!io.supportsAnimation())
                QVERIFY(io.imageCount() > 0);
        } else {
            QCOMPARE(io.imageCount(), -1);
        }
        QImage image = io.read();
        if (!success) {
            QVERIFY(image.isNull());
            return;
        }

        QVERIFY2(!image.isNull(), io.errorString().toLatin1().constData());

        // No format
        QImageReader io2("images/" + fileName);
        QVERIFY2(!io2.read().isNull(), io.errorString().toLatin1().constData());

        // No extension, no format
        QImageReader io3("images/" + fileName.left(fileName.lastIndexOf(QLatin1Char('.'))));
        QVERIFY2(!io3.read().isNull(), io.errorString().toLatin1().constData());

        // Read into \a image2
        QImage image2;
        QImageReader image2Reader("images/" + fileName, i ? QByteArray() : format);
        QCOMPARE(image2Reader.format(), format);
        QVERIFY(image2Reader.read(&image2));
        if (image2Reader.canRead()) {
            if (i)
                QVERIFY(!image2Reader.format().isEmpty());
            else
                QCOMPARE(image2Reader.format(), format);
        } else {
            if (i)
                QVERIFY(image2Reader.format().isEmpty());
            else
                QVERIFY(!image2Reader.format().isEmpty());
        }
        QCOMPARE(image, image2);

        do {
            QVERIFY2(!image.isNull(), io.errorString().toLatin1().constData());
        } while (!(image = io.read()).isNull());
    }
}

void tst_QImageReader::jpegRgbCmyk()
{
    QImage image1(QLatin1String("images/YCbCr_cmyk.jpg"));
    QImage image2(QLatin1String("images/YCbCr_cmyk.png"));

    QCOMPARE(image1, image2);
}

void tst_QImageReader::setScaledSize_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QSize>("newSize");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("BMP: colorful") << "images/colorful" << QSize(200, 200) << QByteArray("bmp");
    QTest::newRow("BMP: font") << "images/font" << QSize(200, 200) << QByteArray("bmp");
    QTest::newRow("XPM: marble") << "images/marble" << QSize(200, 200) << QByteArray("xpm");
    QTest::newRow("PNG: kollada") << "images/kollada" << QSize(200, 200) << QByteArray("png");
    QTest::newRow("PPM: teapot") << "images/teapot" << QSize(200, 200) << QByteArray("ppm");
    QTest::newRow("PPM: runners") << "images/runners.ppm" << QSize(400, 400) << QByteArray("ppm");
    QTest::newRow("PPM: test") << "images/test.ppm" << QSize(10, 10) << QByteArray("ppm");
    QTest::newRow("XBM: gnus") << "images/gnus" << QSize(200, 200) << QByteArray("xbm");
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << "images/beavis" << QSize(200, 200) << QByteArray("jpeg");
#endif // QTEST_HAVE_JPEG
#ifdef QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << "images/earth" << QSize(200, 200) << QByteArray("gif");
    QTest::newRow("GIF: trolltech") << "images/trolltech" << QSize(200, 200) << QByteArray("gif");
#endif // QTEST_HAVE_GIF
#ifdef QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << "images/ball" << QSize(200, 200) << QByteArray("mng");
    QTest::newRow("MNG: fire") << "images/fire" << QSize(200, 200) << QByteArray("mng");
#endif // QTEST_HAVE_MNG
}

void tst_QImageReader::setScaledSize()
{
    QFETCH(QString, fileName);
    QFETCH(QSize, newSize);
    QFETCH(QByteArray, format);

    if (!format.isEmpty() && !QImageReader::supportedImageFormats().contains(format))
        QSKIP("Qt does not support reading the \"" + format + "\" format", SkipSingle);

    QImageReader reader(fileName);
    reader.setScaledSize(newSize);
    QImage image = reader.read();
    QVERIFY(!image.isNull());

    QCOMPARE(image.size(), newSize);
}

void tst_QImageReader::setClipRect_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QRect>("newRect");
    QTest::addColumn<QByteArray>("format");
    QTest::newRow("BMP: colorful") << "images/colorful" << QRect(0, 0, 50, 50) << QByteArray("bmp");
    QTest::newRow("BMP: font") << "images/font" << QRect(0, 0, 50, 50) << QByteArray("bmp");
    QTest::newRow("BMP: 4bpp uncompressed") << "images/tst7.bmp" << QRect(0, 0, 31, 31) << QByteArray("bmp");
    QTest::newRow("XPM: marble") << "images/marble" << QRect(0, 0, 50, 50) << QByteArray("xpm");
    QTest::newRow("PNG: kollada") << "images/kollada" << QRect(0, 0, 50, 50) << QByteArray("png");
    QTest::newRow("PPM: teapot") << "images/teapot" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("PPM: runners") << "images/runners.ppm" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("PPM: test") << "images/test.ppm" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("XBM: gnus") << "images/gnus" << QRect(0, 0, 50, 50) << QByteArray("xbm");
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << "images/beavis" << QRect(0, 0, 50, 50) << QByteArray("jpeg");
#endif // QTEST_HAVE_JPEG
#ifdef QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << "images/earth" << QRect(0, 0, 50, 50) << QByteArray("gif");
    QTest::newRow("GIF: trolltech") << "images/trolltech" << QRect(0, 0, 50, 50) << QByteArray("gif");
#endif // QTEST_HAVE_GIF
#ifdef QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << "images/ball" << QRect(0, 0, 50, 50) << QByteArray("mng");
    QTest::newRow("MNG: fire") << "images/fire" << QRect(0, 0, 50, 50) << QByteArray("mng");
#endif // QTEST_HAVE_MNG
}

void tst_QImageReader::setClipRect()
{
    QFETCH(QString, fileName);
    QFETCH(QRect, newRect);
    QFETCH(QByteArray, format);

    if (!format.isEmpty() && !QImageReader::supportedImageFormats().contains(format))
        QSKIP("Qt does not support reading the \"" + format + "\" format", SkipSingle);

    QImageReader reader(fileName);
    reader.setClipRect(newRect);
    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(image.rect(), newRect);

    QImageReader originalReader(fileName);
    QImage originalImage = originalReader.read();
    QCOMPARE(originalImage.copy(newRect), image);
}

void tst_QImageReader::setScaledClipRect_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QRect>("newRect");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("BMP: colorful") << "images/colorful" << QRect(0, 0, 50, 50) << QByteArray("bmp");
    QTest::newRow("BMP: font") << "images/font" << QRect(0, 0, 50, 50) << QByteArray("bmp");
    QTest::newRow("XPM: marble") << "images/marble" << QRect(0, 0, 50, 50) << QByteArray("xpm");
    QTest::newRow("PNG: kollada") << "images/kollada" << QRect(0, 0, 50, 50) << QByteArray("png");
    QTest::newRow("PPM: teapot") << "images/teapot" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("PPM: runners") << "images/runners.ppm" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("PPM: test") << "images/test.ppm" << QRect(0, 0, 50, 50) << QByteArray("ppm");
    QTest::newRow("XBM: gnus") << "images/gnus" << QRect(0, 0, 50, 50) << QByteArray("xbm");
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << "images/beavis" << QRect(0, 0, 50, 50) << QByteArray("jpeg");
#endif // QTEST_HAVE_JPEG
#ifdef QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << "images/earth" << QRect(0, 0, 50, 50) << QByteArray("gif");
    QTest::newRow("GIF: trolltech") << "images/trolltech" << QRect(0, 0, 50, 50) << QByteArray("gif");
#endif // QTEST_HAVE_GIF
#ifdef QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << "images/ball" << QRect(0, 0, 50, 50) << QByteArray("mng");
    QTest::newRow("MNG: fire") << "images/fire" << QRect(0, 0, 50, 50) << QByteArray("mng");
#endif // QTEST_HAVE_MNG
}

void tst_QImageReader::setScaledClipRect()
{
    QFETCH(QString, fileName);
    QFETCH(QRect, newRect);
    QFETCH(QByteArray, format);

    if (!format.isEmpty() && !QImageReader::supportedImageFormats().contains(format))
        QSKIP("Qt does not support reading the \"" + format + "\" format", SkipSingle);

    QImageReader reader(fileName);
    reader.setScaledSize(QSize(300, 300));
    reader.setScaledClipRect(newRect);
    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(image.rect(), newRect);

    QImageReader originalReader(fileName);
    originalReader.setScaledSize(QSize(300, 300));
    QImage originalImage = originalReader.read();
    QCOMPARE(originalImage.copy(newRect), image);
}

void tst_QImageReader::imageFormat_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");
    QTest::addColumn<QImage::Format>("imageFormat");

    QTest::newRow("pbm") << QString("images/image.pbm") << QByteArray("pbm") << QImage::Format_Mono;
    QTest::newRow("pgm") << QString("images/image.pgm") << QByteArray("pgm") << QImage::Format_Indexed8;
    QTest::newRow("ppm-1") << QString("images/image.ppm") << QByteArray("ppm") << QImage::Format_RGB32;
    QTest::newRow("ppm-2") << QString("images/teapot.ppm") << QByteArray("ppm") << QImage::Format_RGB32;
    QTest::newRow("ppm-3") << QString("images/runners.ppm") << QByteArray("ppm") << QImage::Format_RGB32;
    QTest::newRow("ppm-4") << QString("images/test.ppm") << QByteArray("ppm") << QImage::Format_RGB32;
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("jpeg-1") << QString("images/beavis.jpg") << QByteArray("jpeg") << QImage::Format_Indexed8;
    QTest::newRow("jpeg-2") << QString("images/YCbCr_cmyk.jpg") << QByteArray("jpeg") << QImage::Format_RGB32;
    QTest::newRow("jpeg-3") << QString("images/YCbCr_rgb.jpg") << QByteArray("jpeg") << QImage::Format_RGB32;
#endif
#if defined QTEST_HAVE_GIF
    QTest::newRow("gif-1") << QString("images/earth.gif") << QByteArray("gif") << QImage::Format_Invalid;
    QTest::newRow("gif-2") << QString("images/trolltech.gif") << QByteArray("gif") << QImage::Format_Invalid;
#endif
    QTest::newRow("xbm") << QString("images/gnus.xbm") << QByteArray("xbm") << QImage::Format_MonoLSB;
    QTest::newRow("xpm") << QString("images/marble.xpm") << QByteArray("xpm") << QImage::Format_Indexed8;
    QTest::newRow("bmp-1") << QString("images/colorful.bmp") << QByteArray("bmp") << QImage::Format_Indexed8;
    QTest::newRow("bmp-2") << QString("images/font.bmp") << QByteArray("bmp") << QImage::Format_Indexed8;
    QTest::newRow("png") << QString("images/kollada.png") << QByteArray("png") << QImage::Format_ARGB32;
    QTest::newRow("png-2") << QString("images/YCbCr_cmyk.png") << QByteArray("png") << QImage::Format_RGB32;
    QTest::newRow("mng-1") << QString("images/ball.mng") << QByteArray("mng") << QImage::Format_Invalid;
    QTest::newRow("mng-2") << QString("images/fire.mng") << QByteArray("mng") << QImage::Format_Invalid;
}

void tst_QImageReader::imageFormat()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);
    QFETCH(QImage::Format, imageFormat);
    if (QImageReader::imageFormat(fileName).isEmpty()) {
        if (QByteArray("jpeg") == format)
#ifndef QTEST_HAVE_JPEG
            return;
#endif // !QTEST_HAVE_JPEG
        if (QByteArray("gif") == format)
#ifndef QTEST_HAVE_GIF
            return;
#endif // !QTEST_HAVE_GIF
        if (QByteArray("mng") == format)
#ifndef QTEST_HAVE_MNG
            return;
#endif // !QTEST_HAVE_MNG
        QSKIP(("Qt does not support the " + format + " format.").constData(), SkipSingle);
    } else {
        QCOMPARE(QImageReader::imageFormat(fileName), format);
    }
    QImageReader reader(fileName);
    QCOMPARE(reader.imageFormat(), imageFormat);
}

void tst_QImageReader::blackXPM()
{
    QImage image(QLatin1String("images/black.xpm"));
    QImage image2(QLatin1String("images/black.png"));
    QCOMPARE(image.pixel(25, 25), qRgb(190, 190, 190));
    QCOMPARE(image.pixel(25, 25), image2.pixel(25, 25));
}

void tst_QImageReader::transparentXPM()
{
    QImage image(QLatin1String("images/nontransparent.xpm"));
    QImage image2(QLatin1String("images/transparent.xpm"));
    QCOMPARE(image.format(), QImage::Format_RGB32);
    QCOMPARE(image2.format(), QImage::Format_ARGB32);
}

void tst_QImageReader::multiWordNamedColorXPM()
{
    QImage image(QLatin1String("images/namedcolors.xpm"));
    QCOMPARE(image.pixel(0, 0), qRgb(102, 139, 139)); // pale turquoise 4
    QCOMPARE(image.pixel(0, 1), qRgb(250, 250, 210)); // light golden rod yellow
    QCOMPARE(image.pixel(0, 2), qRgb(255, 250, 205)); // lemon chiffon
}

void tst_QImageReader::supportedFormats()
{
    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    QList<QByteArray> sortedFormats = formats;
    qSort(sortedFormats);

    // check that the list is sorted
    QCOMPARE(formats, sortedFormats);

    QSet<QByteArray> formatSet;
    foreach (QByteArray format, formats)
        formatSet << format;

    // check that the list does not contain duplicates
    QCOMPARE(formatSet.size(), formats.size());
}

void tst_QImageReader::setBackgroundColor_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QColor>("color");

    QTest::newRow("BMP: colorful") << QString("colorful.bmp") << QColor(Qt::white);
    QTest::newRow("BMP: font") << QString("font.bmp") << QColor(Qt::black);
    QTest::newRow("BMP: signed char") << QString("crash-signed-char.bmp") << QColor(Qt::red);
    QTest::newRow("XPM: marble") << QString("marble.xpm") << QColor(Qt::darkRed);
    QTest::newRow("PNG: kollada") << QString("kollada.png") << QColor(Qt::green);
    QTest::newRow("PPM: teapot") << QString("teapot.ppm") << QColor(Qt::darkGreen);
    QTest::newRow("PPM: runners") << QString("runners.ppm") << QColor(Qt::red);
    QTest::newRow("PPM: test") << QString("test.ppm") << QColor(Qt::white);
    QTest::newRow("XBM: gnus") << QString("gnus.xbm") << QColor(Qt::blue);
#if defined QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << QString("beavis.jpg") << QColor(Qt::darkBlue);
#endif
#if defined QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << QString("earth.gif") << QColor(Qt::cyan);
    QTest::newRow("GIF: trolltech") << QString("trolltech.gif") << QColor(Qt::magenta);
#endif
#if defined QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << QString("ball.mng") << QColor(Qt::yellow);
    QTest::newRow("MNG: fire") << QString("fire.mng") << QColor(Qt::gray);
#endif
}

void tst_QImageReader::setBackgroundColor()
{
    QFETCH(QString, fileName);
    QFETCH(QColor, color);
    QImageReader io("images/" + fileName);
    io.setBackgroundColor(color);
    if (io.backgroundColor().isValid())
        QCOMPARE(io.backgroundColor(), color);
}

void tst_QImageReader::supportsAnimation_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("success");

    QTest::newRow("BMP: colorful") << QString("colorful.bmp") << false;
    QTest::newRow("BMP: font") << QString("font.bmp") << false;
    QTest::newRow("BMP: signed char") << QString("crash-signed-char.bmp") << false;
    QTest::newRow("XPM: marble") << QString("marble.xpm") << false;
    QTest::newRow("PNG: kollada") << QString("kollada.png") << false;
    QTest::newRow("PPM: teapot") << QString("teapot.ppm") << false;
    QTest::newRow("PPM: teapot") << QString("teapot.ppm") << false;
    QTest::newRow("PPM: runners") << QString("runners.ppm") << false;
    QTest::newRow("XBM: gnus") << QString("gnus.xbm") << false;
#if defined QTEST_HAVE_JPEG
    QTest::newRow("JPEG: beavis") << QString("beavis.jpg") << false;
#endif
#if defined QTEST_HAVE_GIF
    QTest::newRow("GIF: earth") << QString("earth.gif") << true;
    QTest::newRow("GIF: trolltech") << QString("trolltech.gif") << true;
#endif
#if defined QTEST_HAVE_MNG
    QTest::newRow("MNG: ball") << QString("ball.mng") << true;
    QTest::newRow("MNG: fire") << QString("fire.mng") << true;
#endif
}

void tst_QImageReader::supportsAnimation()
{
    QFETCH(QString, fileName);
    QFETCH(bool, success);
    QImageReader io("images/" + fileName);
    QCOMPARE(io.supportsAnimation(), success);
}

void tst_QImageReader::sizeBeforeRead_data()
{
    imageFormat_data();
}

void tst_QImageReader::sizeBeforeRead()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);
    QImageReader reader(fileName);
    QVERIFY(reader.canRead());
    if (format == "mng") {
        QCOMPARE(reader.size(), QSize());
        return;
    }

    QSize size = reader.size();
    QVERIFY(reader.canRead());
    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(size, image.size());
}

void tst_QImageReader::imageFormatBeforeRead_data()
{
    imageFormat_data();
}

void tst_QImageReader::imageFormatBeforeRead()
{
    QFETCH(QString, fileName);
    QFETCH(QImage::Format, imageFormat);
    QImageReader reader(fileName);
    if (reader.supportsOption(QImageIOHandler::ImageFormat)) {
        QImage::Format fileFormat = reader.imageFormat();
        QCOMPARE(fileFormat, imageFormat);
        QSize size = reader.size();
        QImage image(size, fileFormat);
        QVERIFY(reader.read(&image));
        QCOMPARE(image.format(), fileFormat);
    }
}

#if defined QTEST_HAVE_GIF
void tst_QImageReader::gifHandlerBugs()
{
    {
        QImageReader io("images/trolltech.gif");
        QVERIFY(io.loopCount() != 1);
        int count=0;
        for (; io.canRead(); io.read(), ++count) ;
        QVERIFY(count == 34);
    }

    // Task 95166
    {
        QImageReader io1("images/bat1.gif");
        QImageReader io2("images/bat2.gif");
        QVERIFY(io1.canRead());
        QVERIFY(io2.canRead());
        QImage im1 = io1.read();
        QImage im2 = io2.read();
        QVERIFY(!im1.isNull());
        QVERIFY(!im2.isNull());
        QCOMPARE(im1, im2);
    }

    // Task 9994
    {
        QImageReader io1("images/noclearcode.gif");
        QImageReader io2("images/noclearcode.bmp");
        QVERIFY(io1.canRead());  QVERIFY(io2.canRead());
        QImage im1 = io1.read(); QImage im2 = io2.read();
        QVERIFY(!im1.isNull());  QVERIFY(!im2.isNull());
        QCOMPARE(im1.convertToFormat(QImage::Format_ARGB32), im2.convertToFormat(QImage::Format_ARGB32));
    }
}
#endif

class Server : public QObject
{
    Q_OBJECT
public:
    Server(const QByteArray &data) :serverSocket(0)
    {
        connect(&server, SIGNAL(newConnection()), this, SLOT(acceptNewConnection()));
        server.listen();
        this->data = data;
    }

public slots:
    void runTest()
    {
        connect(&clientSocket, SIGNAL(connected()), this, SLOT(connected()));
        clientSocket.connectToHost(QHostAddress::LocalHost, server.serverPort());
    }

public:
    inline QTcpSocket *socket() const { return serverSocket; }

signals:
    void ready();

private slots:
    void acceptNewConnection()
    {
        serverSocket = server.nextPendingConnection();
        connect(serverSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(remoteHostClosed()));
    }

    void connected()
    {
        clientSocket.write(data);
        clientSocket.disconnectFromHost();
    }

    void remoteHostClosed()
    {
        emit ready();
    }

private:
    QTcpServer server;
    QTcpSocket clientSocket;
    QTcpSocket *serverSocket;
    QByteArray data;
};

void tst_QImageReader::readFromDevice_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("pbm") << QString("images/image.pbm") << QByteArray("pbm");
    QTest::newRow("pgm") << QString("images/image.pgm") << QByteArray("pgm");
    QTest::newRow("ppm-1") << QString("images/image.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-2") << QString("images/teapot.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-3") << QString("images/teapot.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-4") << QString("images/runners.ppm") << QByteArray("ppm");
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("jpeg-1") << QString("images/beavis.jpg") << QByteArray("jpeg");
    QTest::newRow("jpeg-2") << QString("images/YCbCr_cmyk.jpg") << QByteArray("jpeg");
    QTest::newRow("jpeg-3") << QString("images/YCbCr_rgb.jpg") << QByteArray("jpeg");
#endif // QTEST_HAVE_JPEG
#ifdef QTEST_HAVE_GIF
    QTest::newRow("gif-1") << QString("images/earth.gif") << QByteArray("gif");
    QTest::newRow("gif-2") << QString("images/trolltech.gif") << QByteArray("gif");
#endif // QTEST_HAVE_GIF
    QTest::newRow("xbm") << QString("images/gnus.xbm") << QByteArray("xbm");
    QTest::newRow("xpm") << QString("images/marble.xpm") << QByteArray("xpm");
    QTest::newRow("bmp-1") << QString("images/colorful.bmp") << QByteArray("bmp");
    QTest::newRow("bmp-2") << QString("images/font.bmp") << QByteArray("bmp");
    QTest::newRow("png") << QString("images/kollada.png") << QByteArray("png");
#ifdef QTEST_HAVE_MNG
    QTest::newRow("mng-1") << QString("images/ball.mng") << QByteArray("mng");
    QTest::newRow("mng-2") << QString("images/fire.mng") << QByteArray("mng");
#endif // QTEST_HAVE_MNG
}

void tst_QImageReader::readFromDevice()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);

    QImage expectedImage(fileName, format);

    QFile file(fileName);
    QVERIFY(file.open(QFile::ReadOnly));
    QByteArray imageData = file.readAll();
    QVERIFY(!imageData.isEmpty());
    {
        QBuffer buffer;
        buffer.setData(imageData);
        QVERIFY(buffer.open(QBuffer::ReadOnly));

        QImageReader reader(&buffer);
        QVERIFY(reader.canRead());
        QImage imageReaderImage = reader.read();

        QCOMPARE(imageReaderImage, expectedImage);

        buffer.seek(0);

        QImage image1;
        QVERIFY(image1.loadFromData((const uchar *)buffer.data().data(),
                                    buffer.data().size(), format.data()));
        QCOMPARE(image1, expectedImage);

        QByteArray throughBase64 = QByteArray::fromBase64(imageData.toBase64());
        QVERIFY(image1.loadFromData((const uchar *)throughBase64.data(),
                                    throughBase64.size(), format.data()));
        QCOMPARE(image1, expectedImage);
    }

    Server server(imageData);
    QEventLoop loop;
    connect(&server, SIGNAL(ready()), &loop, SLOT(quit()));
    QTimer::singleShot(0, &server, SLOT(runTest()));
    QTimer::singleShot(5000, &loop, SLOT(quit()));
    loop.exec();

    QImageReader reader(server.socket(), format == "xbm" ? "xbm" : "");
    if (format == "xbm")
        QVERIFY(!reader.canRead());
    else
        QVERIFY(reader.canRead());
    QImage imageReaderImage = reader.read();
    QCOMPARE(imageReaderImage, expectedImage);
}

void tst_QImageReader::readFromFileAfterJunk_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("pbm") << QString("images/image.pbm") << QByteArray("pbm");
    QTest::newRow("pgm") << QString("images/image.pgm") << QByteArray("pgm");
    QTest::newRow("ppm-1") << QString("images/image.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-2") << QString("images/teapot.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-3") << QString("images/teapot.ppm") << QByteArray("ppm");
    QTest::newRow("ppm-4") << QString("images/runners.ppm") << QByteArray("ppm");
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("jpeg-1") << QString("images/beavis.jpg") << QByteArray("jpeg");
    QTest::newRow("jpeg-2") << QString("images/YCbCr_cmyk.jpg") << QByteArray("jpeg");
    QTest::newRow("jpeg-3") << QString("images/YCbCr_rgb.jpg") << QByteArray("jpeg");
#endif
#if defined QTEST_HAVE_GIF
//    QTest::newRow("gif-1") << QString("images/earth.gif") << QByteArray("gif");
//    QTest::newRow("gif-2") << QString("images/trolltech.gif") << QByteArray("gif");
#endif
    QTest::newRow("xbm") << QString("images/gnus.xbm") << QByteArray("xbm");
    QTest::newRow("xpm") << QString("images/marble.xpm") << QByteArray("xpm");
    QTest::newRow("bmp-1") << QString("images/colorful.bmp") << QByteArray("bmp");
    QTest::newRow("bmp-2") << QString("images/font.bmp") << QByteArray("bmp");
    QTest::newRow("png") << QString("images/kollada.png") << QByteArray("png");
//    QTest::newRow("mng-1") << QString("images/ball.mng") << QByteArray("mng");
//    QTest::newRow("mng-2") << QString("images/fire.mng") << QByteArray("mng");
}

void tst_QImageReader::readFromFileAfterJunk()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);

    if (!QImageReader::supportedImageFormats().contains(format)) {
        QString cause = QString("Skipping %1; no %2 support").arg(fileName).arg(QString(format));
        QSKIP(qPrintable(cause), SkipSingle);
    }

    QFile::remove("junk");
    QFile junkFile("junk");
    QVERIFY(junkFile.open(QFile::WriteOnly));

    QFile imageFile(fileName);
    QVERIFY(imageFile.open(QFile::ReadOnly));
    QByteArray imageData = imageFile.readAll();
    QVERIFY(!imageData.isNull());

    int iterations = 10;
    if (format == "ppm" || format == "pbm" || format == "pgm")
        iterations = 1;

    if (format == "mng" || !QImageWriter::supportedImageFormats().contains(format)) {
        for (int i = 0; i < iterations; ++i) {
            junkFile.write("deadbeef", 9);
            QCOMPARE(junkFile.write(imageData), qint64(imageData.size()));
        }
    } else {
        for (int i = 0; i < iterations; ++i) {
            QImageWriter writer(&junkFile, format);
            junkFile.write("deadbeef", 9);
            QVERIFY(writer.write(QImage(fileName)));
        }
    }
    junkFile.close();
    junkFile.open(QFile::ReadOnly);

    for (int i = 0; i < iterations; ++i) {
        QByteArray ole = junkFile.read(9);
        junkFile.ungetChar(ole[ole.size() - 1]);
        char c;
        junkFile.getChar(&c);
        QImageReader reader(&junkFile);
        QVERIFY(reader.canRead());
        QVERIFY(!reader.read().isNull());
    }
}

void tst_QImageReader::description_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QStringMap>("description");

    QMap<QString, QString> willem;
    willem["Title"] = "PngSuite";
    willem["Author"] = "Willem A.J. van Schaik (gwillem@ntuvax.ntu.ac.sg)";
    willem["Copyright"] = "Copyright Willem van Schaik, Singapore 1995";
    willem["Description"] = "A compilation of a set of images created to test the "
                            "various color-types of the PNG format. Included are "
                            "black&white, color, paletted, with alpha channel, with "
                            "transparency formats. All bit-depths allowed according "
                            "to the spec are present.";
    willem["Software"] = "Created on a NeXTstation color using \"pnmtopng\".";
    willem["Disclaimer"] = "Freeware.";

    QTest::newRow("PNG") << QString("images/pngwithtext.png") << willem;
    QTest::newRow("PNG Compressed") << QString("images/pngwithcompressedtext.png") << willem;
}

void tst_QImageReader::description()
{
    QFETCH(QString, fileName);
    QFETCH(QStringMap, description);

    // Sanity check
    QVERIFY(!QImage(fileName).isNull());

    QImageReader reader(fileName);

    foreach (QString key, description.keys())
        QCOMPARE(reader.text(key), description.value(key));
    QCOMPARE(reader.textKeys(), QStringList(description.keys()));

    QImage image = reader.read();
    QVERIFY(!image.isNull());

    foreach (QString key, description.keys())
        QCOMPARE(image.text(key), description.value(key));
    QCOMPARE(image.textKeys(), QStringList(description.keys()));

    foreach (QString key, description.keys())
        QCOMPARE(reader.text(key), description.value(key));
    QCOMPARE(reader.textKeys(), QStringList(description.keys()));
}

void tst_QImageReader::readFromResources_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");
    QTest::addColumn<QSize>("size");
    QTest::addColumn<QString>("message");

    QTest::newRow("images/corrupt.bmp") << QString("images/corrupt.bmp")
                                        << QByteArray("bmp") << QSize(0, 0)
                                        << QString("");
    QTest::newRow("images/negativeheight.bmp") << QString("images/negativeheight.bmp")
                                               << QByteArray("bmp") << QSize(127, 64)
                                               << QString("");
    QTest::newRow("images/font.bmp") << QString("images/font.bmp")
                                     << QByteArray("bmp") << QSize(240, 8)
                                     << QString("");
    QTest::newRow("images/noclearcode.bmp") << QString("images/noclearcode.bmp")
                                            << QByteArray("bmp") << QSize(29, 18)
                                            << QString("");
    QTest::newRow("images/colorful.bmp") << QString("images/colorful.bmp")
                                         << QByteArray("bmp") << QSize(320, 200)
                                         << QString("");
    QTest::newRow("images/16bpp.bmp") << QString("images/16bpp.bmp")
                                      << QByteArray("bmp") << QSize(320, 240)
                                      << QString("");
    QTest::newRow("images/crash-signed-char.bmp") << QString("images/crash-signed-char.bmp")
                                                  << QByteArray("bmp") << QSize(360, 280)
                                                  << QString("");
    QTest::newRow("images/4bpp-rle.bmp") << QString("images/4bpp-rle.bmp")
                                         << QByteArray("bmp") << QSize(640, 480)
                                         << QString("");
#ifdef QTEST_HAVE_GIF
    QTest::newRow("images/corrupt.gif") << QString("images/corrupt.gif")
                                        << QByteArray("gif") << QSize(0, 0)
                                        << QString("");
    QTest::newRow("images/trolltech.gif") << QString("images/trolltech.gif")
                                          << QByteArray("gif") << QSize(128, 64)
                                          << QString("");
    QTest::newRow("images/noclearcode.gif") << QString("images/noclearcode.gif")
                                            << QByteArray("gif") << QSize(29, 18)
                                            << QString("");
    QTest::newRow("images/earth.gif") << QString("images/earth.gif")
                                      << QByteArray("gif") << QSize(320, 200)
                                      << QString("");
    QTest::newRow("images/bat1.gif") << QString("images/bat1.gif")
                                     << QByteArray("gif") << QSize(32, 32)
                                     << QString("");
    QTest::newRow("images/bat2.gif") << QString("images/bat2.gif")
                                     << QByteArray("gif") << QSize(32, 32)
                                     << QString("");
#endif
#ifdef QTEST_HAVE_JPEG
    QTest::newRow("images/corrupt.jpg") << QString("images/corrupt.jpg")
                                        << QByteArray("jpg") << QSize(0, 0)
                                        << QString("JPEG datastream contains no image");
    QTest::newRow("images/beavis.jpg") << QString("images/beavis.jpg")
                                       << QByteArray("jpg") << QSize(350, 350)
                                       << QString("");
    QTest::newRow("images/YCbCr_cmyk.jpg") << QString("images/YCbCr_cmyk.jpg")
                                           << QByteArray("jpg") << QSize(75, 50)
                                           << QString("");
    QTest::newRow("images/YCbCr_rgb.jpg") << QString("images/YCbCr_rgb.jpg")
                                          << QByteArray("jpg") << QSize(75, 50)
                                          << QString("");
#endif
#ifdef QTEST_HAVE_MNG
    QTest::newRow("images/corrupt.mng") << QString("images/corrupt.mng")
                                        << QByteArray("mng") << QSize(0, 0)
                                        << QString("MNG error 901: Application signalled I/O error; chunk IHDR; subcode 0:0");
    QTest::newRow("images/fire.mng") << QString("images/fire.mng")
                                     << QByteArray("mng") << QSize(30, 60)
                                     << QString("");
    QTest::newRow("images/ball.mng") << QString("images/ball.mng")
                                     << QByteArray("mng") << QSize(32, 32)
                                     << QString("");
#endif
    QTest::newRow("images/image.pbm") << QString("images/image.pbm")
                                      << QByteArray("pbm") << QSize(16, 6)
                                      << QString("");
    QTest::newRow("images/image.pgm") << QString("images/image.pgm")
                                      << QByteArray("pgm") << QSize(24, 7)
                                      << QString("");
    QTest::newRow("images/corrupt.png") << QString("images/corrupt.png")
                                        << QByteArray("png") << QSize(0, 0)
                                        << QString("");
    QTest::newRow("images/away.png") << QString("images/away.png")
                                     << QByteArray("png") << QSize(16, 16)
                                     << QString("");
    QTest::newRow("images/image.png") << QString("images/image.png")
                                      << QByteArray("png") << QSize(22, 22)
                                      << QString("");
    QTest::newRow("images/pngwithcompressedtext.png") << QString("images/pngwithcompressedtext.png")
                                                      << QByteArray("png") << QSize(32, 32)
                                                      << QString("");
    QTest::newRow("images/pngwithtext.png") << QString("images/pngwithtext.png")
                                            << QByteArray("png") << QSize(32, 32)
                                            << QString("");
    QTest::newRow("images/kollada.png") << QString("images/kollada.png")
                                        << QByteArray("png") << QSize(436, 160)
                                        << QString("");
    QTest::newRow("images/black.png") << QString("images/black.png")
                                      << QByteArray("png") << QSize(48, 48)
                                      << QString("");
    QTest::newRow("images/YCbCr_cmyk.png") << QString("images/YCbCr_cmyk.png")
                                           << QByteArray("png") << QSize(75, 50)
                                           << QString("");
    QTest::newRow("images/teapot.ppm") << QString("images/teapot.ppm")
                                       << QByteArray("ppm") << QSize(256, 256)
                                       << QString("");
    QTest::newRow("images/image.ppm") << QString("images/image.ppm")
                                      << QByteArray("ppm") << QSize(4, 4)
                                      << QString("");
    QTest::newRow("images/runners.ppm") << QString("images/runners.ppm")
                                        << QByteArray("ppm") << QSize(400, 400)
                                        << QString("");
    QTest::newRow("images/test.ppm") << QString("images/test.ppm")
                                     << QByteArray("ppm") << QSize(10, 10)
                                     << QString("");
//    QTest::newRow("images/corrupt.xbm") << QString("images/corrupt.xbm") << QByteArray("xbm") << QSize(0, 0);
    QTest::newRow("images/gnus.xbm") << QString("images/gnus.xbm")
                                     << QByteArray("xbm") << QSize(271, 273)
                                     << QString("");
    QTest::newRow("images/corrupt-colors.xpm") << QString("images/corrupt-colors.xpm")
                                               << QByteArray("xpm") << QSize(0, 0)
                                               << QString("QImage: XPM color specification is missing: bla9an.n#x");
    QTest::newRow("images/corrupt-pixels.xpm") << QString("images/corrupt-pixels.xpm")
                                               << QByteArray("xpm") << QSize(0, 0)
                                               << QString("QImage: XPM pixels missing on image line 3");
    QTest::newRow("images/marble.xpm") << QString("images/marble.xpm")
                                       << QByteArray("xpm") << QSize(240, 240)
                                       << QString("");
    QTest::newRow("images/test.xpm") << QString("images/test.xpm")
                                     << QByteArray("xpm") << QSize(256, 256)
                                     << QString("");
    QTest::newRow("images/black.xpm") << QString("images/black.xpm")
                                      << QByteArray("xpm") << QSize(48, 48)
                                      << QString("");
    QTest::newRow("images/namedcolors.xpm") << QString("images/namedcolors.xpm")
                                            << QByteArray("xpm") << QSize(8, 8)
                                            << QString("");
    QTest::newRow("images/nontransparent.xpm") << QString("images/nontransparent.xpm")
                                               << QByteArray("xpm") << QSize(8, 8)
                                               << QString("");
    QTest::newRow("images/transparent.xpm") << QString("images/transparent.xpm")
                                            << QByteArray("xpm") << QSize(8, 8)
                                            << QString("");
}

void tst_QImageReader::readFromResources()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);
    QFETCH(QSize, size);
    QFETCH(QString, message);

    for (int i = 0; i < 2; ++i) {
        QString file = i ? (":/" + fileName) : fileName;
        {
            // suppress warnings if we expect them
            if (!message.isEmpty()) {
                for (int j = 0; j < 5; ++j)
                    QTest::ignoreMessage(QtWarningMsg, message.toLatin1());
            }

            // 1) full filename, no format
            QImageReader reader(file);
            QImage image = reader.read();
            if (size.isNull())
                QVERIFY(image.isNull());
            else
                QVERIFY(!image.isNull());
            QCOMPARE(image.size(), size);
        }
        {
            // 2) full filename, with format
            QImageReader reader(file, format);
            QImage image = reader.read();
            if (size.isNull())
                QVERIFY(image.isNull());
            else
                QVERIFY(!image.isNull());
            QCOMPARE(image.size(), size);
        }
        {
            // 3) full filename, with uppercase format
            QImageReader reader(file, format.toUpper());
            QImage image = reader.read();
            if (size.isNull())
                QVERIFY(image.isNull());
            else
                QVERIFY(!image.isNull());
            QCOMPARE(image.size(), size);
        }
        {
            // 4) chopped filename, with format
            QImageReader reader(file.left(file.lastIndexOf(QLatin1Char('.'))), format);
            QImage image = reader.read();
            if (size.isNull())
                QVERIFY(image.isNull());
            else
                QVERIFY(!image.isNull());
            QCOMPARE(image.size(), size);
        }
        {
            // 5) chopped filename, with uppercase format
            QImageReader reader(file.left(file.lastIndexOf(QLatin1Char('.'))), format.toUpper());
            QImage image = reader.read();
            if (size.isNull())
                QVERIFY(image.isNull());
            else
                QVERIFY(!image.isNull());
            QCOMPARE(image.size(), size);
        }
    }

    // Check that the results are identical
    if (!message.isEmpty()) {
        QTest::ignoreMessage(QtWarningMsg, message.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, message.toLatin1());
    }
    QCOMPARE(QImageReader(fileName).read(), QImageReader(":/" + fileName).read());
}

void tst_QImageReader::readCorruptImage_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("shouldFail");
    QTest::addColumn<QString>("message");
#if defined QTEST_HAVE_JPEG
    QTest::newRow("corrupt jpeg") << QString("images/corrupt.jpg") << true
                                  << QString("JPEG datastream contains no image");
#endif
#if defined QTEST_HAVE_GIF
    QTest::newRow("corrupt gif") << QString("images/corrupt.gif") << true << QString("");
#endif
#ifdef QTEST_HAVE_MNG
    QTest::newRow("corrupt mng") << QString("images/corrupt.mng") << true
                                 << QString("MNG error 901: Application signalled I/O error; chunk IHDR; subcode 0:0");
#endif
    QTest::newRow("corrupt png") << QString("images/corrupt.png") << true << QString("");
    QTest::newRow("corrupt bmp") << QString("images/corrupt.bmp") << true << QString("");
    QTest::newRow("corrupt xpm (colors)") << QString("images/corrupt-colors.xpm") << true
                                          << QString("QImage: XPM color specification is missing: bla9an.n#x");
    QTest::newRow("corrupt xpm (pixels)") << QString("images/corrupt-pixels.xpm") << true
                                          << QString("QImage: XPM pixels missing on image line 3");
    QTest::newRow("corrupt xbm") << QString("images/corrupt.xbm") << false << QString("");
#if defined QTEST_HAVE_TIFF
    QTest::newRow("corrupt tiff") << QString("images/corrupt-data.tif") << true << QString("");
#endif
}

void tst_QImageReader::readCorruptImage()
{
    QFETCH(QString, fileName);
    QFETCH(bool, shouldFail);
    QFETCH(QString, message);
    if (!message.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, message.toLatin1());
    QImageReader reader(fileName);
    QVERIFY(reader.canRead());
    QCOMPARE(reader.read().isNull(), shouldFail);
}

void tst_QImageReader::readCorruptBmp()
{
    QCOMPARE(QImage("images/tst7.bmp").convertToFormat(QImage::Format_ARGB32_Premultiplied), QImage("images/tst7.png").convertToFormat(QImage::Format_ARGB32_Premultiplied));
}

void tst_QImageReader::supportsOption_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QIntList>("options");

    QTest::newRow("png") << QString("images/black.png")
                         << (QIntList() << QImageIOHandler::Gamma
                              << QImageIOHandler::Description
                              << QImageIOHandler::Quality
                              << QImageIOHandler::Size);
}

void tst_QImageReader::supportsOption()
{
    QFETCH(QString, fileName);
    QFETCH(QIntList, options);

    QSet<QImageIOHandler::ImageOption> allOptions;
    allOptions << QImageIOHandler::Size
               << QImageIOHandler::ClipRect
               << QImageIOHandler::Description
               << QImageIOHandler::ScaledClipRect
               << QImageIOHandler::ScaledSize
               << QImageIOHandler::CompressionRatio
               << QImageIOHandler::Gamma
               << QImageIOHandler::Quality
               << QImageIOHandler::Name
               << QImageIOHandler::SubType
               << QImageIOHandler::IncrementalReading
               << QImageIOHandler::Endianness
               << QImageIOHandler::Animation
               << QImageIOHandler::BackgroundColor;

    QImageReader reader(fileName);
    for (int i = 0; i < options.size(); ++i) {
        QVERIFY(reader.supportsOption(QImageIOHandler::ImageOption(options.at(i))));
        allOptions.remove(QImageIOHandler::ImageOption(options.at(i)));
    }

    foreach (QImageIOHandler::ImageOption option, allOptions)
        QVERIFY(!reader.supportsOption(option));
}

#if defined QTEST_HAVE_TIFF
void tst_QImageReader::tiffCompression_data()
{
    QTest::addColumn<QString>("uncompressedFile");
    QTest::addColumn<QString>("compressedFile");

    QTest::newRow("TIFF: adobedeflate") << "images/rgba_nocompression_littleendian.tif"
                                        << "images/rgba_adobedeflate_littleendian.tif";
    QTest::newRow("TIFF: lzw") << "images/rgba_nocompression_littleendian.tif"
                               << "images/rgba_lzw_littleendian.tif";
    QTest::newRow("TIFF: packbits") << "images/rgba_nocompression_littleendian.tif"
                                    << "images/rgba_packbits_littleendian.tif";
    QTest::newRow("TIFF: zipdeflate") << "images/rgba_nocompression_littleendian.tif"
                                      << "images/rgba_zipdeflate_littleendian.tif";
}

void tst_QImageReader::tiffCompression()
{
    QFETCH(QString, uncompressedFile);
    QFETCH(QString, compressedFile);

    QImage uncompressedImage(uncompressedFile);
    QImage compressedImage(compressedFile);

    QCOMPARE(uncompressedImage, compressedImage);
}

void tst_QImageReader::tiffEndianness()
{
    QImage littleEndian("images/rgba_nocompression_littleendian.tif");
    QImage bigEndian("images/rgba_nocompression_bigendian.tif");

    QCOMPARE(littleEndian, bigEndian);
}

#endif

void tst_QImageReader::dotsPerMeter_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("expectedDotsPerMeterX");
    QTest::addColumn<int>("expectedDotsPerMeterY");

    QTest::newRow("TIFF: 72 dpi") << "images/rgba_nocompression_littleendian.tif" << qRound(72 * (100 / 2.54)) << qRound(72 * (100 / 2.54));
}

void tst_QImageReader::dotsPerMeter()
{
    QFETCH(QString, fileName);
    QFETCH(int, expectedDotsPerMeterX);
    QFETCH(int, expectedDotsPerMeterY);

    QImage image(fileName);

    QCOMPARE(image.dotsPerMeterX(), expectedDotsPerMeterX);
    QCOMPARE(image.dotsPerMeterY(), expectedDotsPerMeterY);
}

void tst_QImageReader::physicalDpi_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("expectedPhysicalDpiX");
    QTest::addColumn<int>("expectedPhysicalDpiY");

    QTest::newRow("TIFF: 72 dpi") << "images/rgba_nocompression_littleendian.tif" << 72 << 72;
}

void tst_QImageReader::physicalDpi()
{
    QFETCH(QString, fileName);
    QFETCH(int, expectedPhysicalDpiX);
    QFETCH(int, expectedPhysicalDpiY);

    QImage image(fileName);

    QCOMPARE(image.physicalDpiX(), expectedPhysicalDpiX);
    QCOMPARE(image.physicalDpiY(), expectedPhysicalDpiY);
}

void tst_QImageReader::autoDetectImageFormat()
{
    // Assume PNG is supported :-)
    {
        // Disables file name extension probing
        QImageReader reader("images/kollada");
        reader.setAutoDetectImageFormat(false);
        QVERIFY(!reader.canRead());
        QVERIFY(reader.read().isNull());
        reader.setAutoDetectImageFormat(true);
        QVERIFY(reader.canRead());
        QVERIFY(!reader.read().isNull());
    }
    {
        // Disables detection based on suffix
        QImageReader reader("images/kollada.png");
        reader.setAutoDetectImageFormat(false);
        QVERIFY(!reader.canRead());
        QVERIFY(reader.read().isNull());
        reader.setAutoDetectImageFormat(true);
        QVERIFY(reader.canRead());
        QVERIFY(!reader.read().isNull());
    }
    {
        // Disables detection based on content
        QImageReader reader("images/kollada-noext");
        reader.setAutoDetectImageFormat(false);
        QVERIFY(!reader.canRead());
        QVERIFY(reader.read().isNull());
        reader.setAutoDetectImageFormat(true);
        QVERIFY(reader.canRead());
        QVERIFY(!reader.read().isNull());
    }
}

void tst_QImageReader::fileNameProbing()
{
    QString name("doesnotexist.png");
    QImageReader r;
    r.setFileName(name); // non-existing / non-readable file
    QCOMPARE(r.fileName(), name);

    r.size();
    QCOMPARE(r.fileName(), name);
    r.read();
    QCOMPARE(r.fileName(), name);
}

QTEST_MAIN(tst_QImageReader)
#include "tst_qimagereader.moc"
