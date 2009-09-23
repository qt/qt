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
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#ifdef Q_WS_X11
QT_BEGIN_NAMESPACE
extern void qt_x11_wait_for_window_manager(QWidget *);
QT_END_NAMESPACE
#endif
#include "chiptester/chiptester.h"
//#define CALLGRIND_DEBUG
#ifdef CALLGRIND_DEBUG
#include "valgrind/callgrind.h"
#endif

//TESTED_FILES=

class QEventWaiter : public QEventLoop
{
public:
    QEventWaiter(QObject *receiver, QEvent::Type type)
	: waiting(false), t(type)
    {
	receiver->installEventFilter(this);
    }

    void wait()
    {
	waiting = true;
	exec();
    }

    bool eventFilter(QObject *receiver, QEvent *event)
    {
	Q_UNUSED(receiver);
	if (waiting && event->type() == t) {
	    waiting = false;
	    exit();
	}
	return false;
    }

private:
    bool waiting;
    QEvent::Type t;
};

class tst_QGraphicsView : public QObject
{
    Q_OBJECT

public:
    tst_QGraphicsView();
    virtual ~tst_QGraphicsView();

public slots:
    void init();
    void cleanup();

private slots:
    void construct();
    void paintSingleItem();
    void paintDeepStackingItems();
    void paintDeepStackingItems_clipped();
    void moveSingleItem();
    void mapPointToScene_data();
    void mapPointToScene();
    void mapPointFromScene_data();
    void mapPointFromScene();
    void mapRectToScene_data();
    void mapRectToScene();
    void mapRectFromScene_data();
    void mapRectFromScene();
    void chipTester_data();
    void chipTester();
    void deepNesting_data();
    void deepNesting();
    void imageRiver_data();
    void imageRiver();
    void textRiver_data();
    void textRiver();
    void moveItemCache_data();
    void moveItemCache();
    void paintItemCache_data();
    void paintItemCache();
};

tst_QGraphicsView::tst_QGraphicsView()
{
}

tst_QGraphicsView::~tst_QGraphicsView()
{
}

void tst_QGraphicsView::init()
{
}

void tst_QGraphicsView::cleanup()
{
}

void tst_QGraphicsView::construct()
{
    QBENCHMARK {
        QGraphicsView view;
    }
}

