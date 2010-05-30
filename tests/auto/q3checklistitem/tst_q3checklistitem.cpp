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
#include <qmap.h>
#include <qapplication.h>
#include <q3listview.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3CheckListItem : public QObject
{
    Q_OBJECT

public:
    tst_Q3CheckListItem();
    virtual ~tst_Q3CheckListItem();

private slots:
    void setState_data();
    void setState();
};

tst_Q3CheckListItem::tst_Q3CheckListItem()
{

}

tst_Q3CheckListItem::~tst_Q3CheckListItem()
{

}

typedef QMap<QString, Q3CheckListItem::ToggleState> StateMap;

QDataStream &operator>>( QDataStream &s, Q3CheckListItem::ToggleState &state )
{
    int tmp;
    s >> tmp;
    state = (Q3CheckListItem::ToggleState)tmp;
    return s;
}
Q_DECLARE_METATYPE(StateMap)


void tst_Q3CheckListItem::setState_data()
{
    QTest::addColumn<StateMap>("states");
    QTest::addColumn<StateMap>("expectedStates");
    {
	StateMap s,e;
	s.insert( "item6", Q3CheckListItem::On );

	e.insert( "item5", Q3CheckListItem::NoChange );
	e.insert( "item1", Q3CheckListItem::NoChange );
	QTest::newRow( "children_affect_parent01" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "item6", Q3CheckListItem::On );
	s.insert( "item7", Q3CheckListItem::On );
	s.insert( "item8", Q3CheckListItem::On );

	e.insert( "item5", Q3CheckListItem::On );
	e.insert( "item1", Q3CheckListItem::NoChange );
	QTest::newRow( "children_affect_parent02" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "a_item5", Q3CheckListItem::On );
	s.insert( "b_item6", Q3CheckListItem::Off );
	s.insert( "b_item7", Q3CheckListItem::Off );
	s.insert( "b_item8", Q3CheckListItem::Off );

	e.insert( "item5", Q3CheckListItem::Off );
	QTest::newRow( "children_affect_parent03" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "item2", Q3CheckListItem::On );
	s.insert( "item3", Q3CheckListItem::On );
	s.insert( "item4", Q3CheckListItem::On );
	s.insert( "item6", Q3CheckListItem::On );
	s.insert( "item7", Q3CheckListItem::On );
	s.insert( "item8", Q3CheckListItem::On );
	s.insert( "item10", Q3CheckListItem::On );
	s.insert( "item11", Q3CheckListItem::On );
	s.insert( "item12", Q3CheckListItem::On );

	e.insert( "item5", Q3CheckListItem::On );
	e.insert( "item9", Q3CheckListItem::On );
	e.insert( "item1", Q3CheckListItem::On );
	QTest::newRow( "children_affect_parent04" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "a_item6", Q3CheckListItem::On );
	s.insert( "b_item6", Q3CheckListItem::Off );

	e.insert( "item6", Q3CheckListItem::Off );
	QTest::newRow( "setting_resetting01" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "item1", Q3CheckListItem::On );

	e.insert( "item2", Q3CheckListItem::On );
	e.insert( "item3", Q3CheckListItem::On );
	e.insert( "item4", Q3CheckListItem::On );
	e.insert( "item5", Q3CheckListItem::On );
	e.insert( "item6", Q3CheckListItem::On );
	e.insert( "item7", Q3CheckListItem::On );
	e.insert( "item8", Q3CheckListItem::On );
	e.insert( "item9", Q3CheckListItem::On );
	e.insert( "item10", Q3CheckListItem::On );
	e.insert( "item11", Q3CheckListItem::On );
	e.insert( "item12", Q3CheckListItem::On );
	QTest::newRow( "parrent_affecting_children01" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "a_item11", Q3CheckListItem::On );
	s.insert( "a_item4", Q3CheckListItem::Off );
	s.insert( "a_item7", Q3CheckListItem::On );
	s.insert( "b_item1", Q3CheckListItem::On );

	e.insert( "item2", Q3CheckListItem::On );
	e.insert( "item3", Q3CheckListItem::On );
	e.insert( "item4", Q3CheckListItem::On );
	e.insert( "item5", Q3CheckListItem::On );
	e.insert( "item6", Q3CheckListItem::On );
	e.insert( "item7", Q3CheckListItem::On );
	e.insert( "item8", Q3CheckListItem::On );
	e.insert( "item9", Q3CheckListItem::On );
	e.insert( "item10", Q3CheckListItem::On );
	e.insert( "item11", Q3CheckListItem::On );
	e.insert( "item12", Q3CheckListItem::On );
	QTest::newRow( "parrent_affecting_children02" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "a_item7", Q3CheckListItem::On );
	s.insert( "b_item5", Q3CheckListItem::Off );

	e.insert( "item6", Q3CheckListItem::Off );
	e.insert( "item7", Q3CheckListItem::Off );
	e.insert( "item8", Q3CheckListItem::Off );
	QTest::newRow( "parrent_affecting_children03" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "a_item7", Q3CheckListItem::On );
	s.insert( "b_item5", Q3CheckListItem::Off );

	e.insert( "item6", Q3CheckListItem::Off );
	e.insert( "item7", Q3CheckListItem::Off );
	e.insert( "item8", Q3CheckListItem::Off );
	QTest::newRow( "parrent_affecting_children04" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "item11", Q3CheckListItem::On );

	e.insert( "item9", Q3CheckListItem::NoChange );
	QTest::newRow( "tristate_01" ) << s << e;
    }
    {
	StateMap s,e;
	s.insert( "item11", Q3CheckListItem::NoChange );

	e.insert( "item11", Q3CheckListItem::Off );
	QTest::newRow( "tristate_02" ) << s << e;
    }
    {
	StateMap s,e;
	// set children
	s.insert( "a_item10", Q3CheckListItem::Off );
	s.insert( "a_item11", Q3CheckListItem::On );
	s.insert( "a_item12", Q3CheckListItem::On );
	// set parent to On (but also saving old state)
	s.insert( "b_item9", Q3CheckListItem::On );
	// recalling old state
	s.insert( "c_item9", Q3CheckListItem::NoChange );

	e.insert( "item9", Q3CheckListItem::NoChange );
	e.insert( "item10", Q3CheckListItem::Off );
	e.insert( "item11", Q3CheckListItem::On );
	e.insert( "item12", Q3CheckListItem::On );
	QTest::newRow( "tristate_03" ) << s << e;
    }
    {
	StateMap s,e;
	// set children
	s.insert( "a_item10", Q3CheckListItem::Off );
	s.insert( "a_item11", Q3CheckListItem::On );
	s.insert( "a_item12", Q3CheckListItem::On );
	// set parent to On (but also saving old state)
	s.insert( "b_item9", Q3CheckListItem::On );
	// recalling old state
	s.insert( "c_item9", Q3CheckListItem::NoChange );
	// setting the last child to on, which should also set the history for item9 to all on
	s.insert( "d_item10", Q3CheckListItem::On );
	// recalling old state which should all be On for all children, in effect setting item9 to On as well
	s.insert( "e_item9", Q3CheckListItem::NoChange );

       	e.insert( "item9", Q3CheckListItem::On );
	e.insert( "item10", Q3CheckListItem::On );
	e.insert( "item11", Q3CheckListItem::On );
	e.insert( "item12", Q3CheckListItem::On );
	QTest::newRow( "tristate_04" ) << s << e;
    }
    {
	StateMap s,e;
	// set children
	s.insert( "a_item10", Q3CheckListItem::Off );
	s.insert( "a_item11", Q3CheckListItem::On );
	s.insert( "a_item12", Q3CheckListItem::On );
	// set parent to On (but also saving old state)
	s.insert( "b_item9", Q3CheckListItem::On );
	// recalling old state
	s.insert( "c_item9", Q3CheckListItem::NoChange );
	// setting the last two children to Off, which should also set the history for item9 to all Off
	s.insert( "d_item11", Q3CheckListItem::Off );
	s.insert( "d_item12", Q3CheckListItem::Off );
	// recalling old state which should all be Off for all children, in effect setting item9 to Off
	s.insert( "e_item9", Q3CheckListItem::NoChange );

	e.insert( "item9", Q3CheckListItem::Off );
	e.insert( "item10", Q3CheckListItem::Off );
	e.insert( "item11", Q3CheckListItem::Off );
	e.insert( "item12", Q3CheckListItem::Off );
	QTest::newRow( "tristate_05" ) << s << e;
    }
    {
	StateMap s,e;
	// set children
	s.insert( "a_item2", Q3CheckListItem::On );
	s.insert( "a_item3", Q3CheckListItem::On );
	s.insert( "a_item4", Q3CheckListItem::On );
	s.insert( "a_item6", Q3CheckListItem::On );
	s.insert( "a_item7", Q3CheckListItem::On );
	s.insert( "a_item8", Q3CheckListItem::On );
	// set item1 to On (storing previous state)
	s.insert( "b_item1", Q3CheckListItem::On );
	// bring back old state
	s.insert( "c_item1", Q3CheckListItem::NoChange );
	// set item9 (and it's children) to On, wich also saves new history for the whole tree to On
	s.insert( "d_item9", Q3CheckListItem::On );
	// bring back old state once again, all should be On now
	s.insert( "e_item1", Q3CheckListItem::NoChange );

	e.insert( "item1", Q3CheckListItem::On );
	e.insert( "item2", Q3CheckListItem::On );
	e.insert( "item3", Q3CheckListItem::On );
	e.insert( "item4", Q3CheckListItem::On );
	e.insert( "item5", Q3CheckListItem::On );
	e.insert( "item6", Q3CheckListItem::On );
	e.insert( "item7", Q3CheckListItem::On );
	e.insert( "item8", Q3CheckListItem::On );
	e.insert( "item9", Q3CheckListItem::On );
	e.insert( "item10", Q3CheckListItem::On );
	e.insert( "item11", Q3CheckListItem::On );
	e.insert( "item12", Q3CheckListItem::On );
	QTest::newRow( "tristate_06" ) << s << e;
    }
}

