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

#include <qapplication.h>
#include <q3listview.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ListViewItemIterator : public QObject
{
    Q_OBJECT

public:
    tst_Q3ListViewItemIterator();
    virtual ~tst_Q3ListViewItemIterator();


public slots:
    void init();
    void cleanup();
private slots:
    void copy_and_assignment();
    void operator_plus_plus_data();
    void operator_plus_plus();
    void operator_minus_minus_data();
    void operator_minus_minus();
    void operator_plus_equals_data();
    void operator_plus_equals();
    void operator_minus_equals_data();
    void operator_minus_equals();

private:
    Q3ListView *testWidget;
    QList<Q3CheckListItem*> testItems;

    void prepareItem( Q3CheckListItem *item, int flags );
};

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

tst_Q3ListViewItemIterator::tst_Q3ListViewItemIterator()
{

}

tst_Q3ListViewItemIterator::~tst_Q3ListViewItemIterator()
{

}

void tst_Q3ListViewItemIterator::copy_and_assignment()
{
    // sets up every second item to be selected (item 1, 3, 5 etc.)
    for (int i=0; i<testItems.count(); ++i ) {
        if (i%2) {
	    prepareItem(testItems.at( i ), Q3ListViewItemIterator::Selected);
        } else {
            prepareItem(testItems.at( i ), 0);
        }
    }

    // create iterator, copy it with copy constructor and assignment operator
    Q3ListViewItemIterator it(testWidget, Q3ListViewItemIterator::Selected);
    Q3ListViewItemIterator copied(it);
    Q3ListViewItemIterator assigned = it;

    // we expect at least one valid current item
    QVERIFY(it.current());
    QVERIFY(copied.current());
    QVERIFY(assigned.current());

    // check that all iterators point to the same current item
    while (it.current()) {
        QVERIFY(it.current() == copied.current());
        QVERIFY(it.current() == assigned.current());
	++it;
        ++copied;
        ++assigned;
    }
}

