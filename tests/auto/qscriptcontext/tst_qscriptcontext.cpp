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


#include <QtTest/QtTest>

#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(QScriptValueList)

QT_BEGIN_NAMESPACE
extern bool qt_script_isJITEnabled();
QT_END_NAMESPACE

class tst_QScriptContext : public QObject
{
    Q_OBJECT

public:
    tst_QScriptContext();
    virtual ~tst_QScriptContext();

private slots:
    void callee();
    void arguments();
    void thisObject();
    void returnValue();
    void throwError();
    void throwValue();
    void evaluateInFunction();
    void pushAndPopContext();
    void lineNumber();
    void backtrace_data();
    void backtrace();
    void scopeChain();
    void pushAndPopScope();
    void getSetActivationObject();
    void inheritActivationAndThisObject();
    void toString();
    void calledAsConstructor();
    void argumentsObjectInNative();
    void jsActivationObject();
    void qobjectAsActivationObject();
    void parentContextCallee_QT2270();
    void popNativeContextScope();
};

tst_QScriptContext::tst_QScriptContext()
{
}

tst_QScriptContext::~tst_QScriptContext()
{
}

static QScriptValue get_callee(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->callee();
}

static QScriptValue store_callee_and_return_primitive(QScriptContext *ctx, QScriptEngine *eng)
{
    ctx->thisObject().setProperty("callee", ctx->callee());
    return QScriptValue(eng, 123);
}

void tst_QScriptContext::callee()
{
    QScriptEngine eng;

    {
        QScriptValue fun = eng.newFunction(get_callee);
        fun.setProperty("foo", QScriptValue(&eng, "bar"));
        eng.globalObject().setProperty("get_callee", fun);

        QScriptValue result = eng.evaluate("get_callee()");
        QCOMPARE(result.isFunction(), true);
        QCOMPARE(result.property("foo").toString(), QString("bar"));
    }

    // callee when toPrimitive() is called internally
    {
        QScriptValue fun = eng.newFunction(store_callee_and_return_primitive);
        QScriptValue obj = eng.newObject();
        obj.setProperty("toString", fun);
        QVERIFY(!obj.property("callee").isValid());
        (void)obj.toString();
        QVERIFY(obj.property("callee").isFunction());
        QVERIFY(obj.property("callee").strictlyEquals(fun));

        obj.setProperty("callee", QScriptValue());
        QVERIFY(!obj.property("callee").isValid());
        obj.setProperty("valueOf", fun);
        (void)obj.toNumber();
        QVERIFY(obj.property("callee").isFunction());
        QVERIFY(obj.property("callee").strictlyEquals(fun));
    }
}

static QScriptValue get_arguments(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue array = eng->newArray();
    for (int i = 0; i < ctx->argumentCount(); ++i)
        array.setProperty(QString::number(i), ctx->argument(i));
    return array;
}

static QScriptValue get_argumentsObject(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->argumentsObject();
}

