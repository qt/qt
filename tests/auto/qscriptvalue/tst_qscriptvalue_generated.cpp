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

#include "tst_qscriptvalue.h"

#define DEFINE_TEST_VALUE(expr) m_values.insert(QString::fromLatin1(#expr), expr)

void tst_QScriptValue::initScriptValues()
{
    m_values.clear();
    if (engine) 
        delete engine;
    engine = new QScriptEngine;
    DEFINE_TEST_VALUE(QScriptValue());
    DEFINE_TEST_VALUE(QScriptValue(QScriptValue::UndefinedValue));
    DEFINE_TEST_VALUE(QScriptValue(QScriptValue::NullValue));
    DEFINE_TEST_VALUE(QScriptValue(true));
    DEFINE_TEST_VALUE(QScriptValue(false));
    DEFINE_TEST_VALUE(QScriptValue(int(122)));
    DEFINE_TEST_VALUE(QScriptValue(uint(124)));
    DEFINE_TEST_VALUE(QScriptValue(0));
    DEFINE_TEST_VALUE(QScriptValue(0.0));
    DEFINE_TEST_VALUE(QScriptValue(123.0));
    DEFINE_TEST_VALUE(QScriptValue(6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(-6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(0x43211234));
    DEFINE_TEST_VALUE(QScriptValue(0x10000));
    DEFINE_TEST_VALUE(QScriptValue(0x10001));
    DEFINE_TEST_VALUE(QScriptValue(qSNaN()));
    DEFINE_TEST_VALUE(QScriptValue(qQNaN()));
    DEFINE_TEST_VALUE(QScriptValue(qInf()));
    DEFINE_TEST_VALUE(QScriptValue(-qInf()));
    DEFINE_TEST_VALUE(QScriptValue("NaN"));
    DEFINE_TEST_VALUE(QScriptValue("Infinity"));
    DEFINE_TEST_VALUE(QScriptValue("-Infinity"));
    DEFINE_TEST_VALUE(QScriptValue("ciao"));
    DEFINE_TEST_VALUE(QScriptValue(QString::fromLatin1("ciao")));
    DEFINE_TEST_VALUE(QScriptValue(QString("")));
    DEFINE_TEST_VALUE(QScriptValue(QString()));
    DEFINE_TEST_VALUE(QScriptValue(QString("0")));
    DEFINE_TEST_VALUE(QScriptValue(QString("123")));
    DEFINE_TEST_VALUE(QScriptValue(QString("12.4")));
    DEFINE_TEST_VALUE(QScriptValue(0, QScriptValue::UndefinedValue));
    DEFINE_TEST_VALUE(QScriptValue(0, QScriptValue::NullValue));
    DEFINE_TEST_VALUE(QScriptValue(0, true));
    DEFINE_TEST_VALUE(QScriptValue(0, false));
    DEFINE_TEST_VALUE(QScriptValue(0, int(122)));
    DEFINE_TEST_VALUE(QScriptValue(0, uint(124)));
    DEFINE_TEST_VALUE(QScriptValue(0, 0));
    DEFINE_TEST_VALUE(QScriptValue(0, 0.0));
    DEFINE_TEST_VALUE(QScriptValue(0, 123.0));
    DEFINE_TEST_VALUE(QScriptValue(0, 6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(0, -6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(0, 0x43211234));
    DEFINE_TEST_VALUE(QScriptValue(0, 0x10000));
    DEFINE_TEST_VALUE(QScriptValue(0, 0x10001));
    DEFINE_TEST_VALUE(QScriptValue(0, qSNaN()));
    DEFINE_TEST_VALUE(QScriptValue(0, qQNaN()));
    DEFINE_TEST_VALUE(QScriptValue(0, qInf()));
    DEFINE_TEST_VALUE(QScriptValue(0, -qInf()));
    DEFINE_TEST_VALUE(QScriptValue(0, "NaN"));
    DEFINE_TEST_VALUE(QScriptValue(0, "Infinity"));
    DEFINE_TEST_VALUE(QScriptValue(0, "-Infinity"));
    DEFINE_TEST_VALUE(QScriptValue(0, "ciao"));
    DEFINE_TEST_VALUE(QScriptValue(0, QString::fromLatin1("ciao")));
    DEFINE_TEST_VALUE(QScriptValue(0, QString("")));
    DEFINE_TEST_VALUE(QScriptValue(0, QString()));
    DEFINE_TEST_VALUE(QScriptValue(0, QString("0")));
    DEFINE_TEST_VALUE(QScriptValue(0, QString("123")));
    DEFINE_TEST_VALUE(QScriptValue(0, QString("12.3")));
    DEFINE_TEST_VALUE(QScriptValue(engine, QScriptValue::UndefinedValue));
    DEFINE_TEST_VALUE(QScriptValue(engine, QScriptValue::NullValue));
    DEFINE_TEST_VALUE(QScriptValue(engine, true));
    DEFINE_TEST_VALUE(QScriptValue(engine, false));
    DEFINE_TEST_VALUE(QScriptValue(engine, int(122)));
    DEFINE_TEST_VALUE(QScriptValue(engine, uint(124)));
    DEFINE_TEST_VALUE(QScriptValue(engine, 0));
    DEFINE_TEST_VALUE(QScriptValue(engine, 0.0));
    DEFINE_TEST_VALUE(QScriptValue(engine, 123.0));
    DEFINE_TEST_VALUE(QScriptValue(engine, 6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(engine, -6.37e-8));
    DEFINE_TEST_VALUE(QScriptValue(engine, 0x43211234));
    DEFINE_TEST_VALUE(QScriptValue(engine, 0x10000));
    DEFINE_TEST_VALUE(QScriptValue(engine, 0x10001));
    DEFINE_TEST_VALUE(QScriptValue(engine, qSNaN()));
    DEFINE_TEST_VALUE(QScriptValue(engine, qQNaN()));
    DEFINE_TEST_VALUE(QScriptValue(engine, qInf()));
    DEFINE_TEST_VALUE(QScriptValue(engine, -qInf()));
    DEFINE_TEST_VALUE(QScriptValue(engine, "NaN"));
    DEFINE_TEST_VALUE(QScriptValue(engine, "Infinity"));
    DEFINE_TEST_VALUE(QScriptValue(engine, "-Infinity"));
    DEFINE_TEST_VALUE(QScriptValue(engine, "ciao"));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString::fromLatin1("ciao")));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString("")));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString()));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString("0")));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString("123")));
    DEFINE_TEST_VALUE(QScriptValue(engine, QString("1.23")));
    DEFINE_TEST_VALUE(engine->evaluate("[]"));
    DEFINE_TEST_VALUE(engine->evaluate("{}"));
    DEFINE_TEST_VALUE(engine->evaluate("Object.prototype"));
    DEFINE_TEST_VALUE(engine->evaluate("Date.prototype"));
    DEFINE_TEST_VALUE(engine->evaluate("Array.prototype"));
    DEFINE_TEST_VALUE(engine->evaluate("Function.prototype"));
    DEFINE_TEST_VALUE(engine->evaluate("Error.prototype"));
    DEFINE_TEST_VALUE(engine->evaluate("Object"));
    DEFINE_TEST_VALUE(engine->evaluate("Array"));
    DEFINE_TEST_VALUE(engine->evaluate("Number"));
    DEFINE_TEST_VALUE(engine->evaluate("Function"));
    DEFINE_TEST_VALUE(engine->evaluate("(function() { return 1; })"));
    DEFINE_TEST_VALUE(engine->evaluate("(function() { return 'ciao'; })"));
    DEFINE_TEST_VALUE(engine->evaluate("(function() { throw new Error('foo'); })"));
    DEFINE_TEST_VALUE(engine->evaluate("/foo/"));
    DEFINE_TEST_VALUE(engine->evaluate("new Object()"));
    DEFINE_TEST_VALUE(engine->evaluate("new Array()"));
    DEFINE_TEST_VALUE(engine->evaluate("new Error()"));
}


void tst_QScriptValue::isValid_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isValid_makeData(const char* expr)
{
    static QSet<QString> isValid;
    if (isValid.isEmpty()) {
        isValid << "QScriptValue(QScriptValue::UndefinedValue)"
                << "QScriptValue(QScriptValue::NullValue)"
                << "QScriptValue(true)"
                << "QScriptValue(false)"
                << "QScriptValue(int(122))"
                << "QScriptValue(uint(124))"
                << "QScriptValue(0)"
                << "QScriptValue(0.0)"
                << "QScriptValue(123.0)"
                << "QScriptValue(6.37e-8)"
                << "QScriptValue(-6.37e-8)"
                << "QScriptValue(0x43211234)"
                << "QScriptValue(0x10000)"
                << "QScriptValue(0x10001)"
                << "QScriptValue(qSNaN())"
                << "QScriptValue(qQNaN())"
                << "QScriptValue(qInf())"
                << "QScriptValue(-qInf())"
                << "QScriptValue(\"NaN\")"
                << "QScriptValue(\"Infinity\")"
                << "QScriptValue(\"-Infinity\")"
                << "QScriptValue(\"ciao\")"
                << "QScriptValue(QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(QString(\"\"))"
                << "QScriptValue(QString())"
                << "QScriptValue(QString(\"0\"))"
                << "QScriptValue(QString(\"123\"))"
                << "QScriptValue(QString(\"12.4\"))"
                << "QScriptValue(0, QScriptValue::UndefinedValue)"
                << "QScriptValue(0, QScriptValue::NullValue)"
                << "QScriptValue(0, true)"
                << "QScriptValue(0, false)"
                << "QScriptValue(0, int(122))"
                << "QScriptValue(0, uint(124))"
                << "QScriptValue(0, 0)"
                << "QScriptValue(0, 0.0)"
                << "QScriptValue(0, 123.0)"
                << "QScriptValue(0, 6.37e-8)"
                << "QScriptValue(0, -6.37e-8)"
                << "QScriptValue(0, 0x43211234)"
                << "QScriptValue(0, 0x10000)"
                << "QScriptValue(0, 0x10001)"
                << "QScriptValue(0, qSNaN())"
                << "QScriptValue(0, qQNaN())"
                << "QScriptValue(0, qInf())"
                << "QScriptValue(0, -qInf())"
                << "QScriptValue(0, \"NaN\")"
                << "QScriptValue(0, \"Infinity\")"
                << "QScriptValue(0, \"-Infinity\")"
                << "QScriptValue(0, \"ciao\")"
                << "QScriptValue(0, QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(0, QString(\"\"))"
                << "QScriptValue(0, QString())"
                << "QScriptValue(0, QString(\"0\"))"
                << "QScriptValue(0, QString(\"123\"))"
                << "QScriptValue(0, QString(\"12.3\"))"
                << "QScriptValue(engine, QScriptValue::UndefinedValue)"
                << "QScriptValue(engine, QScriptValue::NullValue)"
                << "QScriptValue(engine, true)"
                << "QScriptValue(engine, false)"
                << "QScriptValue(engine, int(122))"
                << "QScriptValue(engine, uint(124))"
                << "QScriptValue(engine, 0)"
                << "QScriptValue(engine, 0.0)"
                << "QScriptValue(engine, 123.0)"
                << "QScriptValue(engine, 6.37e-8)"
                << "QScriptValue(engine, -6.37e-8)"
                << "QScriptValue(engine, 0x43211234)"
                << "QScriptValue(engine, 0x10000)"
                << "QScriptValue(engine, 0x10001)"
                << "QScriptValue(engine, qSNaN())"
                << "QScriptValue(engine, qQNaN())"
                << "QScriptValue(engine, qInf())"
                << "QScriptValue(engine, -qInf())"
                << "QScriptValue(engine, \"NaN\")"
                << "QScriptValue(engine, \"Infinity\")"
                << "QScriptValue(engine, \"-Infinity\")"
                << "QScriptValue(engine, \"ciao\")"
                << "QScriptValue(engine, QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(engine, QString(\"\"))"
                << "QScriptValue(engine, QString())"
                << "QScriptValue(engine, QString(\"0\"))"
                << "QScriptValue(engine, QString(\"123\"))"
                << "QScriptValue(engine, QString(\"1.23\"))"
                << "engine->evaluate(\"[]\")"
                << "engine->evaluate(\"{}\")"
                << "engine->evaluate(\"Object.prototype\")"
                << "engine->evaluate(\"Date.prototype\")"
                << "engine->evaluate(\"Array.prototype\")"
                << "engine->evaluate(\"Function.prototype\")"
                << "engine->evaluate(\"Error.prototype\")"
                << "engine->evaluate(\"Object\")"
                << "engine->evaluate(\"Array\")"
                << "engine->evaluate(\"Number\")"
                << "engine->evaluate(\"Function\")"
                << "engine->evaluate(\"(function() { return 1; })\")"
                << "engine->evaluate(\"(function() { return 'ciao'; })\")"
                << "engine->evaluate(\"(function() { throw new Error('foo'); })\")"
                << "engine->evaluate(\"/foo/\")"
                << "engine->evaluate(\"new Object()\")"
                << "engine->evaluate(\"new Array()\")"
                << "engine->evaluate(\"new Error()\")"
               ;
    }
    newRow(expr) << isValid.contains(expr);
}

void tst_QScriptValue::isValid_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isValid(), expected);
}

DEFINE_TEST_FUNCTION(isValid)


void tst_QScriptValue::isBool_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isBool_makeData(const char* expr)
{
    static QSet<QString> isBool;
    if (isBool.isEmpty()) {
        isBool << "QScriptValue(true)"
                << "QScriptValue(false)"
                << "QScriptValue(0, true)"
                << "QScriptValue(0, false)"
                << "QScriptValue(engine, true)"
                << "QScriptValue(engine, false)"
               ;
    }
    newRow(expr) << isBool.contains(expr);
}

void tst_QScriptValue::isBool_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isBool(), expected);
}

DEFINE_TEST_FUNCTION(isBool)


void tst_QScriptValue::isBoolean_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isBoolean_makeData(const char* expr)
{
    static QSet<QString> isBoolean;
    if (isBoolean.isEmpty()) {
        isBoolean << "QScriptValue(true)"
                << "QScriptValue(false)"
                << "QScriptValue(0, true)"
                << "QScriptValue(0, false)"
                << "QScriptValue(engine, true)"
                << "QScriptValue(engine, false)"
               ;
    }
    newRow(expr) << isBoolean.contains(expr);
}

void tst_QScriptValue::isBoolean_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isBoolean(), expected);
}

DEFINE_TEST_FUNCTION(isBoolean)


void tst_QScriptValue::isNumber_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isNumber_makeData(const char* expr)
{
    static QSet<QString> isNumber;
    if (isNumber.isEmpty()) {
        isNumber << "QScriptValue(int(122))"
                << "QScriptValue(uint(124))"
                << "QScriptValue(0)"
                << "QScriptValue(0.0)"
                << "QScriptValue(123.0)"
                << "QScriptValue(6.37e-8)"
                << "QScriptValue(-6.37e-8)"
                << "QScriptValue(0x43211234)"
                << "QScriptValue(0x10000)"
                << "QScriptValue(0x10001)"
                << "QScriptValue(qSNaN())"
                << "QScriptValue(qQNaN())"
                << "QScriptValue(qInf())"
                << "QScriptValue(-qInf())"
                << "QScriptValue(0, int(122))"
                << "QScriptValue(0, uint(124))"
                << "QScriptValue(0, 0)"
                << "QScriptValue(0, 0.0)"
                << "QScriptValue(0, 123.0)"
                << "QScriptValue(0, 6.37e-8)"
                << "QScriptValue(0, -6.37e-8)"
                << "QScriptValue(0, 0x43211234)"
                << "QScriptValue(0, 0x10000)"
                << "QScriptValue(0, 0x10001)"
                << "QScriptValue(0, qSNaN())"
                << "QScriptValue(0, qQNaN())"
                << "QScriptValue(0, qInf())"
                << "QScriptValue(0, -qInf())"
                << "QScriptValue(engine, int(122))"
                << "QScriptValue(engine, uint(124))"
                << "QScriptValue(engine, 0)"
                << "QScriptValue(engine, 0.0)"
                << "QScriptValue(engine, 123.0)"
                << "QScriptValue(engine, 6.37e-8)"
                << "QScriptValue(engine, -6.37e-8)"
                << "QScriptValue(engine, 0x43211234)"
                << "QScriptValue(engine, 0x10000)"
                << "QScriptValue(engine, 0x10001)"
                << "QScriptValue(engine, qSNaN())"
                << "QScriptValue(engine, qQNaN())"
                << "QScriptValue(engine, qInf())"
                << "QScriptValue(engine, -qInf())"
               ;
    }
    newRow(expr) << isNumber.contains(expr);
}

