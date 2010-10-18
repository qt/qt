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

#include <iostream>

#include <QtTest/QtTest>


#include <q3header.h>
#include <q3listview.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qpointer.h>
#include <qvector.h>

Q_DECLARE_METATYPE(QPoint)

QT_BEGIN_NAMESPACE
template<> struct QMetaTypeId<Q3ListView::StringComparisonMode>
{ enum { Defined = 1 }; static inline int qt_metatype_id() { return QMetaType::Int; } };
QT_END_NAMESPACE


//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ListView : public QObject
{
Q_OBJECT

public:
    tst_Q3ListView();
    virtual ~tst_Q3ListView();


public slots:
    void selectionChanged() { changed++; }
    void selectionChanged( Q3ListViewItem* ) { changedItem++; }
    void spacePressed( Q3ListViewItem *item ) { pressCount++; pressedItem = item; }
    void itemRenamed(Q3ListViewItem *item, int column);
    void itemRenamed(Q3ListViewItem *item, int column, const QString &text);
    void contextMenu(Q3ListViewItem *item, const QPoint &pos, int col);
    void doubleClicked(Q3ListViewItem *item) { doubleClickCount++; pressedItem = item; }

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void getSetCheck();
    void sortchild();
    void sortchild2(); // item -> item -> 3 items
    void sortchild3(); // item -> 3 items
    void takeItem_data();
    void takeItem();
    void selections_mouseClick_data();
    void selections_mouseClick();
    void isVisible();
    void itemRenaming();
    void removeColumn();
    void contextMenuRequested_data();
    void contextMenuRequested();
    void itemActivate();
    void findItem_data();
    void findItem();
    void spacePress_data();
    void spacePress();
    void adjustColumn();
    void mouseClickEvents();
    void mouseClickEvents_data();

private:
    QPoint itemCenter( Q3ListView* view, Q3ListViewItem* item, int column = 0);
    QPoint itemBelow( Q3ListView* view, Q3ListViewItem* item );
    QString selectionName( int );
    QString buttonName( int );
    QString keyName( Qt::KeyboardModifiers );
    void resetVariables();

private:
    Q3ListView*testWidget;
    int changed, changedItem;

    int pressCount;
    int doubleClickCount;
    Q3ListViewItem *pressedItem;

    bool itemRenamedSignalOneReceived, itemRenamedSignalTwoReceived;
    Q3ListViewItem *itemRenamedOne, *itemRenamedTwo;
    int columnRenamedOne, columnRenamedTwo;
    QString textRenamed;

    bool contextMenuRequestedSignalReceived;
    Q3ListViewItem *contextMenuRequestedItem;
    QPoint contextMenuRequestedPos;
    int contextMenuRequestedCol;

};

// Testing get/set functions
void tst_Q3ListView::getSetCheck()
{
    Q3ListView obj1;
    // SelectionMode Q3ListView::selectionMode()
    // void Q3ListView::setSelectionMode(SelectionMode)
    obj1.setSelectionMode(Q3ListView::SelectionMode(0));
    QCOMPARE(Q3ListView::SelectionMode(0), obj1.selectionMode());
    obj1.setSelectionMode(Q3ListView::SelectionMode(1));
    QCOMPARE(Q3ListView::SelectionMode(1), obj1.selectionMode());

    // int Q3ListView::sortColumn()
    // void Q3ListView::setSortColumn(int)
    obj1.setSortColumn(0);
    QCOMPARE(0, obj1.sortColumn());
    obj1.setSortColumn(INT_MIN);
    QCOMPARE(INT_MIN, obj1.sortColumn());
    obj1.setSortColumn(INT_MAX);
    QCOMPARE(INT_MAX, obj1.sortColumn());

    Q3CheckListItem obj2(&obj1, "Item1", Q3CheckListItem::CheckBox);
    obj2.setTristate(true);
    // ToggleState Q3CheckListItem::state()
    // void Q3CheckListItem::setState(ToggleState)
    obj2.setState(Q3CheckListItem::ToggleState(Q3CheckListItem::Off));
    QCOMPARE(Q3CheckListItem::ToggleState(Q3CheckListItem::Off), obj2.state());
    obj2.setState(Q3CheckListItem::ToggleState(Q3CheckListItem::NoChange));
    QCOMPARE(Q3CheckListItem::ToggleState(Q3CheckListItem::NoChange), obj2.state());
    obj2.setState(Q3CheckListItem::ToggleState(Q3CheckListItem::On));
    QCOMPARE(Q3CheckListItem::ToggleState(Q3CheckListItem::On), obj2.state());
}

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)

typedef QList<void*> ItemList;
Q_DECLARE_METATYPE(ItemList)

Q_DECLARE_METATYPE(Q3ListView::SelectionMode)

tst_Q3ListView::tst_Q3ListView()
{
}

tst_Q3ListView::~tst_Q3ListView()
{

}

void tst_Q3ListView::initTestCase()
{
    // Create the test class
}

void tst_Q3ListView::cleanupTestCase()
{
}

void tst_Q3ListView::init()
{
    testWidget = new Q3ListView(0,"testObject");
    testWidget->resize(200,200);
    testWidget->show();
    pressCount = 0;
    pressedItem = 0;
    connect( testWidget, SIGNAL( spacePressed( Q3ListViewItem* ) ),
	     this, SLOT( spacePressed( Q3ListViewItem* ) ) );
    itemRenamedSignalOneReceived = FALSE;
    itemRenamedSignalTwoReceived = FALSE;
    itemRenamedOne = 0;
    itemRenamedTwo = 0;
    columnRenamedOne = -1;
    columnRenamedTwo = -1;
    textRenamed = QString();

    contextMenuRequestedSignalReceived = FALSE;
    contextMenuRequestedItem = 0;
    contextMenuRequestedPos = QPoint();
    contextMenuRequestedCol = -1;
}

void tst_Q3ListView::cleanup()
{
    delete testWidget;
    testWidget = 0;
}

void tst_Q3ListView::sortchild()
{
    Q3ListView* listview = new Q3ListView( 0 );

    listview->addColumn( "" );

    Q3ListViewItem* item1 = new Q3ListViewItem( listview, "zzz" );
    Q3ListViewItem* item2 = new Q3ListViewItem( listview, "hhh" );
    Q3ListViewItem* item3 = new Q3ListViewItem( listview, "bbb" );
    Q3ListViewItem* item4 = new Q3ListViewItem( listview, "jjj" );
    Q3ListViewItem* item5 = new Q3ListViewItem( listview, "ddd" );
    Q3ListViewItem* item6 = new Q3ListViewItem( listview, "lll" );

    Q3ListViewItem* item3b = new Q3ListViewItem( item3, "234" );
    Q3ListViewItem* item3c = new Q3ListViewItem( item3, "345" );
    Q3ListViewItem* item3a = new Q3ListViewItem( item3, "123" );

    listview->setOpen( item3, TRUE );

    listview->setSorting( 0, TRUE );
    listview->show();

    Q3ListViewItem *item = listview->firstChild();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item3a );
    item = item->itemBelow();
    QVERIFY( item == item3b );
    item = item->itemBelow();
    QVERIFY( item == item3c );
    item = item->itemBelow();
    QVERIFY( item == item5 );
    item = item->itemBelow();
    QVERIFY( item == item2 );
    item = item->itemBelow();
    QVERIFY( item == item4 );
    item = item->itemBelow();
    QVERIFY( item == item6 );
    item = item->itemBelow();
    QVERIFY( item == item1 );

    listview->setSorting( 0, FALSE );
    
    item = listview->firstChild();
    QVERIFY( item == item1 );
    item = item->itemBelow();
    QVERIFY( item == item6 );
    item = item->itemBelow();
    QVERIFY( item == item4 );
    item = item->itemBelow();
    QVERIFY( item == item2 );
    item = item->itemBelow();
    QVERIFY( item == item5 );
    item = item->itemBelow();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item3c );
    item = item->itemBelow();
    QVERIFY( item == item3b );
    item = item->itemBelow();
    QVERIFY( item == item3a );

    item = listview->firstChild();
    item->moveItem( item->itemBelow() );

    listview->setSorting( 0, FALSE );
    QVERIFY( item == listview->firstChild() );

    delete listview;
}

