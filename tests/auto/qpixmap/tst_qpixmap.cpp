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
#include <qpixmap.h>
#include <qbitmap.h>
#include <qimage.h>
#include <qmatrix.h>
#include <qdesktopwidget.h>
#include <qpaintengine.h>

#include <QSet>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#ifdef Q_WS_QWS
#include <qscreen_qws.h>
#endif

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(QImage::Format)

class tst_QPixmap : public QObject
{
    Q_OBJECT

public:
    tst_QPixmap();
    virtual ~tst_QPixmap();


public slots:
    void init();
    void cleanup();

private slots:
    void setAlphaChannel_data();
    void setAlphaChannel();

    void fromImage_data();
    void fromImage();

    void convertFromImage_data();
    void convertFromImage();

    void testMetrics();

    void fill_data();
    void fill();
    void fill_transparent();

    void createMaskFromColor();

    void mask();
    void bitmapMask();
    void setGetMask_data();
    void setGetMask();
    void cacheKey();
    void drawBitmap();
    void grabWidget();
    void grabWindow();
    void isNull();
    void task_246446();

#ifdef Q_WS_QWS
    void convertFromImageNoDetach();
#endif

    void convertFromImageDetach();

#if defined(Q_WS_WIN)
    void toWinHBITMAP_data();
    void toWinHBITMAP();
    void fromWinHBITMAP_data();
    void fromWinHBITMAP();
#endif

    void onlyNullPixmapsOutsideGuiThread();
    void refUnref();

    void copy();
    void depthOfNullObjects();

#ifdef QT3_SUPPORT
    void resize();
    void resizePreserveMask();
#endif

    void transformed();
    void transformed2();

    void fromImage_crash();

    void fromData();
};

Q_DECLARE_METATYPE(QImage)
Q_DECLARE_METATYPE(QPixmap)
Q_DECLARE_METATYPE(QMatrix)
Q_DECLARE_METATYPE(QBitmap)

tst_QPixmap::tst_QPixmap()
{
}

tst_QPixmap::~tst_QPixmap()
{
}

void tst_QPixmap::init()
{
}

void tst_QPixmap::cleanup()
{
}

void tst_QPixmap::setAlphaChannel_data()
{
    QTest::addColumn<int>("red");
    QTest::addColumn<int>("green");
    QTest::addColumn<int>("blue");
    QTest::addColumn<int>("alpha");

    QTest::newRow("red 0") << 255 << 0 << 0 << 0;
    QTest::newRow("red 24") << 255 << 0 << 0 << 24;
    QTest::newRow("red 124") << 255 << 0 << 0 << 124;
    QTest::newRow("red 255") << 255 << 0 << 0 << 255;

    QTest::newRow("green 0") << 0 << 255 << 0 << 0;
    QTest::newRow("green 24") << 0 << 255 << 0 << 24;
    QTest::newRow("green 124") << 0 << 255 << 0 << 124;
    QTest::newRow("green 255") << 0 << 255 << 0 << 255;

    QTest::newRow("blue 0") << 0 << 0 << 255 << 0;
    QTest::newRow("blue 24") << 0 << 0 << 255 << 24;
    QTest::newRow("blue 124") << 0 << 0 << 255 << 124;
    QTest::newRow("blue 255") << 0 << 0 << 255 << 255;
}

