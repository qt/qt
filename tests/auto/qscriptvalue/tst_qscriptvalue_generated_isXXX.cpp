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

/****************************************************************************
*************** This file has been generated. DO NOT MODIFY! ****************
****************************************************************************/

#include "tst_qscriptvalue.h"


void tst_QScriptValue::isValid_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isValid_array [] = {
    "QScriptValue(QScriptValue::UndefinedValue)",
    "QScriptValue(QScriptValue::NullValue)",
    "QScriptValue(true)",
    "QScriptValue(false)",
    "QScriptValue(int(122))",
    "QScriptValue(uint(124))",
    "QScriptValue(0)",
    "QScriptValue(0.0)",
    "QScriptValue(123.0)",
    "QScriptValue(6.37e-8)",
    "QScriptValue(-6.37e-8)",
    "QScriptValue(0x43211234)",
    "QScriptValue(0x10000)",
    "QScriptValue(0x10001)",
    "QScriptValue(qSNaN())",
    "QScriptValue(qQNaN())",
    "QScriptValue(qInf())",
    "QScriptValue(-qInf())",
    "QScriptValue(\"NaN\")",
    "QScriptValue(\"Infinity\")",
    "QScriptValue(\"-Infinity\")",
    "QScriptValue(\"ciao\")",
    "QScriptValue(QString::fromLatin1(\"ciao\"))",
    "QScriptValue(QString(\"\"))",
    "QScriptValue(QString())",
    "QScriptValue(QString(\"0\"))",
    "QScriptValue(QString(\"123\"))",
    "QScriptValue(QString(\"12.4\"))",
    "QScriptValue(0, QScriptValue::UndefinedValue)",
    "QScriptValue(0, QScriptValue::NullValue)",
    "QScriptValue(0, true)",
    "QScriptValue(0, false)",
    "QScriptValue(0, int(122))",
    "QScriptValue(0, uint(124))",
    "QScriptValue(0, 0)",
    "QScriptValue(0, 0.0)",
    "QScriptValue(0, 123.0)",
    "QScriptValue(0, 6.37e-8)",
    "QScriptValue(0, -6.37e-8)",
    "QScriptValue(0, 0x43211234)",
    "QScriptValue(0, 0x10000)",
    "QScriptValue(0, 0x10001)",
    "QScriptValue(0, qSNaN())",
    "QScriptValue(0, qQNaN())",
    "QScriptValue(0, qInf())",
    "QScriptValue(0, -qInf())",
    "QScriptValue(0, \"NaN\")",
    "QScriptValue(0, \"Infinity\")",
    "QScriptValue(0, \"-Infinity\")",
    "QScriptValue(0, \"ciao\")",
    "QScriptValue(0, QString::fromLatin1(\"ciao\"))",
    "QScriptValue(0, QString(\"\"))",
    "QScriptValue(0, QString())",
    "QScriptValue(0, QString(\"0\"))",
    "QScriptValue(0, QString(\"123\"))",
    "QScriptValue(0, QString(\"12.3\"))",
    "QScriptValue(engine, QScriptValue::UndefinedValue)",
    "QScriptValue(engine, QScriptValue::NullValue)",
    "QScriptValue(engine, true)",
    "QScriptValue(engine, false)",
    "QScriptValue(engine, int(122))",
    "QScriptValue(engine, uint(124))",
    "QScriptValue(engine, 0)",
    "QScriptValue(engine, 0.0)",
    "QScriptValue(engine, 123.0)",
    "QScriptValue(engine, 6.37e-8)",
    "QScriptValue(engine, -6.37e-8)",
    "QScriptValue(engine, 0x43211234)",
    "QScriptValue(engine, 0x10000)",
    "QScriptValue(engine, 0x10001)",
    "QScriptValue(engine, qSNaN())",
    "QScriptValue(engine, qQNaN())",
    "QScriptValue(engine, qInf())",
    "QScriptValue(engine, -qInf())",
    "QScriptValue(engine, \"NaN\")",
    "QScriptValue(engine, \"Infinity\")",
    "QScriptValue(engine, \"-Infinity\")",
    "QScriptValue(engine, \"ciao\")",
    "QScriptValue(engine, QString::fromLatin1(\"ciao\"))",
    "QScriptValue(engine, QString(\"\"))",
    "QScriptValue(engine, QString())",
    "QScriptValue(engine, QString(\"0\"))",
    "QScriptValue(engine, QString(\"123\"))",
    "QScriptValue(engine, QString(\"1.23\"))",
    "engine->evaluate(\"[]\")",
    "engine->evaluate(\"{}\")",
    "engine->evaluate(\"Object.prototype\")",
    "engine->evaluate(\"Date.prototype\")",
    "engine->evaluate(\"Array.prototype\")",
    "engine->evaluate(\"Function.prototype\")",
    "engine->evaluate(\"Error.prototype\")",
    "engine->evaluate(\"Object\")",
    "engine->evaluate(\"Array\")",
    "engine->evaluate(\"Number\")",
    "engine->evaluate(\"Function\")",
    "engine->evaluate(\"(function() { return 1; })\")",
    "engine->evaluate(\"(function() { return 'ciao'; })\")",
    "engine->evaluate(\"(function() { throw new Error('foo'); })\")",
    "engine->evaluate(\"/foo/\")",
    "engine->evaluate(\"new Object()\")",
    "engine->evaluate(\"new Array()\")",
    "engine->evaluate(\"new Error()\")",
    "engine->evaluate(\"a = new Object(); a.foo = 22; a.foo\")",
    "engine->evaluate(\"Undefined\")",
    "engine->evaluate(\"Null\")",
    "engine->evaluate(\"True\")",
    "engine->evaluate(\"False\")",
    "engine->evaluate(\"undefined\")",
    "engine->evaluate(\"null\")",
    "engine->evaluate(\"true\")",
    "engine->evaluate(\"false\")",
    "engine->evaluate(\"122\")",
    "engine->evaluate(\"124\")",
    "engine->evaluate(\"0\")",
    "engine->evaluate(\"0.0\")",
    "engine->evaluate(\"123.0\")",
    "engine->evaluate(\"6.37e-8\")",
    "engine->evaluate(\"-6.37e-8\")",
    "engine->evaluate(\"0x43211234\")",
    "engine->evaluate(\"0x10000\")",
    "engine->evaluate(\"0x10001\")",
    "engine->evaluate(\"NaN\")",
    "engine->evaluate(\"Infinity\")",
    "engine->evaluate(\"-Infinity\")",
    "engine->evaluate(\"'ciao'\")",
    "engine->evaluate(\"''\")",
    "engine->evaluate(\"'0'\")",
    "engine->evaluate(\"'123'\")",
    "engine->evaluate(\"'12.4'\")",
    "engine->nullValue()",
    "engine->undefinedValue()",
    "engine->newObject()",
    "engine->newArray()",
    "engine->newArray(10)",
    "engine->newDate(QDateTime())",
    "engine->newQMetaObject(&QObject::staticMetaObject)",
    "engine->newVariant(QVariant())",
    "engine->newVariant(QVariant(123))",
    "engine->newVariant(QVariant(false))",
    "engine->newQObject(0)",
    "engine->newQObject(engine)",};