void tst_Q3ListView::sortchild2()
{
    Q3ListView* listview = new Q3ListView( 0 );

    listview->addColumn( "" );

    Q3ListViewItem* item1 = new Q3ListViewItem( listview, "zzz" );
    Q3ListViewItem* item2 = new Q3ListViewItem( listview, "hhh" );
    Q3ListViewItem* item3 = new Q3ListViewItem( listview, "bbb" );
    Q3ListViewItem* item4 = new Q3ListViewItem( listview, "jjj" );
    Q3ListViewItem* item5 = new Q3ListViewItem( listview, "ddd" );
    Q3ListViewItem* item6 = new Q3ListViewItem( listview, "lll" );

    Q3ListViewItem* item31 = new Q3ListViewItem( item3, "bbb-level2" );

    Q3ListViewItem* item31b = new Q3ListViewItem( item31, "234" );
    Q3ListViewItem* item31c = new Q3ListViewItem( item31, "345" );
    Q3ListViewItem* item31a = new Q3ListViewItem( item31, "123" );

    listview->setOpen( item3, TRUE );
    listview->setOpen( item31, TRUE );

    listview->setSorting( 0, TRUE );
    listview->show();

    Q3ListViewItem *item = listview->firstChild();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item31 );
    item = item->itemBelow();
    QVERIFY( item == item31a );
    item = item->itemBelow();
    QVERIFY( item == item31b );
    item = item->itemBelow();
    QVERIFY( item == item31c );
    item = item->itemBelow();
    QVERIFY( item == item5 );
    item = item->itemBelow();
    QVERIFY( item == item2 );
    item = item->itemBelow();
    QVERIFY( item == item4 );
    item = item->itemBelow();
    QVERIFY( item == item6 );
    item = item->itemBelow();
    QVERIFY( item == item1 );

    listview->setSorting( 0, FALSE );

    item = listview->firstChild();
    QVERIFY( item == item1 );
    item = item->itemBelow();
    QVERIFY( item == item6 );
    item = item->itemBelow();
    QVERIFY( item == item4 );
    item = item->itemBelow();
    QVERIFY( item == item2 );
    item = item->itemBelow();
    QVERIFY( item == item5 );
    item = item->itemBelow();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item31 );
    item = item->itemBelow();
    QVERIFY( item == item31c );
    item = item->itemBelow();
    QVERIFY( item == item31b );
    item = item->itemBelow();
    QVERIFY( item == item31a );

    item = listview->firstChild();
    item->moveItem( item->itemBelow() );

    listview->setSorting( 0, FALSE );
    QVERIFY( item == listview->firstChild() );

    delete listview;
}

void tst_Q3ListView::sortchild3()
{
    Q3ListView* listview = new Q3ListView( 0 );

    listview->addColumn( "" );

    Q3ListViewItem* item3 = new Q3ListViewItem( listview, "bbb" );


    Q3ListViewItem* item31b = new Q3ListViewItem( item3, "234" );
    Q3ListViewItem* item31c = new Q3ListViewItem( item3, "345" );
    Q3ListViewItem* item31a = new Q3ListViewItem( item3, "123" );

    listview->setOpen( item3, TRUE );

    listview->setSorting( 0, TRUE );
    listview->show();

    Q3ListViewItem *item = listview->firstChild();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item31a );
    item = item->itemBelow();
    QVERIFY( item == item31b );
    item = item->itemBelow();
    QVERIFY( item == item31c );
    item = item->itemBelow();

    listview->setSorting( 0, FALSE );

    item = listview->firstChild();
    QVERIFY( item == item3 );
    item = item->itemBelow();
    QVERIFY( item == item31c );
    item = item->itemBelow();
    QVERIFY( item == item31b );
    item = item->itemBelow();
    QVERIFY( item == item31a );

    delete listview;
}


void tst_Q3ListView::takeItem_data()
{
    QTest::addColumn<Q3ListView::SelectionMode>("selectionMode");
    QTest::addColumn<int>("selectItem");
    QTest::addColumn<int>("selectItemAfterTake");

    QTest::newRow( "SelectionMode::Single, item0 selected" ) << Q3ListView::Single
							  << 0
							  << -1;
    QTest::newRow( "SelectionMode::Single, item1 selected" ) << Q3ListView::Single
							  << 1
							  << -1;
    QTest::newRow( "SelectionMode::Single, item2 selected" ) << Q3ListView::Single
							  << 2
							  << 2;

    QTest::newRow( "SelectionMode::Multi, item0 selected" ) << Q3ListView::Multi
							  << 0
							  << -1;
    QTest::newRow( "SelectionMode::Multi, item1 selected" ) << Q3ListView::Multi
							  << 1
							  << -1;
    QTest::newRow( "SelectionMode::Multi, item2 selected" ) << Q3ListView::Multi
							  << 2
							  << 2;

    QTest::newRow( "SelectionMode::Extended, item0 selected" ) << Q3ListView::Extended
							  << 0
							  << -1;
    QTest::newRow( "SelectionMode::Extended, item1 selected" ) << Q3ListView::Extended
							  << 1
							  << -1;
    QTest::newRow( "SelectionMode::Extended, item2 selected" ) << Q3ListView::Extended
							  << 2
							  << 2;
}

