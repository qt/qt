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

#include <qapplication.h>
#include <qdebug.h>
#include <qsvgrenderer.h>
#include <qsvggenerator.h>
#include <QPainter>
#include <QPen>
#include <QPicture>
#include <QXmlStreamReader>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QSvgRenderer : public QObject
{
Q_OBJECT

public:
    tst_QSvgRenderer();
    virtual ~tst_QSvgRenderer();

private slots:
    void getSetCheck();
    void inexistentUrl();
    void emptyUrl();
    void testStrokeWidth();
    void testMapViewBoxToTarget();
    void testRenderElement();
    void constructorQXmlStreamReader() const;
    void loadQXmlStreamReader() const;
    void nestedQXmlStreamReader() const;
    void stylePropagation() const;
    void matrixForElement() const;
    void gradientStops() const;
    void fillRule();

#ifndef QT_NO_COMPRESS
    void testGzLoading();
    void testGzHelper_data();
    void testGzHelper();
#endif

private:
    static const char *const src;
};

const char *const tst_QSvgRenderer::src = "<svg><g><rect x='250' y='250' width='500' height='500'/>"
                                          "<rect id='foo' x='400' y='400' width='100' height='100'/></g></svg>";

tst_QSvgRenderer::tst_QSvgRenderer()
{
}

tst_QSvgRenderer::~tst_QSvgRenderer()
{
}

// Testing get/set functions
void tst_QSvgRenderer::getSetCheck()
{
    QSvgRenderer obj1;
    // int QSvgRenderer::framesPerSecond()
    // void QSvgRenderer::setFramesPerSecond(int)
    obj1.setFramesPerSecond(20);
    QCOMPARE(20, obj1.framesPerSecond());
    obj1.setFramesPerSecond(0);
    QCOMPARE(0, obj1.framesPerSecond());
    obj1.setFramesPerSecond(INT_MIN);
    QCOMPARE(0, obj1.framesPerSecond()); // Can't have a negative framerate
    obj1.setFramesPerSecond(INT_MAX);
    QCOMPARE(INT_MAX, obj1.framesPerSecond());
}

void tst_QSvgRenderer::inexistentUrl()
{
    const char *src = "<svg><g><path d=\"\" style=\"stroke:url(#inexistent)\"/></g></svg>";

    QByteArray data(src);
    QSvgRenderer renderer(data);

    QVERIFY(renderer.isValid());
}

void tst_QSvgRenderer::emptyUrl()
{
    const char *src = "<svg><text fill=\"url()\" /></svg>";

    QByteArray data(src);
    QSvgRenderer renderer(data);

    QVERIFY(renderer.isValid());
}

void tst_QSvgRenderer::testStrokeWidth()
{
    qreal squareSize = 30.0;
    qreal strokeWidth = 1.0;
    qreal topLeft = 100.0;

    QSvgGenerator generator;

    QBuffer buffer;
    QByteArray byteArray;
    buffer.setBuffer(&byteArray);
    generator.setOutputDevice(&buffer);

    QPainter painter(&generator);
    painter.setBrush(Qt::blue);

    // Draw a rect with stroke
    painter.setPen(QPen(Qt::black, strokeWidth));
    painter.drawRect(topLeft, topLeft, squareSize, squareSize);

    // Draw a rect without stroke
    painter.setPen(Qt::NoPen);
    painter.drawRect(topLeft, topLeft, squareSize, squareSize);
    painter.end();

    // Insert ID tags into the document
    byteArray.insert(byteArray.indexOf("stroke=\"#000000\""), "id=\"SquareStroke\" ");
    byteArray.insert(byteArray.indexOf("stroke=\"none\""), "id=\"SquareNoStroke\" ");

    QSvgRenderer renderer(byteArray);

    QRectF noStrokeRect = renderer.boundsOnElement("SquareNoStroke");
    QCOMPARE(noStrokeRect.width(), squareSize);
    QCOMPARE(noStrokeRect.height(), squareSize);
    QCOMPARE(noStrokeRect.x(), topLeft);
    QCOMPARE(noStrokeRect.y(), topLeft);

    QRectF strokeRect = renderer.boundsOnElement("SquareStroke");
    QCOMPARE(strokeRect.width(), squareSize + strokeWidth);
    QCOMPARE(strokeRect.height(), squareSize + strokeWidth);
    QCOMPARE(strokeRect.x(), topLeft - (strokeWidth / 2));
    QCOMPARE(strokeRect.y(), topLeft - (strokeWidth / 2));
}

