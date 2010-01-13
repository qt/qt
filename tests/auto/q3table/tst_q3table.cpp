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

#include <q3datetimeedit.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <q3table.h>
#include <qlayout.h>

#include "../../shared/util.h"

#define WAITS 1
#ifdef WAITS
#endif

Q_DECLARE_METATYPE(QPoint)

//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(Q3Table)

class tst_Q3Table : public QObject
{
    Q_OBJECT
public:
    tst_Q3Table();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void setNumRowsColsItems();
    void setNumRowsColsWidgets();

    void selection();
    void selectComboTableItem_data();
    void selectComboTableItem();
    void pageUpDownNavigation_data();
    void pageUpDownNavigation();
    void simpleKeyboardNavigation();
    void setRowStretchable();
    void setColumnStretchable();
    void selectRow_data();
    void selectRow();
    void selectColumn_data();
    void selectColumn();
    void removeSelection_selectionChanged();
    void cellNavigation();
    void cellNavigationWhileEditing();
    void removeRow_data();
    void removeRow();
    void swapRowMemoryLeak();
    void insertRows_data();
    void insertRows();
    void editCheck();
    void setCellWidgetFocus();
    void selectionWithMouse_data();
    void selectionWithMouse();
    void valueChanged();
    void numSelections(); // Task specific, moreNumSelections() should cover all bases
    void dateTimeEdit();
    void onValueChanged(int, int);
    void moreNumSelections_data();
    void moreNumSelections();
    void headerSwapIconset();
    void propagateEscapeKey();
public slots:
    void selectionWasChanged();
private:
    void setupTableItems();
    Q3Table *testWidget;
    bool isValueChanged, receivedSelectionChanged;
};

Q_DECLARE_METATYPE(Q3Table::SelectionMode);

#if 0
/* XPM */
static const char *fileopen[] = {
"    16    13        5            1",
". c #040404",
"# c #808304",
"a c None",
"b c #f3f704",
"c c #f3f7f3",
"aaaaaaaaa...aaaa",
"aaaaaaaa.aaa.a.a",
"aaaaaaaaaaaaa..a",
"a...aaaaaaaa...a",
".bcb.......aaaaa",
".cbcbcbcbc.aaaaa",
".bcbcbcbcb.aaaaa",
".cbcb...........",
".bcb.#########.a",
".cb.#########.aa",
".b.#########.aaa",
"..#########.aaaa",
"...........aaaaa"
};
#endif

tst_Q3Table::tst_Q3Table()
    : isValueChanged( false )
{
}

void tst_Q3Table::initTestCase()
{
}

void tst_Q3Table::cleanupTestCase()
{
}

void tst_Q3Table::init()
{
    // Reset the table
    testWidget = new Q3Table(4,4,0,"testWidget");
    testWidget->show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager( testWidget );
#endif
}

void tst_Q3Table::cleanup()
{
    testWidget->hide();
    delete testWidget;
    testWidget = 0;
}

/*
    Function that will popuplate the testWidget with a variety of Q3TableItems
*/

void tst_Q3Table::setupTableItems()
{
    QStringList list;
    list << "A" << "B";
    Q3ComboTableItem *cbi = new Q3ComboTableItem(testWidget, list);
    testWidget->setItem(1, 1, cbi);
    Q3ComboTableItem *ecbi = new Q3ComboTableItem(testWidget, list, TRUE);
    testWidget->setItem(1, 2, ecbi);
    Q3CheckTableItem *chi = new Q3CheckTableItem(testWidget, "Check");
    testWidget->setItem(2, 2, chi);
    Q3TableItem *ne = new Q3TableItem(testWidget, Q3TableItem::Never, "Never");
    testWidget->setItem(0, 0, ne);
    Q3TableItem *ot = new Q3TableItem(testWidget, Q3TableItem::OnTyping, "On Typing");
    testWidget->setItem(0, 1, ot);
    Q3TableItem *wc = new Q3TableItem(testWidget, Q3TableItem::WhenCurrent, "When Current");
    testWidget->setItem(0, 2, wc);
    Q3TableItem *al = new Q3TableItem(testWidget, Q3TableItem::Always, "Always");
    testWidget->setItem(0, 3, al);
}

#define CELL_TEXT(r, c) QString::number( r ) + "/" + QString::number( c )

void tst_Q3Table::setNumRowsColsItems()
{
    Q3Table *t = new Q3Table( 0, "test_table" );

    const int initial_num_rows = 4;
    const int initial_num_cols = 4;

    const int larger_num_rows = 8;
    const int larger_num_cols = 8;

    const int smaller_num_rows = 3;
    const int smaller_num_cols = 3;

    t->setNumRows( initial_num_cols );
    t->setNumCols( initial_num_cols );

    QCOMPARE( t->numRows(), initial_num_rows );
    QCOMPARE( t->numCols(), initial_num_cols );

    int r, c;
    for ( r = 0; r < initial_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    t->setText( r, c, CELL_TEXT( r, c ) );
	}
    }

    for ( r = 0; r < initial_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    QCOMPARE( t->text( r, c ), CELL_TEXT( r, c ) );
	}
    }

    t->setNumRows( larger_num_rows );

    for ( r = 0; r < larger_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    if ( r < initial_num_rows && c < initial_num_cols ) {
		QCOMPARE( t->text( r, c ), CELL_TEXT( r, c ) );
	    } else {
		QCOMPARE( t->text( r, c ), QString() );
	    }
	}
    }

    t->setNumCols( larger_num_cols );

    for ( r = 0; r < larger_num_rows; ++r ) {
	for ( c = 0; c < larger_num_cols; ++c ) {
	    if ( r < initial_num_rows && c < initial_num_cols ) {
		QCOMPARE( t->text( r, c ), CELL_TEXT( r, c ) );
	    } else {
		QCOMPARE( t->text( r, c ), QString() );
	    }
	}
    }

    t->setNumRows( smaller_num_rows );

    for ( r = 0; r < smaller_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    QCOMPARE( t->text( r, c ), CELL_TEXT( r, c ) );
	}
    }

    t->setNumCols( smaller_num_cols );

    for ( r = 0; r < smaller_num_rows; ++r ) {
	for ( c = 0; c < smaller_num_cols; ++c ) {
	    QCOMPARE( t->text( r, c ), CELL_TEXT( r, c ) );
	}
    }

    delete t;
}