void tst_Q3ListViewItemIterator::operator_plus_plus_data()
{
    QTest::addColumn<IntList>("itemFlags");
    QTest::addColumn<int>("iteratorFlags");
    QTest::addColumn<QStringList>("expectedItems");
    {
	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Selected;

	QStringList expectedItems;
	expectedItems.append( "item1" );
	expectedItems.append( "item3" );
	expectedItems.append( "item4" );

	QTest::newRow( "Selected 01" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Selected;

	QStringList expectedItems;
	expectedItems.append( "item2" );
	expectedItems.append( "item4" );

	QTest::newRow( "Selected 02" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Unselected;

	QStringList expectedItems;
	expectedItems.append( "item3" );
	expectedItems.append( "item5" );
	expectedItems.append( "item6" );
	expectedItems.append( "item7" );
	expectedItems.append( "item8" );
	expectedItems.append( "item9" );
	expectedItems.append( "item10" );
	QTest::newRow( "Unselected" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;

	int flags = Q3ListViewItemIterator::Visible |
		    Q3ListViewItemIterator::Unselected |
		    Q3ListViewItemIterator::Selectable |
		    Q3ListViewItemIterator::DragDisabled |
		    Q3ListViewItemIterator::DropDisabled |
		    Q3ListViewItemIterator::NotExpandable |
		    Q3ListViewItemIterator::NotChecked;

	QStringList expectedItems;
	expectedItems.append( "item1" );
	expectedItems.append( "item2" );
	expectedItems.append( "item3" );
	expectedItems.append( "item4" );
	expectedItems.append( "item5" );
	expectedItems.append( "item6" );
	expectedItems.append( "item7" );
	expectedItems.append( "item8" );
	expectedItems.append( "item9" );
	expectedItems.append( "item10" );
	QTest::newRow( "All default settings" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;

	int flags = 0;

	QStringList expectedItems;
	expectedItems.append( "item1" );
	expectedItems.append( "item2" );
	expectedItems.append( "item3" );
	expectedItems.append( "item4" );
	expectedItems.append( "item5" );
	expectedItems.append( "item6" );
	expectedItems.append( "item7" );
	expectedItems.append( "item8" );
	expectedItems.append( "item9" );
	expectedItems.append( "item10" );
	QTest::newRow( "Normal iterator (no flags)" ) << itemFlags << flags << expectedItems;
    }
    {
	int flags = Q3ListViewItemIterator::Selected |
		    Q3ListViewItemIterator::Checked;

	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( flags );
	itemFlags.append( flags );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	QStringList expectedItems;
	expectedItems.append( "item3" );
	expectedItems.append( "item4" );
	QTest::newRow( "Selected | Checked" ) << itemFlags << flags << expectedItems;
    }
    {
	int flags = Q3ListViewItemIterator::Selected |
		    Q3ListViewItemIterator::Unselected;

	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( flags );
	itemFlags.append( flags );
	itemFlags.append( Q3ListViewItemIterator::Unselected );
	itemFlags.append( Q3ListViewItemIterator::Unselected );
	itemFlags.append( flags );

	QStringList expectedItems;
	QTest::newRow( "Selected | Unselected" ) << itemFlags << flags << expectedItems;
    }
}

void tst_Q3ListViewItemIterator::operator_minus_minus_data()
{
    QTest::addColumn<IntList>("itemFlags");
    QTest::addColumn<int>("iteratorFlags");
    QTest::addColumn<QStringList>("expectedItems");
    {
	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Selected;

	QStringList expectedItems;
	expectedItems.append( "item4" );
	expectedItems.append( "item3" );
	expectedItems.append( "item1" );

	QTest::newRow( "Selected 01" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Checked );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected | Q3ListViewItemIterator::NotChecked );

	int flags = Q3ListViewItemIterator::Selected |
		    Q3ListViewItemIterator::Checked;

	QStringList expectedItems;
	QTest::newRow( "Selected | Checked" ) << itemFlags << flags << expectedItems;
    }
    {
	IntList itemFlags;

	int flags = 0;

	QStringList expectedItems;
	expectedItems.append( "item10" );
	expectedItems.append( "item9" );
	expectedItems.append( "item8" );
	expectedItems.append( "item7" );
	expectedItems.append( "item6" );
	expectedItems.append( "item5" );
	expectedItems.append( "item4" );
	expectedItems.append( "item3" );
	expectedItems.append( "item2" );
	expectedItems.append( "item1" );
	QTest::newRow( "Normal iterator (no flags)" ) << itemFlags << flags << expectedItems;
    }
}

void tst_Q3ListViewItemIterator::operator_plus_equals_data()
{
    QTest::addColumn<IntList>("itemFlags");
    QTest::addColumn<int>("iteratorFlags");
    QTest::addColumn<QStringList>("expectedItems");
    QTest::addColumn<int>("stepSize");
    {
	IntList itemFlags;
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Selected;

	QStringList expectedItems;
	expectedItems.append( "item1" );
	expectedItems.append( "item3" );

	QTest::newRow( "Selected 01" ) << itemFlags << flags << expectedItems << 2;
    }
}

void tst_Q3ListViewItemIterator::operator_minus_equals_data()
{
    QTest::addColumn<IntList>("itemFlags");
    QTest::addColumn<int>("iteratorFlags");
    QTest::addColumn<QStringList>("expectedItems");
    QTest::addColumn<int>("stepSize");
    {
	IntList itemFlags;
    	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( 0 );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( Q3ListViewItemIterator::Selected );
	itemFlags.append( 0 );
	itemFlags.append( Q3ListViewItemIterator::Selected );

	int flags = Q3ListViewItemIterator::Selected;

	QStringList expectedItems;
	expectedItems.append( "item10" );
	expectedItems.append( "item7" );
	expectedItems.append( "item1" );

	QTest::newRow( "Selected 01" ) << itemFlags << flags << expectedItems << 2;
    }
}

void tst_Q3ListViewItemIterator::init()
{
    testWidget = new Q3ListView();
    testWidget->addColumn( "Testing" );
    testWidget->setRootIsDecorated( TRUE );
    testWidget->setSorting( -1 );

    testItems.clear();

    // this listviewitem tree should become more complex in later tests
    for ( int i =10; i>0; --i) {
	Q3CheckListItem *item = new Q3CheckListItem( testWidget, "item" + QString::number( i ), Q3CheckListItem::CheckBox );
	testItems.insert( 0, item );
    }
}

void tst_Q3ListViewItemIterator::cleanup()
{
    if ( testWidget )
	delete testWidget;
    testWidget = 0;
}


void tst_Q3ListViewItemIterator::operator_plus_plus()
{
    QFETCH( IntList, itemFlags );
    QFETCH( int, iteratorFlags );
    QFETCH( QStringList, expectedItems );

    // sets up the items
    for (int i=0; i<testItems.count(); ++i ) {
	if ( itemFlags.count() > i )
	    prepareItem( testItems.at( i ), itemFlags[ i ] );
    }

    // gets the iterator
    Q3ListViewItemIterator it( testWidget );
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( testWidget, iteratorFlags );

    // iterates over them and compares with expectedItems
    QStringList::iterator expected = expectedItems.begin();
    if ( it.current() == 0 ) {
	QVERIFY( expectedItems.empty() );
    } else {
	while ( it.current() ) {
	    QCOMPARE( it.current()->text( 0 ), *expected );
	    it++;
	    expected++;
	}
    }
}

void tst_Q3ListViewItemIterator::operator_minus_minus()
{
    QFETCH( IntList, itemFlags );
    QFETCH( int, iteratorFlags );
    QFETCH( QStringList, expectedItems );

    // sets up the items
    for (int i=0; i<testItems.count(); ++i ) {
	if ( itemFlags.count() > i )
	    prepareItem( testItems.at( i ), itemFlags[ i ] );
    }

    // gets the iterator
    Q3ListViewItemIterator it( testWidget );
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( testWidget, iteratorFlags );

    // traverse to the last valid item
    Q3ListViewItem *item = 0;
    while ( it.current() ) {
	item = it.current();
	++it;
    }
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( item, iteratorFlags );
    else
	it = Q3ListViewItemIterator( item );

    // iterates over them and compares with expectedItems
    QStringList::iterator expected = expectedItems.begin();
    if ( it.current() == 0 ) {
	QVERIFY( expectedItems.empty() );
    } else {
	while ( it.current() ) {
	    QCOMPARE( it.current()->text( 0 ), *expected );
	    it--;
	    expected++;
	}
    }
}

void tst_Q3ListViewItemIterator::operator_plus_equals()
{
    QFETCH( IntList, itemFlags );
    QFETCH( int, iteratorFlags );
    QFETCH( QStringList, expectedItems );
    QFETCH( int, stepSize );

    // sets up the items
    for (int i=0; i<testItems.count(); ++i ) {
	if ( itemFlags.count() > i )
	    prepareItem( testItems.at( i ), itemFlags[ i ] );
    }

    // gets the iterator
    Q3ListViewItemIterator it( testWidget );
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( testWidget, iteratorFlags );

    // iterates over them and compares with expectedItems
    QStringList::iterator expected = expectedItems.begin();
    if ( it.current() == 0 ) {
	QVERIFY( expectedItems.empty() );
    } else {
	while ( it.current() ) {
	    QCOMPARE( it.current()->text( 0 ), *expected );
	    it += stepSize;
	    expected++;
	}
    }
}

void tst_Q3ListViewItemIterator::operator_minus_equals()
{
    QFETCH( IntList, itemFlags );
    QFETCH( int, iteratorFlags );
    QFETCH( QStringList, expectedItems );
    QFETCH( int, stepSize );

    // sets up the items
    for (int i=0; i<testItems.count(); ++i ) {
	if ( itemFlags.count() > i )
	    prepareItem( testItems.at( i ), itemFlags[ i ] );
    }

    // gets the iterator
    Q3ListViewItemIterator it( testWidget );
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( testWidget, iteratorFlags );

    // traverse to the last valid item
    Q3ListViewItem *item = 0;
    while ( it.current() ) {
	item = it.current();
	++it;
    }
    if ( iteratorFlags != 0 )
	it = Q3ListViewItemIterator( item, iteratorFlags );
    else
	it = Q3ListViewItemIterator( item );

    // iterates over them and compares with expectedItems
    QStringList::iterator expected = expectedItems.begin();
    if ( it.current() == 0 ) {
	QVERIFY( expectedItems.empty() );
    } else {
	while ( it.current() ) {
	    QCOMPARE( it.current()->text( 0 ), *expected );
	    it -= stepSize;
	    expected++;
	}
    }
}

void tst_Q3ListViewItemIterator::prepareItem( Q3CheckListItem *item, int flags )
{
    if ( flags & Q3ListViewItemIterator::Visible )
	item->setVisible( TRUE );
    if ( flags & Q3ListViewItemIterator::Invisible )
	item->setVisible( FALSE );
    if ( flags & Q3ListViewItemIterator::Selected )
	item->setSelected( TRUE );
    if ( flags & Q3ListViewItemIterator::Unselected )
	item->setSelected( FALSE );
    if ( flags & Q3ListViewItemIterator::Selectable )
	item->setSelectable( TRUE );
    if ( flags & Q3ListViewItemIterator::NotSelectable )
	item->setSelectable( FALSE );
    if ( flags & Q3ListViewItemIterator::DragEnabled )
	item->setDragEnabled( TRUE );
    if ( flags & Q3ListViewItemIterator::DragDisabled )
	item->setDragEnabled( FALSE );
    if ( flags & Q3ListViewItemIterator::DropEnabled )
	item->setDropEnabled( TRUE );
    if ( flags & Q3ListViewItemIterator::DropDisabled )
	item->setDropEnabled( FALSE );
    if ( flags & Q3ListViewItemIterator::Expandable )
	item->setExpandable( TRUE );
    if ( flags & Q3ListViewItemIterator::NotExpandable )
	item->setExpandable( FALSE );
    if ( flags & Q3ListViewItemIterator::Checked )
	item->setOn( TRUE );
    if ( flags & Q3ListViewItemIterator::NotChecked )
	item->setOn( FALSE );
}



QTEST_MAIN(tst_Q3ListViewItemIterator)
#include "tst_q3listviewitemiterator.moc"

