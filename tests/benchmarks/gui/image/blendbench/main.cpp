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
#include <QtGui>
#include <QString>

#include <qtest.h>

void paint(QPaintDevice *device)
{
    QPainter p(device);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(0, 0, device->width(), device->height(), Qt::transparent);

    QLinearGradient g(QPoint(0, 0), QPoint(1, 1));
//    g.setCoordinateMode(QGradient::ObjectBoundingMode);
    g.setColorAt(0, Qt::magenta);
    g.setColorAt(0, Qt::white);

//    p.setOpacity(0.8);
    p.setPen(Qt::NoPen);
    p.setBrush(g);
    p.setRenderHint(QPainter::Antialiasing);
    p.setOpacity(0.9);
    p.drawEllipse(0, 0, device->width(), device->height());
}

QLatin1String compositionModes[] = {
    QLatin1String("SourceOver"),
    QLatin1String("DestinationOver"),
    QLatin1String("Clear"),
    QLatin1String("Source"),
    QLatin1String("Destination"),
    QLatin1String("SourceIn"),
    QLatin1String("DestinationIn"),
    QLatin1String("SourceOut"),
    QLatin1String("DestinationOut"),
    QLatin1String("SourceAtop"),
    QLatin1String("DestinationAtop"),
    QLatin1String("Xor"),

    //svg 1.2 blend modes
    QLatin1String("Plus"),
    QLatin1String("Multiply"),
    QLatin1String("Screen"),
    QLatin1String("Overlay"),
    QLatin1String("Darken"),
    QLatin1String("Lighten"),
    QLatin1String("ColorDodge"),
    QLatin1String("ColorBurn"),
    QLatin1String("HardLight"),
    QLatin1String("SoftLight"),
    QLatin1String("Difference"),
    QLatin1String("Exclusion")
};

enum BrushType { ImageBrush, SolidBrush };
QLatin1String brushTypes[] = {
    QLatin1String("ImageBrush"),
    QLatin1String("SolidBrush"),
};

class BlendBench : public QObject
{
    Q_OBJECT
private slots:		
    void blendBench_data();
    void blendBench();
};

void BlendBench::blendBench_data()
{
    int first = 0;
    int limit = 12;
    if (qApp->arguments().contains("--extended")) {
        first = 12;
        limit = 24;
    }

    QTest::addColumn<int>("brushType");
    QTest::addColumn<int>("compositionMode");

    for (int brush = ImageBrush; brush <= SolidBrush; ++brush)
        for (int mode = first; mode < limit; ++mode)
            QTest::newRow(QString("brush=%1; mode=%2")
                          .arg(brushTypes[brush]).arg(compositionModes[mode]).toAscii().data())
                << brush << mode;
}

void BlendBench::blendBench()
{
    QFETCH(int, brushType);
    QFETCH(int, compositionMode);

    QImage img(512, 512, QImage::Format_ARGB32_Premultiplied);
    QImage src(512, 512, QImage::Format_ARGB32_Premultiplied);
    paint(&src);
    QPainter p(&img);
    p.setPen(Qt::NoPen);

    p.setCompositionMode(QPainter::CompositionMode(compositionMode));
    if (brushType == ImageBrush) {
        p.setBrush(QBrush(src));
    } else if (brushType == SolidBrush) {
        p.setBrush(QColor(127, 127, 127, 127));
    }

    QBENCHMARK {
        p.drawRect(0, 0, 512, 512);
    }
}

QTEST_MAIN(BlendBench)

#include "main.moc"