void tst_QScriptValue::isNumber_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isNumber(), expected);
}

DEFINE_TEST_FUNCTION(isNumber)


void tst_QScriptValue::isFunction_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isFunction_makeData(const char* expr)
{
    static QSet<QString> isFunction;
    if (isFunction.isEmpty()) {
        isFunction << "engine->evaluate(\"Function.prototype\")"
                << "engine->evaluate(\"Object\")"
                << "engine->evaluate(\"Array\")"
                << "engine->evaluate(\"Number\")"
                << "engine->evaluate(\"Function\")"
                << "engine->evaluate(\"(function() { return 1; })\")"
                << "engine->evaluate(\"(function() { return 'ciao'; })\")"
                << "engine->evaluate(\"(function() { throw new Error('foo'); })\")"
                << "engine->evaluate(\"/foo/\")"
               ;
    }
    newRow(expr) << isFunction.contains(expr);
}

void tst_QScriptValue::isFunction_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isFunction(), expected);
}

DEFINE_TEST_FUNCTION(isFunction)


void tst_QScriptValue::isNull_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isNull_makeData(const char* expr)
{
    static QSet<QString> isNull;
    if (isNull.isEmpty()) {
        isNull << "QScriptValue(QScriptValue::NullValue)"
                << "QScriptValue(0, QScriptValue::NullValue)"
                << "QScriptValue(engine, QScriptValue::NullValue)"
               ;
    }
    newRow(expr) << isNull.contains(expr);
}

void tst_QScriptValue::isNull_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isNull(), expected);
}

DEFINE_TEST_FUNCTION(isNull)


void tst_QScriptValue::isString_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isString_makeData(const char* expr)
{
    static QSet<QString> isString;
    if (isString.isEmpty()) {
        isString << "QScriptValue(\"NaN\")"
                << "QScriptValue(\"Infinity\")"
                << "QScriptValue(\"-Infinity\")"
                << "QScriptValue(\"ciao\")"
                << "QScriptValue(QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(QString(\"\"))"
                << "QScriptValue(QString())"
                << "QScriptValue(QString(\"0\"))"
                << "QScriptValue(QString(\"123\"))"
                << "QScriptValue(QString(\"12.4\"))"
                << "QScriptValue(0, \"NaN\")"
                << "QScriptValue(0, \"Infinity\")"
                << "QScriptValue(0, \"-Infinity\")"
                << "QScriptValue(0, \"ciao\")"
                << "QScriptValue(0, QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(0, QString(\"\"))"
                << "QScriptValue(0, QString())"
                << "QScriptValue(0, QString(\"0\"))"
                << "QScriptValue(0, QString(\"123\"))"
                << "QScriptValue(0, QString(\"12.3\"))"
                << "QScriptValue(engine, \"NaN\")"
                << "QScriptValue(engine, \"Infinity\")"
                << "QScriptValue(engine, \"-Infinity\")"
                << "QScriptValue(engine, \"ciao\")"
                << "QScriptValue(engine, QString::fromLatin1(\"ciao\"))"
                << "QScriptValue(engine, QString(\"\"))"
                << "QScriptValue(engine, QString())"
                << "QScriptValue(engine, QString(\"0\"))"
                << "QScriptValue(engine, QString(\"123\"))"
                << "QScriptValue(engine, QString(\"1.23\"))"
               ;
    }
    newRow(expr) << isString.contains(expr);
}

void tst_QScriptValue::isString_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isString(), expected);
}

DEFINE_TEST_FUNCTION(isString)


void tst_QScriptValue::isUndefined_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isUndefined_makeData(const char* expr)
{
    static QSet<QString> isUndefined;
    if (isUndefined.isEmpty()) {
        isUndefined << "QScriptValue(QScriptValue::UndefinedValue)"
                << "QScriptValue(0, QScriptValue::UndefinedValue)"
                << "QScriptValue(engine, QScriptValue::UndefinedValue)"
                << "engine->evaluate(\"{}\")"
               ;
    }
    newRow(expr) << isUndefined.contains(expr);
}

void tst_QScriptValue::isUndefined_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isUndefined(), expected);
}

DEFINE_TEST_FUNCTION(isUndefined)





void tst_QScriptValue::isObject_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isObject_makeData(const char* expr)
{
    static QSet<QString> isObject;
    if (isObject.isEmpty()) {
        isObject << "engine->evaluate(\"[]\")"
                << "engine->evaluate(\"Object.prototype\")"
                << "engine->evaluate(\"Date.prototype\")"
                << "engine->evaluate(\"Array.prototype\")"
                << "engine->evaluate(\"Function.prototype\")"
                << "engine->evaluate(\"Error.prototype\")"
                << "engine->evaluate(\"Object\")"
                << "engine->evaluate(\"Array\")"
                << "engine->evaluate(\"Number\")"
                << "engine->evaluate(\"Function\")"
                << "engine->evaluate(\"(function() { return 1; })\")"
                << "engine->evaluate(\"(function() { return 'ciao'; })\")"
                << "engine->evaluate(\"(function() { throw new Error('foo'); })\")"
                << "engine->evaluate(\"/foo/\")"
                << "engine->evaluate(\"new Object()\")"
                << "engine->evaluate(\"new Array()\")"
                << "engine->evaluate(\"new Error()\")"
               ;
    }
    newRow(expr) << isObject.contains(expr);
}

void tst_QScriptValue::isObject_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isObject(), expected);
}

DEFINE_TEST_FUNCTION(isObject)


void tst_QScriptValue::isDate_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isDate_makeData(const char* expr)
{
    static QSet<QString> isDate;
    if (isDate.isEmpty()) {
        isDate << "engine->evaluate(\"Date.prototype\")"
               ;
    }
    newRow(expr) << isDate.contains(expr);
}

void tst_QScriptValue::isDate_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isDate(), expected);
}

DEFINE_TEST_FUNCTION(isDate)


void tst_QScriptValue::isRegExp_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isRegExp_makeData(const char* expr)
{
    static QSet<QString> isRegExp;
    if (isRegExp.isEmpty()) {
        isRegExp << "engine->evaluate(\"/foo/\")"
               ;
    }
    newRow(expr) << isRegExp.contains(expr);
}

void tst_QScriptValue::isRegExp_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isRegExp(), expected);
}

DEFINE_TEST_FUNCTION(isRegExp)


void tst_QScriptValue::isArray_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isArray_makeData(const char* expr)
{
    static QSet<QString> isArray;
    if (isArray.isEmpty()) {
        isArray << "engine->evaluate(\"[]\")"
                << "engine->evaluate(\"Array.prototype\")"
                << "engine->evaluate(\"new Array()\")"
               ;
    }
    newRow(expr) << isArray.contains(expr);
}

void tst_QScriptValue::isArray_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isArray(), expected);
}

DEFINE_TEST_FUNCTION(isArray)


void tst_QScriptValue::isError_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::isError_makeData(const char* expr)
{
    static QSet<QString> isError;
    if (isError.isEmpty()) {
        isError << "engine->evaluate(\"Error.prototype\")"
                << "engine->evaluate(\"new Error()\")"
               ;
    }
    newRow(expr) << isError.contains(expr);
}

void tst_QScriptValue::isError_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isError(), expected);
}

DEFINE_TEST_FUNCTION(isError)



void tst_QScriptValue::toString_initData()
{
    QTest::addColumn<QString>("expected");
    initScriptValues();
}

void tst_QScriptValue::toString_makeData(const char* expr)
{
    static QHash<QString, QString> toString;
    if (toString.isEmpty()) {
        toString.insert("QScriptValue()", "");
        toString.insert("QScriptValue(QScriptValue::UndefinedValue)", "undefined");
        toString.insert("QScriptValue(QScriptValue::NullValue)", "null");
        toString.insert("QScriptValue(true)", "true");
        toString.insert("QScriptValue(false)", "false");
        toString.insert("QScriptValue(int(122))", "122");
        toString.insert("QScriptValue(uint(124))", "124");
        toString.insert("QScriptValue(0)", "0");
        toString.insert("QScriptValue(0.0)", "0");
        toString.insert("QScriptValue(123.0)", "123");
        toString.insert("QScriptValue(6.37e-8)", "6.37e-8");
        toString.insert("QScriptValue(-6.37e-8)", "-6.37e-8");
        toString.insert("QScriptValue(0x43211234)", "1126240820");
        toString.insert("QScriptValue(0x10000)", "65536");
        toString.insert("QScriptValue(0x10001)", "65537");
        toString.insert("QScriptValue(qSNaN())", "NaN");
        toString.insert("QScriptValue(qQNaN())", "NaN");
        toString.insert("QScriptValue(qInf())", "Infinity");
        toString.insert("QScriptValue(-qInf())", "-Infinity");
        toString.insert("QScriptValue(\"NaN\")", "NaN");
        toString.insert("QScriptValue(\"Infinity\")", "Infinity");
        toString.insert("QScriptValue(\"-Infinity\")", "-Infinity");
        toString.insert("QScriptValue(\"ciao\")", "ciao");
        toString.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", "ciao");
        toString.insert("QScriptValue(QString(\"\"))", "");
        toString.insert("QScriptValue(QString())", "");
        toString.insert("QScriptValue(QString(\"0\"))", "0");
        toString.insert("QScriptValue(QString(\"123\"))", "123");
        toString.insert("QScriptValue(QString(\"12.4\"))", "12.4");
        toString.insert("QScriptValue(0, QScriptValue::UndefinedValue)", "undefined");
        toString.insert("QScriptValue(0, QScriptValue::NullValue)", "null");
        toString.insert("QScriptValue(0, true)", "true");
        toString.insert("QScriptValue(0, false)", "false");
        toString.insert("QScriptValue(0, int(122))", "122");
        toString.insert("QScriptValue(0, uint(124))", "124");
        toString.insert("QScriptValue(0, 0)", "0");
        toString.insert("QScriptValue(0, 0.0)", "0");
        toString.insert("QScriptValue(0, 123.0)", "123");
        toString.insert("QScriptValue(0, 6.37e-8)", "6.37e-8");
        toString.insert("QScriptValue(0, -6.37e-8)", "-6.37e-8");
        toString.insert("QScriptValue(0, 0x43211234)", "1126240820");
        toString.insert("QScriptValue(0, 0x10000)", "65536");
        toString.insert("QScriptValue(0, 0x10001)", "65537");
        toString.insert("QScriptValue(0, qSNaN())", "NaN");
        toString.insert("QScriptValue(0, qQNaN())", "NaN");
        toString.insert("QScriptValue(0, qInf())", "Infinity");
        toString.insert("QScriptValue(0, -qInf())", "-Infinity");
        toString.insert("QScriptValue(0, \"NaN\")", "NaN");
        toString.insert("QScriptValue(0, \"Infinity\")", "Infinity");
        toString.insert("QScriptValue(0, \"-Infinity\")", "-Infinity");
        toString.insert("QScriptValue(0, \"ciao\")", "ciao");
        toString.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", "ciao");
        toString.insert("QScriptValue(0, QString(\"\"))", "");
        toString.insert("QScriptValue(0, QString())", "");
        toString.insert("QScriptValue(0, QString(\"0\"))", "0");
        toString.insert("QScriptValue(0, QString(\"123\"))", "123");
        toString.insert("QScriptValue(0, QString(\"12.3\"))", "12.3");
        toString.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", "undefined");
        toString.insert("QScriptValue(engine, QScriptValue::NullValue)", "null");
        toString.insert("QScriptValue(engine, true)", "true");
        toString.insert("QScriptValue(engine, false)", "false");
        toString.insert("QScriptValue(engine, int(122))", "122");
        toString.insert("QScriptValue(engine, uint(124))", "124");
        toString.insert("QScriptValue(engine, 0)", "0");
        toString.insert("QScriptValue(engine, 0.0)", "0");
        toString.insert("QScriptValue(engine, 123.0)", "123");
        toString.insert("QScriptValue(engine, 6.37e-8)", "6.37e-8");
        toString.insert("QScriptValue(engine, -6.37e-8)", "-6.37e-8");
        toString.insert("QScriptValue(engine, 0x43211234)", "1126240820");
        toString.insert("QScriptValue(engine, 0x10000)", "65536");
        toString.insert("QScriptValue(engine, 0x10001)", "65537");
        toString.insert("QScriptValue(engine, qSNaN())", "NaN");
        toString.insert("QScriptValue(engine, qQNaN())", "NaN");
        toString.insert("QScriptValue(engine, qInf())", "Infinity");
        toString.insert("QScriptValue(engine, -qInf())", "-Infinity");
        toString.insert("QScriptValue(engine, \"NaN\")", "NaN");
        toString.insert("QScriptValue(engine, \"Infinity\")", "Infinity");
        toString.insert("QScriptValue(engine, \"-Infinity\")", "-Infinity");
        toString.insert("QScriptValue(engine, \"ciao\")", "ciao");
        toString.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", "ciao");
        toString.insert("QScriptValue(engine, QString(\"\"))", "");
        toString.insert("QScriptValue(engine, QString())", "");
        toString.insert("QScriptValue(engine, QString(\"0\"))", "0");
        toString.insert("QScriptValue(engine, QString(\"123\"))", "123");
        toString.insert("QScriptValue(engine, QString(\"1.23\"))", "1.23");
        toString.insert("engine->evaluate(\"[]\")", "");
        toString.insert("engine->evaluate(\"{}\")", "undefined");
        toString.insert("engine->evaluate(\"Object.prototype\")", "[object Object]");
        toString.insert("engine->evaluate(\"Date.prototype\")", "Invalid Date");
        toString.insert("engine->evaluate(\"Array.prototype\")", "");
        toString.insert("engine->evaluate(\"Function.prototype\")", "function () {\n    [native code]\n}");
        toString.insert("engine->evaluate(\"Error.prototype\")", "Error: Unknown error");
        toString.insert("engine->evaluate(\"Object\")", "function Object() {\n    [native code]\n}");
        toString.insert("engine->evaluate(\"Array\")", "function Array() {\n    [native code]\n}");
        toString.insert("engine->evaluate(\"Number\")", "function Number() {\n    [native code]\n}");
        toString.insert("engine->evaluate(\"Function\")", "function Function() {\n    [native code]\n}");
        toString.insert("engine->evaluate(\"(function() { return 1; })\")", "function () { return 1; }");
        toString.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", "function () { return 'ciao'; }");
        toString.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", "function () { throw new Error('foo'); }");
        toString.insert("engine->evaluate(\"/foo/\")", "/foo/");
        toString.insert("engine->evaluate(\"new Object()\")", "[object Object]");
        toString.insert("engine->evaluate(\"new Array()\")", "");
        toString.insert("engine->evaluate(\"new Error()\")", "Error: Unknown error");
    }
    newRow(expr) << toString.value(expr);
}

void tst_QScriptValue::toString_test(const char*, const QScriptValue& value)
{
    QFETCH(QString, expected);
    QCOMPARE(value.toString(), expected);
}

DEFINE_TEST_FUNCTION(toString)


void tst_QScriptValue::toNumber_initData()
{
    QTest::addColumn<qsreal>("expected");
    initScriptValues();
}

