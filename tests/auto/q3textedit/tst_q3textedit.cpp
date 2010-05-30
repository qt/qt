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

#include <limits.h>

#include <q3textedit.h>
#include <q3textedit.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qimage.h>
#include <private/q3richtext_p.h>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3TextEdit : public QObject
{
    Q_OBJECT
public:
    tst_Q3TextEdit();
    virtual ~tst_Q3TextEdit();


public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void getSetCheck();
    void redoAvailableEmitted( bool );
    void undoAvailableEmitted( bool );
    void copyAvailableEmitted( bool );

    void cursorPosition_data();
    void cursorPosition();
    void doKeyboardAction_data();
    void doKeyboardAction();
    void text();
    void text_data();

    void insert();

    void keyClicks_data();
    void keyClicks();

    //problem specific tests
    void copyPaste();
    void copyPaste_data();

    void setReadOnly();

    void find_data();
    void find();
    void findSC();
    void cut();

    void clear();
    void selectAll();
    void getSelection_data();
    void getSelection();

    void setCurrentFont();

    void undoRedo();

    void length_data();
    void length();
    void findBackwards();
    void anchorTest();

private:
    bool nativeClipboardWorking();
    Q3TextEdit *textEdit;
    bool redoA, undoA, copyA;

};

bool tst_Q3TextEdit::nativeClipboardWorking()
{
#ifdef Q_WS_MAC
    PasteboardRef pasteboard;
    OSStatus status = PasteboardCreate(0, &pasteboard);
    if (status == noErr)
        CFRelease(pasteboard);
    return status == noErr;
#endif
    return true;
}



class My3TextEdit : public Q3TextEdit
{
public:
    My3TextEdit() : Q3TextEdit() {}
    void setDocument(Q3TextDocument *doc) { Q3TextEdit::setDocument(doc); }
    Q3TextDocument *document() { return Q3TextEdit::document(); }
};

// Testing get/set functions
void tst_Q3TextEdit::getSetCheck()
{
    My3TextEdit obj1;
    // int Q3TextEdit::maxLogLines()
    // void Q3TextEdit::setMaxLogLines(int)
    obj1.setMaxLogLines(0);
    QCOMPARE(obj1.maxLogLines(), 0);
    obj1.setMaxLogLines(INT_MIN);
    QCOMPARE(obj1.maxLogLines(), -1);
    obj1.setMaxLogLines(INT_MAX);
    QCOMPARE(obj1.maxLogLines(), INT_MAX);

    // Q3TextDocument * Q3TextEdit::document()
    // void Q3TextEdit::setDocument(Q3TextDocument *)
    Q3TextDocument *var2 = new Q3TextDocument(0);
    obj1.setDocument(var2);
    QCOMPARE(obj1.document(), var2);
    // Should've done as QTextEdit, and created a new document, if you setDocument(0).
    // Unfortunately it doesn't, and we cannot change it.
    obj1.setDocument((Q3TextDocument *)0);
    QCOMPARE(obj1.document(), var2);
    // delete var2; // No delete, since Q3TextEdit takes ownership
}

typedef QList<int> IntList;
Q_DECLARE_METATYPE(IntList)
Q_DECLARE_METATYPE(Q3TextEdit::KeyboardAction)

static const char *const bullet_xpm[] =
    {
        "11 11 5 1",
        ". c None",
        "b c #3100c5",
        "# c #313062",
        "c c #3189ff",
        "a c #6265cd",
        "...........",
        "...####....",
        ".a#bbbb#a..",
        ".#ccbbbb#..",
        "#bccbbbbb#.",
        "#bbbbbbbb#.",
        "#bbbbbbcb#.",
        "#bbbbbccb#.",
        ".#bbbccb#..",
        ".a#bbbb#a..",
        "...####...."
    };

tst_Q3TextEdit::tst_Q3TextEdit()
{
}

tst_Q3TextEdit::~tst_Q3TextEdit()
{
}

void tst_Q3TextEdit::initTestCase()
{
    textEdit = new Q3TextEdit( 0, "textEdit" );
    textEdit->setFixedSize( 200, 200 );
    textEdit->mimeSourceFactory()->setImage( "bullet", QImage( bullet_xpm ) );
    qApp->setMainWidget( textEdit );
    textEdit->show();
}