void tst_QPixmap::setAlphaChannel()
{
    QFETCH(int, red);
    QFETCH(int, green);
    QFETCH(int, blue);
    QFETCH(int, alpha);

    int width = 100;
    int height = 100;

    QPixmap pixmap(width, height);
    pixmap.fill(QColor(red, green, blue));

    QPixmap alphaChannel(width, height);
    alphaChannel.fill(QColor(alpha, alpha, alpha));
    pixmap.setAlphaChannel(alphaChannel);

#ifdef Q_WS_X11
    if (!pixmap.x11PictureHandle())
        QSKIP("Requires XRender support", SkipAll);
#endif

    QImage result;
    bool ok = true;

    QPixmap outAlpha = pixmap.alphaChannel();
    QCOMPARE(outAlpha.size(), pixmap.size());

    result = outAlpha.toImage().convertToFormat(QImage::Format_ARGB32);;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            ok &= qGray(result.pixel(x, y)) == alpha;
        }
    }
    QVERIFY(ok);

    result = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    QRgb expected = alpha == 0 ? 0 : qRgba(red, green, blue, alpha);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (result.numColors() > 0) {
                ok &= result.pixelIndex(x, y) == expected;
            } else {
                ok &= result.pixel(x, y) == expected;
            }
        }
    }
    QVERIFY(ok);
}

void tst_QPixmap::fromImage_data()
{
    QTest::addColumn<QImage::Format>("format");

    QTest::newRow("Format_Mono") << QImage::Format_Mono;
    QTest::newRow("Format_MonoLSB") << QImage::Format_MonoLSB;
//    QTest::newRow("Format_Indexed8") << QImage::Format_Indexed8;
    QTest::newRow("Format_RGB32") << QImage::Format_RGB32;
    QTest::newRow("Format_ARGB32") << QImage::Format_ARGB32;
    QTest::newRow("Format_ARGB32_Premultiplied") << QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("Format_RGB16") << QImage::Format_RGB16;
}

void tst_QPixmap::fromImage()
{
    QFETCH(QImage::Format, format);

    QImage image(37, 16, format);

    if (image.numColors() == 2) {
        image.setColor(0, QColor(Qt::color0).rgba());
        image.setColor(1, QColor(Qt::color1).rgba());
    }
    image.fill(0x7f7f7f7f);

    const QPixmap pixmap = QPixmap::fromImage(image);
#ifdef Q_WS_X11
    if (!pixmap.x11PictureHandle())
        QSKIP("Requires XRender support", SkipAll);
#endif
    const QImage result = pixmap.toImage();
    image = image.convertToFormat(result.format());
    QCOMPARE(result, image);
}

void tst_QPixmap::convertFromImage_data()
{
    QTest::addColumn<QImage>("img1");
    QTest::addColumn<QImage>("img2");

    const QString prefix = QLatin1String(SRCDIR) + "/convertFromImage";
    {
        QImage img1;
        QImage img2;
        QVERIFY(img1.load(prefix + "/task31722_0/img1.png"));
        QVERIFY(img2.load(prefix + "/task31722_0/img2.png"));
        QVERIFY(img1.load(prefix + "/task31722_0/img1.png"));
        QVERIFY(img2.load(prefix + "/task31722_0/img2.png"));
        QTest::newRow("Task 31722 0") << img1 << img2;
    }
    {
        QImage img1;
        QImage img2;
        QVERIFY(img1.load(prefix + "/task31722_1/img1.png"));
        QVERIFY(img2.load(prefix + "/task31722_1/img2.png"));
        QTest::newRow("Task 31722 1") << img1 << img2;
    }
}

void tst_QPixmap::convertFromImage()
{
    QFETCH(QImage, img1);
    QFETCH(QImage, img2);

    QPixmap pix = QPixmap::fromImage(img1);
    pix = QPixmap::fromImage(img2);

    QPixmap res = QPixmap::fromImage(img2);
    QVERIFY( pixmapsAreEqual(&pix, &res) );
}

