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
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QImage>

#include <QtOpenGl/QGLWidget>

#include <qstatictext.h>
#include <private/qstatictext_p.h>

// #define DEBUG_SAVE_IMAGE

class tst_QStaticText: public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void constructionAndDestruction();
    void drawToPoint_data();
    void drawToPoint();
    void drawToRect_data();
    void drawToRect();
    void setFont();
    void setMaximumSize();
    void prepareToCorrectData();
    void prepareToWrongData();

    void translatedPainter();
    void rotatedPainter();
    void scaledPainter();
    void projectedPainter();
    void rotatedScaledAndTranslatedPainter();
    void transformationChanged();    

    void drawToGLWidget();
    void drawToGLWidgetRepeated();
    void drawToGLWidgetWithCaching();
    void drawToGLWidgetWithCachingRepeated();
};

void tst_QStaticText::init()
{
}

void tst_QStaticText::cleanup()
{
}

void tst_QStaticText::constructionAndDestruction()
{
    QStaticText text("My text");
}

void tst_QStaticText::drawToPoint_data()
{
    QTest::addColumn<bool>("useBackendOptimizations");

    QTest::newRow("Without backend optimizations") << false;
    QTest::newRow("With backend optimizations") << true;
}

void tst_QStaticText::drawToPoint()
{
    QFETCH(bool, useBackendOptimizations);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        text.setUseBackendOptimizations(useBackendOptimizations);
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::drawToRect_data()
{
    QTest::addColumn<bool>("useBackendOptimizations");

    QTest::newRow("Without backend optimizations") << false;
    QTest::newRow("With backend optimizations") << true;
}

void tst_QStaticText::drawToRect()
{
    QFETCH(bool, useBackendOptimizations);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.drawText(QRectF(11, 12, 10, 500), "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.", QSizeF(10, 500));
        text.setUseBackendOptimizations(useBackendOptimizations);
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);   
}