void tst_QSvgRenderer::testMapViewBoxToTarget()
{
    const char *src = "<svg><g><rect x=\"250\" y=\"250\" width=\"500\" height=\"500\" /></g></svg>";
    QByteArray data(src);

    { // No viewport, viewBox, targetRect, or deviceRect -> boundingRect
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter);
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(0, 0, 500, 500));
    }

    { // No viewport, viewBox, targetRect -> deviceRect
        QPicture picture;
        picture.setBoundingRect(QRect(100, 100, 200, 200));
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter);
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(100, 100, 200, 200));
    }

    { // No viewport, viewBox -> targetRect
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QRectF(50, 50, 250, 250));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(50, 50, 250, 250));

    }

    data.replace("<svg>", "<svg viewBox=\"0 0 1000 1000\">");

    { // No viewport, no targetRect -> viewBox
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter);
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(250, 250, 500, 500));
    }

    data.replace("<svg", "<svg width=\"500\" height=\"500\"");

    { // Viewport
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter);
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(125, 125, 250, 250));
    }

}

void tst_QSvgRenderer::testRenderElement()
{
    QByteArray data(src);

    { // No viewport, viewBox, targetRect, or deviceRect -> boundingRect
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QLatin1String("foo"));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));
    }

    { // No viewport, viewBox, targetRect -> deviceRect
        QPicture picture;
        picture.setBoundingRect(QRect(100, 100, 200, 200));
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QLatin1String("foo"));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(100, 100, 200, 200));
    }

    { // No viewport, viewBox -> targetRect
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QLatin1String("foo"), QRectF(50, 50, 250, 250));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(50, 50, 250, 250));

    }

    data.replace("<svg>", "<svg viewBox=\"0 0 1000 1000\">");

    { // No viewport, no targetRect -> view box size
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QLatin1String("foo"));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));
    }

    data.replace("<svg", "<svg width=\"500\" height=\"500\"");

    { // Viewport
        QPicture picture;
        QPainter painter(&picture);
        QSvgRenderer rend(data);
        rend.render(&painter, QLatin1String("foo"));
        painter.end();
        QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));
    }

}

void tst_QSvgRenderer::constructorQXmlStreamReader() const
{
    const QByteArray data(src);

    QXmlStreamReader reader(data);

    QPicture picture;
    QPainter painter(&picture);
    QSvgRenderer rend(&reader);
    rend.render(&painter, QLatin1String("foo"));
    painter.end();
    QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));
}

void tst_QSvgRenderer::loadQXmlStreamReader() const
{
    const QByteArray data(src);

    QXmlStreamReader reader(data);
    QPicture picture;
    QPainter painter(&picture);
    QSvgRenderer rend;
    rend.load(&reader);
    rend.render(&painter, QLatin1String("foo"));
    painter.end();
    QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));
}


void tst_QSvgRenderer::nestedQXmlStreamReader() const
{
    const QByteArray data(QByteArray("<bar>") + QByteArray(src) + QByteArray("</bar>"));

    QXmlStreamReader reader(data);

    QCOMPARE(reader.readNext(), QXmlStreamReader::StartDocument);
    QCOMPARE(reader.readNext(), QXmlStreamReader::StartElement);
    QCOMPARE(reader.name().toString(), QLatin1String("bar"));
    
    QPicture picture;
    QPainter painter(&picture);
    QSvgRenderer renderer(&reader);
    renderer.render(&painter, QLatin1String("foo"));
    painter.end();
    QCOMPARE(picture.boundingRect(), QRect(0, 0, 100, 100));

    QCOMPARE(reader.readNext(), QXmlStreamReader::EndElement);
    QCOMPARE(reader.name().toString(), QLatin1String("bar"));
    QCOMPARE(reader.readNext(), QXmlStreamReader::EndDocument);

    QVERIFY(reader.atEnd());
    QVERIFY(!reader.hasError());
}