void tst_Q3TextEdit::cleanupTestCase()
{
    delete textEdit;
}

void tst_Q3TextEdit::doKeyboardAction()
{
    QFETCH( QString, text );
    QFETCH( int, paragraph );
    QFETCH( int, index );
    QFETCH( Q3TextEdit::KeyboardAction, action );

    textEdit->setText( text );
    textEdit->setCursorPosition( paragraph, index );
    textEdit->doKeyboardAction( action );
    QTEST( textEdit->text(), "final" );
}

void tst_Q3TextEdit::doKeyboardAction_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("paragraph");
    QTest::addColumn<int>("index");
    QTest::addColumn<Q3TextEdit::KeyboardAction>("action");
    QTest::addColumn<QString>("final");

    QTest::newRow( "Bush1" ) <<
	QString("Republicans understand the importance of bondage between mother and child. - Gov GWB")
			    << 0
			    << 49
			    << Q3TextEdit::ActionWordDelete
			    <<
	QString("Republicans understand the importance of bondage mother and child. - Gov GWB");

    QTest::newRow( "Bush2" ) <<
	QString("If we don't succeed, we run the risk of failure. ...George W. Bush, Jr.")
			    << 0
			    << 2
			    << Q3TextEdit::ActionWordBackspace
			    <<
	QString(" we don't succeed, we run the risk of failure. ...George W. Bush, Jr.");

    QTest::newRow( "Bush3" ) <<
	QString("Welcome to Mrs. Bush, and my fellow astronauts. ...Governor George W. Bush, Jr.")
			    << 0
			    << 35
			    << Q3TextEdit::ActionWordDelete
			    <<
	QString("Welcome to Mrs. Bush, and my fellowastronauts. ...Governor George W. Bush, Jr.");

    QTest::newRow( "Bush4" ) <<
	QString("Mars is essentially in the same orbit... Mars is somewhat the same distance from "
	"the Sun, which is very important. We have seen pictures where there are canals, we "
	"believe, and water. If there is water, that means there is oxygen. If oxygen, "
	"that means we can breathe. ...Gov GW Bush, Jr., 8/11/94")
			    << 0
			    << 57
			    << Q3TextEdit::ActionWordBackspace
			    <<
	QString("Mars is essentially in the same orbit... Mars is  the same distance from "
	"the Sun, which is very important. We have seen pictures where there are canals, we "
	"believe, and water. If there is water, that means there is oxygen. If oxygen, "
	"that means we can breathe. ...Gov GW Bush, Jr., 8/11/94");


    QTest::newRow( "Bush5" ) <<
	QString("The Holocaust was an obscene period in our nation's history. I mean in this century's history. But we all lived in this century. I didn't live in this century.   GW Bus, 9/15/95")
			    << 0
			    << 139
			    << Q3TextEdit::ActionWordDelete
			    <<
	QString("The Holocaust was an obscene period in our nation's history. I mean in this century's history. But we all lived in this century. I didn't lin this century.   GW Bus, 9/15/95");

    QTest::newRow( "Bush6" ) <<
	QString("I believe we are on an irreversible trend toward more freedom and democracy, but that could change. GWB 5/22/98")
			    << 0
			    << 51
			    << Q3TextEdit::ActionWordBackspace
			    <<
	QString("I believe we are on an irreversible trend toward re freedom and democracy, but that could change. GWB 5/22/98");

    QTest::newRow( "Bush7" ) <<
	QString("One word sums up probably the responsibility of any Governor, and that one\n\n"
	"word is 'to be prepared'. ...Governor George W. Bush, Jr., 12/6/93")
			    << 1
			    << 0
			    << Q3TextEdit::ActionWordDelete
			    <<
	QString("One word sums up probably the responsibility of any Governor, and that one\n"
	"word is 'to be prepared'. ...Governor George W. Bush, Jr., 12/6/93");

    QTest::newRow( "Bush8" ) <<
	QString("Verbosity leads to unclear, inarticulate things.\n\n"
	"...Governor George W. Bush, Jr., 11/30/96")
			    << 1
			    << 0
			    << Q3TextEdit::ActionWordBackspace
			    <<
	QString("Verbosity leads to unclear, inarticulate things.\n"
	"...Governor George W. Bush, Jr., 11/30/96");

    QTest::newRow( "Bush9" ) <<
	QString("The future will be better tomorrow. ...Governor George W. Bush, Jr.")
			    << 0
			    << 29
			    << Q3TextEdit::ActionWordDelete
			    <<
	QString("The future will be better tomGovernor George W. Bush, Jr.");

    QTest::newRow( "Bush10" ) <<
	QString("The future will be better tomorrow. ...Governor George W. Bush, Jr.")
			     << 0
			     << 36
			     << Q3TextEdit::ActionWordBackspace
			     <<
	QString("The future will be better ...Governor George W. Bush, Jr.");

    QTest::newRow( "Bush11" ) <<
	QString("The future will be better tomorrow. ...Governor George W. Bush, Jr.")
			     << 0
			     << 37
			     << Q3TextEdit::ActionWordDelete
			     <<
	QString("The future will be better tomorrow. .Governor George W. Bush, Jr.");

    QTest::newRow( "Bush12" ) <<
	QString("The future will be better tomorrow. ...Governor George W. Bush, Jr.")
			     << 0
			     << 37
			     << Q3TextEdit::ActionWordBackspace
			     <<
	QString("The future will be better ..Governor George W. Bush, Jr.");
}