void tst_QScriptContext::arguments()
{
    QScriptEngine eng;

    {
        QScriptValue args = eng.currentContext()->argumentsObject();
        QVERIFY(args.isObject());
        QCOMPARE(args.property("length").toInt32(), 0);
    }
    {
        QScriptValue fun = eng.newFunction(get_arguments);
        eng.globalObject().setProperty("get_arguments", fun);
    }

    for (int x = 0; x < 2; ++x) {
        QString prefix;
        if (x == 0)
            prefix = "";
        else
            prefix = "new ";
        {
            QScriptValue result = eng.evaluate(prefix+"get_arguments()");
            QCOMPARE(result.isArray(), true);
            QCOMPARE(result.property("length").toUInt32(), quint32(0));
        }

        {
            QScriptValue result = eng.evaluate(prefix+"get_arguments(123)");
            QCOMPARE(result.isArray(), true);
            QCOMPARE(result.property("length").toUInt32(), quint32(1));
            QCOMPARE(result.property("0").isNumber(), true);
            QCOMPARE(result.property("0").toNumber(), 123.0);
        }

        {
            QScriptValue result = eng.evaluate(prefix+"get_arguments(\"ciao\", null, true, undefined)");
            QCOMPARE(result.isArray(), true);
            QCOMPARE(result.property("length").toUInt32(), quint32(4));
            QCOMPARE(result.property("0").isString(), true);
            QCOMPARE(result.property("0").toString(), QString("ciao"));
            QCOMPARE(result.property("1").isNull(), true);
            QCOMPARE(result.property("2").isBoolean(), true);
            QCOMPARE(result.property("2").toBoolean(), true);
            QCOMPARE(result.property("3").isUndefined(), true);
        }

        {
            QScriptValue fun = eng.newFunction(get_argumentsObject);
            eng.globalObject().setProperty("get_argumentsObject", fun);
        }

        {
            QScriptValue fun = eng.evaluate("get_argumentsObject");
            QCOMPARE(fun.isFunction(), true);
            QScriptValue result = eng.evaluate(prefix+"get_argumentsObject()");
            QCOMPARE(result.isArray(), false);
            QVERIFY(result.isObject());
            QCOMPARE(result.property("length").toUInt32(), quint32(0));
            QCOMPARE(result.propertyFlags("length"), QScriptValue::SkipInEnumeration);
            QCOMPARE(result.property("callee").strictlyEquals(fun), true);
            QCOMPARE(result.propertyFlags("callee"), QScriptValue::SkipInEnumeration);
            QScriptValue replacedCallee(&eng, 123);
            result.setProperty("callee", replacedCallee);
            QVERIFY(result.property("callee").equals(replacedCallee));
            QScriptValue replacedLength(&eng, 456);
            result.setProperty("length", replacedLength);
            QVERIFY(result.property("length").equals(replacedLength));
            result.setProperty("callee", QScriptValue());
            QVERIFY(!result.property("callee").isValid());
            result.setProperty("length", QScriptValue());
            QVERIFY(!result.property("length").isValid());
        }

        {
            QScriptValue result = eng.evaluate(prefix+"get_argumentsObject(123)");
            eng.evaluate("function nestedArg(x,y,z) { var w = get_argumentsObject('ABC' , x+y+z); return w; }");
            QScriptValue result2 = eng.evaluate("nestedArg(1, 'a', 2)");
            QCOMPARE(result.isArray(), false);
            QVERIFY(result.isObject());
            QCOMPARE(result.property("length").toUInt32(), quint32(1));
            QCOMPARE(result.property("0").isNumber(), true);
            QCOMPARE(result.property("0").toNumber(), 123.0);
            QVERIFY(result2.isObject());
            QCOMPARE(result2.property("length").toUInt32(), quint32(2));
            QCOMPARE(result2.property("0").toString(), QString::fromLatin1("ABC"));
            QCOMPARE(result2.property("1").toString(), QString::fromLatin1("1a2"));
        }

        {
            QScriptValue result = eng.evaluate(prefix+"get_argumentsObject(\"ciao\", null, true, undefined)");
            QCOMPARE(result.isArray(), false);
            QCOMPARE(result.property("length").toUInt32(), quint32(4));
            QCOMPARE(result.property("0").isString(), true);
            QCOMPARE(result.property("0").toString(), QString("ciao"));
            QCOMPARE(result.property("1").isNull(), true);
            QCOMPARE(result.property("2").isBoolean(), true);
            QCOMPARE(result.property("2").toBoolean(), true);
            QCOMPARE(result.property("3").isUndefined(), true);
        }

        // arguments object returned from script
        {
            QScriptValue result = eng.evaluate("(function() { return arguments; })(123)");
            QCOMPARE(result.isArray(), false);
            QVERIFY(result.isObject());
            QCOMPARE(result.property("length").toUInt32(), quint32(1));
            QCOMPARE(result.property("0").isNumber(), true);
            QCOMPARE(result.property("0").toNumber(), 123.0);
        }

        {
            QScriptValue result = eng.evaluate("(function() { return arguments; })('ciao', null, true, undefined)");
            QCOMPARE(result.isArray(), false);
            QCOMPARE(result.property("length").toUInt32(), quint32(4));
            QCOMPARE(result.property("0").isString(), true);
            QCOMPARE(result.property("0").toString(), QString("ciao"));
            QCOMPARE(result.property("1").isNull(), true);
            QCOMPARE(result.property("2").isBoolean(), true);
            QCOMPARE(result.property("2").toBoolean(), true);
            QCOMPARE(result.property("3").isUndefined(), true);
        }
    }
}

static QScriptValue get_thisObject(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->thisObject();
}

void tst_QScriptContext::thisObject()
{
    QScriptEngine eng;

    QScriptValue fun = eng.newFunction(get_thisObject);
    eng.globalObject().setProperty("get_thisObject", fun);

    {
        QScriptValue result = eng.evaluate("get_thisObject()");
        QCOMPARE(result.isObject(), true);
        QCOMPARE(result.toString(), QString("[object global]"));
    }

    {
        QScriptValue result = eng.evaluate("get_thisObject.apply(new Number(123))");
        QCOMPARE(result.isObject(), true);
        QCOMPARE(result.toNumber(), 123.0);
    }

    {
        QScriptValue obj = eng.newObject();
        eng.currentContext()->setThisObject(obj);
        QVERIFY(eng.currentContext()->thisObject().equals(obj));
        eng.currentContext()->setThisObject(QScriptValue());
        QVERIFY(eng.currentContext()->thisObject().equals(obj));

        QScriptEngine eng2;
        QScriptValue obj2 = eng2.newObject();
        QTest::ignoreMessage(QtWarningMsg, "QScriptContext::setThisObject() failed: cannot set an object created in a different engine");
        eng.currentContext()->setThisObject(obj2);
    }
}

void tst_QScriptContext::returnValue()
{
    QSKIP("Internal function not implemented in JSC-based back-end", SkipAll);
    QScriptEngine eng;
    eng.evaluate("123");
    QCOMPARE(eng.currentContext()->returnValue().toNumber(), 123.0);
    eng.evaluate("\"ciao\"");
    QCOMPARE(eng.currentContext()->returnValue().toString(), QString("ciao"));
}

static QScriptValue throw_Error(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::UnknownError, "foo");
}

static QScriptValue throw_TypeError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::TypeError, "foo");
}

static QScriptValue throw_ReferenceError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::ReferenceError, "foo");
}

static QScriptValue throw_SyntaxError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::SyntaxError, "foo");
}

static QScriptValue throw_RangeError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::RangeError, "foo");
}

static QScriptValue throw_URIError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(QScriptContext::URIError, "foo");
}

static QScriptValue throw_ErrorAndReturnUndefined(QScriptContext *ctx, QScriptEngine *eng)
{
    ctx->throwError(QScriptContext::UnknownError, "foo");
    return eng->undefinedValue();
}

