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


#include <qglobal.h>
#ifdef Q_WS_MAC
#include <private/qt_mac_p.h>
#undef verify
#endif

#include <QtTest/QtTest>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qmenubar.h>

#include <qdebug.h>

#include <q3popupmenu.h>
#include <qmainwindow.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3PopupMenu : public QObject
{
Q_OBJECT

public:
    tst_Q3PopupMenu();
    virtual ~tst_Q3PopupMenu();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void task177490_highlighted();
    void task177490_activated();

    void testAccels();
    void fontPropagation();
    void accel_data();
    void accel();
    void testItemParameter();

protected slots:
    void onExclItem();
    void onShiftItem();
    void onSpicy();
    void onSubItem();
    // Needed to slience QObject about non existant slot
    void dummySlot() {}
    void itemParameterChanged(int p = 0){itemParameter = p; }

private:
    QMainWindow *testWidget;
    Q3PopupMenu *popup;
    Q3PopupMenu *subPopup;
    bool excl, shft, spicy, subItem;
    int itemParameter;
};

tst_Q3PopupMenu::tst_Q3PopupMenu()
{
}

tst_Q3PopupMenu::~tst_Q3PopupMenu()
{
}

void tst_Q3PopupMenu::task177490_highlighted()
{
#ifdef Q_WS_MAC
    QSKIP("On Mac, native mouse events are needed for this test", SkipAll);
#endif
#ifdef Q_WS_QWS
    QApplication::processEvents();
#endif
    Q3PopupMenu menu1;
    //don't let the window manager move the popup while we are testing
    menu1.setWindowFlags(Qt::X11BypassWindowManagerHint);
    menu1.insertItem("Item 1");

    Q3PopupMenu menu2;
    menu2.insertItem("Item 2");
    menu1.insertItem("Menu 2", &menu2);

    menu1.show();

    const int y1 = menu1.itemGeometry(0).center().y();
    const int y2 = menu1.itemGeometry(1).center().y();
    const int x = menu1.itemGeometry(0).center().x();

    QSignalSpy spy(&menu1, SIGNAL(highlighted(int)));

    QTest::mouseMove(&menu1, QPoint(x, y1));
    QTest::mouseMove(&menu1, QPoint(x, y1 + 1));
    QTest::qWait(1000);

    QTest::mouseMove(&menu1, QPoint(x, y2));
    QTest::mouseMove(&menu1, QPoint(x, y2 + 1));
    QTest::qWait(1000);

    QCOMPARE(spy.count(), 2); // one per menu item
}

void tst_Q3PopupMenu::task177490_activated()
{
#ifdef Q_WS_MAC
    QSKIP("On Mac, native mouse events are needed for this test", SkipAll);
#endif
    Q3PopupMenu menu1;
    menu1.insertItem("Item 1");

    Q3PopupMenu menu2;
    menu2.insertItem("Item 2");
    menu1.insertItem("Menu 2", &menu2);

    Q3PopupMenu menu3;
    menu3.insertItem("Item 3");
    menu2.insertItem("Menu 3", &menu3);

    Q3PopupMenu menu4;
    menu4.insertItem("Item 4");
    menu3.insertItem("Menu 4", &menu4);

    menu1.show();

    QTest::mouseMove(&menu1, QPoint());

    QPoint pos;

    pos = menu1.itemGeometry(1).center();
    QTest::mouseMove(&menu1, pos);
    QTest::mouseMove(&menu1, QPoint(pos.x() + 1, pos.y()));
    QTest::qWait(1000);

    pos = menu2.itemGeometry(1).center();
    QTest::mouseMove(&menu2, pos);
    QTest::mouseMove(&menu2, QPoint(pos.x() + 1, pos.y()));
    QTest::qWait(1000);

    pos = menu3.itemGeometry(1).center();
    QTest::mouseMove(&menu3, pos);
    QTest::mouseMove(&menu3, QPoint(pos.x() + 1, pos.y()));
    QTest::qWait(1000);

    pos = menu4.itemGeometry(0).center();
    QTest::mouseMove(&menu4, pos);
    QTest::mouseMove(&menu4, QPoint(pos.x() + 1, pos.y()));
    QTest::qWait(1000);
    const bool menu4_wasVisible = menu4.isVisible();

    QSignalSpy spy1(&menu1, SIGNAL(activated(int)));
    QSignalSpy spy2(&menu2, SIGNAL(activated(int)));
    QSignalSpy spy3(&menu3, SIGNAL(activated(int)));
    QSignalSpy spy4(&menu4, SIGNAL(activated(int)));

    QTest::mouseClick(&menu4, Qt::LeftButton, 0, menu4.itemGeometry(0).center());

    QCOMPARE(spy1.count(), 0);
    QCOMPARE(spy2.count(), 0);
    QCOMPARE(spy3.count(), 0);
    if (!menu4_wasVisible)
        QEXPECT_FAIL(
            "", "expected failure due to visibilty/focus problem; to be investigated later",
            Abort);
    QCOMPARE(spy4.count(), 1);
}

Q_DECLARE_METATYPE(QKeySequence)

