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


#include <QtTest/QtTest>

#include "q3scrollview.h"
#include <qapplication.h>
#include <qpainter.h>
#include <qlabel.h>

#include <q3scrollview.h>

//TESTED_CLASS=
//TESTED_FILES=compat/widgets/q3scrollview.h compat/widgets/q3scrollview.cpp

QT_FORWARD_DECLARE_CLASS(QTestScrollView)
Q_DECLARE_METATYPE(QTestScrollView *)
Q_DECLARE_METATYPE(QPixmap)

class tst_Q3ScrollView : public QObject
{
Q_OBJECT

public:
    tst_Q3ScrollView();
    virtual ~tst_Q3ScrollView();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void resizePolicy();
    void setResizePolicy();

private:
    QTestScrollView *testWidget;
};

// *************************************************
// *************************************************

#include <q3scrollview.h>
#include <qapplication.h>
#include <qmenubar.h>
#include <q3popupmenu.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <q3multilineedit.h>
#include <qsizegrip.h>
#include <stdlib.h>



static const int style_id	= 0x1000;
static const int lw_id		= 0x2000;
static const int mlw_id		= 0x4000;
static const int mw_id		= 0x8000;
static const int max_lw		= 16;
static const int max_mlw	= 5;
static const int max_mw		= 10;


class BigShrinker : public QFrame
{
    Q_OBJECT
public:
    BigShrinker(QWidget* parent) : QFrame(parent)
    {
	setFrameStyle(QFrame::Box | QFrame::Sunken);
	int h=35;
	int b=0;
	for (int y=0; y<2000-h; y+=h+10) {
	    QString str;
	    str.sprintf("Button %d", b++);
	    (new QPushButton(str, this))->move(y/2,y);
	}
	resize(1000,2000);
    }
};

class BigMatrix : public Q3ScrollView
{
    Q3MultiLineEdit *dragging;
public:
    BigMatrix(QWidget* parent) : Q3ScrollView(parent,"matrix", Qt::WStaticContents),
	bg("bg.ppm")
    {
	bg.load("bg.ppm");
	resizeContents(400000,300000);

	dragging = 0;
    }

    void viewportMousePressEvent(QMouseEvent* e)
    {
	int x, y;
	viewportToContents( e->x(),  e->y(), x, y );
	dragging = new Q3MultiLineEdit(viewport(),"Another");
	dragging->setText("Thanks!");
	dragging->resize(100,100);
	addChild(dragging, x, y);
	showChild(dragging);
    }

    void viewportMouseReleaseEvent(QMouseEvent*)
    {
	dragging = 0;
    }

    void viewportMouseMoveEvent(QMouseEvent* e)
    {
	if ( dragging ) {
	    int mx, my;
	    viewportToContents( e->x(),  e->y(), mx, my );
	    int cx = childX(dragging);
	    int cy = childY(dragging);
	    int w = mx - cx + 1;
	    int h = my - cy + 1;
	    QString msg;
	    msg.sprintf("at (%d,%d) %d by %d",cx,cy,w,h);
	    dragging->setText(msg);
	    dragging->resize(w,h);
	}
    }

protected:
    void drawContents(QPainter* p, int cx, int cy, int cw, int ch)
    {
	// The Background
	if ( !bg.isNull() ) {
	    int rowheight=bg.height();
	    int toprow=cy/rowheight;
	    int bottomrow=(cy+ch+rowheight-1)/rowheight;
	    int colwidth=bg.width();
	    int leftcol=cx/colwidth;
	    int rightcol=(cx+cw+colwidth-1)/colwidth;
	    for (int r=toprow; r<=bottomrow; r++) {
		int py=r*rowheight;
		for (int c=leftcol; c<=rightcol; c++) {
		    int px=c*colwidth;
		    p->drawPixmap(px, py, bg);
		}
	    }
	} else {
	    p->fillRect(cx, cy, cw, ch, QColor(240,222,208));
	}

	// The Numbers
	{
	    QFontMetrics fm=p->fontMetrics();
	    int rowheight=fm.lineSpacing();
	    int toprow=cy/rowheight;
	    int bottomrow=(cy+ch+rowheight-1)/rowheight;
	    int colwidth=fm.width("00000,000000 ")+3;
	    int leftcol=cx/colwidth;
	    int rightcol=(cx+cw+colwidth-1)/colwidth;
	    QString str;
	    for (int r=toprow; r<=bottomrow; r++) {
		int py=r*rowheight;
		for (int c=leftcol; c<=rightcol; c++) {
		    int px=c*colwidth;
		    str.sprintf("%d,%d",c,r);
		    p->drawText(px+3, py+fm.ascent(), str);
		}
	    }

/*
	    // The Big Hint
	    if (leftcol<10 && toprow<5) {
		p->setFont(QFont("Charter",30));
		p->setPen(red);
		QString text;
		text.sprintf("HINT:  Look at %d,%d",215000/colwidth,115000/rowheight);
		p->drawText(100,50,text);
	    }
*/
        }

	// The Big X
	{
	    if (cx+cw>200000 && cy+ch>100000 && cx<230000 && cy<130000) {
		// Note that some X server cannot even handle co-ordinates
		// beyond about 4000, so you might not see this.
		p->drawLine(200000,100000,229999,129999);
		p->drawLine(229999,100000,200000,129999);

		// X marks the spot!
		p->setFont(QFont("Charter",100));
		p->setPen(Qt::blue);
		p->drawText(215000-500,115000-100,1000,200,Qt::AlignCenter,"YOU WIN!!!!!");
	    }
	}
    }

private:
    QPixmap bg;
};

