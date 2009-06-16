/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <QtScript/qscriptcontextinfo.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptable.h>
#include <QtScript/qscriptengineagent.h>

Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QScriptContextInfo)
Q_DECLARE_METATYPE(QList<QScriptContextInfo>)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptContextInfo : public QObject, public QScriptable
{
    Q_OBJECT
    Q_PROPERTY(QScriptValue testProperty READ testProperty WRITE setTestProperty)
public:
    tst_QScriptContextInfo();
    virtual ~tst_QScriptContextInfo();

    QScriptValue testProperty() const
    {
        return engine()->globalObject().property("getContextInfoList").call();
    }

    QScriptValue setTestProperty(const QScriptValue &) const
    {
        return engine()->globalObject().property("getContextInfoList").call();
    }

public slots:
    QScriptValue testSlot(double a, double b)
    {
        Q_UNUSED(a); Q_UNUSED(b);
        return engine()->globalObject().property("getContextInfoList").call();
    }

    QScriptValue testSlot(const QString &s)
    {
        Q_UNUSED(s);
        return engine()->globalObject().property("getContextInfoList").call();
    }

private slots:
    void nativeFunction();
    void scriptFunction();
    void qtFunction();
    void qtPropertyFunction();
    void builtinFunctionNames_data();
    void builtinFunctionNames();
    void nullContext();
    void streaming();
};

tst_QScriptContextInfo::tst_QScriptContextInfo()
{
}

tst_QScriptContextInfo::~tst_QScriptContextInfo()
{
}

static QScriptValue getContextInfoList(QScriptContext *ctx, QScriptEngine *eng)
{
    QList<QScriptContextInfo> lst;
    while (ctx) {
        QScriptContextInfo info(ctx);
        lst.append(info);
        ctx = ctx->parentContext();
    }
    return qScriptValueFromValue(eng, lst);
}

void tst_QScriptContextInfo::nativeFunction()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("getContextInfoList", eng.newFunction(getContextInfoList));

    QString fileName = "foo.qs";
    int lineNumber = 123;
    QScriptValue ret = eng.evaluate("getContextInfoList()", fileName, lineNumber);
    QList<QScriptContextInfo> lst = qscriptvalue_cast<QList<QScriptContextInfo> >(ret);
    QCOMPARE(lst.size(), 2);

    {
        // getContextInfoList()
        QScriptContextInfo info = lst.at(0);
        QVERIFY(!info.isNull());
        QCOMPARE(info.functionType(), QScriptContextInfo::NativeFunction);
        QCOMPARE(info.scriptId(), (qint64)-1);
        QCOMPARE(info.fileName(), QString());
        QCOMPARE(info.lineNumber(), -1);
        QCOMPARE(info.columnNumber(), -1);
        QCOMPARE(info.functionName(), QString());
        QCOMPARE(info.functionEndLineNumber(), -1);
        QCOMPARE(info.functionStartLineNumber(), -1);
        QCOMPARE(info.functionParameterNames().size(), 0);
        QCOMPARE(info.functionMetaIndex(), -1);
    }

    {
        // evaluate()
        QScriptContextInfo info = lst.at(1);
        QVERIFY(!info.isNull());
        QCOMPARE(info.functionType(), QScriptContextInfo::NativeFunction);
        QVERIFY(info.scriptId() != -1);
        QCOMPARE(info.fileName(), fileName);
        QCOMPARE(info.lineNumber(), lineNumber);
        QCOMPARE(info.columnNumber(), 1);
        QCOMPARE(info.functionName(), QString());
        QCOMPARE(info.functionEndLineNumber(), -1);
        QCOMPARE(info.functionStartLineNumber(), -1);
        QCOMPARE(info.functionParameterNames().size(), 0);
        QCOMPARE(info.functionMetaIndex(), -1);
    }
}

