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

#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(QScriptValueList)

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
    void backtrace();
    void scopeChain();
    void pushAndPopScope();
    void getSetActivationObject();
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
        }

        {
            QScriptValue result = eng.evaluate(prefix+"get_argumentsObject(123)");
            QCOMPARE(result.isArray(), false);
            QCOMPARE(result.property("length").toUInt32(), quint32(1));
            QCOMPARE(result.property("0").isNumber(), true);
            QCOMPARE(result.property("0").toNumber(), 123.0);
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
    QCOMPARE(ctx->parentContext(), topLevel);
    QCOMPARE(eng.currentContext(), ctx);
    QCOMPARE(ctx->engine(), &eng);
    QCOMPARE(ctx->state(), QScriptContext::NormalState);
    QCOMPARE(ctx->isCalledAsConstructor(), false);
    QCOMPARE(ctx->argumentCount(), 0);
    QCOMPARE(ctx->argument(0).isUndefined(), true);
    QCOMPARE(ctx->argumentsObject().isObject(), true);
    QCOMPARE(ctx->activationObject().isObject(), true);
    QCOMPARE(ctx->callee().isValid(), false);
    QCOMPARE(ctx->thisObject().strictlyEquals(eng.globalObject()), true);

    QScriptContext *ctx2 = eng.pushContext();
    QCOMPARE(ctx2->parentContext(), ctx);
    QCOMPARE(eng.currentContext(), ctx2);

    eng.popContext();
    QCOMPARE(eng.currentContext(), ctx);
    eng.popContext();
    QCOMPARE(eng.currentContext(), topLevel);

    // popping the top-level context is not allowed
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

void tst_QScriptContext::lineNumber()
{
    QScriptEngine eng;

    QScriptValue result = eng.evaluate("try { eval(\"foo = 123;\\n this[is{a{syntax|error@#$%@#% \"); } catch (e) { e.lineNumber; }", "foo.qs", 123);
    QVERIFY(!eng.hasUncaughtException());
    QVERIFY(result.isNumber());
    QCOMPARE(result.toInt32(), 124);

    result = eng.evaluate("foo = 123;\n bar = 42\n0 = 0");
    QVERIFY(eng.hasUncaughtException());
    QCOMPARE(eng.uncaughtExceptionLineNumber(), 3);
    QCOMPARE(result.property("lineNumber").toInt32(), 3);
}

static QScriptValue getBacktrace(QScriptContext *ctx, QScriptEngine *eng)
{
    return qScriptValueFromValue(eng, ctx->backtrace());
}

void tst_QScriptContext::backtrace()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("bt", eng.newFunction(getBacktrace));

    QString fileName = "testfile";
    QStringList expected;
    expected << "<native>(123)@:-1"
             << "foo(hello,[object Object])@testfile:2"
             << "<global>()@testfile:4";

    QScriptValue ret = eng.evaluate(
        "function foo() {\n"
        "  return bt(123);\n"
        "}\n"
        "foo('hello', { })", fileName);

    QVERIFY(ret.isArray());
    QStringList slist = qscriptvalue_cast<QStringList>(ret);
    QCOMPARE(slist, expected);
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
        QCOMPARE(ret.size(), 0); // we aren't evaluating code
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
    QVERIFY(ctx->scopeChain().isEmpty());

    QScriptValue obj = eng.newObject();
    ctx->pushScope(obj);
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(obj));

    QVERIFY(ctx->popScope().strictlyEquals(obj));
    QVERIFY(ctx->scopeChain().isEmpty());

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

    ctx->pushScope(obj);
    QCOMPARE(ctx->scopeChain().size(), 1);
    QVERIFY(ctx->scopeChain().at(0).strictlyEquals(obj));
    QVERIFY(!obj.property("foo").isValid());
    eng.evaluate("function foo() {}");
    // function declarations should always end up in the activation object (ECMA-262, chapter 13)
    QVERIFY(!obj.property("foo").isValid());
    QVERIFY(ctx->activationObject().property("foo").isFunction());

    QScriptEngine eng2;
    QScriptValue obj2 = eng2.newObject();
    QTest::ignoreMessage(QtWarningMsg, "QScriptContext::pushScope() failed: cannot push an object created in a different engine");
    ctx->pushScope(obj2);

    QVERIFY(ctx->popScope().strictlyEquals(obj));
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

    {
        QScriptEngine eng2;
        QScriptValue obj2 = eng2.newObject();
        QTest::ignoreMessage(QtWarningMsg, "QScriptContext::setActivationObject() failed: cannot set an object created in a different engine");
        ctx->setActivationObject(obj2);
        QVERIFY(ctx->activationObject().equals(obj));
    }

    ctx->setActivationObject(eng.globalObject());
    QScriptValue fun = eng.newFunction(get_activationObject);
    eng.globalObject().setProperty("get_activationObject", fun);
    {
        QScriptValue ret = eng.evaluate("get_activationObject(1, 2, 3)");
        QVERIFY(ret.isObject());
        QVERIFY(ret.property("arguments").isObject());
        QCOMPARE(ret.property("arguments").property("length").toInt32(), 3);
    }
}

QTEST_MAIN(tst_QScriptContext)
#include "tst_qscriptcontext.moc"