void tst_QGraphicsView::paintSingleItem()
{
    QGraphicsScene scene(0, 0, 100, 100);
    scene.addRect(0, 0, 10, 10);

    QGraphicsView view(&scene);
    view.show();
    view.resize(100, 100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    QBENCHMARK {
	view.viewport()->render(&painter);
    }
}

void tst_QGraphicsView::paintDeepStackingItems()
{
    QGraphicsScene scene(0, 0, 100, 100);
    QGraphicsRectItem *item = scene.addRect(0, 0, 10, 10);
    QGraphicsRectItem *lastRect = item;
    for (int i = 0; i < 1000; ++i) {
	QGraphicsRectItem *rect = scene.addRect(0, 0, 10, 10);
	rect->setPos(1, 1);
	rect->setParentItem(lastRect);	
	lastRect = rect;
    }

    QGraphicsView view(&scene);
    view.show();
    view.resize(100, 100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    QBENCHMARK {
	view.viewport()->render(&painter);
    }
}

void tst_QGraphicsView::paintDeepStackingItems_clipped()
{
    QGraphicsScene scene(0, 0, 100, 100);
    QGraphicsRectItem *item = scene.addRect(0, 0, 10, 10);
    item->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QGraphicsRectItem *lastRect = item;
    for (int i = 0; i < 1000; ++i) {
	QGraphicsRectItem *rect = scene.addRect(0, 0, 10, 10);
	rect->setPos(1, 1);
	rect->setParentItem(lastRect);	
	lastRect = rect;
    }

    QGraphicsView view(&scene);
    view.show();
    view.resize(100, 100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    QBENCHMARK {
	view.viewport()->render(&painter);
    }
}

void tst_QGraphicsView::moveSingleItem()
{
    QGraphicsScene scene(0, 0, 100, 100);
    QGraphicsRectItem *item = scene.addRect(0, 0, 10, 10);

    QGraphicsView view(&scene);
    view.show();
    view.resize(100, 100);
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QEventWaiter waiter(view.viewport(), QEvent::Paint);
    int n = 1;
    QBENCHMARK {
        item->setPos(25 * n, 25 * n);
	waiter.wait();
	n = n ? 0 : 1;
    }
}

void tst_QGraphicsView::mapPointToScene_data()
{
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QPoint>("point");

    QTest::newRow("null") << QTransform() << QPoint();
    QTest::newRow("identity  QPoint(100, 100)") << QTransform() << QPoint(100, 100);
    QTest::newRow("rotate    QPoint(100, 100)") << QTransform().rotate(90) << QPoint(100, 100);
    QTest::newRow("scale     QPoint(100, 100)") << QTransform().scale(5, 5) << QPoint(100, 100);
    QTest::newRow("translate QPoint(100, 100)") << QTransform().translate(5, 5) << QPoint(100, 100);
    QTest::newRow("shear     QPoint(100, 100)") << QTransform().shear(1.5, 1.5) << QPoint(100, 100);
    QTest::newRow("perspect  QPoint(100, 100)") << QTransform().rotate(45, Qt::XAxis) << QPoint(100, 100);
}

void tst_QGraphicsView::mapPointToScene()
{
    QFETCH(QTransform, transform);
    QFETCH(QPoint, point);

    QGraphicsView view;
    view.setTransform(transform);
    QBENCHMARK {
        view.mapToScene(point);
    }
}

void tst_QGraphicsView::mapPointFromScene_data()
{
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QPointF>("point");

    QTest::newRow("null") << QTransform() << QPointF();
    QTest::newRow("identity  QPointF(100, 100)") << QTransform() << QPointF(100, 100);
    QTest::newRow("rotate    QPointF(100, 100)") << QTransform().rotate(90) << QPointF(100, 100);
    QTest::newRow("scale     QPointF(100, 100)") << QTransform().scale(5, 5) << QPointF(100, 100);
    QTest::newRow("translate QPointF(100, 100)") << QTransform().translate(5, 5) << QPointF(100, 100);
    QTest::newRow("shear     QPointF(100, 100)") << QTransform().shear(1.5, 1.5) << QPointF(100, 100);
    QTest::newRow("perspect  QPointF(100, 100)") << QTransform().rotate(45, Qt::XAxis) << QPointF(100, 100);
}

void tst_QGraphicsView::mapPointFromScene()
{
    QFETCH(QTransform, transform);
    QFETCH(QPointF, point);

    QGraphicsView view;
    view.setTransform(transform);
    QBENCHMARK {
        view.mapFromScene(point);
    }
}

void tst_QGraphicsView::mapRectToScene_data()
{
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QRect>("rect");

    QTest::newRow("null") << QTransform() << QRect();
    QTest::newRow("identity  QRect(0, 0, 100, 100)") << QTransform() << QRect(0, 0, 100, 100);
    QTest::newRow("rotate    QRect(0, 0, 100, 100)") << QTransform().rotate(90) << QRect(0, 0, 100, 100);
    QTest::newRow("scale     QRect(0, 0, 100, 100)") << QTransform().scale(5, 5) << QRect(0, 0, 100, 100);
    QTest::newRow("translate QRect(0, 0, 100, 100)") << QTransform().translate(5, 5) << QRect(0, 0, 100, 100);
    QTest::newRow("shear     QRect(0, 0, 100, 100)") << QTransform().shear(1.5, 1.5) << QRect(0, 0, 100, 100);
    QTest::newRow("perspect  QRect(0, 0, 100, 100)") << QTransform().rotate(45, Qt::XAxis) << QRect(0, 0, 100, 100);
}

void tst_QGraphicsView::mapRectToScene()
{
    QFETCH(QTransform, transform);
    QFETCH(QRect, rect);

    QGraphicsView view;
    view.setTransform(transform);
    QBENCHMARK {
        view.mapToScene(rect);
    }
}

void tst_QGraphicsView::mapRectFromScene_data()
{
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QRectF>("rect");

    QTest::newRow("null") << QTransform() << QRectF();
    QTest::newRow("identity  QRectF(0, 0, 100, 100)") << QTransform() << QRectF(0, 0, 100, 100);
    QTest::newRow("rotate    QRectF(0, 0, 100, 100)") << QTransform().rotate(90) << QRectF(0, 0, 100, 100);
    QTest::newRow("scale     QRectF(0, 0, 100, 100)") << QTransform().scale(5, 5) << QRectF(0, 0, 100, 100);
    QTest::newRow("translate QRectF(0, 0, 100, 100)") << QTransform().translate(5, 5) << QRectF(0, 0, 100, 100);
    QTest::newRow("shear     QRectF(0, 0, 100, 100)") << QTransform().shear(1.5, 1.5) << QRectF(0, 0, 100, 100);
    QTest::newRow("perspect  QRectF(0, 0, 100, 100)") << QTransform().rotate(45, Qt::XAxis) << QRectF(0, 0, 100, 100);
}

void tst_QGraphicsView::mapRectFromScene()
{
    QFETCH(QTransform, transform);
    QFETCH(QRectF, rect);

    QGraphicsView view;
    view.setTransform(transform);
    QBENCHMARK {
        view.mapFromScene(rect);
    }
}

void tst_QGraphicsView::chipTester_data()
{
    QTest::addColumn<bool>("antialias");
    QTest::addColumn<bool>("opengl");
    QTest::addColumn<int>("operation");
    QTest::newRow("rotate, normal") << false << false << 0;
    QTest::newRow("rotate, normal, antialias") << true << false << 0;
    QTest::newRow("rotate, opengl") << false << true << 0;
    QTest::newRow("rotate, opengl, antialias") << true << true << 0;
    QTest::newRow("zoom, normal") << false << false << 1;
    QTest::newRow("zoom, normal, antialias") << true << false << 1;
    QTest::newRow("zoom, opengl") << false << true << 1;
    QTest::newRow("zoom, opengl, antialias") << true << true << 1;
    QTest::newRow("translate, normal") << false << false << 2;
    QTest::newRow("translate, normal, antialias") << true << false << 2;
    QTest::newRow("translate, opengl") << false << true << 2;
    QTest::newRow("translate, opengl, antialias") << true << true << 2;
}

void tst_QGraphicsView::chipTester()
{
    QFETCH(bool, antialias);
    QFETCH(bool, opengl);
    QFETCH(int, operation);

    ChipTester tester;
    tester.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&tester);
#endif
    tester.setAntialias(antialias);
    tester.setOpenGL(opengl);
    tester.setOperation(ChipTester::Operation(operation));
    QBENCHMARK {
	tester.runBenchmark();
    }
}

static void addChildHelper(QGraphicsItem *parent, int n, bool rotate)
{
    if (!n)
	return;
    QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(0, 0, 50, 50), parent);
    item->setPos(10, 10);
    if (rotate)
	item->rotate(10);
    addChildHelper(item, n - 1, rotate);
}

void tst_QGraphicsView::deepNesting_data()
{
    QTest::addColumn<bool>("rotate");
    QTest::addColumn<bool>("sortCache");
    QTest::addColumn<bool>("bsp");

    QTest::newRow("bsp, no transform") << false << false << true;
    QTest::newRow("bsp, rotation") << true << false << true;
    QTest::newRow("bsp, no transform, sort cache") << false << true << true;
    QTest::newRow("bsp, rotation, sort cache") << true << true << true;
    QTest::newRow("no transform") << false << false << false;
    QTest::newRow("rotation") << true << false << false;
    QTest::newRow("no transform, sort cache") << false << true << false;
    QTest::newRow("rotation, sort cache") << true << true << false;
}

void tst_QGraphicsView::deepNesting()
{
    QFETCH(bool, rotate);
    QFETCH(bool, sortCache);
    QFETCH(bool, bsp);

    QGraphicsScene scene;
    for (int y = 0; y < 15; ++y) {
	for (int x = 0; x < 15; ++x) {
	    QGraphicsItem *item1 = scene.addRect(QRectF(0, 0, 50, 50));
	    if (rotate) item1->rotate(10);
	    item1->setPos(x * 25, y * 25);
	    addChildHelper(item1, 30, rotate);
	}
    }
    scene.setItemIndexMethod(bsp ? QGraphicsScene::BspTreeIndex : QGraphicsScene::NoIndex);
    scene.setSortCacheEnabled(sortCache);

    QGraphicsView view(&scene);
    view.setRenderHint(QPainter::Antialiasing);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(250);

    QBENCHMARK {
#ifdef CALLGRIND_DEBUG
	CALLGRIND_START_INSTRUMENTATION
#endif
	view.viewport()->repaint();
#ifdef CALLGRIND_DEBUG
	CALLGRIND_STOP_INSTRUMENTATION
#endif
    }
}

class AnimatedPixmapItem : public QGraphicsPixmapItem
{
public:
    AnimatedPixmapItem(int x, int y, bool rot, bool scal, QGraphicsItem *parent = 0)
        : QGraphicsPixmapItem(parent), rotateFactor(0), scaleFactor(0)
    {
	rotate = rot;
	scale = scal;
	xspeed = x;
	yspeed = y;
    }

protected:
    void advance(int i)
    { 
	if (!i)
	    return;
	int x = int(pos().x()) + pixmap().width();
	x += xspeed;
	x = (x % (300 + pixmap().width() * 2)) - pixmap().width();
	int y = int(pos().y()) + pixmap().width();
	y += yspeed;
	y = (y % (300 + pixmap().width() * 2)) - pixmap().width();
	setPos(x, y);

	int rot = rotateFactor;
	int sca = scaleFactor;
        if (rotate)
	    rotateFactor = 1 + (rot + xspeed) % 360;
        if (scale)
	    scaleFactor = 1 + (sca + yspeed) % 50;

	if (rotate || scale) {
	    qreal s = 0.5 + scaleFactor / 50.0;
	    setTransform(QTransform().rotate(rotateFactor).scale(s, s));
	}
    }

private:
    int xspeed;
    int yspeed;
    int rotateFactor;
    int scaleFactor;
    bool rotate;
    bool scale;
};

class CountPaintEventView : public QGraphicsView
{
public:
    CountPaintEventView(QGraphicsScene *scene = 0)
        : QGraphicsView(scene), count(0)
    { }

    int count;

protected:
    void paintEvent(QPaintEvent *event)
    {
        ++count;
        QGraphicsView::paintEvent(event);
    };
};

void tst_QGraphicsView::imageRiver_data()
{
    QTest::addColumn<int>("direction");
    QTest::addColumn<bool>("rotation");
    QTest::addColumn<bool>("scale");
    QTest::newRow("horizontal") << 0 << false << false;
    QTest::newRow("vertical") << 1 << false << false;
    QTest::newRow("both") << 2 << false << false;
    QTest::newRow("horizontal rot") << 0 << true << false;
    QTest::newRow("horizontal scale") << 0 << false << true;
    QTest::newRow("horizontal rot + scale") << 0 << true << true;
}

void tst_QGraphicsView::imageRiver()
{
    QFETCH(int, direction);
    QFETCH(bool, rotation);
    QFETCH(bool, scale);

    QGraphicsScene scene(0, 0, 300, 300);

    CountPaintEventView view(&scene);
    view.resize(300, 300);
    view.setFrameStyle(0);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();

    QPixmap pix(":/images/designer.png");
    QVERIFY(!pix.isNull());   

    QList<QGraphicsItem *> items;
    QFile file(":/random.data");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream str(&file);
    for (int i = 0; i < 100; ++i) {
	AnimatedPixmapItem *item;
	if (direction == 0) item = new AnimatedPixmapItem((i % 4) + 1, 0, rotation, scale);
	if (direction == 1) item = new AnimatedPixmapItem(0, (i % 4) + 1, rotation, scale);
	if (direction == 2) item = new AnimatedPixmapItem((i % 4) + 1, (i % 4) + 1, rotation, scale);
	item->setPixmap(pix);
        int rnd1, rnd2;
        str >> rnd1 >> rnd2;
	item->setPos(-pix.width() + rnd1 % (view.width() + pix.width()),
		     -pix.height() + rnd2 % (view.height() + pix.height()));
	scene.addItem(item);
    }

    view.count = 0;

    QBENCHMARK {
#ifdef CALLGRIND_DEBUG
	CALLGRIND_START_INSTRUMENTATION
#endif
	for (int i = 0; i < 100; ++i) {
	    scene.advance();
	    while (view.count < (i+1))
		qApp->processEvents();
	}
#ifdef CALLGRIND_DEBUG
	CALLGRIND_STOP_INSTRUMENTATION
#endif
    }
}

class AnimatedTextItem : public QGraphicsSimpleTextItem
{
public:
    AnimatedTextItem(int x, int y, bool rot, bool scal, QGraphicsItem *parent = 0)
        : QGraphicsSimpleTextItem(parent), rotateFactor(0), scaleFactor(25)
    {
	setText("River of text");
	rotate = rot;
	scale = scal;
	xspeed = x;
	yspeed = y;
    }

protected:
    void advance(int i)
    { 
	if (!i)
	    return;
	QRect r = boundingRect().toRect();
	int x = int(pos().x()) + r.width();
	x += xspeed;
	x = (x % (300 + r.width() * 2)) - r.width();
	int y = int(pos().y()) + r.width();
	y += yspeed;
	y = (y % (300 + r.width() * 2)) - r.width();
	setPos(x, y);

	int rot = rotateFactor;
	int sca = scaleFactor;
        if (rotate)
	    rotateFactor = 1 + (rot + xspeed) % 360;
        if (scale)
	    scaleFactor = 1 + (sca + yspeed) % 50;

	if (rotate || scale) {
	    qreal s = 0.5 + scaleFactor / 50.0;
	    setTransform(QTransform().rotate(rotateFactor).scale(s, s));
	}
    }

private:
    int xspeed;
    int yspeed;
    int rotateFactor;
    int scaleFactor;
    bool rotate;
    bool scale;
};

void tst_QGraphicsView::textRiver_data()
{
    QTest::addColumn<int>("direction");
    QTest::addColumn<bool>("rotation");
    QTest::addColumn<bool>("scale");
    QTest::newRow("horizontal") << 0 << false << false;
    QTest::newRow("vertical") << 1 << false << false;
    QTest::newRow("both") << 2 << false << false;
    QTest::newRow("horizontal rot") << 0 << true << false;
    QTest::newRow("horizontal scale") << 0 << false << true;
    QTest::newRow("horizontal rot + scale") << 0 << true << true;
}

void tst_QGraphicsView::textRiver()
{
    QFETCH(int, direction);
    QFETCH(bool, rotation);
    QFETCH(bool, scale);

    QGraphicsScene scene(0, 0, 300, 300);

    CountPaintEventView view(&scene);
    view.resize(300, 300);
    view.setFrameStyle(0);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();

    QPixmap pix(":/images/designer.png");
    QVERIFY(!pix.isNull());   

    QList<QGraphicsItem *> items;
    QFile file(":/random.data");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream str(&file);
    for (int i = 0; i < 100; ++i) {
	AnimatedTextItem *item;
	if (direction == 0) item = new AnimatedTextItem((i % 4) + 1, 0, rotation, scale);
	if (direction == 1) item = new AnimatedTextItem(0, (i % 4) + 1, rotation, scale);
	if (direction == 2) item = new AnimatedTextItem((i % 4) + 1, (i % 4) + 1, rotation, scale);
        int rnd1, rnd2;
        str >> rnd1 >> rnd2;
	item->setPos(-pix.width() + rnd1 % (view.width() + pix.width()),
		     -pix.height() + rnd2 % (view.height() + pix.height()));
	scene.addItem(item);
    }

    view.count = 0;

    QBENCHMARK {
#ifdef CALLGRIND_DEBUG
	CALLGRIND_START_INSTRUMENTATION
#endif
	for (int i = 0; i < 100; ++i) {
	    scene.advance();
	    while (view.count < (i+1))
		qApp->processEvents();
	}
#ifdef CALLGRIND_DEBUG
	CALLGRIND_STOP_INSTRUMENTATION
#endif
    }
}

class AnimatedPixmapCacheItem : public QGraphicsPixmapItem
{
public:
    AnimatedPixmapCacheItem(int x, int y, QGraphicsItem *parent = 0)
        : QGraphicsPixmapItem(parent)
    {
        xspeed = x;
        yspeed = y;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        QGraphicsPixmapItem::paint(painter,option,widget);
        //We just want to wait, and we don't want to process the event loop with qWait
        QTest::qSleep(3);
    }
protected:
    void advance(int i)
    {
        if (!i)
            return;
        int x = int(pos().x()) + pixmap().width();
        x += xspeed;
        x = (x % (300 + pixmap().width() * 2)) - pixmap().width();
        int y = int(pos().y()) + pixmap().width();
        y += yspeed;
        y = (y % (300 + pixmap().width() * 2)) - pixmap().width();
        setPos(x, y);
    }

private:
    int xspeed;
    int yspeed;
};

void tst_QGraphicsView::moveItemCache_data()
{
    QTest::addColumn<int>("direction");
    QTest::addColumn<bool>("rotation");
    QTest::addColumn<int>("cacheMode");
    QTest::newRow("Horizontal movement : ItemCoordinate Cache") << 0 << false << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Horizontal movement : DeviceCoordinate Cache") << 0 << false << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Horizontal movement : No Cache") << 0 << false << (int)QGraphicsItem::NoCache;
    QTest::newRow("Vertical +  Horizontal movement : ItemCoordinate Cache") << 2 << false <<  (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Vertical +  Horizontal movement : DeviceCoordinate Cache") << 2 << false <<  (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Vertical +  Horizontal movement : No Cache") << 2 << false << (int)QGraphicsItem::NoCache;
    QTest::newRow("Horizontal movement + Rotation : ItemCoordinate Cache") << 0 << true << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Horizontal movement + Rotation : DeviceCoordinate Cache") << 0 << true << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Horizontal movement + Rotation : No Cache") << 0 << true << (int)QGraphicsItem::NoCache;
}