void tst_Q3Table::setNumRowsColsWidgets()
{
    Q3Table *t = new Q3Table( 0, "test_table" );

    const int initial_num_rows = 4;
    const int initial_num_cols = 4;

    const int larger_num_rows = 8;
    const int larger_num_cols = 8;

    const int smaller_num_rows = 3;
    const int smaller_num_cols = 3;

    t->setNumRows( initial_num_cols );
    t->setNumCols( initial_num_cols );

    QCOMPARE( t->numRows(), initial_num_rows );
    QCOMPARE( t->numCols(), initial_num_cols );

    int r, c;

    for ( r = 0; r < initial_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    t->setCellWidget( r, c, new QWidget( t, CELL_TEXT( r, c ) ) );
	}
    }

    for ( r = 0; r < initial_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    QCOMPARE( QString( t->cellWidget( r, c )->name() ), CELL_TEXT( r, c ) );
	}
    }

    t->setNumRows( larger_num_rows );

    for ( r = 0; r < larger_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    if ( r < initial_num_rows && c < initial_num_cols ) {
		QCOMPARE( QString( t->cellWidget( r, c )->name() ), CELL_TEXT( r, c ) );
	    } else {
		QCOMPARE( t->cellWidget( r, c ), (QWidget*)0 );
	    }
	}
    }

    t->setNumCols( larger_num_cols );

    for ( r = 0; r < larger_num_rows; ++r ) {
	for ( c = 0; c < larger_num_cols; ++c ) {
	    if ( r < initial_num_rows && c < initial_num_cols ) {
		QCOMPARE( QString( t->cellWidget( r, c )->name() ), CELL_TEXT( r, c ) );
	    } else {
		QCOMPARE( t->cellWidget( r, c ), (QWidget*)0 );
	    }
	}
    }

    t->setNumRows( smaller_num_rows );

    for ( r = 0; r < smaller_num_rows; ++r ) {
	for ( c = 0; c < initial_num_cols; ++c ) {
	    QCOMPARE( QString( t->cellWidget( r, c )->name() ), CELL_TEXT( r, c ) );
	}
    }

    t->setNumCols( smaller_num_cols );

    for ( r = 0; r < smaller_num_rows; ++r ) {
	for ( c = 0; c < smaller_num_cols; ++c ) {
	    QCOMPARE( QString( t->cellWidget( r, c )->name() ), CELL_TEXT( r, c ) );
	}
    }

    delete t;
}


void tst_Q3Table::selection()
{
    Q3Table *t = new Q3Table( 20, 20, 0, "test_table" );

    t->selectCells( 0, 0, 50, 50 );

    Q3TableSelection sel( 60, 60, 100, 100 );
    t->addSelection( sel );

    t->show();
    QCOMPARE( t->numSelections(), 2 );
    delete t;
}

void tst_Q3Table::selectComboTableItem_data()
{
    QTest::addColumn<bool>("editable");

    QTest::newRow( "editable" ) << TRUE;
    QTest::newRow( "non-editable" ) << FALSE;

}

class MyTable : public Q3Table
{
    // Conveience subclass used to be able to call isEditing()
public:
    MyTable(int rows,int cols,QWidget *parent = 0,const char *name = 0)
	: Q3Table(rows,cols,parent,name) {}
    bool isEditing() {
	return Q3Table::isEditing();
    }
};

void tst_Q3Table::selectComboTableItem()
{
#if 0
    MyTable table(4,4,0,"subclassTestWidget");
    // Test for task #25026
    QFETCH( bool, editable );

    QStringList strings;
    strings << "One" << "Two" << "Three" << "Four";
    QComboTableItem *cbi = new QComboTableItem(testWidget,strings,editable);
    table.setItem(0,0,cbi);
    table.hide();
    table.show();
    QComboBox *cb;
    if ( !editable )
	cb = (QComboBox*)table.viewport()->child("qt_editor_cb","QComboBox");
    else
	cb = (QComboBox*)table.cellWidget(0,0);
    QVERIFY( cb );
    QCOMPARE(table.text(0,0),QString("One"));
    QTest::mouseClick(cb,QtTestCase::LeftButton,QtTestCase::NoButton,QPoint(cb->width()-7,cb->height()-5));
    QVERIFY(cb->listBox()->isVisible());
    QTest::mouseClick(cb->listBox()->viewport(),QtTestCase::LeftButton,QtTestCase::NoButton,QPoint(5,cb->listBox()->height()-5));
    QCOMPARE(table.text(0,0),QString("Four"));

    // Ensure the edit is accepted
    QTest::mouseClick(table.viewport(),QtTestCase::LeftButton,QtTestCase::NoButton,QPoint(table.width()-10,table.height()-10));
    QVERIFY(!table.isEditing());
    QCOMPARE(table.text(0,0),QString("Four"));

    // Now check that Escape will revert any changes
    QTest::mouseClick(cb,QtTestCase::LeftButton,QtTestCase::NoButton,QPoint(cb->width()-7,cb->height()-5));
    QVERIFY(cb->listBox()->isVisible());
    QTest::mouseClick(cb->listBox()->viewport(),QtTestCase::LeftButton,QtTestCase::NoButton,QPoint(5,5));
    QCOMPARE(cb->currentText(),QString("One"));
    QTest::keyClick(cb,Qt::Key_Escape);
    QVERIFY(!table.isEditing());
    QCOMPARE(table.text(0,0),QString("Four"));
#else
    QSKIP("This test will fail at the moment since the way qtestlib iss handling mouse events is broken",SkipAll);
#endif
}

void tst_Q3Table::pageUpDownNavigation_data()
{
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");
    QTest::addColumn<bool>("activeSelection");
    QTest::addColumn<int>("pgUpTopRow");
    QTest::addColumn<int>("pgUpBottomRow");
    QTest::addColumn<int>("pgUpLeftCol");
    QTest::addColumn<int>("pgUpRightCol");
    QTest::addColumn<int>("pgDownTopRow");
    QTest::addColumn<int>("pgDownBottomRow");
    QTest::addColumn<int>("pgDownLeftCol");
    QTest::addColumn<int>("pgDownRightCol");

    QTest::newRow("NoSelection") << Q3Table::NoSelection << FALSE << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("SingleSelection") << Q3Table::Single << FALSE << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MultiSelection") << Q3Table::Multi << FALSE << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("SingleRowSelection") << Q3Table::SingleRow << TRUE << 0 << 0 << 0 << 3 << 3 << 3 << 0 << 3;
    QTest::newRow("MultiRowSelection") << Q3Table::MultiRow << TRUE << 0 << 0 << 0 << 3 << 3 << 3 << 0 << 3;
}