void tst_QScriptContext::throwError()
{
    QScriptEngine eng;

    {
        QScriptValue fun = eng.newFunction(throw_Error);
        eng.globalObject().setProperty("throw_Error", fun);
        QScriptValue result = eng.evaluate("throw_Error()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("Error: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_TypeError);
        eng.globalObject().setProperty("throw_TypeError", fun);
        QScriptValue result = eng.evaluate("throw_TypeError()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("TypeError: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_ReferenceError);
        eng.globalObject().setProperty("throw_ReferenceError", fun);
        QScriptValue result = eng.evaluate("throw_ReferenceError()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("ReferenceError: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_SyntaxError);
        eng.globalObject().setProperty("throw_SyntaxError", fun);
        QScriptValue result = eng.evaluate("throw_SyntaxError()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("SyntaxError: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_RangeError);
        eng.globalObject().setProperty("throw_RangeError", fun);
        QScriptValue result = eng.evaluate("throw_RangeError()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("RangeError: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_URIError);
        eng.globalObject().setProperty("throw_URIError", fun);
        QScriptValue result = eng.evaluate("throw_URIError()");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(result.isError(), true);
        QCOMPARE(result.toString(), QString("URIError: foo"));
    }

    {
        QScriptValue fun = eng.newFunction(throw_ErrorAndReturnUndefined);
        eng.globalObject().setProperty("throw_ErrorAndReturnUndefined", fun);
        QScriptValue result = eng.evaluate("throw_ErrorAndReturnUndefined()");
        QVERIFY(eng.hasUncaughtException());
        QVERIFY(result.isError());
        QCOMPARE(result.toString(), QString("Error: foo"));
    }

}

static QScriptValue throw_value(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwValue(ctx->argument(0));
}

void tst_QScriptContext::throwValue()
{
    QScriptEngine eng;

    QScriptValue fun = eng.newFunction(throw_value);
    eng.globalObject().setProperty("throw_value", fun);

    {
        QScriptValue result = eng.evaluate("throw_value(123)");
        QCOMPARE(result.isError(), false);
        QCOMPARE(result.toNumber(), 123.0);
        QCOMPARE(eng.hasUncaughtException(), true);
    }
}

static QScriptValue evaluate(QScriptContext *, QScriptEngine *eng)
{
    return eng->evaluate("a = 123; a");
//    return eng->evaluate("a");
}

void tst_QScriptContext::evaluateInFunction()
{
    QScriptEngine eng;

    QScriptValue fun = eng.newFunction(evaluate);
    eng.globalObject().setProperty("evaluate", fun);

    QScriptValue result = eng.evaluate("evaluate()");
    QCOMPARE(result.isError(), false);
    QCOMPARE(result.isNumber(), true);
    QCOMPARE(result.toNumber(), 123.0);
    QCOMPARE(eng.hasUncaughtException(), false);

    QCOMPARE(eng.evaluate("a").toNumber(), 123.0);
}

void tst_QScriptContext::pushAndPopContext()
{
    QScriptEngine eng;
    QScriptContext *topLevel = eng.currentContext();
    QCOMPARE(topLevel->engine(), &eng);

    QScriptContext *ctx = eng.pushContext();
    QVERIFY(ctx != 0);
    QCOMPARE(ctx->parentContext(), topLevel);
    QCOMPARE(eng.currentContext(), ctx);
    QCOMPARE(ctx->engine(), &eng);
    QCOMPARE(ctx->state(), QScriptContext::NormalState);
    QCOMPARE(ctx->isCalledAsConstructor(), false);
    QCOMPARE(ctx->argumentCount(), 0);
    QCOMPARE(ctx->argument(0).isUndefined(), true);
    QVERIFY(!ctx->argument(-1).isValid());
    QCOMPARE(ctx->argumentsObject().isObject(), true);
    QCOMPARE(ctx->activationObject().isObject(), true);
    QCOMPARE(ctx->callee().isValid(), false);
    QCOMPARE(ctx->thisObject().strictlyEquals(eng.globalObject()), true);
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).equals(ctx->activationObject()));
    QVERIFY(ctx->scopeChain().at(1).equals(eng.globalObject()));

    QScriptContext *ctx2 = eng.pushContext();
    QCOMPARE(ctx2->parentContext(), ctx);
    QCOMPARE(eng.currentContext(), ctx2);

    eng.popContext();
    QCOMPARE(eng.currentContext(), ctx);
    eng.popContext();
    QCOMPARE(eng.currentContext(), topLevel);

    // popping the top-level context is not allowed
    QTest::ignoreMessage(QtWarningMsg, "QScriptEngine::popContext() doesn't match with pushContext()");
    eng.popContext();
    QCOMPARE(eng.currentContext(), topLevel);

    {
        QScriptContext *ctx3 = eng.pushContext();
        ctx3->activationObject().setProperty("foo", QScriptValue(&eng, 123));
        QVERIFY(eng.evaluate("foo").strictlyEquals(QScriptValue(&eng, 123)));
        eng.evaluate("var bar = 'ciao'");
        QVERIFY(ctx3->activationObject().property("bar", QScriptValue::ResolveLocal).strictlyEquals(QScriptValue(&eng, "ciao")));
        eng.popContext();
    }

    {
        QScriptContext *ctx4 = eng.pushContext();
        QScriptValue obj = eng.newObject();
        obj.setProperty("prop", QScriptValue(&eng, 456));
        ctx4->setThisObject(obj);
        QScriptValue ret = eng.evaluate("var tmp = this.prop; tmp + 1");
        QCOMPARE(eng.currentContext(), ctx4);
        QVERIFY(ret.strictlyEquals(QScriptValue(&eng, 457)));
        eng.popContext();
    }

    // throwing an exception
    {
        QScriptContext *ctx5 = eng.pushContext();
        QScriptValue ret = eng.evaluate("throw new Error('oops')");
        QVERIFY(ret.isError());
        QVERIFY(eng.hasUncaughtException());
        QCOMPARE(eng.currentContext(), ctx5);
        eng.popContext();
    }
}

