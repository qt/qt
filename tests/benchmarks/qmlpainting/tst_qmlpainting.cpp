/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qtest.h>
#include <QPainter>
#include <QImage>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class tst_QmlPainting : public QObject
{
    Q_OBJECT

    public:
    tst_QmlPainting() {}

private slots:
    void drawRoundedRect();
    void drawScaledRoundedRect();
    void drawTransformedRoundedRect();
    void drawAntialiasedRoundedRect();
    void drawScaledAntialiasedRoundedRect_data();
    void drawScaledAntialiasedRoundedRect();
    void drawTransformedAntialiasedRoundedRect_data();
    void drawTransformedAntialiasedRoundedRect();
    void drawImageRoundedRect();
    void drawScaledImageRoundedRect_data();
    void drawScaledImageRoundedRect();
    void drawTransformedImageRoundedRect_data();
    void drawTransformedImageRoundedRect();
    void drawScaleGridRoundedRect();
    void drawScaledScaleGridRoundedRect_data();
    void drawScaledScaleGridRoundedRect();
    void drawTransformedScaleGridRoundedRect_data();
    void drawTransformedScaleGridRoundedRect();
    void drawTransformedTransparentImage_data();
    void drawTransformedTransparentImage();
    void drawTransformedSemiTransparentImage_data();
    void drawTransformedSemiTransparentImage();
    void drawTransformedFilledImage_data();
    void drawTransformedFilledImage();
};

const qreal inv_dist_to_plane = 1. / 1024.;
QTransform transformForAngle(qreal angle)
{
    QTransform transform;

    QTransform rotTrans;
    rotTrans.translate(-40, 0);
    QTransform rotTrans2;
    rotTrans2.translate(40, 0);

    qreal rad = angle * 2. * M_PI / 360.;
    qreal c = ::cos(rad);
    qreal s = ::sin(rad);

    qreal x = 0;
    qreal y = 80;
    qreal z = 0;

    qreal len = x * x + y * y + z * z;
    if (len != 1.) {
        len = ::sqrt(len);
        x /= len;
        y /= len;
        z /= len;
    }

    QTransform rot(x*x*(1-c)+c, x*y*(1-c)-z*s, x*z*(1-c)+y*s*inv_dist_to_plane,
                   y*x*(1-c)+z*s, y*y*(1-c)+c, y*z*(1-c)-x*s*inv_dist_to_plane,
                   0, 0, 1);

    transform *= rotTrans;
    transform *= rot;
    transform *= rotTrans2;

    return transform;
}

void tst_QmlPainting::drawRoundedRect()
{
    QImage surface(100, 100, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);

    QBENCHMARK {
        p.drawRoundedRect(QRectF(.5, .5, 80, 80), 10, 10);
    }
    surface.save("regular.png");
}

void tst_QmlPainting::drawScaledRoundedRect()
{
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);
    p.scale(3, 3);

    QBENCHMARK {
        p.drawRoundedRect(10, 10, 80, 80, 10, 10);
    }
    surface.save("scaled.png");
}

void tst_QmlPainting::drawTransformedRoundedRect()
{
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);

    QBENCHMARK {
        p.setWorldTransform(QTransform(0.956957, 0, 0.000704124, 0, 1, 0, 16.141, 0, 0.735953));
        p.drawRoundedRect(100, 100, 80, 80, 10, 10);
    }
    surface.save("transformed.png");
}

void tst_QmlPainting::drawAntialiasedRoundedRect()
{
    QImage surface(100, 100, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);

    QBENCHMARK {
        p.drawRoundedRect(QRectF(.5, .5, 80, 80), 10, 10);
    }
    surface.save("aar.png");
}

void tst_QmlPainting::drawScaledAntialiasedRoundedRect_data()
{
    QTest::addColumn<float>("scale");

    for (float i = 0; i < 3; i += .1)
        QTest::newRow(QString(QLatin1String("scale=%1")).arg(i).toLatin1()) << i;
}

void tst_QmlPainting::drawScaledAntialiasedRoundedRect()
{
    QFETCH(float, scale);

    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);
    p.scale(scale, scale);

    QBENCHMARK {
        p.drawRoundedRect(10, 10, 80, 80, 10, 10);
    }
    surface.save("aas.png");
}