void tst_QGraphicsView::moveItemCache()
{
    QFETCH(int, direction);
    QFETCH(bool, rotation);
    QFETCH(int, cacheMode);

    QGraphicsScene scene(0, 0, 300, 300);

    CountPaintEventView view(&scene);
    view.resize(600, 600);
    view.setFrameStyle(0);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();

    QPixmap pix(":/images/wine.jpeg");
    QVERIFY(!pix.isNull());

    QList<QGraphicsItem *> items;
    QFile file(":/random.data");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream str(&file);
    for (int i = 0; i < 50; ++i) {
        AnimatedPixmapCacheItem *item;
        if (direction == 0) item = new AnimatedPixmapCacheItem((i % 4) + 1, 0);
        if (direction == 1) item = new AnimatedPixmapCacheItem(0, (i % 4) + 1);
        if (direction == 2) item = new AnimatedPixmapCacheItem((i % 4) + 1, (i % 4) + 1);
        item->setPixmap(pix);
        item->setCacheMode((QGraphicsItem::CacheMode)cacheMode);
        if (rotation)
            item->setTransform(QTransform().rotate(45));
        int rnd1, rnd2;
        str >> rnd1 >> rnd2;
        item->setPos(-pix.width() + rnd1 % (view.width() + pix.width()),
                     -pix.height() + rnd2 % (view.height() + pix.height()));
        scene.addItem(item);
    }

    view.count = 0;

    QBENCHMARK {
#ifdef CALLGRIND_DEBUG
        CALLGRIND_START_INSTRUMENTATION
#endif
        for (int i = 0; i < 100; ++i) {
            scene.advance();
            while (view.count < (i+1))
                qApp->processEvents();
        }
#ifdef CALLGRIND_DEBUG
        CALLGRIND_STOP_INSTRUMENTATION
#endif
    }
}