void tst_QScriptContext::popNativeContextScope()
{
    QScriptEngine eng;
    QScriptContext *ctx = eng.pushContext();
    QVERIFY(ctx->popScope().isObject()); // the activation object

    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));
    // This was different in 4.5: scope and activation were decoupled
    QVERIFY(ctx->activationObject().strictlyEquals(eng.globalObject()));

    QVERIFY(!eng.evaluate("var foo = 123; function bar() {}").isError());
    QVERIFY(eng.globalObject().property("foo").isNumber());
    QVERIFY(eng.globalObject().property("bar").isFunction());

    QScriptValue customScope = eng.newObject();
    ctx->pushScope(customScope);
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(customScope));
    QVERIFY(ctx->scopeChain().at(1).strictlyEquals(eng.globalObject()));
    QVERIFY(ctx->activationObject().strictlyEquals(eng.globalObject()));
    ctx->setActivationObject(customScope);
    QVERIFY(ctx->activationObject().strictlyEquals(customScope));
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(customScope));
    QEXPECT_FAIL("", "QTBUG-11012", Continue);
    QVERIFY(ctx->scopeChain().at(1).strictlyEquals(eng.globalObject()));

    QVERIFY(!eng.evaluate("baz = 456; var foo = 789; function barbar() {}").isError());
    QEXPECT_FAIL("", "QTBUG-11012", Continue);
    QVERIFY(eng.globalObject().property("baz").isNumber());
    QVERIFY(customScope.property("foo").isNumber());
    QVERIFY(customScope.property("barbar").isFunction());

    QVERIFY(ctx->popScope().strictlyEquals(customScope));
    QCOMPARE(ctx->scopeChain().size(), 1);
    QEXPECT_FAIL("", "QTBUG-11012", Continue);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    // Need to push another object, otherwise we crash in popContext() (QTBUG-11012)
    ctx->pushScope(customScope);
    eng.popContext();
}

void tst_QScriptContext::lineNumber()
{
    QScriptEngine eng;

    QScriptValue result = eng.evaluate("try { eval(\"foo = 123;\\n this[is{a{syntax|error@#$%@#% \"); } catch (e) { e.lineNumber; }", "foo.qs", 123);
    QVERIFY(!eng.hasUncaughtException());
    QVERIFY(result.isNumber());
    QCOMPARE(result.toInt32(), 2);

    result = eng.evaluate("foo = 123;\n bar = 42\n0 = 0");
    QVERIFY(eng.hasUncaughtException());
    QCOMPARE(eng.uncaughtExceptionLineNumber(), 3);
    QCOMPARE(result.property("lineNumber").toInt32(), 3);
}

static QScriptValue getBacktrace(QScriptContext *ctx, QScriptEngine *eng)
{
    return qScriptValueFromValue(eng, ctx->backtrace());
}

static QScriptValue custom_eval(QScriptContext *ctx, QScriptEngine *eng)
{
    return eng->evaluate(ctx->argumentsObject().property(0).toString(), ctx->argumentsObject().property(1).toString());
}

static QScriptValue custom_call(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->argumentsObject().property(0).call(QScriptValue(), QScriptValueList() << ctx->argumentsObject().property(1));
}

static QScriptValue native_recurse(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue func = ctx->argumentsObject().property(0);
    QScriptValue n = ctx->argumentsObject().property(1);

    if(n.toUInt32() <= 1) {
        return func.call(QScriptValue(), QScriptValueList());
    } else {
        return eng->evaluate("native_recurse").call(QScriptValue(),
                                                    QScriptValueList() << func << QScriptValue(n.toUInt32() - 1));
    }
}

