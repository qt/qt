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

#include "qmlnumberformatter_p.h"

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

//TODO: set locale
//      docs
//      this is a wrapper around qnumberformat (test integration)
//      if number or format haven't been explictly set, text should be an empty string

class QmlNumberFormatterPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlNumberFormatter)
public:
    QmlNumberFormatterPrivate() : locale(QLocale::system()), number(0), componentComplete(true) {}

    void updateText();

    QLocale locale;
    QString format;
    QNumberFormat numberFormat;
    QString text;
    qreal number;
    bool componentComplete;
};
/*!
    \qmlclass NumberFormatter
    \brief The NumberFormatter allows you to control the format of a number string.

    The format property documentation has more details on how the format can be manipulated.

    In the following example, the text element will display the text "1,234.57".
    \code
    NumberFormatter { id: formatter; number: 1234.5678; format: "##,##0.##" }
    Text { text: formatter.text }
    \endcode

    */
/*!
    \internal
    \class QmlNumberFormatter
    \ingroup group_utility
    \brief The QmlNumberFormatter class allows you to format a number to a particular string format/locale specific number format.
*/

QmlNumberFormatter::QmlNumberFormatter(QObject *parent)
: QObject(*(new QmlNumberFormatterPrivate), parent) 
{
}

QmlNumberFormatter::~QmlNumberFormatter()
{
}

/*!
    \qmlproperty string NumberFormatter::text

    The number in the specified format.

    If no format is specified the text will be empty.
*/

QString QmlNumberFormatter::text() const
{
    Q_D(const QmlNumberFormatter);
    return d->text;
}

/*!
    \qmlproperty real NumberFormatter::number
   
    A single point precision number. (Doubles are not yet supported)

*/
qreal QmlNumberFormatter::number() const
{
    Q_D(const QmlNumberFormatter);
    return d->number;
}

/*!
    \qmlproperty string NumberFormatter::format

    The particular format the number will adhere to during the conversion to text.

    The format syntax follows a style similar to the Unicode Standard (UTS35).

    The table below shows the characters, patterns that can be used in the format.

    \table
    \header
        \o Character
        \o Meaning
    \row
        \o #
        \o Any digit(s), zero shows as absent (for leading/trailing zeroes).
    \row
        \o 0
        \o Implicit digit. Zero will show in the case that the input number is too small.
    \row
        \o .
        \o Decimal separator. Output decimal seperator will be dependant on system locale.
    \row
        \o ,
        \o Grouping separator. The number of digits (either #, or 0) between the grouping separator and the decimal (or the rightmost digit) will determine the groupingSize).
    \row
        \o other
        \o Any other character will be taken as a string literal and placed directly into the output string.
    \endtable
    
    Invalid formats will not guarantee a meaningful text output.
    
    \note Input numbers that are too long for the given format will be rounded dependent on precison based on the position of the decimal point.
    
    The following table illustrates the output text created by applying some examples of numeric formats to the formatter.

    \table
    \header
        \o Format
        \o Number
        \o Output
    \row
        \o ###
        \o 123456
        \o  123456
    \row
        \o  000
        \o  123456
        \o  123456
    \row
        \o  ######
        \o  1234
        \o  1234
    \row
        \o  000000
        \o  1234
        \o  001234
    \row
        \o  ##,##0.##
        \o  1234.456
        \o  1,234.46 (for US locale)
        \codeline 1 234,46 (for FR locale)
    \row
        \o  000000,000.#
        \o  123456
        \o  000,123,456 (for US locale)
        \codeline 000 123 456 (for FR locale)
    \row
        \o  0.0###
        \o  0.999997
        \o  1.0
    \row
        \o  (000) 000 - 000
        \o  12345678
        \o  (012) 345 - 678
    \row
        \o  #A
        \o 12
        \o 12A
    \endtable

*/
QString QmlNumberFormatter::format() const
{
    Q_D(const QmlNumberFormatter);
    return d->format;
}

void QmlNumberFormatter::setNumber(const qreal &number)
{
    Q_D(QmlNumberFormatter);
    if (d->number == number)
        return;
    d->number = number;
    d->updateText();
}

void QmlNumberFormatter::setFormat(const QString &format)
{
    Q_D(QmlNumberFormatter);
    //no format checking
    if (format.isEmpty()) 
        d->format = QString::null; 
    else
        d->format = format;
    d->updateText();
}

void QmlNumberFormatterPrivate::updateText()
{
    Q_Q(QmlNumberFormatter);
    if (!componentComplete)
        return;

    QNumberFormat tempFormat;
    tempFormat.setFormat(format);
    tempFormat.setNumber(number);

    text = tempFormat.text();
    
    emit q->textChanged();
}

void QmlNumberFormatter::classBegin()
{
    Q_D(QmlNumberFormatter);
    d->componentComplete = false;
}

void QmlNumberFormatter::componentComplete()
{
    Q_D(QmlNumberFormatter);
    d->componentComplete = true;
    d->updateText();
}


QT_END_NAMESPACE