void tst_Q3Table::pageUpDownNavigation()
{
    setupTableItems();

    // Test for task #25842
    QFETCH(Q3Table::SelectionMode,selectionMode);
    QFETCH(bool,activeSelection);
    QFETCH(int,pgUpTopRow);
    QFETCH(int,pgUpBottomRow);
    QFETCH(int,pgUpLeftCol);
    QFETCH(int,pgUpRightCol);
    QFETCH(int,pgDownTopRow);
    QFETCH(int,pgDownBottomRow);
    QFETCH(int,pgDownLeftCol);
    QFETCH(int,pgDownRightCol);

    // Needed to force the table to have "no selection"
    testWidget->setCurrentCell(-1,-1);

    testWidget->setSelectionMode(selectionMode);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(testWidget->viewport(),Qt::Key_PageUp);
#ifdef WAITS
    QTest::qWait(50);
#endif
    Q3TableSelection ts = testWidget->selection(testWidget->currentSelection());
    QVERIFY(ts.isActive() == activeSelection);
    if (activeSelection) {
	QVERIFY(ts.topRow() == pgUpTopRow);
	QVERIFY(ts.bottomRow() == pgUpBottomRow);
	QVERIFY(ts.leftCol() == pgUpLeftCol);
	QVERIFY(ts.rightCol() == pgUpRightCol);
    }
    QTest::keyClick(testWidget->viewport(),Qt::Key_PageDown);
#ifdef WAITS
    QTest::qWait(50);
#endif
    ts = testWidget->selection(testWidget->currentSelection());
    QVERIFY(ts.isActive() == activeSelection);
    if (activeSelection) {
	QVERIFY(ts.topRow() == pgDownTopRow);
	QVERIFY(ts.bottomRow() == pgDownBottomRow);
	QVERIFY(ts.leftCol() == pgDownLeftCol);
	QVERIFY(ts.rightCol() == pgDownRightCol);
    }
}

void tst_Q3Table::simpleKeyboardNavigation()
{
    QApplication::setActiveWindow(testWidget);
    QTRY_COMPARE(QApplication::activeWindow(), testWidget);
    QWidget *w;

    // Test for task #24726
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::mouseClick(testWidget->viewport(),
               Qt::LeftButton, Qt::NoModifier,
               QPoint(testWidget->columnPos(0) + 3, testWidget->rowPos(0) + 3));
#ifdef WAITS
    QTest::qWait(50);
#endif

    QCOMPARE(testWidget->currentColumn(), 0);
    QCOMPARE(testWidget->currentRow(), 0);

    QTest::keyClick(testWidget->viewport(), Qt::Key_T);

    // After the first keyevent, the table starts editing the item
    w = testWidget->cellWidget(0, 0);
    QVERIFY(w);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_E);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_C);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_H);

#ifdef WAITS
    QTest::qWait(50);
#endif

#ifdef Q_WS_MAC // Mac lineedits grab Up and Down
    QTest::keyClick(w, Qt::Key_Return);
#else
    QTest::keyClick(testWidget->viewport(), Qt::Key_Down);
#endif

#ifdef WAITS
    QTest::qWait(50);
#endif

    QCOMPARE(testWidget->text(0,0), QString("tech"));
    QCOMPARE(testWidget->currentColumn(), 0);
    QCOMPARE(testWidget->currentRow(), 1);

    QTest::keyClick(testWidget->viewport(), Qt::Key_A);
    w = testWidget->cellWidget(0, 1);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_B);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_C);

#ifdef WAITS
    QTest::qWait(50);
#endif

#ifdef Q_WS_MAC // Mac lineedits grab Up and Down
    QTest::keyClick(w, Qt::Key_Return);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Up);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Up);
#else
    QTest::keyClick(w, Qt::Key_Up);
#endif

#ifdef WAITS
    QTest::qWait(50);
#endif

    QCOMPARE(testWidget->text(1,0), QString("abc"));
    QCOMPARE(testWidget->currentColumn(), 0);
    QCOMPARE(testWidget->currentRow(), 0);
    w = testWidget->cellWidget(0, 0);

    QTest::keyClick(w, Qt::Key_D);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_E);

#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::keyClick(w, Qt::Key_F);

#ifdef WAITS
    QTest::qWait(50);
#endif

#ifdef Q_WS_MAC // Mac lineedits grab Up and Down
    QTest::keyClick(w, Qt::Key_Return);
#else
    QTest::keyClick(w, Qt::Key_Down);
#endif

#ifdef WAITS
    QTest::qWait(50);
#endif

    QCOMPARE(testWidget->text(0,0), QString("def"));
    QCOMPARE(testWidget->currentColumn(), 0);
    QCOMPARE(testWidget->currentRow(), 1);
}

void tst_Q3Table::setRowStretchable()
{
#if 1
    QSKIP("This test currently fails on all versions", SkipAll);
#endif
    // Part of test for task 24471
    // Only tests TRUE case, need FALSE case written
    // Also needs to test the case where the user tries to resize it
    testWidget->setRowStretchable(0,TRUE);
    testWidget->show();
    int height = testWidget->rowHeight(0);
    testWidget->setRowHeight(0,height * 2);
    QCOMPARE(height,testWidget->rowHeight(0));
}

void tst_Q3Table::setColumnStretchable()
{
#if 1
    QSKIP("This test currently fails on all versions", SkipAll);
#endif
    // Part of test for task 24471
    // Only tests TRUE case, need FALSE case written
    // Also needs to test the case where the user tries to resize it
    testWidget->setColumnStretchable(0,TRUE);
    testWidget->show();
    int width = testWidget->columnWidth(0);
    testWidget->setColumnWidth(0,width * 2);
    QCOMPARE(width,testWidget->columnWidth(0));
}

