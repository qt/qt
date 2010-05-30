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


#include <q3datetimeedit.h>

//TESTED_CLASS=
//TESTED_FILES=qt3support/text/q3textedit.h qt3support/text/q3textedit.cpp

class tst_Q3TimeEdit : public QObject
{
    Q_OBJECT

public:
    tst_Q3TimeEdit();
    virtual ~tst_Q3TimeEdit();



public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void valueRange_data();
    void valueRange(); // Need a better name for this function

    void userKeyPress_AMPM_data();
    void userKeyPress_AMPM();

private:
    Q3TimeEdit* testWidget;
};

Q_DECLARE_METATYPE(QTime)

/*
    NOTE.
    Q3TimeEdit has a really strange behaviour IMO.
    This testcase tests that behaviour, which is totally different from what I
    would expect or what I like.

    In Q3TimeEdit...
    - the hour, minutes, seconds or AMPM have 'full' focus.
    - you don't get a blinking cursor.
    - you can't backspace or delete one digit from the value.
    - pressing backspace or delete 'resets' the hour to 0 (in 24 hour mode) or to
      12 (in 12 hour mode).
    - pressing backspace or delete 'resets' the minute to 0 (in both modes).
    - when you fast type two digits these are entered into the field that has the focus:
      example: entering 1 and then 2 results in '12' being entered into the hour field.
    - if you fast enter an invalid value, e.g. 2 and the 5 (in the hour field) only the
      2 is shown, and the 5 is ignored.
    - if you enter a 2, then wait for 2 seconds and then enter 5 then first the 2 is shown
      and then replaced by the 5. The 2 seconds is a timeout value. After that Q3TimeEdit
      assumes you start a new editing session and apparantly want to replace the contents
      of the focused field with something new. AGAIN.. this is a totally different behaviour
      from what I would expect, but it's the behaviour.
*/

tst_Q3TimeEdit::tst_Q3TimeEdit()
{
}

tst_Q3TimeEdit::~tst_Q3TimeEdit()
{

}

void tst_Q3TimeEdit::initTestCase()
{
    testWidget = new Q3TimeEdit(0, "testWidget");
    testWidget->show();
    qApp->setActiveWindow(testWidget);
    qApp->setMainWidget(testWidget);
    QTest::qWait(100);
}

void tst_Q3TimeEdit::cleanupTestCase()
{
    delete testWidget;
}

void tst_Q3TimeEdit::init()
{
    QTime minimumTime(0, 0, 0);
    QTime maximumTime(23, 59, 59);
    testWidget->setMinValue(minimumTime);
    testWidget->setMaxValue(maximumTime);
    // We don't want the locale impacting on the test
    testWidget->setDisplay(Q3TimeEdit::Hours | Q3TimeEdit::Minutes | Q3TimeEdit::Seconds);
    testWidget->setTime(QTime(11, 0, 0));

    // make sure we start with the hour focused
    QWidget *editBase = qFindChild<QWidget*>(testWidget, "time edit base");
    QTest::keyClick(editBase, Qt::Key_Left);
    QTest::keyClick(editBase, Qt::Key_Left);
    QTest::keyClick(editBase, Qt::Key_Left);
}

void tst_Q3TimeEdit::cleanup()
{
}

void tst_Q3TimeEdit::valueRange_data()
{
    QTest::addColumn<int>("minimumHours");
    QTest::addColumn<int>("minimumMinutes");
    QTest::addColumn<int>("minimumSeconds");
    QTest::addColumn<int>("maximumHours");
    QTest::addColumn<int>("maximumMinutes");
    QTest::addColumn<int>("maximumSeconds");

    QTest::newRow("data0") << 0 << 0 << 0 << 2 << 2 << 2;
}


