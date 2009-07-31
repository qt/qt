/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlnumberformatter.h"
#include "private/qobject_p.h"

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
    NumberFormatter { id: Formatter; number: 1234.5678; format: "##,##0.##" }
    Text { text: Formatter.text }
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
    <br>
    If no format is specified the text will be empty.
*/

QString QmlNumberFormatter::text() const
{
    Q_D(const QmlNumberFormatter);
    return d->text;
}

/*!
    \qmlproperty qreal NumberFormatter::number
   
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
    <br>
    The format syntax follows a style similar to the Unicode Standard (UTS35).

    The table below shows the characters, patterns that can be used in the format.

    <table border="0" align="center">
    <tr style="background-color: #D6E2E8"><th> Character </th><th> Meaning </th></tr>
    <tr><td> # </td><td> Any digit(s), zero shows as absent (for leading/trailing zeroes) </td></tr>
    <tr><td> 0 </td><td> Implicit digit. Zero will show in the case that the input number is too small.</td></tr>
    <tr><td> . </td><td> Decimal separator. Output decimal seperator will be dependant on system locale.</td></tr>
    <tr><td> , </td><td> Grouping separator. The number of digits (either #, or 0) between the grouping separator and the decimal (or the rightmost digit) will determine the groupingSize)</td></tr>
    <tr><td> other </td><td> Any other character will be taken as a string literal and placed directly into the output string </td></tr>
    </table>
    
    Invalid formats will not guarantee a meaningful text output.<br>
    
    \note <i>Input numbers that are too long for the given format will be rounded dependent on precison based on the position of the decimal point </i>
    
    The following table illustrates the output text created by applying some examples of numeric formats to the formatter.

    <table border="0" align="center">
    <tr style="background-color: #D6E2E8"><th> Format </th><th> Number </th><th> Output </th></tr>
    <tr><td> ### </td><td> 123456 </td><td> 123456 </td></tr>
    <tr><td> 000 </td><td> 123456 </td><td> 123456 </td></tr>
    <tr><td> ###### </td><td> 1234 </td><td> 1234 </td></tr>
    <tr><td> 000000 </td><td> 1234 </td><td> 001234 </td></tr>
    <tr><td> ##,##0.## </td><td> 1234.456 </td><td> 1,234.46 (for US locale)<br> 1 234,46 (for FR locale)</td></tr>
    <tr><td> 000000,000.# </td><td> 123456 </td><td> 000,123,456 (for US locale)<br> 000 123 456 (for FR locale)</td></tr>
    <tr><td> 0.0### </td><td> 0.999997 </td><td> 1.0 </td></tr> 
    <tr><td> (000) 000 - 000 </td><td> 12345678 </td><td> (012) 345 - 678 </td></tr>
    <tr><td> #A</td><td>12</td><td>12A</td></tr>
    </table>

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
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,NumberFormatter,QmlNumberFormatter);

QT_END_NAMESPACE