void tst_QScriptValue::toNumber_makeData(const char* expr)
{
    static QHash<QString, qsreal> toNumber;
    if (toNumber.isEmpty()) {
        toNumber.insert("QScriptValue()", 0);
        toNumber.insert("QScriptValue(QScriptValue::UndefinedValue)", qQNaN());
        toNumber.insert("QScriptValue(QScriptValue::NullValue)", 0);
        toNumber.insert("QScriptValue(true)", 1);
        toNumber.insert("QScriptValue(false)", 0);
        toNumber.insert("QScriptValue(int(122))", 122);
        toNumber.insert("QScriptValue(uint(124))", 124);
        toNumber.insert("QScriptValue(0)", 0);
        toNumber.insert("QScriptValue(0.0)", 0);
        toNumber.insert("QScriptValue(123.0)", 123);
        toNumber.insert("QScriptValue(6.37e-8)", 6.369999999999999e-08);
        toNumber.insert("QScriptValue(-6.37e-8)", -6.369999999999999e-08);
        toNumber.insert("QScriptValue(0x43211234)", 1126240820);
        toNumber.insert("QScriptValue(0x10000)", 65536);
        toNumber.insert("QScriptValue(0x10001)", 65537);
        toNumber.insert("QScriptValue(qSNaN())", qQNaN());
        toNumber.insert("QScriptValue(qQNaN())", qQNaN());
        toNumber.insert("QScriptValue(qInf())", qInf());
        toNumber.insert("QScriptValue(-qInf())", qInf());
        toNumber.insert("QScriptValue(\"NaN\")", qQNaN());
        toNumber.insert("QScriptValue(\"Infinity\")", qInf());
        toNumber.insert("QScriptValue(\"-Infinity\")", qInf());
        toNumber.insert("QScriptValue(\"ciao\")", qQNaN());
        toNumber.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", qQNaN());
        toNumber.insert("QScriptValue(QString(\"\"))", 0);
        toNumber.insert("QScriptValue(QString())", 0);
        toNumber.insert("QScriptValue(QString(\"0\"))", 0);
        toNumber.insert("QScriptValue(QString(\"123\"))", 123);
        toNumber.insert("QScriptValue(QString(\"12.4\"))", 12.4);
        toNumber.insert("QScriptValue(0, QScriptValue::UndefinedValue)", qQNaN());
        toNumber.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        toNumber.insert("QScriptValue(0, true)", 1);
        toNumber.insert("QScriptValue(0, false)", 0);
        toNumber.insert("QScriptValue(0, int(122))", 122);
        toNumber.insert("QScriptValue(0, uint(124))", 124);
        toNumber.insert("QScriptValue(0, 0)", 0);
        toNumber.insert("QScriptValue(0, 0.0)", 0);
        toNumber.insert("QScriptValue(0, 123.0)", 123);
        toNumber.insert("QScriptValue(0, 6.37e-8)", 6.369999999999999e-08);
        toNumber.insert("QScriptValue(0, -6.37e-8)", -6.369999999999999e-08);
        toNumber.insert("QScriptValue(0, 0x43211234)", 1126240820);
        toNumber.insert("QScriptValue(0, 0x10000)", 65536);
        toNumber.insert("QScriptValue(0, 0x10001)", 65537);
        toNumber.insert("QScriptValue(0, qSNaN())", qQNaN());
        toNumber.insert("QScriptValue(0, qQNaN())", qQNaN());
        toNumber.insert("QScriptValue(0, qInf())", qInf());
        toNumber.insert("QScriptValue(0, -qInf())", qInf());
        toNumber.insert("QScriptValue(0, \"NaN\")", qQNaN());
        toNumber.insert("QScriptValue(0, \"Infinity\")", qInf());
        toNumber.insert("QScriptValue(0, \"-Infinity\")", qInf());
        toNumber.insert("QScriptValue(0, \"ciao\")", qQNaN());
        toNumber.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", qQNaN());
        toNumber.insert("QScriptValue(0, QString(\"\"))", 0);
        toNumber.insert("QScriptValue(0, QString())", 0);
        toNumber.insert("QScriptValue(0, QString(\"0\"))", 0);
        toNumber.insert("QScriptValue(0, QString(\"123\"))", 123);
        toNumber.insert("QScriptValue(0, QString(\"12.3\"))", 12.3);
        toNumber.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", qQNaN());
        toNumber.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        toNumber.insert("QScriptValue(engine, true)", 1);
        toNumber.insert("QScriptValue(engine, false)", 0);
        toNumber.insert("QScriptValue(engine, int(122))", 122);
        toNumber.insert("QScriptValue(engine, uint(124))", 124);
        toNumber.insert("QScriptValue(engine, 0)", 0);
        toNumber.insert("QScriptValue(engine, 0.0)", 0);
        toNumber.insert("QScriptValue(engine, 123.0)", 123);
        toNumber.insert("QScriptValue(engine, 6.37e-8)", 6.369999999999999e-08);
        toNumber.insert("QScriptValue(engine, -6.37e-8)", -6.369999999999999e-08);
        toNumber.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        toNumber.insert("QScriptValue(engine, 0x10000)", 65536);
        toNumber.insert("QScriptValue(engine, 0x10001)", 65537);
        toNumber.insert("QScriptValue(engine, qSNaN())", qQNaN());
        toNumber.insert("QScriptValue(engine, qQNaN())", qQNaN());
        toNumber.insert("QScriptValue(engine, qInf())", qInf());
        toNumber.insert("QScriptValue(engine, -qInf())", qInf());
        toNumber.insert("QScriptValue(engine, \"NaN\")", qQNaN());
        toNumber.insert("QScriptValue(engine, \"Infinity\")", qInf());
        toNumber.insert("QScriptValue(engine, \"-Infinity\")", qInf());
        toNumber.insert("QScriptValue(engine, \"ciao\")", qQNaN());
        toNumber.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", qQNaN());
        toNumber.insert("QScriptValue(engine, QString(\"\"))", 0);
        toNumber.insert("QScriptValue(engine, QString())", 0);
        toNumber.insert("QScriptValue(engine, QString(\"0\"))", 0);
        toNumber.insert("QScriptValue(engine, QString(\"123\"))", 123);
        toNumber.insert("QScriptValue(engine, QString(\"1.23\"))", 1.23);
        toNumber.insert("engine->evaluate(\"[]\")", 0);
        toNumber.insert("engine->evaluate(\"{}\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Object.prototype\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Date.prototype\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Array.prototype\")", 0);
        toNumber.insert("engine->evaluate(\"Function.prototype\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Error.prototype\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Object\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Array\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Number\")", qQNaN());
        toNumber.insert("engine->evaluate(\"Function\")", qQNaN());
        toNumber.insert("engine->evaluate(\"(function() { return 1; })\")", qQNaN());
        toNumber.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", qQNaN());
        toNumber.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", qQNaN());
        toNumber.insert("engine->evaluate(\"/foo/\")", qQNaN());
        toNumber.insert("engine->evaluate(\"new Object()\")", qQNaN());
        toNumber.insert("engine->evaluate(\"new Array()\")", 0);
        toNumber.insert("engine->evaluate(\"new Error()\")", qQNaN());
    }
    newRow(expr) << toNumber.value(expr);
}

void tst_QScriptValue::toNumber_test(const char*, const QScriptValue& value)
{
    QFETCH(qsreal, expected);
    if (qIsNaN(expected)) {
        QVERIFY(qIsNaN(value.toNumber()));
        return;
    }
    if (qIsInf(expected)) {
        QVERIFY(qIsInf(value.toNumber()));
        return;
    }
    QCOMPARE(value.toNumber(), expected);
}

DEFINE_TEST_FUNCTION(toNumber)


void tst_QScriptValue::toBool_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::toBool_makeData(const char* expr)
{
    static QHash<QString, bool> toBool;
    if (toBool.isEmpty()) {
        toBool.insert("QScriptValue()", false);
        toBool.insert("QScriptValue(QScriptValue::UndefinedValue)", false);
        toBool.insert("QScriptValue(QScriptValue::NullValue)", false);
        toBool.insert("QScriptValue(true)", true);
        toBool.insert("QScriptValue(false)", false);
        toBool.insert("QScriptValue(int(122))", true);
        toBool.insert("QScriptValue(uint(124))", true);
        toBool.insert("QScriptValue(0)", false);
        toBool.insert("QScriptValue(0.0)", false);
        toBool.insert("QScriptValue(123.0)", true);
        toBool.insert("QScriptValue(6.37e-8)", true);
        toBool.insert("QScriptValue(-6.37e-8)", true);
        toBool.insert("QScriptValue(0x43211234)", true);
        toBool.insert("QScriptValue(0x10000)", true);
        toBool.insert("QScriptValue(0x10001)", true);
        toBool.insert("QScriptValue(qSNaN())", false);
        toBool.insert("QScriptValue(qQNaN())", false);
        toBool.insert("QScriptValue(qInf())", true);
        toBool.insert("QScriptValue(-qInf())", true);
        toBool.insert("QScriptValue(\"NaN\")", true);
        toBool.insert("QScriptValue(\"Infinity\")", true);
        toBool.insert("QScriptValue(\"-Infinity\")", true);
        toBool.insert("QScriptValue(\"ciao\")", true);
        toBool.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", true);
        toBool.insert("QScriptValue(QString(\"\"))", false);
        toBool.insert("QScriptValue(QString())", false);
        toBool.insert("QScriptValue(QString(\"0\"))", true);
        toBool.insert("QScriptValue(QString(\"123\"))", true);
        toBool.insert("QScriptValue(QString(\"12.4\"))", true);
        toBool.insert("QScriptValue(0, QScriptValue::UndefinedValue)", false);
        toBool.insert("QScriptValue(0, QScriptValue::NullValue)", false);
        toBool.insert("QScriptValue(0, true)", true);
        toBool.insert("QScriptValue(0, false)", false);
        toBool.insert("QScriptValue(0, int(122))", true);
        toBool.insert("QScriptValue(0, uint(124))", true);
        toBool.insert("QScriptValue(0, 0)", false);
        toBool.insert("QScriptValue(0, 0.0)", false);
        toBool.insert("QScriptValue(0, 123.0)", true);
        toBool.insert("QScriptValue(0, 6.37e-8)", true);
        toBool.insert("QScriptValue(0, -6.37e-8)", true);
        toBool.insert("QScriptValue(0, 0x43211234)", true);
        toBool.insert("QScriptValue(0, 0x10000)", true);
        toBool.insert("QScriptValue(0, 0x10001)", true);
        toBool.insert("QScriptValue(0, qSNaN())", false);
        toBool.insert("QScriptValue(0, qQNaN())", false);
        toBool.insert("QScriptValue(0, qInf())", true);
        toBool.insert("QScriptValue(0, -qInf())", true);
        toBool.insert("QScriptValue(0, \"NaN\")", true);
        toBool.insert("QScriptValue(0, \"Infinity\")", true);
        toBool.insert("QScriptValue(0, \"-Infinity\")", true);
        toBool.insert("QScriptValue(0, \"ciao\")", true);
        toBool.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", true);
        toBool.insert("QScriptValue(0, QString(\"\"))", false);
        toBool.insert("QScriptValue(0, QString())", false);
        toBool.insert("QScriptValue(0, QString(\"0\"))", true);
        toBool.insert("QScriptValue(0, QString(\"123\"))", true);
        toBool.insert("QScriptValue(0, QString(\"12.3\"))", true);
        toBool.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", false);
        toBool.insert("QScriptValue(engine, QScriptValue::NullValue)", false);
        toBool.insert("QScriptValue(engine, true)", true);
        toBool.insert("QScriptValue(engine, false)", false);
        toBool.insert("QScriptValue(engine, int(122))", true);
        toBool.insert("QScriptValue(engine, uint(124))", true);
        toBool.insert("QScriptValue(engine, 0)", false);
        toBool.insert("QScriptValue(engine, 0.0)", false);
        toBool.insert("QScriptValue(engine, 123.0)", true);
        toBool.insert("QScriptValue(engine, 6.37e-8)", true);
        toBool.insert("QScriptValue(engine, -6.37e-8)", true);
        toBool.insert("QScriptValue(engine, 0x43211234)", true);
        toBool.insert("QScriptValue(engine, 0x10000)", true);
        toBool.insert("QScriptValue(engine, 0x10001)", true);
        toBool.insert("QScriptValue(engine, qSNaN())", false);
        toBool.insert("QScriptValue(engine, qQNaN())", false);
        toBool.insert("QScriptValue(engine, qInf())", true);
        toBool.insert("QScriptValue(engine, -qInf())", true);
        toBool.insert("QScriptValue(engine, \"NaN\")", true);
        toBool.insert("QScriptValue(engine, \"Infinity\")", true);
        toBool.insert("QScriptValue(engine, \"-Infinity\")", true);
        toBool.insert("QScriptValue(engine, \"ciao\")", true);
        toBool.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", true);
        toBool.insert("QScriptValue(engine, QString(\"\"))", false);
        toBool.insert("QScriptValue(engine, QString())", false);
        toBool.insert("QScriptValue(engine, QString(\"0\"))", true);
        toBool.insert("QScriptValue(engine, QString(\"123\"))", true);
        toBool.insert("QScriptValue(engine, QString(\"1.23\"))", true);
        toBool.insert("engine->evaluate(\"[]\")", true);
        toBool.insert("engine->evaluate(\"{}\")", false);
        toBool.insert("engine->evaluate(\"Object.prototype\")", true);
        toBool.insert("engine->evaluate(\"Date.prototype\")", true);
        toBool.insert("engine->evaluate(\"Array.prototype\")", true);
        toBool.insert("engine->evaluate(\"Function.prototype\")", true);
        toBool.insert("engine->evaluate(\"Error.prototype\")", true);
        toBool.insert("engine->evaluate(\"Object\")", true);
        toBool.insert("engine->evaluate(\"Array\")", true);
        toBool.insert("engine->evaluate(\"Number\")", true);
        toBool.insert("engine->evaluate(\"Function\")", true);
        toBool.insert("engine->evaluate(\"(function() { return 1; })\")", true);
        toBool.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", true);
        toBool.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", true);
        toBool.insert("engine->evaluate(\"/foo/\")", true);
        toBool.insert("engine->evaluate(\"new Object()\")", true);
        toBool.insert("engine->evaluate(\"new Array()\")", true);
        toBool.insert("engine->evaluate(\"new Error()\")", true);
    }
    newRow(expr) << toBool.value(expr);
}

void tst_QScriptValue::toBool_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.toBool(), expected);
}

DEFINE_TEST_FUNCTION(toBool)