void tst_Q3Table::selectRow_data()
{
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");
    QTest::addColumn<bool>("activeSelection");
    QTest::addColumn<int>("topRow");
    QTest::addColumn<int>("bottomRow");
    QTest::addColumn<int>("leftCol");
    QTest::addColumn<int>("rightCol");

    QTest::newRow("NoSelection") << Q3Table::NoSelection << TRUE << 1 << 1 << 0 << 3;
    QTest::newRow("SingleSelection") << Q3Table::Single << TRUE << 1 << 1 << 0 << 3;
    QTest::newRow("MultiSelection") << Q3Table::Multi << TRUE << 1 << 1 << 0 << 3;
    QTest::newRow("SingleRowSelection") << Q3Table::SingleRow << TRUE << 1 << 1 << 0 << 3;
    QTest::newRow("MultiRowSelection") << Q3Table::MultiRow << TRUE << 1 << 1 << 0 << 3;
}

void tst_Q3Table::selectRow()
{
    QFETCH(Q3Table::SelectionMode,selectionMode);
    QFETCH(bool,activeSelection);
    QFETCH(int,topRow);
    QFETCH(int,bottomRow);
    QFETCH(int,leftCol);
    QFETCH(int,rightCol);

    testWidget->clearSelection();
    testWidget->setSelectionMode(selectionMode);
    testWidget->selectRow(1);

    int current = testWidget->currentSelection();
    QVERIFY(current != -1);
    Q3TableSelection ts = testWidget->selection(current);

    QCOMPARE(ts.isActive(), activeSelection);

    if (activeSelection) {
        QCOMPARE(ts.topRow(), topRow);
	QCOMPARE(ts.bottomRow(), bottomRow);
	QCOMPARE(ts.leftCol(), leftCol);
	QCOMPARE(ts.rightCol(), rightCol);
    }

    // make sure selection is cleared
    testWidget->clearSelection();
    current = testWidget->currentSelection();
    QVERIFY(current == -1);

    testWidget->selectRow(1);
    QEXPECT_FAIL("SingleRowSelection", "This currently fails in all versions", Abort);
    QVERIFY(testWidget->numSelections() > 0); // selectRow in SingleRowSelection doesn't select anything (see qtable.cpp)
    current = testWidget->currentSelection();
    QVERIFY(current != -1);
    ts = testWidget->selection(current);

    QCOMPARE(ts.isActive(), activeSelection);

    if (activeSelection) {
	QCOMPARE(ts.topRow(), topRow);
	QCOMPARE(ts.bottomRow(), bottomRow);
	QCOMPARE(ts.leftCol(), leftCol);
	QCOMPARE(ts.rightCol(), rightCol);
    }
}

void tst_Q3Table::selectColumn_data()
{
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");
    QTest::addColumn<bool>("activeSelection");
    QTest::addColumn<int>("topRow");
    QTest::addColumn<int>("bottomRow");
    QTest::addColumn<int>("leftCol");
    QTest::addColumn<int>("rightCol");

    QTest::newRow("NoSelection") << Q3Table::NoSelection << TRUE << 0 << 3 << 1 << 1;
    QTest::newRow("SingleSelection") << Q3Table::Single << TRUE << 0 << 3 << 1 << 1;
    QTest::newRow("MultiSelection") << Q3Table::Multi << TRUE << 0 << 3 << 1 << 1;
    QTest::newRow("SingleRowSelection") << Q3Table::SingleRow << TRUE << 0 << 3 << 1 << 1;
    QTest::newRow("MultiRowSelection") << Q3Table::MultiRow << TRUE << 0 << 3 << 1 << 1;
}

void tst_Q3Table::selectColumn()
{
    QFETCH(Q3Table::SelectionMode,selectionMode);
    QFETCH(bool,activeSelection);
    QFETCH(int,topRow);
    QFETCH(int,bottomRow);
    QFETCH(int,leftCol);
    QFETCH(int,rightCol);

    testWidget->setSelectionMode(selectionMode);
    testWidget->selectColumn(1);
    Q3TableSelection ts = testWidget->selection(testWidget->currentSelection());
    QCOMPARE(ts.isActive(), activeSelection);

    if (activeSelection) {
	QCOMPARE(ts.topRow(), topRow);
	QCOMPARE(ts.bottomRow(), bottomRow);
	QCOMPARE(ts.leftCol(), leftCol);
	QCOMPARE(ts.rightCol(), rightCol);
    }

    testWidget->clearSelection();
    ts = testWidget->selection(testWidget->currentSelection());
    QVERIFY(!ts.isActive());

    testWidget->selectColumn(1);
    ts = testWidget->selection(testWidget->currentSelection());
    QCOMPARE(ts.isActive(), activeSelection);
    if (activeSelection) {
        QCOMPARE(ts.topRow(), topRow);
	QCOMPARE(ts.bottomRow(), bottomRow);
	QCOMPARE(ts.leftCol(), leftCol);
	QCOMPARE(ts.rightCol(), rightCol);
    }
}

void tst_Q3Table::cellNavigation()
{
#if 0
    setupTableItems();
#endif

    testWidget->setCurrentCell(0, 0);

    // Check tab key first
    QCOMPARE(testWidget->currentRow(), 0);
    QCOMPARE(testWidget->currentColumn(), 0);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Tab);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->currentRow(), 0);
    QCOMPARE(testWidget->currentColumn(), 1);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Tab);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->currentRow(), 0);
    QCOMPARE(testWidget->currentColumn(), 2);
    QTest::keyClick(testWidget->viewport(), Qt::Key_Tab);
#ifdef WAITS
    QTest::qWait(50);
#endif

    // One more, and it should stay where it is
    QTest::keyClick(testWidget->viewport(), Qt::Key_Tab);
    QCOMPARE(testWidget->currentRow(), 0);
    QCOMPARE(testWidget->currentColumn(), 3);

    testWidget->setCurrentCell(3,3);

    // Now check backtab key
    QCOMPARE(testWidget->currentRow(), 3);
    QCOMPARE(testWidget->currentColumn(), 3);
    QTest::keyClick(testWidget->viewport(), Qt::Key_BackTab);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->currentRow(), 3);
    QCOMPARE(testWidget->currentColumn(), 2);
    QTest::keyClick(testWidget->viewport(), Qt::Key_BackTab);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->currentRow(), 3);
    QCOMPARE(testWidget->currentColumn(), 1);
    QTest::keyClick(testWidget->viewport(), Qt::Key_BackTab);
#ifdef WAITS
    QTest::qWait(50);
