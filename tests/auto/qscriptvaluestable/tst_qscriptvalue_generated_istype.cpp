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

/****************************************************************************
*************** This file has been generated. DO NOT MODIFY! ****************
****************************************************************************/

#include "tst_qscriptvalue.h"


static const QString isValid_array[] = {
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
    "engine->newArray(10)"};

void tst_QScriptValue::isValid_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(134);
    for (uint i = 0; i < 134; ++i)
        expectedValue.insert(isValid_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isValid()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isValid(), expected);
    QCOMPARE(value.isValid(), expected);
}

static const QString isBool_array[] = {
    "QScriptValue(true)",
    "QScriptValue(false)",
    "QScriptValue(0, true)",
    "QScriptValue(0, false)",
    "QScriptValue(engine, true)",
    "QScriptValue(engine, false)",
    "engine->evaluate(\"true\")",
    "engine->evaluate(\"false\")"};

void tst_QScriptValue::isBool_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(8);
    for (uint i = 0; i < 8; ++i)
        expectedValue.insert(isBool_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isBool()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isBool(), expected);
    QCOMPARE(value.isBool(), expected);
}

static const QString isBoolean_array[] = {
    "QScriptValue(true)",
    "QScriptValue(false)",
    "QScriptValue(0, true)",
    "QScriptValue(0, false)",
    "QScriptValue(engine, true)",
    "QScriptValue(engine, false)",
    "engine->evaluate(\"true\")",
    "engine->evaluate(\"false\")"};

void tst_QScriptValue::isBoolean_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(8);
    for (uint i = 0; i < 8; ++i)
        expectedValue.insert(isBoolean_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isBoolean()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isBoolean(), expected);
    QCOMPARE(value.isBoolean(), expected);
}

static const QString isNumber_array[] = {
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
    "engine->evaluate(\"-Infinity\")"};

void tst_QScriptValue::isNumber_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(56);
    for (uint i = 0; i < 56; ++i)
        expectedValue.insert(isNumber_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isNumber()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isNumber(), expected);
    QCOMPARE(value.isNumber(), expected);
}

static const QString isFunction_array[] = {
    "engine->evaluate(\"Function.prototype\")",
    "engine->evaluate(\"Object\")",
    "engine->evaluate(\"Array\")",
    "engine->evaluate(\"Number\")",
    "engine->evaluate(\"Function\")",
    "engine->evaluate(\"(function() { return 1; })\")",
    "engine->evaluate(\"(function() { return 'ciao'; })\")",
    "engine->evaluate(\"(function() { throw new Error('foo'); })\")",
    "engine->evaluate(\"/foo/\")"};

void tst_QScriptValue::isFunction_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(9);
    for (uint i = 0; i < 9; ++i)
        expectedValue.insert(isFunction_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isFunction()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isFunction(), expected);
    QCOMPARE(value.isFunction(), expected);
}

static const QString isNull_array[] = {
    "QScriptValue(QScriptValue::NullValue)",
    "QScriptValue(0, QScriptValue::NullValue)",
    "QScriptValue(engine, QScriptValue::NullValue)",
    "engine->evaluate(\"null\")",
    "engine->nullValue()"};

void tst_QScriptValue::isNull_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(5);
    for (uint i = 0; i < 5; ++i)
        expectedValue.insert(isNull_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isNull()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isNull(), expected);
    QCOMPARE(value.isNull(), expected);
}

static const QString isString_array[] = {
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
    "engine->evaluate(\"'12.4'\")"};

void tst_QScriptValue::isString_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(35);
    for (uint i = 0; i < 35; ++i)
        expectedValue.insert(isString_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isString()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isString(), expected);
    QCOMPARE(value.isString(), expected);
}

static const QString isUndefined_array[] = {
    "QScriptValue(QScriptValue::UndefinedValue)",
    "QScriptValue(0, QScriptValue::UndefinedValue)",
    "QScriptValue(engine, QScriptValue::UndefinedValue)",
    "engine->evaluate(\"{}\")",
    "engine->evaluate(\"undefined\")",
    "engine->undefinedValue()"};

void tst_QScriptValue::isUndefined_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(6);
    for (uint i = 0; i < 6; ++i)
        expectedValue.insert(isUndefined_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isUndefined()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isUndefined(), expected);
    QCOMPARE(value.isUndefined(), expected);
}




static const QString isObject_array[] = {
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
    "engine->newArray(10)"};

void tst_QScriptValue::isObject_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(24);
    for (uint i = 0; i < 24; ++i)
        expectedValue.insert(isObject_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isObject()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isObject(), expected);
    QCOMPARE(value.isObject(), expected);
}

static const QString isArray_array[] = {
    "engine->evaluate(\"[]\")",
    "engine->evaluate(\"Array.prototype\")",
    "engine->evaluate(\"new Array()\")",
    "engine->newArray()",
    "engine->newArray(10)"};

void tst_QScriptValue::isArray_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(5);
    for (uint i = 0; i < 5; ++i)
        expectedValue.insert(isArray_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isArray()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isArray(), expected);
    QCOMPARE(value.isArray(), expected);
}

static const QString isError_array[] = {
    "engine->evaluate(\"Error.prototype\")",
    "engine->evaluate(\"new Error()\")",
    "engine->evaluate(\"Undefined\")",
    "engine->evaluate(\"Null\")",
    "engine->evaluate(\"True\")",
    "engine->evaluate(\"False\")"};

void tst_QScriptValue::isError_data()
{
    QTest::addColumn<QScriptValue>("value");
    QTest::addColumn<bool>("expected");
    if (m_engine)
        delete m_engine;
    m_engine = new QScriptEngine();
    QSet<QString> expectedValue;
    expectedValue.reserve(6);
    for (uint i = 0; i < 6; ++i)
        expectedValue.insert(isError_array[i]);
    for (uint i = 0; i < 135; ++i) {
        QPair<QString, QScriptValue> testcase = initScriptValues(i);
        QTest::newRow(testcase.first.toAscii().constData()) << testcase.second << expectedValue.contains(testcase.first);
    }
}

void tst_QScriptValue::isError()
{
    QFETCH(QScriptValue, value);
    QFETCH(bool, expected);
    QCOMPARE(value.isError(), expected);
    QCOMPARE(value.isError(), expected);
}