void tst_QScriptContext::backtrace_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<QStringList>("expectedbacktrace");

    {
        QString source(
                 "function foo() {\n"
                 "  return bt(123);\n"
                 "}\n"
                 "foo('hello', { })\n"
                 "var r = 0;");

        QStringList expected;
        expected << "<native>(123) at -1"
                 << "foo('hello', [object Object]) at testfile:2"
                 << "<global>() at testfile:4";


        QTest::newRow("simple") << source << expected;
    }

    {
        QStringList expected;
        QString source = QString(
            "function foo(arg1 , arg2) {\n"
            "  return eval(\"%1\");\n"
            "}\n"
            "foo('hello', 456)\n"
            "var a = 0;"
           ).arg("\\n \\n bt('hey'); \\n");

           expected << "<native>('hey') at -1"
                    << "<eval>() at 3"
                    << QString::fromLatin1("foo(arg1 = 'hello', arg2 = 456) at testfile:%0")
               // interpreter unfortunately doesn't provide line number for eval()
               .arg(qt_script_isJITEnabled() ? 2 : -1);
           expected
                    << "<global>() at testfile:4";

            QTest::newRow("eval") << source << expected;
    }

    {
        QString eval_code(
                "function bar(a) {\\n"
                "  return bt('m');\\n"
                "}\\n"
                "bar('b'); \\n");
        QString source = QString(
                "function foo() {\n"
                "  return custom_eval(\"%1\", 'eval.js');\n"
                "}\n"
                "foo()"
            ).arg(eval_code);

        QStringList expected;
        expected << "<native>('m') at -1"
                 << "bar(a = 'b') at eval.js:2"
                 << "<eval>() at eval.js:4"
                 << QString("<native>('%1', 'eval.js') at -1").arg(eval_code.replace("\\n", "\n"))
                 << "foo() at testfile:2"
                 << "<global>() at testfile:4";

        QTest::newRow("custom_eval") << source << expected;
    }
    {
        QString f("function (a) {\n return bt(a); \n  }");
        QString source = QString(
                "function foo(f) {\n"
                "  return f('b');\n"
                "}\n"
                "foo(%1)"
            ).arg(f);

        QStringList expected;
        expected << "<native>('b') at -1"
                 << "<anonymous>(a = 'b') at testfile:5"
                 << QString("foo(f = %1) at testfile:2").arg(f)
                 << "<global>() at testfile:6";

        QTest::newRow("closure") << source << expected;
    }

    {
        QStringList expected;
        QString source = QString(
            "var o = new Object;\n"
            "o.foo = function plop() {\n"
            "  return eval(\"%1\");\n"
            "}\n"
            "o.foo('hello', 456)\n"
           ).arg("\\n \\n bt('hey'); \\n");

           expected << "<native>('hey') at -1"
                    << "<eval>() at 3"
                    << QString::fromLatin1("plop('hello', 456) at testfile:%0")
               // interpreter unfortunately doesn't provide line number for eval()
               .arg(qt_script_isJITEnabled() ? 3 : -1);
           expected
                    << "<global>() at testfile:5";

            QTest::newRow("eval in member") << source << expected;
    }

    {
        QString source(
                 "function foo(a) {\n"
                 "  return bt(123);\n"
                 "}\n"
                 "function bar() {\n"
                 "  var v = foo('arg', 4);\n"
                 "  return v;\n"
                 "}\n"
                 "bar('hello', { });\n");

        QStringList expected;
        expected << "<native>(123) at -1"
                 << "foo(a = 'arg', 4) at testfile:2"
                 << "bar('hello', [object Object]) at testfile:5"
                 << "<global>() at testfile:8";


        QTest::newRow("two function") << source << expected;
    }

    {
        QString func("function foo(a, b) {\n"
                     "  return bt(a);\n"
                     "}");

        QString source = func + QString::fromLatin1("\n"
                 "custom_call(foo, 'hello');\n"
                 "var a = 1\n");

        QStringList expected;
        expected << "<native>('hello') at -1"
                 << "foo(a = 'hello') at testfile:2"
                 << QString("<native>(%1, 'hello') at -1").arg(func)
                 << "<global>() at testfile:4";

        QTest::newRow("call") << source << expected;
    }

    {
        QString source = QString::fromLatin1("\n"
        "custom_call(bt, 'hello_world');\n"
        "var a = 1\n");

        QStringList expected;
        expected << "<native>('hello_world') at -1"
        << "<native>(function () {\n    [native code]\n}, 'hello_world') at -1"
        << "<global>() at testfile:2";

        QTest::newRow("call native") << source << expected;
    }

    {
        QLatin1String func( "function f1() {\n"
            "    eval('var q = 4');\n"
            "    return custom_call(bt, 22);\n"
            "}");

        QString source = QString::fromLatin1("\n"
            "function f2() {\n"
            "    func = %1\n"
            "    return custom_call(func, 12);\n"
            "}\n"
            "f2();\n").arg(func);

        QStringList expected;
        expected << "<native>(22) at -1"
            << "<native>(function () {\n    [native code]\n}, 22) at -1"
            << "f1(12) at testfile:5"
            << QString::fromLatin1("<native>(%1, 12) at -1").arg(func)
            << "f2() at testfile:7"
            << "<global>() at testfile:9";


        QTest::newRow("calls with closures") << source << expected;
    }

    {
        QLatin1String func( "function js_bt() {\n"
            "    return bt();\n"
            "}");

        QString source = QString::fromLatin1("\n"
            "%1\n"
            "function f() {\n"
            "    return native_recurse(js_bt, 12);\n"
            "}\n"
            "f();\n").arg(func);

        QStringList expected;
        expected << "<native>() at -1" << "js_bt() at testfile:3";
        for(int n = 1; n <= 12; n++) {
            expected << QString::fromLatin1("<native>(%1, %2) at -1")
                .arg(func).arg(n);
        }
        expected << "f() at testfile:6";
        expected << "<global>() at testfile:8";

        QTest::newRow("native recursive") << source << expected;
    }

    {
        QString source = QString::fromLatin1("\n"
            "function finish() {\n"
            "    return bt();\n"
            "}\n"
            "function rec(n) {\n"
            "    if(n <= 1)\n"
            "        return finish();\n"
            "    else\n"
            "        return rec (n - 1);\n"
            "}\n"
            "function f() {\n"
            "    return rec(12);\n"
            "}\n"
            "f();\n");

        QStringList expected;
        expected << "<native>() at -1" << "finish() at testfile:3";
        for(int n = 1; n <= 12; n++) {
            expected << QString::fromLatin1("rec(n = %1) at testfile:%2")
                .arg(n).arg((n==1) ? 7 : 9);
        }
        expected << "f() at testfile:12";
        expected << "<global>() at testfile:14";

        QTest::newRow("js recursive") << source << expected;
    }
}