#endif

    // One more, and it should stay where it is
    QTest::keyClick(testWidget->viewport(), Qt::Key_BackTab);
    QCOMPARE(testWidget->currentRow(), 3);
    QCOMPARE(testWidget->currentColumn(), 0);
#ifdef WAITS
    QTest::qWait(50);
#endif

    testWidget->setCurrentCell(0, 0);

    // Now check enter key
    for (int row = 0; row < 3; ++row) {
        QCOMPARE(testWidget->currentRow(), row);
        QCOMPARE(testWidget->currentColumn(), 0);
        QTest::keyClick(testWidget->viewport(), Qt::Key_Enter);
#ifdef WAITS
        QTest::qWait(50);
#endif
    }
    // One more, and it should go to back
    QTest::keyClick(testWidget->viewport(), Qt::Key_Enter);
    QCOMPARE(testWidget->currentRow(), 0);
    QCOMPARE(testWidget->currentColumn(), 1);
}

void tst_Q3Table::cellNavigationWhileEditing()
{
    // This is basically the cellNavigation() test
    // with the current cell being edited before
    // navigation.
#if 0
    setupTableItems();

    int a, b;
    QPoint contentPos;
    // Check tab key first
    for (a = 0;a < 4;a++) {
	for (b = 0;b < 4;b++) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Tab);
	}
    }
    // One more should put it at the beginning again
    QTest::keyClick(testWidget->viewport(),Qt::Key_Tab);
    QCOMPARE(testWidget->currentRow(),0);
    QCOMPARE(testWidget->currentColumn(),0);

    testWidget->setCurrentCell(3,3);
    // Now check backtab key
    for (a = 3;a > -1;a--) {
	for (b = 3;b > -1;b--) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_BackTab);
	}
    }
    // One more should put it at the end again
    QTest::keyClick(testWidget->viewport(),Qt::Key_BackTab);
    QCOMPARE(testWidget->currentRow(),3);
    QCOMPARE(testWidget->currentColumn(),3);

    testWidget->setCurrentCell(0,0);
    // Now check enter key
    for (a = 0;a < 4;a++) {
	for (b = 0;b < 4;b++) {
	    QCOMPARE(testWidget->currentColumn(),a);
	    QCOMPARE(testWidget->currentRow(),b);
	    contentPos = QPoint(testWidget->columnPos(a) + 3, testWidget->rowPos(b) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Enter);
	}
    }
    // One more should put it at the beginning again
    QTest::keyClick(testWidget->viewport(),Qt::Key_Enter);
    QCOMPARE(testWidget->currentRow(),0);
    QCOMPARE(testWidget->currentColumn(),0);

    // Reset it back to the top
    contentPos = QPoint(testWidget->columnPos(0) + 3, testWidget->rowPos(0) + 3);
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
	testWidget->contentsToViewport(contentPos));

    // Now check down key
    for (a = 0;a < 4;a++) {
	for (b = 0;b < 4;b++) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Down);
	    if (a == 3) {
		// Check it hasn't moved from the bottom even
		// after clicking down
		QCOMPARE(testWidget->currentRow(),a);
		QCOMPARE(testWidget->currentColumn(),b);
		// Put it in correct place for next check
		QTest::keyClick(testWidget->viewport(),Qt::Key_Home);
		QTest::keyClick(testWidget->viewport(),Qt::Key_Right);
	    }
	}
    }

    // Start at the bottom now
    contentPos = QPoint(testWidget->columnPos(0) + 3, testWidget->rowPos(3) + 3);
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
	testWidget->contentsToViewport(contentPos));

    // Now check up key
    for (a = 3;a > -1;a--) {
	for (b = 3;b > -1;b--) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Up);
	    if (a == 0) {
		// Check it hasn't moved from the bottom even
		// after clicking down
		QCOMPARE(testWidget->currentRow(),a);
		QCOMPARE(testWidget->currentColumn(),b);
		// Put it in correct place for next check
		QTest::keyClick(testWidget->viewport(),Qt::Key_End);
		QTest::keyClick(testWidget->viewport(),Qt::Key_Right);
	    }
	}
    }

    // Reset it back to the top
    contentPos = QPoint(testWidget->columnPos(0) + 3, testWidget->rowPos(0) + 3);
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
	testWidget->contentsToViewport(contentPos));

    // Now check right key
    for (a = 0;a < 4;a++) {
	for (b = 0;b < 4;b++) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Right);
	    if (b == 3) {
		// Check it hasn't moved from the bottom even
		// after clicking down
		QCOMPARE(testWidget->currentRow(),a);
		QCOMPARE(testWidget->currentColumn(),b);
		// Put it in correct place for next check
		QTest::keyClick(testWidget->viewport(),Qt::Key_Home);
		QTest::keyClick(testWidget->viewport(),Qt::Key_Down);
	    }
	}
    }

    // Start it at the far left
    contentPos = QPoint(testWidget->columnPos(3) + 3, testWidget->rowPos(0) + 3);
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
	testWidget->contentsToViewport(contentPos));

    // Now check up key
    for (a = 3;a > -1;a--) {
	for (b = 3;b > -1;b--) {
	    QCOMPARE(testWidget->currentRow(),a);
	    QCOMPARE(testWidget->currentColumn(),b);
	    contentPos = QPoint(testWidget->columnPos(b) + 3, testWidget->rowPos(a) + 3);
	    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoButton,
		testWidget->contentsToViewport(contentPos));
	    if(a == 0 && b == 2) // On typing item
		QTest::keyClick(testWidget->viewport(), Qt::Key_A);
	    QTest::keyClick(testWidget->viewport(),Qt::Key_Left);
	    if (b == 0) {
		// Check it hasn't moved from the bottom even
		// after clicking down
		QCOMPARE(testWidget->currentRow(),a);
		QCOMPARE(testWidget->currentColumn(),b);
		// Put it in correct place for next check
		QTest::keyClick(testWidget->viewport(),Qt::Key_End);
		QTest::keyClick(testWidget->viewport(),Qt::Key_Down);
	    }
	}
    }
#else
    QSKIP( "This test currently fails on all versions", SkipAll);
#endif
}

void tst_Q3Table::removeRow_data()
{
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");

    QTest::newRow("NoSelection") << Q3Table::NoSelection;
    QTest::newRow("SingleSelection") << Q3Table::Single;
    QTest::newRow("MultiSelection") << Q3Table::Multi;
    QTest::newRow("SingleRowSelection") << Q3Table::SingleRow;
    QTest::newRow("MultiRowSelection") << Q3Table::MultiRow;
}