void tst_QScriptValue::toBoolean_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::toBoolean_makeData(const char* expr)
{
    static QHash<QString, bool> toBoolean;
    if (toBoolean.isEmpty()) {
        toBoolean.insert("QScriptValue()", false);
        toBoolean.insert("QScriptValue(QScriptValue::UndefinedValue)", false);
        toBoolean.insert("QScriptValue(QScriptValue::NullValue)", false);
        toBoolean.insert("QScriptValue(true)", true);
        toBoolean.insert("QScriptValue(false)", false);
        toBoolean.insert("QScriptValue(int(122))", true);
        toBoolean.insert("QScriptValue(uint(124))", true);
        toBoolean.insert("QScriptValue(0)", false);
        toBoolean.insert("QScriptValue(0.0)", false);
        toBoolean.insert("QScriptValue(123.0)", true);
        toBoolean.insert("QScriptValue(6.37e-8)", true);
        toBoolean.insert("QScriptValue(-6.37e-8)", true);
        toBoolean.insert("QScriptValue(0x43211234)", true);
        toBoolean.insert("QScriptValue(0x10000)", true);
        toBoolean.insert("QScriptValue(0x10001)", true);
        toBoolean.insert("QScriptValue(qSNaN())", false);
        toBoolean.insert("QScriptValue(qQNaN())", false);
        toBoolean.insert("QScriptValue(qInf())", true);
        toBoolean.insert("QScriptValue(-qInf())", true);
        toBoolean.insert("QScriptValue(\"NaN\")", true);
        toBoolean.insert("QScriptValue(\"Infinity\")", true);
        toBoolean.insert("QScriptValue(\"-Infinity\")", true);
        toBoolean.insert("QScriptValue(\"ciao\")", true);
        toBoolean.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", true);
        toBoolean.insert("QScriptValue(QString(\"\"))", false);
        toBoolean.insert("QScriptValue(QString())", false);
        toBoolean.insert("QScriptValue(QString(\"0\"))", true);
        toBoolean.insert("QScriptValue(QString(\"123\"))", true);
        toBoolean.insert("QScriptValue(QString(\"12.4\"))", true);
        toBoolean.insert("QScriptValue(0, QScriptValue::UndefinedValue)", false);
        toBoolean.insert("QScriptValue(0, QScriptValue::NullValue)", false);
        toBoolean.insert("QScriptValue(0, true)", true);
        toBoolean.insert("QScriptValue(0, false)", false);
        toBoolean.insert("QScriptValue(0, int(122))", true);
        toBoolean.insert("QScriptValue(0, uint(124))", true);
        toBoolean.insert("QScriptValue(0, 0)", false);
        toBoolean.insert("QScriptValue(0, 0.0)", false);
        toBoolean.insert("QScriptValue(0, 123.0)", true);
        toBoolean.insert("QScriptValue(0, 6.37e-8)", true);
        toBoolean.insert("QScriptValue(0, -6.37e-8)", true);
        toBoolean.insert("QScriptValue(0, 0x43211234)", true);
        toBoolean.insert("QScriptValue(0, 0x10000)", true);
        toBoolean.insert("QScriptValue(0, 0x10001)", true);
        toBoolean.insert("QScriptValue(0, qSNaN())", false);
        toBoolean.insert("QScriptValue(0, qQNaN())", false);
        toBoolean.insert("QScriptValue(0, qInf())", true);
        toBoolean.insert("QScriptValue(0, -qInf())", true);
        toBoolean.insert("QScriptValue(0, \"NaN\")", true);
        toBoolean.insert("QScriptValue(0, \"Infinity\")", true);
        toBoolean.insert("QScriptValue(0, \"-Infinity\")", true);
        toBoolean.insert("QScriptValue(0, \"ciao\")", true);
        toBoolean.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", true);
        toBoolean.insert("QScriptValue(0, QString(\"\"))", false);
        toBoolean.insert("QScriptValue(0, QString())", false);
        toBoolean.insert("QScriptValue(0, QString(\"0\"))", true);
        toBoolean.insert("QScriptValue(0, QString(\"123\"))", true);
        toBoolean.insert("QScriptValue(0, QString(\"12.3\"))", true);
        toBoolean.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", false);
        toBoolean.insert("QScriptValue(engine, QScriptValue::NullValue)", false);
        toBoolean.insert("QScriptValue(engine, true)", true);
        toBoolean.insert("QScriptValue(engine, false)", false);
        toBoolean.insert("QScriptValue(engine, int(122))", true);
        toBoolean.insert("QScriptValue(engine, uint(124))", true);
        toBoolean.insert("QScriptValue(engine, 0)", false);
        toBoolean.insert("QScriptValue(engine, 0.0)", false);
        toBoolean.insert("QScriptValue(engine, 123.0)", true);
        toBoolean.insert("QScriptValue(engine, 6.37e-8)", true);
        toBoolean.insert("QScriptValue(engine, -6.37e-8)", true);
        toBoolean.insert("QScriptValue(engine, 0x43211234)", true);
        toBoolean.insert("QScriptValue(engine, 0x10000)", true);
        toBoolean.insert("QScriptValue(engine, 0x10001)", true);
        toBoolean.insert("QScriptValue(engine, qSNaN())", false);
        toBoolean.insert("QScriptValue(engine, qQNaN())", false);
        toBoolean.insert("QScriptValue(engine, qInf())", true);
        toBoolean.insert("QScriptValue(engine, -qInf())", true);
        toBoolean.insert("QScriptValue(engine, \"NaN\")", true);
        toBoolean.insert("QScriptValue(engine, \"Infinity\")", true);
        toBoolean.insert("QScriptValue(engine, \"-Infinity\")", true);
        toBoolean.insert("QScriptValue(engine, \"ciao\")", true);
        toBoolean.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", true);
        toBoolean.insert("QScriptValue(engine, QString(\"\"))", false);
        toBoolean.insert("QScriptValue(engine, QString())", false);
        toBoolean.insert("QScriptValue(engine, QString(\"0\"))", true);
        toBoolean.insert("QScriptValue(engine, QString(\"123\"))", true);
        toBoolean.insert("QScriptValue(engine, QString(\"1.23\"))", true);
        toBoolean.insert("engine->evaluate(\"[]\")", true);
        toBoolean.insert("engine->evaluate(\"{}\")", false);
        toBoolean.insert("engine->evaluate(\"Object.prototype\")", true);
        toBoolean.insert("engine->evaluate(\"Date.prototype\")", true);
        toBoolean.insert("engine->evaluate(\"Array.prototype\")", true);
        toBoolean.insert("engine->evaluate(\"Function.prototype\")", true);
        toBoolean.insert("engine->evaluate(\"Error.prototype\")", true);
        toBoolean.insert("engine->evaluate(\"Object\")", true);
        toBoolean.insert("engine->evaluate(\"Array\")", true);
        toBoolean.insert("engine->evaluate(\"Number\")", true);
        toBoolean.insert("engine->evaluate(\"Function\")", true);
        toBoolean.insert("engine->evaluate(\"(function() { return 1; })\")", true);
        toBoolean.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", true);
        toBoolean.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", true);
        toBoolean.insert("engine->evaluate(\"/foo/\")", true);
        toBoolean.insert("engine->evaluate(\"new Object()\")", true);
        toBoolean.insert("engine->evaluate(\"new Array()\")", true);
        toBoolean.insert("engine->evaluate(\"new Error()\")", true);
    }
    newRow(expr) << toBoolean.value(expr);
}

void tst_QScriptValue::toBoolean_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.toBoolean(), expected);
}

DEFINE_TEST_FUNCTION(toBoolean)


void tst_QScriptValue::toInteger_initData()
{
    QTest::addColumn<qsreal>("expected");
    initScriptValues();
}

