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
#include <qapplication.h>
#include <q3accel.h>
#include <qtextedit.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qstring.h>

class AccelForm;
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QMainWindow)

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3Accel : public QObject
{
    Q_OBJECT
public:
    tst_Q3Accel();
    virtual ~tst_Q3Accel();


public slots:
    void accelTrig1() { currentResult = Accel1Triggered; }
    void accelTrig2() { currentResult = Accel2Triggered; }
    void ambig1() { currentResult = Ambiguous; ambigResult = Accel1Triggered; }
    void ambig2() { currentResult = Ambiguous; ambigResult = Accel2Triggered; }
    void statusMessage( const QString& message ) { sbText = message; }

public slots:
    void initTestCase();
private slots:
    void number_data();
    void number();
    void text_data();
    void text();
    void disabledItems();
    void ambiguousItems();
    void unicodeCompare();
    void unicodeCompose();

protected:
    static int toButtons( int key );
    void defElements();
    void sendKeyEvents( int k1, QChar c1 = 0, int k2 = 0, QChar c2 = 0, int k3 = 0, QChar c3 = 0, int k4 = 0, QChar c4 = 0 );
    void testElement();

    enum Action {
        SetupAccel,
        TestAccel,
        ClearAll
    } currentAction;

    enum Widget {
        NoWidget,
        Accel1,
        Accel2
    };

    enum Result {
        NoResult,
        Accel1Triggered,
        Accel2Triggered,
        Ambiguous
    } currentResult;

    QMainWindow *mainW;
    Q3Accel *accel1;
    Q3Accel *accel2;
    QTextEdit *edit;
    QString sbText;
    Result ambigResult;
};

// copied from qkeysequence.cpp
const QString MacCtrl = QString(QChar(0x2318));
const QString MacMeta = QString(QChar(0x2303));
const QString MacAlt = QString(QChar(0x2325));
const QString MacShift = QString(QChar(0x21E7));

tst_Q3Accel::tst_Q3Accel(): mainW( 0 )
{
}

tst_Q3Accel::~tst_Q3Accel()
{
    delete mainW;
}

void tst_Q3Accel::initTestCase()
{
    currentResult = NoResult;
    mainW = new QMainWindow(0);
    mainW->setObjectName("main window");
    mainW->setFixedSize( 100, 100 );
    accel1 = new Q3Accel(mainW, "test_accel1" );
    accel2 = new Q3Accel(mainW, "test_accel2" );
    edit   = new QTextEdit(mainW);
    edit->setObjectName("test_edit");
    connect( accel1, SIGNAL(activated(int)), this, SLOT(accelTrig1()) );
    connect( accel2, SIGNAL(activated(int)), this, SLOT(accelTrig2()) );
    connect( accel1, SIGNAL(activatedAmbiguously(int)), this, SLOT(ambig1()) );
    connect( accel2, SIGNAL(activatedAmbiguously(int)), this, SLOT(ambig2()) );
    mainW->setCentralWidget( edit );
    connect( mainW->statusBar(), SIGNAL(messageChanged(const QString&)),
             this, SLOT(statusMessage(const QString&)) );
    qApp->setMainWidget( mainW );
    mainW->show();
}

int tst_Q3Accel::toButtons( int key )
{
    int result = 0;
    if ( key & Qt::SHIFT )
        result |= Qt::ShiftModifier;
    if ( key & Qt::CTRL )
        result |= Qt::ControlModifier;
    if ( key & Qt::META )
        result |= Qt::MetaModifier;
    if ( key & Qt::ALT )
        result |= Qt::AltModifier;
    return result;
}

void tst_Q3Accel::defElements()
{
    QTest::addColumn<int>("theAction");
    QTest::addColumn<int>("theTestWidget");
    QTest::addColumn<QString>("theString");
    QTest::addColumn<int>("the1Key");
    QTest::addColumn<int>("the1Char");
    QTest::addColumn<int>("the2Key");
    QTest::addColumn<int>("the2Char");
    QTest::addColumn<int>("the3Key");
    QTest::addColumn<int>("the3Char");
    QTest::addColumn<int>("the4Key");
    QTest::addColumn<int>("the4Char");
    QTest::addColumn<int>("theResult");
}