void tst_Q3ListView::takeItem()
{
    testWidget->clear();

    QFETCH( Q3ListView::SelectionMode, selectionMode );
    QFETCH( int, selectItem );
    QFETCH( int, selectItemAfterTake );

    QVector<Q3ListViewItem*> items(3);

    // tree:
    // Item0
    //     Item1
    // Item2
    for ( int i=0; i<3; i++ ) {
	if ( i == 1 ) {
	    items.insert( i, new Q3ListViewItem( items.at( 0 ), QString("Item: %1").arg( i ) ) );
	} else
	    items.insert( i, new Q3ListViewItem( testWidget, QString("Item: %1").arg( i ) ) );
    }

    testWidget->setSelectionMode( selectionMode );
    testWidget->setSelected( items.at( selectItem ), TRUE );

    changed = 0;
    changedItem = 0;
    connect( testWidget, SIGNAL( selectionChanged() ),
	     this, SLOT( selectionChanged() ) );
    connect( testWidget, SIGNAL( selectionChanged( Q3ListViewItem* ) ),
	     this, SLOT( selectionChanged( Q3ListViewItem* ) ) );

    testWidget->takeItem( items.at( 0 ) );

    if ( selectionMode == Q3ListView::Single ) {
	Q3ListViewItem *item = selectItemAfterTake == -1 ? 0 : items.at( selectItemAfterTake );
	// verify that selectedItem have been properly updated after take
	QVERIFY( testWidget->selectedItem() == item );
	if ( selectItemAfterTake == -1 ) {
	    // verify that the selected taken item is unselected as well
	    QVERIFY( items.at( selectItem )->isSelected() == FALSE );
	    // verify that that taking the selectedItem (or parent of it) emits selectionChanged (but not selectionChanged( item )
	    QVERIFY( changed == 1 );
	    QVERIFY( changedItem == 0 );
	} else {
	    // verify that if we still have selection, no selectionChanged was emitted
	    QVERIFY( changed == 0 );
	    QVERIFY( changedItem == 0 );
	}
    } else {
	// verify for Multi and Extended that no selectionChanged is emitted
	QVERIFY( changed == 0 );
	QVERIFY( changedItem == 0 );
    }

    disconnect( testWidget, 0, this, 0 );

    delete items[0];
    delete items[2];
}

Qt::KeyboardModifiers intToKey( int stateKey )
{
    switch( stateKey )
    {
    case 0: return Qt::NoModifier;
    case 1: return Qt::ShiftModifier;
    case 2: return Qt::ControlModifier;
    case 3: return Qt::AltModifier;
    }
    return Qt::NoModifier;
}

void tst_Q3ListView::selections_mouseClick_data()
{
    QTest::addColumn<IntList>("selectedItems");
    QTest::addColumn<IntList>("clickItems");
    QTest::addColumn<IntList>("buttonList");
    QTest::addColumn<IntList>("keyList");
    QTest::addColumn<Q3ListView::SelectionMode>("selectionMode");



    for (int mode = Q3ListView::Single; mode <= Q3ListView::NoSelection; mode++ ) {
	for ( int button = Qt::LeftButton; button <= Qt::RightButton; button++ ) {
	    for ( int s = 0; s < 4; s++ ) {
                Qt::KeyboardModifiers key = intToKey( s );
		if ( key != Qt::AltModifier ) {
		    /* tests clicking on item0 with all different selection modes, buttons and statekeys (except Alt) */
		    IntList selectedItems;
		    IntList clickItems;
		    IntList buttonList;
		    IntList keyList;

		    if ( mode == Q3ListView::NoSelection ||
			 (button == Qt::RightButton && key == Qt::ControlModifier ) );
		    // don't expect any selections here
		    else
			selectedItems << 0;
		    clickItems << 0;
		    buttonList << button;
		    keyList << key;
		    QTest::newRow( "Clicking " + buttonName( button ) + " on item0 with "
				+ keyName( key ) + " in " + selectionName( mode ) )
				    << selectedItems
				    << clickItems
				    << buttonList
				    << keyList
				    << (Q3ListView::SelectionMode) mode;
		}
		if ( mode != Q3ListView::NoSelection && key == Qt::NoModifier ) {
		    /* tests selecting item0 and item1, then clicking outside the items to clear the selections */
		    IntList selectedItems;
		    IntList clickItems;
		    IntList buttonList;
		    IntList keyList;

		    clickItems << 0 << 1 << -1;
		    buttonList << Qt::LeftButton << Qt::LeftButton << button;
		    keyList << Qt::NoModifier << Qt::NoModifier <<  key;

                    QTest::newRow( "Selecting item0 and item1, then clicking "
				+ buttonName( button ) + " right of item0 with "
				+ keyName( key ) + " in " + selectionName( mode ) )
				    << selectedItems
				    << clickItems
				    << buttonList
				    << keyList
				    << (Q3ListView::SelectionMode) mode;
		}
		if ( mode != Q3ListView::NoSelection &&
		     button == Qt::RightButton &&
		     key == Qt::ControlModifier ) {
		    /* tests selecting item0, then selects item1 with RightButton+ControlKey, selection should stay the same */
		    IntList selectedItems;
		    IntList clickItems;
		    IntList buttonList;
		    IntList keyList;

		    selectedItems << 0;
		    clickItems << 0 << 1;
		    buttonList << Qt::LeftButton << button;
		    keyList << Qt::NoModifier << key;

                    QTest::newRow( "Selecting item0, then clicking "
				+ buttonName( button ) + " on item1 with "
				+ keyName( key ) + " in " + selectionName( mode ) )
				    << selectedItems
				    << clickItems
				    << buttonList
				    << keyList
				    << (Q3ListView::SelectionMode) mode;
		}
		if ( mode != Q3ListView::NoSelection &&
		     button == Qt::RightButton &&
		     key == Qt::ControlModifier ) {
		    /* tests selecting item0, then click right of item0 with RightButton+ControlKey, selection should stay the same */
		    IntList selectedItems;
		    IntList clickItems;
		    IntList buttonList;
		    IntList keyList;

		    selectedItems << 0;
		    clickItems << 0 << -1;
		    buttonList << Qt::LeftButton << button;
		    keyList << Qt::NoModifier << key;

                    QTest::newRow( "Selecting item0, then clicking "
				+ buttonName( button ) + " right of item0 with "
				+ keyName( key ) + " in " + selectionName( mode ) )
				    << selectedItems
				    << clickItems
				    << buttonList
				    << keyList
				    << (Q3ListView::SelectionMode) mode;
		}
	    }
	}
    }
}

void tst_Q3ListView::selections_mouseClick()
{

    QFETCH( IntList, selectedItems );
    QFETCH( IntList, clickItems );
    QFETCH( IntList, buttonList );
    QFETCH( IntList, keyList );
    QFETCH( Q3ListView::SelectionMode, selectionMode );

    QVERIFY( clickItems.count() == buttonList.count() &&
	    buttonList.count() == keyList.count() );

    Q3ListView listView( 0 );

    listView.setSelectionMode( selectionMode );
    listView.addColumn( "First" );
    listView.addColumn( "Second" );

    // tree:
    // Item0
    // Item1
    // Item2
    QVector<Q3ListViewItem*> items;
    int i;
    for (i = 0; i < 3; ++i) {
	items.append(new Q3ListViewItem( &listView, QString("Item: %1").arg( i ) ));
    }
    listView.setSorting( 0, TRUE );
    listView.show();

    for ( i=0; i<(int)clickItems.count(); i++ ) {
	// find point to click, -1 means outside any item
	QPoint clickPoint = (clickItems[ i ] == -1) ?
			    itemBelow( &listView, items[ 2 ] ) :
			    itemCenter( &listView, items[ clickItems[i] ] );
	// send the mouse click event
	QTest::mouseClick( listView.viewport(), (Qt::MouseButton)buttonList.at(i),
		    (Qt::KeyboardModifier) keyList.at(i),
		    clickPoint );
    }

//     while ( listView.isVisible() )
//	qApp->processEvents();

    for (i = 0; i < items.count(); ++i) {
	Q3ListViewItem *item = items.at(i);
        Q_ASSERT(item);
	if ( item->isSelected() ) {
	    QVERIFY( selectedItems.contains( i ) );
	} else {
	    QVERIFY( !selectedItems.contains( i ) );
	}
    }
}