void tst_QScriptContextInfo::scriptFunction()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("getContextInfoList", eng.newFunction(getContextInfoList));

    QString fileName = "ciao.qs";
    int lineNumber = 456;
    QScriptValue ret = eng.evaluate("function bar(a, b, c) {\n return getContextInfoList();\n}\nbar()",
                                    fileName, lineNumber);
    QList<QScriptContextInfo> lst = qscriptvalue_cast<QList<QScriptContextInfo> >(ret);
    QCOMPARE(lst.size(), 3);

    // getContextInfoList()
    QCOMPARE(lst.at(0).functionType(), QScriptContextInfo::NativeFunction);

    {
        // bar()
        QScriptContextInfo info = lst.at(1);
        QCOMPARE(info.functionType(), QScriptContextInfo::ScriptFunction);
        QVERIFY(info.scriptId() != -1);
        QCOMPARE(info.fileName(), fileName);
        QCOMPARE(info.lineNumber(), lineNumber + 1);
        QCOMPARE(info.columnNumber(), 2);
        QCOMPARE(info.functionName(), QString::fromLatin1("bar"));
        QCOMPARE(info.functionStartLineNumber(), lineNumber);
        QCOMPARE(info.functionEndLineNumber(), lineNumber + 2);
        QCOMPARE(info.functionParameterNames().size(), 3);
        QCOMPARE(info.functionParameterNames().at(0), QString::fromLatin1("a"));
        QCOMPARE(info.functionParameterNames().at(1), QString::fromLatin1("b"));
        QCOMPARE(info.functionParameterNames().at(2), QString::fromLatin1("c"));
        QCOMPARE(info.functionMetaIndex(), -1);
    }

    {
        // evaluate()
        QScriptContextInfo info = lst.at(2);
        QCOMPARE(info.functionType(), QScriptContextInfo::NativeFunction);
        QVERIFY(info.scriptId() != -1);
        QCOMPARE(info.fileName(), fileName);
        QCOMPARE(info.lineNumber(), lineNumber + 3);
        QCOMPARE(info.columnNumber(), 1);
        QCOMPARE(info.functionName(), QString());
        QCOMPARE(info.functionEndLineNumber(), -1);
        QCOMPARE(info.functionStartLineNumber(), -1);
        QCOMPARE(info.functionParameterNames().size(), 0);
        QCOMPARE(info.functionMetaIndex(), -1);
    }
}

void tst_QScriptContextInfo::qtFunction()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("getContextInfoList", eng.newFunction(getContextInfoList));
    eng.globalObject().setProperty("qobj", eng.newQObject(this));

    for (int x = 0; x < 2; ++x) {
        QString code;
        const char *sig;
        QStringList pnames;
        if (x == 0) {
            code = "qobj.testSlot(1, 2)";
            sig = "testSlot(double,double)";
            pnames << "a" << "b";
        } else {
            code = "qobj.testSlot('ciao')";
            sig = "testSlot(QString)";
            pnames << "s";
        }
        QScriptValue ret = eng.evaluate(code);
        QList<QScriptContextInfo> lst = qscriptvalue_cast<QList<QScriptContextInfo> >(ret);
        QCOMPARE(lst.size(), 3);

        // getContextInfoList()
        QCOMPARE(lst.at(0).functionType(), QScriptContextInfo::NativeFunction);

        {
            // testSlot(double a, double b)
            QScriptContextInfo info = lst.at(1);
            QCOMPARE(info.functionType(), QScriptContextInfo::QtFunction);
            QCOMPARE(info.scriptId(), (qint64)-1);
            QCOMPARE(info.fileName(), QString());
            QCOMPARE(info.lineNumber(), -1);
            QCOMPARE(info.columnNumber(), -1);
            QCOMPARE(info.functionName(), QString::fromLatin1("testSlot"));
            QCOMPARE(info.functionEndLineNumber(), -1);
            QCOMPARE(info.functionStartLineNumber(), -1);
            QCOMPARE(info.functionParameterNames().size(), pnames.size());
            QCOMPARE(info.functionParameterNames(), pnames);
            QCOMPARE(info.functionMetaIndex(), metaObject()->indexOfMethod(sig));
        }

        // evaluate()
        QCOMPARE(lst.at(0).functionType(), QScriptContextInfo::NativeFunction);
    }
}