class UpdatedPixmapCacheItem : public QGraphicsPixmapItem
{
public:
    UpdatedPixmapCacheItem(bool partial, QGraphicsItem *parent = 0)
        : QGraphicsPixmapItem(parent), partial(partial)
    {
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        QGraphicsPixmapItem::paint(painter,option,widget);
    }
protected:
    void advance(int i)
    {
        if (partial)
            update(QRectF(boundingRect().center().x(), boundingRect().center().x(), 30, 30));
        else
            update();
    }

private:
    bool partial;
};

void tst_QGraphicsView::paintItemCache_data()
{
    QTest::addColumn<bool>("updatePartial");
    QTest::addColumn<bool>("rotation");
    QTest::addColumn<int>("cacheMode");
    QTest::newRow("Partial Update : ItemCoordinate Cache") << true << false << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Partial Update : DeviceCoordinate Cache") << true << false << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Partial Update : No Cache") << true << false << (int)QGraphicsItem::NoCache;
    QTest::newRow("Full Update : ItemCoordinate Cache") << false << false << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Full Update : DeviceCoordinate Cache") << false << false << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Full Update : No Cache") << false << false << (int)QGraphicsItem::NoCache;
    QTest::newRow("Partial Update : ItemCoordinate Cache item rotated") << true << true << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Partial Update : DeviceCoordinate Cache item rotated") << true << true << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Partial Update : No Cache item rotated") << true << true << (int)QGraphicsItem::NoCache;
    QTest::newRow("Full Update : ItemCoordinate Cache item rotated") << false  << true << (int)QGraphicsItem::ItemCoordinateCache;
    QTest::newRow("Full Update : DeviceCoordinate Cache item rotated") << false << true << (int)QGraphicsItem::DeviceCoordinateCache;
    QTest::newRow("Full Update : No Cache item rotated") << false << true <<(int)QGraphicsItem::NoCache;
}