QPoint tst_Q3ListView::itemCenter( Q3ListView* view, Q3ListViewItem* item, int )
{
    if ( view && item )
	return QPoint( view->itemRect( item ).x() + 10, view->itemRect( item ).center().y() );
    return QPoint();
}

QPoint tst_Q3ListView::itemBelow( Q3ListView* view, Q3ListViewItem* item )
{
    if ( view && item ) {
	QRect i = view->itemRect( item );
	return QPoint( i.center().x(), i.bottom()+10 );
    }
    return QPoint();
}


QString tst_Q3ListView::selectionName( int selectionMode )
{
    switch ( selectionMode ) {
    case Q3ListView::Single:
	return "Single";
    case Q3ListView::Multi:
	return "Multi";
    case Q3ListView::Extended:
	return "Extended";
    case Q3ListView::NoSelection:
	return "NoSelection";
    default:
	return "Unknown SelectionMode";
    }
}

QString tst_Q3ListView::buttonName( int mouseButton )
{
    switch ( mouseButton ) {
    case Qt::LeftButton:
	return "LeftButton";
    case Qt::MidButton:
	return "MidButton";
    case Qt::RightButton:
	return "RightButton";
    default:
	return "Unknown button";
    }
}

QString tst_Q3ListView::keyName( Qt::KeyboardModifiers stateKey )
{
    switch ( stateKey ) {
    case Qt::NoModifier:
	return "NoKey";
    case Qt::ShiftModifier:
	return "ShiftKey";
    case Qt::ControlModifier:
	return "ControlKey";
    case Qt::AltModifier:
	return "AltKey";
    default:
	return "Unknown key";
    }
}

void tst_Q3ListView::isVisible()
{
    Q3ListView listview( 0 );
    Q3ListViewItem* tlOne = new Q3ListViewItem( &listview, "Item One" );
    Q3ListViewItem* tlTwo = new Q3ListViewItem( &listview, "Item Two" );
    Q3ListViewItem* tlThree = new Q3ListViewItem( &listview, "Item Three" );

    Q3ListViewItem* tlTwoCOne = new Q3ListViewItem( tlTwo, "Child One of Item Two" );
    Q3ListViewItem* tlTwoCTwo = new Q3ListViewItem( tlTwo, "Child Two of Item Two" );
    Q3ListViewItem* tlTwoCThree = new Q3ListViewItem( tlTwo, "Child Three of Item Two" );

    Q3ListViewItem* tlTwoCTwoCOne = new Q3ListViewItem( tlTwoCTwo, "Child One of Child Two of Item Two" );
    Q3ListViewItem* tlTwoCTwoCTwo = new Q3ListViewItem( tlTwoCTwo, "Child Two of Child Two of Item Two" );
    Q3ListViewItem* tlTwoCTwoCThree = new Q3ListViewItem( tlTwoCTwo, "Child Three of Child Two of Item Two" );
    Q_UNUSED(tlTwoCTwoCThree);
    Q_UNUSED(tlTwoCTwoCOne);
    Q_UNUSED(tlTwoCThree);
    Q_UNUSED(tlThree);

    listview.show();
    tlOne->setVisible( FALSE );

    QVERIFY( !tlOne->isVisible() );
    QVERIFY( tlTwo->isVisible() );
    QVERIFY( tlTwoCOne->isVisible() );
    QVERIFY( tlTwoCTwoCTwo->isVisible() );

    tlTwo->setVisible( FALSE );
    QVERIFY( !tlTwo->isVisible() );
    QVERIFY( !tlTwoCOne->isVisible() );
    QVERIFY( !tlTwoCTwoCTwo->isVisible() );

    tlTwoCTwoCTwo->setVisible( TRUE );
    QVERIFY( !tlTwo->isVisible() );
    QVERIFY( !tlTwoCOne->isVisible() );
    QVERIFY( !tlTwoCTwoCTwo->isVisible() );

    tlTwo->setVisible( TRUE );
    QVERIFY( tlTwo->isVisible() );
    QVERIFY( tlTwoCOne->isVisible() );
    QVERIFY( tlTwoCTwoCTwo->isVisible() );
}

void tst_Q3ListView::itemRenamed(Q3ListViewItem *item, int column, const QString &text)
{
    itemRenamedOne = item;
    columnRenamedOne = column;
    textRenamed = text;
    itemRenamedSignalOneReceived = TRUE;
}

void tst_Q3ListView::itemRenamed(Q3ListViewItem *item, int column)
{
    itemRenamedTwo = item;
    columnRenamedTwo = column;
    itemRenamedSignalTwoReceived = TRUE;
}

void tst_Q3ListView::resetVariables()
{
    itemRenamedSignalOneReceived = FALSE;
    itemRenamedSignalTwoReceived = FALSE;
    itemRenamedOne = 0;
    itemRenamedTwo = 0;
    columnRenamedOne = -1;
    columnRenamedTwo = -1;
    textRenamed = QString();
}

