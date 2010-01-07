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

#include <q3dockarea.h>
#include <q3dockwindow.h>
#include <q3mainwindow.h>
#include <qapplication.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3DockWindow : public QObject
{
    Q_OBJECT
public:
    tst_Q3DockWindow();
    virtual ~tst_Q3DockWindow();


public slots:
    void initTestCase();
    void cleanupTestCase();
private slots:
    void parents();
    void showChild();
};

QFrame *makeFrame( const char *text, QWidget *parent )
{
    QFrame* frame = new QFrame(parent);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setAutoAdd(true);
    new QLabel(text, frame);
    frame->setMinimumSize(200, 200);
    return frame;
}

Q3DockWindow* makeDock( const char* text, QWidget* parent )
{
    Q3DockWindow* dock = new Q3DockWindow(Q3DockWindow::InDock, parent, text);
    dock->setResizeEnabled(true);
    dock->setCloseMode(Q3DockWindow::Always);
    dock->setCaption(text);
    dock->setWidget(makeFrame(text, dock));
    dock->show();

    return dock;
}


tst_Q3DockWindow::tst_Q3DockWindow()

{
}

tst_Q3DockWindow::~tst_Q3DockWindow()
{
}

void tst_Q3DockWindow::initTestCase()
{
    // create a default mainwindow
    // If you run a widget test, this will be replaced in the testcase by the
    // widget under test
    QWidget *w = new QWidget(0,"mainWidget");
    w->setFixedSize( 200, 200 );
    qApp->setMainWidget( w );
    w->show();
}

void tst_Q3DockWindow::cleanupTestCase()
{
    delete qApp->mainWidget();
}

void tst_Q3DockWindow::parents()
{
    // create 5 dock windows, one for each dock area
    // and one for the mainwindow, in the end they should
    // all except the one with the mainwindow as parent should
    // have the same dock() and parent() pointer.
    Q3MainWindow mw;
    QFrame *central = makeFrame( "Central", &mw );
    mw.setCentralWidget( central );

    Q3DockWindow *topDock = makeDock( "Top", mw.topDock() );
    QVERIFY( topDock->area() == topDock->parent() );

    Q3DockWindow *leftDock = makeDock( "Left", mw.leftDock() );
    QVERIFY( leftDock->area() == leftDock->parent() );

    Q3DockWindow *rightDock= makeDock( "Right", mw.rightDock() );
    QVERIFY( rightDock->area() == rightDock->parent() );

    Q3DockWindow *bottomDock = makeDock( "Bottom", mw.bottomDock() );
    QVERIFY( bottomDock->area() == mw.bottomDock() );

    Q3DockWindow *mainDock = makeDock( "MainWindow as parent", &mw );
    QVERIFY( mainDock->parent() == mw.topDock() );
}


void tst_Q3DockWindow::showChild()
{
    // task 26225
    // calling show dose not propergate to child widgets if
    // main window is already showing

    Q3MainWindow mw;
    mw.show();
    Q3DockWindow * dock = new Q3DockWindow(&mw);
    QPushButton  * qpb = new QPushButton("hi", dock);
    dock->setWidget(qpb);
    dock->show();
    QVERIFY( mw.isVisible() );
    QVERIFY( dock->isVisible() );
    QVERIFY( qpb->isVisible() );
}



QTEST_MAIN(tst_Q3DockWindow)
#include "tst_q3dockwindow.moc"