void tst_Q3Table::removeRow()
{
    QFETCH(Q3Table::SelectionMode, selectionMode);

#if 1
    if(selectionMode == Q3Table::SingleRow
       || selectionMode == Q3Table::MultiRow)
	QSKIP("removeRow() is expected to fail in SingleRow/MultiRow selection modes in all versions", SkipAll);
#endif
    // This tests task 26419


    setupTableItems();
    testWidget->setSelectionMode(selectionMode);
    for(int a = 3;a > -1;a--) {
	QPoint contentPos(testWidget->columnPos(0) + 3, testWidget->rowPos(a) + 3);
	QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier,
	    testWidget->contentsToViewport(contentPos));
#ifdef WAITS
    QTest::qWait(50);
#endif
	if (selectionMode != Q3Table::NoSelection) {
	    Q3TableSelection ts = testWidget->selection(testWidget->currentSelection());
	    QVERIFY(ts.isActive());
	    QVERIFY(ts.topRow() == a);
	    QVERIFY(ts.bottomRow() == a);
	}
	testWidget->removeRow(a);
	// a is actually now the number of rows because the row sections are
	// zero-based
	QVERIFY(testWidget->numRows() == a);
	QVERIFY(!testWidget->selection(testWidget->currentSelection()).isActive());
    }
}


class SwapRowMemoryLeakTableItem : public Q3TableItem
{
public:
    SwapRowMemoryLeakTableItem(Q3Table * table, const QString & text, bool * deletionFlag)
	: Q3TableItem(table, OnTyping, text)
    {
	this->deletionFlag = deletionFlag;
    }

    ~SwapRowMemoryLeakTableItem()
    {
	*deletionFlag = TRUE;
    }

private:
    bool * deletionFlag;
};


void tst_Q3Table::swapRowMemoryLeak()
{
    //Although this should fail due to code in Q3Table::swapRows() it dose not becuse it is saved
    //in ~Q3Table
    Q3Table * quickTable = new Q3Table(2, 1);


    bool i1deletionFlag = FALSE;
    SwapRowMemoryLeakTableItem * i1 = new SwapRowMemoryLeakTableItem(quickTable,"item 1", &i1deletionFlag);
    quickTable->setItem(0, 0, i1);
    bool i2deletionFlag = FALSE;
    SwapRowMemoryLeakTableItem * i2 = new SwapRowMemoryLeakTableItem(quickTable,"item 2", &i2deletionFlag);
    quickTable->setItem(1, 0, i2);

    quickTable->swapRows(0, 1);

   // quickTable->removeRow(0);

    delete quickTable;

    QVERIFY(i1deletionFlag);
    QVERIFY(i2deletionFlag);
}

void tst_Q3Table::insertRows_data()
{
    QTest::addColumn<int>("rowToInsertAt");
    QTest::addColumn<int>("numberOfRowsToInsert");

    QTest::newRow("firstRowOneRow") << 1 << 1;
    QTest::newRow("secondRowTenRows") << 2 << 10;
    QTest::newRow("thirdRowNoRows") << 3 << 0;
    QTest::newRow("zerothRowNegativeRows") << 0 << -1;
}

void tst_Q3Table::insertRows()
{
    QFETCH(int, rowToInsertAt);
    QFETCH(int, numberOfRowsToInsert);

    int i = 0;
    for (i = 0;i < 4;i++)
        testWidget->setText(i, 0, QString("Item %0,0").arg(i));

    testWidget->insertRows(rowToInsertAt, numberOfRowsToInsert);
    if(numberOfRowsToInsert <= 0) {
	QVERIFY(testWidget->numRows() == 4);
    } else {
	QVERIFY(testWidget->numRows() == 4 + numberOfRowsToInsert);
    }

    int rowNumber = 0;
    for (i = 0;i < rowToInsertAt;i++) {
	QCOMPARE(testWidget->text(i, 0), QString("Item %0,0").arg(rowNumber));
	rowNumber++;
    }

    if (numberOfRowsToInsert > 0) {
	for (i = rowToInsertAt;i < rowToInsertAt + numberOfRowsToInsert;i++)
	    QCOMPARE(testWidget->text(i, 0), QString());

	for (i = rowToInsertAt + numberOfRowsToInsert;i < 4 + numberOfRowsToInsert;i++) {
	    QCOMPARE(testWidget->text(i, 0), QString("Item %0,0").arg(rowNumber));
	    rowNumber++;
	}
    }
}

class EditCheckQ3Table : public Q3Table
{
public:
    int endEditCount;
    int beginEditCount;

    EditCheckQ3Table(int r, int c, QWidget *parent = 0, const char *name = 0)
        : Q3Table(r, c, parent, name)
    {
	endEditCount = beginEditCount = 0;
    }
protected:
    void endEdit(int row, int col, bool accept, bool replace)
    {
	endEditCount++;
	Q3Table::endEdit(row, col, accept, replace);
    }
    QWidget * beginEdit(int row, int col, bool replace)
    {
	beginEditCount++;
	return Q3Table::beginEdit(row, col, replace);
    }
};

void tst_Q3Table::editCheck()
{
    // Test for task 28086
#if 1
    QSKIP("This test currently fails on all versions", SkipAll);
#endif

    EditCheckQ3Table table(10, 10, 0);
    table.show();
    QApplication::setActiveWindow(&table);
    QTest::qWaitForWindowShown(&table);
    QTRY_COMPARE(QApplication::activeWindow(), &table);
    table.setCurrentCell(0, 0);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(table.viewport(), Qt::Key_T);
#ifdef WAITS
    QTest::qWait(50);
#endif
    // After the first keyevent, the table starts editing the item
    QLineEdit *le = qFindChild<QLineEdit *>(testWidget->viewport(), "qt_lineeditor");
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(le, Qt::Key_Enter);
    le = 0;
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(table.beginEditCount, 1);
    QEXPECT_FAIL("", "this fails because endEdit is called for the old current item when we set a new one", Abort);
    QCOMPARE(table.endEditCount, 1);
}