void tst_Q3TextEdit::cursorPosition_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("paragraph");
    QTest::addColumn<int>("index");
    QTest::newRow( "curpos1" )
	<< QString("where to find the icon in the application and the\n"
	   "application name. Also describe to him what the new icon should\n"
	   "convey. John will create the icon and send you an email asking if it\n"
	   "fits your wildest dreams. Iterations are expected to occur.") << 2 << 8;
}

void tst_Q3TextEdit::cursorPosition()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard and cron-started unit tests do not work", SkipAll);

    QFETCH( QString, text );
    QFETCH( int, paragraph );
    QFETCH( int, index );

    int para, in;
    QApplication::clipboard()->setText( text, QClipboard::Clipboard );

    textEdit->clear();
    textEdit->paste();
    textEdit->undo();
    textEdit->paste();
    textEdit->setCursorPosition( paragraph, index );
    textEdit->moveCursor( Q3TextEdit::MoveWordForward, TRUE );
    textEdit->removeSelectedText();
    textEdit->getCursorPosition( &para, &in );
    QCOMPARE( para, paragraph );
    QCOMPARE( in, index );
}


void tst_Q3TextEdit::text()
{
    QFETCH( int, format );
    QFETCH( QString, text );
    QFETCH( QString, expectedText );

    textEdit->clear();
    textEdit->setTextFormat( (Qt::TextFormat) format );
    QCOMPARE( textEdit->text(), QString("") );
    textEdit->setText( text );

    QCOMPARE( textEdit->text(), expectedText );
}

void tst_Q3TextEdit::text_data()
{
    QTest::addColumn<int>("format");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("expectedText");

    QTest::newRow( "PlainText and nbsp" ) << int(Qt::PlainText)
				       << QString( "Hello" ) + QChar(0xa0) + "World"
				       << QString( "Hello" ) + QChar(0xa0) + "World";
    QTest::newRow( "Empty PlainText" ) << int(Qt::PlainText)
				   << QString( "" )
				   << QString( "" );

    QTest::newRow( "Empty RichText" ) << int(Qt::RichText)
				   << QString( "" )
				   << QString( "" );
}