void tst_QScriptValue::toInteger_makeData(const char* expr)
{
    static QHash<QString, qsreal> toInteger;
    if (toInteger.isEmpty()) {
        toInteger.insert("QScriptValue()", 0);
        toInteger.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        toInteger.insert("QScriptValue(QScriptValue::NullValue)", 0);
        toInteger.insert("QScriptValue(true)", 1);
        toInteger.insert("QScriptValue(false)", 0);
        toInteger.insert("QScriptValue(int(122))", 122);
        toInteger.insert("QScriptValue(uint(124))", 124);
        toInteger.insert("QScriptValue(0)", 0);
        toInteger.insert("QScriptValue(0.0)", 0);
        toInteger.insert("QScriptValue(123.0)", 123);
        toInteger.insert("QScriptValue(6.37e-8)", 0);
        toInteger.insert("QScriptValue(-6.37e-8)", 0);
        toInteger.insert("QScriptValue(0x43211234)", 1126240820);
        toInteger.insert("QScriptValue(0x10000)", 65536);
        toInteger.insert("QScriptValue(0x10001)", 65537);
        toInteger.insert("QScriptValue(qSNaN())", 0);
        toInteger.insert("QScriptValue(qQNaN())", 0);
        toInteger.insert("QScriptValue(qInf())", qInf());
        toInteger.insert("QScriptValue(-qInf())", qInf());
        toInteger.insert("QScriptValue(\"NaN\")", 0);
        toInteger.insert("QScriptValue(\"Infinity\")", qInf());
        toInteger.insert("QScriptValue(\"-Infinity\")", qInf());
        toInteger.insert("QScriptValue(\"ciao\")", 0);
        toInteger.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        toInteger.insert("QScriptValue(QString(\"\"))", 0);
        toInteger.insert("QScriptValue(QString())", 0);
        toInteger.insert("QScriptValue(QString(\"0\"))", 0);
        toInteger.insert("QScriptValue(QString(\"123\"))", 123);
        toInteger.insert("QScriptValue(QString(\"12.4\"))", 12);
        toInteger.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        toInteger.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        toInteger.insert("QScriptValue(0, true)", 1);
        toInteger.insert("QScriptValue(0, false)", 0);
        toInteger.insert("QScriptValue(0, int(122))", 122);
        toInteger.insert("QScriptValue(0, uint(124))", 124);
        toInteger.insert("QScriptValue(0, 0)", 0);
        toInteger.insert("QScriptValue(0, 0.0)", 0);
        toInteger.insert("QScriptValue(0, 123.0)", 123);
        toInteger.insert("QScriptValue(0, 6.37e-8)", 0);
        toInteger.insert("QScriptValue(0, -6.37e-8)", 0);
        toInteger.insert("QScriptValue(0, 0x43211234)", 1126240820);
        toInteger.insert("QScriptValue(0, 0x10000)", 65536);
        toInteger.insert("QScriptValue(0, 0x10001)", 65537);
        toInteger.insert("QScriptValue(0, qSNaN())", 0);
        toInteger.insert("QScriptValue(0, qQNaN())", 0);
        toInteger.insert("QScriptValue(0, qInf())", qInf());
        toInteger.insert("QScriptValue(0, -qInf())", qInf());
        toInteger.insert("QScriptValue(0, \"NaN\")", 0);
        toInteger.insert("QScriptValue(0, \"Infinity\")", qInf());
        toInteger.insert("QScriptValue(0, \"-Infinity\")", qInf());
        toInteger.insert("QScriptValue(0, \"ciao\")", 0);
        toInteger.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        toInteger.insert("QScriptValue(0, QString(\"\"))", 0);
        toInteger.insert("QScriptValue(0, QString())", 0);
        toInteger.insert("QScriptValue(0, QString(\"0\"))", 0);
        toInteger.insert("QScriptValue(0, QString(\"123\"))", 123);
        toInteger.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        toInteger.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        toInteger.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        toInteger.insert("QScriptValue(engine, true)", 1);
        toInteger.insert("QScriptValue(engine, false)", 0);
        toInteger.insert("QScriptValue(engine, int(122))", 122);
        toInteger.insert("QScriptValue(engine, uint(124))", 124);
        toInteger.insert("QScriptValue(engine, 0)", 0);
        toInteger.insert("QScriptValue(engine, 0.0)", 0);
        toInteger.insert("QScriptValue(engine, 123.0)", 123);
        toInteger.insert("QScriptValue(engine, 6.37e-8)", 0);
        toInteger.insert("QScriptValue(engine, -6.37e-8)", 0);
        toInteger.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        toInteger.insert("QScriptValue(engine, 0x10000)", 65536);
        toInteger.insert("QScriptValue(engine, 0x10001)", 65537);
        toInteger.insert("QScriptValue(engine, qSNaN())", 0);
        toInteger.insert("QScriptValue(engine, qQNaN())", 0);
        toInteger.insert("QScriptValue(engine, qInf())", qInf());
        toInteger.insert("QScriptValue(engine, -qInf())", qInf());
        toInteger.insert("QScriptValue(engine, \"NaN\")", 0);
        toInteger.insert("QScriptValue(engine, \"Infinity\")", qInf());
        toInteger.insert("QScriptValue(engine, \"-Infinity\")", qInf());
        toInteger.insert("QScriptValue(engine, \"ciao\")", 0);
        toInteger.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        toInteger.insert("QScriptValue(engine, QString(\"\"))", 0);
        toInteger.insert("QScriptValue(engine, QString())", 0);
        toInteger.insert("QScriptValue(engine, QString(\"0\"))", 0);
        toInteger.insert("QScriptValue(engine, QString(\"123\"))", 123);
        toInteger.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        toInteger.insert("engine->evaluate(\"[]\")", 0);
        toInteger.insert("engine->evaluate(\"{}\")", 0);
        toInteger.insert("engine->evaluate(\"Object.prototype\")", 0);
        toInteger.insert("engine->evaluate(\"Date.prototype\")", 0);
        toInteger.insert("engine->evaluate(\"Array.prototype\")", 0);
        toInteger.insert("engine->evaluate(\"Function.prototype\")", 0);
        toInteger.insert("engine->evaluate(\"Error.prototype\")", 0);
        toInteger.insert("engine->evaluate(\"Object\")", 0);
        toInteger.insert("engine->evaluate(\"Array\")", 0);
        toInteger.insert("engine->evaluate(\"Number\")", 0);
        toInteger.insert("engine->evaluate(\"Function\")", 0);
        toInteger.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        toInteger.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        toInteger.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        toInteger.insert("engine->evaluate(\"/foo/\")", 0);
        toInteger.insert("engine->evaluate(\"new Object()\")", 0);
        toInteger.insert("engine->evaluate(\"new Array()\")", 0);
        toInteger.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << toInteger.value(expr);
}

void tst_QScriptValue::toInteger_test(const char*, const QScriptValue& value)
{
    QFETCH(qsreal, expected);
    if (qIsInf(expected)) {
        QVERIFY(qIsInf(value.toInteger()));
        return;
    }
    QCOMPARE(value.toInteger(), expected);
}

DEFINE_TEST_FUNCTION(toInteger)


void tst_QScriptValue::toInt32_initData()
{
    QTest::addColumn<qint32>("expected");
    initScriptValues();
}

void tst_QScriptValue::toInt32_makeData(const char* expr)
{
    static QHash<QString, qint32> toInt32;
    if (toInt32.isEmpty()) {
        toInt32.insert("QScriptValue()", 0);
        toInt32.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        toInt32.insert("QScriptValue(QScriptValue::NullValue)", 0);
        toInt32.insert("QScriptValue(true)", 1);
        toInt32.insert("QScriptValue(false)", 0);
        toInt32.insert("QScriptValue(int(122))", 122);
        toInt32.insert("QScriptValue(uint(124))", 124);
        toInt32.insert("QScriptValue(0)", 0);
        toInt32.insert("QScriptValue(0.0)", 0);
        toInt32.insert("QScriptValue(123.0)", 123);
        toInt32.insert("QScriptValue(6.37e-8)", 0);
        toInt32.insert("QScriptValue(-6.37e-8)", 0);
        toInt32.insert("QScriptValue(0x43211234)", 1126240820);
        toInt32.insert("QScriptValue(0x10000)", 65536);
        toInt32.insert("QScriptValue(0x10001)", 65537);
        toInt32.insert("QScriptValue(qSNaN())", 0);
        toInt32.insert("QScriptValue(qQNaN())", 0);
        toInt32.insert("QScriptValue(qInf())", 0);
        toInt32.insert("QScriptValue(-qInf())", 0);
        toInt32.insert("QScriptValue(\"NaN\")", 0);
        toInt32.insert("QScriptValue(\"Infinity\")", 0);
        toInt32.insert("QScriptValue(\"-Infinity\")", 0);
        toInt32.insert("QScriptValue(\"ciao\")", 0);
        toInt32.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        toInt32.insert("QScriptValue(QString(\"\"))", 0);
        toInt32.insert("QScriptValue(QString())", 0);
        toInt32.insert("QScriptValue(QString(\"0\"))", 0);
        toInt32.insert("QScriptValue(QString(\"123\"))", 123);
        toInt32.insert("QScriptValue(QString(\"12.4\"))", 12);
        toInt32.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        toInt32.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        toInt32.insert("QScriptValue(0, true)", 1);
        toInt32.insert("QScriptValue(0, false)", 0);
        toInt32.insert("QScriptValue(0, int(122))", 122);
        toInt32.insert("QScriptValue(0, uint(124))", 124);
        toInt32.insert("QScriptValue(0, 0)", 0);
        toInt32.insert("QScriptValue(0, 0.0)", 0);
        toInt32.insert("QScriptValue(0, 123.0)", 123);
        toInt32.insert("QScriptValue(0, 6.37e-8)", 0);
        toInt32.insert("QScriptValue(0, -6.37e-8)", 0);
        toInt32.insert("QScriptValue(0, 0x43211234)", 1126240820);
        toInt32.insert("QScriptValue(0, 0x10000)", 65536);
        toInt32.insert("QScriptValue(0, 0x10001)", 65537);
        toInt32.insert("QScriptValue(0, qSNaN())", 0);
        toInt32.insert("QScriptValue(0, qQNaN())", 0);
        toInt32.insert("QScriptValue(0, qInf())", 0);
        toInt32.insert("QScriptValue(0, -qInf())", 0);
        toInt32.insert("QScriptValue(0, \"NaN\")", 0);
        toInt32.insert("QScriptValue(0, \"Infinity\")", 0);
        toInt32.insert("QScriptValue(0, \"-Infinity\")", 0);
        toInt32.insert("QScriptValue(0, \"ciao\")", 0);
        toInt32.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        toInt32.insert("QScriptValue(0, QString(\"\"))", 0);
        toInt32.insert("QScriptValue(0, QString())", 0);
        toInt32.insert("QScriptValue(0, QString(\"0\"))", 0);
        toInt32.insert("QScriptValue(0, QString(\"123\"))", 123);
        toInt32.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        toInt32.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        toInt32.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        toInt32.insert("QScriptValue(engine, true)", 1);
        toInt32.insert("QScriptValue(engine, false)", 0);
        toInt32.insert("QScriptValue(engine, int(122))", 122);
        toInt32.insert("QScriptValue(engine, uint(124))", 124);
        toInt32.insert("QScriptValue(engine, 0)", 0);
        toInt32.insert("QScriptValue(engine, 0.0)", 0);
        toInt32.insert("QScriptValue(engine, 123.0)", 123);
        toInt32.insert("QScriptValue(engine, 6.37e-8)", 0);
        toInt32.insert("QScriptValue(engine, -6.37e-8)", 0);
        toInt32.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        toInt32.insert("QScriptValue(engine, 0x10000)", 65536);
        toInt32.insert("QScriptValue(engine, 0x10001)", 65537);
        toInt32.insert("QScriptValue(engine, qSNaN())", 0);
        toInt32.insert("QScriptValue(engine, qQNaN())", 0);
        toInt32.insert("QScriptValue(engine, qInf())", 0);
        toInt32.insert("QScriptValue(engine, -qInf())", 0);
        toInt32.insert("QScriptValue(engine, \"NaN\")", 0);
        toInt32.insert("QScriptValue(engine, \"Infinity\")", 0);
        toInt32.insert("QScriptValue(engine, \"-Infinity\")", 0);
        toInt32.insert("QScriptValue(engine, \"ciao\")", 0);
        toInt32.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        toInt32.insert("QScriptValue(engine, QString(\"\"))", 0);
        toInt32.insert("QScriptValue(engine, QString())", 0);
        toInt32.insert("QScriptValue(engine, QString(\"0\"))", 0);
        toInt32.insert("QScriptValue(engine, QString(\"123\"))", 123);
        toInt32.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        toInt32.insert("engine->evaluate(\"[]\")", 0);
        toInt32.insert("engine->evaluate(\"{}\")", 0);
        toInt32.insert("engine->evaluate(\"Object.prototype\")", 0);
        toInt32.insert("engine->evaluate(\"Date.prototype\")", 0);
        toInt32.insert("engine->evaluate(\"Array.prototype\")", 0);
        toInt32.insert("engine->evaluate(\"Function.prototype\")", 0);
        toInt32.insert("engine->evaluate(\"Error.prototype\")", 0);
        toInt32.insert("engine->evaluate(\"Object\")", 0);
        toInt32.insert("engine->evaluate(\"Array\")", 0);
        toInt32.insert("engine->evaluate(\"Number\")", 0);
        toInt32.insert("engine->evaluate(\"Function\")", 0);
        toInt32.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        toInt32.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        toInt32.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        toInt32.insert("engine->evaluate(\"/foo/\")", 0);
        toInt32.insert("engine->evaluate(\"new Object()\")", 0);
        toInt32.insert("engine->evaluate(\"new Array()\")", 0);
        toInt32.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << toInt32.value(expr);
}

void tst_QScriptValue::toInt32_test(const char*, const QScriptValue& value)
{
    QFETCH(qint32, expected);
    QCOMPARE(value.toInt32(), expected);
}

DEFINE_TEST_FUNCTION(toInt32)


void tst_QScriptValue::toUInt32_initData()
{
    QTest::addColumn<quint32>("expected");
    initScriptValues();
}

void tst_QScriptValue::toUInt32_makeData(const char* expr)
{
    static QHash<QString, quint32> toUInt32;
    if (toUInt32.isEmpty()) {
        toUInt32.insert("QScriptValue()", 0);
        toUInt32.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        toUInt32.insert("QScriptValue(QScriptValue::NullValue)", 0);
        toUInt32.insert("QScriptValue(true)", 1);
        toUInt32.insert("QScriptValue(false)", 0);
        toUInt32.insert("QScriptValue(int(122))", 122);
        toUInt32.insert("QScriptValue(uint(124))", 124);
        toUInt32.insert("QScriptValue(0)", 0);
        toUInt32.insert("QScriptValue(0.0)", 0);
        toUInt32.insert("QScriptValue(123.0)", 123);
        toUInt32.insert("QScriptValue(6.37e-8)", 0);
        toUInt32.insert("QScriptValue(-6.37e-8)", 0);
        toUInt32.insert("QScriptValue(0x43211234)", 1126240820);
        toUInt32.insert("QScriptValue(0x10000)", 65536);
        toUInt32.insert("QScriptValue(0x10001)", 65537);
        toUInt32.insert("QScriptValue(qSNaN())", 0);
        toUInt32.insert("QScriptValue(qQNaN())", 0);
        toUInt32.insert("QScriptValue(qInf())", 0);
        toUInt32.insert("QScriptValue(-qInf())", 0);
        toUInt32.insert("QScriptValue(\"NaN\")", 0);
        toUInt32.insert("QScriptValue(\"Infinity\")", 0);
        toUInt32.insert("QScriptValue(\"-Infinity\")", 0);
        toUInt32.insert("QScriptValue(\"ciao\")", 0);
        toUInt32.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        toUInt32.insert("QScriptValue(QString(\"\"))", 0);
        toUInt32.insert("QScriptValue(QString())", 0);
        toUInt32.insert("QScriptValue(QString(\"0\"))", 0);
        toUInt32.insert("QScriptValue(QString(\"123\"))", 123);
        toUInt32.insert("QScriptValue(QString(\"12.4\"))", 12);
        toUInt32.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        toUInt32.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        toUInt32.insert("QScriptValue(0, true)", 1);
        toUInt32.insert("QScriptValue(0, false)", 0);
        toUInt32.insert("QScriptValue(0, int(122))", 122);
        toUInt32.insert("QScriptValue(0, uint(124))", 124);
        toUInt32.insert("QScriptValue(0, 0)", 0);
        toUInt32.insert("QScriptValue(0, 0.0)", 0);
        toUInt32.insert("QScriptValue(0, 123.0)", 123);
        toUInt32.insert("QScriptValue(0, 6.37e-8)", 0);
        toUInt32.insert("QScriptValue(0, -6.37e-8)", 0);
        toUInt32.insert("QScriptValue(0, 0x43211234)", 1126240820);
        toUInt32.insert("QScriptValue(0, 0x10000)", 65536);
        toUInt32.insert("QScriptValue(0, 0x10001)", 65537);
        toUInt32.insert("QScriptValue(0, qSNaN())", 0);
        toUInt32.insert("QScriptValue(0, qQNaN())", 0);
        toUInt32.insert("QScriptValue(0, qInf())", 0);
        toUInt32.insert("QScriptValue(0, -qInf())", 0);
        toUInt32.insert("QScriptValue(0, \"NaN\")", 0);
        toUInt32.insert("QScriptValue(0, \"Infinity\")", 0);
        toUInt32.insert("QScriptValue(0, \"-Infinity\")", 0);
        toUInt32.insert("QScriptValue(0, \"ciao\")", 0);
        toUInt32.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        toUInt32.insert("QScriptValue(0, QString(\"\"))", 0);
        toUInt32.insert("QScriptValue(0, QString())", 0);
        toUInt32.insert("QScriptValue(0, QString(\"0\"))", 0);
        toUInt32.insert("QScriptValue(0, QString(\"123\"))", 123);
        toUInt32.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        toUInt32.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        toUInt32.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        toUInt32.insert("QScriptValue(engine, true)", 1);
        toUInt32.insert("QScriptValue(engine, false)", 0);
        toUInt32.insert("QScriptValue(engine, int(122))", 122);
        toUInt32.insert("QScriptValue(engine, uint(124))", 124);
        toUInt32.insert("QScriptValue(engine, 0)", 0);
        toUInt32.insert("QScriptValue(engine, 0.0)", 0);
        toUInt32.insert("QScriptValue(engine, 123.0)", 123);
        toUInt32.insert("QScriptValue(engine, 6.37e-8)", 0);
        toUInt32.insert("QScriptValue(engine, -6.37e-8)", 0);
        toUInt32.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        toUInt32.insert("QScriptValue(engine, 0x10000)", 65536);
        toUInt32.insert("QScriptValue(engine, 0x10001)", 65537);
        toUInt32.insert("QScriptValue(engine, qSNaN())", 0);
        toUInt32.insert("QScriptValue(engine, qQNaN())", 0);
        toUInt32.insert("QScriptValue(engine, qInf())", 0);
        toUInt32.insert("QScriptValue(engine, -qInf())", 0);
        toUInt32.insert("QScriptValue(engine, \"NaN\")", 0);
        toUInt32.insert("QScriptValue(engine, \"Infinity\")", 0);
        toUInt32.insert("QScriptValue(engine, \"-Infinity\")", 0);
        toUInt32.insert("QScriptValue(engine, \"ciao\")", 0);
        toUInt32.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        toUInt32.insert("QScriptValue(engine, QString(\"\"))", 0);
        toUInt32.insert("QScriptValue(engine, QString())", 0);
        toUInt32.insert("QScriptValue(engine, QString(\"0\"))", 0);
        toUInt32.insert("QScriptValue(engine, QString(\"123\"))", 123);
        toUInt32.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        toUInt32.insert("engine->evaluate(\"[]\")", 0);
        toUInt32.insert("engine->evaluate(\"{}\")", 0);
        toUInt32.insert("engine->evaluate(\"Object.prototype\")", 0);
        toUInt32.insert("engine->evaluate(\"Date.prototype\")", 0);
        toUInt32.insert("engine->evaluate(\"Array.prototype\")", 0);
        toUInt32.insert("engine->evaluate(\"Function.prototype\")", 0);
        toUInt32.insert("engine->evaluate(\"Error.prototype\")", 0);
        toUInt32.insert("engine->evaluate(\"Object\")", 0);
        toUInt32.insert("engine->evaluate(\"Array\")", 0);
        toUInt32.insert("engine->evaluate(\"Number\")", 0);
        toUInt32.insert("engine->evaluate(\"Function\")", 0);
        toUInt32.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        toUInt32.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        toUInt32.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        toUInt32.insert("engine->evaluate(\"/foo/\")", 0);
        toUInt32.insert("engine->evaluate(\"new Object()\")", 0);
        toUInt32.insert("engine->evaluate(\"new Array()\")", 0);
        toUInt32.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << toUInt32.value(expr);
}

void tst_QScriptValue::toUInt32_test(const char*, const QScriptValue& value)
{
    QFETCH(quint32, expected);
    QCOMPARE(value.toUInt32(), expected);
}

DEFINE_TEST_FUNCTION(toUInt32)


void tst_QScriptValue::toUInt16_initData()
{
    QTest::addColumn<quint16>("expected");
    initScriptValues();
}

void tst_QScriptValue::toUInt16_makeData(const char* expr)
{
    static QHash<QString, quint16> toUInt16;
    if (toUInt16.isEmpty()) {
        toUInt16.insert("QScriptValue()", 0);
        toUInt16.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        toUInt16.insert("QScriptValue(QScriptValue::NullValue)", 0);
        toUInt16.insert("QScriptValue(true)", 1);
        toUInt16.insert("QScriptValue(false)", 0);
        toUInt16.insert("QScriptValue(int(122))", 122);
        toUInt16.insert("QScriptValue(uint(124))", 124);
        toUInt16.insert("QScriptValue(0)", 0);
        toUInt16.insert("QScriptValue(0.0)", 0);
        toUInt16.insert("QScriptValue(123.0)", 123);
        toUInt16.insert("QScriptValue(6.37e-8)", 0);
        toUInt16.insert("QScriptValue(-6.37e-8)", 0);
        toUInt16.insert("QScriptValue(0x43211234)", 4660);
        toUInt16.insert("QScriptValue(0x10000)", 0);
        toUInt16.insert("QScriptValue(0x10001)", 1);
        toUInt16.insert("QScriptValue(qSNaN())", 0);
        toUInt16.insert("QScriptValue(qQNaN())", 0);
        toUInt16.insert("QScriptValue(qInf())", 0);
        toUInt16.insert("QScriptValue(-qInf())", 0);
        toUInt16.insert("QScriptValue(\"NaN\")", 0);
        toUInt16.insert("QScriptValue(\"Infinity\")", 0);
        toUInt16.insert("QScriptValue(\"-Infinity\")", 0);
        toUInt16.insert("QScriptValue(\"ciao\")", 0);
        toUInt16.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        toUInt16.insert("QScriptValue(QString(\"\"))", 0);
        toUInt16.insert("QScriptValue(QString())", 0);
        toUInt16.insert("QScriptValue(QString(\"0\"))", 0);
        toUInt16.insert("QScriptValue(QString(\"123\"))", 123);
        toUInt16.insert("QScriptValue(QString(\"12.4\"))", 12);
        toUInt16.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        toUInt16.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        toUInt16.insert("QScriptValue(0, true)", 1);
        toUInt16.insert("QScriptValue(0, false)", 0);
        toUInt16.insert("QScriptValue(0, int(122))", 122);
        toUInt16.insert("QScriptValue(0, uint(124))", 124);
        toUInt16.insert("QScriptValue(0, 0)", 0);
        toUInt16.insert("QScriptValue(0, 0.0)", 0);
        toUInt16.insert("QScriptValue(0, 123.0)", 123);
        toUInt16.insert("QScriptValue(0, 6.37e-8)", 0);
        toUInt16.insert("QScriptValue(0, -6.37e-8)", 0);
        toUInt16.insert("QScriptValue(0, 0x43211234)", 4660);
        toUInt16.insert("QScriptValue(0, 0x10000)", 0);
        toUInt16.insert("QScriptValue(0, 0x10001)", 1);
        toUInt16.insert("QScriptValue(0, qSNaN())", 0);
        toUInt16.insert("QScriptValue(0, qQNaN())", 0);
        toUInt16.insert("QScriptValue(0, qInf())", 0);
        toUInt16.insert("QScriptValue(0, -qInf())", 0);
        toUInt16.insert("QScriptValue(0, \"NaN\")", 0);
        toUInt16.insert("QScriptValue(0, \"Infinity\")", 0);
        toUInt16.insert("QScriptValue(0, \"-Infinity\")", 0);
        toUInt16.insert("QScriptValue(0, \"ciao\")", 0);
        toUInt16.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        toUInt16.insert("QScriptValue(0, QString(\"\"))", 0);
        toUInt16.insert("QScriptValue(0, QString())", 0);
        toUInt16.insert("QScriptValue(0, QString(\"0\"))", 0);
        toUInt16.insert("QScriptValue(0, QString(\"123\"))", 123);
        toUInt16.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        toUInt16.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        toUInt16.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        toUInt16.insert("QScriptValue(engine, true)", 1);
        toUInt16.insert("QScriptValue(engine, false)", 0);
        toUInt16.insert("QScriptValue(engine, int(122))", 122);
        toUInt16.insert("QScriptValue(engine, uint(124))", 124);
        toUInt16.insert("QScriptValue(engine, 0)", 0);
        toUInt16.insert("QScriptValue(engine, 0.0)", 0);
        toUInt16.insert("QScriptValue(engine, 123.0)", 123);
        toUInt16.insert("QScriptValue(engine, 6.37e-8)", 0);
        toUInt16.insert("QScriptValue(engine, -6.37e-8)", 0);
        toUInt16.insert("QScriptValue(engine, 0x43211234)", 4660);
        toUInt16.insert("QScriptValue(engine, 0x10000)", 0);
        toUInt16.insert("QScriptValue(engine, 0x10001)", 1);
        toUInt16.insert("QScriptValue(engine, qSNaN())", 0);
        toUInt16.insert("QScriptValue(engine, qQNaN())", 0);
        toUInt16.insert("QScriptValue(engine, qInf())", 0);
        toUInt16.insert("QScriptValue(engine, -qInf())", 0);
        toUInt16.insert("QScriptValue(engine, \"NaN\")", 0);
        toUInt16.insert("QScriptValue(engine, \"Infinity\")", 0);
        toUInt16.insert("QScriptValue(engine, \"-Infinity\")", 0);
        toUInt16.insert("QScriptValue(engine, \"ciao\")", 0);
        toUInt16.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        toUInt16.insert("QScriptValue(engine, QString(\"\"))", 0);
        toUInt16.insert("QScriptValue(engine, QString())", 0);
        toUInt16.insert("QScriptValue(engine, QString(\"0\"))", 0);
        toUInt16.insert("QScriptValue(engine, QString(\"123\"))", 123);
        toUInt16.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        toUInt16.insert("engine->evaluate(\"[]\")", 0);
        toUInt16.insert("engine->evaluate(\"{}\")", 0);
        toUInt16.insert("engine->evaluate(\"Object.prototype\")", 0);
        toUInt16.insert("engine->evaluate(\"Date.prototype\")", 0);
        toUInt16.insert("engine->evaluate(\"Array.prototype\")", 0);
        toUInt16.insert("engine->evaluate(\"Function.prototype\")", 0);
        toUInt16.insert("engine->evaluate(\"Error.prototype\")", 0);
        toUInt16.insert("engine->evaluate(\"Object\")", 0);
        toUInt16.insert("engine->evaluate(\"Array\")", 0);
        toUInt16.insert("engine->evaluate(\"Number\")", 0);
        toUInt16.insert("engine->evaluate(\"Function\")", 0);
        toUInt16.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        toUInt16.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        toUInt16.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        toUInt16.insert("engine->evaluate(\"/foo/\")", 0);
        toUInt16.insert("engine->evaluate(\"new Object()\")", 0);
        toUInt16.insert("engine->evaluate(\"new Array()\")", 0);
        toUInt16.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << toUInt16.value(expr);
}

void tst_QScriptValue::toUInt16_test(const char*, const QScriptValue& value)
{
    QFETCH(quint16, expected);
    QCOMPARE(value.toUInt16(), expected);
}

DEFINE_TEST_FUNCTION(toUInt16)


void tst_QScriptValue::equals_initData()
{
    QTest::addColumn<QScriptValue>("other");
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::equals_makeData(const char *expr)
{
    static QSet<QString> equals;
    if (equals.isEmpty()) {
        equals.insert("QScriptValue() <=> QScriptValue()");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(false) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(false) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(false) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(false) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0.0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0.0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0.0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, false) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, false) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, false) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, 0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, 0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, 0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, 0.0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, 0.0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, 0.0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, false) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, false) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, false) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, 0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, 0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, 0.0) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, 0.0) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(false)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString(\"\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString())");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, false)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 0.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString(\"\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString())");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, false)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 0.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString())");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"[]\")");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("engine->evaluate(\"{}\") <=> engine->evaluate(\"{}\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"Date.prototype\") <=> engine->evaluate(\"Date.prototype\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(false)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString(\"\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, false)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 0.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString(\"\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, false)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 0.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"Object\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Number\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(false)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString(\"\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString())");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, false)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 0.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString(\"\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString())");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, false)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 0.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString())");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"new Array()\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"new Error()\")");
    }
    QHash<QString, QScriptValue>::const_iterator it;
    for (it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        QString tag = QString::fromLatin1("%20 <=> %21").arg(expr).arg(it.key());
        newRow(tag.toLatin1()) << it.value() << equals.contains(tag);
    }
}