void tst_QScriptContextInfo::qtPropertyFunction()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("getContextInfoList", eng.newFunction(getContextInfoList));
    eng.globalObject().setProperty("qobj", eng.newQObject(this));

    QScriptValue ret = eng.evaluate("qobj.testProperty");
    QList<QScriptContextInfo> lst = qscriptvalue_cast<QList<QScriptContextInfo> >(ret);
    QCOMPARE(lst.size(), 3);

    // getContextInfoList()
    QCOMPARE(lst.at(0).functionType(), QScriptContextInfo::NativeFunction);

    {
        // testProperty()
        QScriptContextInfo info = lst.at(1);
        QCOMPARE(info.functionType(), QScriptContextInfo::QtPropertyFunction);
        QCOMPARE(info.scriptId(), (qint64)-1);
        QCOMPARE(info.fileName(), QString());
        QCOMPARE(info.lineNumber(), -1);
        QCOMPARE(info.columnNumber(), -1);
        QCOMPARE(info.functionName(), QString::fromLatin1("testProperty"));
        QCOMPARE(info.functionEndLineNumber(), -1);
        QCOMPARE(info.functionStartLineNumber(), -1);
        QCOMPARE(info.functionParameterNames().size(), 0);
        QCOMPARE(info.functionMetaIndex(), metaObject()->indexOfProperty("testProperty"));
    }

    // evaluate()
    QCOMPARE(lst.at(0).functionType(), QScriptContextInfo::NativeFunction);
}