class ScrollViewExample : public QWidget
{
    Q_OBJECT

public:
    ScrollViewExample( int technique, QWidget* parent=0, const char* name=0) : QWidget(parent,name)
    {
/*
	QMenuBar* menubar = new QMenuBar(this);
	Q_CHECK_PTR( menubar );

	QPopupMenu* file = new QPopupMenu( menubar );
	Q_CHECK_PTR( file );
	menubar->insertItem( "&File", file );
	file->insertItem( "Quit", qApp,  SLOT(quit()) );

	vp_options = new QPopupMenu( menubar );
	Q_CHECK_PTR( vp_options );
	vp_options->setCheckable( TRUE );
	menubar->insertItem( "&ScrollView", vp_options );
	connect( vp_options, SIGNAL(activated(int)), this, SLOT(doVPMenuItem(int)) );

	vauto_id = vp_options->insertItem( "Vertical Auto" );
	vaoff_id = vp_options->insertItem( "Vertical AlwaysOff" );
	vaon_id = vp_options->insertItem( "Vertical AlwaysOn" );
	vp_options->insertSeparator();
	hauto_id = vp_options->insertItem( "Horizontal Auto" );
	haoff_id = vp_options->insertItem( "Horizontal AlwaysOff" );
	haon_id = vp_options->insertItem( "Horizontal AlwaysOn" );
	vp_options->insertSeparator();
	corn_id = vp_options->insertItem( "cornerWidget" );
*/
	if (technique == 1) {
	    vp = new Q3ScrollView(this);
	    BigShrinker *bs = new BigShrinker(0);
	    vp->addChild(bs);
	    bs->setAcceptDrops(TRUE);
	} else {
	    vp = new BigMatrix(this);
	    if ( technique == 3 )
		vp->enableClipper(TRUE);
	    srand(1);
/*
	    for (int i=0; i<30; i++) {
		QMultiLineEdit *l = new QMultiLineEdit(vp->viewport(),"First");
		l->setText("Drag out more of these.");
		l->resize(100,100);
		vp->addChild(l, rand()%800, rand()%10000);
	    }
*/
            vp->viewport()->setBackgroundMode(Qt::NoBackground);
	}

/*
	f_options = new QPopupMenu( menubar );
	Q_CHECK_PTR( f_options );
	f_options->setCheckable( TRUE );
	menubar->insertItem( "F&rame", f_options );
	connect( f_options, SIGNAL(activated(int)), this, SLOT(doFMenuItem(int)) );

	f_options->insertItem( "No Frame", style_id );
	f_options->insertItem( "Box", style_id|QFrame::Box );
	f_options->insertItem( "Panel", style_id|QFrame::Panel );
	f_options->insertItem( "WinPanel", style_id|QFrame::WinPanel );
	f_options->insertSeparator();
	f_options->insertItem( "Plain", style_id|QFrame::Plain );
	f_options->insertItem( "Raised", style_id|QFrame::Raised );
	f_laststyle = f_options->indexOf(
	    f_options->insertItem( "Sunken", style_id|QFrame::Sunken ));
	f_options->insertSeparator();

        lw_options = new QPopupMenu( menubar );
	Q_CHECK_PTR( lw_options );
	lw_options->setCheckable( TRUE );
	for (int lw = 1; lw <= max_lw; lw++) {
	    QString str;
	    str.sprintf("%d Pixels", lw);
	    lw_options->insertItem( str, lw_id | lw );
	}
	f_options->insertItem( "Line Width", lw_options );
	connect( lw_options, SIGNAL(activated(int)), this, SLOT(doFMenuItem(int)) );

        mlw_options = new QPopupMenu( menubar );
	Q_CHECK_PTR( mlw_options );
	mlw_options->setCheckable( TRUE );
	for (int mlw = 0; mlw <= max_mlw; mlw++) {
	    QString str;
	    str.sprintf("%d Pixels", mlw);
	    mlw_options->insertItem( str, mlw_id | mlw );
	}
	f_options->insertItem( "Midline Width", mlw_options );
	connect( mlw_options, SIGNAL(activated(int)), this, SLOT(doFMenuItem(int)) );

        mw_options = new QPopupMenu( menubar );
	Q_CHECK_PTR( mw_options );
	mw_options->setCheckable( TRUE );
	for (int mw = 0; mw <= max_mw; mw++) {
	    QString str;
	    str.sprintf("%d Pixels", mw);
	    mw_options->insertItem( str, mw_id | mw );
	}
	f_options->insertItem( "Margin Width", mw_options );
	connect( mw_options, SIGNAL(activated(int)), this, SLOT(doFMenuItem(int)) );

	setVPMenuItems();
	setFMenuItems();
*/
	QVBoxLayout* vbox = new QVBoxLayout(this);
/*
	vbox->setMenuBar(menubar);
	menubar->setSeparator(QMenuBar::InWindowsStyle);
*/
        vbox->addWidget(vp);
	vbox->activate();

	corner = new QSizeGrip(this);
	corner->hide();
    }