void tst_Q3TextEdit::copyPaste_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("expectedText");

    QTest::newRow( "0" ) << QString( "Hello" ) << QString( "Hello" );
    QTest::newRow( "1" ) << QString( "Hello<br>Hello" ) << QString( "Hello\nHello" );
    QTest::newRow( "2" ) << QString( "<p>Hello</p><p>Hello</p>" ) << QString( "Hello\nHello" );
    QTest::newRow( "3" ) << QString( "<img src=\"bullet\">Hello" ) << QString( "Hello" );
    QTest::newRow( "4" ) << QString( "<code>Hello</code>" ) << QString( "Hello" );
    QTest::newRow( "5" ) << QString( "<p><img src=\"bullet\"></img>Hello</p><p>Hello</p>" ) << QString( "Hello\nHello" );
    QTest::newRow( "6" ) << QString( "<p><img src=\"bullet\">Hello</p><p>Hello</p>" ) << QString( "Hello\nHello" );
}

// tests copying richtext and pasting as plain text
void tst_Q3TextEdit::copyPaste()
{
    if (!nativeClipboardWorking())
        QSKIP("Native clipboard and cron-started unit tests do not work", SkipAll);

    QFETCH( QString, text );
    QFETCH( QString, expectedText );

    textEdit->clear();
    textEdit->setTextFormat( Qt::RichText );

    textEdit->setText( text );
    textEdit->selectAll();
    textEdit->copy();

    QString pastedText = QApplication::clipboard()->text();
    QEXPECT_FAIL( "6", "funny behaviour if img tag is not closed", Abort );
    QCOMPARE( pastedText, expectedText );
}