void tst_Q3ListView::itemRenaming()
{
    int a;

    testWidget->clear();
    connect(testWidget, SIGNAL(itemRenamed(Q3ListViewItem *, int, const QString &)),
	    this, SLOT(itemRenamed(Q3ListViewItem *, int, const QString &)));
    connect(testWidget, SIGNAL(itemRenamed(Q3ListViewItem *, int)),
	    this, SLOT(itemRenamed(Q3ListViewItem *, int)));

    Q3ListViewItem *itemOne = new Q3ListViewItem(testWidget, "A - Rename Me One", "Rename Me One Col One");
    Q3ListViewItem *itemTwo = new Q3ListViewItem(testWidget, "B - Rename Me Two", "Rename Me Two Col One");
    Q3ListViewItem *itemThree = new Q3ListViewItem(testWidget, "C - Rename Me Three", "Rename Me Three Col One");

    QVERIFY(!itemOne->renameEnabled(0));
    QVERIFY(!itemOne->renameEnabled(1));
    for (a = 0;a < 2;a++) {
	itemOne->setRenameEnabled(a,TRUE);
	itemTwo->setRenameEnabled(a,TRUE);
	itemThree->setRenameEnabled(a,TRUE);
    }
    QVERIFY(itemOne->renameEnabled(0));
    QVERIFY(itemOne->renameEnabled(1));

    // Check if programatic renaming works

    itemOne->startRename(0);
    QLineEdit *renameBox = qFindChild<QLineEdit*>(testWidget->viewport(), "qt_renamebox");
    QVERIFY(renameBox);
    QVERIFY(renameBox->isVisible());
    QTest::keyClick(renameBox, Qt::Key_R);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_N);
    QTest::keyClick(renameBox, Qt::Key_A);
    QTest::keyClick(renameBox, Qt::Key_M);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_D);
    QTest::keyPress(renameBox, Qt::Key_Return);

    QVERIFY(itemRenamedSignalOneReceived);
    QVERIFY(itemRenamedOne == itemOne );
    QVERIFY(columnRenamedOne == 0);
    QVERIFY(textRenamed == "renamed");
    QVERIFY(itemRenamedSignalTwoReceived);
    QVERIFY(itemRenamedTwo == itemOne);
    QVERIFY(columnRenamedTwo == 0);

    QCOMPARE(itemOne->text(0), QString("renamed"));
    QCOMPARE(itemOne->text(1), QString("Rename Me One Col One"));

    resetVariables();

    itemOne->startRename(1);
    renameBox = (QLineEdit *)testWidget->viewport()->child("qt_renamebox", "QLineEdit");
    QVERIFY(renameBox);
    QVERIFY(renameBox->isVisible());
    QTest::keyClick(renameBox, Qt::Key_R);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_N);
    QTest::keyClick(renameBox, Qt::Key_A);
    QTest::keyClick(renameBox, Qt::Key_M);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_D);
    QTest::keyClick(renameBox, Qt::Key_Space);
    QTest::keyClick(renameBox, Qt::Key_C);
    QTest::keyClick(renameBox, Qt::Key_O);
    QTest::keyClick(renameBox, Qt::Key_L);
    QTest::keyClick(renameBox, Qt::Key_1);
    QTest::keyPress(renameBox, Qt::Key_Return);
    QCOMPARE(itemOne->text(0), QString("renamed"));
    QCOMPARE(itemOne->text(1), QString("renamed col1"));

    QVERIFY(itemRenamedSignalOneReceived);
    QVERIFY(itemRenamedOne == itemOne );
    QVERIFY(columnRenamedOne == 1);
    QVERIFY(textRenamed == "renamed col1");
    QVERIFY(itemRenamedSignalTwoReceived);
    QVERIFY(itemRenamedTwo == itemOne);
    QVERIFY(columnRenamedTwo == 1);

    // Check if renaming via keyboard works
    resetVariables();

    QTest::keyClick(testWidget->viewport(), Qt::Key_Space);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Down);
    QVERIFY(testWidget->currentItem() == itemTwo);
    QTest::keyClick(testWidget->viewport(), Qt::Key_F2);
    renameBox = (QLineEdit *)testWidget->viewport()->child("qt_renamebox", "QLineEdit");
    QVERIFY(renameBox);
    QVERIFY(renameBox->isVisible());
    QTest::keyClick(renameBox, Qt::Key_R);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_N);
    QTest::keyClick(renameBox, Qt::Key_A);
    QTest::keyClick(renameBox, Qt::Key_M);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_D);
    QTest::keyClick(renameBox, Qt::Key_2);
    QTest::keyPress(renameBox, Qt::Key_Return);

    QVERIFY(itemRenamedSignalOneReceived);
    QVERIFY(itemRenamedOne == itemTwo);
    QVERIFY(columnRenamedOne == 0);
    QVERIFY(textRenamed == "renamed2");
    QVERIFY(itemRenamedSignalTwoReceived);
    QVERIFY(itemRenamedTwo == itemTwo);
    QVERIFY(columnRenamedTwo == 0);

    QCOMPARE(itemTwo->text(0), QString("renamed2"));
    QCOMPARE(itemTwo->text(1), QString("Rename Me Two Col One"));

    resetVariables();
#if 0
    // Check if renaming via mouse works
    QPoint itemPos;
    itemPos.setX(testWidget->header()->sectionPos(0) + 5);
    itemPos.setY(itemThree->itemPos() + 5);
    qDebug("%d-%d",itemPos.x(),itemPos.y());
    QTest::mousePress(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, itemPos);

    // Should be enough to wait for a release
    for (a = 0;a < 100; a++)
	qApp->processEvents();

    QTest::mouseRelease(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, itemPos);

    renameBox = (QLineEdit *)testWidget->viewport()->child("qt_renamebox", "QLineEdit");
    QVERIFY(renameBox);
    QVERIFY(renameBox->isVisible());
    QTest::keyClick(renameBox, Qt::Key_R);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_N);
    QTest::keyClick(renameBox, Qt::Key_A);
    QTest::keyClick(renameBox, Qt::Key_M);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_D);
    QTest::keyClick(renameBox, Qt::Key_3);
    QTest::keyClick(renameBox, Qt::Key_Return);

    QVERIFY(itemRenamedSignalOneReceived);
    QVERIFY(itemRenamedOne == itemThree);
    QVERIFY(columnRenamedOne == 0);
    QVERIFY(textRenamed == "renamed3");
    QVERIFY(itemRenamedSignalTwoReceived);
    QVERIFY(itemRenamedTwo == itemThree);
    QVERIFY(columnRenamedTwo == 0);

    QCOMPARE(itemOne->text(0), QString("renamed3"));
    QCOMPARE(itemOne->text(1), QString("Rename Me Three Col One"));

    resetVariables();

    itemPos.setX(testWidget->header()->sectionPos(1) + 5);
    itemPos.setY(itemThree->itemPos() + 5);
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, itemPos);

    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, itemPos);
    renameBox = (QLineEdit *)testWidget->viewport()->child("qt_renamebox", "QLineEdit");
    QVERIFY(renameBox);
    QVERIFY(renameBox->isVisible());
    QTest::keyClick(renameBox, Qt::Key_R);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_N);
    QTest::keyClick(renameBox, Qt::Key_A);
    QTest::keyClick(renameBox, Qt::Key_M);
    QTest::keyClick(renameBox, Qt::Key_E);
    QTest::keyClick(renameBox, Qt::Key_D);
    QTest::keyClick(renameBox, Qt::Key_3);
    QTest::keyClick(renameBox, Qt::Key_Space);
    QTest::keyClick(renameBox, Qt::Key_C);
    QTest::keyClick(renameBox, Qt::Key_O);
    QTest::keyClick(renameBox, Qt::Key_L);
    QTest::keyClick(renameBox, Qt::Key_1);
    QTest::keyClick(renameBox, Qt::Key_Return);
    QCOMPARE(itemThree->text(0), QString("renamed3"));
    QCOMPARE(itemThree->text(1), QString("renamed3 col1"));

    QVERIFY(itemRenamedSignalOneReceived);
    QVERIFY(itemRenamedOne == itemThree);
    QVERIFY(columnRenamedOne == 1);
    QVERIFY(textRenamed == "renamed3 col1");
    QVERIFY(itemRenamedSignalTwoReceived);
    QVERIFY(itemRenamedTwo == itemThree);
    QVERIFY(columnRenamedTwo == 1);
#endif
}