void tst_Q3PopupMenu::initTestCase()
{
    // Create the test class

    testWidget = new QMainWindow(0);
    popup = new Q3PopupMenu( testWidget->menuBar(), "popup" );
    testWidget->menuBar()->insertItem( "menu&bar", popup );
    excl = FALSE;
    shft = FALSE;
    spicy = FALSE;
    subItem = FALSE;
    qApp->setMainWidget(testWidget);
    testWidget->resize( 200, 200 );
    testWidget->show();
    popup->insertItem( tr("Men&u"), this, SLOT(onShiftItem()) );
    popup->insertItem( tr("thing&!"), this, SLOT(onExclItem()) );
    popup->insertItem( tr("Hot && Spic&y" ), this, SLOT(onSpicy()) );

    subPopup = new Q3PopupMenu( popup, "subpopup" );
    subPopup->insertItem( "sub menu &item", this, SLOT(onSubItem()) );

    popup->insertItem( "&sub Popup", subPopup );
}

void tst_Q3PopupMenu::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3PopupMenu::init()
{
    QApplication::setActiveWindow(testWidget);
    QApplication::processEvents();
}

void tst_Q3PopupMenu::cleanup()
{
    QApplication::processEvents();
}
void tst_Q3PopupMenu::onExclItem()
{
    excl = TRUE;
}

void tst_Q3PopupMenu::onShiftItem()
{
    shft = TRUE;
}

void tst_Q3PopupMenu::onSpicy()
{
    spicy = TRUE;
}

void tst_Q3PopupMenu::onSubItem()
{
    subItem = TRUE;
}

void tst_Q3PopupMenu::testAccels()
{
#if !defined(Q_WS_MAC)
    QTest::keyClick( testWidget, Qt::Key_B, Qt::AltModifier );
    while (!popup->isVisible())
        QApplication::processEvents();
    QTest::keyClick( popup, Qt::Key_S );
    while (!subPopup->isVisible())
        QApplication::processEvents();
    QTest::keyClick( subPopup, Qt::Key_I );
    QVERIFY( subItem );
    QTest::keyClick( testWidget, Qt::Key_B, Qt::AltModifier );
    while (!popup->isVisible())
        QApplication::processEvents();
    QTest::keyClick( popup, 'U' );
    QVERIFY( shft );

    QTest::keyClick( testWidget, Qt::Key_B, Qt::AltModifier );
    QTest::keyClick( popup, '!' );
    QVERIFY( excl );

    QTest::keyClick( testWidget, Qt::Key_B, Qt::AltModifier );
    QTest::keyClick( popup, 'Y' );
    QVERIFY( spicy );
#else
    QSKIP("Mac OS X doesn't use mnemonics", SkipAll);
#endif

}

void tst_Q3PopupMenu::fontPropagation()
{
    QFont newfont = QFont( "times", 24 );
    QFont originalFont = popup->font();
    testWidget->setFont( QFont( "times", 24 ) );
    QVERIFY( !popup->ownFont() );
    QVERIFY( !(popup->font() == newfont) );
    QApplication::setFont( newfont, TRUE );
    QVERIFY( !popup->ownFont() );
    QVERIFY( popup->font() == newfont );
}

void tst_Q3PopupMenu::accel_data()
{
    QTest::addColumn<QKeySequence>("accelerator");
    QTest::addColumn<int>("id");
    QTest::addColumn<QString>("accelString");

#ifndef Q_WS_MAC
    QTest::newRow("simple_accel") << QKeySequence("CTRL+C") << 1 << QString("Ctrl+C");
    QTest::newRow("complex_accel") << QKeySequence("CTRL+ALT+SHIFT+T") << 2 << QString("Ctrl+Alt+Shift+T");
#else
    QTest::newRow("simple_accel") << QKeySequence("CTRL+C") << 1
                               << QString(QChar(kCommandUnicode) + 'C');
    QTest::newRow("complex_accel") << QKeySequence("CTRL+ALT+SHIFT+T") << 2
                                << QString(QChar(kOptionUnicode) + QString(QChar(kShiftUnicode))
                                   + QString(QChar(kCommandUnicode)) + 'T');
#endif
}

void tst_Q3PopupMenu::accel()
{
    QFETCH(QKeySequence, accelerator);
    QFETCH(int, id);
    QFETCH(QString, accelString);

    popup->insertItem("Dummy item", this, SLOT(dummySlot()), accelerator, id);
    QCOMPARE(accelString, (QString)popup->accel(id));
}

void tst_Q3PopupMenu::testItemParameter()
{
#if !defined(Q_WS_MAC)
    itemParameter = 0;
    int id = popup->insertItem( tr("&ItemParameter"), this, SLOT(itemParameterChanged(int)));
    popup->setItemParameter(id, 17);
    QTest::keyClick( testWidget, Qt::Key_B, Qt::AltModifier );
    while (!popup->isVisible())
        QApplication::processEvents();
    QTest::keyClick( popup, 'I' );
    QCOMPARE(itemParameter, 17);
#else
    QSKIP("Mac OS X doesn't use mnemonics", SkipAll);
#endif
}


QTEST_MAIN(tst_Q3PopupMenu)
#include "tst_q3popupmenu.moc"