void tst_QScriptContextInfo::builtinFunctionNames_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedName");

    QTest::newRow("print") << QString("print") << QString("print");
    QTest::newRow("parseInt") << QString("parseInt") << QString("parseInt");
    QTest::newRow("parseFloat") << QString("parseFloat") << QString("parseFloat");
    QTest::newRow("isNaN") << QString("isNaN") << QString("isNaN");
    QTest::newRow("isFinite") << QString("isFinite") << QString("isFinite");
    QTest::newRow("decodeURI") << QString("decodeURI") << QString("decodeURI");
    QTest::newRow("decodeURIComponent") << QString("decodeURIComponent") << QString("decodeURIComponent");
    QTest::newRow("encodeURI") << QString("encodeURI") << QString("encodeURI");
    QTest::newRow("encodeURIComponent") << QString("encodeURIComponent") << QString("encodeURIComponent");
    QTest::newRow("escape") << QString("escape") << QString("escape");
    QTest::newRow("unescape") << QString("unescape") << QString("unescape");
    QTest::newRow("version") << QString("version") << QString("version");
    QTest::newRow("gc") << QString("gc") << QString("gc");

    QTest::newRow("Array") << QString("Array") << QString("Array");
    QTest::newRow("Array.prototype.toString") << QString("Array.prototype.toString") << QString("toString");
    QTest::newRow("Array.prototype.toLocaleString") << QString("Array.prototype.toLocaleString") << QString("toLocaleString");
    QTest::newRow("Array.prototype.concat") << QString("Array.prototype.concat") << QString("concat");
    QTest::newRow("Array.prototype.join") << QString("Array.prototype.join") << QString("join");
    QTest::newRow("Array.prototype.pop") << QString("Array.prototype.pop") << QString("pop");
    QTest::newRow("Array.prototype.push") << QString("Array.prototype.push") << QString("push");
    QTest::newRow("Array.prototype.reverse") << QString("Array.prototype.reverse") << QString("reverse");
    QTest::newRow("Array.prototype.shift") << QString("Array.prototype.shift") << QString("shift");
    QTest::newRow("Array.prototype.slice") << QString("Array.prototype.slice") << QString("slice");
    QTest::newRow("Array.prototype.sort") << QString("Array.prototype.sort") << QString("sort");
    QTest::newRow("Array.prototype.splice") << QString("Array.prototype.splice") << QString("splice");
    QTest::newRow("Array.prototype.unshift") << QString("Array.prototype.unshift") << QString("unshift");

    QTest::newRow("Boolean") << QString("Boolean") << QString("Boolean");
    QTest::newRow("Boolean.prototype.toString") << QString("Boolean.prototype.toString") << QString("toString");

    QTest::newRow("Date") << QString("Date") << QString("Date");
    QTest::newRow("Date.prototype.toString") << QString("Date.prototype.toString") << QString("toString");
    QTest::newRow("Date.prototype.toDateString") << QString("Date.prototype.toDateString") << QString("toDateString");
    QTest::newRow("Date.prototype.toTimeString") << QString("Date.prototype.toTimeString") << QString("toTimeString");
    QTest::newRow("Date.prototype.toLocaleString") << QString("Date.prototype.toLocaleString") << QString("toLocaleString");
    QTest::newRow("Date.prototype.toLocaleDateString") << QString("Date.prototype.toLocaleDateString") << QString("toLocaleDateString");
    QTest::newRow("Date.prototype.toLocaleTimeString") << QString("Date.prototype.toLocaleTimeString") << QString("toLocaleTimeString");
    QTest::newRow("Date.prototype.valueOf") << QString("Date.prototype.valueOf") << QString("valueOf");
    QTest::newRow("Date.prototype.getTime") << QString("Date.prototype.getTime") << QString("getTime");
    QTest::newRow("Date.prototype.getYear") << QString("Date.prototype.getYear") << QString("getYear");
    QTest::newRow("Date.prototype.getFullYear") << QString("Date.prototype.getFullYear") << QString("getFullYear");
    QTest::newRow("Date.prototype.getUTCFullYear") << QString("Date.prototype.getUTCFullYear") << QString("getUTCFullYear");
    QTest::newRow("Date.prototype.getMonth") << QString("Date.prototype.getMonth") << QString("getMonth");
    QTest::newRow("Date.prototype.getUTCMonth") << QString("Date.prototype.getUTCMonth") << QString("getUTCMonth");
    QTest::newRow("Date.prototype.getDate") << QString("Date.prototype.getDate") << QString("getDate");
    QTest::newRow("Date.prototype.getUTCDate") << QString("Date.prototype.getUTCDate") << QString("getUTCDate");
    QTest::newRow("Date.prototype.getDay") << QString("Date.prototype.getDay") << QString("getDay");
    QTest::newRow("Date.prototype.getUTCDay") << QString("Date.prototype.getUTCDay") << QString("getUTCDay");
    QTest::newRow("Date.prototype.getHours") << QString("Date.prototype.getHours") << QString("getHours");
    QTest::newRow("Date.prototype.getUTCHours") << QString("Date.prototype.getUTCHours") << QString("getUTCHours");
    QTest::newRow("Date.prototype.getMinutes") << QString("Date.prototype.getMinutes") << QString("getMinutes");
    QTest::newRow("Date.prototype.getUTCMinutes") << QString("Date.prototype.getUTCMinutes") << QString("getUTCMinutes");
    QTest::newRow("Date.prototype.getSeconds") << QString("Date.prototype.getSeconds") << QString("getSeconds");
    QTest::newRow("Date.prototype.getUTCSeconds") << QString("Date.prototype.getUTCSeconds") << QString("getUTCSeconds");
    QTest::newRow("Date.prototype.getMilliseconds") << QString("Date.prototype.getMilliseconds") << QString("getMilliseconds");
    QTest::newRow("Date.prototype.getUTCMilliseconds") << QString("Date.prototype.getUTCMilliseconds") << QString("getUTCMilliseconds");
    QTest::newRow("Date.prototype.getTimezoneOffset") << QString("Date.prototype.getTimezoneOffset") << QString("getTimezoneOffset");
    QTest::newRow("Date.prototype.setTime") << QString("Date.prototype.setTime") << QString("setTime");
    QTest::newRow("Date.prototype.setMilliseconds") << QString("Date.prototype.setMilliseconds") << QString("setMilliseconds");
    QTest::newRow("Date.prototype.setUTCMilliseconds") << QString("Date.prototype.setUTCMilliseconds") << QString("setUTCMilliseconds");
    QTest::newRow("Date.prototype.setSeconds") << QString("Date.prototype.setSeconds") << QString("setSeconds");
    QTest::newRow("Date.prototype.setUTCSeconds") << QString("Date.prototype.setUTCSeconds") << QString("setUTCSeconds");
    QTest::newRow("Date.prototype.setMinutes") << QString("Date.prototype.setMinutes") << QString("setMinutes");
    QTest::newRow("Date.prototype.setUTCMinutes") << QString("Date.prototype.setUTCMinutes") << QString("setUTCMinutes");
    QTest::newRow("Date.prototype.setHours") << QString("Date.prototype.setHours") << QString("setHours");
    QTest::newRow("Date.prototype.setUTCHours") << QString("Date.prototype.setUTCHours") << QString("setUTCHours");
    QTest::newRow("Date.prototype.setDate") << QString("Date.prototype.setDate") << QString("setDate");
    QTest::newRow("Date.prototype.setUTCDate") << QString("Date.prototype.setUTCDate") << QString("setUTCDate");
    QTest::newRow("Date.prototype.setMonth") << QString("Date.prototype.setMonth") << QString("setMonth");
    QTest::newRow("Date.prototype.setUTCMonth") << QString("Date.prototype.setUTCMonth") << QString("setUTCMonth");
    QTest::newRow("Date.prototype.setYear") << QString("Date.prototype.setYear") << QString("setYear");
    QTest::newRow("Date.prototype.setFullYear") << QString("Date.prototype.setFullYear") << QString("setFullYear");
    QTest::newRow("Date.prototype.setUTCFullYear") << QString("Date.prototype.setUTCFullYear") << QString("setUTCFullYear");
    QTest::newRow("Date.prototype.toUTCString") << QString("Date.prototype.toUTCString") << QString("toUTCString");
    QTest::newRow("Date.prototype.toGMTString") << QString("Date.prototype.toGMTString") << QString("toGMTString");

    QTest::newRow("Error") << QString("Error") << QString("Error");
    QTest::newRow("Error.prototype.backtrace") << QString("Error.prototype.backtrace") << QString("backtrace");
    QTest::newRow("Error.prototype.toString") << QString("Error.prototype.toString") << QString("toString");

    QTest::newRow("EvalError") << QString("EvalError") << QString("EvalError");
    QTest::newRow("RangeError") << QString("RangeError") << QString("RangeError");
    QTest::newRow("ReferenceError") << QString("ReferenceError") << QString("ReferenceError");
    QTest::newRow("SyntaxError") << QString("SyntaxError") << QString("SyntaxError");
    QTest::newRow("TypeError") << QString("TypeError") << QString("TypeError");
    QTest::newRow("URIError") << QString("URIError") << QString("URIError");

    QTest::newRow("Function") << QString("Function") << QString("Function");
    QTest::newRow("Function.prototype.toString") << QString("Function.prototype.toString") << QString("toString");
    QTest::newRow("Function.prototype.apply") << QString("Function.prototype.apply") << QString("apply");
    QTest::newRow("Function.prototype.call") << QString("Function.prototype.call") << QString("call");
    QTest::newRow("Function.prototype.connect") << QString("Function.prototype.connect") << QString("connect");
    QTest::newRow("Function.prototype.disconnect") << QString("Function.prototype.disconnect") << QString("disconnect");

    QTest::newRow("Math.abs") << QString("Math.abs") << QString("abs");
    QTest::newRow("Math.acos") << QString("Math.acos") << QString("acos");
    QTest::newRow("Math.asin") << QString("Math.asin") << QString("asin");
    QTest::newRow("Math.atan") << QString("Math.atan") << QString("atan");
    QTest::newRow("Math.atan2") << QString("Math.atan2") << QString("atan2");
    QTest::newRow("Math.ceil") << QString("Math.ceil") << QString("ceil");
    QTest::newRow("Math.cos") << QString("Math.cos") << QString("cos");
    QTest::newRow("Math.exp") << QString("Math.exp") << QString("exp");
    QTest::newRow("Math.floor") << QString("Math.floor") << QString("floor");
    QTest::newRow("Math.log") << QString("Math.log") << QString("log");
    QTest::newRow("Math.max") << QString("Math.max") << QString("max");
    QTest::newRow("Math.min") << QString("Math.min") << QString("min");
    QTest::newRow("Math.pow") << QString("Math.pow") << QString("pow");
    QTest::newRow("Math.random") << QString("Math.random") << QString("random");
    QTest::newRow("Math.round") << QString("Math.round") << QString("round");
    QTest::newRow("Math.sin") << QString("Math.sin") << QString("sin");
    QTest::newRow("Math.sqrt") << QString("Math.sqrt") << QString("sqrt");
    QTest::newRow("Math.tan") << QString("Math.tan") << QString("tan");

    QTest::newRow("Number") << QString("Number") << QString("Number");
    QTest::newRow("Number.prototype.toString") << QString("Number.prototype.toString") << QString("toString");
    QTest::newRow("Number.prototype.toLocaleString") << QString("Number.prototype.toLocaleString") << QString("toLocaleString");
    QTest::newRow("Number.prototype.valueOf") << QString("Number.prototype.valueOf") << QString("valueOf");
    QTest::newRow("Number.prototype.toFixed") << QString("Number.prototype.toFixed") << QString("toFixed");
    QTest::newRow("Number.prototype.toExponential") << QString("Number.prototype.toExponential") << QString("toExponential");
    QTest::newRow("Number.prototype.toPrecision") << QString("Number.prototype.toPrecision") << QString("toPrecision");

    QTest::newRow("Object") << QString("Object") << QString("Object");
    QTest::newRow("Object.prototype.toString") << QString("Object.prototype.toString") << QString("toString");
    QTest::newRow("Object.prototype.toLocaleString") << QString("Object.prototype.toLocaleString") << QString("toLocaleString");
    QTest::newRow("Object.prototype.valueOf") << QString("Object.prototype.valueOf") << QString("valueOf");
    QTest::newRow("Object.prototype.hasOwnProperty") << QString("Object.prototype.hasOwnProperty") << QString("hasOwnProperty");
    QTest::newRow("Object.prototype.isPrototypeOf") << QString("Object.prototype.isPrototypeOf") << QString("isPrototypeOf");
    QTest::newRow("Object.prototype.propertyIsEnumerable") << QString("Object.prototype.propertyIsEnumerable") << QString("propertyIsEnumerable");
    QTest::newRow("Object.prototype.__defineGetter__") << QString("Object.prototype.__defineGetter__") << QString("__defineGetter__");
    QTest::newRow("Object.prototype.__defineSetter__") << QString("Object.prototype.__defineSetter__") << QString("__defineSetter__");

    QTest::newRow("RegExp") << QString("RegExp") << QString("RegExp");
    QTest::newRow("RegExp.prototype.exec") << QString("RegExp.prototype.exec") << QString("exec");
    QTest::newRow("RegExp.prototype.test") << QString("RegExp.prototype.test") << QString("test");
    QTest::newRow("RegExp.prototype.toString") << QString("RegExp.prototype.toString") << QString("toString");

    QTest::newRow("String") << QString("String") << QString("String");
    QTest::newRow("String.prototype.toString") << QString("String.prototype.toString") << QString("toString");
    QTest::newRow("String.prototype.valueOf") << QString("String.prototype.valueOf") << QString("valueOf");
    QTest::newRow("String.prototype.charAt") << QString("String.prototype.charAt") << QString("charAt");
    QTest::newRow("String.prototype.charCodeAt") << QString("String.prototype.charCodeAt") << QString("charCodeAt");
    QTest::newRow("String.prototype.concat") << QString("String.prototype.concat") << QString("concat");
    QTest::newRow("String.prototype.indexOf") << QString("String.prototype.indexOf") << QString("indexOf");
    QTest::newRow("String.prototype.lastIndexOf") << QString("String.prototype.lastIndexOf") << QString("lastIndexOf");
    QTest::newRow("String.prototype.localeCompare") << QString("String.prototype.localeCompare") << QString("localeCompare");
    QTest::newRow("String.prototype.match") << QString("String.prototype.match") << QString("match");
    QTest::newRow("String.prototype.replace") << QString("String.prototype.replace") << QString("replace");
    QTest::newRow("String.prototype.search") << QString("String.prototype.search") << QString("search");
    QTest::newRow("String.prototype.slice") << QString("String.prototype.slice") << QString("slice");
    QTest::newRow("String.prototype.split") << QString("String.prototype.split") << QString("split");
    QTest::newRow("String.prototype.substring") << QString("String.prototype.substring") << QString("substring");
    QTest::newRow("String.prototype.toLowerCase") << QString("String.prototype.toLowerCase") << QString("toLowerCase");
    QTest::newRow("String.prototype.toLocaleLowerCase") << QString("String.prototype.toLocaleLowerCase") << QString("toLocaleLowerCase");
    QTest::newRow("String.prototype.toUpperCase") << QString("String.prototype.toUpperCase") << QString("toUpperCase");
    QTest::newRow("String.prototype.toLocaleUpperCase") << QString("String.prototype.toLocaleUpperCase") << QString("toLocaleUpperCase");
}