void tst_QScriptValue::equals_test(const char *, const QScriptValue& value)
{
    QFETCH(QScriptValue, other);
    QFETCH(bool, expected);
    QCOMPARE(value.equals(other), expected);
}

DEFINE_TEST_FUNCTION(equals)


void tst_QScriptValue::strictlyEquals_initData()
{
    QTest::addColumn<QScriptValue>("other");
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::strictlyEquals_makeData(const char *expr)
{
    static QSet<QString> equals;
    if (equals.isEmpty()) {
        equals.insert("QScriptValue() <=> QScriptValue()");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(\"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(0, QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("QScriptValue(engine, QScriptValue::UndefinedValue) <=> engine->evaluate(\"{}\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(-qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, -qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, -qInf())");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"[]\")");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(0, QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> QScriptValue(engine, QScriptValue::UndefinedValue)");
        equals.insert("engine->evaluate(\"{}\") <=> engine->evaluate(\"{}\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"Date.prototype\") <=> engine->evaluate(\"Date.prototype\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"Object\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Number\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"new Array()\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"new Error()\")");
    }
    QHash<QString, QScriptValue>::const_iterator it;
    for (it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        QString tag = QString::fromLatin1("%20 <=> %21").arg(expr).arg(it.key());
        newRow(tag.toLatin1()) << it.value() << equals.contains(tag);
    }
}

void tst_QScriptValue::strictlyEquals_test(const char *, const QScriptValue& value)
{
    QFETCH(QScriptValue, other);
    QFETCH(bool, expected);
    QCOMPARE(value.strictlyEquals(other), expected);
}

DEFINE_TEST_FUNCTION(strictlyEquals)


void tst_QScriptValue::lessThan_initData()
{
    QTest::addColumn<QScriptValue>("other");
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::lessThan_makeData(const char *expr)
{
    static QSet<QString> equals;
    if (equals.isEmpty()) {
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(true)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(true) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(true) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(true) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(true) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(true) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(true) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(true)");
        equals.insert("QScriptValue(false) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(false) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(false) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(false) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(false) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(false) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(false) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(int(122)) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(uint(124)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0.0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(123.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(false)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(-6.37e-8) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(-6.37e-8) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(-6.37e-8) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0x43211234) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0x10000) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0x10001) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(-qInf()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(-qInf()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(-qInf()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(-qInf()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(\"NaN\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(\"NaN\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(\"Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(\"Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(true)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(false)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(\"-Infinity\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(\"ciao\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(QString(\"\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(QString()) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(QString(\"0\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(QString(\"123\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(QString(\"12.4\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, true) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, false) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, int(122)) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, uint(124)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, 0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, 0.0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 123.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, 6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, -6.37e-8) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, -6.37e-8) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, -6.37e-8) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 0x43211234) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 0x10000) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, 0x10001) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, -qInf()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(0, -qInf()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(0, -qInf()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, \"NaN\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, \"Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(false)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, \"-Infinity\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, \"ciao\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, QString(\"\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, QString()) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, QString(\"0\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, QString(\"123\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(0, QString(\"12.3\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QScriptValue::NullValue) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, true) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, false) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, int(122)) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, uint(124)) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 0.0) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 123.0) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, 6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, -6.37e-8) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 0x43211234) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 0x10000) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, 0x10001) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QString(\"\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QString())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QString(\"\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QString())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QString(\"\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QString())");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, -qInf()) <=> engine->evaluate(\"[]\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> engine->evaluate(\"Array.prototype\")");
        equals.insert("QScriptValue(engine, -qInf()) <=> engine->evaluate(\"new Array()\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, \"NaN\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, \"Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(false)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(-6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, false)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 0.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, -6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, QScriptValue::NullValue)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, false)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 0.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, -6.37e-8)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, \"-Infinity\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, \"ciao\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, QString(\"\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(\"-Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString()) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"/foo/\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, QString()) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(true)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, true)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, true)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, QString(\"0\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, QString(\"123\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(int(122))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(uint(124))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(123.0)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0x10000)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0x10001)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(qInf())");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(\"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(\"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(\"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, int(122))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, qInf())");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, int(122))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, uint(124))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, 123.0)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, 0x43211234)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, 0x10000)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, 0x10001)");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, qInf())");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, \"NaN\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, \"ciao\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Object\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Array\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Number\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"Function\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"new Object()\")");
        equals.insert("QScriptValue(engine, QString(\"1.23\")) <=> engine->evaluate(\"new Error()\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(true)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(int(122))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(uint(124))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(123.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(6.37e-8)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0x43211234)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0x10000)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0x10001)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(qInf())");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, true)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, int(122))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, uint(124))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 123.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 0x10000)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, 0x10001)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, qInf())");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, true)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, int(122))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, uint(124))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 123.0)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, qInf())");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("engine->evaluate(\"[]\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"Object.prototype\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(true)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(int(122))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(uint(124))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(123.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(6.37e-8)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0x43211234)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0x10000)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0x10001)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(qInf())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, true)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, int(122))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, uint(124))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 123.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 0x10000)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, 0x10001)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, qInf())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, true)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, int(122))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, uint(124))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 123.0)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, qInf())");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Number\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(true)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(int(122))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(uint(124))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(123.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(6.37e-8)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0x43211234)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0x10000)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0x10001)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(qInf())");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(\"-Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString(\"0\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString(\"123\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(QString(\"12.4\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, true)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, int(122))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, uint(124))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 123.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 6.37e-8)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 0x43211234)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 0x10000)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, 0x10001)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, qInf())");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, \"-Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString(\"0\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString(\"123\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(0, QString(\"12.3\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, true)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, int(122))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, uint(124))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 123.0)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 6.37e-8)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 0x43211234)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 0x10000)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, 0x10001)");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, qInf())");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, \"-Infinity\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString(\"0\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString(\"123\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> QScriptValue(engine, QString(\"1.23\"))");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Error.prototype\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"/foo/\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"new Object()\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"new Error()\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(\"NaN\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(\"Infinity\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(\"ciao\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(0, \"NaN\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(0, \"Infinity\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(0, \"ciao\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(0, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(engine, \"NaN\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(engine, \"Infinity\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(engine, \"ciao\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> QScriptValue(engine, QString::fromLatin1(\"ciao\"))");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Object.prototype\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Function.prototype\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Number\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"(function() { return 1; })\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"(function() { return 'ciao'; })\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"(function() { throw new Error('foo'); })\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"new Object()\")");
    }
    QHash<QString, QScriptValue>::const_iterator it;
    for (it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        QString tag = QString::fromLatin1("%20 <=> %21").arg(expr).arg(it.key());
        newRow(tag.toLatin1()) << it.value() << equals.contains(tag);
    }
}

void tst_QScriptValue::lessThan_test(const char *, const QScriptValue& value)
{
    QFETCH(QScriptValue, other);
    QFETCH(bool, expected);
    QCOMPARE(value.lessThan(other), expected);
}

DEFINE_TEST_FUNCTION(lessThan)


void tst_QScriptValue::instanceOf_initData()
{
    QTest::addColumn<QScriptValue>("other");
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::instanceOf_makeData(const char *expr)
{
    static QSet<QString> equals;
    if (equals.isEmpty()) {
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"[]\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"Date.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Function.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Error.prototype\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Object\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Object\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Array\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Number\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Number\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"Function\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { return 1; })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { return 'ciao'; })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\") <=> engine->evaluate(\"Function\")");
        equals.insert("engine->evaluate(\"/foo/\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Object()\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Object\")");
        equals.insert("engine->evaluate(\"new Array()\") <=> engine->evaluate(\"Array\")");
        equals.insert("engine->evaluate(\"new Error()\") <=> engine->evaluate(\"Object\")");
    }
    QHash<QString, QScriptValue>::const_iterator it;
    for (it = m_values.constBegin(); it != m_values.constEnd(); ++it) {
        QString tag = QString::fromLatin1("%20 <=> %21").arg(expr).arg(it.key());
        newRow(tag.toLatin1()) << it.value() << equals.contains(tag);
    }
}

void tst_QScriptValue::instanceOf_test(const char *, const QScriptValue& value)
{
    QFETCH(QScriptValue, other);
    QFETCH(bool, expected);
    QCOMPARE(value.instanceOf(other), expected);
}

DEFINE_TEST_FUNCTION(instanceOf)


