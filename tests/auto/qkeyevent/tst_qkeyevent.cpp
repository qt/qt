/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <qevent.h>
#include <qapplication.h>


#include <qevent.h>

//TESTED_CLASS=
//TESTED_FILES=gui/itemviews/qitemselectionmodel.h gui/itemviews/qitemselectionmodel.cpp

class KeyEventWidget : public QWidget
{
public:
    KeyEventWidget( QWidget* parent = 0, const char* name = 0 );
    ~KeyEventWidget();
    QKeyEvent* getLastKeyPress();
    QKeyEvent* getLastKeyRelease();
    bool recievedKeyPress;
    bool recievedKeyRelease;
protected:
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );
private:
    QKeyEvent* lastKeyPress;
    QKeyEvent* lastKeyRelease;
};

class tst_QKeyEvent : public QObject
{
    Q_OBJECT
public:
    tst_QKeyEvent();
    ~tst_QKeyEvent();
public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void sendRecieveKeyEvents_data();
    void sendRecieveKeyEvents();
    void standardKey();
private:
    KeyEventWidget* testWidget;
};


KeyEventWidget::KeyEventWidget( QWidget* parent, const char* name )
    : QWidget( parent ), recievedKeyPress( FALSE ), recievedKeyRelease( FALSE ),
    lastKeyPress( 0 ), lastKeyRelease( 0 )
{
    setObjectName(name);
}

KeyEventWidget::~KeyEventWidget()
{
}

QKeyEvent* KeyEventWidget::getLastKeyPress()
{
    return lastKeyPress;
}

QKeyEvent* KeyEventWidget::getLastKeyRelease()
{
    return lastKeyRelease;
}

void KeyEventWidget::keyPressEvent( QKeyEvent* e )
{
    lastKeyPress = e;
    recievedKeyPress = TRUE;
}

void KeyEventWidget::keyReleaseEvent( QKeyEvent* e )
{
    lastKeyRelease = e;
    recievedKeyRelease = TRUE;
}

tst_QKeyEvent::tst_QKeyEvent()
{
}

tst_QKeyEvent::~tst_QKeyEvent()
{
}

void tst_QKeyEvent::initTestCase()
{
    testWidget = new KeyEventWidget( 0 );
    testWidget->show();
}

void tst_QKeyEvent::cleanupTestCase()
{
    delete testWidget;
}

void tst_QKeyEvent::sendRecieveKeyEvents_data()
{
    QTest::addColumn<int>("key");
    QTest::addColumn<bool>("textExpected");
    QTest::addColumn<QString>("text");
    int a;
    for ( a = Qt::Key_Escape; a < Qt::Key_Direction_R; a++ ) {
	if ( ( a > Qt::Key_Clear && a < Qt::Key_Home )
	    || ( a > Qt::Key_PageDown && a < Qt::Key_Shift )
	    || ( a > Qt::Key_ScrollLock && a < Qt::Key_F1 ) ) {
	    // There is no representation for these values
	    continue;
	}
	if ( a == Qt::Key_Backtab ) // Actually SHIFT+Tab
	    QTest::newRow( QString("key - %1").arg( a ).toLatin1() ) << int(Qt::Key_Tab) << FALSE << "";
	else
	    QTest::newRow( QString("key - %1").arg( a ).toLatin1() ) << a << FALSE << "";
    }

    for ( a = Qt::Key_Space; a < Qt::Key_ydiaeresis; a++ ) {
	QTest::newRow( QString("key - %1").arg( a ).toLatin1() ) << a << TRUE << QString( QChar(a) );
    }
}

void tst_QKeyEvent::standardKey()
{

}

void tst_QKeyEvent::sendRecieveKeyEvents()
{
    QSKIP( "Skipped while it is being worked on", SkipAll);
    QFETCH( int, key );
    QFETCH( bool, textExpected );
    QFETCH( QString, text );
    testWidget->recievedKeyPress = FALSE;

#ifdef Q_WS_WIN
    // Will be eaten by Windows system
    if ( key == Qt::Key_Print )
	return;

    // This is mapped to nothing on Windows
    if ( key == Qt::Key_SysReq )
	return;

    // Not supported on Windows
    if ( key >= Qt::Key_F25 && key <= Qt::Key_Super_R )
	return;
    if ( key >= Qt::Key_Hyper_L && key <= Qt::Key_Hyper_R )
	return;
    if ( key == Qt::Key_Help )
	return;

    // Not sure on how to add support for these yet
    if ( key >= Qt::Key_Direction_L && key <= Qt::Key_Direction_R )
	return;

    // Not sure on how to test these yet, since they use SHIFT etc
    if ( key >= Qt::Key_Exclam && key <= Qt::Key_Slash )
	return;
    if ( key >= Qt::Key_Colon && key <= Qt::Key_At )
	return;
    if ( key >= Qt::Key_BracketRight && key <= Qt::Key_ydiaeresis )
	return;
#endif // Q_WS_WIN

    if ( key == Qt::Key_F1 )
	return; // Ignore for the moment

    QTest::keyPress( testWidget, (Qt::Key)key );
    while ( !testWidget->recievedKeyPress )
        qApp->processEvents();
    QKeyEvent* ke = testWidget->getLastKeyPress();
    QCOMPARE( ke->key(), key );
    if ( textExpected )
        QCOMPARE( ke->text(), text );
    testWidget->recievedKeyRelease = FALSE;
    QTest::keyRelease( testWidget, (Qt::Key)key );
    while ( !testWidget->recievedKeyRelease )
        qApp->processEvents();
    ke = testWidget->getLastKeyRelease();
    QCOMPARE( ke->key(), key );
    if ( textExpected )
        QCOMPARE( ke->text(), text );
}

QTEST_MAIN(tst_QKeyEvent)
#include "tst_qkeyevent.moc"