void tst_Q3ListView::removeColumn()
{
    // clear all items and columns
    while ( testWidget->columns() )
	testWidget->removeColumn( 0 );
    testWidget->clear();
    QVERIFY( !testWidget->firstChild() );
    QVERIFY( !testWidget->columns() );

    // add one column
    // test columns() and header()->count()
    testWidget->addColumn( "One" );
    QCOMPARE( testWidget->columns(), 1 );
    QCOMPARE( testWidget->header() ? testWidget->header()->count() : -1,
	     testWidget->columns() );
    // add a second column
    testWidget->addColumn( "Two" );
    QCOMPARE( testWidget->columns(), 2 );
    QCOMPARE( testWidget->header() ? testWidget->header()->count() : -1,
	     testWidget->columns() );
    // add a third column
    testWidget->addColumn( "Three" );
    QCOMPARE( testWidget->columns(), 3 );
    QCOMPARE( testWidget->header() ? testWidget->header()->count() : -1,
	     testWidget->columns() );

    // actually remove
    while ( testWidget->columns() ) {
	testWidget->removeColumn( 0 );
	// make sure header and q3listview always are in sync
	QCOMPARE( testWidget->header() ? testWidget->header()->count() : -1,
		 testWidget->columns() );
    }

    // check that there are no columns
    QVERIFY( !testWidget->columns() );
    // check that there are no sections in the header
    QVERIFY( testWidget->header() ? !testWidget->header()->count() : FALSE );
}

void tst_Q3ListView::contextMenu(Q3ListViewItem *item, const QPoint &pos, int col)
{
    // Slot to gather information from the contextMenuRequested signal
    contextMenuRequestedItem = item;
    contextMenuRequestedPos = pos;
    contextMenuRequestedCol = col;
    contextMenuRequestedSignalReceived = TRUE;
}

void tst_Q3ListView::contextMenuRequested_data()
{
    QTest::addColumn<bool>("mouse");
    QTest::addColumn<QPoint>("clickPos");
    QTest::addColumn<QString>("textOfListViewItem");
    // QTest::addColumn<QPoint>("menuPos"); -- Need a way to reliably test this
    QTest::addColumn<int>("column");

    QTest::newRow("mouseClickTopItemFirstColumn") << TRUE << QPoint(22, 5) /* << QPoint()*/ << QString("Item One") << 0;
    QTest::newRow("keyClickTopItemFirstColumn") << FALSE << QPoint(22, 5) /* << QPoint()*/  << QString("Item One") << -1;
    QTest::newRow("mouseClick2ndItemSecondColumn") << TRUE << QPoint(40, 20) /* << QPoint()*/  << QString("Item Two") << 0;
    QTest::newRow("keyClick2ndItemSecondColumn") << FALSE << QPoint(40, 20) /* << QPoint()*/  << QString("Item One") << -1;
    QTest::newRow("noItemClicked") << TRUE << QPoint(100, 100) /* << QPoint()*/  << QString() << -1;
}

void tst_Q3ListView::contextMenuRequested()
{
    QFETCH(bool, mouse);
    QFETCH(QPoint, clickPos);
    QFETCH(QString, textOfListViewItem);
    //QFETCH(QPoint, menuPos);
    QFETCH(int, column);

    QSKIP("Does not work with qtestlib's mouse/keyboard handling.", SkipAll);

    testWidget->clear();
    testWidget->addColumn("Column 1");
    testWidget->addColumn("Column 2");
    testWidget->connect(testWidget, SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint &, int)),
			this, SLOT(contextMenu(Q3ListViewItem *, const QPoint &, int)));

    testWidget->setColumnWidth(0, 35);
    testWidget->setColumnWidth(1, 35);
    Q3ListViewItem *itemOne = new Q3ListViewItem(testWidget, "Item One", "Item One 2");
    Q3ListViewItem *itemTwo = new Q3ListViewItem(testWidget, itemOne, "Item Two", "Item Two 2");
    Q_UNUSED(itemOne);
    Q_UNUSED(itemTwo);


    if (mouse) {
	QTest::mouseClick(testWidget->viewport(), Qt::RightButton, Qt::NoModifier, clickPos);
    } else {
	// Then it's a keypress
        QTest::mouseMove(testWidget->viewport(), clickPos);
	QTest::keyClick(testWidget->viewport(), Qt::Key_Menu);
    }

    QVERIFY(contextMenuRequestedSignalReceived);
    if (textOfListViewItem.isNull()) {
	QVERIFY(!contextMenuRequestedItem);
    } else {
	QCOMPARE(contextMenuRequestedItem->text(0), textOfListViewItem);
    }
    //QCOMPARE(contextMenuRequestedPos, menuPos);
    QCOMPARE(contextMenuRequestedCol, column);
}

// Exists solely for testing the activate() function
class ActivateListViewItem : public Q3ListViewItem
{
public:
    ActivateListViewItem(Q3ListView *parent) : Q3ListViewItem(parent),
	activated(false) {}
    bool activated;
protected:
    void activate()
    {
 	activated = true;
    }
};

void tst_Q3ListView::itemActivate()
{
    testWidget->addColumn("Test 1");
    ActivateListViewItem *alvi = new ActivateListViewItem(testWidget);
    alvi->setText(0, "Test");
    new Q3ListViewItem(alvi, "Testing");
    testWidget->setCurrentItem(alvi);
    QTest::keyPress(testWidget->viewport(), Qt::Key_Space);
    QVERIFY(alvi->activated);
    QTest::keyRelease(testWidget->viewport(), Qt::Key_Space);

    alvi->activated = false;
    QTest::mousePress(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,alvi->itemPos()));
    QVERIFY(alvi->activated);
    QTest::mouseRelease(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, QPoint(5,alvi->itemPos()));
    QVERIFY(alvi->activated);
}

/*
Qt::CaseSensitive - The strings must match case sensitively.
Q3ListView::ExactMatch - The target and search strings must match exactly.
Q3ListView::BeginsWith - The target string begins with the search string.
Q3ListView::EndsWith - The target string ends with the search string.
Q3ListView::Contains - The target string contains the search string.

the search criteria be applied in the following order: ExactMatch, BeginsWith, EndsWith, Contains
*/