void tst_QStaticText::prepareToCorrectData()
{
    QTransform transform;
    transform.scale(2.0, 2.0);
    transform.rotate(90, Qt::ZAxis);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.setTransform(transform);
        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.setTransform(transform);
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        text.prepare(transform, p.font());
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::prepareToWrongData()
{
    QTransform transform;
    transform.scale(2.0, 2.0);
    transform.rotate(90, Qt::ZAxis);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        text.prepare(transform, p.font());
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}


void tst_QStaticText::setFont()
{
    QFont font = QApplication::font();
    font.setBold(true);
    font.setPointSize(28);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.drawText(0, 0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

        p.setFont(font);
        p.drawText(11, 120, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);

        QStaticText text;
        text.setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

        p.drawStaticText(0, 0, text);

        p.setFont(font);
        p.drawStaticText(11, 120, text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::setMaximumSize()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.drawText(QRectF(11, 12, 10, 500), "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        text.setMaximumSize(QSizeF(10, 500));
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::translatedPainter()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.translate(100, 200);

        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.translate(100, 200);

        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::rotatedPainter()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.rotate(30.0);
        p.drawText(0, 0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

        QPainter p(&imageDrawStaticText);
        p.rotate(30.0);
        p.drawStaticText(QPoint(0, 0), text);
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("rotatedPainter_imageDrawText.png");
    imageDrawStaticText.save("rotatedPainter_imageDrawStaticText.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::scaledPainter()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.scale(2.0, 0.2);

        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.scale(2.0, 0.2);

        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::projectedPainter()
{
    QTransform transform;
    transform.rotate(90, Qt::XAxis);

    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.setTransform(transform);

        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.setTransform(transform);

        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        p.drawStaticText(QPointF(11, 12), text);
    }

    QCOMPARE(imageDrawStaticText, imageDrawText);

}

void tst_QStaticText::rotatedScaledAndTranslatedPainter()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.rotate(45.0);
        p.scale(2.0, 2.0);
        p.translate(100, 200);

        p.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.rotate(45.0);
        p.scale(2.0, 2.0);
        p.translate(100, 200);

        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        p.drawStaticText(QPointF(11, 12), text);
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("rotatedScaledAndPainter_imageDrawText.png");
    imageDrawStaticText.save("rotatedScaledAndPainter_imageDrawStaticText.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::transformationChanged()
{
    QPixmap imageDrawText(1000, 1000);
    imageDrawText.fill(Qt::white);
    {
        QPainter p(&imageDrawText);
        p.rotate(33.0);
        p.scale(0.5, 0.7);

        p.drawText(0, 0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");

        p.scale(7.0, 5.0);
        p.drawText(0, 0, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
    }

    QPixmap imageDrawStaticText(1000, 1000);
    imageDrawStaticText.fill(Qt::white);
    {
        QPainter p(&imageDrawStaticText);
        p.rotate(33.0);
        p.scale(0.5, 0.7);

        QStaticText text("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        p.drawStaticText(QPointF(0, 0), text);

        p.scale(7.0, 5.0);
        p.drawStaticText(QPointF(0, 0), text);
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("transformationChanged_imageDrawText.png");
    imageDrawStaticText.save("transformationChanged_imageDrawStaticText.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

class MyGLWidget: public QGLWidget
{
public:
    enum PaintEventType {
        NormalDrawText,
        NormalDrawStaticText,
        DrawStaticTextWithCaching
    };

    MyGLWidget(PaintEventType paintEventType, QWidget *parent = 0)
        : QGLWidget(parent), m_paintEventType(paintEventType)
    {
        m_staticText.setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
        if (m_paintEventType == DrawStaticTextWithCaching)
            m_staticText.setUseBackendOptimizations(true);
        shown = false;
    }

    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.fillRect(rect(), Qt::white);
        switch (m_paintEventType) {
        case NormalDrawStaticText:
        case DrawStaticTextWithCaching:
            painter.drawStaticText(QPointF(11, 12), m_staticText);
            break;
        case NormalDrawText:
            painter.drawText(11, 12, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.");
            break;
        };
        shown = true;
    }

    bool shown;

private:
    QStaticText m_staticText;
    PaintEventType m_paintEventType;
};

void tst_QStaticText::drawToGLWidget()
{
    QImage imageDrawText;
    {
        MyGLWidget glWidget(MyGLWidget::NormalDrawText);
        glWidget.show();
        while (!glWidget.shown)
            QApplication::processEvents();
        imageDrawText = glWidget.grabFrameBuffer();
    }

    QImage imageDrawStaticText;
    {
        MyGLWidget glWidget(MyGLWidget::NormalDrawStaticText);
        glWidget.show();
        while (!glWidget.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidget.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidget_imageDrawText.png");
    imageDrawStaticText.save("drawToGLWidget_imageDrawStaticText.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::drawToGLWidgetWithCaching()
{
    QImage imageDrawText;
    {
        MyGLWidget glWidget(MyGLWidget::NormalDrawText);
        glWidget.show();
        while (!glWidget.shown)
            QApplication::processEvents();
        imageDrawText = glWidget.grabFrameBuffer();
    }

    QImage imageDrawStaticText;
    {
        MyGLWidget glWidget(MyGLWidget::DrawStaticTextWithCaching);
        glWidget.show();
        while (!glWidget.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidget.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidgetWithCaching_imageDrawText.png");
    imageDrawStaticText.save("drawToGLWidgetWithCaching_imageDrawStaticText.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::drawToGLWidgetWithCachingRepeated()
{    
    QImage imageDrawText;
    MyGLWidget glWidgetNormal(MyGLWidget::NormalDrawText);

    {
        glWidgetNormal.show();
        while (!glWidgetNormal.shown)
            QApplication::processEvents();
        imageDrawText = glWidgetNormal.grabFrameBuffer();
    }

    QImage imageDrawStaticText;
    MyGLWidget glWidgetAbnormal(MyGLWidget::DrawStaticTextWithCaching);
    {

        glWidgetAbnormal.show();
        while (!glWidgetAbnormal.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidgetAbnormal.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidgetWithCachingRepeated_imageDrawText1.png");
    imageDrawStaticText.save("drawToGLWidgetWithCachingRepeated_imageDrawStaticText1.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);

    {
        glWidgetNormal.shown = false;
        glWidgetNormal.update();
        while (!glWidgetNormal.shown)
            QApplication::processEvents();
        imageDrawText = glWidgetNormal.grabFrameBuffer();
    }

    {
        glWidgetAbnormal.shown = false;
        glWidgetAbnormal.update();
        while (!glWidgetAbnormal.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidgetAbnormal.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidgetWithCachingRepeated_imageDrawText2.png");
    imageDrawStaticText.save("drawToGLWidgetWithCachingRepeated_imageDrawStaticText2.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}

void tst_QStaticText::drawToGLWidgetRepeated()
{
    QImage imageDrawText;
    MyGLWidget glWidgetNormal(MyGLWidget::NormalDrawText);

    {
        glWidgetNormal.show();
        while (!glWidgetNormal.shown)
            QApplication::processEvents();
        imageDrawText = glWidgetNormal.grabFrameBuffer();
    }

    QImage imageDrawStaticText;
    MyGLWidget glWidgetAbnormal(MyGLWidget::NormalDrawStaticText);
    {

        glWidgetAbnormal.show();
        while (!glWidgetAbnormal.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidgetAbnormal.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidgetWithCachingRepeated_imageDrawText1.png");
    imageDrawStaticText.save("drawToGLWidgetWithCachingRepeated_imageDrawStaticText1.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);

    {
        glWidgetNormal.shown = false;
        glWidgetNormal.update();
        while (!glWidgetNormal.shown)
            QApplication::processEvents();
        imageDrawText = glWidgetNormal.grabFrameBuffer();
    }

    {
        glWidgetAbnormal.shown = false;
        glWidgetAbnormal.update();
        while (!glWidgetAbnormal.shown)
            QApplication::processEvents();
        imageDrawStaticText = glWidgetAbnormal.grabFrameBuffer();
    }

#if defined(DEBUG_SAVE_IMAGE)
    imageDrawText.save("drawToGLWidgetWithCachingRepeated_imageDrawText2.png");
    imageDrawStaticText.save("drawToGLWidgetWithCachingRepeated_imageDrawStaticText2.png");
#endif

    QCOMPARE(imageDrawStaticText, imageDrawText);
}


QTEST_MAIN(tst_QStaticText)
#include "tst_qstatictext.moc"