void tst_QScriptValue::isValid_makeData(const char* expr)
{
    static QSet<QString> isValid;
    if (isValid.isEmpty()) {
        isValid.reserve(141);
        for (unsigned i = 0; i < 141; ++i)
            isValid.insert(isValid_array[i]);
    }
    newRow(expr) << isValid.contains(expr);
}

void tst_QScriptValue::isValid_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isValid(), expected);
    QCOMPARE(value.isValid(), expected);
}

DEFINE_TEST_FUNCTION(isValid)


void tst_QScriptValue::isBool_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isBool_array [] = {
    "QScriptValue(true)",
    "QScriptValue(false)",
    "QScriptValue(0, true)",
    "QScriptValue(0, false)",
    "QScriptValue(engine, true)",
    "QScriptValue(engine, false)",
    "engine->evaluate(\"true\")",
    "engine->evaluate(\"false\")",};

void tst_QScriptValue::isBool_makeData(const char* expr)
{
    static QSet<QString> isBool;
    if (isBool.isEmpty()) {
        isBool.reserve(8);
        for (unsigned i = 0; i < 8; ++i)
            isBool.insert(isBool_array[i]);
    }
    newRow(expr) << isBool.contains(expr);
}

void tst_QScriptValue::isBool_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isBool(), expected);
    QCOMPARE(value.isBool(), expected);
}