void tst_Q3TimeEdit::valueRange()
{
    QFETCH(int, minimumHours);
    QFETCH(int, minimumMinutes);
    QFETCH(int, minimumSeconds);
    QFETCH(int, maximumHours);
    QFETCH(int, maximumMinutes);
    QFETCH(int, maximumSeconds);

//    Q3TimeEdit timeEdit(0);
    QTime minimumTime(minimumHours, minimumMinutes, minimumSeconds);
    QTime maximumTime(maximumHours, maximumMinutes, maximumSeconds);
    testWidget->setMinValue(minimumTime);
    testWidget->setMaxValue(maximumTime);
    // We don't want the locale impacting on the test
    testWidget->setDisplay(Q3TimeEdit::Hours | Q3TimeEdit::Minutes | Q3TimeEdit::Seconds);

    // When pressing Key_Up we want to check it goes to the minimum time
    testWidget->setTime(maximumTime);

    QKeyEvent ke(QEvent::KeyPress, Qt::Key_Up, 0, Qt::NoButton);

    // We need to say focusWidget() because the focus is inside the widget in the Q3TimeEdit which
    // Q3TimeEdit doesn't allow us to access directly.

    testWidget->setFocus();
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().hour(), minimumHours);

    // When pressing Key_Down we want to check it goes to the maximum time
    testWidget->setTime(minimumTime);

    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().hour(), maximumHours);

    // Now we test the minutes
    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    testWidget->setTime(maximumTime);

    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().minute(), minimumMinutes);

    testWidget->setTime(minimumTime);

    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().minute(), maximumMinutes);

    // Now we test the seconds
    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    testWidget->setTime(maximumTime);

    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().second(), minimumSeconds);

    testWidget->setTime(minimumTime);

    ke = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, 0, Qt::NoButton);
    QApplication::sendEvent(qApp->focusWidget(), &ke);
    QCOMPARE(testWidget->time().second(), maximumSeconds);
}