void tst_QPixmap::fill_data()
{
    QTest::addColumn<uint>("pixel");
    QTest::addColumn<bool>("syscolor");
    QTest::addColumn<bool>("bitmap");
    for (int color = Qt::black; color < Qt::darkYellow; ++color)
        QTest::newRow(QString("syscolor_%1").arg(color).toLatin1())
            << uint(color) << true << false;

#ifdef Q_WS_QWS
    if (QScreen::instance()->depth() >= 24) {
#elif defined (Q_WS_X11)
    QPixmap pm(1, 1);
    if (pm.x11PictureHandle()) {
#elif defined (Q_OS_WINCE)
    QPixmap pixmap(1,1);
    if (QPixmap::grabWidget(QApplication::desktop()).depth() >= 24) {
#else
    QPixmap pixmap(1, 1); {
#endif
        QTest::newRow("alpha_7f_red")   << 0x7fff0000u << false << false;
        QTest::newRow("alpha_3f_blue")  << 0x3f0000ffu << false << false;
        QTest::newRow("alpha_b7_green") << 0xbf00ff00u << false << false;
        QTest::newRow("alpha_7f_white") << 0x7fffffffu << false << false;
        QTest::newRow("alpha_3f_white") << 0x3fffffffu << false << false;
        QTest::newRow("alpha_b7_white") << 0xb7ffffffu << false << false;
        QTest::newRow("alpha_7f_black") << 0x7f000000u << false << false;
        QTest::newRow("alpha_3f_black") << 0x3f000000u << false << false;
        QTest::newRow("alpha_b7_black") << 0xbf000000u << false << false;
    }

    QTest::newRow("bitmap_color0") << uint(Qt::color0) << true << true;
    QTest::newRow("bitmap_color1") << uint(Qt::color1) << true << true;
}

void tst_QPixmap::fill()
{
    QFETCH(uint, pixel);
    QFETCH(bool, syscolor);
    QFETCH(bool, bitmap);

    QColor color;

    if (syscolor)
        color = QColor(Qt::GlobalColor(pixel));
    else
        color = QColor(qRed(pixel), qGreen(pixel), qBlue(pixel), qAlpha(pixel));

    QColor compareColor = color;
    if (bitmap && syscolor) {
        // special case color0 and color1 for bitmaps.
        if (pixel == Qt::color0)
            compareColor.setRgb(255, 255, 255);
        else
            compareColor.setRgb(0, 0, 0);
    }

    QPixmap pm;

    if (bitmap)
        pm = QBitmap(400, 400);
    else
        pm = QPixmap(400, 400);

#if defined(Q_WS_X11)
    if (!bitmap && !pm.x11PictureHandle())
        QSKIP("Requires XRender support", SkipSingle);
#endif

    pm.fill(color);
    if (syscolor && !bitmap && pm.depth() < 24) {
        QSKIP("Test does not work on displays without true color", SkipSingle);
    }

    QImage image = pm.toImage();
    if (bitmap && syscolor) {
        int pixelindex = (pixel == Qt::color0) ? 0 : 1;
        QVERIFY(image.pixelIndex(0,0) == pixelindex);
    }
    QImage::Format format = compareColor.alpha() != 255
                            ? QImage::Format_ARGB32
                            : QImage::Format_RGB32;
    image = image.convertToFormat(format);


    QImage shouldBe(400, 400, format);
    shouldBe.fill(compareColor.rgba());

    QCOMPARE(image, shouldBe);
}

void tst_QPixmap::fill_transparent()
{
    QPixmap pixmap(10, 10);
#ifdef Q_WS_X11
    if (!pixmap.x11PictureHandle())
        QSKIP("Requires XRender support", SkipAll);
#endif
    pixmap.fill(Qt::transparent);
    QVERIFY(pixmap.hasAlphaChannel());
}

void tst_QPixmap::mask()
{
    QPixmap pm(100, 100);
    QBitmap bm(100, 100);

    pm.fill();
    bm.fill();

    QVERIFY(!pm.isNull());
    QVERIFY(!bm.isNull());
    // hw: todo: this will fail if the default pixmap format is
    // argb32_premultiplied. The mask will be all 1's
    QVERIFY(pm.mask().isNull());

    QImage img = bm.toImage();
    QVERIFY(img.format() == QImage::Format_MonoLSB
            || img.format() == QImage::Format_Mono);

    pm.setMask(bm);
    QVERIFY(!pm.mask().isNull());

    bm = QBitmap();
    // Invalid format here, since isNull() == true
    QVERIFY(bm.toImage().isNull());
    QCOMPARE(bm.toImage().format(), QImage::Format_Invalid);
    pm.setMask(bm);
    QVERIFY(pm.mask().isNull());

    bm = QBitmap(100, 100);
    bm.fill();
    pm.setMask(bm);
    QVERIFY(!pm.mask().isNull());
}

void tst_QPixmap::bitmapMask()
{
    QImage image(3, 3, QImage::Format_Mono);
    image.setColor(0, Qt::color0);
    image.setColor(1, Qt::color1);
    image.fill(Qt::color0);
    image.setPixel(1, 1, Qt::color1);
    image.setPixel(0, 0, Qt::color1);

    QImage image_mask(3, 3, QImage::Format_Mono);
    image_mask.setColor(0, Qt::color0);
    image_mask.setColor(1, Qt::color1);
    image_mask.fill(Qt::color0);
    image_mask.setPixel(1, 1, Qt::color1);
    image_mask.setPixel(2, 0, Qt::color1);

    QBitmap pm = QBitmap::fromImage(image);
    QBitmap pm_mask = QBitmap::fromImage(image_mask);
    pm.setMask(pm_mask);

    image = pm.toImage();
    image.setColor(0, Qt::color0);
    image.setColor(1, Qt::color1);
    image_mask = pm_mask.toImage();
    image_mask.setColor(0, Qt::color0);
    image_mask.setColor(1, Qt::color1);

    QVERIFY(!image.pixel(0, 0));
    QVERIFY(!image.pixel(2, 0));
    QVERIFY(image.pixel(1, 1));
}

void tst_QPixmap::setGetMask_data()
{
    QTest::addColumn<QPixmap>("pixmap");
    QTest::addColumn<QBitmap>("mask");
    QTest::addColumn<QBitmap>("expected");

    QPixmap pixmap(10, 10);
    QBitmap mask(10, 10);
    QPainter p;

    p.begin(&pixmap);
    p.fillRect(0, 0, 10, 10, QColor(Qt::black));
    p.end();

    QTest::newRow("nullmask 0") << QPixmap() << QBitmap() << QBitmap();
    QTest::newRow("nullmask 1") << pixmap << QBitmap() << QBitmap();
    mask.clear();
    QTest::newRow("nullmask 2") << pixmap << mask << mask;
    QTest::newRow("nullmask 3") << QPixmap(QBitmap()) << QBitmap() << QBitmap();

    p.begin(&mask);
    p.fillRect(1, 1, 5, 5, QColor(Qt::color1));
    p.end();
    QTest::newRow("simple mask 0") << pixmap << mask << mask;
}

void tst_QPixmap::setGetMask()
{
    QFETCH(QPixmap, pixmap);
    QFETCH(QBitmap, mask);
    QFETCH(QBitmap, expected);

    pixmap.setMask(mask);
    QBitmap result = pixmap.mask();

    QImage resultImage = result.toImage();
    QImage expectedImage = expected.toImage();
    QCOMPARE(resultImage.convertToFormat(expectedImage.format()),
             expectedImage);
}

void tst_QPixmap::testMetrics()
{
    QPixmap pixmap(100, 100);

    QCOMPARE(pixmap.width(), 100);
    QCOMPARE(pixmap.height(), 100);
    QCOMPARE(pixmap.depth(), QPixmap::defaultDepth());

    QBitmap bitmap(100, 100);

    QCOMPARE(bitmap.width(), 100);
    QCOMPARE(bitmap.height(), 100);
    QCOMPARE(bitmap.depth(), 1);
}

void tst_QPixmap::createMaskFromColor()
{
    QImage image(3, 3, QImage::Format_Indexed8);
    image.setNumColors(10);
    image.setColor(0, 0xffffffff);
    image.setColor(1, 0xff000000);
    image.setColor(2, 0xffff0000);
    image.setColor(3, 0xff0000ff);
    image.fill(0);
    image.setPixel(1, 0, 1);
    image.setPixel(0, 1, 2);
    image.setPixel(1, 1, 3);

    QImage im_mask = image.createMaskFromColor(0xffff0000);
    QCOMPARE((uint) im_mask.pixel(0, 1), QColor(Qt::color0).rgba());
    QCOMPARE((uint) im_mask.pixel(0, 1), QColor(Qt::color0).rgba());

    QPixmap pixmap = QPixmap::fromImage(image);
    QBitmap mask = pixmap.createMaskFromColor(Qt::red);
    QBitmap inv_mask = pixmap.createMaskFromColor(Qt::red, Qt::MaskOutColor);
    QCOMPARE((uint) mask.toImage().pixel(0, 1), QColor(Qt::color0).rgba());
    QCOMPARE((uint) inv_mask.toImage().pixel(0, 1), QColor(Qt::color1).rgba());
}


void tst_QPixmap::cacheKey()
{
    QPixmap pixmap1(1, 1);
    QPixmap pixmap2(1, 1);
    qint64 pixmap1_key = pixmap1.cacheKey();

    QVERIFY(pixmap1.cacheKey() != pixmap2.cacheKey());

    pixmap2 = pixmap1;
    QVERIFY(pixmap2.cacheKey() == pixmap1.cacheKey());

    pixmap2.detach();
    QVERIFY(pixmap2.cacheKey() != pixmap1.cacheKey());
    QVERIFY(pixmap1.cacheKey() == pixmap1_key);
}

// Test drawing a bitmap on a pixmap.
void tst_QPixmap::drawBitmap()
{
    QBitmap bitmap(10,10);
    bitmap.fill(Qt::color1);

    QPixmap pixmap(10,10);
    QPainter painter2(&pixmap);
    painter2.fillRect(0,0,10,10, QBrush(Qt::green));
    painter2.setPen(Qt::red);
    painter2.drawPixmap(0,0,10,10, bitmap);
    painter2.end();
    QCOMPARE(pixmap.toImage().pixel(5,5), QColor(Qt::red).rgb());
}

void tst_QPixmap::grabWidget()
{
    QWidget widget;
    widget.setPalette(Qt::green);
    widget.resize(128, 128);

    QPixmap expected(64, 64);
    expected.fill(Qt::green);

    QPixmap actual = QPixmap::grabWidget(&widget, QRect(64, 64, 64, 64));

    QCOMPARE(actual, expected);
}

void tst_QPixmap::grabWindow()
{
#ifdef Q_OS_WINCE
    // We get out of memory, if the desktop itself is too big.
    if (QApplication::desktop()->width() <= 480)
#endif
    QVERIFY(QPixmap::grabWindow(QApplication::desktop()->winId()).isNull() == false);
    QWidget w;
    w.resize(640, 480);
    w.show();
    QTest::qWait(100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&w);
#endif
    QVERIFY(QPixmap::grabWindow(w.winId()).isNull() == false);

    QWidget child(&w);
    child.setGeometry(50, 50, 100, 100);
    child.setPalette(Qt::red);
    child.setAutoFillBackground(true);
    child.show();
    QTest::qWait(100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&child);
#endif

    QPixmap grabWindowPixmap = QPixmap::grabWindow(child.winId());
    QPixmap grabWidgetPixmap = QPixmap::grabWidget(&child);
    QCOMPARE(grabWindowPixmap, grabWidgetPixmap);
}

void tst_QPixmap::isNull()
{
    {
        QPixmap pixmap(1,1);
        QVERIFY(pixmap.isNull() == false);
    }
    {
        QPixmap pixmap(0,0);
        QVERIFY(pixmap.isNull());
    }

    {
        QPixmap pixmap(0,1);
        QVERIFY(pixmap.isNull());
    }
    {
        QPixmap pixmap(1,0);
        QVERIFY(pixmap.isNull());
    }
    {
        QPixmap pixmap(-1,-1);
        QVERIFY(pixmap.isNull());
    }
    {
        QPixmap pixmap(-1,5);
        QVERIFY(pixmap.isNull());
    }
}

#ifdef Q_WS_QWS
void tst_QPixmap::convertFromImageNoDetach()
{
    //first get the screen format
    QPixmap randomPixmap(10,10);
    QImage::Format screenFormat = randomPixmap.toImage().format();
    QVERIFY(screenFormat != QImage::Format_Invalid);

    QImage orig(100,100, screenFormat);

    QPixmap pix = QPixmap::fromImage(orig);
    QImage copy = pix.toImage();

    QVERIFY(copy.format() == screenFormat);

    const QImage constOrig = orig;
    const QImage constCopy = copy;
    QVERIFY(constOrig.bits() == constCopy.bits());
}
#endif //Q_WS_QWS

void tst_QPixmap::convertFromImageDetach()
{
    QImage img(10,10, QImage::Format_RGB32);
    img.fill(0);
    QVERIFY(!img.isNull());
    QPixmap p = QPixmap::fromImage(img);
    QVERIFY(p.isDetached());
    QPixmap copy = p;
    QVERIFY(!copy.isDetached());
    QVERIFY(!p.isDetached());
    img.fill(1);
    p = QPixmap::fromImage(img);
    QVERIFY(copy.isDetached());
}

#if defined(Q_WS_WIN)
void tst_QPixmap::toWinHBITMAP_data()
{
    QTest::addColumn<int>("red");
    QTest::addColumn<int>("green");
    QTest::addColumn<int>("blue");

    QTest::newRow("red")   << 255 << 0 << 0;
    QTest::newRow("green") << 0 << 255 << 0;
    QTest::newRow("blue")  << 0 << 0 << 255;
}

void tst_QPixmap::toWinHBITMAP()
{
    QFETCH(int, red);
    QFETCH(int, green);
    QFETCH(int, blue);

    QPixmap pm(100, 100);
    pm.fill(QColor(red, green, blue));

    HBITMAP bitmap = pm.toWinHBITMAP();

    QVERIFY(bitmap != 0);

    // Verify size
    BITMAP bitmap_info;
    memset(&bitmap_info, 0, sizeof(BITMAP));

    int res;
    QT_WA({
        res = GetObjectW(bitmap, sizeof(BITMAP), &bitmap_info);
    } , {
        res = GetObjectA(bitmap, sizeof(BITMAP), &bitmap_info);
    });
    QVERIFY(res);

    QCOMPARE(100, (int) bitmap_info.bmWidth);
    QCOMPARE(100, (int) bitmap_info.bmHeight);

    HDC display_dc = GetDC(0);
    HDC bitmap_dc = CreateCompatibleDC(display_dc);

    HBITMAP null_bitmap = (HBITMAP) SelectObject(bitmap_dc, bitmap);

    COLORREF pixel = GetPixel(bitmap_dc, 0, 0);
    QCOMPARE((int)GetRValue(pixel), red);
    QCOMPARE((int)GetGValue(pixel), green);
    QCOMPARE((int)GetBValue(pixel), blue);

    // Clean up
    SelectObject(bitmap_dc, null_bitmap);
    DeleteObject(bitmap);
    DeleteDC(bitmap_dc);
    ReleaseDC(0, display_dc);

}

void tst_QPixmap::fromWinHBITMAP_data()
{
    toWinHBITMAP_data();
}

void tst_QPixmap::fromWinHBITMAP()
{
    QFETCH(int, red);
    QFETCH(int, green);
    QFETCH(int, blue);

    HDC display_dc = GetDC(0);
    HDC bitmap_dc = CreateCompatibleDC(display_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(display_dc, 100, 100);
    HBITMAP null_bitmap = (HBITMAP) SelectObject(bitmap_dc, bitmap);

    SelectObject(bitmap_dc, GetStockObject(NULL_PEN));
    HGDIOBJ old_brush = SelectObject(bitmap_dc, CreateSolidBrush(RGB(red, green, blue)));
    Rectangle(bitmap_dc, 0, 0, 100, 100);

#ifdef Q_OS_WINCE //the device context has to be deleted before ::fromWinHBITMAP()
    DeleteDC(bitmap_dc);
#endif
    QPixmap pixmap = QPixmap::fromWinHBITMAP(bitmap);
    QCOMPARE(pixmap.width(), 100);
    QCOMPARE(pixmap.height(), 100);

    QImage image = pixmap.toImage();
    QRgb pixel = image.pixel(0, 0);
    QCOMPARE(qRed(pixel), red);
    QCOMPARE(qGreen(pixel), green);
    QCOMPARE(qBlue(pixel), blue);

    DeleteObject(SelectObject(bitmap_dc, old_brush));
    DeleteObject(SelectObject(bitmap_dc, bitmap));
#ifndef Q_OS_WINCE
    DeleteDC(bitmap_dc);
#endif
    ReleaseDC(0, display_dc);
}

#endif

void tst_QPixmap::onlyNullPixmapsOutsideGuiThread()
{
#if !defined(Q_WS_WIN)
    class Thread : public QThread
    {
    public:
        void run()
        {
            QTest::ignoreMessage(QtWarningMsg,
                                 "QPixmap: It is not safe to use pixmaps outside the GUI thread");
            QPixmap pixmap;
            QVERIFY(pixmap.isNull());

            QTest::ignoreMessage(QtWarningMsg,
                                 "QPixmap: It is not safe to use pixmaps outside the GUI thread");
            QPixmap pixmap1(100, 100);
            QVERIFY(pixmap1.isNull());

            QTest::ignoreMessage(QtWarningMsg,
                                 "QPixmap: It is not safe to use pixmaps outside the GUI thread");
            QPixmap pixmap2(pixmap1);
            QVERIFY(pixmap2.isNull());
        }
    };
    Thread thread;
    thread.start();
    thread.wait();
#endif
}

void tst_QPixmap::refUnref()
{
    // Simple ref/unref
    {
        QPixmap p;
    }
    {
        QBitmap b;
    }

    // Get a copy of a pixmap that goes out of scope
    {
        QPixmap b;
        {
            QPixmap a(10, 10);
            a.fill(Qt::color0);
            b = a;
        }
    }
    {
        QBitmap mask;
        {
            QBitmap bitmap(10, 10);
            bitmap.fill(Qt::color1);
            mask = bitmap.mask();
        }
        mask.fill(Qt::color0);
    }

}

void tst_QPixmap::copy()
{
    QPixmap src(32, 32);
    {
        QPainter p(&src);
        p.fillRect(0, 0, 32, 32, Qt::red);
        p.fillRect(10, 10, 10, 10, Qt::blue);
    }

    QPixmap dest = src.copy(10, 10, 10, 10);
    QImage result = dest.toImage().convertToFormat(QImage::Format_RGB32);
    QImage expected(10, 10, QImage::Format_RGB32);
    expected.fill(0xff0000ff);
    QCOMPARE(result, expected);
}

#ifdef QT3_SUPPORT
void tst_QPixmap::resize()
{
    QPixmap p1(10, 10);
    p1.fill(Qt::red);

    QPixmap p2 = p1;
    QPixmap p3(50, 50);
    p3.fill(Qt::red);

    p1.resize(p3.size());
    p1.resize(p2.size());
    p3.resize(p2.size());
    QCOMPARE(p1.toImage(), p2.toImage());
    QCOMPARE(p1.toImage(), p3.toImage());

    QBitmap b1;
    b1.resize(10, 10);
    QVERIFY(b1.depth() == 1);
    QPixmap p4;
    p4.resize(10, 10);
    QVERIFY(p4.depth() != 0);
}

void tst_QPixmap::resizePreserveMask()
{
    QPixmap pm(100, 100);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.fillRect(10, 10, 80, 80, Qt::red);
    p.drawRect(0, 0, 99, 99);
    p.end();

    QBitmap mask = pm.mask();
    pm.resize(50, 50);

    QCOMPARE(pm.mask().toImage(), mask.toImage().copy(0, 0, 50, 50));

    pm = QPixmap(100, 100);
    pm.fill(Qt::red);
    pm.setMask(mask);
    pm.resize(50, 50);

    QCOMPARE(pm.mask().toImage(), mask.toImage().copy(0, 0, 50, 50));
}
#endif

void tst_QPixmap::depthOfNullObjects()
{
    QBitmap b1;
    QVERIFY(b1.depth() == 0);
    QPixmap p4;
    QVERIFY(p4.depth() == 0);
}

void tst_QPixmap::transformed()
{
    QPixmap p1(20, 10);
    p1.fill(Qt::red);
    {
        QPainter p(&p1);
        p.drawRect(0, 0, p1.width() - 1, p1.height() - 1);
    }

    QPixmap p2(10, 20);
    p2.fill(Qt::red);
    {
        QPainter p(&p2);
        p.drawRect(0, 0, p2.width() - 1, p2.height() - 1);
    }

    QPixmap p1_90 = p1.transformed(QTransform().rotate(90));
    QPixmap p1_180 = p1.transformed(QTransform().rotate(180));
    QPixmap p1_270 = p1.transformed(QTransform().rotate(270));

    QCOMPARE(p1_90.size(), p2.size());
    QCOMPARE(p1_90.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied),
             p2.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));

    QCOMPARE(p1_180.size(), p1.size());
    QCOMPARE(p1_180.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied),
             p1.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));

    QCOMPARE(p1_270.size(), p2.size());
    QCOMPARE(p1_270.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied),
             p2.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));
}