DEFINE_TEST_FUNCTION(isBool)


void tst_QScriptValue::isBoolean_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isBoolean_array [] = {
    "QScriptValue(true)",
    "QScriptValue(false)",
    "QScriptValue(0, true)",
    "QScriptValue(0, false)",
    "QScriptValue(engine, true)",
    "QScriptValue(engine, false)",
    "engine->evaluate(\"true\")",
    "engine->evaluate(\"false\")",};

void tst_QScriptValue::isBoolean_makeData(const char* expr)
{
    static QSet<QString> isBoolean;
    if (isBoolean.isEmpty()) {
        isBoolean.reserve(8);
        for (unsigned i = 0; i < 8; ++i)
            isBoolean.insert(isBoolean_array[i]);
    }
    newRow(expr) << isBoolean.contains(expr);
}

void tst_QScriptValue::isBoolean_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isBoolean(), expected);
    QCOMPARE(value.isBoolean(), expected);
}

DEFINE_TEST_FUNCTION(isBoolean)


void tst_QScriptValue::isNumber_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isNumber_array [] = {
    "QScriptValue(int(122))",
    "QScriptValue(uint(124))",
    "QScriptValue(0)",
    "QScriptValue(0.0)",
    "QScriptValue(123.0)",
    "QScriptValue(6.37e-8)",
    "QScriptValue(-6.37e-8)",
    "QScriptValue(0x43211234)",
    "QScriptValue(0x10000)",
    "QScriptValue(0x10001)",
    "QScriptValue(qSNaN())",
    "QScriptValue(qQNaN())",
    "QScriptValue(qInf())",
    "QScriptValue(-qInf())",
    "QScriptValue(0, int(122))",
    "QScriptValue(0, uint(124))",
    "QScriptValue(0, 0)",
    "QScriptValue(0, 0.0)",
    "QScriptValue(0, 123.0)",
    "QScriptValue(0, 6.37e-8)",
    "QScriptValue(0, -6.37e-8)",
    "QScriptValue(0, 0x43211234)",
    "QScriptValue(0, 0x10000)",
    "QScriptValue(0, 0x10001)",
    "QScriptValue(0, qSNaN())",
    "QScriptValue(0, qQNaN())",
    "QScriptValue(0, qInf())",
    "QScriptValue(0, -qInf())",
    "QScriptValue(engine, int(122))",
    "QScriptValue(engine, uint(124))",
    "QScriptValue(engine, 0)",
    "QScriptValue(engine, 0.0)",
    "QScriptValue(engine, 123.0)",
    "QScriptValue(engine, 6.37e-8)",
    "QScriptValue(engine, -6.37e-8)",
    "QScriptValue(engine, 0x43211234)",
    "QScriptValue(engine, 0x10000)",
    "QScriptValue(engine, 0x10001)",
    "QScriptValue(engine, qSNaN())",
    "QScriptValue(engine, qQNaN())",
    "QScriptValue(engine, qInf())",
    "QScriptValue(engine, -qInf())",
    "engine->evaluate(\"a = new Object(); a.foo = 22; a.foo\")",
    "engine->evaluate(\"122\")",
    "engine->evaluate(\"124\")",
    "engine->evaluate(\"0\")",
    "engine->evaluate(\"0.0\")",
    "engine->evaluate(\"123.0\")",
    "engine->evaluate(\"6.37e-8\")",
    "engine->evaluate(\"-6.37e-8\")",
    "engine->evaluate(\"0x43211234\")",
    "engine->evaluate(\"0x10000\")",
    "engine->evaluate(\"0x10001\")",
    "engine->evaluate(\"NaN\")",
    "engine->evaluate(\"Infinity\")",
    "engine->evaluate(\"-Infinity\")",};

void tst_QScriptValue::isNumber_makeData(const char* expr)
{
    static QSet<QString> isNumber;
    if (isNumber.isEmpty()) {
        isNumber.reserve(56);
        for (unsigned i = 0; i < 56; ++i)
            isNumber.insert(isNumber_array[i]);
    }
    newRow(expr) << isNumber.contains(expr);
}

void tst_QScriptValue::isNumber_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isNumber(), expected);
    QCOMPARE(value.isNumber(), expected);
}

DEFINE_TEST_FUNCTION(isNumber)