void tst_Q3CheckListItem::setState()
{
    // makes the listview
    Q3ListView view( 0 );
    view.addColumn( "Testing" );
    view.setRootIsDecorated( TRUE );
    view.setSorting( -1 );

    Q3CheckListItem item1( &view ,"item1", Q3CheckListItem::CheckBoxController );

    Q3CheckListItem item2( &item1 ,"item2", Q3CheckListItem::CheckBox );
    Q3CheckListItem item3( &item1 ,"item3", Q3CheckListItem::CheckBox );
    Q3CheckListItem item4( &item1 ,"item4", Q3CheckListItem::CheckBox );

    Q3CheckListItem item5( &item1 ,"item5", Q3CheckListItem::CheckBoxController );

    Q3CheckListItem item6( &item5 ,"item6", Q3CheckListItem::CheckBox );
    Q3CheckListItem item7( &item5 ,"item7", Q3CheckListItem::CheckBox );
    Q3CheckListItem item8( &item5 ,"item8", Q3CheckListItem::CheckBox );

    Q3CheckListItem item9( &item1 ,"item9", Q3CheckListItem::CheckBoxController );

    Q3CheckListItem item10( &item9 ,"item10", Q3CheckListItem::CheckBox );
    Q3CheckListItem item11( &item9 ,"item11", Q3CheckListItem::CheckBox );
    Q3CheckListItem item12( &item9 ,"item12", Q3CheckListItem::CheckBox );

    view.show();

//    while ( view.isVisible() )
//	qApp->processEvents();


    // fetches the states
    QFETCH( StateMap, states );
    QFETCH( StateMap, expectedStates );

    // sets the states
    StateMap::Iterator stateIt;
    for ( stateIt = states.begin(); stateIt != states.end(); ++stateIt ) {
	QString key =  stateIt.key().section( '_', -1 );

	Q3ListViewItemIterator it( &view );
	while ( it.current() ) {
	    if ( key == it.current()->text( 0 ) && it.current()->rtti() == 1 ) {
		((Q3CheckListItem*)it.current())->setState( stateIt.data() );
		//qDebug( "setting %s to %d", key.latin1(), (int)stateIt.data() );
	    }
	    ++it;
	}
    }

    // check if we get what we expected
    Q3ListViewItemIterator it( &view );
    while ( it.current() ) {
	QString key = it.current()->text( 0 );
	if ( expectedStates.contains( key ) && it.current()->rtti() == 1 ) {
	    QCOMPARE( ((Q3CheckListItem*)it.current())->state(), expectedStates[ key ] );
	}
	++it;
    }
}

QTEST_MAIN(tst_Q3CheckListItem)
#include "tst_q3checklistitem.moc"