void tst_QPixmap::transformed2()
{
    QPixmap pm(3, 3);
    pm.fill(Qt::red);
    QPainter p(&pm);
    p.fillRect(0, 0, 3, 3, QBrush(Qt::Dense4Pattern));
    p.end();

    QTransform transform;
    transform.rotate(-90);
    transform.scale(3, 3);

    QPixmap actual = pm.transformed(transform);

    QPixmap expected(9, 9);
    expected.fill(Qt::red);
    p.begin(&expected);
    p.setBrush(Qt::black);
    p.setPen(Qt::NoPen);
    p.drawRect(3, 0, 3, 3);
    p.drawRect(0, 3, 3, 3);
    p.drawRect(6, 3, 3, 3);
    p.drawRect(3, 6, 3, 3);
    p.end();

    QCOMPARE(actual.size(), expected.size());
    QCOMPARE(actual.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied),
             expected.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied));
}

void tst_QPixmap::fromImage_crash()
{
    QImage *img = new QImage(64, 64, QImage::Format_ARGB32_Premultiplied);

    QPixmap pm = QPixmap::fromImage(*img);
    QPainter painter(&pm);

    delete img;
}

void tst_QPixmap::fromData()
{
    unsigned char bits[] = { 0xaa, 0x55 };

    QBitmap bm = QBitmap::fromData(QSize(8, 2), bits);
    QImage img = bm.toImage();

    QSet<QRgb> colors;
    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x)
            colors << img.pixel(x, y);

    QCOMPARE(colors.size(), 2);

    QCOMPARE(img.pixel(0, 0), QRgb(0xffffffff));
    QCOMPARE(img.pixel(0, 1), QRgb(0xff000000));
}

void tst_QPixmap::task_246446()
{
    // This crashed without the bugfix in 246446
    QPixmap pm(10, 10);
    pm.fill(Qt::transparent); // force 32-bit depth
    QBitmap bm;
    pm.setMask(bm);
    {
        QPixmap pm2(pm);
    }
    QVERIFY(pm.width() == 10);
    QVERIFY(pm.mask().isNull());
}


QTEST_MAIN(tst_QPixmap)
#include "tst_qpixmap.moc"
