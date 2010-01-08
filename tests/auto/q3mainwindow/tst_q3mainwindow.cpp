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

#include <q3dockwindow.h>
#include <q3mainwindow.h>
#include <q3textedit.h>
#include <qapplication.h>
#include <qevent.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <qaction.h>

//TESTED_CLASS=Q3DockArea Q3DockAreaLayout
//TESTED_FILES=

class testMainWindow : public Q3MainWindow
{
public:
    testMainWindow(QWidget* parent=0, const char* name=0);
    ~testMainWindow();
    bool keysuccess;
protected:
    void keyPressEvent(QKeyEvent*);
};

class testLineEdit : public QLineEdit
{
public:
    testLineEdit(QWidget* parent=0, const char* name =0);
    ~testLineEdit();
protected:
    void keyPressEvent(QKeyEvent*);
};


class tst_Q3MainWindow : public QObject
{
    Q_OBJECT
public:
    tst_Q3MainWindow();
    ~tst_Q3MainWindow();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();
private slots:
    void propagateEscapeKeyTest();
    void testDockWindowMinimized();
    void testSetUsesBigPixmaps();

    void hideAndShow();

    // task-specific tests:
    void task176544_setDockEnabled();
    void task240766_layoutcrash();

private:
    testMainWindow* testWidget;
    testLineEdit* le;
};

testMainWindow::testMainWindow( QWidget* parent, const char* name )
    : Q3MainWindow( parent, name )
{
    keysuccess = FALSE;
}

testMainWindow::~testMainWindow()
{
}

void testMainWindow::keyPressEvent( QKeyEvent* ke )
{
    if ( ke->key() == Qt::Key_Escape )
	keysuccess = TRUE;
}


testLineEdit::testLineEdit( QWidget* parent, const char* name )
    : QLineEdit( parent, name )
{
}

testLineEdit::~testLineEdit()
{
}

void testLineEdit::keyPressEvent( QKeyEvent* ke )
{
    if ( ke->key() == Qt::Key_Escape )
	ke->ignore();
}

/*
    Nothing to do here.
*/

tst_Q3MainWindow::tst_Q3MainWindow()
{
}

/*
    Nothing to do here.
    The testwidget is deleted automatically.
*/

tst_Q3MainWindow::~tst_Q3MainWindow()
{
}

/*
    This function is called once when a testcase is being executed.
    You can use it to create the instance of a widget class and set it for instance
    as the mainwidget.
*/

void tst_Q3MainWindow::initTestCase()
{
    testWidget = new testMainWindow(0);
    QWidget *w = new QWidget(testWidget);
    testWidget->setCentralWidget(w);
    QVBoxLayout *vbl = new QVBoxLayout(w);
    le = new testLineEdit( w );
    vbl->addWidget(le);
    new Q3ToolBar(testWidget);
    qApp->setMainWidget( testWidget );
    testWidget->show();
}

void tst_Q3MainWindow::cleanupTestCase()
{
    delete testWidget;
}

/*
    Nothing to do here, but you could for instance use this to clean up temporary files
    you have been using in a test.
*/

void tst_Q3MainWindow::cleanup()
{
}

void tst_Q3MainWindow::propagateEscapeKeyTest()
{
    QTest::keyClick( testWidget, Qt::Key_Escape );
    QVERIFY( testWidget->keysuccess );
}

void tst_Q3MainWindow::testDockWindowMinimized()
{
    Q3MainWindow mw;
    Q3DockWindow *dw = new Q3DockWindow(&mw);
    QToolButton *btn = new QToolButton(dw);
    btn->setUsesTextLabel(true);
    btn->setTextLabel("foo");
    dw->setWidget(btn);
    mw.addDockWindow(dw, Qt::DockMinimized);
    mw.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&mw);
#endif
    qApp->processEvents();
    QEXPECT_FAIL(0, "This test started failing sometime during the 3.x lifetime", Continue);
    QVERIFY(dw->x() + dw->width() < 0);
    QEXPECT_FAIL(0, "This test started failing sometime during the 3.x lifetime", Continue);
    QVERIFY(dw->y() + dw->height() < 0);
}

void tst_Q3MainWindow::hideAndShow()
{
    Q3MainWindow mw;
    mw.show();

    Q3DockWindow *dw = new Q3DockWindow(&mw);
    QToolButton *btn = new QToolButton(dw);
    dw->setWidget(btn);
    mw.addDockWindow(dw, Qt::DockTornOff);

    QVERIFY(dw->isVisible());
    mw.hide();
    QTest::qWait(250);
    QVERIFY(!dw->isVisible());
    mw.show();
    QTest::qWait(250);

    QVERIFY(dw->isVisible());
}

void tst_Q3MainWindow::testSetUsesBigPixmaps()
{
    Q3MainWindow mw;
    Q3ToolBar toolbar(&mw);
    QAction action(&toolbar);
    toolbar.addAction(&action);

    QPixmap pix(16,16);
    action.setIcon(pix);

    mw.show();
    qApp->processEvents();

    mw.setUsesBigPixmaps(false);
    QToolButton *button = qFindChild<QToolButton*>(&toolbar);
    const QSize smallSize = button->size();

    mw.setUsesBigPixmaps(true);
    QVERIFY(button->width() > smallSize.width() && button->height() > smallSize.height());

}

void tst_Q3MainWindow::task176544_setDockEnabled()
{
    Q3MainWindow *mainWindow = new Q3MainWindow;
    mainWindow->setWindowTitle("Main Window");
    mainWindow->show();
    QCOMPARE(mainWindow->dockWindows().size(), 0);

    Q3DockWindow *dockWindow = new Q3DockWindow(mainWindow);
    dockWindow->show();
    QCOMPARE(mainWindow->dockWindows().size(), 1);

    mainWindow->setDockEnabled(dockWindow, Qt::DockLeft, false);
    QCOMPARE(mainWindow->dockWindows().size(), 1);

    mainWindow->setDockEnabled(dockWindow, Qt::DockRight, false);
    QCOMPARE(mainWindow->dockWindows().size(), 1);
}

void tst_Q3MainWindow::task240766_layoutcrash()
{
    Q3MainWindow main;
    
    QMenuBar* menubar = main.menuBar();
    
    Q3DockWindow *emptyDock = new Q3DockWindow(Q3DockWindow::InDock, &main);
    main.addDockWindow(emptyDock, Qt::DockLeft);
    
    Q3DockWindow *dock = new Q3DockWindow(Q3DockWindow::InDock, &main);
    main.addDockWindow(dock, Qt::DockLeft);
    dock->setResizeEnabled(true);
    
    Q3TextEdit *plotDialog = new Q3TextEdit(dock);
    dock->setWidget(plotDialog);
    main.show();
    QTest::qWait(100); 
    main.resize(180,180);
    QTest::qWait(100); //should not crash;
}

QTEST_MAIN(tst_Q3MainWindow)
#include "tst_q3mainwindow.moc"