void tst_QScriptValue::isFunction_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isFunction_array [] = {
    "engine->evaluate(\"Function.prototype\")",
    "engine->evaluate(\"Object\")",
    "engine->evaluate(\"Array\")",
    "engine->evaluate(\"Number\")",
    "engine->evaluate(\"Function\")",
    "engine->evaluate(\"(function() { return 1; })\")",
    "engine->evaluate(\"(function() { return 'ciao'; })\")",
    "engine->evaluate(\"(function() { throw new Error('foo'); })\")",
    "engine->evaluate(\"/foo/\")",
    "engine->newQMetaObject(&QObject::staticMetaObject)",};

void tst_QScriptValue::isFunction_makeData(const char* expr)
{
    static QSet<QString> isFunction;
    if (isFunction.isEmpty()) {
        isFunction.reserve(10);
        for (unsigned i = 0; i < 10; ++i)
            isFunction.insert(isFunction_array[i]);
    }
    newRow(expr) << isFunction.contains(expr);
}

void tst_QScriptValue::isFunction_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isFunction(), expected);
    QCOMPARE(value.isFunction(), expected);
}

DEFINE_TEST_FUNCTION(isFunction)


void tst_QScriptValue::isNull_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isNull_array [] = {
    "QScriptValue(QScriptValue::NullValue)",
    "QScriptValue(0, QScriptValue::NullValue)",
    "QScriptValue(engine, QScriptValue::NullValue)",
    "engine->evaluate(\"null\")",
    "engine->nullValue()",
    "engine->newQObject(0)",};

void tst_QScriptValue::isNull_makeData(const char* expr)
{
    static QSet<QString> isNull;
    if (isNull.isEmpty()) {
        isNull.reserve(6);
        for (unsigned i = 0; i < 6; ++i)
            isNull.insert(isNull_array[i]);
    }
    newRow(expr) << isNull.contains(expr);
}

void tst_QScriptValue::isNull_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isNull(), expected);
    QCOMPARE(value.isNull(), expected);
}

DEFINE_TEST_FUNCTION(isNull)


void tst_QScriptValue::isString_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isString_array [] = {
    "QScriptValue(\"NaN\")",
    "QScriptValue(\"Infinity\")",
    "QScriptValue(\"-Infinity\")",
    "QScriptValue(\"ciao\")",
    "QScriptValue(QString::fromLatin1(\"ciao\"))",
    "QScriptValue(QString(\"\"))",
    "QScriptValue(QString())",
    "QScriptValue(QString(\"0\"))",
    "QScriptValue(QString(\"123\"))",
    "QScriptValue(QString(\"12.4\"))",
    "QScriptValue(0, \"NaN\")",
    "QScriptValue(0, \"Infinity\")",
    "QScriptValue(0, \"-Infinity\")",
    "QScriptValue(0, \"ciao\")",
    "QScriptValue(0, QString::fromLatin1(\"ciao\"))",
    "QScriptValue(0, QString(\"\"))",
    "QScriptValue(0, QString())",
    "QScriptValue(0, QString(\"0\"))",
    "QScriptValue(0, QString(\"123\"))",
    "QScriptValue(0, QString(\"12.3\"))",
    "QScriptValue(engine, \"NaN\")",
    "QScriptValue(engine, \"Infinity\")",
    "QScriptValue(engine, \"-Infinity\")",
    "QScriptValue(engine, \"ciao\")",
    "QScriptValue(engine, QString::fromLatin1(\"ciao\"))",
    "QScriptValue(engine, QString(\"\"))",
    "QScriptValue(engine, QString())",
    "QScriptValue(engine, QString(\"0\"))",
    "QScriptValue(engine, QString(\"123\"))",
    "QScriptValue(engine, QString(\"1.23\"))",
    "engine->evaluate(\"'ciao'\")",
    "engine->evaluate(\"''\")",
    "engine->evaluate(\"'0'\")",
    "engine->evaluate(\"'123'\")",
    "engine->evaluate(\"'12.4'\")",};

void tst_QScriptValue::isString_makeData(const char* expr)
{
    static QSet<QString> isString;
    if (isString.isEmpty()) {
        isString.reserve(35);
        for (unsigned i = 0; i < 35; ++i)
            isString.insert(isString_array[i]);
    }
    newRow(expr) << isString.contains(expr);
}

