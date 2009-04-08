/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "sidedecorationimpl.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qregexp.h>

/* XPM */
static char *check_data[] = {
/* width height num_colors chars_per_pixel */
"    11    12        4            1",
/* colors */
". c #939393",
"# c #dcdcdc",
"a c None",
"b c #191919",
/* pixels */
"aaaaaaaaaa#",
"aaaaaaaaabb",
"aaaaaaaabba",
"aaaaaaabbaa",
"aaaaaabbaaa",
"a#aaabbaaaa",
"ab.a.b.aaaa",
"a#bbbbaaaaa",
"aabbbaaaaaa",
"aa#b.aaaaaa",
"aaa.aaaaaaa",
"aaaaaaaaaaa"
};

/* XPM */
static char *arrow_data[] = {
/* width height num_colors chars_per_pixel */
"    11    11        4            1",
/* colors */
". c None",
"# c #b9b9b9",
"a c #8a8a8a",
"b c #0d0d0d",
/* pixels */
"...##......",
"...ab#.....",
"...abb#....",
"...abbb#...",
"...abbbb#..",
"...abbbba..",
"...abbba...",
"...abba....",
"...aba.....",
"...aa......",
"..........."
};

/* XPM */
static char *cross_data[] = {
/* width height num_colors chars_per_pixel */
"    11    11        3            1",
/* colors */
". c #cc0000",
"# c None",
"a c #fc3464",
/* pixels */
"###########",
"###########",
"########a.#",
"##a####a.##",
"##a.###.###",
"###a...a###",
"####...####",
"####...a###",
"###.a##..##",
"##a.####aa#",
"##.########"
};


SideDecorationImpl::SideDecorationImpl( QWidget* parent, const char* name, WindowFlags fl ) :
    SideDecoration( parent, name, fl ),
    checkPix( ( const char** ) check_data ),
    arrowPix( ( const char** ) arrow_data ),
    crossPix( ( const char** ) cross_data ),
    activeBullet( -1 )
{
    Q_ASSERT( layout() != 0 );
    if ( layout()->inherits("QBoxLayout") ) {
	((QBoxLayout*)layout())->setMargin( 0 );
    }
    setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding) );
    if ( globalInformation.reconfig() ) {
	versionLabel->setText( "Reconfigure Qt " + globalInformation.qtVersionStr() );
    } else {
#if defined(QSA)
	QString versionStr = globalInformation.qsaVersionStr();
	versionStr.replace( QRegExp(" Evaluation"), "" );
	versionLabel->setText( versionLabel->text().replace( "Qt", "QSA" ) + " " + versionStr );
#elif defined(EVAL)
	QString versionStr = globalInformation.qtVersionStr();
	versionStr.replace( QRegExp(" Evaluation"), "" );
	versionLabel->setText( versionLabel->text() + " " + versionStr );
#elif defined(NON_COMMERCIAL)
	QString versionStr = globalInformation.qtVersionStr();
	versionStr.replace( QRegExp(" Non-Commercial"), "" );
	versionLabel->setText( versionLabel->text() + " " + versionStr );
#elif defined(EDU)
	QString versionStr = globalInformation.qtVersionStr();
	versionStr.replace( QRegExp(" Educational"), "" );
	versionLabel->setText( versionLabel->text() + " " + versionStr );
#else
	versionLabel->setText( versionLabel->text() + " " + globalInformation.qtVersionStr() );
#endif
    }
#if defined(EVAL)
    editionLabel->setText( "Evaluation Version" );
#elif defined(NON_COMMERCIAL)
    editionLabel->setText( "Non-Commercial Edition" );
#elif defined(EDU)
    editionLabel->setText( "Educational Edition" );
#else
    editionLabel->setText( "" );
#endif
}

SideDecorationImpl::~SideDecorationImpl()
{
}

void SideDecorationImpl::wizardPages( const QPtrList<Page>& li )
{
    QBoxLayout *lay = 0;
    Q_ASSERT( layout() != 0 );
    if ( layout()->inherits("QBoxLayout") ) {
	lay = (QBoxLayout*)layout();
    } else {
	return;
    }
    QPtrList<Page> list = li;
    Page *page;
    QGrid *grid = new QGrid( 2, this );
    grid->setSpacing( 2 );
    for ( page=list.first(); page; page=list.next() ) {
	QLabel *l = new QLabel( grid );
	l->setSizePolicy( QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed) );
	bullets.append( l );
	l = new QLabel( page->shortTitle(), grid );
    }
    lay->insertWidget( -1, grid );
    lay->insertStretch( -1 );
}

void SideDecorationImpl::wizardPageShowed( int a )
{
    if ( activeBullet>=0 && (uint)activeBullet<bullets.count() ) {
	if ( a < activeBullet )
	    bullets.at(activeBullet)->clear();
	else
	    bullets.at(activeBullet)->setPixmap( checkPix );
    }
    bullets.at(a)->setPixmap( arrowPix );
    activeBullet = a;
}

void SideDecorationImpl::wizardPageFailed( int a )
{
    bullets.at(a)->setPixmap( crossPix );
}
