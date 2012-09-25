/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDateTime>
#include <QMainWindow>
#include <QStatusBar>
#include <QMessageBox>
#include <QMenuBar>
#include <QApplication>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QLabel>
#include <QImage>
#include <QProgressDialog>
#include <QPixmap>
#include <QMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

#include "canvas.h"

// We use a global variable to save memory - all the brushes and pens in
// the mesh are shared.
static QBrush *tb = 0;
static QPen *tp = 0;

class EdgeItem;
class NodeItem;

class EdgeItem: public QGraphicsLineItem
{
public:
    EdgeItem( NodeItem*, NodeItem* );
    void setFromPoint( int x, int y ) ;
    void setToPoint( int x, int y );
    static int count() { return c; }
private:
    static int c;
};


class ImageItem: public QGraphicsRectItem
{
public:
    ImageItem( QImage img );
protected:
    void paint( QPainter *, const QStyleOptionGraphicsItem *option, QWidget *widget );
private:
    QImage image;
    QPixmap pixmap;
};


ImageItem::ImageItem( QImage img )
    : image(img)
{
    setRect(0, 0, image.width(), image.height());
    setFlag(ItemIsMovable);
#if !defined(Q_WS_QWS)
    pixmap.fromImage(image, Qt::OrderedAlphaDither);
#endif
}

void ImageItem::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget * )
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#if defined(Q_WS_QWS)
    p->drawImage( option->exposedRect, image, option->exposedRect, Qt::OrderedAlphaDither );
#else
    p->drawPixmap( option->exposedRect, pixmap, option->exposedRect );
#endif
}

class NodeItem: public QGraphicsEllipseItem
{
public:
    NodeItem();
    ~NodeItem() {}

    void addInEdge( EdgeItem *edge ) { inList.append( edge ); }
    void addOutEdge( EdgeItem *edge ) { outList.append( edge ); }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);


    //    QPoint center() { return boundingRect().center(); }
private:
    QList<EdgeItem*> inList;
    QList<EdgeItem*> outList;
};


int EdgeItem::c = 0;

EdgeItem::EdgeItem( NodeItem *from, NodeItem *to )
    : QGraphicsLineItem( )
{
    c++;
    setPen( *tp );
    from->addOutEdge( this );
    to->addInEdge( this );
    setLine( QLineF(int(from->x()), int(from->y()), int(to->x()), int(to->y()) ));
    setZValue( 127 );
    setBoundingRegionGranularity(0.05);
}

void EdgeItem::setFromPoint( int x, int y )
{
    setLine(QLineF( x,y, line().p2().x(), line().p2().y() ));
}

void EdgeItem::setToPoint( int x, int y )
{
    setLine(QLineF( line().p1().x(), line().p1().y(), x, y ));
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        EdgeItem *edge;
        foreach(edge, inList) {
            edge->setToPoint( int(x()), int(y()) );
        }

        foreach(edge, outList) {
            edge->setFromPoint( int(x()), int(y()) );
        }
    }

    return QGraphicsEllipseItem::itemChange(change, value);
}

NodeItem::NodeItem( )
    : QGraphicsEllipseItem( QRectF(-3, -3, 6, 6) )
{
    setPen( *tp );
    setBrush( *tb );
    setZValue( 128 );
    setFlag(ItemIsMovable);
}

