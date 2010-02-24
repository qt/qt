/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmldatetimeformatter_p.h"

#include <QtCore/qlocale.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

//TODO: may need optimisation as the QDateTime member may not be needed?
//      be able to set a locale?

class QmlDateTimeFormatterPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDateTimeFormatter)
public:
    QmlDateTimeFormatterPrivate() : locale(QLocale::system()), longStyle(false), componentComplete(true) {}

    void updateText();

    QDateTime dateTime;
    QDate date;
    QTime time;
    QLocale locale;
    QString dateTimeText;
    QString dateText;
    QString timeText;
    QString dateTimeFormat; //set for convienience?
    QString dateFormat;
    QString timeFormat;
    bool longStyle;
    bool componentComplete;
};

/*!
    \qmlclass DateTimeFormatter QmlDateTimeFormatter
  \since 4.7
    \brief The DateTimeFormatter allows you to control the format of a date string.

    \code
    DateTimeFormatter { id: formatter; date: System.date }
    Text { text: formatter.dateText }
    \endcode

    By default, the text properties (dateText, timeText, and dateTimeText) will return the
    date and time using the current system locale's format.
*/

/*!
    \internal
    \class QmlDateTimeFormatter
    \ingroup group_utility
    \brief The QmlDateTimeFormatter class allows you to format a date string.
*/

QmlDateTimeFormatter::QmlDateTimeFormatter(QObject *parent)
: QObject(*(new QmlDateTimeFormatterPrivate), parent)
{
}

QmlDateTimeFormatter::~QmlDateTimeFormatter()
{
}

/*!
    \qmlproperty string DateTimeFormatter::dateText
    \qmlproperty string DateTimeFormatter::timeText
    \qmlproperty string DateTimeFormatter::dateTimeText

    Formatted text representations of the \c date, \c time,
    and \c {date and time}, respectively.

    If there is no explictly specified format the DateTimeFormatter
    will use the system locale's default 'short' setting.

    \code
    // specify source date (assuming today is February 19, 2009)
    DateTimeFormatter { id: formatter; dateTime: Today.date }

    // display the full date and time
    Text { text: formatter.dateText }
    \endcode

    Would be equivalent to the following for a US English locale:

    \code
    // display the date
    Text { text: "2/19/09" }
    \endcode
*/
QString QmlDateTimeFormatter::dateTimeText() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->dateTimeText;
}

QString QmlDateTimeFormatter::dateText() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->dateText;
}

QString QmlDateTimeFormatter::timeText() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->timeText;
}

/*!
    \qmlproperty date DateTimeFormatter::date
    \qmlproperty time DateTimeFormatter::time
    \qmlproperty datetime DateTimeFormatter::dateTime

    The source date and time to be used by the formatter.

    \code
    // setting the date and time
    DateTimeFormatter { date: System.date; time: System.time }
    \endcode

    For convienience it is possible to set the datetime property to set both the date and the time.
    \code
    // setting the datetime
    DateTimeFormatter { dateTime: System.dateTime }
    \endcode

    There can only be one instance of date and time per formatter; if date, time, and dateTime are all
    set the actual date and time used is not guaranteed.

    \note If no date is set, dateTimeText will be just the date;
    If no time is set, the dateTimeText will be just the time.
    
*/
QDate QmlDateTimeFormatter::date() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->date;
}

QTime QmlDateTimeFormatter::time() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->time;
}

QDateTime QmlDateTimeFormatter::dateTime() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->dateTime;
}

/*!
    \qmlproperty string DateTimeFormatter::dateFormat
    \qmlproperty string DateTimeFormatter::timeFormat
    \qmlproperty string DateTimeFormatter::dateTimeFormat

    Specifies a custom format which the DateTime Formatter can use.

    If there is no explictly specified format the DateTimeFormatter
    will use the system locale's default 'short' setting.

    The text's format may be modified by setting:
    \list
    \i \c dateFormat
    \i \c timeFormat
    \i \c dateTimeFormat
    \endlist

    If only the format for date is defined, the time and dateTime formats will be defined
    as the system locale default and likewise for the others.

    Syntax for the format is based on the QDateTime::toString() formatting options.

    \code
    // Format the date such that the dateText is: '1997-12-12'
    DateTimeFormatter { id: formatter; dateTime: Today.dateTime; formatDate: "yyyy-MM-d" }
    \endcode

    Assigning an empty string to a particular format will reset it.
*/
QString QmlDateTimeFormatter::dateTimeFormat() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->dateTimeFormat;
}