void tst_Q3Accel::number()
{
    testElement();
}
void tst_Q3Accel::text()
{
    testElement();
}
// ------------------------------------------------------------------
// Number Elements --------------------------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::number_data()
{
    defElements();

    // Clear all
    QTest::newRow( "N00 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    //===========================================
    // [Shift + key] on non-shift accels testing
    //===========================================

    /* Testing Single Sequences
       Shift + Qt::Key_M    on  Qt::Key_M
               Qt::Key_M    on  Qt::Key_M
       Shift + Qt::Key_Plus on  Qt::Key_Pluss
               Qt::Key_Plus on  Qt::Key_Pluss
    */
    QTest::newRow( "N001 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N001:Shift + M - [M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N001:M - [M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "N001 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N001:Shift + + [+]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "N001:+ [+]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    QTest::newRow( "N001 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + Qt::Key_M    on  Shift + Qt::Key_M
               Qt::Key_M    on  Shift + Qt::Key_M
       Shift + Qt::Key_Plus on  Shift + Qt::Key_Pluss
               Qt::Key_Plus on  Shift + Qt::Key_Pluss
    */
    QTest::newRow( "N002 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N002:Shift + M - [Shift + M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N002:M - [Shift + M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N002 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Plus) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N002:Shift + + [Shift + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "N002:+ [Shift + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N002 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + Qt::Key_F1   on  Qt::Key_F1
               Qt::Key_F1   on  Qt::Key_F1
    */
    QTest::newRow( "N003 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N003:Shift + F1 - [F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N003:F1 - [F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "N003 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + Qt::Key_F1   on  Shift + Qt::Key_F1
               Qt::Key_F1   on  Shift + Qt::Key_F1
    */
    QTest::newRow( "N004 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N004:Shift + F1 - [Shift + F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N004:F1 - [Shift + F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N004 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
               Qt::Key_BackTab  on  Qt::Key_Tab
       Shift + Qt::Key_Tab      on  Qt::Key_Tab
               Qt::Key_Tab      on  Qt::Key_Tab
    */
    QTest::newRow( "N005 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N005:BackTab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_BackTab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N005:Shift + Tab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + Qt::Key_Tab << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N005:Tab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "N005 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
               Qt::Key_BackTab  on  Shift + Qt::Key_Tab
       Shift + Qt::Key_Tab      on  Shift + Qt::Key_Tab
               Qt::Key_Tab      on  Shift + Qt::Key_Tab
    */
    QTest::newRow( "N006 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N006:BackTab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_BackTab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N006:Shift + Tab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N006:Tab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N006 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;


    //===========================================
    // [Shift + key] and [key] on accels with
    // and without modifiers
    //===========================================

    /* Testing Single Sequences
       Qt::Key_F1
       Shift + Qt::Key_F1
    */
    QTest::newRow( "N007 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N007 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N007:F1" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N007:Shift + F1" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    QTest::newRow( "N007 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Qt::Key_M
       Shift + Qt::Key_M
       Ctrl  + Qt::Key_M
       Alt   + Qt::Key_M
    */
    QTest::newRow( "N01 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N02 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N03 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::CTRL) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N04 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::ALT) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);


    QTest::newRow( "N:Qt::Key_M" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:Shift + Qt::Key_M" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "N:Ctrl + Qt::Key_M" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::CTRL) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:Alt + Qt::Key_M" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::ALT) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    /* Testing Single Sequence Ambiguity
       Qt::Key_M on accel2
    */
    QTest::newRow( "N05 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N:int(Qt::Key_M) on int(Accel2)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Ambiguous);

    /* Testing Single Specialkeys
       int(Qt::Key_aring)
       int(Qt::Key_Aring)
       UNICODE_ACCEL + int(Qt::Key_K)
    */
    QTest::newRow( "N06 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_aring) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N08 - sA2" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::UNICODE_ACCEL) + int(Qt::Key_K) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N:int(Qt::Key_aring)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_aring) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:int(Qt::Key_aring) - Text Form" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_unknown) << 0xE5 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:int(Qt::Key_Aring) - Text Form" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_unknown) << 0xC5 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:UNICODE_ACCEL + int(Qt::Key_K)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::UNICODE_ACCEL) + int(Qt::Key_K) << int('k') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    // Clear all
    QTest::newRow( "N09 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Multiple Sequences
       int(Qt::Key_M)
       int(Qt::Key_I), int(Qt::Key_M)
       Shift+int(Qt::Key_I), int(Qt::Key_M)
    */
    QTest::newRow( "N10 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N11 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("")
        << int(Qt::Key_I) << 0 << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "N12 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_I) << 0 << int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "N:int(Qt::Key_M) (2)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "N:int(Qt::Key_I), int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_I) << int('i') << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "N:Shift+int(Qt::Key_I), int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_I) << int('I') << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << int(Accel1Triggered);
}

// ------------------------------------------------------------------
// Text Elements ----------------------------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::text_data()
{
    defElements();
    // Clear all
    QTest::newRow( "T00 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    //===========================================
    // [Shift + key] on non-shift accels testing
    //===========================================

    /* Testing Single Sequences
       Shift + int(Qt::Key_M)    on  int(Qt::Key_M)
               int(Qt::Key_M)    on  int(Qt::Key_M)
       Shift + int(Qt::Key_Plus) on  Qt::Key_Pluss
               int(Qt::Key_Plus) on  Qt::Key_Pluss
    */
    QTest::newRow( "T001 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T001:Shift + M - [M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T001:M - [M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "T001 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("+")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T001:Shift + + [+]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "T001:+ [+]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    QTest::newRow( "T001 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + int(Qt::Key_M)    on  Shift + int(Qt::Key_M)
               int(Qt::Key_M)    on  Shift + int(Qt::Key_M)
       Shift + int(Qt::Key_Plus) on  Shift + Qt::Key_Pluss
               int(Qt::Key_Plus) on  Shift + Qt::Key_Pluss
       Shift + Ctrl + int(Qt::Key_Plus) on  Ctrl + Qt::Key_Pluss
               Ctrl + int(Qt::Key_Plus) on  Ctrl + Qt::Key_Pluss
    */
    QTest::newRow( "T002 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Shift+M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T002:Shift + M - [Shift + M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T002:M - [Shift + M]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T002 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("Shift++")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T002:Shift + + [Shift + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "T002:+ [Shift + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T002 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + Ctrl + int(Qt::Key_Plus) on  Ctrl + int(Qt::Key_Plus)
               Ctrl + int(Qt::Key_Plus) on  Ctrl + int(Qt::Key_Plus)
                      int(Qt::Key_Plus) on  Ctrl + int(Qt::Key_Plus)
    */
    QTest::newRow( "T002b - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Ctrl++")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T002b:Shift + Ctrl + + [Ctrl + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::CTRL) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T002b:Ctrl + [Ctrl + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::CTRL) + int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T002b: + [Ctrl + +]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Plus) << int('+') << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    /* Testing Single Sequences
       Shift + int(Qt::Key_F1)   on  int(Qt::Key_F1)
               int(Qt::Key_F1)   on  int(Qt::Key_F1)
    */
    QTest::newRow( "T003 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("F1")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T003:Shift + F1 - [F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T003:F1 - [F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "T003 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       Shift + int(Qt::Key_F1)   on  Shift + int(Qt::Key_F1)
               int(Qt::Key_F1)   on  Shift + int(Qt::Key_F1)
    */
    QTest::newRow( "T004 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Shift+F1")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T004:Shift + F1 - [Shift + F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T004:F1 - [Shift + F1]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T004 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
               int(Qt::Key_BackTab)  on  int(Qt::Key_Tab)
       Shift + int(Qt::Key_Tab)      on  int(Qt::Key_Tab)
               int(Qt::Key_Tab)      on  int(Qt::Key_Tab)
    */
    QTest::newRow( "T005 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Tab")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T005:BackTab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_BackTab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T005:Shift + Tab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T005:Tab - [Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    QTest::newRow( "N005 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
               int(Qt::Key_BackTab)  on  Shift + int(Qt::Key_Tab)
       Shift + int(Qt::Key_Tab)      on  Shift + int(Qt::Key_Tab)
               int(Qt::Key_Tab)      on  Shift + int(Qt::Key_Tab)
    */
    QTest::newRow( "T006 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Shift+Tab")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T006:BackTab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_BackTab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T006:Shift + Tab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T006:Tab - [Shift + Tab]" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_Tab) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T006 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    //===========================================
    // [Shift + key] and [key] on accels with
    // and without modifiers
    //===========================================

    /* Testing Single Sequences
       int(Qt::Key_F1)
       Shift + int(Qt::Key_F1)
    */
    QTest::newRow( "T007 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("F1")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T007 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("Shift+F1")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T007:F1" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T007:Shift + F1" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_F1) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    QTest::newRow( "T005 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Single Sequences
       int(Qt::Key_M)
       Shift + int(Qt::Key_M)
       Ctrl  + int(Qt::Key_M)
       Alt   + int(Qt::Key_M)
    */
    QTest::newRow( "T01 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T02 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("Shift+M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T03 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Ctrl+M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T04 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("Alt+M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);


    QTest::newRow( "T:int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:Shift + int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_M) << int('M') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "T:Ctrl + int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::CTRL) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:Alt + int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::ALT) + int(Qt::Key_M) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel2Triggered);

    /* Testing Single Sequence Ambiguity
       int(Qt::Key_M) on accel2
    */
    QTest::newRow( "T05 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T:int(Qt::Key_M) on int(Accel2)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Ambiguous);

    /* Testing Single Specialkeys
       int(Qt::Key_aring)
       int(Qt::Key_Aring)
       UNICODE_ACCEL + int(Qt::Key_K)
    */
    /* see comments above on the #ifdef'ery */
    QTest::newRow( "T06 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("\x0E5")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T08 - sA2" ) << int(SetupAccel) << int(Accel1) << QString("K")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T:int(Qt::Key_aring)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_aring) << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:int(Qt::Key_aring) - Text Form" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_unknown) << 0xE5 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:int(Qt::Key_Aring) - Text Form" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_unknown) << 0xC5 << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:UNICODE_ACCEL + int(Qt::Key_K)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::UNICODE_ACCEL) + int(Qt::Key_K) << int('k') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);

    // Clear all
    QTest::newRow( "T09 - clear" ) << int(ClearAll) <<0<<QString("")<<0<<0<<0<<0<<0<<0<<0<<0<<0;

    /* Testing Multiple Sequences
       int(Qt::Key_M)
       int(Qt::Key_I), int(Qt::Key_M)
       Shift+int(Qt::Key_I), int(Qt::Key_M)
    */
    QTest::newRow( "T10 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T11 - sA2" ) << int(SetupAccel) << int(Accel2) << QString("I, M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);
    QTest::newRow( "T12 - sA1" ) << int(SetupAccel) << int(Accel1) << QString("Shift+I, M")
        << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << int(NoResult);

    QTest::newRow( "T:int(Qt::Key_M) (2)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << 0 << 0 << int(Accel1Triggered);
    QTest::newRow( "T:int(Qt::Key_I), int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::Key_I) << int('i') << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << int(Accel2Triggered);
    QTest::newRow( "T:Shift+int(Qt::Key_I), int(Qt::Key_M)" ) << int(TestAccel) << int(NoWidget) << QString("")
        << int(Qt::SHIFT) + int(Qt::Key_I) << int('I') << int(Qt::Key_M) << int('m') << 0 << 0 << 0 << 0 << int(Accel1Triggered);
}

// ------------------------------------------------------------------
// Disabled Elements ------------------------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::disabledItems()
{
    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );

    /* Testing Disabled Accel
       Qt::Key_M          on A1
       Shift + Qt::Key_M  on A1
       Qt::Key_M          on A2 (disabled)
       Shift + Qt::Key_M  on A2 (disabled)
    */
    accel1->clear();
    accel2->clear();
    accel1->insertItem( QKeySequence("M") );
    accel1->insertItem( QKeySequence("Shift+M") );
    accel2->insertItem( QKeySequence("M") );
    accel2->insertItem( QKeySequence("Shift+M") );
    accel2->setEnabled( FALSE );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_M, 'm' );
    QCOMPARE( currentResult, Accel1Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::SHIFT+Qt::Key_M, 'M' );
    QCOMPARE( currentResult, Accel1Triggered );
    accel2->setEnabled( TRUE ); //PS!

    /* Testing Disabled Accel
       Qt::Key_M          on A1
       Shift + Qt::Key_M  on A1 (disabled)
       Qt::Key_M          on A2 (disabled)
       Shift + Qt::Key_M  on A2
    */
    accel1->clear();
    accel2->clear();
    accel1->insertItem( QKeySequence("M") );
    int i1 = accel1->insertItem( QKeySequence("Shift+M") );
    int i2 = accel2->insertItem( QKeySequence("M") );
    accel2->insertItem( QKeySequence("Shift+M") );
    accel1->setItemEnabled( i1, FALSE );
    accel2->setItemEnabled( i2, FALSE );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_M, 'm' );
    QCOMPARE( currentResult, Accel1Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::SHIFT+Qt::Key_M, 'M' );
    QCOMPARE( currentResult, Accel2Triggered );

    /* Testing Disabled Accel Items
       Qt::Key_F5          on A1
       Shift + Qt::Key_F5  on A2 (disabled)
    */
    accel1->clear();
    accel2->clear();
    i1 = accel1->insertItem( QKeySequence("F5") );
    i2 = accel2->insertItem( QKeySequence("Shift+F5") );
    accel1->setItemEnabled( i1, TRUE );
    accel2->setItemEnabled( i2, FALSE );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_F5, 0 );
    QCOMPARE( currentResult, Accel1Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::SHIFT+Qt::Key_F5, 0 );
    QCOMPARE( currentResult, NoResult );

    /* Testing Disabled Accel
       Qt::Key_F5          on A1
       Shift + Qt::Key_F5  on A2 (disabled)
    */
    accel1->clear();
    accel2->clear();
    i1 = accel1->insertItem( QKeySequence("F5") );
    i2 = accel2->insertItem( QKeySequence("Shift+F5") );
    accel1->setEnabled( TRUE );
    accel2->setEnabled( FALSE );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_F5, 0 );
    QCOMPARE( currentResult, Accel1Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::SHIFT+Qt::Key_F5, 0 );
    QCOMPARE( currentResult, NoResult );



    /* Testing Disabled Accel, and the corresponding statusbar feedback
       Ctrl + Qt::Key_K, Ctrl + Qt::Key_L on A1
       Ctrl + Qt::Key_K, Ctrl + Qt::Key_M on A2 (disabled)
    */
    accel1->clear();
    accel2->clear();
    i1 = accel1->insertItem( QKeySequence("Ctrl+K, Ctrl+L") );
    i2 = accel2->insertItem( QKeySequence("Ctrl+K, Ctrl+M") );
    accel1->setItemEnabled( i1, TRUE );
    accel2->setItemEnabled( i2, FALSE );
    currentResult = NoResult;
    sendKeyEvents( Qt::CTRL+Qt::Key_K, 0 );
    sendKeyEvents( Qt::CTRL+Qt::Key_Q, 0 );
    QCOMPARE( currentResult, NoResult );
#ifndef Q_WS_MAC
    QCOMPARE( sbText, QString("Ctrl+K, Ctrl+Q not defined") );
#else
    QCOMPARE(sbText, MacCtrl + "K, " + MacCtrl + "Q not defined");
#endif
    currentResult = NoResult;
    sendKeyEvents( Qt::CTRL+Qt::Key_K, 0 );
    sendKeyEvents( Qt::CTRL+Qt::Key_M, 0 );
    QCOMPARE( currentResult, NoResult );
    QCOMPARE(sbText, QString());
    currentResult = NoResult;
    sendKeyEvents( Qt::CTRL+Qt::Key_K, 0 );
    sendKeyEvents( Qt::CTRL+Qt::Key_L, 0 );
    QCOMPARE( currentResult, Accel1Triggered );
    QCOMPARE(sbText, QString());

    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );
}

// ------------------------------------------------------------------
// Ambiguous Elements -----------------------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::ambiguousItems()
{
    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );

    /* Testing Disabled Accel
       Qt::Key_M  on A1
       Qt::Key_M  on A2
    */
    accel1->clear();
    accel2->clear();
    accel1->insertItem( QKeySequence("M") );
    accel2->insertItem( QKeySequence("M") );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_M, 'm' );
    QCOMPARE( currentResult, Ambiguous );
    QCOMPARE( ambigResult, Accel1Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_M, 'm' );
    QCOMPARE( currentResult, Ambiguous );
    QCOMPARE( ambigResult, Accel2Triggered );
    currentResult = NoResult;
    sendKeyEvents( Qt::Key_M, 'm' );
    QCOMPARE( currentResult, Ambiguous );
    QCOMPARE( ambigResult, Accel1Triggered );

    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );
}

// ------------------------------------------------------------------
// Unicode and non-unicode Elements ---------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::unicodeCompare()
{
    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );

    QKeySequence ks1("Ctrl+M");
    QKeySequence ks2( Qt::CTRL+Qt::Key_M );
    accel1->clear();
    accel2->clear();
    accel1->insertItem( ks1 );
    accel2->insertItem( ks2 );
    currentResult = NoResult;
    sendKeyEvents( Qt::CTRL+Qt::Key_M, 0 );
    QCOMPARE( currentResult, Ambiguous );
    // They _are_ ambiguous, so the QKeySequence operator==
    // should indicate the same
    QVERIFY( ks1 == ks2 );
    QVERIFY( !(ks1 != ks2) );
}

// ------------------------------------------------------------------
// Unicode composed keys using keypad, and number accels ------------
// ------------------------------------------------------------------
void tst_Q3Accel::unicodeCompose()
{
    accel1->setEnabled( TRUE );
    accel2->setEnabled( TRUE );

#if 0
//#if QT_VERSION >= 0x30200
    accel1->clear();
    accel2->clear();
    accel1->insertItem( META+Qt::Key_9 );
    edit->setFocus();
    currentResult = NoResult;

    // Turn on unicode composing, and
    // compose a smileyface. ( 9786 = 0x263A )
    QApplication::setMetaComposedUnicode();
    QChar ch1( '9' );
    QChar ch2( '7' );
    QChar ch3( '8' );
    QChar ch4( '6' );
    QString s1( ch1 );
    QString s2( ch2 );
    QString s3( ch3 );
    QString s4( ch4 );
    QKeyEvent k1a( QEvent::AccelOverride, Qt::Key_9, ch1.row() ? 0 : ch1.cell(), Qt::MetaModifier + Qt::KeypadModifier, s1 );
    QKeyEvent k1p( QEvent::KeyPress, Qt::Key_9, ch1.row() ? 0 : ch1.cell(), Qt::MetaModifier + Qt::KeypadModifier, s1 );
    QKeyEvent k1r( QEvent::KeyRelease, Qt::Key_9, ch1.row() ? 0 : ch1.cell(), Qt::MetaModifier + Qt::KeypadModifier, s1 );
    QKeyEvent k2a( QEvent::AccelOverride, Qt::Key_7, ch2.row() ? 0 : ch2.cell(), Qt::MetaModifier + Qt::KeypadModifier, s2 );
    QKeyEvent k2p( QEvent::KeyPress, Qt::Key_7, ch2.row() ? 0 : ch2.cell(), Qt::MetaModifier + Qt::KeypadModifier, s2 );
    QKeyEvent k2r( QEvent::KeyRelease, Qt::Key_7, ch2.row() ? 0 : ch2.cell(), Qt::MetaModifier + Qt::KeypadModifier, s2 );
    QKeyEvent k3a( QEvent::AccelOverride, Qt::Key_8, ch3.row() ? 0 : ch3.cell(), Qt::MetaModifier + Qt::KeypadModifier, s3 );
    QKeyEvent k3p( QEvent::KeyPress, Qt::Key_8, ch3.row() ? 0 : ch3.cell(), Qt::MetaModifier + Qt::KeypadModifier, s3 );
    QKeyEvent k3r( QEvent::KeyRelease, Qt::Key_8, ch3.row() ? 0 : ch3.cell(), Qt::MetaModifier + Qt::KeypadModifier, s3 );
    QKeyEvent k4a( QEvent::AccelOverride, Qt::Key_6, ch4.row() ? 0 : ch4.cell(), Qt::MetaModifier + Qt::KeypadModifier, s4 );
    QKeyEvent k4p( QEvent::KeyPress, Qt::Key_6, ch4.row() ? 0 : ch4.cell(), Qt::MetaModifier + Qt::KeypadModifier, s4 );
    QKeyEvent k4r( QEvent::KeyRelease, Qt::Key_6, ch4.row() ? 0 : ch4.cell(), Qt::MetaModifier + Qt::KeypadModifier, s4 );
    QKeyEvent k5r( QEvent::KeyRelease, Qt::Key_Meta, 0, 0, "" );
    QApplication::sendEvent( edit, &k1a );
    QApplication::sendEvent( edit, &k1p );
    QApplication::sendEvent( edit, &k1r );
    QCOMPARE( currentResult, NoResult );

    QApplication::sendEvent( edit, &k2a );
    QApplication::sendEvent( edit, &k2p );
    QApplication::sendEvent( edit, &k2r );
    QCOMPARE( currentResult, NoResult );

    QApplication::sendEvent( edit, &k3a );
    QApplication::sendEvent( edit, &k3p );
    QApplication::sendEvent( edit, &k3r );
    QCOMPARE( currentResult, NoResult );

    QApplication::sendEvent( edit, &k4a );
    QApplication::sendEvent( edit, &k4p );
    QApplication::sendEvent( edit, &k4r );
    QCOMPARE( currentResult, NoResult );

    QApplication::sendEvent( edit, &k5r );
    // Unicode character is fully composed, and
    // shouldn't have triggered any accels
    QCOMPARE( currentResult, NoResult );

    // Verify that the unicode character indeed
    // is composed
    QVERIFY( edit->text().length() == 1 );
    QCOMPARE( (int)edit->text()[0].unicode(), 9786 );

    sendKeyEvents( META+Qt::Key_9, 0 );
    QCOMPARE( currentResult, Accel1Triggered );
#else
    QSKIP( "Unicode composing non-existant in Qt 3.y.z", SkipAll);
#endif
}

// ------------------------------------------------------------------
// Element Testing  -------------------------------------------------
// ------------------------------------------------------------------
void tst_Q3Accel::testElement()
{
#if 0
    currentResult = NoResult;
    QFETCH( int, theAction );
    QFETCH( int, theTestWidget );
    QFETCH( QString, theString );
    QFETCH( int, the1Key );
    QFETCH( int, the1Char );
    QFETCH( int, the2Key );
    QFETCH( int, the2Char );
    QFETCH( int, the3Key );
    QFETCH( int, the3Char );
    QFETCH( int, the4Key );
    QFETCH( int, the4Char );
    QFETCH( int, theResult );

    Q3Accel *ac = 0;
    switch( theTestWidget ) {
    case Accel1:
        ac = accel1;
        break;
    case Accel2:
        ac = accel2;
        break;
    case NoWidget:
    default:
        ac = 0;
    }

    if ( theAction == ClearAll ) {
        // Clear all current accelerators
        accel1->clear();
        accel2->clear();
        QCOMPARE( TRUE, TRUE );
    } else if ( theAction == SetupAccel ) {
        // Set up accelerator for next test
        QKeySequence ks;
        if ( QString(theString).isEmpty() ) {
            ks =  QKeySequence( the1Key, the2Key, the3Key, the4Key );
        } else {
            ks = QKeySequence( theString );
        }
        ac->insertItem( ks );
        QCOMPARE( TRUE, TRUE );
    } else {
        // Send keyevents
        sendKeyEvents( the1Key, the1Char,
                       the2Key, the2Char,
                       the3Key, the3Char,
                       the4Key, the4Char );
        // Verify the result
        QCOMPARE( currentResult, theResult );
    }
#endif
}

void tst_Q3Accel::sendKeyEvents( int k1, QChar c1, int k2, QChar c2, int k3, QChar c3, int k4, QChar c4 )
{
    int b1 = toButtons( k1 );
    int b2 = toButtons( k2 );
    int b3 = toButtons( k3 );
    int b4 = toButtons( k4 );
    k1 &= ~Qt::MODIFIER_MASK;
    k2 &= ~Qt::MODIFIER_MASK;
    k3 &= ~Qt::MODIFIER_MASK;
    k4 &= ~Qt::MODIFIER_MASK;
    QKeyEvent ke( QEvent::Accel, k1, (Qt::KeyboardModifiers)b1, QString(c1) );
    QApplication::sendEvent( mainW, &ke );
    if ( k2 ) {
        QKeyEvent ke( QEvent::Accel, k2, k2, b2, QString(c2) );
        QApplication::sendEvent( mainW, &ke );
    }
    if ( k3 ) {
        QKeyEvent ke( QEvent::Accel, k3, k3, b3, QString(c3) );
        QApplication::sendEvent( mainW, &ke );
    }
    if ( k4 ) {
        QKeyEvent ke( QEvent::Accel, k4, k4, b4, QString(c4) );
        QApplication::sendEvent( mainW, &ke );
    }
}

QTEST_MAIN(tst_Q3Accel)
#include "tst_q3accel.moc"