void tst_Q3Table::setCellWidgetFocus()
{
#define CELLWIDGET_CHEAT 1
    // Test for task 27683
    int numRows = testWidget->numRows();
    int numCols = testWidget->numCols();
    testWidget->setNumRows(1);
    testWidget->setNumCols(1);
#if CELLWIDGET_CHEAT
    qApp->processEvents();
#endif
    QLineEdit *lineedit = new QLineEdit(testWidget);
    testWidget->setCellWidget(0, 0, lineedit);
    testWidget->show();
    qApp->setActiveWindow(testWidget);
    lineedit->setFocus();
    QVERIFY(lineedit->hasFocus());
    testWidget->setNumRows(numRows);
    testWidget->setNumCols(numCols);
}

void tst_Q3Table::selectionWithMouse_data()
{
    QTest::addColumn<QPoint>("clickPos");
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");
    QTest::addColumn<int>("numSelections");
    QTest::addColumn<bool>("activeSelection");
    QTest::addColumn<int>("topRow");
    QTest::addColumn<int>("bottomRow");
    QTest::addColumn<int>("leftCol");
    QTest::addColumn<int>("rightCol");

    QTest::newRow("NoSelection") << QPoint(20, 15) << Q3Table::NoSelection << 0 << FALSE << -1 << -1 << -1 << -1;
    QTest::newRow("SingleSelection") << QPoint(20, 15) << Q3Table::Single << 1 << TRUE << 0 << 0 << 0 << 0;
    QTest::newRow("MultiSelection") << QPoint(20, 15) << Q3Table::Multi << 1 << TRUE << 0 << 0 << 0 << 0;
    QTest::newRow("SingleRowSelection") << QPoint(20, 15) << Q3Table::SingleRow << 1 << TRUE << 0 << 0 << 0 << 3;
    QTest::newRow("MultiRowSelection") << QPoint(20, 15) << Q3Table::MultiRow << 1 << TRUE << 0 << 0 << 0 << 3;
}

void tst_Q3Table::selectionWithMouse()
{
    QFETCH(QPoint, clickPos);
    QFETCH(Q3Table::SelectionMode, selectionMode);
    QFETCH(int, numSelections);
    QFETCH(bool, activeSelection);
    QFETCH(int, topRow);
    QFETCH(int, bottomRow);
    QFETCH(int, leftCol);
    QFETCH(int, rightCol);

    testWidget->setSelectionMode((Q3Table::SelectionMode)selectionMode);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::mouseClick(testWidget->viewport(), Qt::LeftButton, Qt::NoModifier, clickPos);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QVERIFY(testWidget->numSelections() == numSelections);
    Q3TableSelection ts = testWidget->selection(testWidget->currentSelection());
    QVERIFY(ts.isActive() == activeSelection);
    if (activeSelection) {
	QVERIFY(ts.topRow() == topRow);
	QVERIFY(ts.bottomRow() == bottomRow);
	QVERIFY(ts.leftCol() == leftCol);
	QVERIFY(ts.rightCol() == rightCol);
    }
}

class TimeTableItem : public Q3TableItem
{
public:
    TimeTableItem(Q3Table *parent=0)
	: Q3TableItem(parent, Q3TableItem::OnTyping, ""),
	  editorWasTimeEdit(false)
    { setReplaceable(false); }

    QWidget* createEditor() const
    {
        return new Q3TimeEdit(table()->viewport(), "time_edit");
    }

    void setContentFromEditor(QWidget* widget)
    {
        Q3TimeEdit *time_edit = ::qobject_cast<Q3TimeEdit*>(widget);
	if (!time_edit) {
	    editorWasTimeEdit = false;
	    Q3TableItem::setContentFromEditor(widget);
	    return;
	}
	editorWasTimeEdit = true;
        QTime res = time_edit->time();
        setText(QString::number(QTime().secsTo(res)));
    }

    bool editorWasTimeEdit;
};

void tst_Q3Table::onValueChanged(int, int)
{
    isValueChanged = true;
}