void tst_Q3TimeEdit::userKeyPress_AMPM_data()
{
    QTest::addColumn<QTime>("start_time");
    QTest::addColumn<bool>("ampm");
    QTest::addColumn<QTestEventList>("keys");
    QTest::addColumn<QTime>("expected_time");

    int time_delay = 4100;

    // ***************** test backspace ***************

    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Backspace);
        QTime expected(20, 0, 0);
        QTest::newRow("backspace sec: hh value: 12") << QTime(12, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Backspace);
        keys.addKeyClick(Qt::Key_Backspace);
        QTime expected(0, 0, 0);
        QTest::newRow("backspace x 2 sec: hh value: 12") << QTime(12, 0, 0) << bool(true) << keys << expected;
    }

    // ***************** test delete ***************

    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Delete);
        QTime expected(1, 0, 0);
        QTest::newRow("delete sec: hh value: 12") << QTime(12, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Delete);
        keys.addKeyClick(Qt::Key_Delete);
        QTime expected(0, 0, 0);
        QTest::newRow("delete x 2 sec: hh value: 12") << QTime(12, 0, 0) << bool(true) << keys << expected;
    }

    // ***************** test the hours ***************

    // use up/down keys to change hour in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(10, 0, 0);
        QTest::newRow("1") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<5; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(6, 0, 0);
        QTest::newRow("2") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<10; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(1, 0, 0);
        QTest::newRow("3") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<12; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(23, 0, 0);
        QTest::newRow("4") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Up);
        QTime expected(12, 0, 0);
        QTest::newRow("5") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(13, 0, 0);
        QTest::newRow("6") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // use up/down keys to change hour in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(10, 0, 0);
        QTest::newRow("7") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<5; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(6, 0, 0);
        QTest::newRow("8") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<10; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(1, 0, 0);
        QTest::newRow("9") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<12; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(23, 0, 0);
        QTest::newRow("10") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Up);
        QTime expected(12, 0, 0);
        QTest::newRow("11") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    {
        QTestEventList keys;
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(13, 0, 0);
        QTest::newRow("12") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a one digit valid hour
    {
        QTestEventList keys;
        keys.addKeyClick('5');
        QTime expected(5, 0, 0);
        QTest::newRow("13") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // entering a two digit valid hour
    {
        QTestEventList keys;
        keys.addKeyClick('1');
        keys.addKeyClick('1');
        QTime expected(11, 0, 0);
        QTest::newRow("14") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // entering an invalid hour
    {
        QTestEventList keys;
        keys.addKeyClick('2');
        // the '5' creates an invalid hour(25) so it must be ignored
        keys.addKeyClick('5');
        QTime expected(2, 0, 0);
        QTest::newRow("15") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter a value, change your mind and enter a new one
    {
        QTestEventList keys;
        keys.addKeyClick('2');
        keys.addDelay(time_delay);
        keys.addKeyClick('1');
        QTime expected(1, 0, 0);
        QTest::newRow("16") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter a one digit valid hour in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick('5');
        QTime expected(5, 0, 0);
        QTest::newRow("17") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a two digit valid hour in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick('1');
        keys.addKeyClick('1');
        QTime expected(11, 0, 0);
        QTest::newRow("18") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a two digit valid hour(>12) in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick('1');
        keys.addKeyClick('5');
        QTime expected(15, 0, 0);
        QTest::newRow("19") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a two digit valid hour(>20) in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick('2');
        keys.addKeyClick('1');
        QTime expected(21, 0, 0);
        QTest::newRow("20") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a two digit invalid hour(>23) in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick('2');
        keys.addKeyClick('4');
        QTime expected(2, 0, 0);
        QTest::newRow("21") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // ***************** test the minutes ***************

    // use up/down keys to change the minutes in 12 hour mode
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 2, 0);
        QTest::newRow("22") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<16; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 16, 0);
        QTest::newRow("23") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test maximum value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<59; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 59, 0);
        QTest::newRow("24") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test 'overflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<60; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 0);
        QTest::newRow("25") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test 'underflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 59, 0);
        QTest::newRow("26") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 58, 0);
        QTest::newRow("27") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // use up/down keys to change the minutes in 24 hour mode

    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 2, 0);
        QTest::newRow("28") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Left);
        keys.addKeyClick(Qt::Key_Left);
        keys.addKeyClick(Qt::Key_Left);
        keys.addKeyClick(Qt::Key_Left);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<16; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 16, 0);
        QTest::newRow("29") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test maximum value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<59; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 59, 0);
        QTest::newRow("30") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test 'overflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<60; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 0);
        QTest::newRow("31") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test 'underflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 59, 0);
        QTest::newRow("32") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 58, 0);
        QTest::newRow("33") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a valid one digit minute in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        QTime expected(11, 2, 0);
        QTest::newRow("34") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter a valid two digit minutes in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        keys.addKeyClick('4');
        QTime expected(11, 24, 0);
        QTest::newRow("35") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // check the lower limit of the minutes in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("36") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // check the upper limit of the minutes in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('5');
        keys.addKeyClick('9');
        QTime expected(11, 59, 0);
        QTest::newRow("37") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter an invalid two digit minutes in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('6');
        // '60' is invalid, so I would expect the '0' to be ignored...
        // but the edit is reset to '0'
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("38") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // test minutes in 24 hour mode. Behaviour should be exactly the same

    // enter a valid one digit minute in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        QTime expected(11, 2, 0);
        QTest::newRow("39") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a valid two digit minutes in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        keys.addKeyClick('4');
        QTime expected(11, 24, 0);
        QTest::newRow("40") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // check the lower limit of the minutes in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("41") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // check the upper limit of the minutes in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('5');
        keys.addKeyClick('9');
        QTime expected(11, 59, 0);
        QTest::newRow("42") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter an invalid two digit minutes in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('6');
        // '60' is invalid, so I would expect the '0' to be ignored...
        // but the edit is reset to '0'
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("43") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // ***************** test the seconds ***************

    // use up/down to edit the seconds...

    // use up/down keys to change the seconds in 12 hour mode
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 2);
        QTest::newRow("44") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<16; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 16);
        QTest::newRow("45") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test maximum value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<59; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 59);
        QTest::newRow("46") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test 'overflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<60; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 0);
        QTest::newRow("47") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test 'underflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 59);
        QTest::newRow("48") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    { // test valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 58);
        QTest::newRow("49") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // use up/down keys to change the seconds in 24 hour mode

    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 2);
        QTest::newRow("50") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test a valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<16; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 16);
        QTest::newRow("51") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test maximum value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<59; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 59);
        QTest::newRow("52") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test 'overflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<60; i++)
            keys.addKeyClick(Qt::Key_Up);
        QTime expected(11, 0, 0);
        QTest::newRow("53") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test 'underflow'
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 59);
        QTest::newRow("54") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }
    { // test valid value
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        for (uint i=0; i<2; i++)
            keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 58);
        QTest::newRow("55") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    /////////////////
    // enter a valid one digit second in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        QTime expected(11, 0, 2);
        QTest::newRow("56") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter a valid two digit seconds in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        keys.addKeyClick('4');
        QTime expected(11, 0, 24);
        QTest::newRow("57") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // check the lower limit of the seconds in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("58") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // check the upper limit of the seconds in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('5');
        keys.addKeyClick('9');
        QTime expected(11, 0, 59);
        QTest::newRow("59") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // enter an invalid two digit seconds in 12 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('6');
        // '60' is invalid, so I would expect the '0' to be ignored...
        // but the edit is reset to '0'
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("60") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }

    // test seconds in 24 hour mode. Behaviour should be exactly the same

    // enter a valid one digit minute in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        QTime expected(11, 0, 2);
        QTest::newRow("61") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter a valid two digit seconds in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('2');
        keys.addKeyClick('4');
        QTime expected(11, 0, 24);
        QTest::newRow("62") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // check the lower limit of the seconds in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("63") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // check the upper limit of the seconds in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('5');
        keys.addKeyClick('9');
        QTime expected(11, 0, 59);
        QTest::newRow("64") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // enter an invalid two digit seconds in 24 h mode
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick('6');
        // '60' is invalid, so I would expect the '0' to be ignored...
        // but the edit is reset to '0'
        keys.addKeyClick('0');
        QTime expected(11, 0, 0);
        QTest::newRow("65") << QTime(11, 0, 0) << bool(false) << keys << expected;
    }

    // Test the AMPM indicator
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Up);
        QTime expected(23, 0, 0);
        QTest::newRow("66") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    // Test the AMPM indicator
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(23, 0, 0);
        QTest::newRow("67") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    // Test the AMPM indicator
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Down);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 0);
        QTest::newRow("68") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
    // Test the AMPM indicator
    {
        QTestEventList keys;
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Right);
        keys.addKeyClick(Qt::Key_Up);
        keys.addKeyClick(Qt::Key_Down);
        QTime expected(11, 0, 0);
        QTest::newRow("69") << QTime(11, 0, 0) << bool(true) << keys << expected;
    }
}

void tst_Q3TimeEdit::userKeyPress_AMPM()
{
    // READ THE NOTE AT THE TOP FIRST!!!!!

    QFETCH(QTime, start_time);
    QFETCH(QTestEventList, keys);
    QFETCH(QTime, expected_time);
    QFETCH(bool, ampm);

    if (ampm)
        testWidget->setDisplay(Q3TimeEdit::Hours | Q3TimeEdit::Minutes | Q3TimeEdit::Seconds | Q3TimeEdit::AMPM);
    else
        testWidget->setDisplay(Q3TimeEdit::Hours | Q3TimeEdit::Minutes | Q3TimeEdit::Seconds);
    testWidget->setTime(start_time);
    keys.simulate(qFindChild<QWidget*>(testWidget, "time edit base"));
    QCOMPARE(testWidget->time(), expected_time);
}


QTEST_MAIN(tst_Q3TimeEdit)
#include "tst_q3timeedit.moc"