void tst_QScriptContext::backtrace()
{
    QFETCH(QString, code);
    QFETCH(QStringList, expectedbacktrace);

    QScriptEngine eng;
    eng.globalObject().setProperty("bt", eng.newFunction(getBacktrace));
    eng.globalObject().setProperty("custom_eval", eng.newFunction(custom_eval));
    eng.globalObject().setProperty("custom_call", eng.newFunction(custom_call));
    eng.globalObject().setProperty("native_recurse", eng.newFunction(native_recurse));

    QString fileName = "testfile";
    QScriptValue ret = eng.evaluate(code, fileName);
    QVERIFY(!eng.hasUncaughtException());
    QVERIFY(ret.isArray());
    QStringList slist = qscriptvalue_cast<QStringList>(ret);
    QCOMPARE(slist, expectedbacktrace);
}

static QScriptValue getScopeChain(QScriptContext *ctx, QScriptEngine *eng)
{
    return qScriptValueFromValue(eng, ctx->parentContext()->scopeChain());
}

void tst_QScriptContext::scopeChain()
{
    QScriptEngine eng;
    {
        QScriptValueList ret = eng.currentContext()->scopeChain();
        QCOMPARE(ret.size(), 1);
        QVERIFY(ret.at(0).strictlyEquals(eng.globalObject()));
    }
    {
        eng.globalObject().setProperty("getScopeChain", eng.newFunction(getScopeChain));
        QScriptValueList ret = qscriptvalue_cast<QScriptValueList>(eng.evaluate("getScopeChain()"));
        QCOMPARE(ret.size(), 1);
        QVERIFY(ret.at(0).strictlyEquals(eng.globalObject()));
    }
    {
        eng.evaluate("function foo() { function bar() { return getScopeChain(); } return bar() }");
        QScriptValueList ret = qscriptvalue_cast<QScriptValueList>(eng.evaluate("foo()"));
        QEXPECT_FAIL("", "Number of items in returned scope chain is incorrect", Abort);
        QCOMPARE(ret.size(), 3);
        QVERIFY(ret.at(2).strictlyEquals(eng.globalObject()));
        QCOMPARE(ret.at(1).toString(), QString::fromLatin1("activation"));
        QVERIFY(ret.at(1).property("arguments").isObject());
        QCOMPARE(ret.at(0).toString(), QString::fromLatin1("activation"));
        QVERIFY(ret.at(0).property("arguments").isObject());
    }
    {
        QScriptValueList ret = qscriptvalue_cast<QScriptValueList>(eng.evaluate("o = { x: 123 }; with(o) getScopeChain();"));
        QCOMPARE(ret.size(), 2);
        QVERIFY(ret.at(1).strictlyEquals(eng.globalObject()));
        QVERIFY(ret.at(0).isObject());
        QCOMPARE(ret.at(0).property("x").toInt32(), 123);
    }
    {
        QScriptValueList ret = qscriptvalue_cast<QScriptValueList>(
            eng.evaluate("o1 = { x: 123}; o2 = { y: 456 }; with(o1) { with(o2) { getScopeChain(); } }"));
        QCOMPARE(ret.size(), 3);
        QVERIFY(ret.at(2).strictlyEquals(eng.globalObject()));
        QVERIFY(ret.at(1).isObject());
        QCOMPARE(ret.at(1).property("x").toInt32(), 123);
        QVERIFY(ret.at(0).isObject());
        QCOMPARE(ret.at(0).property("y").toInt32(), 456);
    }
}

void tst_QScriptContext::pushAndPopScope()
{
    QScriptEngine eng;
    QScriptContext *ctx = eng.currentContext();
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    QVERIFY(ctx->popScope().strictlyEquals(eng.globalObject()));
    ctx->pushScope(eng.globalObject());
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    QScriptValue obj = eng.newObject();
    ctx->pushScope(obj);
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(obj));
    QVERIFY(ctx->scopeChain().at(1).strictlyEquals(eng.globalObject()));

    QVERIFY(ctx->popScope().strictlyEquals(obj));
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    {
        QScriptValue ret = eng.evaluate("x");
        QVERIFY(ret.isError());
        eng.clearExceptions();
    }
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    // task 236685
    QScriptValue qobj = eng.newQObject(this, QScriptEngine::QtOwnership, QScriptEngine::AutoCreateDynamicProperties);
    ctx->pushScope(qobj);
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(qobj));
    QVERIFY(ctx->scopeChain().at(1).strictlyEquals(eng.globalObject()));
    {
        QScriptValue ret = eng.evaluate("print");
        QVERIFY(ret.isFunction());
    }
    ctx->popScope();

    ctx->pushScope(obj);
    QCOMPARE(ctx->scopeChain().size(), 2);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(obj));
    obj.setProperty("x", 123);
    {
        QScriptValue ret = eng.evaluate("x");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    QVERIFY(ctx->popScope().strictlyEquals(obj));
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(eng.globalObject()));

    ctx->pushScope(QScriptValue());
    QCOMPARE(ctx->scopeChain().size(), 1);

    QVERIFY(ctx->popScope().strictlyEquals(eng.globalObject()));
    QVERIFY(ctx->scopeChain().isEmpty());

    // Used to work with old back-end, doesn't with new one because JSC requires that the last object in
    // a scope chain is the Global Object.
    QTest::ignoreMessage(QtWarningMsg, "QScriptContext::pushScope() failed: initial object in scope chain has to be the Global Object");
    ctx->pushScope(obj);
    QCOMPARE(ctx->scopeChain().size(), 0);

    QScriptEngine eng2;
    QScriptValue obj2 = eng2.newObject();
    QTest::ignoreMessage(QtWarningMsg, "QScriptContext::pushScope() failed: cannot push an object created in a different engine");
    ctx->pushScope(obj2);
    QVERIFY(ctx->scopeChain().isEmpty());

    QVERIFY(!ctx->popScope().isValid());
}