void tst_QSvgRenderer::stylePropagation() const
{
    QByteArray data("<svg>"
                      "<g id='foo' style='fill:#ffff00;'>"
                        "<g id='bar' style='fill:#ff00ff;'>"
                          "<g id='baz' style='fill:#00ffff;'>"
                            "<rect id='alpha' x='0' y='0' width='100' height='100'/>"
                          "</g>"
                          "<rect id='beta' x='100' y='0' width='100' height='100'/>"
                        "</g>"
                        "<rect id='gamma' x='0' y='100' width='100' height='100'/>"
                      "</g>"
                      "<rect id='delta' x='100' y='100' width='100' height='100'/>"
                    "</svg>"); // alpha=cyan, beta=magenta, gamma=yellow, delta=black

    QImage image1(200, 200, QImage::Format_RGB32);
    QImage image2(200, 200, QImage::Format_RGB32);
    QImage image3(200, 200, QImage::Format_RGB32);
    QPainter painter;
    QSvgRenderer renderer(data);
    QLatin1String parts[4] = {QLatin1String("alpha"), QLatin1String("beta"), QLatin1String("gamma"), QLatin1String("delta")};

    QVERIFY(painter.begin(&image1));
    for (int i = 0; i < 4; ++i)
        renderer.render(&painter, parts[i], QRectF(renderer.boundsOnElement(parts[i])));
    painter.end();

    QVERIFY(painter.begin(&image2));
    renderer.render(&painter, renderer.viewBoxF());
    painter.end();

    QVERIFY(painter.begin(&image3));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(Qt::cyan));
    painter.drawRect(0, 0, 100, 100);
    painter.setBrush(QBrush(Qt::magenta));
    painter.drawRect(100, 0, 100, 100);
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawRect(0, 100, 100, 100);
    painter.setBrush(QBrush(Qt::black));
    painter.drawRect(100, 100, 100, 100);
    painter.end();

    QCOMPARE(image1, image2);
    QCOMPARE(image1, image3);
}

static qreal transformNorm(const QTransform &m)
{
    return qSqrt(m.m11() * m.m11()
        + m.m12() * m.m12()
        + m.m13() * m.m13()
        + m.m21() * m.m21()
        + m.m22() * m.m22()
        + m.m23() * m.m23()
        + m.m31() * m.m31()
        + m.m32() * m.m32()
        + m.m33() * m.m33());
}

static bool diffIsSmallEnough(double diff, double norm)
{
    return diff <= 1e-12 * norm;
}

static bool diffIsSmallEnough(float diff, float norm)
{
    return diff <= 1e-5 * norm;
}

static void compareTransforms(const QTransform &m1, const QTransform &m2)
{
    qreal norm1 = transformNorm(m1);
    qreal norm2 = transformNorm(m2);
    qreal diffNorm = transformNorm(QTransform(m1.m11() - m2.m11(),
                                              m1.m12() - m2.m12(),
                                              m1.m13() - m2.m13(),
                                              m1.m21() - m2.m21(),
                                              m1.m22() - m2.m22(),
                                              m1.m23() - m2.m23(),
                                              m1.m31() - m2.m31(),
                                              m1.m32() - m2.m32(),
                                              m1.m33() - m2.m33()));
    QVERIFY(diffIsSmallEnough(diffNorm, qMin(norm1, norm2)));
}

void tst_QSvgRenderer::matrixForElement() const
{
    QByteArray data("<svg>"
                      "<g id='ichi' transform='translate(-3,1)'>"
                        "<g id='ni' transform='rotate(45)'>"
                          "<g id='san' transform='scale(4,2)'>"
                            "<g id='yon' transform='matrix(1,2,3,4,5,6)'>"
                              "<rect id='firkant' x='-1' y='-1' width='2' height='2'/>"
                            "</g>"
                          "</g>"
                        "</g>"
                      "</g>"
                    "</svg>");

    QImage image(13, 37, QImage::Format_RGB32);
    QPainter painter(&image);
    QSvgRenderer renderer(data);

    compareTransforms(QTransform(painter.worldMatrix()), QTransform(renderer.matrixForElement(QLatin1String("ichi"))));
    painter.translate(-3, 1);
    compareTransforms(QTransform(painter.worldMatrix()), QTransform(renderer.matrixForElement(QLatin1String("ni"))));
    painter.rotate(45);
    compareTransforms(QTransform(painter.worldMatrix()), QTransform(renderer.matrixForElement(QLatin1String("san"))));
    painter.scale(4, 2);
    compareTransforms(QTransform(painter.worldMatrix()), QTransform(renderer.matrixForElement(QLatin1String("yon"))));
    painter.setWorldMatrix(QMatrix(1, 2, 3, 4, 5, 6), true);
    compareTransforms(QTransform(painter.worldMatrix()), QTransform(renderer.matrixForElement(QLatin1String("firkant"))));
}