void tst_QGraphicsView::paintItemCache()
{
    QFETCH(bool, updatePartial);
    QFETCH(bool, rotation);
    QFETCH(int, cacheMode);

    QGraphicsScene scene(0, 0, 300, 300);

    CountPaintEventView view(&scene);
    view.resize(600, 600);
    view.setFrameStyle(0);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();

    QPixmap pix(":/images/wine.jpeg");
    QVERIFY(!pix.isNull());

    QList<QGraphicsItem *> items;
    QFile file(":/random.data");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QDataStream str(&file);
    UpdatedPixmapCacheItem *item = new UpdatedPixmapCacheItem(updatePartial);
    item->setPixmap(pix);
    item->setCacheMode((QGraphicsItem::CacheMode)cacheMode);
    if (rotation)
        item->setTransform(QTransform().rotate(45));
    item->setPos(-100, -100);
    scene.addItem(item);

    QPixmap pix2(":/images/wine-big.jpeg");
    item = new UpdatedPixmapCacheItem(updatePartial);
    item->setPixmap(pix2);
    item->setCacheMode((QGraphicsItem::CacheMode)cacheMode);
    if (rotation)
        item->setTransform(QTransform().rotate(45));
    item->setPos(0, 0);
    scene.addItem(item);

    view.count = 0;

    QBENCHMARK {
#ifdef CALLGRIND_DEBUG
        CALLGRIND_START_INSTRUMENTATION
#endif
        for (int i = 0; i < 50; ++i) {
            scene.advance();
            while (view.count < (i+1))
                qApp->processEvents();
        }
#ifdef CALLGRIND_DEBUG
        CALLGRIND_STOP_INSTRUMENTATION
#endif
    }
}

QTEST_MAIN(tst_QGraphicsView)
#include "tst_qgraphicsview.moc"