class CallSpy : public QScriptEngineAgent
{
public:
    CallSpy(QScriptEngine *engine) : QScriptEngineAgent(engine)
    { engine->setAgent(this); }

    void functionEntry(qint64 scriptId)
    {
        if (functionName.isEmpty() && engine()->currentContext()->parentContext()) {
            QScriptContextInfo info(engine()->currentContext());
            functionName = info.functionName();
            expectedScriptId = scriptId;
            actualScriptId = info.scriptId();
        }
    }

    qint64 expectedScriptId;
    qint64 actualScriptId;
    QString functionName;
};

void tst_QScriptContextInfo::builtinFunctionNames()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedName);
    QScriptEngine eng;
    CallSpy *spy = new CallSpy(&eng);
    (void)eng.evaluate(QString::fromLatin1("%0()").arg(expression));
    QCOMPARE(spy->functionName, expectedName);
    QCOMPARE(spy->actualScriptId, spy->expectedScriptId);
}

void tst_QScriptContextInfo::nullContext()
{
    QScriptContextInfo info((QScriptContext*)0);
    QVERIFY(info.isNull());
    QCOMPARE(info.columnNumber(), -1);
    QCOMPARE(info.scriptId(), (qint64)-1);
    QCOMPARE(info.fileName(), QString());
    QCOMPARE(info.functionEndLineNumber(), -1);
    QCOMPARE(info.functionMetaIndex(), -1);
    QCOMPARE(info.functionName(), QString());
    QCOMPARE(info.functionParameterNames().size(), 0);
    QCOMPARE(info.functionStartLineNumber(), -1);
    QCOMPARE(info.functionType(), QScriptContextInfo::NativeFunction);
}