void tst_Q3Table::valueChanged()
{
    // task 27496
    isValueChanged = false;
    TimeTableItem *ti = new TimeTableItem(testWidget);
    testWidget->setItem(0, 0, ti);
    connect(testWidget,SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
    testWidget->show();
    QApplication::setActiveWindow(testWidget);
    QTest::qWaitForWindowShown(testWidget);
    QTRY_COMPARE(QApplication::activeWindow(), testWidget);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(testWidget->viewport(), Qt::Key_1);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTRY_VERIFY(qApp->focusWidget());
    QTest::keyClick(qApp->focusWidget(), Qt::Key_Enter);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QVERIFY(ti->editorWasTimeEdit);
    disconnect(testWidget,SIGNAL(valueChanged(int,int)),this,SLOT(onValueChanged(int,int)));
}

void tst_Q3Table::numSelections()
{
    // task 28017
    int numRows = testWidget->numRows();
    int numCols = testWidget->numCols();
    testWidget->setNumRows(10);
    testWidget->setNumCols(10);
    for (int i=0;i<10;++i)
	for (int j=0;j<10;++j)
	    testWidget->setItem(i,j,new Q3TableItem(testWidget,Q3TableItem::OnTyping,"Item"));
    QCOMPARE(testWidget->numSelections(),0);
    testWidget->clearSelection();
    QCOMPARE(testWidget->numSelections(),0);
    testWidget->selectRow(2);
    QCOMPARE(testWidget->numSelections(),1);
    testWidget->selectRow(4);
    QCOMPARE(testWidget->numSelections(),2);
    testWidget->selectRow(5);
    QCOMPARE(testWidget->numSelections(),3);
    testWidget->clearSelection();
    QCOMPARE(testWidget->numSelections(),0);
    testWidget->setNumRows(numRows);
    testWidget->setNumCols(numCols);
}

void tst_Q3Table::dateTimeEdit()
{
    // task 29002
    TimeTableItem *ti = new TimeTableItem(testWidget);
    testWidget->setItem(0, 0, ti);
    testWidget->show();
    QApplication::setActiveWindow(testWidget);
    QTest::qWaitForWindowShown(testWidget);
    QTRY_COMPARE(QApplication::activeWindow(), testWidget);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(testWidget->viewport(), Qt::Key_1);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTRY_VERIFY(qApp->focusWidget());
    QTest::keyClick(qApp->focusWidget(), Qt::Key_Enter);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QVERIFY(ti->editorWasTimeEdit);
}

void tst_Q3Table::moreNumSelections_data()
{
    QTest::addColumn<Q3Table::SelectionMode>("selectionMode");
    QTest::addColumn<int>("numSelectionsBeforeAnyRowClicked");
    QTest::addColumn<int>("numSelectionsAfter1stRowClicked");
    QTest::addColumn<int>("numSelectionsAfter3rdRowClicked");
    QTest::addColumn<int>("numSelectionsAfter3rdRowClickedAgain");

    QTest::newRow("NoSelection") << Q3Table::NoSelection << 0 << 0 << 0 << 0;
    QTest::newRow("SingleSelection") << Q3Table::Single << 0 << 1 << 1 << 1;
    QTest::newRow("MultiSelection") << Q3Table::Multi << 0 << 1 << 2 << 2;
    // in row selection modes the current item is always selected
    QTest::newRow("SingleRowSelection") << Q3Table::SingleRow << 1 << 1 << 1 << 1;
    QTest::newRow("MultiRowSelection") << Q3Table::MultiRow << 1 << 1 << 2 << 2;
}

void tst_Q3Table::moreNumSelections()
{
#if 0
    QFETCH(Q3Table::SelectionMode, selectionMode);
    QFETCH(int, numSelectionsBeforeAnyRowClicked);
    QFETCH(int, numSelectionsAfter1stRowClicked);
    QFETCH(int, numSelectionsAfter3rdRowClicked);
    QFETCH(int, numSelectionsAfter3rdRowClickedAgain);

    if((Q3Table::SelectionMode)selectionMode == Q3Table::Multi ||
       (Q3Table::SelectionMode)selectionMode == Q3Table::MultiRow) {
	QSKIP("numSelections() currently fails on all versions", SkipAll);
    }

    testWidget->setSelectionMode((Q3Table::SelectionMode)selectionMode);
    QCOMPARE(testWidget->numSelections(), numSelectionsBeforeAnyRowClicked);
#ifdef WAITS
    QTest::qWait(50);
#endif

    QTest::mouseClick(testWidget->verticalHeader(), QtTestCase::LeftButton, QtTestCase::NoButton,
	       QPoint(5, testWidget->verticalHeader()->sectionPos(0)+2));
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->numSelections(), numSelectionsAfter1stRowClicked);

    QTest::mouseClick(testWidget->verticalHeader(), QtTestCase::LeftButton, QtTestCase::NoButton,
	       QPoint(5, testWidget->verticalHeader()->sectionPos(2)+2));
#ifdef WAITS
    QTest::qWait(50);
#endif
    QEXPECT_FAIL("MultiSelection", "Will not fix this for 3.x", Abort);
    QEXPECT_FAIL("MultiRowSelection", "Will not fix this for 3.x", Abort);
    QCOMPARE(testWidget->numSelections(), numSelectionsAfter3rdRowClicked);

    QTest::mouseClick(testWidget->verticalHeader(), QtTestCase::LeftButton, QtTestCase::NoButton,
	       QPoint(5, testWidget->verticalHeader()->sectionPos(2)+2));
#ifdef WAITS
    QTest::qWait(50);
#endif
    QCOMPARE(testWidget->numSelections(), numSelectionsAfter3rdRowClickedAgain);
#else
    QSKIP( "This test is expected to fail on >= 3.2", SkipAll);
#endif
}

void tst_Q3Table::headerSwapIconset()
{
#if 0
    QIconSet is0, is1;
    QString hl0, hl1;
    if (testWidget->horizontalHeader()->iconSet(0))
	is0 = *testWidget->horizontalHeader()->iconSet(0);
    hl0 = testWidget->horizontalHeader()->label(0);
    if (testWidget->horizontalHeader()->iconSet(1))
	is1 = *testWidget->horizontalHeader()->iconSet(1);
    hl1 = testWidget->horizontalHeader()->label(1);

    testWidget->show();
    testWidget->horizontalHeader()->setLabel(1, QPixmap(fileopen), "Test");
    testWidget->swapColumns(0, 1, true);
    QVERIFY(!testWidget->horizontalHeader()->iconSet(1)
	   || testWidget->horizontalHeader()->iconSet(1)->isNull());

    testWidget->horizontalHeader()->setLabel(0, is0, hl0);
    testWidget->horizontalHeader()->setLabel(1, is1, hl1);
#else
    QSKIP( "This test fails in Qt 4", SkipAll);
#endif
}


class EscapeKeyDialog : public QDialog
{
public:
    EscapeKeyDialog() : QDialog(0), recievedEscape(false) {}
    bool recievedEscape;
protected:
    void keyPressEvent(QKeyEvent *e)
    {
	if (e->key() == Qt::Key_Escape)
	    recievedEscape = TRUE;
	QDialog::keyPressEvent(e);
    }
};

void tst_Q3Table::propagateEscapeKey()
{
#if 0
    EscapeKeyDialog ekd;
    Q3Table *table = new Q3Table(5, 5, &ekd);
    ekd.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&ekd);
#endif
    table->setCurrentCell(0,0);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(table->viewport(), Qt::Key_A);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QTest::keyClick(table->viewport(), Qt::Key_Escape);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QVERIFY(!ekd.recievedEscape);
    QVERIFY(table->text(0,0).isEmpty());
    QTest::keyClick(table->viewport(), Qt::Key_Escape);
#ifdef WAITS
    QTest::qWait(50);
#endif
    QVERIFY(ekd.recievedEscape);
#else
    QSKIP("This test fails for all versions of Qt", SkipAll);
#endif
}

void tst_Q3Table::removeSelection_selectionChanged()
{
    int numRows = testWidget->numRows();
    int numCols = testWidget->numCols();
    testWidget->setNumRows(3);
    testWidget->setNumCols(3);
    Q3TableSelection sel(1, 1, 2, 2);
    testWidget->addSelection(sel);
    QObject::connect(testWidget, SIGNAL(selectionChanged()),
		     this, SLOT(selectionWasChanged()));
    receivedSelectionChanged = false;
    testWidget->removeSelection(sel);
    qApp->processEvents();
    QVERIFY(receivedSelectionChanged);
    testWidget->setNumRows(numRows);
    testWidget->setNumCols(numCols);
    QObject::disconnect(testWidget, SIGNAL(selectionChanged()),
			this, SLOT(selectionWasChanged()));
}

void tst_Q3Table::selectionWasChanged()
{
    receivedSelectionChanged = true;
}

QTEST_MAIN(tst_Q3Table)
#include "tst_q3table.moc"