void tst_QScriptValue::isString_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isString(), expected);
    QCOMPARE(value.isString(), expected);
}

DEFINE_TEST_FUNCTION(isString)


void tst_QScriptValue::isUndefined_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isUndefined_array [] = {
    "QScriptValue(QScriptValue::UndefinedValue)",
    "QScriptValue(0, QScriptValue::UndefinedValue)",
    "QScriptValue(engine, QScriptValue::UndefinedValue)",
    "engine->evaluate(\"{}\")",
    "engine->evaluate(\"undefined\")",
    "engine->undefinedValue()",};

void tst_QScriptValue::isUndefined_makeData(const char* expr)
{
    static QSet<QString> isUndefined;
    if (isUndefined.isEmpty()) {
        isUndefined.reserve(6);
        for (unsigned i = 0; i < 6; ++i)
            isUndefined.insert(isUndefined_array[i]);
    }
    newRow(expr) << isUndefined.contains(expr);
}

void tst_QScriptValue::isUndefined_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isUndefined(), expected);
    QCOMPARE(value.isUndefined(), expected);
}

DEFINE_TEST_FUNCTION(isUndefined)


void tst_QScriptValue::isVariant_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isVariant_array [] = {
    "engine->newVariant(QVariant())",
    "engine->newVariant(QVariant(123))",
    "engine->newVariant(QVariant(false))",};

void tst_QScriptValue::isVariant_makeData(const char* expr)
{
    static QSet<QString> isVariant;
    if (isVariant.isEmpty()) {
        isVariant.reserve(3);
        for (unsigned i = 0; i < 3; ++i)
            isVariant.insert(isVariant_array[i]);
    }
    newRow(expr) << isVariant.contains(expr);
}

void tst_QScriptValue::isVariant_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isVariant(), expected);
    QCOMPARE(value.isVariant(), expected);
}

DEFINE_TEST_FUNCTION(isVariant)


void tst_QScriptValue::isQObject_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isQObject_array [] = {
    "engine->newQObject(engine)",};

void tst_QScriptValue::isQObject_makeData(const char* expr)
{
    static QSet<QString> isQObject;
    if (isQObject.isEmpty()) {
        isQObject.reserve(1);
        for (unsigned i = 0; i < 1; ++i)
            isQObject.insert(isQObject_array[i]);
    }
    newRow(expr) << isQObject.contains(expr);
}

void tst_QScriptValue::isQObject_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isQObject(), expected);
    QCOMPARE(value.isQObject(), expected);
}

DEFINE_TEST_FUNCTION(isQObject)


void tst_QScriptValue::isQMetaObject_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isQMetaObject_array [] = {
    "engine->newQMetaObject(&QObject::staticMetaObject)",};

void tst_QScriptValue::isQMetaObject_makeData(const char* expr)
{
    static QSet<QString> isQMetaObject;
    if (isQMetaObject.isEmpty()) {
        isQMetaObject.reserve(1);
        for (unsigned i = 0; i < 1; ++i)
            isQMetaObject.insert(isQMetaObject_array[i]);
    }
    newRow(expr) << isQMetaObject.contains(expr);
}

void tst_QScriptValue::isQMetaObject_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isQMetaObject(), expected);
    QCOMPARE(value.isQMetaObject(), expected);
}

DEFINE_TEST_FUNCTION(isQMetaObject)


void tst_QScriptValue::isObject_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isObject_array [] = {
    "engine->evaluate(\"[]\")",
    "engine->evaluate(\"Object.prototype\")",
    "engine->evaluate(\"Date.prototype\")",
    "engine->evaluate(\"Array.prototype\")",
    "engine->evaluate(\"Function.prototype\")",
    "engine->evaluate(\"Error.prototype\")",
    "engine->evaluate(\"Object\")",
    "engine->evaluate(\"Array\")",
    "engine->evaluate(\"Number\")",
    "engine->evaluate(\"Function\")",
    "engine->evaluate(\"(function() { return 1; })\")",
    "engine->evaluate(\"(function() { return 'ciao'; })\")",
    "engine->evaluate(\"(function() { throw new Error('foo'); })\")",
    "engine->evaluate(\"/foo/\")",
    "engine->evaluate(\"new Object()\")",
    "engine->evaluate(\"new Array()\")",
    "engine->evaluate(\"new Error()\")",
    "engine->evaluate(\"Undefined\")",
    "engine->evaluate(\"Null\")",
    "engine->evaluate(\"True\")",
    "engine->evaluate(\"False\")",
    "engine->newObject()",
    "engine->newArray()",
    "engine->newArray(10)",
    "engine->newDate(QDateTime())",
    "engine->newQMetaObject(&QObject::staticMetaObject)",
    "engine->newVariant(QVariant())",
    "engine->newVariant(QVariant(123))",
    "engine->newVariant(QVariant(false))",
    "engine->newQObject(engine)",};