void tst_QSvgRenderer::gradientStops() const
{
    {
        QByteArray data("<svg>"
                          "<defs>"
                            "<linearGradient id=\"gradient\">"
                            "</linearGradient>"
                          "</defs>"
                          "<rect fill=\"url(#gradient)\" height=\"64\" width=\"64\" x=\"0\" y=\"0\"/>"
                        "</svg>");
        QSvgRenderer renderer(data);

        QImage image(64, 64, QImage::Format_ARGB32_Premultiplied), refImage(64, 64, QImage::Format_ARGB32_Premultiplied);
        image.fill(0x87654321);
        refImage.fill(0x87654321);

        QPainter painter(&image);
        renderer.render(&painter);
        QCOMPARE(image, refImage);
    }

    {
        QByteArray data("<svg>"
                          "<defs>"
                            "<linearGradient id=\"gradient\">"
                              "<stop offset=\"1\" stop-color=\"cyan\"/>"
                            "</linearGradient>"
                          "</defs>"
                          "<rect fill=\"url(#gradient)\" height=\"64\" width=\"64\" x=\"0\" y=\"0\"/>"
                        "</svg>");
        QSvgRenderer renderer(data);

        QImage image(64, 64, QImage::Format_ARGB32_Premultiplied), refImage(64, 64, QImage::Format_ARGB32_Premultiplied);
        refImage.fill(0xff00ffff);

        QPainter painter(&image);
        renderer.render(&painter);
        QCOMPARE(image, refImage);
    }

    {
        QByteArray data("<svg>"
                          "<defs>"
                            "<linearGradient id=\"gradient\">"
                              "<stop offset=\"0\" stop-color=\"red\"/>"
                              "<stop offset=\"0\" stop-color=\"cyan\"/>"
                              "<stop offset=\"0.5\" stop-color=\"cyan\"/>"
                              "<stop offset=\"0.5\" stop-color=\"magenta\"/>"
                              "<stop offset=\"0.5\" stop-color=\"yellow\"/>"
                              "<stop offset=\"1\" stop-color=\"yellow\"/>"
                              "<stop offset=\"1\" stop-color=\"blue\"/>"
                            "</linearGradient>"
                          "</defs>"
                          "<rect fill=\"url(#gradient)\" height=\"64\" width=\"64\" x=\"0\" y=\"0\"/>"
                        "</svg>");
        QSvgRenderer renderer(data);

        QImage image(64, 64, QImage::Format_ARGB32_Premultiplied), refImage(64, 64, QImage::Format_ARGB32_Premultiplied);

        QPainter painter;
        painter.begin(&refImage);
        painter.fillRect(QRectF(0, 0, 32, 64), Qt::cyan);
        painter.fillRect(QRectF(32, 0, 32, 64), Qt::yellow);
        painter.end();

        painter.begin(&image);
        renderer.render(&painter);
        painter.end();

        QCOMPARE(image, refImage);
    }

    const char *svgs[] = {
        "<svg>"
            "<defs>"
                "<linearGradient id=\"gradient\">"
                    "<stop offset=\"0\" stop-color=\"red\" stop-opacity=\"0\"/>"
                    "<stop offset=\"1\" stop-color=\"blue\"/>"
                "</linearGradient>"
            "</defs>"
            "<rect fill=\"url(#gradient)\" height=\"8\" width=\"256\" x=\"0\" y=\"0\"/>"
        "</svg>",
        "<svg>"
            "<defs>"
                "<linearGradient id=\"gradient\" xlink:href=\"#gradient0\">"
                "</linearGradient>"
                "<linearGradient id=\"gradient0\">"
                    "<stop offset=\"0\" stop-color=\"red\" stop-opacity=\"0\"/>"
                    "<stop offset=\"1\" stop-color=\"blue\"/>"
                "</linearGradient>"
            "</defs>"
            "<rect fill=\"url(#gradient)\" height=\"8\" width=\"256\" x=\"0\" y=\"0\"/>"
        "</svg>",
        "<svg>"
            "<defs>"
                "<linearGradient id=\"gradient0\">"
                    "<stop offset=\"0\" stop-color=\"red\" stop-opacity=\"0\"/>"
                    "<stop offset=\"1\" stop-color=\"blue\"/>"
                "</linearGradient>"
                "<linearGradient id=\"gradient\" xlink:href=\"#gradient0\">"
                "</linearGradient>"
            "</defs>"
            "<rect fill=\"url(#gradient)\" height=\"8\" width=\"256\" x=\"0\" y=\"0\"/>"
        "</svg>"
    };
    for (int i = 0 ; i < sizeof(svgs) / sizeof(svgs[0]) ; ++i)
    {
        QByteArray data = svgs[i];
        QSvgRenderer renderer(data);

        QImage image(256, 8, QImage::Format_ARGB32_Premultiplied);
        image.fill(0);

        QPainter painter(&image);
        renderer.render(&painter);

        const QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(3));
        QRgb left = line[0]; // transparent black
        QRgb mid = line[127]; // semi transparent magenta
        QRgb right = line[255]; // opaque blue

        QVERIFY((qAlpha(left) < 3) && (qRed(left) < 3) && (qGreen(left) == 0) && (qBlue(left) < 3));
        QVERIFY((qAbs(qAlpha(mid) - 127) < 3) && (qAbs(qRed(mid) - 63) < 4) && (qGreen(mid) == 0) && (qAbs(qBlue(mid) - 63) < 4));
        QVERIFY((qAlpha(right) > 253) && (qRed(right) < 3) && (qGreen(right) == 0) && (qBlue(right) > 251));
    }
}