QString QmlDateTimeFormatter::dateFormat() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->dateFormat;
}

QString QmlDateTimeFormatter::timeFormat() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->timeFormat;
}

/*!
    \qmlproperty bool DateTimeFormatter::longStyle

    This property causes the formatter to use the system locale's long format rather than short format
    by default.

    This setting is off by default.
*/
bool QmlDateTimeFormatter::longStyle() const
{
    Q_D(const QmlDateTimeFormatter);
    return d->longStyle;
}

void QmlDateTimeFormatter::setDateTime(const QDateTime &dateTime)
{
    Q_D(QmlDateTimeFormatter);
    if (d->dateTime == dateTime)
        return;
    d->dateTime = dateTime;
    d->date = d->dateTime.date();
    d->time = d->dateTime.time();
    d->updateText();
}

void QmlDateTimeFormatter::setTime(const QTime &time)
{
    Q_D(QmlDateTimeFormatter);
    if (d->dateTime.time() == time)
        return;
    d->time = time;
    d->dateTime.setTime(time);
    d->updateText();
}

void QmlDateTimeFormatter::setDate(const QDate &date)
{
    Q_D(QmlDateTimeFormatter);
    if (d->dateTime.date() == date)
        return;
    d->date = date;
    bool clearTime = d->dateTime.time().isValid() ? false : true;   //because setting date generates default time
    d->dateTime.setDate(date);
    if (clearTime)
        d->dateTime.setTime(QTime());
    d->updateText();
}

//DateTime formatting may be a combination of date and time?
void QmlDateTimeFormatter::setDateTimeFormat(const QString &format)
{
    Q_D(QmlDateTimeFormatter);
    //no format checking
    d->dateTimeFormat = format;
    d->updateText();
}

void QmlDateTimeFormatter::setDateFormat(const QString &format)
{
    Q_D(QmlDateTimeFormatter);
    //no format checking
    d->dateFormat = format;
    d->updateText();
}

void QmlDateTimeFormatter::setTimeFormat(const QString &format)
{
    Q_D(QmlDateTimeFormatter);
    //no format checking
    d->timeFormat = format;
    d->updateText();
}

void QmlDateTimeFormatter::setLongStyle(bool longStyle)
{
    Q_D(QmlDateTimeFormatter);
    d->longStyle = longStyle;
    d->updateText();
}

void QmlDateTimeFormatterPrivate::updateText()
{
    Q_Q(QmlDateTimeFormatter);
    if (!componentComplete)
        return;

    QString str;
    QString str1;
    QString str2;

    Qt::DateFormat defaultFormat = longStyle ? Qt::SystemLocaleLongDate : Qt::SystemLocaleShortDate;

    if (dateFormat.isEmpty())
        str1 = date.toString(defaultFormat);
    else
        str1 = date.toString(dateFormat);

    if (timeFormat.isEmpty())
        str2 = time.toString(defaultFormat);
    else
        str2 = time.toString(timeFormat);

    if (dateTimeFormat.isEmpty())
        str = dateTime.toString(defaultFormat);
    //else if (!formatTime.isEmpty() && !formatDate.isEmpty())
    //    str = str1 + QLatin1Char(' ') + str2;
    else
        str = dateTime.toString(dateTimeFormat);

    if (dateTimeText == str && dateText == str1 && timeText == str2)
        return;

    dateTimeText = str;
    dateText = str1;
    timeText = str2;

    emit q->textChanged();
}

void QmlDateTimeFormatter::classBegin()
{
    Q_D(QmlDateTimeFormatter);
    d->componentComplete = false;
}

void QmlDateTimeFormatter::componentComplete()
{
    Q_D(QmlDateTimeFormatter);
    d->componentComplete = true;
    d->updateText();
}



QT_END_NAMESPACE