void tst_QScriptContextInfo::streaming()
{
    {
        QScriptContextInfo info((QScriptContext*)0);
        QByteArray ba;
        QDataStream stream(&ba, QIODevice::ReadWrite);
        stream << info;
        stream.device()->seek(0);
        QScriptContextInfo info2;
        stream >> info2;
        QVERIFY(stream.device()->atEnd());
        QCOMPARE(info.functionType(), info2.functionType());
        QCOMPARE(info.scriptId(), info2.scriptId());
        QCOMPARE(info.fileName(), info2.fileName());
        QCOMPARE(info.lineNumber(), info2.lineNumber());
        QCOMPARE(info.columnNumber(), info2.columnNumber());
        QCOMPARE(info.functionName(), info2.functionName());
        QCOMPARE(info.functionEndLineNumber(), info2.functionEndLineNumber());
        QCOMPARE(info.functionStartLineNumber(), info2.functionStartLineNumber());
        QCOMPARE(info.functionParameterNames(), info2.functionParameterNames());
        QCOMPARE(info.functionMetaIndex(), info2.functionMetaIndex());
    }
    {
        QScriptEngine eng;
        eng.globalObject().setProperty("getContextInfoList", eng.newFunction(getContextInfoList));

        QString fileName = "ciao.qs";
        int lineNumber = 456;
        QScriptValue ret = eng.evaluate("function bar(a, b, c) {\n return getContextInfoList();\n}\nbar()",
                                        fileName, lineNumber);
        QList<QScriptContextInfo> lst = qscriptvalue_cast<QList<QScriptContextInfo> >(ret);
        QCOMPARE(lst.size(), 3);
        for (int i = 0; i < lst.size(); ++i) {
            const QScriptContextInfo &info = lst.at(i);
            QByteArray ba;
            QDataStream stream(&ba, QIODevice::ReadWrite);
            stream << info;
            stream.device()->seek(0);
            QScriptContextInfo info2;
            stream >> info2;
            QVERIFY(stream.device()->atEnd());
            QCOMPARE(info.functionType(), info2.functionType());
            QCOMPARE(info.scriptId(), info2.scriptId());
            QCOMPARE(info.fileName(), info2.fileName());
            QCOMPARE(info.lineNumber(), info2.lineNumber());
            QCOMPARE(info.columnNumber(), info2.columnNumber());
            QCOMPARE(info.functionName(), info2.functionName());
            QCOMPARE(info.functionEndLineNumber(), info2.functionEndLineNumber());
            QCOMPARE(info.functionStartLineNumber(), info2.functionStartLineNumber());
            QCOMPARE(info.functionParameterNames(), info2.functionParameterNames());
            QCOMPARE(info.functionMetaIndex(), info2.functionMetaIndex());
        }
    }
}

QTEST_MAIN(tst_QScriptContextInfo)
#include "tst_qscriptcontextinfo.moc"