void tst_QmlPainting::drawTransformedAntialiasedRoundedRect_data()
{
    QTest::addColumn<QTransform>("transform");

    for (float angle = 0; angle < 360; angle += 10)
        QTest::newRow(QString(QLatin1String("angle=%1")).arg(angle).toLatin1()) << transformForAngle(angle);
}

void tst_QmlPainting::drawTransformedAntialiasedRoundedRect()
{
    QFETCH(QTransform, transform);

    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 1));
    p.setBrush(Qt::red);

    QBENCHMARK {
        p.setWorldTransform(transform);
        p.drawRoundedRect(100, 100, 80, 80, 10, 10);
    }
    surface.save("aat.png");
}

void tst_QmlPainting::drawImageRoundedRect()
{
    //setup image
    const int radius = 10;
    QImage rectImage(81, 81, QImage::Format_ARGB32_Premultiplied);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    rp.drawRoundedRect(QRectF(.5, .5, 80, 80), radius, radius);

    //setup surface
    QImage surface(100, 100, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.drawImage(0,0, rectImage);
    }
    surface.save("ri.png");
}

void tst_QmlPainting::drawScaledImageRoundedRect_data()
{
    QTest::addColumn<int>("imageType");

    QTest::newRow("imagetype=ARGB32_Pre") << (int)QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("imagetype=ARGB8565_Pre") << (int)QImage::Format_ARGB8565_Premultiplied;
}

void tst_QmlPainting::drawScaledImageRoundedRect()
{
    QFETCH(int, imageType);

    //setup image
    const int radius = 10;
    QImage rectImage(81, 81, (QImage::Format)imageType);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    rp.drawRoundedRect(QRectF(.5, .5, 80, 80), radius, radius);

    //setup surface
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);
    p.scale(3, 3);

    QBENCHMARK {
        p.drawImage(0,0, rectImage);
    }
    surface.save("si.png");
}

void tst_QmlPainting::drawTransformedImageRoundedRect_data()
{
    QTest::addColumn<int>("imageType");

    QTest::newRow("imagetype=ARGB32_Pre") << (int)QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("imagetype=ARGB8565_Pre") << (int)QImage::Format_ARGB8565_Premultiplied;
}

void tst_QmlPainting::drawTransformedImageRoundedRect()
{
    QFETCH(int, imageType);

    //setup image
    const int radius = 10;
    QImage rectImage(81, 81, (QImage::Format)imageType);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    rp.drawRoundedRect(QRectF(.5, .5, 80, 80), radius, radius);

    //setup surface
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.setWorldTransform(QTransform(0.956957, 0, 0.000704124, 0, 1, 0, 16.141, 0, 0.735953));
        p.drawImage(100,100, rectImage);
    }
    surface.save("ti.png");
}