void tst_Q3TextEdit::setReadOnly()
{
    // Check that text() returns the right thing when
    // in read-only mode

    textEdit->clear();
    textEdit->setTextFormat( Qt::PlainText );
    textEdit->setReadOnly( TRUE );
    QCOMPARE( textEdit->text(), QString("") );
    textEdit->setText("This is a test");
    QCOMPARE( textEdit->text(), QString("This is a test") );
    textEdit->clear();
    QCOMPARE( textEdit->text(), QString("")  );
    textEdit->append( "Foobar test" );
    QCOMPARE( textEdit->text(), QString("Foobar test") );
    textEdit->clear();
    QCOMPARE( textEdit->text(), QString("")  );
    textEdit->setText( "This is a test" );
    QCOMPARE( textEdit->text(), QString("This is a test") );
    textEdit->append( " foobar" );
    QCOMPARE( textEdit->text(), QString("This is a test\n foobar") );

    // Reset read only state
    textEdit->setReadOnly( FALSE );
}
/*
typedef QValueList<int> IntList;
Q_DECLARE_METATYPE(IntList)
*/
void tst_Q3TextEdit::find_data()
{
    // For the moment, this searches plain text, it should be extended to search
    // rich text and log text

    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("searchString");
    QTest::addColumn<bool>("caseSensitive");
    QTest::addColumn<bool>("wholeWord");
    QTest::addColumn<bool>("forward");
    QTest::addColumn<IntList>("paragraph");
    QTest::addColumn<IntList>("index");

    {
	// Check for t's non case-sensitive not whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 0 << 0 << 1 << 1 << 1 << 1;
	index << 0 << 10 << 13 << 4 << 15 << 20 << 23;
	QTest::newRow( "t-noncs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for t's case-sensitive not whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 0 << 1 << 1 << 1 << 1;
	index << 10 << 13 << 4 << 15 << 20 << 23;
	QTest::newRow( "t-cs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for t's non case-sensitive whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	QTest::newRow( "t-noncs-ww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for t's non case-sensitive not whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 1 << 1 << 1 << 0 << 0 << 0;
	index << 23 << 20 << 15 << 4 << 13 << 10 << 0;
	QTest::newRow( "t-noncs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for t's case-sensitive not whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 1 << 1 << 1 << 0 << 0;
	index << 23 << 20 << 15 << 4 << 13 << 10;
	QTest::newRow( "t-cs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for t's non case-sensitive whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "t";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	QTest::newRow( "t-noncs-ww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' non case-sensitive not whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 1;
	index << 0 << 4;
	QTest::newRow( "this-noncs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' case-sensitive not whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 4;
	QTest::newRow( "this-cs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' non case-sensitive whole word forward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 1;
	index << 0 << 4;
	QTest::newRow( "this-noncs-ww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' non case-sensitive not whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 0;
	index << 4 << 0;
	QTest::newRow( "this-noncs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' case-sensitive not whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 4;
	QTest::newRow( "this-cs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for 'this' non case-sensitive whole word backward
	QString text = "This is a test\nand this is another test";
	QString searchString = "this";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 0;
	index << 4 << 0;
	QTest::newRow( "this-noncs-ww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

        {
	// Check for ('s non case-sensitive not whole word forward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 1 << 1;
	index << 8 << 4 << 32;
	QTest::newRow( "(-noncs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for ('s case-sensitive not whole word forward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 0 << 1 << 1;
	index << 8 << 4 << 32;
	QTest::newRow( "(-cs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for ('s non case-sensitive whole word forward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 32;
	QTest::newRow( "(-noncs-ww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for ('s non case-sensitive not whole word backward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = FALSE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 1 << 0;
	index << 32 << 4 << 8;
	QTest::newRow( "(-noncs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for ('s case-sensitive not whole word backward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = TRUE;
	bool wholeWord = FALSE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1 << 1 << 0;
	index << 32 << 4 << 8;
	QTest::newRow( "(-cs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for ('s non case-sensitive whole word backward
	QString text = "This is (a te)st\nand (this is another test) with ( brackets)";
	QString searchString = "(";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 32;
	QTest::newRow( "(-noncs-ww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for (this's non case-sensitive whole word backward
	QString text = "This is (a te)st\nand (this is another test) with (brackets)";
	QString searchString = "(this";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = FALSE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 4;
	QTest::newRow( "(this-noncs-ww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for (this's non case-sensitive whole word forward
	QString text = "This is (a te)st\nand (this is another test) with (brackets)";
	QString searchString = "(this";
	bool caseSensitive = FALSE;
	bool wholeWord = TRUE;
	bool forward = TRUE;
	IntList paragraph;
	IntList index;
	paragraph << 1;
	index << 4;
	QTest::newRow( "(this-noncs-ww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for " "'s non case-sensitive non whole word forward
	QString text = "foo";
	QString searchString = " ";
	bool caseSensitive = false;
	bool wholeWord = false;
	bool forward = true;
	IntList paragraph;
	IntList index;
// 	paragraph << 0 << 1 << 1 << 1;
//         index << 3 << 0 << 6 << 12;
	QTest::newRow( " -noncs-nonww-forward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }

    {
	// Check for " "'s non case-sensitive non whole word backward
	QString text = "foo ";
	QString searchString = " ";
	bool caseSensitive = false;
	bool wholeWord = false;
	bool forward = false;
	IntList paragraph;
	IntList index;
	paragraph << 0;
        index << 3;
	QTest::newRow( " -noncs-nonww-backward" ) << text << searchString << caseSensitive << wholeWord << forward << paragraph << index;
    }
}


void tst_Q3TextEdit::find()
{
    QFETCH( QString, text );
    QFETCH( QString, searchString );
    QFETCH( bool, caseSensitive );
    QFETCH( bool, wholeWord );
    QFETCH( bool, forward );
    QFETCH( IntList, paragraph );
    QFETCH( IntList, index );

    textEdit->clear();
    textEdit->setTextFormat( Qt::PlainText );
    textEdit->setText( text );

    IntList::Iterator pIt;
    IntList::Iterator iIt = index.begin();
    int paraFound = forward ? 0 : INT_MAX;
    int indexFound = forward ? 0 : INT_MAX;

    QEXPECT_FAIL(" -noncs-nonww-forward", "Searching for space is broken in Q3TextEdit because of the extra space added at the end of all paragraphs", Abort );
    QEXPECT_FAIL(" -noncs-nonww-backward", "Searching for space is broken in Q3TextEdit because of the extra space added at the end of all paragraphs", Abort );

    // It should not find anything
    if ( !paragraph.count() ) {
	QVERIFY( !textEdit->find( searchString, caseSensitive, wholeWord, forward, &paraFound, &indexFound ) );
    } else {
	for ( pIt = paragraph.begin(); pIt != paragraph.end(); ++pIt ) {
	    QVERIFY( textEdit->find( searchString, caseSensitive, wholeWord, forward, &paraFound, &indexFound ) );
	    QCOMPARE( paraFound, (*pIt) );
	    QCOMPARE( indexFound, (*iIt) );

	    // This sounds dubious to me, it should search from the next character specified by indexFound
	    if ( forward )
		indexFound++;
	    else
		indexFound--;

	    ++iIt;
	}
    }
}

void tst_Q3TextEdit::findSC()
{
    // Special bug case #29649 test
    textEdit->clear();
    textEdit->setTextFormat(Qt::PlainText);
    textEdit->setText("foo do soo boo arg\ndf df foo a;ls lkdf lsdkjf");
    textEdit->setCursorPosition(1, 16);
    int paraFound, indexFound;
    QVERIFY(textEdit->find("foo", FALSE, FALSE, FALSE));
    textEdit->getCursorPosition(&paraFound, &indexFound);
    QCOMPARE(paraFound, 1);
    QCOMPARE(indexFound, 6);
    QVERIFY(textEdit->find("foo", FALSE, FALSE, FALSE));
    textEdit->getCursorPosition(&paraFound, &indexFound);
    QCOMPARE(paraFound, 0);
    QCOMPARE(indexFound, 0);
    QVERIFY(!textEdit->find("foo", FALSE, FALSE, FALSE));
    QVERIFY(textEdit->find("foo", FALSE, FALSE, FALSE));
    textEdit->getCursorPosition(&paraFound, &indexFound);
    QCOMPARE(paraFound, 1);
    QCOMPARE(indexFound, 6);
}



void tst_Q3TextEdit::findBackwards()
{
    textEdit->clear();
    textEdit->setTextFormat(Qt::PlainText);
    textEdit->setText("ABCABCABCABC");
    textEdit->setCursorPosition(0, 3);
    QVERIFY(textEdit->find("ABC", FALSE, FALSE, FALSE));
    QCOMPARE(textEdit->selectedText(), QString("ABC"));
    int pFrom, pTo, iFrom, iTo;
    QString qw;

    textEdit->getSelection(&pFrom, &iFrom, &pTo, &iTo);
    QCOMPARE(pFrom, 0);
    QCOMPARE(iFrom, 0);
    QCOMPARE(pTo, 0);
    QCOMPARE(iTo, 3);
}
void tst_Q3TextEdit::redoAvailableEmitted( bool t )
{
    redoA = t;
}

void tst_Q3TextEdit::undoAvailableEmitted( bool t )
{
    undoA = t;
}

void tst_Q3TextEdit::copyAvailableEmitted( bool t )
{
    copyA = t;
}

void tst_Q3TextEdit::cut()
{
    connect( textEdit, SIGNAL( copyAvailable(bool) ), this, SLOT( copyAvailableEmitted(bool) ) );
    copyA = FALSE;
    textEdit->setText( "I believe that people who are going to commit crimes shouldn't have guns" );
    textEdit->selectAll();
    QTest::qWait( 2000 );
    QVERIFY( copyA );
    textEdit->cut();
    QTest::qWait( 2000 );
    QVERIFY( !copyA );
}

void tst_Q3TextEdit::clear()
{
    // Clear is actually tested all over the place, this is
    // really just to test for task 21355
    textEdit->clear();
    textEdit->setTextFormat( Qt::RichText );
    textEdit->setText( "<table align=right></table>" );
    textEdit->clear();
    // If it got here, then it didn't crash
    QVERIFY( TRUE );
}

void tst_Q3TextEdit::insert()
{
    Qt::TextFormat fmt = textEdit->textFormat();
    textEdit->setTextFormat(Qt::LogText);
    textEdit->setText("<blue>The<red> following: </red><b>fooooooooooooooooooff</b>\nLine <green>Number 2\nLine</green> Number 3");
    textEdit->insertParagraph("<orange>#orange#</orange>", 1);
    textEdit->insertAt(" #\nper\nka<blue>ar<yellow>e\nreo</yellow>le</blue># \n", 2, 3 );
    textEdit->insertAt("\n#<b>o</b><i>l</i><u>e</u>#\n\n", 100, 100);
    textEdit->insertAt("<u>#o\nle#</u>", 0, 10);
    QString result = "<blue>The<red> follow<u>#o\nle#</u>ing: </red><b>fooooooooooooooooooff</b>\n<orange>#orange#</orange>\n"
		     "Lin #\nper\nka<blue>ar<yellow>e\nreo</yellow>le</blue># \ne <green>Number 2\nLine</green> Number 3\n"
		     "#<b>o</b><i>l</i><u>e</u>#\n\n\n";
    QCOMPARE(textEdit->text(), result);
    textEdit->setTextFormat(fmt);
}

void tst_Q3TextEdit::keyClicks_data()
{
    QTest::addColumn<QString>("inputString");

    QTest::newRow("English") << "This is not a sentence.";
    QTest::newRow("Norwegian") << "\x0e6\x0f8\x0e5\x0c6\x0d8\x0c5 er norske bokstaver.";
}

void tst_Q3TextEdit::keyClicks()
{
    QFETCH(QString, inputString);

    textEdit->clear();
    textEdit->setTextFormat(Qt::PlainText);
    textEdit->setReadOnly(false);

    QVERIFY(textEdit->text().isEmpty());
    QTest::keyClicks(textEdit, inputString);
    QCOMPARE(textEdit->text(), inputString);
}



void tst_Q3TextEdit::selectAll()
{
    QString testString = "This is test for task #24092";
    textEdit->setText( testString );

    textEdit->setTextFormat(Qt::PlainText);
    textEdit->selectAll(TRUE);
    QVERIFY( textEdit->hasSelectedText() );
    textEdit->selectAll(FALSE);
    QVERIFY( !textEdit->hasSelectedText() );

    textEdit->setTextFormat(Qt::RichText);
    textEdit->selectAll(TRUE);
    QVERIFY( textEdit->hasSelectedText() );
    textEdit->selectAll(FALSE);
    QVERIFY( !textEdit->hasSelectedText() );
    textEdit->setTextFormat(Qt::LogText);
    textEdit->selectAll(TRUE);
    QVERIFY( textEdit->hasSelectedText() );
    textEdit->selectAll(FALSE);
    QVERIFY( !textEdit->hasSelectedText() );
}

void tst_Q3TextEdit::setCurrentFont()
{
    textEdit->clear();
    textEdit->setTextFormat(Qt::LogText);
    textEdit->setCurrentFont(QFont("Courier New", 9));
    textEdit->setText("Just some dummy text");
    QCOMPARE(textEdit->text(), QString("Just some dummy text\n"));
}

void tst_Q3TextEdit::undoRedo()
{
    Q3TextEdit edit;
    edit.setTextFormat(Qt::PlainText);

    QString deftext("Just some text");
    edit.setText(deftext);
    edit.insertAt("<tag>",0, 5);
    QCOMPARE(edit.text(), QString("Just <tag>some text"));
    edit.undo();
    QCOMPARE(edit.text(), deftext);
    edit.insertAt("<tag>",0, 5);
    edit.insertAt("</tag>",0, 14);
    QCOMPARE(edit.text(), QString("Just <tag>some</tag> text"));
    edit.undo();
    QCOMPARE(edit.text(), QString("Just <tag>some text"));
    edit.undo();
    QCOMPARE(edit.text(), QString("Just some text"));
    edit.insertAt("<tag>",0, 5);
    edit.insertAt("</tag>",0, 10);
    QCOMPARE(edit.text(), QString("Just <tag></tag>some text"));
    edit.undo();
    QCOMPARE(edit.text(), QString("Just some text"));
}

void tst_Q3TextEdit::length_data()
{
    // The expected length values are based on what is returned in Qt 3.2.1.
    // We don't want the semantics of this function to change again, no
    // matter how broken it is, so if this function fails, the test data has to be seen as
    // correct.  In Qt 4.0, this function will not exist in this form.

    QTest::addColumn<int>("textFormat");
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("expectedLength");

    QTest::newRow("plainText") << int(Qt::PlainText) << "This is a test" << 14;
    QTest::newRow("plainTextNewLines") << int(Qt::PlainText) << "This is a test\nThis is a test\nThis is a test" << 44;
    QTest::newRow("logText") << int(Qt::LogText) << "This is a test" << 14;
    QTest::newRow("logTextNewLines") << int(Qt::LogText) << "This is a test\nThis is a test\nThis is a test" << 42;
    QTest::newRow("logTextTags") << int(Qt::LogText) << "<b>This is a test</b>" << 14;
    QTest::newRow("logTextTagsNewLines") << int(Qt::LogText) << "<b>This is a test\nThis is a test\nThis is a test</b>" << 42;
    QTest::newRow("richText") << int(Qt::RichText) << "This is a test" << 14;
    QTest::newRow("richTextNewLines") << int(Qt::RichText) << "This is a test\nThis is a test\nThis is a test" << 44;
    QTest::newRow("richTextTags") << int(Qt::RichText) << "<b>This is a test</b>" << 14;
}

void tst_Q3TextEdit::length()
{
    QFETCH(int, textFormat);
    QFETCH(QString, text);
    QFETCH(int, expectedLength);

    textEdit->setTextFormat((Qt::TextFormat)textFormat);
    textEdit->setText(text);
    QCOMPARE(textEdit->length(), expectedLength);
}

void tst_Q3TextEdit::getSelection_data()
{
    QTest::addColumn<int>("textFormat");
    QTest::addColumn<QString>("text");

    QTest::addColumn<int>("paragFrom");
    QTest::addColumn<int>("indexFrom");
    QTest::addColumn<int>("paragTo");
    QTest::addColumn<int>("indexTo");

    QTest::addColumn<int>("selParagFrom");
    QTest::addColumn<int>("selIndexFrom");
    QTest::addColumn<int>("selParagTo");
    QTest::addColumn<int>("selIndexTo");

    QTest::newRow("plainText") << int(Qt::PlainText) << "This is a test" << 0 << 0 << 0 << 2 << 0 << 0 << 0 << 2;
    QTest::newRow("richText") << int(Qt::RichText) << "This is a test" << 0 << 0 << 0 << 2 << 0 << 0 << 0 << 2;
    QTest::newRow("logText") << int(Qt::LogText) << "This is a test" << 0 << 0 << 0 << 2 << 0 << 0 << 0 << 2;
}

void tst_Q3TextEdit::getSelection()
{
    QFETCH(int, textFormat);
    QFETCH(QString, text);

    QFETCH(int, paragFrom);
    QFETCH(int, indexFrom);
    QFETCH(int, paragTo);
    QFETCH(int, indexTo);

    QFETCH(int, selParagFrom);
    QFETCH(int, selIndexFrom);
    QFETCH(int, selParagTo);
    QFETCH(int, selIndexTo);

    int pFr, iFr, pTo, iTo;
    textEdit->setText(text);
    textEdit->setTextFormat((Qt::TextFormat)textFormat);
    textEdit->setSelection(paragFrom, indexFrom, paragTo, indexTo);
    textEdit->getSelection(&pFr, &iFr, &pTo, &iTo);

    QCOMPARE(pFr, selParagFrom);
    QCOMPARE(iFr, selIndexFrom);
    QCOMPARE(pTo, selParagTo);
    QCOMPARE(iTo, selIndexTo);
}

void tst_Q3TextEdit::anchorTest()
{
    // This is from task 57709
    Q3TextEdit edit;
    QString richText = "<p dir=\"ltr\"><a name=\"Gold\"></a>something</p>\n"
                       "<p dir=\"ltr\"><a name=\"Silver\"></a>more"
                       "<a name=\"Bronze\"></a>stuff</p>\n";
    edit.setText(richText);

    QString expected = "<html><head><meta name=\"qrichtext\" content=\"1\" /></head>"
                    "<body style=\"font-size:%1pt;font-family:%2\">\n"
                       + richText + "</body></html>\n";
    QFont f = edit.font();
    expected = expected.arg(f.pointSize()).arg(f.family());
    QCOMPARE(edit.text(),expected);
}

QTEST_MAIN(tst_Q3TextEdit)
#include "tst_q3textedit.moc"

