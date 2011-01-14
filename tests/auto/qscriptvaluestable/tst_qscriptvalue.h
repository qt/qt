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

#ifndef tst_qscriptvalue_h
#define tst_qscriptvalue_h

#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qnumeric.h>
#include <QtTest/qtest.h>

#define DEFINE_TEST_VALUE(expr) m_values.insert(QString::fromLatin1(#expr), expr)

Q_DECLARE_METATYPE(QScriptValue*);
Q_DECLARE_METATYPE(QScriptValue);
typedef QPair<QString, QScriptValue> QPairQStringAndQScriptValue;
Q_DECLARE_METATYPE(QPairQStringAndQScriptValue);

class tst_QScriptValue : public QObject {
    Q_OBJECT

public:
    tst_QScriptValue();
    virtual ~tst_QScriptValue();

private slots:
    void toStringSimple_data();
    void toStringSimple();
    void dataSharing();
    void constructors_data();
    void constructors();
    void getSetPrototype();
    void call();
    void callBoundArgs();
    void callWithThisObject();
    void construct();
    void ctor();
    void toObjectSimple();
    void getPropertySimple_data();
    void getPropertySimple();
    void setPropertySimple();
    void setProperty_data();
    void setProperty();
    void getSetProperty();
    void getPropertyResolveFlag();
    void propertyFlag_data();
    void propertyFlag();
    void globalObjectChanges();
    void assignAndCopyConstruct_data();
    void assignAndCopyConstruct();

    // Generated test functions.
    void isArray_data();
    void isArray();

    void isBool_data();
    void isBool();

    void isBoolean_data();
    void isBoolean();

    void isError_data();
    void isError();

    void isNumber_data();
    void isNumber();

    void isFunction_data();
    void isFunction();

    void isNull_data();
    void isNull();

    void isObject_data();
    void isObject();

    void isString_data();
    void isString();

    void isUndefined_data();
    void isUndefined();

    void isValid_data();
    void isValid();

    void toString_data();
    void toString();

    void toNumber_data();
    void toNumber();

    void toBool_data();
    void toBool();

    void toBoolean_data();
    void toBoolean();

    void toInteger_data();
    void toInteger();

    void toInt32_data();
    void toInt32();

    void toUInt32_data();
    void toUInt32();

    void toUInt16_data();
    void toUInt16();

    void equals_data();
    void equals();

    void strictlyEquals_data();
    void strictlyEquals();

    void instanceOf_data();
    void instanceOf();

private:
    /*!
        \internal
        A helper function for getPropertySimple and toString that removes \n and spaces from a string
    */
    QString removeWhiteSpace(QString str)
    {
        static QRegExp cleaner("[\n ]+");
        return str.remove(cleaner);
    }

    QPair<QString, QScriptValue> initScriptValues(uint idx);
    QScriptEngine* m_engine;
};

#endif // tst_qscriptvalue_h
