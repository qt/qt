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

#include "qnumberformat.h"


QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QNumberFormat,NumberFormat)

QNumberFormat::QNumberFormat(QObject *parent) : QObject(parent), _number(0), _type(Decimal),
                                 _groupingSize(0)
{
    _locale = QLocale::system();
    _groupingSeparator = _locale.groupSeparator();
    _decimalSeparator = _locale.decimalPoint();
    _currencySymbol = QLatin1Char('$');
}

QNumberFormat::~QNumberFormat()
{

}

void QNumberFormat::updateText()
{
    QTime t;
    t.start();
    static int totalTime;

    handleFormat();

    totalTime += t.elapsed();
    emit textChanged();
}

void QNumberFormat::handleFormat()
{
    // ### is extremely messy 
    if (_format.isEmpty()) {
        _text = QString(QLatin1String("%1")).arg(_number, -1, 'f', -1);
        return;
    }

    QString inputString;

    // ### possible to use the following parsed data in the future

    int remainingLength = _format.size();
    int currentIndex = _format.size()-1;

    int maxDigits = 0;
    int minDigits = 0;
    int decimalLength = 0;

    while (remainingLength > 0) {
       switch(_format.at(currentIndex).unicode()) {
            case ',':
                if (decimalLength && !_groupingSize)
                    setGroupingSize(maxDigits - decimalLength);
                else if (!_groupingSize)
                    setGroupingSize(maxDigits);
                break;
            case '.':
                if (!decimalLength)
                    decimalLength = maxDigits;
                break;
            case '0':
                minDigits++;
            case '#':
                maxDigits++;
                break;
            default:
                break;
       }
       currentIndex--;
       remainingLength--;
    }

    // round given the decimal length/precision
    inputString = QString(QLatin1String("%1")).arg(_number, -1, 'f', decimalLength);

    QStringList parts = inputString.split(QLatin1Char('.'));
    QStringList formatParts = _format.split(QLatin1Char('.'));

    if (formatParts.size() > 2 || parts.size() > 2 )
        return;

    QString formatInt = formatParts.at(0);

    QString formatDec;
    if (formatParts.size() == 2)
        formatDec = formatParts.at(1);

    QString integer = parts.at(0);

    QString decimal;
    if (parts.size() == 2)
        decimal = parts.at(1);

    QString outputDecimal = formatDecimal(formatDec, decimal);
    QString outputInteger = formatInteger(formatInt, integer);

    // insert separators
    if (_groupingSize) {
        unsigned int count = 0;
        for (int i = outputInteger.size()-1; i > 0; i--) {
            if (outputInteger.at(i).digitValue() >= 0) {
                if (count == _groupingSize - 1) {
                    count = 0;
                    outputInteger.insert(i, _groupingSeparator);
                }
                else
                    count++;
            }
        }
    }
    if (!outputDecimal.isEmpty())
        _text = outputInteger + _decimalSeparator + outputDecimal;
    else
        _text = outputInteger;
}

QString QNumberFormat::formatInteger(const QString &formatInt, const QString &integer)
{
    if (formatInt.isEmpty() || integer.isEmpty())
        return QString();

    QString outputInteger;
    int formatIndex = formatInt.size()-1;

    //easier for carry?
    for (int index= integer.size()-1; index >= 0; index--) {
        if (formatIndex < 0) {
            outputInteger.push_front(integer.at(index));
        }
        else {
            switch(formatInt.at(formatIndex).unicode()) {
                case '0':
                    if (index > integer.size()-1) {
                        outputInteger.push_front(QLatin1Char('0'));
                        break;
                    }
                case '#':
                    outputInteger.push_front(integer.at(index));
                    break;
                case ',':
                    index++;
                    break;
                default:
                    outputInteger.push_front(formatInt.at(formatIndex));
                    index++;
                    break;
            }
            formatIndex--;
        }
    }
    while (formatIndex >= 0) {
        if (formatInt.at(formatIndex).unicode() != '#' && formatInt.at(formatIndex).unicode() != ',')
            outputInteger.push_front(formatInt.at(formatIndex));
        formatIndex--;
    }
    return outputInteger;
}

QString QNumberFormat::formatDecimal(const QString &formatDec, const QString &decimal)
{
    QString outputDecimal;

    // up to max 6 decimal places 
    for (int index=formatDec.size()-1; index >= 0; index--) {
        switch(formatDec.at(index).unicode()) {
            case '0':
                outputDecimal.push_front(decimal.at(index));
                break;
            case '#':
                if (decimal.at(index) != QLatin1Char('0') || outputDecimal.size() > 0)
                    outputDecimal.push_front(decimal.at(index));
                break;
            default:
                outputDecimal.push_front(formatDec.at(index));
                break;
        }
    }
    return outputDecimal;
}
QT_END_NAMESPACE
