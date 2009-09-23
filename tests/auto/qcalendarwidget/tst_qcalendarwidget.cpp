/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qcalendarwidget.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qmenu.h>
#include <qdebug.h>
#include <qdatetime.h>
#include <qtextformat.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_QCalendarWidget : public QObject
{
    Q_OBJECT

public:
    tst_QCalendarWidget();
    virtual ~tst_QCalendarWidget();
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void getSetCheck();
    void buttonClickCheck();

    void setTextFormat();
    void resetTextFormat();

    void setWeekdayFormat();
};

// Testing get/set functions
void tst_QCalendarWidget::getSetCheck()
{
    QCalendarWidget object;

    //horizontal header formats
    object.setHorizontalHeaderFormat(QCalendarWidget::NoHorizontalHeader);
    QCOMPARE(QCalendarWidget::NoHorizontalHeader, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    QCOMPARE(QCalendarWidget::SingleLetterDayNames, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    QCOMPARE(QCalendarWidget::ShortDayNames, object.horizontalHeaderFormat());
    object.setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
    QCOMPARE(QCalendarWidget::LongDayNames, object.horizontalHeaderFormat());
    //vertical header formats
    object.setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    QCOMPARE(QCalendarWidget::ISOWeekNumbers, object.verticalHeaderFormat());
    object.setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    QCOMPARE(QCalendarWidget::NoVerticalHeader, object.verticalHeaderFormat());
    //maximum Date
    QDate maxDate(2006, 7, 3);
    object.setMaximumDate(maxDate);
    QCOMPARE(maxDate, object.maximumDate());
    //minimum date
    QDate minDate(2004, 7, 3);
    object.setMinimumDate(minDate);
    QCOMPARE(minDate, object.minimumDate());
    //day of week
    object.setFirstDayOfWeek(Qt::Thursday);
    QCOMPARE(Qt::Thursday, object.firstDayOfWeek());
    //grid visible
    object.setGridVisible(true);
    QVERIFY(object.isGridVisible());
    object.setGridVisible(false);
    QVERIFY(!object.isGridVisible());
    //header visible
    object.setHeaderVisible(true);
    QVERIFY(object.isHeaderVisible());
    object.setHeaderVisible(false);
    QVERIFY(!object.isHeaderVisible());
    //selection mode
    QCOMPARE(QCalendarWidget::SingleSelection, object.selectionMode());
    object.setSelectionMode(QCalendarWidget::NoSelection);
    QCOMPARE(QCalendarWidget::NoSelection, object.selectionMode());
    object.setSelectionMode(QCalendarWidget::SingleSelection);
    QCOMPARE(QCalendarWidget::SingleSelection, object.selectionMode());
   //selected date
    QDate selectedDate(2005, 7, 3);
    QSignalSpy spy(&object, SIGNAL(selectionChanged()));
    object.setSelectedDate(selectedDate);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(selectedDate, object.selectedDate());
    //month and year
    object.setCurrentPage(2004, 1);
    QCOMPARE(1, object.monthShown());
    QCOMPARE(2004, object.yearShown());
    object.showNextMonth();
    QCOMPARE(2, object.monthShown());
    object.showPreviousMonth();
    QCOMPARE(1, object.monthShown());
    object.showNextYear();
    QCOMPARE(2005, object.yearShown());
    object.showPreviousYear();
    QCOMPARE(2004, object.yearShown());
    //date range
    minDate = QDate(2006,1,1);
    maxDate = QDate(2010,12,31);
    object.setDateRange(minDate, maxDate);
    QCOMPARE(maxDate, object.maximumDate());
    QCOMPARE(minDate, object.minimumDate());

    //date should not go beyond the minimum.
    selectedDate = minDate.addDays(-10);
    object.setSelectedDate(selectedDate);
    QCOMPARE(minDate, object.selectedDate());
    QVERIFY(selectedDate != object.selectedDate());
    //date should not go beyond the maximum.
    selectedDate = maxDate.addDays(10);
    object.setSelectedDate(selectedDate);
    QCOMPARE(maxDate, object.selectedDate());
    QVERIFY(selectedDate != object.selectedDate());
    //show today
    QDate today = QDate::currentDate();
    object.showToday();
    QCOMPARE(today.month(), object.monthShown());
    QCOMPARE(today.year(), object.yearShown());
    //slect a different date and move.
    object.setSelectedDate(minDate);
    object.showSelectedDate();
    QCOMPARE(minDate.month(), object.monthShown());
    QCOMPARE(minDate.year(), object.yearShown());
}

void tst_QCalendarWidget::buttonClickCheck()
{
    QCalendarWidget object;
    QSize size = object.sizeHint();
    object.setGeometry(0,0,size.width(), size.height());
    object.show();

    QRect rect = object.geometry();
    QDate selectedDate(2005, 1, 1);
    //click on the month buttons
    int month = object.monthShown();
    QToolButton *button = qFindChild<QToolButton *>(&object, "qt_calendar_prevmonth");
    QTest::mouseClick(button, Qt::LeftButton);
	QCOMPARE(month > 1 ? month-1 : 12, object.monthShown());
    button = qFindChild<QToolButton *>(&object, "qt_calendar_nextmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(month, object.monthShown());

    button = qFindChild<QToolButton *>(&object, "qt_calendar_yearbutton");
    QTest::mouseClick(button, Qt::LeftButton);
    QVERIFY(!button->isVisible());
    QSpinBox *spinbox = qFindChild<QSpinBox *>(&object, "qt_calendar_yearedit");
    QTest::qWait(500);
    QTest::keyClick(spinbox, '2');
    QTest::keyClick(spinbox, '0');
    QTest::keyClick(spinbox, '0');
    QTest::keyClick(spinbox, '6');
    QTest::qWait(500);
    QWidget *widget = qFindChild<QWidget *>(&object, "qt_calendar_calendarview");
    QTest::mouseMove(widget);
    QTest::mouseClick(widget, Qt::LeftButton);
    QCOMPARE(2006, object.yearShown());
    object.setSelectedDate(selectedDate);
    object.showSelectedDate();
    QTest::keyClick(widget, Qt::Key_Down);
    QVERIFY(selectedDate != object.selectedDate());

    object.setDateRange(QDate(2006,1,1), QDate(2006,2,28));
    object.setSelectedDate(QDate(2006,1,1));
    object.showSelectedDate();
    button = qFindChild<QToolButton *>(&object, "qt_calendar_prevmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(1, object.monthShown());

    button = qFindChild<QToolButton *>(&object, "qt_calendar_nextmonth");
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(2, object.monthShown());
    QTest::mouseClick(button, Qt::LeftButton);
    QCOMPARE(2, object.monthShown());

}

void tst_QCalendarWidget::setTextFormat()
{
    QCalendarWidget calendar;
    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    const QDate date(1984, 10, 20);
    calendar.setDateTextFormat(date, format);
    QCOMPARE(calendar.dateTextFormat(date), format);
}

void tst_QCalendarWidget::resetTextFormat()
{
    QCalendarWidget calendar;
    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    const QDate date(1984, 10, 20);
    calendar.setDateTextFormat(date, format);

    calendar.setDateTextFormat(QDate(), QTextCharFormat());
    QCOMPARE(calendar.dateTextFormat(date), QTextCharFormat());
}

void tst_QCalendarWidget::setWeekdayFormat()
{
    QCalendarWidget calendar;

    QTextCharFormat format;
    format.setFontItalic(true);
    format.setForeground(Qt::green);

    calendar.setWeekdayTextFormat(Qt::Wednesday, format);

    // check the format of the a given month
    for (int i = 1; i <= 31; ++i) {
        const QDate date(1984, 10, i);
        const Qt::DayOfWeek dayOfWeek = static_cast<Qt::DayOfWeek>(date.dayOfWeek());
        if (dayOfWeek == Qt::Wednesday)
            QCOMPARE(calendar.weekdayTextFormat(dayOfWeek), format);
        else
            QVERIFY(calendar.weekdayTextFormat(dayOfWeek) != format);
    }
}

tst_QCalendarWidget::tst_QCalendarWidget()
{
}

tst_QCalendarWidget::~tst_QCalendarWidget()
{
}

void tst_QCalendarWidget::initTestCase()
{
}

void tst_QCalendarWidget::cleanupTestCase()
{
}

void tst_QCalendarWidget::init()
{
}

void tst_QCalendarWidget::cleanup()
{
}

QTEST_MAIN(tst_QCalendarWidget)
#include "tst_qcalendarwidget.moc"