    void center( int x, int y )
    {
        vp->center( x, y );
    }

private slots:
/*
    void doVPMenuItem(int id)
    {
	if (id == vauto_id ) {
	    vp->setVScrollBarMode(Q3ScrollView::Auto);
	} else if (id == vaoff_id) {
	    vp->setVScrollBarMode(Q3ScrollView::AlwaysOff);
	} else if (id == vaon_id) {
	    vp->setVScrollBarMode(Q3ScrollView::AlwaysOn);
	} else if (id == hauto_id) {
	    vp->setHScrollBarMode(Q3ScrollView::Auto);
	} else if (id == haoff_id) {
	    vp->setHScrollBarMode(Q3ScrollView::AlwaysOff);
	} else if (id == haon_id) {
	    vp->setHScrollBarMode(Q3ScrollView::AlwaysOn);
	} else if (id == corn_id) {
	    bool corn = !vp->cornerWidget();
	    vp->setCornerWidget(corn ? corner : 0);
	} else {
	    return; // Not for us to process.
	}
	setVPMenuItems();
    }
*/

/*
    void setVPMenuItems()
    {
	Q3ScrollView::ScrollBarMode vm = vp->vScrollBarMode();
	vp_options->setItemChecked( vauto_id, vm == Q3ScrollView::Auto );
	vp_options->setItemChecked( vaoff_id, vm == Q3ScrollView::AlwaysOff );
	vp_options->setItemChecked( vaon_id, vm == Q3ScrollView::AlwaysOn );

	Q3ScrollView::ScrollBarMode hm = vp->hScrollBarMode();
	vp_options->setItemChecked( hauto_id, hm == Q3ScrollView::Auto );
	vp_options->setItemChecked( haoff_id, hm == Q3ScrollView::AlwaysOff );
	vp_options->setItemChecked( haon_id, hm == Q3ScrollView::AlwaysOn );

	vp_options->setItemChecked( corn_id, !!vp->cornerWidget() );
    }
*/

/*
    void doFMenuItem(int id)
    {
	if (id & style_id) {
	    int sty;

	    if (id == style_id) {
		sty = 0;
	    } else if (id & QFrame::MShape) {
		sty = vp->frameStyle()&QFrame::MShadow;
		sty = (sty ? sty : QFrame::Plain) | (id&QFrame::MShape);
	    } else {
		sty = vp->frameStyle()&QFrame::MShape;
		sty = (sty ? sty : QFrame::Box) | (id&QFrame::MShadow);
	    }
	    vp->setFrameStyle(sty);
	} else if (id & lw_id) {
	    vp->setLineWidth(id&~lw_id);
	} else if (id & mlw_id) {
	    vp->setMidLineWidth(id&~mlw_id);
	} else {
	    vp->setMargin(id&~mw_id);
	}

	vp->update();
	setFMenuItems();
    }
*/

/*
    void setFMenuItems()
    {
	int sty = vp->frameStyle();

	f_options->setItemChecked( style_id, !sty );

	for (int i=1; i <= f_laststyle; i++) {
	    int id = f_options->idAt(i);
	    if (id & QFrame::MShape)
		f_options->setItemChecked( id,
		    ((id&QFrame::MShape) == (sty&QFrame::MShape)) );
	    else
		f_options->setItemChecked( id,
		    ((id&QFrame::MShadow) == (sty&QFrame::MShadow)) );
	}

	for (int lw=1; lw<=max_lw; lw++)
	    lw_options->setItemChecked( lw_id|lw, vp->lineWidth() == lw );

	for (int mlw=0; mlw<=max_mlw; mlw++)
	    mlw_options->setItemChecked( mlw_id|mlw, vp->midLineWidth() == mlw );

	for (int mw=0; mw<=max_mw; mw++)
	    mw_options->setItemChecked( mw_id|mw, vp->margin() == mw );
    }
*/

private:
    Q3ScrollView* vp;
/*
    QPopupMenu* vp_options;
    QPopupMenu* f_options;
    QPopupMenu* lw_options;
    QPopupMenu* mlw_options;
    QPopupMenu* mw_options;
*/
    QSizeGrip* corner;

/*
    int vauto_id, vaoff_id, vaon_id,
	hauto_id, haoff_id, haon_id,
	corn_id;

    int f_laststyle;
*/
};