#ifndef QT_NO_COMPRESS
void tst_QSvgRenderer::testGzLoading()
{
    QSvgRenderer renderer(QLatin1String("heart.svgz"));
    QVERIFY(renderer.isValid());

    QSvgRenderer resourceRenderer(QLatin1String(":/heart.svgz"));
    QVERIFY(resourceRenderer.isValid());

    QFile largeFileGz("large.svgz");
    largeFileGz.open(QIODevice::ReadOnly);
    QByteArray data = largeFileGz.readAll();
    QSvgRenderer autoDetectGzData(data);
    QVERIFY(autoDetectGzData.isValid());
}

QT_BEGIN_NAMESPACE
QByteArray qt_inflateGZipDataFrom(QIODevice *device);
QT_END_NAMESPACE

void tst_QSvgRenderer::testGzHelper_data()
{
    QTest::addColumn<QByteArray>("in");
    QTest::addColumn<QByteArray>("out");

    QTest::newRow("empty") << QByteArray() << QByteArray();
    QTest::newRow("small") << QByteArray::fromHex(QByteArray("1f8b08005819934800034b"
            "cbcfe70200a865327e04000000")) << QByteArray("foo\n");

    QFile largeFileGz("large.svgz");
    largeFileGz.open(QIODevice::ReadOnly);
    QFile largeFile("large.svg");
    largeFile.open(QIODevice::ReadOnly);
    QTest::newRow("large") << largeFileGz.readAll() << largeFile.readAll();

    QTest::newRow("zeroes") << QByteArray::fromHex(QByteArray("1f8b0800131f9348000333"
            "301805a360148c54c00500d266708601040000")) << QByteArray(1024, '0').append('\n');

    QTest::newRow("twoMembers") << QByteArray::fromHex(QByteArray("1f8b08001c2a934800034b"
            "cbcfe70200a865327e040000001f8b08001c2a934800034b4a2ce20200e9b3a20404000000"))
        << QByteArray("foo\nbar\n");

    // We should still get data of the first member if subsequent members are corrupt
    QTest::newRow("corruptedSecondMember") << QByteArray::fromHex(QByteArray("1f8b08001c2a934800034b"
            "cbcfe70200a865327e040000001f8c08001c2a934800034b4a2ce20200e9b3a20404000000"))
        << QByteArray("foo\n");

}

void tst_QSvgRenderer::testGzHelper()
{
    QFETCH(QByteArray, in);
    QFETCH(QByteArray, out);

    QBuffer buffer(&in);
    buffer.open(QIODevice::ReadOnly);
    QVERIFY(buffer.isReadable());
    QByteArray result = qt_inflateGZipDataFrom(&buffer);
    QCOMPARE(result, out);
}
#endif

void tst_QSvgRenderer::fillRule()
{
    QByteArray data(
        "<svg>"
        "<rect x=\"0\" y=\"0\" height=\"300\" width=\"400\" fill=\"blue\" />"
        "<path d=\"M100 200 L300 200 L300 100 L100 100 M100 200 L300 200 L300 100 L100 100 Z\" fill=\"red\" stroke=\"black\" />"
        "</svg>");

    QSvgRenderer renderer(data);

    QImage image(128, 128, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    QPainter painter(&image);
    renderer.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(64, 64), QColor(Qt::red).rgba());
}

QTEST_MAIN(tst_QSvgRenderer)
#include "tst_qsvgrenderer.moc"