static QScriptValue get_activationObject(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->activationObject();
}

void tst_QScriptContext::getSetActivationObject()
{
    QScriptEngine eng;
    QScriptContext *ctx = eng.currentContext();
    QVERIFY(ctx->activationObject().equals(eng.globalObject()));

    ctx->setActivationObject(QScriptValue());
    QVERIFY(ctx->activationObject().equals(eng.globalObject()));
    QCOMPARE(ctx->engine(), &eng);

    QScriptValue obj = eng.newObject();
    ctx->setActivationObject(obj);
    QVERIFY(ctx->activationObject().equals(obj));
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).equals(obj));

    {
        QScriptEngine eng2;
        QScriptValue obj2 = eng2.newObject();
        QTest::ignoreMessage(QtWarningMsg, "QScriptContext::setActivationObject() failed: cannot set an object created in a different engine");
        QScriptValue was = ctx->activationObject();
        ctx->setActivationObject(obj2);
        QVERIFY(ctx->activationObject().equals(was));
    }

    ctx->setActivationObject(eng.globalObject());
    QVERIFY(ctx->activationObject().equals(eng.globalObject()));
    QScriptValue fun = eng.newFunction(get_activationObject);
    eng.globalObject().setProperty("get_activationObject", fun);
    {
        QScriptValue ret = eng.evaluate("get_activationObject(1, 2, 3)");
        QVERIFY(ret.isObject());
        QScriptValue arguments = ret.property("arguments");
        QEXPECT_FAIL("", "Getting arguments property of activation object doesn't work", Abort);
        QVERIFY(arguments.isObject());
        QCOMPARE(arguments.property("length").toInt32(), 3);
        QCOMPARE(arguments.property("0").toInt32(), 1);
        QCOMPARE(arguments.property("1").toInt32(), 1);
        QCOMPARE(arguments.property("2").toInt32(), 1);
    }
}

static QScriptValue myEval(QScriptContext *ctx, QScriptEngine *eng)
{
     QString code = ctx->argument(0).toString();
     ctx->setActivationObject(ctx->parentContext()->activationObject());
     ctx->setThisObject(ctx->parentContext()->thisObject());
     return eng->evaluate(code);
}

void tst_QScriptContext::inheritActivationAndThisObject()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("myEval", eng.newFunction(myEval));
    {
        QScriptValue ret = eng.evaluate("var a = 123; myEval('a')");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        QScriptValue ret = eng.evaluate("(function() { return myEval('this'); }).call(Number)");
        QVERIFY(ret.isFunction());
        QVERIFY(ret.equals(eng.globalObject().property("Number")));
    }
    {
        QScriptValue ret = eng.evaluate("(function(a) { return myEval('a'); })(123)");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }

    // QT-2219
    {
        eng.globalObject().setProperty("a", 123);
        QScriptValue ret = eng.evaluate("(function() { myEval('var a = 456'); return a; })()");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 456);
        QEXPECT_FAIL("", "QT-2219: Wrong activation object is returned from native function's parent context", Continue);
        QVERIFY(eng.globalObject().property("a").strictlyEquals(123));
    }
}

static QScriptValue parentContextToString(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->parentContext()->toString();
}

void tst_QScriptContext::toString()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("parentContextToString", eng.newFunction(parentContextToString));
    QScriptValue ret = eng.evaluate("function foo(first, second, third) {\n"
                                    "    return parentContextToString();\n"
                                    "}; foo(1, 2, 3)", "script.qs");
    QVERIFY(ret.isString());
    QCOMPARE(ret.toString(), QString::fromLatin1("foo(first = 1, second = 2, third = 3) at script.qs:2"));
}

static QScriptValue storeCalledAsConstructor(QScriptContext *ctx, QScriptEngine *eng)
{
    ctx->callee().setProperty("calledAsConstructor", ctx->isCalledAsConstructor());
    return eng->undefinedValue();
}

static QScriptValue storeCalledAsConstructorV2(QScriptContext *ctx, QScriptEngine *eng, void *)
{
    ctx->callee().setProperty("calledAsConstructor", ctx->isCalledAsConstructor());
    return eng->undefinedValue();
}

static QScriptValue storeCalledAsConstructorV3(QScriptContext *ctx, QScriptEngine *eng)
{
    ctx->callee().setProperty("calledAsConstructor", ctx->parentContext()->isCalledAsConstructor());
    return eng->undefinedValue();
}

void tst_QScriptContext::calledAsConstructor()
{
    QScriptEngine eng;
    QScriptValue fun1 = eng.newFunction(storeCalledAsConstructor);
    {
        fun1.call();
        QVERIFY(!fun1.property("calledAsConstructor").toBool());
        fun1.construct();
        QVERIFY(fun1.property("calledAsConstructor").toBool());
    }
    {
        QScriptValue fun = eng.newFunction(storeCalledAsConstructorV2, (void*)0);
        fun.call();
        QVERIFY(!fun.property("calledAsConstructor").toBool());
        fun.construct();
        QVERIFY(fun.property("calledAsConstructor").toBool());
    }
    {
        eng.globalObject().setProperty("fun1", fun1);
        eng.evaluate("fun1();");
        QVERIFY(!fun1.property("calledAsConstructor").toBool());
        eng.evaluate("new fun1();");
        QVERIFY(fun1.property("calledAsConstructor").toBool());
    }
    {
        QScriptValue fun3 = eng.newFunction(storeCalledAsConstructorV3);
        eng.globalObject().setProperty("fun3", fun3);
        eng.evaluate("function test() { fun3() }");
        eng.evaluate("test();");
        QVERIFY(!fun3.property("calledAsConstructor").toBool());
        eng.evaluate("new test();");
        if (qt_script_isJITEnabled())
            QEXPECT_FAIL("", "QTBUG-6132: calledAsConstructor is not correctly set for JS functions when JIT is enabled", Continue);
        QVERIFY(fun3.property("calledAsConstructor").toBool());
    }

}