Q_DECLARE_METATYPE(ScrollViewExample *)

// *************************************************
// *************************************************

class QTestScrollView : public Q3ScrollView
{
public:
    QString scroll_text;

//private:
    void drawContents( QPainter *p, int cx, int cy, int cw, int ch )
    {
        if (scroll_text == "") {
            p->fillRect( cx, cy, cw, ch, Qt::white );

        } else {
            p->fillRect( cx, cy, cw, ch, Qt::white );

            QFont f;
            f.setFamily( "tahoma" );
            f.setPointSize( 24 );
            QFontMetrics fm(f);
            p->setFont( f );

            int height = fm.height();
            int y = 10;
            int x = 10;
            for( int i = 0; i < (int)scroll_text.length(); i++ ) {
                if ( !scroll_text[i].isMark() )
                    y += height + 5;

                p->setPen( Qt::red );
                QRect br = fm.boundingRect( scroll_text.at(i) );
                br.moveBy( x, y );
                p->drawRect( br );
                p->setPen( Qt::black );
                p->drawText( x, y, scroll_text, i, 1 );
            }
            resizeContents( 170, y + 100 );
        }
    }
};

// *************************************************
// *************************************************

tst_Q3ScrollView::tst_Q3ScrollView()
{
}

tst_Q3ScrollView::~tst_Q3ScrollView()
{

}

void tst_Q3ScrollView::initTestCase()
{
    testWidget = new QTestScrollView;
    testWidget->resize(200,200);
    qApp->setMainWidget(testWidget);
    testWidget->show();
}

void tst_Q3ScrollView::cleanupTestCase()
{
    delete testWidget;
    testWidget = 0;
}

void tst_Q3ScrollView::init()
{
    testWidget->setResizePolicy( Q3ScrollView::Default );
    testWidget->scroll_text = "";
}

void tst_Q3ScrollView::cleanup()
{
    if (QTest::currentTestFunction() == QLatin1String("center") ) {
        qApp->setMainWidget( testWidget );
        testWidget->show();
    }
}

void tst_Q3ScrollView::resizePolicy()
{
    DEPENDS_ON( "setResizePolicy");
}

void tst_Q3ScrollView::setResizePolicy()
{
    testWidget->setResizePolicy( Q3ScrollView::Default );
    QVERIFY( testWidget->resizePolicy() == Q3ScrollView::Default );

    testWidget->setResizePolicy( Q3ScrollView::Manual );
    QVERIFY( testWidget->resizePolicy() == Q3ScrollView::Manual );

    testWidget->setResizePolicy( Q3ScrollView::AutoOne );
    QVERIFY( testWidget->resizePolicy() == Q3ScrollView::AutoOne );

    testWidget->setResizePolicy( Q3ScrollView::AutoOneFit );
    QVERIFY( testWidget->resizePolicy() == Q3ScrollView::AutoOneFit );
}

QTEST_MAIN(tst_Q3ScrollView)
#include "tst_qscrollview.moc"
