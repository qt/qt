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

#include <QApplication>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QGLWidget>
#include <QTextLayout>
#include <QVBoxLayout>
#include <QTime>
#include <QDebug>

#ifdef HAVE_STATICTEXT
#include <private/qstatictext_p.h>
#endif

int iterations = 20;
const int count = 600;
const int lines = 12;
const int spacing = 36;
QSizeF size(1000, 800);
const qreal lineWidth = 1000;
QString strings[lines];

void paint_QTextLayout(QPainter &p, bool useCache)
{
    static bool first = true;
    static QTextLayout *textLayout[lines];
    if (first) {
        for (int i = 0; i < lines; ++i) {
            textLayout[i] = new QTextLayout(strings[i]);
            int leading = p.fontMetrics().leading();
            qreal height = 0;
            qreal widthUsed = 0;
            textLayout[i]->setCacheEnabled(useCache);
            textLayout[i]->beginLayout();
            while (1) {
                QTextLine line = textLayout[i]->createLine();
                if (!line.isValid())
                        break;

                line.setLineWidth(lineWidth);
                height += leading;
                line.setPosition(QPointF(0, height));
                height += line.height();
                widthUsed = qMax(widthUsed, line.naturalTextWidth());
            }
            textLayout[i]->endLayout();
        }
        first = false;
    }
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < lines; ++j) {
            textLayout[j]->draw(&p, QPoint(0, j*spacing));
        }
    }
}

void paint_QTextLayout_noCache(QPainter &p)
{
    paint_QTextLayout(p, false);
}

void paint_QTextLayout_cache(QPainter &p)
{
    paint_QTextLayout(p, true);
}

#ifdef HAVE_STATICTEXT
void paint_QStaticText(QPainter &p, bool useOptimizations)
{
    static QStaticText *staticText[lines];
    static bool first = true;
    if (first) {
        for (int i = 0; i < lines; ++i) {
            staticText[i] = new QStaticText(strings[i]);
            staticText[i]->setUseBackendOptimizations(useOptimizations);
        }
        first = false;
    }
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < lines; ++j) {
            p.drawStaticText(QPointF(0, 30 + j*spacing), *staticText[j]);
        }
    }
}

void paint_QStaticText_noOptimizations(QPainter &p)
{
    paint_QStaticText(p, false);
}

void paint_QStaticText_optimizations(QPainter &p)
{
    paint_QStaticText(p, true);
}
#endif

void paint_QPixmapCachedText(QPainter &p)
{
    static bool first = true;
    static QPixmap cacheText[lines];
    if (first) {
        for (int i = 0; i < lines; ++i) {
            QRectF trueSize;
            trueSize = p.boundingRect(QRectF(QPointF(0,0), size), 0, strings[i]);
            cacheText[i] = QPixmap(trueSize.size().toSize());
            cacheText[i].fill(Qt::transparent);
            QPainter paint(&cacheText[i]);
            paint.setPen(Qt::black);
            paint.drawText(QRectF(QPointF(0,0), trueSize.size().toSize()), strings[i]);
        }
        first = false;
    }
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < lines; ++j) {
            p.drawPixmap(0,j*spacing,cacheText[j]);
        }
    }
}

void paint_RoundedRect(QPainter &p)
{
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::black);
    p.setBrush(Qt::red);
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < lines; ++j) {
            QSize size((j+1)*50, spacing-1);
            p.drawRoundedRect(QRectF(QPointF(0,j*spacing), size), 8, 8);
        }
    }
}

void paint_QPixmapCachedRoundedRect(QPainter &p)
{
    static bool first = true;
    static QPixmap cacheRect[lines];
    if (first) {
        for (int i = 0; i < lines; ++i) {
            QSize size((i+1)*50, spacing-1);
            cacheRect[i] = QPixmap(size);
            cacheRect[i].fill(Qt::transparent);
            QPainter paint(&cacheRect[i]);
            paint.setRenderHint(QPainter::Antialiasing);
            paint.setPen(Qt::black);
            paint.setBrush(Qt::red);
            paint.drawRoundedRect(QRect(QPoint(0,0), size), 8, 8);
        }
        first = false;
    }
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < lines; ++j) {
            p.drawPixmap(0,j*spacing,cacheRect[j]);
        }
    }
}

typedef void(*PaintFunc)(QPainter &);

struct {
    const char *name;
    PaintFunc func;
} funcs[] = {
    { "QTextLayoutNoCache", &paint_QTextLayout_noCache },
    { "QTextLayoutWithCache", &paint_QTextLayout_cache },
#ifdef HAVE_STATICTEXT
    { "QStaticTextNoBackendOptimizations", &paint_QStaticText_noOptimizations },
    { "QStaticTextWithBackendOptimizations", &paint_QStaticText_optimizations },
#endif
    { "CachedText", &paint_QPixmapCachedText },
    { "RoundedRect", &paint_RoundedRect },
    { "CachedRoundedRect", &paint_QPixmapCachedRoundedRect },
    { 0, 0 }
};

PaintFunc testFunc = 0;

class MyGLWidget : public QGLWidget
{
public:
    MyGLWidget(const QGLFormat &format) : QGLWidget(format), frames(0) {
        const char chars[] = "abcd efgh ijkl mnop qrst uvwx yz!$. ABCD 1234";
        int len = strlen(chars);
        for (int i = 0; i < lines; ++i) {
            for (int j = 0; j < 60; j++) {
                strings[i] += QChar(chars[rand() % len]);
            }
        }
    }

    void paintEvent(QPaintEvent *) {
        static int last = 0;
        static bool firstRun = true;
        if (firstRun == 0) {
            timer.start();
            firstRun = false;
        } else {
            int elapsed = timer.elapsed();
            qDebug() << "frame elapsed:" << elapsed - last;
            last = elapsed;
        }
        QPainter p(this);
	p.fillRect(rect(), Qt::white);
	p.setPen(Qt::black);
        QTime drawTimer;
        drawTimer.start();
        testFunc(p);
        qDebug() << "draw time" << drawTimer.elapsed();
        if (iterations--)
            update();
        else
            qApp->quit();
    }

    QTime timer;
    int frames;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    bool sampleBuffers = false;

    for (int i = 1; i < argc; ++i) {
        QString arg = argv[i];
        if (arg == "-test") {
            arg = argv[++i];
            int j = 0;
            while (funcs[j].name) {
                if (arg == funcs[j].name) {
                    testFunc = funcs[j].func;
                    qDebug() << "Running test" << arg;
                    break;
                }
                ++j;
            }
        } else if (arg == "-iterations") {
            arg = argv[++i];
            iterations = arg.toInt();
        } else if (arg == "-sampleBuffers") {
            sampleBuffers = true;
        }
    }

    if (testFunc == 0) {
        qDebug() << "Usage: textspeed -test <test> [-sampleBuffers] [-iterations n]";
        qDebug() << "where <test> can be:";
        int j = 0;
        while (funcs[j].name) {
            qDebug() << "  " << funcs[j].name;
            ++j;
        }
        exit(1);
    }

    QWidget w;
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSampleBuffers(sampleBuffers);
    MyGLWidget *glw = new MyGLWidget(format);
    glw->setAutoFillBackground(false);
    QVBoxLayout *layout = new QVBoxLayout(&w);
    w.setLayout(layout);
    layout->addWidget(glw);
    w.showFullScreen();
    app.exec();

    return 0;
}
