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

#ifndef NUMBERFORMAT_H
#define NUMBERFORMAT_H 

#include <qdeclarative.h>

#include <QtCore/QLocale>
#include <QtCore/QTime>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

// TODO 
// be able to set Locale, instead of default system for dynamic formatting
// add currency support
// add additional syntax, extend to format scientific, percentiles, significant digits etc


class QNumberFormat : public QObject
{
        Q_OBJECT
        Q_ENUMS(NumberType)
public:
        QNumberFormat(QObject *parent=0);
        ~QNumberFormat();

        enum NumberType {
            Percent,
            Scientific,
            Currency,
            Decimal
        };

        //external property, only visible
        Q_PROPERTY(QString text READ text NOTIFY textChanged)

        //mutatable properties to modify the output (text)
        Q_PROPERTY(qreal number READ number WRITE setNumber) 
        Q_PROPERTY(QString format READ format WRITE setFormat)
        Q_PROPERTY(QLocale locale READ locale WRITE setLocale)

        //Format specific settings
        Q_PROPERTY(unsigned short groupingSeparator READ groupingSeparator WRITE setGroupingSeparator)
        Q_PROPERTY(unsigned short decimalSeperator READ decimalSeparator WRITE setDecimalSeparator)
        Q_PROPERTY(unsigned int groupingSize READ groupingSize WRITE setGroupingSize)
        Q_PROPERTY(unsigned short currencySymbol READ currencySymbol WRITE setCurrencySymbol)


        QString text() const { return _text; }

        qreal number() const { return _number; }
        void setNumber(qreal n) {
            if (_number == n)
                return;
            _number = n;
            updateText(); 
        }

        QString format() const { return _format; }
        void setFormat(const QString &format) {
            if (format.isEmpty())
                _format = QString::null;
            else if (_format == format)
                return;

            _format = format;
            updateText(); 
        }

        QLocale locale() const { return _locale; }
        void setLocale(const QLocale &locale) { _locale = locale; updateText(); }

        //Do we deal with unicode standard? or create our own
        // ### since this is the backend for the number conversions, we will use the unicode
        // the front-end will handle the QChar/QString -> short int 

        unsigned short groupingSeparator() { return _groupingSeparator.unicode(); }
        void setGroupingSeparator(unsigned short unicodeSymbol) 
        {
            _groupingSeparator = QChar(unicodeSymbol);
        }

        unsigned short decimalSeparator() { return _decimalSeparator.unicode(); }
        void setDecimalSeparator(unsigned short unicodeSymbol)
        {
            _decimalSeparator = QChar(unicodeSymbol);
        }

        unsigned short currencySymbol() { return _currencySymbol.unicode(); }
        void setCurrencySymbol(unsigned short unicodeSymbol)
        {
            _currencySymbol = QChar(unicodeSymbol);
        }

        unsigned int groupingSize() { return _groupingSize; }
        void setGroupingSize(unsigned int size)
        {
            _groupingSize = size;
        }

Q_SIGNALS:
        void textChanged();

private:
        void updateText();
        void handleFormat();
        QString formatInteger(const QString &formatInt, const QString &integer);
        QString formatDecimal(const QString &formatDec, const QString &decimal);

        qreal _number;
        NumberType _type;
        QChar _groupingSeparator;
        QChar _decimalSeparator;
        QChar _currencySymbol;
        unsigned int _groupingSize;

        QLocale _locale;
        QString _format;

        // only hooked member at the moment
        QString _text;

};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QNumberFormat)

QT_END_HEADER

#endif