void tst_QScriptValue::qscriptvalue_castQString_initData()
{
    QTest::addColumn<QString>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castQString_makeData(const char* expr)
{
    static QHash<QString, QString> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", "");
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", "");
        value.insert("QScriptValue(QScriptValue::NullValue)", "");
        value.insert("QScriptValue(true)", "true");
        value.insert("QScriptValue(false)", "false");
        value.insert("QScriptValue(int(122))", "122");
        value.insert("QScriptValue(uint(124))", "124");
        value.insert("QScriptValue(0)", "0");
        value.insert("QScriptValue(0.0)", "0");
        value.insert("QScriptValue(123.0)", "123");
        value.insert("QScriptValue(6.37e-8)", "6.37e-8");
        value.insert("QScriptValue(-6.37e-8)", "-6.37e-8");
        value.insert("QScriptValue(0x43211234)", "1126240820");
        value.insert("QScriptValue(0x10000)", "65536");
        value.insert("QScriptValue(0x10001)", "65537");
        value.insert("QScriptValue(qSNaN())", "NaN");
        value.insert("QScriptValue(qQNaN())", "NaN");
        value.insert("QScriptValue(qInf())", "Infinity");
        value.insert("QScriptValue(-qInf())", "-Infinity");
        value.insert("QScriptValue(\"NaN\")", "NaN");
        value.insert("QScriptValue(\"Infinity\")", "Infinity");
        value.insert("QScriptValue(\"-Infinity\")", "-Infinity");
        value.insert("QScriptValue(\"ciao\")", "ciao");
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", "ciao");
        value.insert("QScriptValue(QString(\"\"))", "");
        value.insert("QScriptValue(QString())", "");
        value.insert("QScriptValue(QString(\"0\"))", "0");
        value.insert("QScriptValue(QString(\"123\"))", "123");
        value.insert("QScriptValue(QString(\"12.4\"))", "12.4");
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", "");
        value.insert("QScriptValue(0, QScriptValue::NullValue)", "");
        value.insert("QScriptValue(0, true)", "true");
        value.insert("QScriptValue(0, false)", "false");
        value.insert("QScriptValue(0, int(122))", "122");
        value.insert("QScriptValue(0, uint(124))", "124");
        value.insert("QScriptValue(0, 0)", "0");
        value.insert("QScriptValue(0, 0.0)", "0");
        value.insert("QScriptValue(0, 123.0)", "123");
        value.insert("QScriptValue(0, 6.37e-8)", "6.37e-8");
        value.insert("QScriptValue(0, -6.37e-8)", "-6.37e-8");
        value.insert("QScriptValue(0, 0x43211234)", "1126240820");
        value.insert("QScriptValue(0, 0x10000)", "65536");
        value.insert("QScriptValue(0, 0x10001)", "65537");
        value.insert("QScriptValue(0, qSNaN())", "NaN");
        value.insert("QScriptValue(0, qQNaN())", "NaN");
        value.insert("QScriptValue(0, qInf())", "Infinity");
        value.insert("QScriptValue(0, -qInf())", "-Infinity");
        value.insert("QScriptValue(0, \"NaN\")", "NaN");
        value.insert("QScriptValue(0, \"Infinity\")", "Infinity");
        value.insert("QScriptValue(0, \"-Infinity\")", "-Infinity");
        value.insert("QScriptValue(0, \"ciao\")", "ciao");
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", "ciao");
        value.insert("QScriptValue(0, QString(\"\"))", "");
        value.insert("QScriptValue(0, QString())", "");
        value.insert("QScriptValue(0, QString(\"0\"))", "0");
        value.insert("QScriptValue(0, QString(\"123\"))", "123");
        value.insert("QScriptValue(0, QString(\"12.3\"))", "12.3");
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", "");
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", "");
        value.insert("QScriptValue(engine, true)", "true");
        value.insert("QScriptValue(engine, false)", "false");
        value.insert("QScriptValue(engine, int(122))", "122");
        value.insert("QScriptValue(engine, uint(124))", "124");
        value.insert("QScriptValue(engine, 0)", "0");
        value.insert("QScriptValue(engine, 0.0)", "0");
        value.insert("QScriptValue(engine, 123.0)", "123");
        value.insert("QScriptValue(engine, 6.37e-8)", "6.37e-8");
        value.insert("QScriptValue(engine, -6.37e-8)", "-6.37e-8");
        value.insert("QScriptValue(engine, 0x43211234)", "1126240820");
        value.insert("QScriptValue(engine, 0x10000)", "65536");
        value.insert("QScriptValue(engine, 0x10001)", "65537");
        value.insert("QScriptValue(engine, qSNaN())", "NaN");
        value.insert("QScriptValue(engine, qQNaN())", "NaN");
        value.insert("QScriptValue(engine, qInf())", "Infinity");
        value.insert("QScriptValue(engine, -qInf())", "-Infinity");
        value.insert("QScriptValue(engine, \"NaN\")", "NaN");
        value.insert("QScriptValue(engine, \"Infinity\")", "Infinity");
        value.insert("QScriptValue(engine, \"-Infinity\")", "-Infinity");
        value.insert("QScriptValue(engine, \"ciao\")", "ciao");
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", "ciao");
        value.insert("QScriptValue(engine, QString(\"\"))", "");
        value.insert("QScriptValue(engine, QString())", "");
        value.insert("QScriptValue(engine, QString(\"0\"))", "0");
        value.insert("QScriptValue(engine, QString(\"123\"))", "123");
        value.insert("QScriptValue(engine, QString(\"1.23\"))", "1.23");
        value.insert("engine->evaluate(\"[]\")", "");
        value.insert("engine->evaluate(\"{}\")", "");
        value.insert("engine->evaluate(\"Object.prototype\")", "[object Object]");
        value.insert("engine->evaluate(\"Date.prototype\")", "Invalid Date");
        value.insert("engine->evaluate(\"Array.prototype\")", "");
        value.insert("engine->evaluate(\"Function.prototype\")", "function () {\n    [native code]\n}");
        value.insert("engine->evaluate(\"Error.prototype\")", "Error: Unknown error");
        value.insert("engine->evaluate(\"Object\")", "function Object() {\n    [native code]\n}");
        value.insert("engine->evaluate(\"Array\")", "function Array() {\n    [native code]\n}");
        value.insert("engine->evaluate(\"Number\")", "function Number() {\n    [native code]\n}");
        value.insert("engine->evaluate(\"Function\")", "function Function() {\n    [native code]\n}");
        value.insert("engine->evaluate(\"(function() { return 1; })\")", "function () { return 1; }");
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", "function () { return 'ciao'; }");
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", "function () { throw new Error('foo'); }");
        value.insert("engine->evaluate(\"/foo/\")", "/foo/");
        value.insert("engine->evaluate(\"new Object()\")", "[object Object]");
        value.insert("engine->evaluate(\"new Array()\")", "");
        value.insert("engine->evaluate(\"new Error()\")", "Error: Unknown error");
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castQString_test(const char*, const QScriptValue& value)
{
    QFETCH(QString, expected);
    QCOMPARE(qscriptvalue_cast<QString>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castQString)


void tst_QScriptValue::qscriptvalue_castqsreal_initData()
{
    QTest::addColumn<qsreal>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castqsreal_makeData(const char* expr)
{
    static QHash<QString, qsreal> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", 0);
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", qQNaN());
        value.insert("QScriptValue(QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(true)", 1);
        value.insert("QScriptValue(false)", 0);
        value.insert("QScriptValue(int(122))", 122);
        value.insert("QScriptValue(uint(124))", 124);
        value.insert("QScriptValue(0)", 0);
        value.insert("QScriptValue(0.0)", 0);
        value.insert("QScriptValue(123.0)", 123);
        value.insert("QScriptValue(6.37e-8)", 6.369999999999999e-08);
        value.insert("QScriptValue(-6.37e-8)", -6.369999999999999e-08);
        value.insert("QScriptValue(0x43211234)", 1126240820);
        value.insert("QScriptValue(0x10000)", 65536);
        value.insert("QScriptValue(0x10001)", 65537);
        value.insert("QScriptValue(qSNaN())", qQNaN());
        value.insert("QScriptValue(qQNaN())", qQNaN());
        value.insert("QScriptValue(qInf())", qInf());
        value.insert("QScriptValue(-qInf())", qInf());
        value.insert("QScriptValue(\"NaN\")", qQNaN());
        value.insert("QScriptValue(\"Infinity\")", qInf());
        value.insert("QScriptValue(\"-Infinity\")", qInf());
        value.insert("QScriptValue(\"ciao\")", qQNaN());
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", qQNaN());
        value.insert("QScriptValue(QString(\"\"))", 0);
        value.insert("QScriptValue(QString())", 0);
        value.insert("QScriptValue(QString(\"0\"))", 0);
        value.insert("QScriptValue(QString(\"123\"))", 123);
        value.insert("QScriptValue(QString(\"12.4\"))", 12.4);
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", qQNaN());
        value.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(0, true)", 1);
        value.insert("QScriptValue(0, false)", 0);
        value.insert("QScriptValue(0, int(122))", 122);
        value.insert("QScriptValue(0, uint(124))", 124);
        value.insert("QScriptValue(0, 0)", 0);
        value.insert("QScriptValue(0, 0.0)", 0);
        value.insert("QScriptValue(0, 123.0)", 123);
        value.insert("QScriptValue(0, 6.37e-8)", 6.369999999999999e-08);
        value.insert("QScriptValue(0, -6.37e-8)", -6.369999999999999e-08);
        value.insert("QScriptValue(0, 0x43211234)", 1126240820);
        value.insert("QScriptValue(0, 0x10000)", 65536);
        value.insert("QScriptValue(0, 0x10001)", 65537);
        value.insert("QScriptValue(0, qSNaN())", qQNaN());
        value.insert("QScriptValue(0, qQNaN())", qQNaN());
        value.insert("QScriptValue(0, qInf())", qInf());
        value.insert("QScriptValue(0, -qInf())", qInf());
        value.insert("QScriptValue(0, \"NaN\")", qQNaN());
        value.insert("QScriptValue(0, \"Infinity\")", qInf());
        value.insert("QScriptValue(0, \"-Infinity\")", qInf());
        value.insert("QScriptValue(0, \"ciao\")", qQNaN());
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", qQNaN());
        value.insert("QScriptValue(0, QString(\"\"))", 0);
        value.insert("QScriptValue(0, QString())", 0);
        value.insert("QScriptValue(0, QString(\"0\"))", 0);
        value.insert("QScriptValue(0, QString(\"123\"))", 123);
        value.insert("QScriptValue(0, QString(\"12.3\"))", 12.3);
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", qQNaN());
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(engine, true)", 1);
        value.insert("QScriptValue(engine, false)", 0);
        value.insert("QScriptValue(engine, int(122))", 122);
        value.insert("QScriptValue(engine, uint(124))", 124);
        value.insert("QScriptValue(engine, 0)", 0);
        value.insert("QScriptValue(engine, 0.0)", 0);
        value.insert("QScriptValue(engine, 123.0)", 123);
        value.insert("QScriptValue(engine, 6.37e-8)", 6.369999999999999e-08);
        value.insert("QScriptValue(engine, -6.37e-8)", -6.369999999999999e-08);
        value.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        value.insert("QScriptValue(engine, 0x10000)", 65536);
        value.insert("QScriptValue(engine, 0x10001)", 65537);
        value.insert("QScriptValue(engine, qSNaN())", qQNaN());
        value.insert("QScriptValue(engine, qQNaN())", qQNaN());
        value.insert("QScriptValue(engine, qInf())", qInf());
        value.insert("QScriptValue(engine, -qInf())", qInf());
        value.insert("QScriptValue(engine, \"NaN\")", qQNaN());
        value.insert("QScriptValue(engine, \"Infinity\")", qInf());
        value.insert("QScriptValue(engine, \"-Infinity\")", qInf());
        value.insert("QScriptValue(engine, \"ciao\")", qQNaN());
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", qQNaN());
        value.insert("QScriptValue(engine, QString(\"\"))", 0);
        value.insert("QScriptValue(engine, QString())", 0);
        value.insert("QScriptValue(engine, QString(\"0\"))", 0);
        value.insert("QScriptValue(engine, QString(\"123\"))", 123);
        value.insert("QScriptValue(engine, QString(\"1.23\"))", 1.23);
        value.insert("engine->evaluate(\"[]\")", 0);
        value.insert("engine->evaluate(\"{}\")", qQNaN());
        value.insert("engine->evaluate(\"Object.prototype\")", qQNaN());
        value.insert("engine->evaluate(\"Date.prototype\")", qQNaN());
        value.insert("engine->evaluate(\"Array.prototype\")", 0);
        value.insert("engine->evaluate(\"Function.prototype\")", qQNaN());
        value.insert("engine->evaluate(\"Error.prototype\")", qQNaN());
        value.insert("engine->evaluate(\"Object\")", qQNaN());
        value.insert("engine->evaluate(\"Array\")", qQNaN());
        value.insert("engine->evaluate(\"Number\")", qQNaN());
        value.insert("engine->evaluate(\"Function\")", qQNaN());
        value.insert("engine->evaluate(\"(function() { return 1; })\")", qQNaN());
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", qQNaN());
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", qQNaN());
        value.insert("engine->evaluate(\"/foo/\")", qQNaN());
        value.insert("engine->evaluate(\"new Object()\")", qQNaN());
        value.insert("engine->evaluate(\"new Array()\")", 0);
        value.insert("engine->evaluate(\"new Error()\")", qQNaN());
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castqsreal_test(const char*, const QScriptValue& value)
{
    QFETCH(qsreal, expected);
    if (qIsNaN(expected)) {
        QVERIFY(qIsNaN(qscriptvalue_cast<qsreal>(value)));
        return;
    }
    if (qIsInf(expected)) {
        QVERIFY(qIsInf(qscriptvalue_cast<qsreal>(value)));
        return;
    }
    QCOMPARE(qscriptvalue_cast<qsreal>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castqsreal)


void tst_QScriptValue::qscriptvalue_castbool_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castbool_makeData(const char* expr)
{
    static QHash<QString, bool> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", false);
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", false);
        value.insert("QScriptValue(QScriptValue::NullValue)", false);
        value.insert("QScriptValue(true)", true);
        value.insert("QScriptValue(false)", false);
        value.insert("QScriptValue(int(122))", true);
        value.insert("QScriptValue(uint(124))", true);
        value.insert("QScriptValue(0)", false);
        value.insert("QScriptValue(0.0)", false);
        value.insert("QScriptValue(123.0)", true);
        value.insert("QScriptValue(6.37e-8)", true);
        value.insert("QScriptValue(-6.37e-8)", true);
        value.insert("QScriptValue(0x43211234)", true);
        value.insert("QScriptValue(0x10000)", true);
        value.insert("QScriptValue(0x10001)", true);
        value.insert("QScriptValue(qSNaN())", false);
        value.insert("QScriptValue(qQNaN())", false);
        value.insert("QScriptValue(qInf())", true);
        value.insert("QScriptValue(-qInf())", true);
        value.insert("QScriptValue(\"NaN\")", true);
        value.insert("QScriptValue(\"Infinity\")", true);
        value.insert("QScriptValue(\"-Infinity\")", true);
        value.insert("QScriptValue(\"ciao\")", true);
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", true);
        value.insert("QScriptValue(QString(\"\"))", false);
        value.insert("QScriptValue(QString())", false);
        value.insert("QScriptValue(QString(\"0\"))", true);
        value.insert("QScriptValue(QString(\"123\"))", true);
        value.insert("QScriptValue(QString(\"12.4\"))", true);
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", false);
        value.insert("QScriptValue(0, QScriptValue::NullValue)", false);
        value.insert("QScriptValue(0, true)", true);
        value.insert("QScriptValue(0, false)", false);
        value.insert("QScriptValue(0, int(122))", true);
        value.insert("QScriptValue(0, uint(124))", true);
        value.insert("QScriptValue(0, 0)", false);
        value.insert("QScriptValue(0, 0.0)", false);
        value.insert("QScriptValue(0, 123.0)", true);
        value.insert("QScriptValue(0, 6.37e-8)", true);
        value.insert("QScriptValue(0, -6.37e-8)", true);
        value.insert("QScriptValue(0, 0x43211234)", true);
        value.insert("QScriptValue(0, 0x10000)", true);
        value.insert("QScriptValue(0, 0x10001)", true);
        value.insert("QScriptValue(0, qSNaN())", false);
        value.insert("QScriptValue(0, qQNaN())", false);
        value.insert("QScriptValue(0, qInf())", true);
        value.insert("QScriptValue(0, -qInf())", true);
        value.insert("QScriptValue(0, \"NaN\")", true);
        value.insert("QScriptValue(0, \"Infinity\")", true);
        value.insert("QScriptValue(0, \"-Infinity\")", true);
        value.insert("QScriptValue(0, \"ciao\")", true);
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", true);
        value.insert("QScriptValue(0, QString(\"\"))", false);
        value.insert("QScriptValue(0, QString())", false);
        value.insert("QScriptValue(0, QString(\"0\"))", true);
        value.insert("QScriptValue(0, QString(\"123\"))", true);
        value.insert("QScriptValue(0, QString(\"12.3\"))", true);
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", false);
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", false);
        value.insert("QScriptValue(engine, true)", true);
        value.insert("QScriptValue(engine, false)", false);
        value.insert("QScriptValue(engine, int(122))", true);
        value.insert("QScriptValue(engine, uint(124))", true);
        value.insert("QScriptValue(engine, 0)", false);
        value.insert("QScriptValue(engine, 0.0)", false);
        value.insert("QScriptValue(engine, 123.0)", true);
        value.insert("QScriptValue(engine, 6.37e-8)", true);
        value.insert("QScriptValue(engine, -6.37e-8)", true);
        value.insert("QScriptValue(engine, 0x43211234)", true);
        value.insert("QScriptValue(engine, 0x10000)", true);
        value.insert("QScriptValue(engine, 0x10001)", true);
        value.insert("QScriptValue(engine, qSNaN())", false);
        value.insert("QScriptValue(engine, qQNaN())", false);
        value.insert("QScriptValue(engine, qInf())", true);
        value.insert("QScriptValue(engine, -qInf())", true);
        value.insert("QScriptValue(engine, \"NaN\")", true);
        value.insert("QScriptValue(engine, \"Infinity\")", true);
        value.insert("QScriptValue(engine, \"-Infinity\")", true);
        value.insert("QScriptValue(engine, \"ciao\")", true);
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", true);
        value.insert("QScriptValue(engine, QString(\"\"))", false);
        value.insert("QScriptValue(engine, QString())", false);
        value.insert("QScriptValue(engine, QString(\"0\"))", true);
        value.insert("QScriptValue(engine, QString(\"123\"))", true);
        value.insert("QScriptValue(engine, QString(\"1.23\"))", true);
        value.insert("engine->evaluate(\"[]\")", true);
        value.insert("engine->evaluate(\"{}\")", false);
        value.insert("engine->evaluate(\"Object.prototype\")", true);
        value.insert("engine->evaluate(\"Date.prototype\")", true);
        value.insert("engine->evaluate(\"Array.prototype\")", true);
        value.insert("engine->evaluate(\"Function.prototype\")", true);
        value.insert("engine->evaluate(\"Error.prototype\")", true);
        value.insert("engine->evaluate(\"Object\")", true);
        value.insert("engine->evaluate(\"Array\")", true);
        value.insert("engine->evaluate(\"Number\")", true);
        value.insert("engine->evaluate(\"Function\")", true);
        value.insert("engine->evaluate(\"(function() { return 1; })\")", true);
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", true);
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", true);
        value.insert("engine->evaluate(\"/foo/\")", true);
        value.insert("engine->evaluate(\"new Object()\")", true);
        value.insert("engine->evaluate(\"new Array()\")", true);
        value.insert("engine->evaluate(\"new Error()\")", true);
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castbool_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(qscriptvalue_cast<bool>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castbool)


void tst_QScriptValue::qscriptvalue_castqint32_initData()
{
    QTest::addColumn<qint32>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castqint32_makeData(const char* expr)
{
    static QHash<QString, qint32> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", 0);
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(true)", 1);
        value.insert("QScriptValue(false)", 0);
        value.insert("QScriptValue(int(122))", 122);
        value.insert("QScriptValue(uint(124))", 124);
        value.insert("QScriptValue(0)", 0);
        value.insert("QScriptValue(0.0)", 0);
        value.insert("QScriptValue(123.0)", 123);
        value.insert("QScriptValue(6.37e-8)", 0);
        value.insert("QScriptValue(-6.37e-8)", 0);
        value.insert("QScriptValue(0x43211234)", 1126240820);
        value.insert("QScriptValue(0x10000)", 65536);
        value.insert("QScriptValue(0x10001)", 65537);
        value.insert("QScriptValue(qSNaN())", 0);
        value.insert("QScriptValue(qQNaN())", 0);
        value.insert("QScriptValue(qInf())", 0);
        value.insert("QScriptValue(-qInf())", 0);
        value.insert("QScriptValue(\"NaN\")", 0);
        value.insert("QScriptValue(\"Infinity\")", 0);
        value.insert("QScriptValue(\"-Infinity\")", 0);
        value.insert("QScriptValue(\"ciao\")", 0);
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(QString(\"\"))", 0);
        value.insert("QScriptValue(QString())", 0);
        value.insert("QScriptValue(QString(\"0\"))", 0);
        value.insert("QScriptValue(QString(\"123\"))", 123);
        value.insert("QScriptValue(QString(\"12.4\"))", 12);
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(0, true)", 1);
        value.insert("QScriptValue(0, false)", 0);
        value.insert("QScriptValue(0, int(122))", 122);
        value.insert("QScriptValue(0, uint(124))", 124);
        value.insert("QScriptValue(0, 0)", 0);
        value.insert("QScriptValue(0, 0.0)", 0);
        value.insert("QScriptValue(0, 123.0)", 123);
        value.insert("QScriptValue(0, 6.37e-8)", 0);
        value.insert("QScriptValue(0, -6.37e-8)", 0);
        value.insert("QScriptValue(0, 0x43211234)", 1126240820);
        value.insert("QScriptValue(0, 0x10000)", 65536);
        value.insert("QScriptValue(0, 0x10001)", 65537);
        value.insert("QScriptValue(0, qSNaN())", 0);
        value.insert("QScriptValue(0, qQNaN())", 0);
        value.insert("QScriptValue(0, qInf())", 0);
        value.insert("QScriptValue(0, -qInf())", 0);
        value.insert("QScriptValue(0, \"NaN\")", 0);
        value.insert("QScriptValue(0, \"Infinity\")", 0);
        value.insert("QScriptValue(0, \"-Infinity\")", 0);
        value.insert("QScriptValue(0, \"ciao\")", 0);
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(0, QString(\"\"))", 0);
        value.insert("QScriptValue(0, QString())", 0);
        value.insert("QScriptValue(0, QString(\"0\"))", 0);
        value.insert("QScriptValue(0, QString(\"123\"))", 123);
        value.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(engine, true)", 1);
        value.insert("QScriptValue(engine, false)", 0);
        value.insert("QScriptValue(engine, int(122))", 122);
        value.insert("QScriptValue(engine, uint(124))", 124);
        value.insert("QScriptValue(engine, 0)", 0);
        value.insert("QScriptValue(engine, 0.0)", 0);
        value.insert("QScriptValue(engine, 123.0)", 123);
        value.insert("QScriptValue(engine, 6.37e-8)", 0);
        value.insert("QScriptValue(engine, -6.37e-8)", 0);
        value.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        value.insert("QScriptValue(engine, 0x10000)", 65536);
        value.insert("QScriptValue(engine, 0x10001)", 65537);
        value.insert("QScriptValue(engine, qSNaN())", 0);
        value.insert("QScriptValue(engine, qQNaN())", 0);
        value.insert("QScriptValue(engine, qInf())", 0);
        value.insert("QScriptValue(engine, -qInf())", 0);
        value.insert("QScriptValue(engine, \"NaN\")", 0);
        value.insert("QScriptValue(engine, \"Infinity\")", 0);
        value.insert("QScriptValue(engine, \"-Infinity\")", 0);
        value.insert("QScriptValue(engine, \"ciao\")", 0);
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(engine, QString(\"\"))", 0);
        value.insert("QScriptValue(engine, QString())", 0);
        value.insert("QScriptValue(engine, QString(\"0\"))", 0);
        value.insert("QScriptValue(engine, QString(\"123\"))", 123);
        value.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        value.insert("engine->evaluate(\"[]\")", 0);
        value.insert("engine->evaluate(\"{}\")", 0);
        value.insert("engine->evaluate(\"Object.prototype\")", 0);
        value.insert("engine->evaluate(\"Date.prototype\")", 0);
        value.insert("engine->evaluate(\"Array.prototype\")", 0);
        value.insert("engine->evaluate(\"Function.prototype\")", 0);
        value.insert("engine->evaluate(\"Error.prototype\")", 0);
        value.insert("engine->evaluate(\"Object\")", 0);
        value.insert("engine->evaluate(\"Array\")", 0);
        value.insert("engine->evaluate(\"Number\")", 0);
        value.insert("engine->evaluate(\"Function\")", 0);
        value.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        value.insert("engine->evaluate(\"/foo/\")", 0);
        value.insert("engine->evaluate(\"new Object()\")", 0);
        value.insert("engine->evaluate(\"new Array()\")", 0);
        value.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castqint32_test(const char*, const QScriptValue& value)
{
    QFETCH(qint32, expected);
    QCOMPARE(qscriptvalue_cast<qint32>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castqint32)


void tst_QScriptValue::qscriptvalue_castquint32_initData()
{
    QTest::addColumn<quint32>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castquint32_makeData(const char* expr)
{
    static QHash<QString, quint32> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", 0);
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(true)", 1);
        value.insert("QScriptValue(false)", 0);
        value.insert("QScriptValue(int(122))", 122);
        value.insert("QScriptValue(uint(124))", 124);
        value.insert("QScriptValue(0)", 0);
        value.insert("QScriptValue(0.0)", 0);
        value.insert("QScriptValue(123.0)", 123);
        value.insert("QScriptValue(6.37e-8)", 0);
        value.insert("QScriptValue(-6.37e-8)", 0);
        value.insert("QScriptValue(0x43211234)", 1126240820);
        value.insert("QScriptValue(0x10000)", 65536);
        value.insert("QScriptValue(0x10001)", 65537);
        value.insert("QScriptValue(qSNaN())", 0);
        value.insert("QScriptValue(qQNaN())", 0);
        value.insert("QScriptValue(qInf())", 0);
        value.insert("QScriptValue(-qInf())", 0);
        value.insert("QScriptValue(\"NaN\")", 0);
        value.insert("QScriptValue(\"Infinity\")", 0);
        value.insert("QScriptValue(\"-Infinity\")", 0);
        value.insert("QScriptValue(\"ciao\")", 0);
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(QString(\"\"))", 0);
        value.insert("QScriptValue(QString())", 0);
        value.insert("QScriptValue(QString(\"0\"))", 0);
        value.insert("QScriptValue(QString(\"123\"))", 123);
        value.insert("QScriptValue(QString(\"12.4\"))", 12);
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(0, true)", 1);
        value.insert("QScriptValue(0, false)", 0);
        value.insert("QScriptValue(0, int(122))", 122);
        value.insert("QScriptValue(0, uint(124))", 124);
        value.insert("QScriptValue(0, 0)", 0);
        value.insert("QScriptValue(0, 0.0)", 0);
        value.insert("QScriptValue(0, 123.0)", 123);
        value.insert("QScriptValue(0, 6.37e-8)", 0);
        value.insert("QScriptValue(0, -6.37e-8)", 0);
        value.insert("QScriptValue(0, 0x43211234)", 1126240820);
        value.insert("QScriptValue(0, 0x10000)", 65536);
        value.insert("QScriptValue(0, 0x10001)", 65537);
        value.insert("QScriptValue(0, qSNaN())", 0);
        value.insert("QScriptValue(0, qQNaN())", 0);
        value.insert("QScriptValue(0, qInf())", 0);
        value.insert("QScriptValue(0, -qInf())", 0);
        value.insert("QScriptValue(0, \"NaN\")", 0);
        value.insert("QScriptValue(0, \"Infinity\")", 0);
        value.insert("QScriptValue(0, \"-Infinity\")", 0);
        value.insert("QScriptValue(0, \"ciao\")", 0);
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(0, QString(\"\"))", 0);
        value.insert("QScriptValue(0, QString())", 0);
        value.insert("QScriptValue(0, QString(\"0\"))", 0);
        value.insert("QScriptValue(0, QString(\"123\"))", 123);
        value.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(engine, true)", 1);
        value.insert("QScriptValue(engine, false)", 0);
        value.insert("QScriptValue(engine, int(122))", 122);
        value.insert("QScriptValue(engine, uint(124))", 124);
        value.insert("QScriptValue(engine, 0)", 0);
        value.insert("QScriptValue(engine, 0.0)", 0);
        value.insert("QScriptValue(engine, 123.0)", 123);
        value.insert("QScriptValue(engine, 6.37e-8)", 0);
        value.insert("QScriptValue(engine, -6.37e-8)", 0);
        value.insert("QScriptValue(engine, 0x43211234)", 1126240820);
        value.insert("QScriptValue(engine, 0x10000)", 65536);
        value.insert("QScriptValue(engine, 0x10001)", 65537);
        value.insert("QScriptValue(engine, qSNaN())", 0);
        value.insert("QScriptValue(engine, qQNaN())", 0);
        value.insert("QScriptValue(engine, qInf())", 0);
        value.insert("QScriptValue(engine, -qInf())", 0);
        value.insert("QScriptValue(engine, \"NaN\")", 0);
        value.insert("QScriptValue(engine, \"Infinity\")", 0);
        value.insert("QScriptValue(engine, \"-Infinity\")", 0);
        value.insert("QScriptValue(engine, \"ciao\")", 0);
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(engine, QString(\"\"))", 0);
        value.insert("QScriptValue(engine, QString())", 0);
        value.insert("QScriptValue(engine, QString(\"0\"))", 0);
        value.insert("QScriptValue(engine, QString(\"123\"))", 123);
        value.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        value.insert("engine->evaluate(\"[]\")", 0);
        value.insert("engine->evaluate(\"{}\")", 0);
        value.insert("engine->evaluate(\"Object.prototype\")", 0);
        value.insert("engine->evaluate(\"Date.prototype\")", 0);
        value.insert("engine->evaluate(\"Array.prototype\")", 0);
        value.insert("engine->evaluate(\"Function.prototype\")", 0);
        value.insert("engine->evaluate(\"Error.prototype\")", 0);
        value.insert("engine->evaluate(\"Object\")", 0);
        value.insert("engine->evaluate(\"Array\")", 0);
        value.insert("engine->evaluate(\"Number\")", 0);
        value.insert("engine->evaluate(\"Function\")", 0);
        value.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        value.insert("engine->evaluate(\"/foo/\")", 0);
        value.insert("engine->evaluate(\"new Object()\")", 0);
        value.insert("engine->evaluate(\"new Array()\")", 0);
        value.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castquint32_test(const char*, const QScriptValue& value)
{
    QFETCH(quint32, expected);
    QCOMPARE(qscriptvalue_cast<quint32>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castquint32)


void tst_QScriptValue::qscriptvalue_castquint16_initData()
{
    QTest::addColumn<quint16>("expected");
    initScriptValues();
}

void tst_QScriptValue::qscriptvalue_castquint16_makeData(const char* expr)
{
    static QHash<QString, quint16> value;
    if (value.isEmpty()) {
        value.insert("QScriptValue()", 0);
        value.insert("QScriptValue(QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(true)", 1);
        value.insert("QScriptValue(false)", 0);
        value.insert("QScriptValue(int(122))", 122);
        value.insert("QScriptValue(uint(124))", 124);
        value.insert("QScriptValue(0)", 0);
        value.insert("QScriptValue(0.0)", 0);
        value.insert("QScriptValue(123.0)", 123);
        value.insert("QScriptValue(6.37e-8)", 0);
        value.insert("QScriptValue(-6.37e-8)", 0);
        value.insert("QScriptValue(0x43211234)", 4660);
        value.insert("QScriptValue(0x10000)", 0);
        value.insert("QScriptValue(0x10001)", 1);
        value.insert("QScriptValue(qSNaN())", 0);
        value.insert("QScriptValue(qQNaN())", 0);
        value.insert("QScriptValue(qInf())", 0);
        value.insert("QScriptValue(-qInf())", 0);
        value.insert("QScriptValue(\"NaN\")", 0);
        value.insert("QScriptValue(\"Infinity\")", 0);
        value.insert("QScriptValue(\"-Infinity\")", 0);
        value.insert("QScriptValue(\"ciao\")", 0);
        value.insert("QScriptValue(QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(QString(\"\"))", 0);
        value.insert("QScriptValue(QString())", 0);
        value.insert("QScriptValue(QString(\"0\"))", 0);
        value.insert("QScriptValue(QString(\"123\"))", 123);
        value.insert("QScriptValue(QString(\"12.4\"))", 12);
        value.insert("QScriptValue(0, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(0, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(0, true)", 1);
        value.insert("QScriptValue(0, false)", 0);
        value.insert("QScriptValue(0, int(122))", 122);
        value.insert("QScriptValue(0, uint(124))", 124);
        value.insert("QScriptValue(0, 0)", 0);
        value.insert("QScriptValue(0, 0.0)", 0);
        value.insert("QScriptValue(0, 123.0)", 123);
        value.insert("QScriptValue(0, 6.37e-8)", 0);
        value.insert("QScriptValue(0, -6.37e-8)", 0);
        value.insert("QScriptValue(0, 0x43211234)", 4660);
        value.insert("QScriptValue(0, 0x10000)", 0);
        value.insert("QScriptValue(0, 0x10001)", 1);
        value.insert("QScriptValue(0, qSNaN())", 0);
        value.insert("QScriptValue(0, qQNaN())", 0);
        value.insert("QScriptValue(0, qInf())", 0);
        value.insert("QScriptValue(0, -qInf())", 0);
        value.insert("QScriptValue(0, \"NaN\")", 0);
        value.insert("QScriptValue(0, \"Infinity\")", 0);
        value.insert("QScriptValue(0, \"-Infinity\")", 0);
        value.insert("QScriptValue(0, \"ciao\")", 0);
        value.insert("QScriptValue(0, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(0, QString(\"\"))", 0);
        value.insert("QScriptValue(0, QString())", 0);
        value.insert("QScriptValue(0, QString(\"0\"))", 0);
        value.insert("QScriptValue(0, QString(\"123\"))", 123);
        value.insert("QScriptValue(0, QString(\"12.3\"))", 12);
        value.insert("QScriptValue(engine, QScriptValue::UndefinedValue)", 0);
        value.insert("QScriptValue(engine, QScriptValue::NullValue)", 0);
        value.insert("QScriptValue(engine, true)", 1);
        value.insert("QScriptValue(engine, false)", 0);
        value.insert("QScriptValue(engine, int(122))", 122);
        value.insert("QScriptValue(engine, uint(124))", 124);
        value.insert("QScriptValue(engine, 0)", 0);
        value.insert("QScriptValue(engine, 0.0)", 0);
        value.insert("QScriptValue(engine, 123.0)", 123);
        value.insert("QScriptValue(engine, 6.37e-8)", 0);
        value.insert("QScriptValue(engine, -6.37e-8)", 0);
        value.insert("QScriptValue(engine, 0x43211234)", 4660);
        value.insert("QScriptValue(engine, 0x10000)", 0);
        value.insert("QScriptValue(engine, 0x10001)", 1);
        value.insert("QScriptValue(engine, qSNaN())", 0);
        value.insert("QScriptValue(engine, qQNaN())", 0);
        value.insert("QScriptValue(engine, qInf())", 0);
        value.insert("QScriptValue(engine, -qInf())", 0);
        value.insert("QScriptValue(engine, \"NaN\")", 0);
        value.insert("QScriptValue(engine, \"Infinity\")", 0);
        value.insert("QScriptValue(engine, \"-Infinity\")", 0);
        value.insert("QScriptValue(engine, \"ciao\")", 0);
        value.insert("QScriptValue(engine, QString::fromLatin1(\"ciao\"))", 0);
        value.insert("QScriptValue(engine, QString(\"\"))", 0);
        value.insert("QScriptValue(engine, QString())", 0);
        value.insert("QScriptValue(engine, QString(\"0\"))", 0);
        value.insert("QScriptValue(engine, QString(\"123\"))", 123);
        value.insert("QScriptValue(engine, QString(\"1.23\"))", 1);
        value.insert("engine->evaluate(\"[]\")", 0);
        value.insert("engine->evaluate(\"{}\")", 0);
        value.insert("engine->evaluate(\"Object.prototype\")", 0);
        value.insert("engine->evaluate(\"Date.prototype\")", 0);
        value.insert("engine->evaluate(\"Array.prototype\")", 0);
        value.insert("engine->evaluate(\"Function.prototype\")", 0);
        value.insert("engine->evaluate(\"Error.prototype\")", 0);
        value.insert("engine->evaluate(\"Object\")", 0);
        value.insert("engine->evaluate(\"Array\")", 0);
        value.insert("engine->evaluate(\"Number\")", 0);
        value.insert("engine->evaluate(\"Function\")", 0);
        value.insert("engine->evaluate(\"(function() { return 1; })\")", 0);
        value.insert("engine->evaluate(\"(function() { return 'ciao'; })\")", 0);
        value.insert("engine->evaluate(\"(function() { throw new Error('foo'); })\")", 0);
        value.insert("engine->evaluate(\"/foo/\")", 0);
        value.insert("engine->evaluate(\"new Object()\")", 0);
        value.insert("engine->evaluate(\"new Array()\")", 0);
        value.insert("engine->evaluate(\"new Error()\")", 0);
    }
    newRow(expr) << value.value(expr);
}

void tst_QScriptValue::qscriptvalue_castquint16_test(const char*, const QScriptValue& value)
{
    QFETCH(quint16, expected);
    QCOMPARE(qscriptvalue_cast<quint16>(value), expected);
}

DEFINE_TEST_FUNCTION(qscriptvalue_castquint16)