void tst_Q3ListView::findItem_data()
{
    QTest::addColumn<int>("indexOfCurrentItem");
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("column");
    QTest::addColumn<int>("comparisonFlags");
    QTest::addColumn<bool>("itemFound");
    QTest::addColumn<QString>("textOfItemFound");

    QTest::newRow("noCurrentItem") << -1 << "item one" << 0 << 0 << TRUE << "Item One";
    QTest::newRow("noCurrentItem-bogusColumn") << -1 << "item one" << 99 << 0 << FALSE << QString();
    QTest::newRow("noCurrentItem-invalidItem") << -1 << "blah" << 0 << 0 << FALSE << QString();
    QTest::newRow("noCurrentItem-caseSensitive-invalidItem") << -1 << "item one" << 0 << Q3ListView::CaseSensitive << FALSE << QString();
    QTest::newRow("noCurrentItem-caseSensitive") << -1 << "Item One" << 0 << Q3ListView::CaseSensitive << TRUE << "Item One";
    QTest::newRow("noCurrentItem-exactMatch-invalidItem") << -1 << "blah" << 0 << Q3ListView::ExactMatch << FALSE << QString();
    QTest::newRow("noCurrentItem-exactMatch") << -1 << "Item One" << 0 << Q3ListView::ExactMatch << TRUE << "Item One";
    QTest::newRow("noCurrentItem-beginsWith-invalidItem") << -1 << "blah" << 0 << Q3ListView::BeginsWith << FALSE << QString();
    QTest::newRow("noCurrentItem-beginsWith") << -1 << "Item One" << 0 << Q3ListView::BeginsWith << TRUE << "Item One";
    QTest::newRow("noCurrentItem-endsWith-invalidItem") << -1 << "blah" << 0 << Q3ListView::EndsWith << FALSE << QString();
    QTest::newRow("noCurrentItem-endsWith") << -1 << "Item One" << 0 << Q3ListView::EndsWith << TRUE << "Item One";
    QTest::newRow("noCurrentItem-contains-invalidItem") << -1 << "blah" << 0 << Q3ListView::Contains << FALSE << QString();
    QTest::newRow("noCurrentItem-contains") << -1 << "Item One" << 0 << Q3ListView::Contains << TRUE << "Item One";

    // Now check that case sensitivity has no effect unless specified
    QTest::newRow("noCurrentItem-exactMatch-nocs") << -1 << "ITEM ONE" << 0 << Q3ListView::ExactMatch << TRUE << "Item One";
    QTest::newRow("noCurrentItem-beginsWith-nocs") << -1 << "ITEM ONE" << 0 << Q3ListView::BeginsWith << TRUE << "Item One";
    QTest::newRow("noCurrentItem-endsWith-nocs") << -1 << "ITEM ONE" << 0 << Q3ListView::EndsWith << TRUE << "Item One";
    QTest::newRow("noCurrentItem-contains-nocs") << -1 << "ITEM ONE" << 0 << Q3ListView::Contains << TRUE << "Item One";
    QTest::newRow("noCurrentItem-exactMatch-cs") << -1 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::ExactMatch) << FALSE << QString();
    QTest::newRow("noCurrentItem-beginsWith-cs") << -1 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::BeginsWith) << FALSE << QString();
    QTest::newRow("noCurrentItem-endsWith-cs") << -1 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::EndsWith) << FALSE << QString();
    QTest::newRow("noCurrentItem-contains-cs") << -1 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::Contains) << FALSE << QString();

    // Now check that the search criteria order is adhered to
    QTest::newRow("noCurrentItem-exactMatch-beginswith-nocs") << -1 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::BeginsWith) << TRUE << "Item One";
    QTest::newRow("noCurrentItem-exactMatch-endswith-nocs") << -1 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::EndsWith) << TRUE << "Item One";
    QTest::newRow("noCurrentItem-exactMatch-contains-nocs") << -1 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::Contains) << TRUE << "Item One";
    QTest::newRow("noCurrentItem-beginswith-endswith-nocs") << -1 << "Item One" << 0 << (Q3ListView::BeginsWith | Q3ListView::EndsWith) << TRUE << "Item One";
    QTest::newRow("noCurrentItem-beginswith-contains-nocs") << -1 << "Item One" << 0 << (Q3ListView::BeginsWith | Q3ListView::Contains) << TRUE << "Item One";
    QTest::newRow("noCurrentItem-endswith-contains-nocs") << -1 << "Item One" << 0 << (Q3ListView::EndsWith | Q3ListView::Contains) << TRUE << "Item One";

    // Now check that the current item set has the effect of searching from that item
    QTest::newRow("currentItemIsTwo") << 2 << "item one" << 0 << 0 << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-bogusColumn") << 2 << "item one" << 99 << 0 << FALSE << QString();
    QTest::newRow("currentItemIsTwo-invalidItem") << 2 << "blah" << 0 << 0 << FALSE << QString();
    QTest::newRow("currentItemIsTwo-caseSensitive-invalidItem") << 2 << "item one" << 0 << Q3ListView::CaseSensitive << FALSE << QString();
    QTest::newRow("currentItemIsTwo-caseSensitive") << 2 << "Item One" << 0 << Q3ListView::CaseSensitive << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-exactMatch-invalidItem") << 2 << "blah" << 0 << Q3ListView::ExactMatch << FALSE << QString();
    QTest::newRow("currentItemIsTwo-exactMatch") << 2 << "Item One" << 0 << Q3ListView::ExactMatch << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-beginsWith-invalidItem") << 2 << "blah" << 0 << Q3ListView::BeginsWith << FALSE << QString();
    QTest::newRow("currentItemIsTwo-beginsWith") << 2 << "Item One" << 0 << Q3ListView::BeginsWith << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-endsWith-invalidItem") << 2 << "blah" << 0 << Q3ListView::EndsWith << FALSE << QString();
    QTest::newRow("currentItemIsTwo-endsWith") << 2 << "Item One" << 0 << Q3ListView::EndsWith << TRUE << "Item One Item One";
    QTest::newRow("currentItemIsTwo-contains-invalidItem") << 2 << "blah" << 0 << Q3ListView::Contains << FALSE << QString();
    QTest::newRow("currentItemIsTwo-contains") << 2 << "Item One" << 0 << Q3ListView::Contains << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-exactMatch-nocs") << 2 << "ITEM ONE" << 0 << Q3ListView::ExactMatch << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-beginsWith-nocs") << 2 << "ITEM ONE" << 0 << Q3ListView::BeginsWith << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-endsWith-nocs") << 2 << "ITEM ONE" << 0 << Q3ListView::EndsWith << TRUE << "Item One Item One";
    QTest::newRow("currentItemIsTwo-contains-nocs") << 2 << "ITEM ONE" << 0 << Q3ListView::Contains << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-exactMatch-cs") << 2 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::ExactMatch) << FALSE << QString();
    QTest::newRow("currentItemIsTwo-beginsWith-cs") << 2 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::BeginsWith) << FALSE << QString();
    QTest::newRow("currentItemIsTwo-endsWith-cs") << 2 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::EndsWith) << FALSE << QString();
    QTest::newRow("currentItemIsTwo-contains-cs") << 2 << "ITEM ONE" << 0 << (Q3ListView::CaseSensitive | Q3ListView::Contains) << FALSE << QString();
    QTest::newRow("currentItemIsTwo-exactMatch-beginswith-nocs") << 2 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::BeginsWith) << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-exactMatch-endswith-nocs") << 2 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::EndsWith) << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-exactMatch-contains-nocs") << 2 << "Item One" << 0 << (Q3ListView::ExactMatch | Q3ListView::Contains) << TRUE << "Item One";
    QTest::newRow("currentItemIsTwo-beginswith-endswith-nocs") << 2 << "Item One" << 0 << (Q3ListView::BeginsWith | Q3ListView::EndsWith) << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-beginswith-contains-nocs") << 2 << "Item One" << 0 << (Q3ListView::BeginsWith | Q3ListView::Contains) << TRUE << "Item One Two Three";
    QTest::newRow("currentItemIsTwo-endswith-contains-nocs") << 2 << "Item One" << 0 << (Q3ListView::EndsWith | Q3ListView::Contains) << TRUE << "Item One Item One";
    QTest::newRow("itemWithEmptyText") << 2 << "" << 0 << (Q3ListView::ExactMatch | Q3ListView::CaseSensitive) << TRUE << "";
    QTest::newRow("itemWithEmptyText-2") << 2 << "" << 0 << (Q3ListView::BeginsWith | Q3ListView::Contains) << FALSE << "";
}