FigureEditor::FigureEditor(
	QGraphicsScene& c, QWidget* parent,
	const char* name, Qt::WindowFlags f) :
    QGraphicsView(&c,parent)
{
    setObjectName(name);
    setWindowFlags(f);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void FigureEditor::clear()
{
    scene()->clear();
}

BouncyLogo::BouncyLogo() :
    xvel(0), yvel(0)
{
    setPixmap(QPixmap(":/trolltech/examples/graphicsview/portedcanvas/qt-trans.xpm"));
}

const int logo_rtti = 1234;

int BouncyLogo::type() const
{
    return logo_rtti;
}

QPainterPath BouncyLogo::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void BouncyLogo::initPos()
{
    initSpeed();
    int trial=1000;
    do {
	setPos(qrand()%int(scene()->width()),qrand()%int(scene()->height()));
	advance(0);
    } while (trial-- && xvel==0.0 && yvel==0.0);
}

void BouncyLogo::initSpeed()
{
    const double speed = 4.0;
    double d = (double)(qrand()%1024) / 1024.0;
    xvel = d*speed*2-speed;
    yvel = (1-d)*speed*2-speed;
}

void BouncyLogo::advance(int stage)
{
    switch ( stage ) {
      case 0: {
	double vx = xvel;
	double vy = yvel;

	if ( vx == 0.0 && vy == 0.0 ) {
	    // stopped last turn
	    initSpeed();
	    vx = xvel;
	    vy = yvel;
	}

	double nx = x() + vx;
	double ny = y() + vy;

	if ( nx < 0 || nx >= scene()->width() )
	    vx = -vx;
	if ( ny < 0 || ny >= scene()->height() )
	    vy = -vy;

	for (int bounce=0; bounce<4; bounce++) {
	    QList<QGraphicsItem *> l=scene()->collidingItems(this);
            for (QList<QGraphicsItem *>::Iterator it=l.begin(); it!=l.end(); ++it) {
                QGraphicsItem *hit = *it;
                QPainterPath advancedShape = QMatrix().translate(xvel, yvel).map(shape());
                if ( hit->type()==logo_rtti && hit->collidesWithPath(mapToItem(hit, advancedShape)) ) {
		    switch ( bounce ) {
		      case 0:
			vx = -vx;
			break;
		      case 1:
			vy = -vy;
			vx = -vx;
			break;
		      case 2:
			vx = -vx;
			break;
		      case 3:
			// Stop for this turn
			vx = 0;
			vy = 0;
			break;
		    }
		    xvel = vx;
                    yvel = vy;
		    break;
		}
	    }
        }

	if ( x()+vx < 0 || x()+vx >= scene()->width() )
	    vx = 0;
	if ( y()+vy < 0 || y()+vy >= scene()->height() )
	    vy = 0;

	xvel = vx;
        yvel = vy;
      } break;
      case 1:
        moveBy(xvel, yvel);
	break;
    }
}

static uint mainCount = 0;
static QImage *butterflyimg;
static QImage *logoimg;

Main::Main(QGraphicsScene& c, QWidget* parent, Qt::WindowFlags f) :
    QMainWindow(parent, f),
    canvas(c)
{
    editor = new FigureEditor(canvas,this);

    QMenuBar* menu = menuBar();

    QMenu* file = new QMenu("&File", menu );
    file->addAction("&Fill canvas", this, SLOT(init()), Qt::CTRL+Qt::Key_F);
    file->addAction("&Erase canvas", this, SLOT(clear()), Qt::CTRL+Qt::Key_E);
    file->addAction("&New view", this, SLOT(newView()), Qt::CTRL+Qt::Key_N);
    file->addSeparator();
    file->addAction("&Print...", this, SLOT(print()), Qt::CTRL+Qt::Key_P);
    file->addSeparator();
    file->addAction("E&xit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
    menu->addMenu(file);

    QMenu* edit = new QMenu("&Edit", menu );
    edit->addAction("Add &Circle", this, SLOT(addCircle()), Qt::ALT+Qt::Key_C);
    edit->addAction("Add &Hexagon", this, SLOT(addHexagon()), Qt::ALT+Qt::Key_H);
    edit->addAction("Add &Polygon", this, SLOT(addPolygon()), Qt::ALT+Qt::Key_P);
    edit->addAction("Add Spl&ine", this, SLOT(addSpline()), Qt::ALT+Qt::Key_I);
    edit->addAction("Add &Text", this, SLOT(addText()), Qt::ALT+Qt::Key_T);
    edit->addAction("Add &Line", this, SLOT(addLine()), Qt::ALT+Qt::Key_L);
    edit->addAction("Add &Rectangle", this, SLOT(addRectangle()), Qt::ALT+Qt::Key_R);
    edit->addAction("Add &Sprite", this, SLOT(addSprite()), Qt::ALT+Qt::Key_S);
    edit->addAction("Create &Mesh", this, SLOT(addMesh()), Qt::ALT+Qt::Key_M );
    edit->addAction("Add &Alpha-blended image", this, SLOT(addButterfly()), Qt::ALT+Qt::Key_A);
    menu->addMenu(edit);

    QMenu* view = new QMenu("&View", menu );
    view->addAction("&Enlarge", this, SLOT(enlarge()), Qt::SHIFT+Qt::CTRL+Qt::Key_Plus);
    view->addAction("Shr&ink", this, SLOT(shrink()), Qt::SHIFT+Qt::CTRL+Qt::Key_Minus);
    view->addSeparator();
    view->addAction("&Rotate clockwise", this, SLOT(rotateClockwise()), Qt::CTRL+Qt::Key_PageDown);
    view->addAction("Rotate &counterclockwise", this, SLOT(rotateCounterClockwise()), Qt::CTRL+Qt::Key_PageUp);
    view->addAction("&Zoom in", this, SLOT(zoomIn()), Qt::CTRL+Qt::Key_Plus);
    view->addAction("Zoom &out", this, SLOT(zoomOut()), Qt::CTRL+Qt::Key_Minus);
    view->addAction("Translate left", this, SLOT(moveL()), Qt::CTRL+Qt::Key_Left);
    view->addAction("Translate right", this, SLOT(moveR()), Qt::CTRL+Qt::Key_Right);
    view->addAction("Translate up", this, SLOT(moveU()), Qt::CTRL+Qt::Key_Up);
    view->addAction("Translate down", this, SLOT(moveD()), Qt::CTRL+Qt::Key_Down);
    view->addAction("&Mirror", this, SLOT(mirror()), Qt::CTRL+Qt::Key_Home);
    menu->addMenu(view);

    menu->addSeparator();

    QMenu* help = new QMenu("&Help", menu );
    help->addAction("&About", this, SLOT(help()), Qt::Key_F1);
    menu->addMenu(help);

    statusBar();

    setCentralWidget(editor);

#if !defined(Q_OS_SYMBIAN)
    printer = 0;
#endif

    init();
}

void Main::init()
{
    clear();

    static int r=24;
    qsrand(++r);

    mainCount++;
    butterflyimg = 0;
    logoimg = 0;

    int i;
    for ( i=0; i < int(canvas.width()) / 56; i++) {
	addButterfly();
    }
    for ( i=0; i < int(canvas.width()) / 85; i++) {
	addHexagon();
    }
    for ( i=0; i < int(canvas.width()) / 128; i++) {
	addLogo();
    }
}

Main::~Main()
{
#if !defined(Q_OS_SYMBIAN)
    delete printer;
#endif
    if ( !--mainCount ) {
	delete[] butterflyimg;
	butterflyimg = 0;
	delete[] logoimg;
	logoimg = 0;
    }
}

void Main::newView()
{
    // Open a new view... have it delete when closed.
    Main *m = new Main(canvas, 0); // AKr, Qt::WA_DeleteOnClose);
    m->show();
}

void Main::clear()
{
    editor->clear();
}

void Main::help()
{
    static QMessageBox* about = new QMessageBox( "Qt Canvas Example",
	    "<h3>The QCanvas classes example</h3>"
	    "<ul>"
		"<li> Press ALT-S for some sprites."
		"<li> Press ALT-C for some circles."
		"<li> Press ALT-L for some lines."
		"<li> Drag the objects around."
		"<li> Read the code!"
            "</ul>", QMessageBox::Information, 1, 0, 0, this, 0);
    about->setButtonText( 1, "Dismiss" );
    about->show();
}

void Main::aboutQt()
{
    QMessageBox::aboutQt( this, "Qt Canvas Example" );
}

void Main::enlarge()
{
    canvas.setSceneRect(0, 0, canvas.width()*4/3, canvas.height()*4/3);
}

void Main::shrink()
{
    canvas.setSceneRect(0, 0, qMax(canvas.width()*3/4, qreal(1.0)), qMax(canvas.height()*3/4, qreal(1.0)));
}

void Main::rotateClockwise()
{
    editor->rotate( 22.5 );
}

void Main::rotateCounterClockwise()
{
    editor->rotate( -22.5 );
}

void Main::zoomIn()
{
    editor->scale( 2.0, 2.0 );
}

void Main::zoomOut()
{
    editor->scale( 0.5, 0.5 );
}

void Main::mirror()
{
    editor->scale( -1, 1 );
}

void Main::moveL()
{
    editor->translate( -16, 0 );
}

void Main::moveR()
{
    editor->translate( +16, 0 );
}

void Main::moveU()
{
    editor->translate( 0, -16 );
}

void Main::moveD()
{
    editor->translate( 0, +16 );
}

void Main::print()
{
#if !defined(Q_OS_SYMBIAN)
    if ( !printer ) printer = new QPrinter;
    QPrintDialog dialog(printer, this);
    if(dialog.exec()) {
        QPainter pp(printer);
        canvas.render(&pp);
    }
#endif
}


void Main::addSprite()
{
    BouncyLogo* i = new BouncyLogo;
    canvas.addItem(i);
    i->initPos();
    i->setZValue(qrand()%256);
}

QString butterfly_fn;
QString logo_fn;


void Main::addButterfly()
{
    if ( butterfly_fn.isEmpty() )
	return;
    if ( !butterflyimg ) {
	butterflyimg = new QImage[4];
	butterflyimg[0].load( butterfly_fn );
        butterflyimg[1] = butterflyimg[0].scaled( int(butterflyimg[0].width()*0.75),
                int(butterflyimg[0].height()*0.75), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        butterflyimg[2] = butterflyimg[0].scaled( int(butterflyimg[0].width()*0.5),
                int(butterflyimg[0].height()*0.5), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        butterflyimg[3] = butterflyimg[0].scaled( int(butterflyimg[0].width()*0.25),
                int(butterflyimg[0].height()*0.25), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    QAbstractGraphicsShapeItem* i = new ImageItem(butterflyimg[qrand()%4]);
    canvas.addItem(i);
    i->setPos(qrand()%int(canvas.width()-butterflyimg->width()),
	    qrand()%int(canvas.height()-butterflyimg->height()));
    i->setZValue(qrand()%256+250);
}

void Main::addLogo()
{
    if ( logo_fn.isEmpty() )
	return;
    if ( !logoimg ) {
	logoimg = new QImage[4];
	logoimg[0].load( logo_fn );
        logoimg[1] = logoimg[0].scaled( int(logoimg[0].width()*0.75),
                int(logoimg[0].height()*0.75), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        logoimg[2] = logoimg[0].scaled( int(logoimg[0].width()*0.5),
                int(logoimg[0].height()*0.5), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        logoimg[3] = logoimg[0].scaled( int(logoimg[0].width()*0.25),
                int(logoimg[0].height()*0.25), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    QAbstractGraphicsShapeItem* i = new ImageItem(logoimg[qrand()%4]);
    canvas.addItem(i);
    i->setPos(qrand()%int(canvas.width()-logoimg->width()),
	    qrand()%int(canvas.height()-logoimg->width()));
    i->setZValue(qrand()%256+256);
}



void Main::addCircle()
{
    QAbstractGraphicsShapeItem* i = canvas.addEllipse(QRectF(0,0,50,50));
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addHexagon()
{
    const int size = int(canvas.width() / 25);
    QPolygon polygon;
    polygon << QPoint(2*size,0)
            << QPoint(size,-size*173/100)
            << QPoint(-size,-size*173/100)
            << QPoint(-2*size,0)
            << QPoint(-size,size*173/100)
            << QPoint(size,size*173/100);

    QGraphicsPolygonItem* i = canvas.addPolygon(polygon);
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addPolygon()
{
    const int size = int(canvas.width()/2);
    QPolygon polygon;
    polygon << QPoint(0,0)
            << QPoint(size,size/5)
            << QPoint(size*4/5,size)
            << QPoint(size/6,size*5/4)
            << QPoint(size*3/4,size*3/4)
            << QPoint(size*3/4,size/4);

    QGraphicsPolygonItem* i = canvas.addPolygon(polygon);
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen(Qt::NoPen);
    i->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addSpline()
{
    const int size = int(canvas.width()/6);

    QPolygon polygon;
    polygon << QPoint(0,0)
            << QPoint(size/2,0)
            << QPoint(size,size/2)
            << QPoint(size,size)
            << QPoint(size,size*3/2)
            << QPoint(size/2,size*2)
            << QPoint(0,size*2)
            << QPoint(-size/2,size*2)
            << QPoint(size/4,size*3/2)
            << QPoint(0,size)
            << QPoint(-size/4,size/2)
            << QPoint(-size/2,0);

    QPainterPath path;
    path.moveTo(polygon[0]);
    for (int i = 1; i < polygon.size(); i += 3)
        path.cubicTo(polygon[i], polygon[(i + 1) % polygon.size()], polygon[(i + 2) % polygon.size()]);

    QGraphicsPathItem* item = canvas.addPath(path);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setPen(Qt::NoPen);
    item->setBrush( QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8) );
    item->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    item->setZValue(qrand()%256);
}

void Main::addText()
{
    QGraphicsTextItem* i = canvas.addText("QCanvasText");
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPos(qrand()%int(canvas.width()),qrand()%int(canvas.height()));
    i->setZValue(qrand()%256);
}

void Main::addLine()
{
    QGraphicsLineItem* i = canvas.addLine(QLineF( qrand()%int(canvas.width()), qrand()%int(canvas.height()),
                                                  qrand()%int(canvas.width()), qrand()%int(canvas.height()) ));
    i->setFlag(QGraphicsItem::ItemIsMovable);
    i->setPen( QPen(QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8), 6) );
    i->setZValue(qrand()%256);
}

void Main::addMesh()
{
    int x0 = 0;
    int y0 = 0;

    if ( !tb ) tb = new QBrush( Qt::red );
    if ( !tp ) tp = new QPen( Qt::black );

    int nodecount = 0;

    int w = int(canvas.width());
    int h = int(canvas.height());

    const int dist = 30;
    int rows = h / dist;
    int cols = w / dist;

#ifndef QT_NO_PROGRESSDIALOG
    QProgressDialog progress("Creating mesh...", "Abort", 0, rows, this);
#endif

    canvas.update();
    
    QVector<NodeItem*> lastRow(cols);
    for ( int j = 0; j < rows; j++ ) {
	int n = j%2 ? cols-1 : cols;
	NodeItem *prev = 0;
	for ( int i = 0; i < n; i++ ) {
	    NodeItem *el = new NodeItem;
            canvas.addItem(el);
	    nodecount++;
	    int r = qrand();
	    int xrand = r %20;
	    int yrand = (r/20) %20;
	    el->setPos( xrand + x0 + i*dist + (j%2 ? dist/2 : 0 ),
                        yrand + y0 + j*dist );

	    if ( j > 0 ) {
		if ( i < cols-1 )
		    canvas.addItem(new EdgeItem( lastRow[i], el));
		if ( j%2 )
		    canvas.addItem(new EdgeItem( lastRow[i+1], el));
		else if ( i > 0 )
		    canvas.addItem(new EdgeItem( lastRow[i-1], el));
	    }
	    if ( prev ) {
		canvas.addItem(new EdgeItem( prev, el));
	    }
	    if ( i > 0 ) lastRow[i-1] = prev;
	    prev = el;
	}
	lastRow[n-1]=prev;
#ifndef QT_NO_PROGRESSDIALOG
        progress.setValue( j );
        if ( progress.wasCanceled() )
	    break;
#endif
    }
#ifndef QT_NO_PROGRESSDIALOG
    progress.setValue( rows );
#endif
    // qDebug( "%d nodes, %d edges", nodecount, EdgeItem::count() );
}

void Main::addRectangle()
{
    QAbstractGraphicsShapeItem *i = canvas.addRect( QRectF(qrand()%int(canvas.width()),
                                                          qrand()%int(canvas.height()),
                                                          canvas.width()/5,
                                                          canvas.width()/5) );
    i->setFlag(QGraphicsItem::ItemIsMovable);
    int z = qrand()%256;
    i->setBrush( QColor(z,z,z) );
    i->setPen( QPen(QColor(qrand()%32*8,qrand()%32*8,qrand()%32*8), 6) );
    i->setZValue(z);
}