//code from QmlGraphicsRect for drawing rounded rects
void tst_QmlPainting::drawScaleGridRoundedRect()
{
    //setup image
    const int pw = 1;
    const int radius = 10;
    QImage rectImage(radius*2 + 3 + pw*2, radius*2 + 3 + pw*2, QImage::Format_ARGB32_Premultiplied);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    if (pw%2)
        rp.drawRoundedRect(QRectF(qreal(pw)/2+1, qreal(pw)/2+1, rectImage.width()-(pw+1), rectImage.height()-(pw+1)), radius, radius);
    else
        rp.drawRoundedRect(QRectF(qreal(pw)/2, qreal(pw)/2, rectImage.width()-pw, rectImage.height()-pw), radius, radius);

    //setup surface
    QImage surface(100, 100, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        const int pw = 2;
        int offset = int(radius+1.5+pw);
        int width = 80;
        int height = 80;

        int w = width+pw;
        int h = height+pw;
        int xOffset = offset;
        int xSide = xOffset * 2;
        bool xMiddles=true;
        if (xSide > w) {
            xMiddles=false;
            xOffset = w/2 + 1;
            xSide = xOffset * 2;
        }
        int yOffset = offset;
        int ySide = yOffset * 2;
        bool yMiddles=true;
        if (ySide > h) {
            yMiddles = false;
            yOffset = h/2 + 1;
            ySide = yOffset * 2;
        }

        // Upper left
        p.drawImage(QRect(-pw/2, -pw/2, xOffset, yOffset), rectImage, QRect(0, 0, xOffset, yOffset));

        // Upper middle
        if (xMiddles)
            p.drawImage(QRect(xOffset-pw/2, -pw/2, width - xSide + pw, yOffset), rectImage,
                                  QRect(rectImage.width()/2, 0, 1, yOffset));
        // Upper right
        p.drawImage(QPoint(width-xOffset+pw/2, -pw/2), rectImage,
                              QRect(rectImage.width()-xOffset, 0, xOffset, yOffset));
        // Middle left
        if (yMiddles)
            p.drawImage(QRect(-pw/2, yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                  QRect(0, rectImage.height()/2, xOffset, 1));

        // Middle
        if (xMiddles && yMiddles)
            p.drawImage(QRect(xOffset-pw/2, yOffset-pw/2, width - xSide + pw, height - ySide + pw), rectImage,
                                QRect(rectImage.width()/2, rectImage.height()/2, 1, 1));
        // Middle right
        if (yMiddles)
            p.drawImage(QRect(width-xOffset+pw/2, yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                QRect(rectImage.width()-xOffset, rectImage.height()/2, xOffset, 1));
        // Lower left
        p.drawImage(QPoint(-pw/2, height - yOffset + pw/2), rectImage, QRect(0, rectImage.height() - yOffset, xOffset, yOffset));

        // Lower Middle
        if (xMiddles)
            p.drawImage(QRect(xOffset-pw/2, height - yOffset +pw/2, width - xSide + pw, yOffset), rectImage,
                                QRect(rectImage.width()/2, rectImage.height() - yOffset, 1, yOffset));
        // Lower Right
        p.drawImage(QPoint(width-xOffset+pw/2, height - yOffset+pw/2), rectImage,
                        QRect(rectImage.width()-xOffset, rectImage.height() - yOffset, xOffset, yOffset));
    }
    surface.save("rsg.png");
}

void tst_QmlPainting::drawScaledScaleGridRoundedRect_data()
{
    QTest::addColumn<float>("scale");
    QTest::addColumn<int>("imageType");

    for (float i = 0; i < 3; i += .1)
        QTest::newRow(QString(QLatin1String("scale=%1; imagetype=ARGB32_Pre")).arg(i).toLatin1()) << i << (int)QImage::Format_ARGB32_Premultiplied;
    //for (float i = 0; i < 3; i += .1)
    //    QTest::newRow(QString(QLatin1String("scale=%1; imagetype=ARGB8565_Pre")).arg(i).toLatin1()) << i << (int)QImage::Format_ARGB8565_Premultiplied;
}

//code from QmlGraphicsRect for drawing rounded rects
void tst_QmlPainting::drawScaledScaleGridRoundedRect()
{
    QFETCH(float, scale);
    QFETCH(int, imageType);

    //setup image
    const int pw = 1;
    const int radius = 10;
    QImage rectImage(radius*2 + 3 + pw*2, radius*2 + 3 + pw*2, (QImage::Format)imageType);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    if (pw%2)
        rp.drawRoundedRect(QRectF(qreal(pw)/2+1, qreal(pw)/2+1, rectImage.width()-(pw+1), rectImage.height()-(pw+1)), radius, radius);
    else
        rp.drawRoundedRect(QRectF(qreal(pw)/2, qreal(pw)/2, rectImage.width()-pw, rectImage.height()-pw), radius, radius);

    //setup surface
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);
    p.scale(scale, scale);

    QBENCHMARK {
        const int pw = 2;
        int offset = int(radius+1.5+pw);
        int width = 80;
        int height = 80;

        int w = width+pw;
        int h = height+pw;
        int xOffset = offset;
        int xSide = xOffset * 2;
        bool xMiddles=true;
        if (xSide > w) {
            xMiddles=false;
            xOffset = w/2 + 1;
            xSide = xOffset * 2;
        }
        int yOffset = offset;
        int ySide = yOffset * 2;
        bool yMiddles=true;
        if (ySide > h) {
            yMiddles = false;
            yOffset = h/2 + 1;
            ySide = yOffset * 2;
        }

        // Upper left
        p.drawImage(QRect(10-pw/2, 10-pw/2, xOffset, yOffset), rectImage, QRect(0, 0, xOffset, yOffset));

        // Upper middle
        if (xMiddles)
            p.drawImage(QRect(10+xOffset-pw/2, 10+-pw/2, width - xSide + pw, yOffset), rectImage,
                                  QRect(rectImage.width()/2, 0, 1, yOffset));
        // Upper right
        p.drawImage(QPoint(10+width-xOffset+pw/2, 10+-pw/2), rectImage,
                              QRect(rectImage.width()-xOffset, 0, xOffset, yOffset));
        // Middle left
        if (yMiddles)
            p.drawImage(QRect(10+-pw/2, 10+yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                  QRect(0, rectImage.height()/2, xOffset, 1));

        // Middle
        if (xMiddles && yMiddles)
            p.drawImage(QRect(10+xOffset-pw/2, 10+yOffset-pw/2, width - xSide + pw, height - ySide + pw), rectImage,
                                QRect(rectImage.width()/2, rectImage.height()/2, 1, 1));
        // Middle right
        if (yMiddles)
            p.drawImage(QRect(10+width-xOffset+pw/2, 10+yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                QRect(rectImage.width()-xOffset, rectImage.height()/2, xOffset, 1));
        // Lower left
        p.drawImage(QPoint(10+-pw/2, 10+height - yOffset + pw/2), rectImage, QRect(0, rectImage.height() - yOffset, xOffset, yOffset));

        // Lower Middle
        if (xMiddles)
            p.drawImage(QRect(10+xOffset-pw/2, 10+height - yOffset +pw/2, width - xSide + pw, yOffset), rectImage,
                                QRect(rectImage.width()/2, rectImage.height() - yOffset, 1, yOffset));
        // Lower Right
        p.drawImage(QPoint(10+width-xOffset+pw/2, 10+height - yOffset+pw/2), rectImage,
                        QRect(rectImage.width()-xOffset, rectImage.height() - yOffset, xOffset, yOffset));
    }
    surface.save("ssg.png");
}

void tst_QmlPainting::drawTransformedScaleGridRoundedRect_data()
{
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<int>("imageType");

    for (float angle = 0; angle < 360; angle += 10)
        QTest::newRow(QString(QLatin1String("angle=%1; imagetype=ARGB32_Pre")).arg(angle).toLatin1()) << transformForAngle(angle) << (int)QImage::Format_ARGB32_Premultiplied;
    //for (float angle = 0; angle < 360; angle += 10)
    //    QTest::newRow(QString(QLatin1String("angle=%1; imagetype=ARGB8565_Pre")).arg(angle).toLatin1()) << transformForAngle(angle) << (int)QImage::Format_ARGB8565_Premultiplied;

}

//code from QmlGraphicsRect for drawing rounded rects
void tst_QmlPainting::drawTransformedScaleGridRoundedRect()
{
    QFETCH(QTransform, transform);
    QFETCH(int, imageType);

    //setup image
    const int pw = 1;
    const int radius = 10;
    QImage rectImage(radius*2 + 3 + pw*2, radius*2 + 3 + pw*2, (QImage::Format)imageType);
    rectImage.fill(0);
    QPainter rp(&rectImage);
    rp.setRenderHint(QPainter::Antialiasing);
    rp.setPen(Qt::black);
    rp.setBrush(Qt::red);
    if (pw%2)
        rp.drawRoundedRect(QRectF(qreal(pw)/2+1, qreal(pw)/2+1, rectImage.width()-(pw+1), rectImage.height()-(pw+1)), radius, radius);
    else
        rp.drawRoundedRect(QRectF(qreal(pw)/2, qreal(pw)/2, rectImage.width()-pw, rectImage.height()-pw), radius, radius);

    //setup surface
    QImage surface(400, 400, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.setWorldTransform(transform);
        const int pw = 2;
        int offset = int(radius+1.5+pw);
        int width = 80;
        int height = 80;

        int w = width+pw;
        int h = height+pw;
        int xOffset = offset;
        int xSide = xOffset * 2;
        bool xMiddles=true;
        if (xSide > w) {
            xMiddles=false;
            xOffset = w/2 + 1;
            xSide = xOffset * 2;
        }
        int yOffset = offset;
        int ySide = yOffset * 2;
        bool yMiddles=true;
        if (ySide > h) {
            yMiddles = false;
            yOffset = h/2 + 1;
            ySide = yOffset * 2;
        }

        // Upper left
        p.drawImage(QRect(100-pw/2, 100-pw/2, xOffset, yOffset), rectImage, QRect(0, 0, xOffset, yOffset));

        // Upper middle
        if (xMiddles)
            p.drawImage(QRect(100+xOffset-pw/2, 100+-pw/2, width - xSide + pw, yOffset), rectImage,
                                  QRect(rectImage.width()/2, 0, 1, yOffset));
        // Upper right
        p.drawImage(QPoint(100+width-xOffset+pw/2, 100+-pw/2), rectImage,
                              QRect(rectImage.width()-xOffset, 0, xOffset, yOffset));
        // Middle left
        if (yMiddles)
            p.drawImage(QRect(100+-pw/2, 100+yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                  QRect(0, rectImage.height()/2, xOffset, 1));

        // Middle
        if (xMiddles && yMiddles)
            p.drawImage(QRect(100+xOffset-pw/2, 100+yOffset-pw/2, width - xSide + pw, height - ySide + pw), rectImage,
                                QRect(rectImage.width()/2, rectImage.height()/2, 1, 1));
        // Middle right
        if (yMiddles)
            p.drawImage(QRect(100+width-xOffset+pw/2, 100+yOffset-pw/2, xOffset, height - ySide + pw), rectImage,
                                QRect(rectImage.width()-xOffset, rectImage.height()/2, xOffset, 1));
        // Lower left
        p.drawImage(QPoint(100+-pw/2, 100+height - yOffset + pw/2), rectImage, QRect(0, rectImage.height() - yOffset, xOffset, yOffset));

        // Lower Middle
        if (xMiddles)
            p.drawImage(QRect(100+xOffset-pw/2, 100+height - yOffset +pw/2, width - xSide + pw, yOffset), rectImage,
                                QRect(rectImage.width()/2, rectImage.height() - yOffset, 1, yOffset));
        // Lower Right
        p.drawImage(QPoint(100+width-xOffset+pw/2, 100+height - yOffset+pw/2), rectImage,
                        QRect(rectImage.width()-xOffset, rectImage.height() - yOffset, xOffset, yOffset));
    }
    surface.save("tsg.png");
}

void tst_QmlPainting::drawTransformedTransparentImage_data()
{
    QTest::addColumn<int>("imageType");

    QTest::newRow("imagetype=ARGB32_Pre") << (int)QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("imagetype=ARGB8565_Pre") << (int)QImage::Format_ARGB8565_Premultiplied;
}

void tst_QmlPainting::drawTransformedTransparentImage()
{
    QFETCH(int, imageType);

    //setup image
    QImage transImage(200, 200, (QImage::Format)imageType);
    transImage.fill(0);

    //setup surface
    QImage surface(200, 200, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.setWorldTransform(QTransform(0.956957, 0, 0.000704124, 0, 1, 0, 16.141, 0, 0.735953));
        p.drawImage(0,0, transImage);
    }
}

void tst_QmlPainting::drawTransformedSemiTransparentImage_data()
{
    QTest::addColumn<int>("imageType");

    QTest::newRow("imagetype=ARGB32_Pre") << (int)QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("imagetype=ARGB8565_Pre") << (int)QImage::Format_ARGB8565_Premultiplied;
}

void tst_QmlPainting::drawTransformedSemiTransparentImage()
{
    QFETCH(int, imageType);

    //setup image
    QImage transImage(200, 200, (QImage::Format)imageType);
    transImage.fill(QColor(0,0,0, 128).rgba());

    //setup surface
    QImage surface(200, 200, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.setWorldTransform(QTransform(0.956957, 0, 0.000704124, 0, 1, 0, 16.141, 0, 0.735953));
        p.drawImage(0,0, transImage);
    }
}

void tst_QmlPainting::drawTransformedFilledImage_data()
{
    QTest::addColumn<int>("imageType");

    QTest::newRow("imagetype=ARGB32_Pre") << (int)QImage::Format_ARGB32_Premultiplied;
    QTest::newRow("imagetype=ARGB8565_Pre") << (int)QImage::Format_ARGB8565_Premultiplied;
}

void tst_QmlPainting::drawTransformedFilledImage()
{
    QFETCH(int, imageType);

    //setup image
    QImage filledImage(200, 200, (QImage::Format)imageType);
    filledImage.fill(QColor(0,0,0).rgb());

    //setup surface
    QImage surface(200, 200, QImage::Format_RGB16);
    surface.fill(QColor(255,255,255).rgb());
    QPainter p(&surface);

    QBENCHMARK {
        p.setWorldTransform(QTransform(0.956957, 0, 0.000704124, 0, 1, 0, 16.141, 0, 0.735953));
        p.drawImage(0,0, filledImage);
    }
}

QTEST_MAIN(tst_QmlPainting)

#include "tst_qmlpainting.moc"