void tst_Q3ListView::findItem()
{
    // ### This test is not complete, any results are still valid though

    QFETCH(int, indexOfCurrentItem);
    QFETCH(QString, text);
    QFETCH(int, column);
    QFETCH(int, comparisonFlags);
    QFETCH(bool, itemFound);
    QFETCH(QString, textOfItemFound);

    testWidget->addColumn("Test");
    testWidget->setSorting(-1);
    Q3ListViewItem *itemOne = new Q3ListViewItem(testWidget, "Item One");
    Q3ListViewItem *itemTwo = new Q3ListViewItem(testWidget, itemOne, "Item One Two");
    Q3ListViewItem *itemThree = new Q3ListViewItem(testWidget, itemTwo, "Item One Two Three");
    new Q3ListViewItem(testWidget, itemThree, "Item One Item One");
    new Q3ListViewItem(testWidget, ""); // For the empty item test

    // We need some way of mapping items to indices
    // and also a way to force no current item
    // currently we assume item two is the current in the test
    if (indexOfCurrentItem != -1)
	testWidget->setCurrentItem(itemThree);

    Q3ListViewItem *foundItem = testWidget->findItem(text, column, QFlag(comparisonFlags));
    if (itemFound) {
	QVERIFY(foundItem);
	QCOMPARE(textOfItemFound, foundItem->text(column));
    } else {
	QVERIFY(!foundItem);
    }
}


void tst_Q3ListView::spacePress_data()
{
    QTest::addColumn<Q3ListView::SelectionMode>("selectionMode");
    QTest::addColumn<int>("itemCount");
    QTest::addColumn<int>("disabledItem");
    QTest::addColumn<QTestEventList>("keys");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<int>("expectedCurrentSelected");

    // testing space pressed on enabled/disabled item
    for (int mode = Q3ListView::Single; mode <= Q3ListView::NoSelection; mode++ ) {
	for (int k=0; k<4; ++k ) {
            Qt::KeyboardModifiers key = intToKey( k );
	    if ( key == Qt::NoModifier || key == Qt::ControlModifier ) {
		for ( int enabled=0; enabled<2; ++enabled ) {
		    QString testName = QString( "Space on %1 item. Selection mode: %2 Button: %3" )
				       .arg( enabled ? "enabled" : "disabled" )
				       .arg( selectionName( mode ))
				       .arg( keyName( key ) );
		    QTestEventList keys;
		    // going to first item and pressing space
		    keys.addKeyClick( Qt::Key_Home );
		    keys.addKeyClick( Qt::Key_Space, key );
		    QTest::newRow( testName ) << (Q3ListView::SelectionMode)mode << 9
                                              << (enabled ? -1 : 0 ) << keys << 1
                                              << ((mode == Q3ListView::Single) ||
                                                  (mode == Q3ListView::Multi && !enabled) ? 1 : -1);
		}
	    }
	}
    }

    {
	QTestEventList keys;
	keys.addKeyClick( Qt::Key_Home );
	keys.addKeyClick( Qt::Key_Down );
	keys.addKeyClick( Qt::Key_Up );
	keys.addKeyClick( Qt::Key_Space );
	keys.addKeyClick( Qt::Key_Space, Qt::ControlModifier );
	QTest::newRow( "Space then Ctrl-Space to unselect in Extended" )
	    << Q3ListView::Extended << 9 << -1 << keys << 2 << 0;
    }
}

void tst_Q3ListView::spacePress()
{
    QFETCH( Q3ListView::SelectionMode, selectionMode );
    QFETCH( int, itemCount );
    QFETCH( int, disabledItem );
    QFETCH( QTestEventList, keys );
    QFETCH( int, expectedCount );
    QFETCH( int, expectedCurrentSelected );

    testWidget->setSorting( 0, TRUE );
    testWidget->addColumn( "Items" );
    testWidget->setSelectionMode( selectionMode );
    for ( int i=0; i<itemCount; ++i) {
	Q3ListViewItem *item = new Q3ListViewItem( testWidget, QString( "Child %1" ).arg( i ) );
	if ( i == disabledItem )
	    item->setEnabled( FALSE );
	if ( i == 0 )
	    testWidget->setSelected( item, TRUE );
    }

    // naviagate to keys and press space
    pressCount = 0;
    keys.simulate( testWidget );
    // verify that spacePressed has been called the correct amount of times
    QCOMPARE( pressCount, expectedCount );
    if ( testWidget->currentItem() && expectedCurrentSelected > -1 )
	QCOMPARE( testWidget->currentItem()->isSelected(), (bool)expectedCurrentSelected );
}

void tst_Q3ListView::adjustColumn()
{
    testWidget->adjustColumn(-1);
    testWidget->adjustColumn(100);
    testWidget->adjustColumn(0);
    QVERIFY(true); // Just to check it did not crash
}

typedef QPair<QByteArray, QVariant> PropertyItem;
typedef QList<PropertyItem> PropertyItemList;
Q_DECLARE_METATYPE(PropertyItemList)

void tst_Q3ListView::mouseClickEvents_data()
{
    QTest::addColumn<QStringList>("itemstrings");
    QTest::addColumn<int>("expectedDoubleClickCount");
    QTest::addColumn<PropertyItemList>("properties");

    QTest::newRow("doubleclick") << (QStringList() << "item 1" << "item 2")
                                 << 2 << PropertyItemList();
    QTest::newRow("doubleclick") << (QStringList() << "item 1" << "item 2")
                                 << 0 << (PropertyItemList() << PropertyItem("enabled", false));

}

void tst_Q3ListView::mouseClickEvents()
{
    QFETCH(QStringList, itemstrings);
    QFETCH(int, expectedDoubleClickCount);
    QFETCH(PropertyItemList, properties);
    
    int i;
    for (i = 0; i < properties.count(); ++i) {
        testWidget->setProperty(properties.at(i).first.constData(), properties.at(i).second);
    }

    doubleClickCount = 0;
    pressedItem = 0;
    connect( testWidget, SIGNAL( doubleClicked( Q3ListViewItem* ) ),
	     this, SLOT( doubleClicked( Q3ListViewItem* ) ) );
    testWidget->addColumn("Items");
    QVector<Q3ListViewItem*> items;
    for ( i=0; i<itemstrings.count(); ++i) {
	Q3ListViewItem *item = new Q3ListViewItem( testWidget, itemstrings.at(i) );
        items.append(item);
    }
    for ( i = 0; i < items.count(); ++i) {
        int prevCount = doubleClickCount;
        QTest::mouseDClick(testWidget->viewport(), Qt::LeftButton, 0, itemCenter(testWidget, items.at(i)));
        if (doubleClickCount > prevCount) {
            QCOMPARE(pressedItem, items.at(i));
        } else {
            QCOMPARE(pressedItem, (Q3ListViewItem*)0);
        }
    }
    QCOMPARE(doubleClickCount, expectedDoubleClickCount);
    disconnect( testWidget, SIGNAL( doubleClicked( Q3ListViewItem* ) ),
	     this, SLOT( doubleClicked( Q3ListViewItem* ) ) );
}

QTEST_MAIN(tst_Q3ListView)
#include "tst_q3listview.moc"

