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

#include <q3canvas.h>
#include <qapplication.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qlabel.h>

//TESTED_CLASS=Q3CanvasEllipse Q3CanvasItem Q3CanvasItemList Q3CanvasLine Q3CanvasPixmap Q3CanvasPixmapArray Q3CanvasPolygon Q3CanvasPolygonalItem Q3CanvasRectangle Q3CanvasSpline Q3CanvasSprite Q3CanvasText Q3CanvasView
//TESTED_FILES=

class tst_Q3Canvas : public QObject
{
    Q_OBJECT

public:
    tst_Q3Canvas();
    virtual ~tst_Q3Canvas();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
   void width();
   void height();
   void onCanvas();
   void task26486();
   void moveLine();

   void moveRectangle();
   void qcanvaspixmaparraycrash();

private:
    Q3Canvas *testWidget;
    Q3CanvasView *testWidgetView;

};


tst_Q3Canvas::tst_Q3Canvas()
{

}

tst_Q3Canvas::~tst_Q3Canvas()
{
}

void tst_Q3Canvas::initTestCase()
{
    testWidget = new Q3Canvas(0);
    testWidgetView = new Q3CanvasView(testWidget);
    testWidgetView->show();
}

void tst_Q3Canvas::cleanupTestCase()
{
    delete testWidget;
    delete testWidgetView;
}

void tst_Q3Canvas::init()
{
}

void tst_Q3Canvas::cleanup()
{
}


void tst_Q3Canvas::width()
{
    testWidget->resize(100,100);
    QVERIFY(testWidget->width() == 100);
}

void tst_Q3Canvas::height()
{
    testWidget->resize(100,100);
    QVERIFY(testWidget->height() == 100);
}


void tst_Q3Canvas::onCanvas()
{
    testWidget->resize(100,100);
    QVERIFY(testWidget->onCanvas(0, 0));
    QVERIFY(!testWidget->onCanvas(testWidget->width(), testWidget->height()));
}


class MyCanvas : public Q3Canvas
{
public:
    MyCanvas(int width, int height) : Q3Canvas(width, height) {}
protected:
	void drawBackground(QPainter & p, const QRect &)
	{
	    p.setPen(QPen(Qt::red));
	    p.drawRect(0,0,width()-1, height()-1);
	}
};


void tst_Q3Canvas::task26486()
{
    /*
    If a rectangle is drawn on the background of the canvas, where the 
    rectangle has a width of Q3Canvas::width()-1 then it should draw on 
    the edge of the canvas. It dose not, it draws 1 pixel in. 
    If a transfomation is applied this dose not happen.
    See Task 26486
    */
    
    MyCanvas canvas(100,100);
    Q3CanvasView view;
    view.setCanvas(&canvas);

    view.resize(canvas.width() + 20, canvas.height() + 20);
    view.show();
        
    QPixmap testPix("backgroundrect.png");

    QEXPECT_FAIL("", "Broken, see task 26486", Continue);
    QPixmap expect = QPixmap::grabWidget(&view);
    QVERIFY(pixmapsAreEqual(&expect,&testPix));

    /*QLabel l(0);
    l.setPixmap(QPixmap::grabWidget(&view));
    l.show();
    while (1)
	qApp->processEvents();
    */
}

void tst_Q3Canvas::moveLine()
{
    Q3CanvasLine canvasLine(testWidget);
    canvasLine.setPoints(0,0,10,10);
    canvasLine.show();
    canvasLine.moveBy(50,50);

    QCOMPARE(canvasLine.startPoint(), QPoint(0,0));
    QCOMPARE(canvasLine.endPoint(), QPoint(10,10));

    QCOMPARE(canvasLine.x(), 50.0);
    QCOMPARE(canvasLine.y(), 50.0);

    canvasLine.moveBy(10, -20);
    QCOMPARE(canvasLine.startPoint(), QPoint(0,0));
    QCOMPARE(canvasLine.endPoint(), QPoint(10,10));

    QCOMPARE(canvasLine.x(), 60.0);
    QCOMPARE(canvasLine.y(), 30.0);
    
    canvasLine.moveBy(-10, -10);
    QCOMPARE(canvasLine.startPoint(), QPoint(0,0));
    QCOMPARE(canvasLine.endPoint(), QPoint(10,10));

    QCOMPARE(canvasLine.x(), 50.0);
    QCOMPARE(canvasLine.y(), 20.0);
}

void tst_Q3Canvas::moveRectangle()
{
    Q3CanvasRectangle canvasRectangle(testWidget);
    canvasRectangle.show();

    canvasRectangle.moveBy(50,50);

    QCOMPARE(canvasRectangle.x(), 50.0);
    QCOMPARE(canvasRectangle.y(), 50.0);

    canvasRectangle.moveBy(10, -20);

    QCOMPARE(canvasRectangle.x(), 60.0);
    QCOMPARE(canvasRectangle.y(), 30.0);
    
    canvasRectangle.moveBy(-10, -10);

    QCOMPARE(canvasRectangle.x(), 50.0);
    QCOMPARE(canvasRectangle.y(), 20.0);

}

void tst_Q3Canvas::qcanvaspixmaparraycrash()
{
    Q3CanvasPixmapArray pixArray("foo%1.png", 2);
    QVERIFY(true);
}


QTEST_MAIN(tst_Q3Canvas)
#include "tst_q3canvas.moc"
