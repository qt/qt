/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTCONVERTER_P_H
#define QSCRIPTCONVERTER_P_H

#include "qscriptvalue.h"
#include <QtCore/qglobal.h>
#include <QtCore/qnumeric.h>
#include <QtCore/qstring.h>
#include <QtCore/qvarlengtharray.h>

#include <v8.h>

QT_BEGIN_NAMESPACE

extern char *qdtoa(double d, int mode, int ndigits, int *decpt, int *sign, char **rve, char **digits_str);

/*
  \internal
  \class QScriptConverter
  QScriptValue and QScriptEngine helper class. This class's responsibility is to convert values
  between JS values and Qt/C++ values.

  This is a nice way to inline these functions in both QScriptValue and QScriptEngine.
*/
class QScriptConverter {
public:
    static quint32 toArrayIndex(const QString& string)
    {
        // FIXME this function should be exported by JSC C API.
        bool ok;
        quint32 idx = string.toUInt(&ok);
        if (!ok || toString(idx) != string)
            idx = 0xffffffff;

        return idx;
    }

    static QString toString(v8::Handle<v8::String> jsString)
    {
        if (jsString.IsEmpty())
            return QString();
        QString qstr;
        qstr.resize(jsString->Length());
        jsString->Write(reinterpret_cast<uint16_t*>(qstr.data()));
        return qstr;
    }

    static v8::Handle<v8::String> toString(const QString& string)
    {
        return v8::String::New(reinterpret_cast<const uint16_t*>(string.data()), string.size());
    }

    static QString toString(double value)
    {
        // FIXME this should be easier. The ideal fix is to create
        // a new function in V8 API which could cover the functionality.

        if (qIsNaN(value))
            return QString::fromLatin1("NaN");
        if (qIsInf(value))
            return QString::fromLatin1(value < 0 ? "-Infinity" : "Infinity");
        if (!value)
            return QString::fromLatin1("0");

        QVarLengthArray<char, 25> buf;
        int decpt;
        int sign;
        char* result = 0;
        char* endresult;
        (void)qdtoa(value, 0, 0, &decpt, &sign, &endresult, &result);

        if (!result)
            return QString();

        int resultLen = endresult - result;
        if (decpt <= 0 && decpt > -6) {
            buf.resize(-decpt + 2 + sign);
            qMemSet(buf.data(), '0', -decpt + 2 + sign);
            if (sign) // fix the sign.
                buf[0] = '-';
            buf[sign + 1] = '.';
            buf.append(result, resultLen);
        } else {
            if (sign)
                buf.append('-');
            int length = buf.size() - sign + resultLen;
            if (decpt <= 21 && decpt > 0) {
                if (length <= decpt) {
                    const char* zeros = "0000000000000000000000000";
                    buf.append(result, resultLen);
                    buf.append(zeros, decpt - length);
                } else {
                    buf.append(result, decpt);
                    buf.append('.');
                    buf.append(result + decpt, resultLen - decpt);
                }
            } else if (result[0] >= '0' && result[0] <= '9') {
                if (length > 1) {
                    buf.append(result, 1);
                    buf.append('.');
                    buf.append(result + 1, resultLen - 1);
                } else
                    buf.append(result, resultLen);
                buf.append('e');
                buf.append(decpt >= 0 ? '+' : '-');
                int e = qAbs(decpt - 1);
                if (e >= 100)
                    buf.append('0' + e / 100);
                if (e >= 10)
                    buf.append('0' + (e % 100) / 10);
                buf.append('0' + e % 10);
            }
        }
        free(result);
        buf.append(0);
        return QString::fromLatin1(buf.constData());
    }

    /*static JSPropertyAttributes toPropertyFlags(const QFlags<QScriptValue::PropertyFlag>& flags)
    {
        JSPropertyAttributes attr = 0;
        if (flags.testFlag(QScriptValue::ReadOnly))
            attr |= kJSPropertyAttributeReadOnly;
        if (flags.testFlag(QScriptValue::Undeletable))
            attr |= kJSPropertyAttributeDontDelete;
        if (flags.testFlag(QScriptValue::SkipInEnumeration))
            attr |= kJSPropertyAttributeDontEnum;
        return attr;
    }*/
};

QT_END_NAMESPACE

#endif