static QScriptValue argumentsObjectInNative_test1(QScriptContext *ctx, QScriptEngine *eng)
{
#define VERIFY(statement) \
    do {\
        if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
            return QString::fromLatin1("Failed "  #statement);\
    } while (0)

    QScriptValue obj = ctx->argumentsObject();
    VERIFY(obj.isObject());
    VERIFY(obj.property(0).toUInt32() == 123);
    VERIFY(obj.property(1).toString() == QString::fromLatin1("456"));

    obj.setProperty(0, "abc");
    VERIFY(eng->evaluate("arguments[0]").toString() == QString::fromLatin1("abc") );

    return QString::fromLatin1("success");
#undef VERIFY
}

void tst_QScriptContext::argumentsObjectInNative()
{
    {
        QScriptEngine eng;
        QScriptValue fun = eng.newFunction(argumentsObjectInNative_test1);
        QScriptValueList args;
        args << QScriptValue(&eng, 123.0);
        args << QScriptValue(&eng, QString::fromLatin1("456"));
        QScriptValue result = fun.call(eng.undefinedValue(), args);
        QVERIFY(!eng.hasUncaughtException());
        QCOMPARE(result.toString(), QString::fromLatin1("success"));
    }
    {
        QScriptEngine eng;
        QScriptValue fun = eng.newFunction(argumentsObjectInNative_test1);
        eng.globalObject().setProperty("func", fun);
        QScriptValue result = eng.evaluate("func(123.0 , 456);");
        QVERIFY(!eng.hasUncaughtException());
        QCOMPARE(result.toString(), QString::fromLatin1("success"));
    }
}

static QScriptValue get_jsActivationObject(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->parentContext()->parentContext()->activationObject();
}

void tst_QScriptContext::jsActivationObject()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("get_jsActivationObject", eng.newFunction(get_jsActivationObject));
    eng.evaluate("function f1() { var w = get_jsActivationObject('arg1');  return w; }");
    eng.evaluate("function f2(x,y,z) { var v1 = 42;\n"
                        //   "function foo() {};\n" //this would avoid JSC to optimize
                        "var v2 = f1(); return v2; }");
    eng.evaluate("function f3() { var v1 = 'nothing'; return f2(1,2,3); }");
    QScriptValue result1 = eng.evaluate("f2('hello', 'useless', 'world')");
    QScriptValue result2 = eng.evaluate("f3()");
    QVERIFY(result1.isObject());
    QEXPECT_FAIL("", "JSC optimize away the activation object", Abort);
    QCOMPARE(result1.property("v1").toInt32() , 42);
    QCOMPARE(result1.property("arguments").property(1).toString() , QString::fromLatin1("useless"));
    QVERIFY(result2.isObject());
    QCOMPARE(result2.property("v1").toInt32() , 42);
    QCOMPARE(result2.property("arguments").property(1).toString() , QString::fromLatin1("2"));
}

void tst_QScriptContext::qobjectAsActivationObject()
{
    QScriptEngine eng;
    QObject object;
    QScriptValue scriptObject = eng.newQObject(&object);
    QScriptContext *ctx = eng.pushContext();
    ctx->setActivationObject(scriptObject);
    QVERIFY(ctx->activationObject().equals(scriptObject));

    QVERIFY(!scriptObject.property("foo").isValid());
    eng.evaluate("function foo() { return 123; }");
    {
        QScriptValue val = scriptObject.property("foo");
        QVERIFY(val.isValid());
        QVERIFY(val.isFunction());
    }
    QVERIFY(!eng.globalObject().property("foo").isValid());

    QVERIFY(!scriptObject.property("bar").isValid());
    eng.evaluate("var bar = 123");
    {
        QScriptValue val = scriptObject.property("bar");
        QVERIFY(val.isValid());
        QVERIFY(val.isNumber());
        QCOMPARE(val.toInt32(), 123);
    }
    QVERIFY(!eng.globalObject().property("bar").isValid());

    {
        QScriptValue val = eng.evaluate("delete foo");
        QVERIFY(val.isBool());
        QVERIFY(val.toBool());
        QVERIFY(!scriptObject.property("foo").isValid());
    }
}

static QScriptValue getParentContextCallee(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->parentContext()->callee();
}

void tst_QScriptContext::parentContextCallee_QT2270()
{
    QScriptEngine engine;
    engine.globalObject().setProperty("getParentContextCallee", engine.newFunction(getParentContextCallee));
    QScriptValue fun = engine.evaluate("(function() { return getParentContextCallee(); })");
    QVERIFY(fun.isFunction());
    QScriptValue callee = fun.call();
    QVERIFY(callee.equals(fun));
}

QTEST_MAIN(tst_QScriptContext)
#include "tst_qscriptcontext.moc"
