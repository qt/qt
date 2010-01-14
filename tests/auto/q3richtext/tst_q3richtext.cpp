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

#include <q3textedit.h>
#include <qapplication.h>
#include <qclipboard.h>

#ifdef Q_WS_MAC
#include <Carbon/Carbon.h>
#endif

//TESTED_FILES=

class tst_Q3RichText : public QObject
{
    Q_OBJECT
public:
    tst_Q3RichText();
    virtual ~tst_Q3RichText();


public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void richtext();
    void append_data();
    void append();
    void cursorPosition();
    void moveCursor();
    void keyPressEvent();
};

tst_Q3RichText::tst_Q3RichText()
{
}

tst_Q3RichText::~tst_Q3RichText()
{
}

void tst_Q3RichText::initTestCase()
{
  // create a default mainwindow
  // If you run a widget test, this will be replaced in the testcase by the
  // widget under test
    QWidget *w = new QWidget(0,"mainWidget");
    w->setFixedSize( 200, 200 );
    qApp->setMainWidget( w );
    w->show();
}

void tst_Q3RichText::cleanupTestCase()
{
    delete qApp->mainWidget();
}

void tst_Q3RichText::richtext()
{
    Q3TextEdit* textedit = new Q3TextEdit();
    QString result( "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\"font-size:%1;font-family:%2\">\n"
	    "<p dir=\"ltr\">k <span style=\"font-family:Arial;font-size:20pt%3\">   </span><span style=\"font-family:Arial;font-size:15pt%4\">  </span><span style=\"font-family:Arial;font-size:20pt%5\">k k  </span></p>\n"
	    "</body></html>\n" );
    QFont font = textedit->font();
    result = result.arg( QString::number(font.pointSize()) + "pt" ).arg( font.family() ).arg( "" ).arg( "" ).arg( "" );

    textedit->setTextFormat( Qt::RichText );

    QKeyEvent ke( QEvent::KeyPress, Qt::Key_K, 107, Qt::NoButton, "k" );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    textedit->setCurrentFont( QFont( "Arial", 20 ) );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    textedit->setCurrentFont( QFont( "Arial", 15 ) );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    textedit->setCurrentFont( QFont( "Arial", 20 ) );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_K, 107, Qt::NoButton, "k" );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_K, 107, Qt::NoButton, "k" );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Space, 32, Qt::NoButton, " " );
    QApplication::sendEvent( textedit->viewport(), &ke );

    QCOMPARE( textedit->text(), result );

    delete textedit;
}

void tst_Q3RichText::append_data()
{
    // should we catch bad alloc or something if it fails?
    // Being this is a test.
    QTest::addColumn<QString>("firstString");
    QTest::addColumn<QString>("secondString");
    QTest::addColumn<QString>("result");
    QTest::addColumn<int>("textFormat");

    QTest::newRow( "data0" ) << QString("Hello World...") << QString("First Time")
			   << QString("Hello World...\nFirst Time")
			   << int(Qt::PlainText);
    QTest::newRow( "data1" ) << QString("<b>Hello World...</b>") << QString("Second Time")
			    << QString("Hello World...</span></p>\n<p dir=\"ltr\">Second Time</p>\n</body></html>\n" ) << int(Qt::RichText);
}


void tst_Q3RichText::append()
{
    Q3TextEdit textedit;
    QFETCH( QString, firstString );
    QFETCH( QString, secondString );
    QFETCH( QString, result );
    QFETCH( int, textFormat );

    textedit.setTextFormat( Qt::TextFormat(textFormat) );
    textedit.append( firstString );
    textedit.append( secondString );
    QCOMPARE( textedit.text().mid(textedit.text().indexOf("Hello World")), result );
}

void tst_Q3RichText::cursorPosition()
{
    Q3TextEdit textedit;
    textedit.setText( "This is a test" );
    textedit.setCursorPosition( textedit.paragraphs() - 1, textedit.paragraphLength( textedit.paragraphs() - 1 ) );
    int para;
    int index;
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 14 );
    textedit.setText( "This is a test\nThis is a test\nThis is a test" );
    textedit.setCursorPosition( textedit.paragraphs() - 1, textedit.paragraphLength( textedit.paragraphs() - 1 ) );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 14 );
}

void tst_Q3RichText::moveCursor()
{
    // Still needs to test for MovePageUp and MovePageDown

    int para, index;

    Q3TextEdit textedit;
    textedit.show();
    textedit.setText( "This is a test" );

    textedit.moveCursor( Q3TextEdit::MoveEnd, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 14 );

    textedit.moveCursor( Q3TextEdit::MoveBackward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 13 );

    textedit.moveCursor( Q3TextEdit::MoveWordBackward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 10 );

    textedit.moveCursor( Q3TextEdit::MoveHome, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 0 );

    textedit.moveCursor( Q3TextEdit::MoveForward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 1 );

    textedit.moveCursor( Q3TextEdit::MoveWordForward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 5 );

    textedit.moveCursor( Q3TextEdit::MoveLineStart, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 0 );

    textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 14 );

    textedit.setText( "This is a test\nThis is a test\nThis is a test" );

    textedit.moveCursor( Q3TextEdit::MoveEnd, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 14 );

    textedit.moveCursor( Q3TextEdit::MoveBackward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 13 );

    textedit.moveCursor( Q3TextEdit::MoveWordBackward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 10 );

    textedit.moveCursor( Q3TextEdit::MoveHome, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 0 );

    textedit.moveCursor( Q3TextEdit::MoveForward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 1 );

    textedit.moveCursor( Q3TextEdit::MoveWordForward, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 5 );

    textedit.moveCursor( Q3TextEdit::MoveDown, FALSE );
    textedit.moveCursor( Q3TextEdit::MoveDown, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 5 );

    textedit.moveCursor( Q3TextEdit::MoveLineStart, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 0 );

    textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 2 );
    QCOMPARE( index, 14 );

    textedit.moveCursor( Q3TextEdit::MoveUp, FALSE );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 1 );
    QCOMPARE( index, 14 );
}