void tst_QScriptValue::isObject_makeData(const char* expr)
{
    static QSet<QString> isObject;
    if (isObject.isEmpty()) {
        isObject.reserve(30);
        for (unsigned i = 0; i < 30; ++i)
            isObject.insert(isObject_array[i]);
    }
    newRow(expr) << isObject.contains(expr);
}

void tst_QScriptValue::isObject_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isObject(), expected);
    QCOMPARE(value.isObject(), expected);
}

DEFINE_TEST_FUNCTION(isObject)


void tst_QScriptValue::isDate_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isDate_array [] = {
    "engine->evaluate(\"Date.prototype\")",
    "engine->newDate(QDateTime())",};

void tst_QScriptValue::isDate_makeData(const char* expr)
{
    static QSet<QString> isDate;
    if (isDate.isEmpty()) {
        isDate.reserve(2);
        for (unsigned i = 0; i < 2; ++i)
            isDate.insert(isDate_array[i]);
    }
    newRow(expr) << isDate.contains(expr);
}

void tst_QScriptValue::isDate_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isDate(), expected);
    QCOMPARE(value.isDate(), expected);
}

DEFINE_TEST_FUNCTION(isDate)


void tst_QScriptValue::isRegExp_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isRegExp_array [] = {
    "engine->evaluate(\"/foo/\")",};

void tst_QScriptValue::isRegExp_makeData(const char* expr)
{
    static QSet<QString> isRegExp;
    if (isRegExp.isEmpty()) {
        isRegExp.reserve(1);
        for (unsigned i = 0; i < 1; ++i)
            isRegExp.insert(isRegExp_array[i]);
    }
    newRow(expr) << isRegExp.contains(expr);
}

void tst_QScriptValue::isRegExp_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isRegExp(), expected);
    QCOMPARE(value.isRegExp(), expected);
}

DEFINE_TEST_FUNCTION(isRegExp)


void tst_QScriptValue::isArray_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isArray_array [] = {
    "engine->evaluate(\"[]\")",
    "engine->evaluate(\"Array.prototype\")",
    "engine->evaluate(\"new Array()\")",
    "engine->newArray()",
    "engine->newArray(10)",};

void tst_QScriptValue::isArray_makeData(const char* expr)
{
    static QSet<QString> isArray;
    if (isArray.isEmpty()) {
        isArray.reserve(5);
        for (unsigned i = 0; i < 5; ++i)
            isArray.insert(isArray_array[i]);
    }
    newRow(expr) << isArray.contains(expr);
}

void tst_QScriptValue::isArray_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isArray(), expected);
    QCOMPARE(value.isArray(), expected);
}

DEFINE_TEST_FUNCTION(isArray)


void tst_QScriptValue::isError_initData()
{
    QTest::addColumn<bool>("expected");
    initScriptValues();
}

static QString isError_array [] = {
    "engine->evaluate(\"Error.prototype\")",
    "engine->evaluate(\"new Error()\")",
    "engine->evaluate(\"Undefined\")",
    "engine->evaluate(\"Null\")",
    "engine->evaluate(\"True\")",
    "engine->evaluate(\"False\")",};

void tst_QScriptValue::isError_makeData(const char* expr)
{
    static QSet<QString> isError;
    if (isError.isEmpty()) {
        isError.reserve(6);
        for (unsigned i = 0; i < 6; ++i)
            isError.insert(isError_array[i]);
    }
    newRow(expr) << isError.contains(expr);
}

void tst_QScriptValue::isError_test(const char*, const QScriptValue& value)
{
    QFETCH(bool, expected);
    QCOMPARE(value.isError(), expected);
    QCOMPARE(value.isError(), expected);
}

DEFINE_TEST_FUNCTION(isError)