void tst_Q3RichText::keyPressEvent()
{
    // Still needs to test Key_Prior and Key_Next

    int para, index;

    Q3TextEdit textedit;
    textedit.show();
    textedit.setText( "This is a test" );

    qWarning( "Consider using QtTestCase::keyEvent() for sending key events" );
    QKeyEvent ke( QEvent::KeyPress, Qt::Key_Right, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 1 );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Right, 0, Qt::ControlModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 5 );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Left, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 4 );

    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Left, 0, Qt::ControlModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    textedit.getCursorPosition( &para, &index );
    QCOMPARE( para, 0 );
    QCOMPARE( index, 0 );

    // Test that the text is removed when Enter/Return is pressed first
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Enter, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("\n") );

    textedit.setText( "This is a test" );
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Return, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("\n") );

    // Now test if the line-break is added in rich text mode
    textedit.setTextFormat( Qt::RichText );

    textedit.setText( "This is a test" );
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Enter, 0, Qt::ControlModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QVERIFY( textedit.text().contains( "<br />" ) );

    textedit.setText( "This is a test" );
    textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Return, 0, Qt::ControlModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QVERIFY( textedit.text().contains( "<br />" ) );

    textedit.setText( "This is a test" );
    textedit.moveCursor( Q3TextEdit::MoveWordForward, FALSE );
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Return, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QString es = QString::fromLatin1("<p dir=\"ltr\">");
    QVERIFY( textedit.text().count( es ) == 2 );

    textedit.setTextFormat( Qt::AutoText );

    textedit.setText( "This is a test" );
    QApplication::clipboard()->setText("");
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Delete, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("") );

#if defined (Q_WS_WIN)
    textedit.setText( "This is a test" );
    QApplication::clipboard()->setText("");
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Delete, 0, Qt::ShiftModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("") );
    QCOMPARE( QApplication::clipboard()->text(), QString("This is a test") );
#endif

    textedit.setText( "This is a test" );
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Delete, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("his is a test") );

    bool nativeClipboardWorking = true;
#if defined (Q_WS_MAC)
    PasteboardRef pasteboard;
    OSStatus status = PasteboardCreate(0, &pasteboard);
    if (status == noErr)
        CFRelease(pasteboard);
    nativeClipboardWorking = status == noErr;
#endif

    if (nativeClipboardWorking) {
        textedit.setText( "This is a test" );
        QApplication::clipboard()->setText(" and this is another test");
        textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
        ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Insert, 0, Qt::ShiftModifier );
        QApplication::sendEvent( textedit.viewport(), &ke );
        QCOMPARE( textedit.text(), QString("This is a test and this is another test") );
    }

#if defined (Q_WS_WIN)
    textedit.setText( "This is a test" );
    QApplication::clipboard()->setText("");
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Insert, 0, Qt::ControlModifier );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( QApplication::clipboard()->text(), QString("This is a test") );
#endif

    textedit.setText( "This is a test" );
    textedit.selectAll();
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Backspace, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString("") );

    textedit.setText( "This is a test" );
    textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
    ke = QKeyEvent( QEvent::KeyPress, Qt::Key_Backspace, 0, Qt::NoButton );
    QApplication::sendEvent( textedit.viewport(), &ke );
    QCOMPARE( textedit.text(), QString( "This is a tes" ) );

    if (nativeClipboardWorking) {
        textedit.setText( "This is a test" );
        QApplication::clipboard()->setText("");
        textedit.selectAll();
        ke = QKeyEvent( QEvent::KeyPress, Qt::Key_F16, 0, Qt::NoButton );
        QApplication::sendEvent( textedit.viewport(), &ke );
        QCOMPARE( QApplication::clipboard()->text(), QString("This is a test") );

        textedit.setText( "This is a test" );
        textedit.moveCursor( Q3TextEdit::MoveLineEnd, FALSE );
        QApplication::clipboard()->setText(" and this is another test");
        ke = QKeyEvent( QEvent::KeyPress, Qt::Key_F18, 0, Qt::NoButton );
        QApplication::sendEvent( textedit.viewport(), &ke );
        QCOMPARE( textedit.text(), QString("This is a test and this is another test") );

        textedit.setText( "This is a test" );
        QApplication::clipboard()->setText("");
        textedit.selectAll();
        ke = QKeyEvent( QEvent::KeyPress, Qt::Key_F20, 0, Qt::NoButton );
        QApplication::sendEvent( textedit.viewport(), &ke );
        QCOMPARE( textedit.text(), QString("") );
        QCOMPARE( QApplication::clipboard()->text(), QString("This is a test") );
    }
}

QTEST_MAIN(tst_Q3RichText)
#include "tst_q3richtext.moc"

