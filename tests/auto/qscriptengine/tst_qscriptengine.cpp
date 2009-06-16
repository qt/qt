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

#include <qscriptengine.h>
#include <qscriptengineagent.h>
#include <qscriptvalueiterator.h>
#include <qgraphicsitem.h>
#include <qstandarditemmodel.h>
#include <QtCore/qnumeric.h>

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QObjectList)

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptEngine : public QObject
{
    Q_OBJECT

public:
    tst_QScriptEngine();
    virtual ~tst_QScriptEngine();

private slots:
    void currentContext();
    void pushPopContext();
    void getSetDefaultPrototype();
    void newFunction();
    void newObject();
    void newArray();
    void newVariant();
    void newRegExp();
    void newDate();
    void newQObject();
    void newQMetaObject();
    void newActivationObject();
    void getSetGlobalObject();
    void checkSyntax_data();
    void checkSyntax();
    void canEvaluate_data();
    void canEvaluate();
    void evaluate_data();
    void evaluate();
    void nestedEvaluate();
    void uncaughtException();
    void valueConversion();
    void importExtension();
    void infiniteRecursion();
    void castWithPrototypeChain();
    void castWithMultipleInheritance();
    void collectGarbage();
    void gcWithNestedDataStructure();
    void processEventsWhileRunning();
    void throwErrorFromProcessEvents();
    void stacktrace();
    void numberParsing_data();
    void numberParsing();
    void automaticSemicolonInsertion();
    void abortEvaluation();
    void isEvaluating();
    void printThrowsException();
    void errorConstructors();
    void argumentsProperty();
    void numberClass();
    void forInStatement();
    void functionExpression();
    void stringObjects();
    void getterSetterThisObject();
    void continueInSwitch();
    void readOnlyPrototypeProperty();
    void toObject();
    void reservedWords_data();
    void reservedWords();
    void futureReservedWords_data();
    void futureReservedWords();
    void throwInsideWithStatement();
    void getSetAgent();
    void reentrancy();
    void incDecNonObjectProperty();
};

tst_QScriptEngine::tst_QScriptEngine()
{
}

tst_QScriptEngine::~tst_QScriptEngine()
{
}

void tst_QScriptEngine::currentContext()
{
    QScriptEngine eng;
    QScriptContext *globalCtx = eng.currentContext();
    QVERIFY(globalCtx != 0);
    QVERIFY(globalCtx->parentContext() == 0);
    QCOMPARE(globalCtx->engine(), &eng);
    QCOMPARE(globalCtx->argumentCount(), 0);
    QCOMPARE(globalCtx->backtrace().size(), 1);
    QVERIFY(!globalCtx->isCalledAsConstructor());
    QVERIFY(!globalCtx->callee().isValid());
    QCOMPARE(globalCtx->state(), QScriptContext::NormalState);
    QVERIFY(globalCtx->thisObject().strictlyEquals(eng.globalObject()));
    QVERIFY(globalCtx->activationObject().strictlyEquals(eng.globalObject()));
    QVERIFY(globalCtx->argumentsObject().isObject());
}

void tst_QScriptEngine::pushPopContext()
{
    QScriptEngine eng;
    QScriptContext *globalCtx = eng.currentContext();
    QScriptContext *ctx = eng.pushContext();
    QVERIFY(ctx != 0);
    QCOMPARE(ctx->parentContext(), globalCtx);
    QVERIFY(!ctx->isCalledAsConstructor());
    QVERIFY(!ctx->callee().isValid());
    QVERIFY(ctx->thisObject().strictlyEquals(eng.globalObject()));
    QCOMPARE(ctx->argumentCount(), 0);
    QCOMPARE(ctx->backtrace().size(), 2);
    QCOMPARE(ctx->engine(), &eng);
    QCOMPARE(ctx->state(), QScriptContext::NormalState);
    QVERIFY(ctx->activationObject().isObject());
    QVERIFY(ctx->argumentsObject().isObject());

    QScriptContext *ctx2 = eng.pushContext();
    QVERIFY(ctx2 != 0);
    QCOMPARE(ctx2->parentContext(), ctx);
    QVERIFY(!ctx2->activationObject().strictlyEquals(ctx->activationObject()));
    QVERIFY(!ctx2->argumentsObject().strictlyEquals(ctx->argumentsObject()));

    eng.popContext();
    eng.popContext();
    eng.popContext(); // ignored
    eng.popContext(); // ignored
}

static QScriptValue myFunction(QScriptContext *, QScriptEngine *eng)
{
    return eng->nullValue();
}

static QScriptValue myFunctionWithVoidArg(QScriptContext *, QScriptEngine *eng, void *)
{
    return eng->nullValue();
}

static QScriptValue myThrowingFunction(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError("foo");
}

void tst_QScriptEngine::newFunction()
{
    QScriptEngine eng;
    {
        QScriptValue fun = eng.newFunction(myFunction);
        QCOMPARE(fun.isValid(), true);
        QCOMPARE(fun.isFunction(), true);
        QCOMPARE(fun.isObject(), true);
        QCOMPARE(fun.scriptClass(), (QScriptClass*)0);
        // a prototype property is automatically constructed
        {
            QScriptValue prot = fun.property("prototype", QScriptValue::ResolveLocal);
            QVERIFY(prot.isObject());
            QVERIFY(prot.property("constructor").strictlyEquals(fun));
            QCOMPARE(fun.propertyFlags("prototype"), QScriptValue::Undeletable);
            QCOMPARE(prot.propertyFlags("constructor"), QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
        }
        // prototype should be Function.prototype
        QCOMPARE(fun.prototype().isValid(), true);
        QCOMPARE(fun.prototype().isFunction(), true);
        QCOMPARE(fun.prototype().strictlyEquals(eng.evaluate("Function.prototype")), true);
        
        QCOMPARE(fun.call().isNull(), true);
        QCOMPARE(fun.construct().isObject(), true);
    }

    // the overload that takes a void*
    {
        QScriptValue fun = eng.newFunction(myFunctionWithVoidArg, (void*)this);
        QVERIFY(fun.isFunction());
        QCOMPARE(fun.scriptClass(), (QScriptClass*)0);
        // a prototype property is automatically constructed
        {
            QScriptValue prot = fun.property("prototype", QScriptValue::ResolveLocal);
            QVERIFY(prot.isObject());
            QVERIFY(prot.property("constructor").strictlyEquals(fun));
            QCOMPARE(fun.propertyFlags("prototype"), QScriptValue::Undeletable);
            QCOMPARE(prot.propertyFlags("constructor"), QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
        }
        // prototype should be Function.prototype
        QCOMPARE(fun.prototype().isValid(), true);
        QCOMPARE(fun.prototype().isFunction(), true);
        QCOMPARE(fun.prototype().strictlyEquals(eng.evaluate("Function.prototype")), true);
        
        QCOMPARE(fun.call().isNull(), true);
        QCOMPARE(fun.construct().isObject(), true);
    }

    // the overload that takes a prototype
    {
        QScriptValue proto = eng.newObject();
        QScriptValue fun = eng.newFunction(myFunction, proto);
        QCOMPARE(fun.isValid(), true);
        QCOMPARE(fun.isFunction(), true);
        QCOMPARE(fun.isObject(), true);
        // internal prototype should be Function.prototype
        QCOMPARE(fun.prototype().isValid(), true);
        QCOMPARE(fun.prototype().isFunction(), true);
        QCOMPARE(fun.prototype().strictlyEquals(eng.evaluate("Function.prototype")), true);
        // public prototype should be the one we passed
        QCOMPARE(fun.property("prototype").strictlyEquals(proto), true);
        QCOMPARE(fun.propertyFlags("prototype"), QScriptValue::Undeletable);
        QCOMPARE(proto.property("constructor").strictlyEquals(fun), true);
        QCOMPARE(proto.propertyFlags("constructor"),
                 QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);

        QCOMPARE(fun.call().isNull(), true);
        QCOMPARE(fun.construct().isObject(), true);
    }
}

void tst_QScriptEngine::newObject()
{
    QScriptEngine eng;
    QScriptValue object = eng.newObject();
    QCOMPARE(object.isValid(), true);
    QCOMPARE(object.isObject(), true);
    QCOMPARE(object.isFunction(), false);
    QCOMPARE(object.scriptClass(), (QScriptClass*)0);
    // prototype should be Object.prototype
    QCOMPARE(object.prototype().isValid(), true);
    QCOMPARE(object.prototype().isObject(), true);
    QCOMPARE(object.prototype().strictlyEquals(eng.evaluate("Object.prototype")), true);
}

void tst_QScriptEngine::newArray()
{
    QScriptEngine eng;
    QScriptValue array = eng.newArray();
    QCOMPARE(array.isValid(), true);
    QCOMPARE(array.isArray(), true);
    QCOMPARE(array.isObject(), true);
    QVERIFY(!array.isFunction());
    QCOMPARE(array.scriptClass(), (QScriptClass*)0);
    // prototype should be Array.prototype
    QCOMPARE(array.prototype().isValid(), true);
    QCOMPARE(array.prototype().isArray(), true);
    QCOMPARE(array.prototype().strictlyEquals(eng.evaluate("Array.prototype")), true);

    // task 218092
    {
        QScriptValue ret = eng.evaluate("[].splice(0, 0, 'a')");
        QVERIFY(ret.isArray());
        QCOMPARE(ret.property("length").toInt32(), 0);
    }
    {
        QScriptValue ret = eng.evaluate("['a'].splice(0, 1, 'b')");
        QVERIFY(ret.isArray());
        QCOMPARE(ret.property("length").toInt32(), 1);
    }
    {
        QScriptValue ret = eng.evaluate("['a', 'b'].splice(0, 1, 'c')");
        QVERIFY(ret.isArray());
        QCOMPARE(ret.property("length").toInt32(), 1);
    }
    {
        QScriptValue ret = eng.evaluate("['a', 'b', 'c'].splice(0, 2, 'd')");
        QVERIFY(ret.isArray());
        QCOMPARE(ret.property("length").toInt32(), 2);
    }
    {
        QScriptValue ret = eng.evaluate("['a', 'b', 'c'].splice(1, 2, 'd', 'e', 'f')");
        QVERIFY(ret.isArray());
        QCOMPARE(ret.property("length").toInt32(), 2);
    }

    // task 233836
    {
        QScriptValue ret = eng.evaluate("a = new Array(4294967295); a.push('foo')");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 0);
        QCOMPARE(eng.evaluate("a[4294967295]").toString(), QString::fromLatin1("foo"));
    }
    {
        QScriptValue ret = eng.newArray(0xFFFFFFFF);
        QCOMPARE(ret.property("length").toUInt32(), uint(0xFFFFFFFF));
        ret.setProperty(0xFFFFFFFF, 123);
        QCOMPARE(ret.property("length").toUInt32(), uint(0xFFFFFFFF));
        QVERIFY(ret.property(0xFFFFFFFF).isNumber());
        QCOMPARE(ret.property(0xFFFFFFFF).toInt32(), 123);
        ret.setProperty(123, 456);
        QCOMPARE(ret.property("length").toUInt32(), uint(0xFFFFFFFF));
        QVERIFY(ret.property(123).isNumber());
        QCOMPARE(ret.property(123).toInt32(), 456);
    }
}

void tst_QScriptEngine::newVariant()
{
    QScriptEngine eng;
    {
        QScriptValue opaque = eng.newVariant(QVariant());
        QCOMPARE(opaque.isValid(), true);
        QCOMPARE(opaque.isVariant(), true);
        QVERIFY(!opaque.isFunction());
        QCOMPARE(opaque.isObject(), true);
        QCOMPARE(opaque.prototype().isValid(), true);
        QCOMPARE(opaque.prototype().isVariant(), true);
        QVERIFY(opaque.property("valueOf").call(opaque).isUndefined());
    }
    // default prototype should be set automatically
    {
        QScriptValue proto = eng.newObject();
        eng.setDefaultPrototype(qMetaTypeId<QString>(), proto);
        QScriptValue ret = eng.newVariant(QVariant(QString::fromLatin1("hello")));
        QVERIFY(ret.isVariant());
        QCOMPARE(ret.scriptClass(), (QScriptClass*)0);
        QVERIFY(ret.prototype().strictlyEquals(proto));
        eng.setDefaultPrototype(qMetaTypeId<QString>(), QScriptValue());
        QScriptValue ret2 = eng.newVariant(QVariant(QString::fromLatin1("hello")));
        QVERIFY(ret2.isVariant());
        QVERIFY(!ret2.prototype().strictlyEquals(proto));
    }
    // "promote" plain object to variant
    {
        QScriptValue object = eng.newObject();
        object.setProperty("foo", eng.newObject());
        object.setProperty("bar", object.property("foo"));
        QVERIFY(object.property("foo").isObject());
        QVERIFY(!object.property("foo").isVariant());
        QScriptValue originalProto = object.property("foo").prototype();
        QScriptValue ret = eng.newVariant(object.property("foo"), QVariant(123));
        QVERIFY(ret.isValid());
        QVERIFY(ret.strictlyEquals(object.property("foo")));
        QVERIFY(ret.isVariant());
        QVERIFY(object.property("foo").isVariant());
        QVERIFY(object.property("bar").isVariant());
        QCOMPARE(ret.toVariant(), QVariant(123));
        QVERIFY(ret.prototype().strictlyEquals(originalProto));
    }
    // replace value of existing object
    {
        QScriptValue object = eng.newVariant(QVariant(123));
        QScriptValue ret = eng.newVariant(object, QVariant(456));
        QVERIFY(ret.isValid());
        QVERIFY(ret.strictlyEquals(object));
        QVERIFY(ret.isVariant());
        QCOMPARE(ret.toVariant(), QVariant(456));
    }

    // valueOf() and toString()
    {
        QScriptValue object = eng.newVariant(QVariant(123));
        QScriptValue value = object.property("valueOf").call(object);
        QVERIFY(value.isNumber());
        QCOMPARE(value.toInt32(), 123);
        QCOMPARE(object.toString(), QString::fromLatin1("123"));
        QCOMPARE(object.toVariant().toString(), object.toString());
    }
    {
        QScriptValue object = eng.newVariant(QVariant(QString::fromLatin1("hello")));
        QScriptValue value = object.property("valueOf").call(object);
        QVERIFY(value.isString());
        QCOMPARE(value.toString(), QString::fromLatin1("hello"));
        QCOMPARE(object.toString(), QString::fromLatin1("hello"));
        QCOMPARE(object.toVariant().toString(), object.toString());
    }
    {
        QScriptValue object = eng.newVariant(QVariant(false));
        QScriptValue value = object.property("valueOf").call(object);
        QVERIFY(value.isBoolean());
        QCOMPARE(value.toBoolean(), false);
        QCOMPARE(object.toString(), QString::fromLatin1("false"));
        QCOMPARE(object.toVariant().toString(), object.toString());
    }
    {
        QScriptValue object = eng.newVariant(QVariant(QPoint(10, 20)));
        QScriptValue value = object.property("valueOf").call(object);
        QVERIFY(value.isObject());
        QVERIFY(value.strictlyEquals(object));
        QCOMPARE(object.toString(), QString::fromLatin1("QVariant(QPoint)"));
    }
}

void tst_QScriptEngine::newRegExp()
{
    QScriptEngine eng;
    for (int x = 0; x < 2; ++x) {
        QScriptValue rexp;
        if (x == 0)
            rexp = eng.newRegExp("foo", "bar");
        else
            rexp = eng.newRegExp(QRegExp("foo"));
        QCOMPARE(rexp.isValid(), true);
        QCOMPARE(rexp.isRegExp(), true);
        QCOMPARE(rexp.isObject(), true);
        QVERIFY(!rexp.isFunction());
        // prototype should be RegExp.prototype
        QCOMPARE(rexp.prototype().isValid(), true);
        QCOMPARE(rexp.prototype().isRegExp(), true);
        QCOMPARE(rexp.prototype().strictlyEquals(eng.evaluate("RegExp.prototype")), true);

        QCOMPARE(rexp.toRegExp().pattern(), QRegExp("foo").pattern());
    }
    {
        QScriptValue r = eng.evaluate("/foo/gim");
        QVERIFY(r.isRegExp());
        QCOMPARE(r.toString(), QString::fromLatin1("/foo/gim"));

        QScriptValue rxCtor = eng.globalObject().property("RegExp");
        QScriptValue r2 = rxCtor.call(QScriptValue(), QScriptValueList() << r);
        QVERIFY(r2.isRegExp());
        QVERIFY(r2.strictlyEquals(r));

        QScriptValue r3 = rxCtor.call(QScriptValue(), QScriptValueList() << r << "gim");
        QVERIFY(r3.isError());
        QCOMPARE(r3.toString(), QString::fromLatin1("TypeError: cannot specify flags when creating a copy of a RegExp"));

        QScriptValue r4 = rxCtor.call(QScriptValue(), QScriptValueList() << "foo" << "gim");
        QVERIFY(r4.isRegExp());

        QScriptValue r5 = rxCtor.construct(QScriptValueList() << r);
        QVERIFY(r5.isRegExp());
        QCOMPARE(r5.toString(), QString::fromLatin1("/foo/gim"));
        QVERIFY(!r5.strictlyEquals(r));

        QScriptValue r6 = rxCtor.construct(QScriptValueList() << "foo" << "bar");
        QVERIFY(r6.isError());
        QCOMPARE(r6.toString(), QString::fromLatin1("SyntaxError: invalid regular expression flag 'b'"));

        QScriptValue r7 = eng.evaluate("/foo/gimp");
        QVERIFY(r7.isError());
        QCOMPARE(r7.toString(), QString::fromLatin1("SyntaxError: Invalid regular expression flag 'p'"));

        QScriptValue r8 = eng.evaluate("/foo/migmigmig");
        QVERIFY(r8.isRegExp());
        QCOMPARE(r8.toString(), QString::fromLatin1("/foo/gim"));

        QScriptValue r9 = rxCtor.construct();
        QVERIFY(r9.isRegExp());
        QCOMPARE(r9.toString(), QString::fromLatin1("/(?:)/"));

        QScriptValue r10 = rxCtor.construct(QScriptValueList() << "" << "gim");
        QVERIFY(r10.isRegExp());
        QCOMPARE(r10.toString(), QString::fromLatin1("/(?:)/gim"));

        QScriptValue r11 = rxCtor.construct(QScriptValueList() << "{1.*}" << "g");
        QVERIFY(r11.isRegExp());
        QCOMPARE(r11.toString(), QString::fromLatin1("/{1.*}/g"));
    }
}

void tst_QScriptEngine::newDate()
{
    QScriptEngine eng;

    {
        QScriptValue date = eng.newDate(0);
        QCOMPARE(date.isValid(), true);
        QCOMPARE(date.isDate(), true);
        QCOMPARE(date.isObject(), true);
        QVERIFY(!date.isFunction());
        // prototype should be Date.prototype
        QCOMPARE(date.prototype().isValid(), true);
        QCOMPARE(date.prototype().isDate(), true);
        QCOMPARE(date.prototype().strictlyEquals(eng.evaluate("Date.prototype")), true);
    }

    {
        QDateTime dt = QDateTime(QDate(1, 2, 3), QTime(4, 5, 6, 7), Qt::LocalTime);
        QScriptValue date = eng.newDate(dt);
        QCOMPARE(date.isValid(), true);
        QCOMPARE(date.isDate(), true);
        QCOMPARE(date.isObject(), true);
        // prototype should be Date.prototype
        QCOMPARE(date.prototype().isValid(), true);
        QCOMPARE(date.prototype().isDate(), true);
        QCOMPARE(date.prototype().strictlyEquals(eng.evaluate("Date.prototype")), true);

        QCOMPARE(date.toDateTime(), dt);
    }

    {
        QDateTime dt = QDateTime(QDate(1, 2, 3), QTime(4, 5, 6, 7), Qt::UTC);
        QScriptValue date = eng.newDate(dt);
        // toDateTime() result should be in local time
        QCOMPARE(date.toDateTime(), dt.toLocalTime());
    }

    // Date.parse() should return NaN when it fails
    {
        QScriptValue ret = eng.evaluate("Date.parse()");
        QVERIFY(ret.isNumber());
        QVERIFY(qIsNaN(ret.toNumber()));
    }

    // Date.parse() should be able to parse the output of Date().toString()
#ifndef Q_WS_WIN // TODO: Test and remove this since 169701 has been fixed
    {
        QScriptValue ret = eng.evaluate("var x = new Date(); var s = x.toString(); s == new Date(Date.parse(s)).toString()");
        QVERIFY(ret.isBoolean());
        QCOMPARE(ret.toBoolean(), true);
    }
#endif
}

void tst_QScriptEngine::newQObject()
{
    QScriptEngine eng;

    {
        QScriptValue qobject = eng.newQObject(0);
        QCOMPARE(qobject.isValid(), true);
        QCOMPARE(qobject.isNull(), true);
        QCOMPARE(qobject.isObject(), false);
        QCOMPARE(qobject.toQObject(), (QObject *)0);
    }
    {
        QScriptValue qobject = eng.newQObject(this);
        QCOMPARE(qobject.isValid(), true);
        QCOMPARE(qobject.isQObject(), true);
        QCOMPARE(qobject.isObject(), true);
        QCOMPARE(qobject.toQObject(), (QObject *)this);
        QVERIFY(!qobject.isFunction());
        // prototype should be QObject.prototype
        QCOMPARE(qobject.prototype().isValid(), true);
        QCOMPARE(qobject.prototype().isQObject(), true);
        QCOMPARE(qobject.scriptClass(), (QScriptClass*)0);
    }

    // test ownership
    {
        QPointer<QObject> ptr = new QObject();
        QVERIFY(ptr != 0);
        {
            QScriptValue v = eng.newQObject(ptr, QScriptEngine::ScriptOwnership);
        }
        eng.evaluate("gc()");
        QVERIFY(ptr == 0);
    }
    {
        QPointer<QObject> ptr = new QObject();
        QVERIFY(ptr != 0);
        {
            QScriptValue v = eng.newQObject(ptr, QScriptEngine::QtOwnership);
        }
        QObject *before = ptr;
        eng.evaluate("gc()");
        QVERIFY(ptr == before);
        delete ptr;
    }
    {
        QObject *parent = new QObject();
        QObject *child = new QObject(parent);
        QScriptValue v = eng.newQObject(child, QScriptEngine::QtOwnership);
        QCOMPARE(v.toQObject(), child);
        delete parent;
        QCOMPARE(v.toQObject(), (QObject *)0);
    }
    {
        QPointer<QObject> ptr = new QObject();
        QVERIFY(ptr != 0);
        {
            QScriptValue v = eng.newQObject(ptr, QScriptEngine::AutoOwnership);
        }
        eng.evaluate("gc()");
        // no parent, so it should be like ScriptOwnership
        QVERIFY(ptr == 0);
    }
    {
        QObject *parent = new QObject();
        QPointer<QObject> child = new QObject(parent);
        QVERIFY(child != 0);
        {
            QScriptValue v = eng.newQObject(child, QScriptEngine::AutoOwnership);
        }
        eng.evaluate("gc()");
        // has parent, so it should be like QtOwnership
        QVERIFY(child != 0);
        delete parent;
    }

    // "promote" plain object to QObject
    {
        QScriptValue obj = eng.newObject();
        QScriptValue originalProto = obj.prototype();
        QScriptValue ret = eng.newQObject(obj, this);
        QVERIFY(ret.isValid());
        QVERIFY(ret.isQObject());
        QVERIFY(ret.strictlyEquals(obj));
        QVERIFY(obj.isQObject());
        QCOMPARE(ret.toQObject(), (QObject *)this);
        QVERIFY(ret.prototype().strictlyEquals(originalProto));
        QScriptValue val = ret.property("objectName");
        QVERIFY(val.isString());
    }
    // replace QObject* of existing object
    {
        QScriptValue object = eng.newQObject(this);
        QScriptValue originalProto = object.prototype();
        QObject otherQObject;
        QScriptValue ret = eng.newQObject(object, &otherQObject);
        QVERIFY(ret.isValid());
        QVERIFY(ret.isQObject());
        QVERIFY(ret.strictlyEquals(object));
        QCOMPARE(ret.toQObject(), (QObject *)&otherQObject);
        QVERIFY(ret.prototype().strictlyEquals(originalProto));
    }

    // calling newQObject() several times with same object
    for (int x = 0; x < 2; ++x) {
        QObject qobj;
        // the default is to create a new wrapper object
        QScriptValue obj1 = eng.newQObject(&qobj);
        QScriptValue obj2 = eng.newQObject(&qobj);
        QVERIFY(!obj2.strictlyEquals(obj1));

        QScriptEngine::QObjectWrapOptions opt = 0;
        bool preferExisting = (x != 0);
        if (preferExisting)
            opt |= QScriptEngine::PreferExistingWrapperObject;

        QScriptValue obj3 = eng.newQObject(&qobj, QScriptEngine::AutoOwnership, opt);
        QVERIFY(!obj3.strictlyEquals(obj2));
        QScriptValue obj4 = eng.newQObject(&qobj, QScriptEngine::AutoOwnership, opt);
        QCOMPARE(obj4.strictlyEquals(obj3), preferExisting);

        QScriptValue obj5 = eng.newQObject(&qobj, QScriptEngine::ScriptOwnership, opt);
        QVERIFY(!obj5.strictlyEquals(obj4));
        QScriptValue obj6 = eng.newQObject(&qobj, QScriptEngine::ScriptOwnership, opt);
        QCOMPARE(obj6.strictlyEquals(obj5), preferExisting);

        QScriptValue obj7 = eng.newQObject(&qobj, QScriptEngine::ScriptOwnership,
                                           QScriptEngine::ExcludeSuperClassMethods | opt);
        QVERIFY(!obj7.strictlyEquals(obj6));
        QScriptValue obj8 = eng.newQObject(&qobj, QScriptEngine::ScriptOwnership,
                                           QScriptEngine::ExcludeSuperClassMethods | opt);
        QCOMPARE(obj8.strictlyEquals(obj7), preferExisting);
    }

    // newQObject() should set the default prototype, if one has been registered
    {
        QScriptValue oldQObjectProto = eng.defaultPrototype(qMetaTypeId<QObject*>());

        QScriptValue qobjectProto = eng.newObject();
        eng.setDefaultPrototype(qMetaTypeId<QObject*>(), qobjectProto);
        {
            QScriptValue ret = eng.newQObject(this);
            QVERIFY(ret.prototype().equals(qobjectProto));
        }
        QScriptValue tstProto = eng.newObject();
        int typeId = qRegisterMetaType<tst_QScriptEngine*>("tst_QScriptEngine*");
        eng.setDefaultPrototype(typeId, tstProto);
        {
            QScriptValue ret = eng.newQObject(this);
            QVERIFY(ret.prototype().equals(tstProto));
        }

        eng.setDefaultPrototype(qMetaTypeId<QObject*>(), oldQObjectProto);
        eng.setDefaultPrototype(typeId, QScriptValue());
    }
}

QT_BEGIN_NAMESPACE
Q_SCRIPT_DECLARE_QMETAOBJECT(QObject, QObject*)
Q_SCRIPT_DECLARE_QMETAOBJECT(QWidget, QWidget*)
QT_END_NAMESPACE

static QScriptValue myConstructor(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue obj;
    if (ctx->isCalledAsConstructor()) {
        obj = ctx->thisObject();
    } else {
        obj = eng->newObject();
        obj.setPrototype(ctx->callee().property("prototype"));
    }
    obj.setProperty("isCalledAsConstructor", QScriptValue(eng, ctx->isCalledAsConstructor()));
    return obj;
}

void tst_QScriptEngine::newQMetaObject()
{
    QScriptEngine eng;
#if 0
    QScriptValue qclass = eng.newQMetaObject<QObject>();
    QScriptValue qclass2 = eng.newQMetaObject<QWidget>();
#else
    QScriptValue qclass = qScriptValueFromQMetaObject<QObject>(&eng);
    QScriptValue qclass2 = qScriptValueFromQMetaObject<QWidget>(&eng);
#endif
    QCOMPARE(qclass.isValid(), true);
    QCOMPARE(qclass.isQMetaObject(), true);
    QCOMPARE(qclass.toQMetaObject(), &QObject::staticMetaObject);
    QCOMPARE(qclass.isFunction(), true);
    QVERIFY(qclass.property("prototype").isObject());

    QCOMPARE(qclass2.isValid(), true);
    QCOMPARE(qclass2.isQMetaObject(), true);
    QCOMPARE(qclass2.toQMetaObject(), &QWidget::staticMetaObject);
    QCOMPARE(qclass2.isFunction(), true);
    QVERIFY(qclass2.property("prototype").isObject());

    // prototype should be QMetaObject.prototype
    QCOMPARE(qclass.prototype().isObject(), true);
    QCOMPARE(qclass2.prototype().isObject(), true);

    QScriptValue instance = qclass.construct();
    QCOMPARE(instance.isQObject(), true);
    QCOMPARE(instance.toQObject()->metaObject(), qclass.toQMetaObject());
    QVERIFY(instance.instanceOf(qclass));

    QScriptValue instance2 = qclass2.construct();
    QCOMPARE(instance2.isQObject(), true);
    QCOMPARE(instance2.toQObject()->metaObject(), qclass2.toQMetaObject());
    QVERIFY(instance2.instanceOf(qclass2));

    QScriptValueList args;
    args << instance;
    QScriptValue instance3 = qclass.construct(args);
    QCOMPARE(instance3.isQObject(), true);
    QCOMPARE(instance3.toQObject()->parent(), instance.toQObject());
    QVERIFY(instance3.instanceOf(qclass));
    args.clear();

    // verify that AutoOwnership is in effect
    instance = QScriptValue();
    eng.collectGarbage();
    QVERIFY(instance.toQObject() == 0);
    QVERIFY(instance3.toQObject() == 0); // was child of instance
    QVERIFY(instance2.toQObject() != 0);
    instance2 = QScriptValue();
    eng.collectGarbage();
    QVERIFY(instance2.toQObject() == 0);

    // with custom constructor
    QScriptValue ctor = eng.newFunction(myConstructor);
    QScriptValue qclass3 = eng.newQMetaObject(&QObject::staticMetaObject, ctor);
    QVERIFY(qclass3.property("prototype").equals(ctor.property("prototype")));
    {
        QScriptValue ret = qclass3.call();
        QVERIFY(ret.isObject());
        QVERIFY(ret.property("isCalledAsConstructor").isBoolean());
        QVERIFY(!ret.property("isCalledAsConstructor").toBoolean());
        QVERIFY(ret.instanceOf(qclass3));
    }
    {
        QScriptValue ret = qclass3.construct();
        QVERIFY(ret.isObject());
        QVERIFY(ret.property("isCalledAsConstructor").isBoolean());
        QVERIFY(ret.property("isCalledAsConstructor").toBoolean());
        QVERIFY(ret.instanceOf(qclass3));
    }

    // subclassing
    qclass2.setProperty("prototype", qclass.construct());
    QVERIFY(qclass2.construct().instanceOf(qclass));

    // with meta-constructor
    QScriptValue qclass4 = eng.newQMetaObject(&QObject::staticMetaObject);
    {
        QScriptValue inst = qclass4.construct();
        QVERIFY(inst.isQObject());
        QVERIFY(inst.toQObject() != 0);
        QCOMPARE(inst.toQObject()->parent(), (QObject*)0);
        QVERIFY(inst.instanceOf(qclass4));
    }
    {
        QScriptValue inst = qclass4.construct(QScriptValueList() << eng.newQObject(this));
        QVERIFY(inst.isQObject());
        QVERIFY(inst.toQObject() != 0);
        QCOMPARE(inst.toQObject()->parent(), (QObject*)this);
        QVERIFY(inst.instanceOf(qclass4));
    }
}

void tst_QScriptEngine::newActivationObject()
{
    QScriptEngine eng;
    QScriptValue act = eng.newActivationObject();
    QCOMPARE(act.isValid(), true);
    QCOMPARE(act.isObject(), true);
    QVERIFY(!act.isFunction());
    QScriptValue v(&eng, 123);
    act.setProperty("prop", v);
    QCOMPARE(act.property("prop").strictlyEquals(v), true);
    QCOMPARE(act.scope().isValid(), false);
    QVERIFY(act.prototype().isNull());
}

void tst_QScriptEngine::getSetGlobalObject()
{
    QScriptEngine eng;
    QScriptValue glob = eng.globalObject();
    QCOMPARE(glob.isValid(), true);
    QCOMPARE(glob.isObject(), true);
    QVERIFY(!glob.isFunction());
    QVERIFY(eng.currentContext()->thisObject().strictlyEquals(glob));
    QVERIFY(eng.currentContext()->activationObject().strictlyEquals(glob));
    QCOMPARE(glob.toString(), QString::fromLatin1("[object global]"));
    // prototype should be Object.prototype
    QCOMPARE(glob.prototype().isValid(), true);
    QCOMPARE(glob.prototype().isObject(), true);
    QCOMPARE(glob.prototype().strictlyEquals(eng.evaluate("Object.prototype")), true);

    QScriptValue obj = eng.newObject();
    eng.setGlobalObject(obj);
    QVERIFY(eng.globalObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->thisObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->activationObject().strictlyEquals(obj));
    QVERIFY(eng.evaluate("this").strictlyEquals(obj));
    QCOMPARE(eng.globalObject().toString(), QString::fromLatin1("[object Object]"));

    glob = QScriptValue(); // kill reference to old global object
    eng.collectGarbage();
    obj = eng.newObject();
    eng.setGlobalObject(obj);
    QVERIFY(eng.globalObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->thisObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->activationObject().strictlyEquals(obj));

    eng.collectGarbage();
    QVERIFY(eng.globalObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->thisObject().strictlyEquals(obj));
    QVERIFY(eng.currentContext()->activationObject().strictlyEquals(obj));
}

void tst_QScriptEngine::checkSyntax_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<int>("expectedState");
    QTest::addColumn<int>("errorLineNumber");
    QTest::addColumn<int>("errorColumnNumber");
    QTest::addColumn<QString>("errorMessage");

    QTest::newRow("0")
        << QString("0") << int(QScriptSyntaxCheckResult::Valid)
        << -1 << -1 << "";
    QTest::newRow("if (")
        << QString("if (\n") << int(QScriptSyntaxCheckResult::Intermediate)
        << 1 << 4 << "";
    QTest::newRow("if else")
        << QString("\nif else") << int(QScriptSyntaxCheckResult::Error)
        << 2 << 4 << "Expected `('";
    QTest::newRow("foo[")
        << QString("foo[") << int(QScriptSyntaxCheckResult::Error)
        << 1 << 4 << "";
    QTest::newRow("foo['bar']")
        << QString("foo['bar']") << int(QScriptSyntaxCheckResult::Valid)
        << -1 << -1 << "";

    QTest::newRow("/*")
        << QString("/*") << int(QScriptSyntaxCheckResult::Intermediate)
        << 1 << 1 << "Unclosed comment at end of file";
    QTest::newRow("/*\nMy comment")
        << QString("/*\nMy comment") << int(QScriptSyntaxCheckResult::Intermediate)
        << 1 << 1 << "Unclosed comment at end of file";
    QTest::newRow("/*\nMy comment */\nfoo = 10")
        << QString("/*\nMy comment */\nfoo = 10") << int(QScriptSyntaxCheckResult::Valid)
        << -1 << -1 << "";
    QTest::newRow("foo = 10 /*")
        << QString("foo = 10 /*") << int(QScriptSyntaxCheckResult::Intermediate)
        << -1 << -1 << "";
    QTest::newRow("foo = 10; /*")
        << QString("foo = 10; /*") << int(QScriptSyntaxCheckResult::Intermediate)
        << 1 << 11 << "Expected `end of file'";
    QTest::newRow("foo = 10 /* My comment */")
        << QString("foo = 10 /* My comment */") << int(QScriptSyntaxCheckResult::Valid)
        << -1 << -1 << "";

    QTest::newRow("/=/")
        << QString("/=/") << int(QScriptSyntaxCheckResult::Valid) << -1 << -1 << "";
    QTest::newRow("/=/g")
        << QString("/=/g") << int(QScriptSyntaxCheckResult::Valid) << -1 << -1 << "";
    QTest::newRow("/a/")
        << QString("/a/") << int(QScriptSyntaxCheckResult::Valid) << -1 << -1 << "";
    QTest::newRow("/a/g")
        << QString("/a/g") << int(QScriptSyntaxCheckResult::Valid) << -1 << -1 << "";
}

void tst_QScriptEngine::checkSyntax()
{
    QFETCH(QString, code);
    QFETCH(int, expectedState);
    QFETCH(int, errorLineNumber);
    QFETCH(int, errorColumnNumber);
    QFETCH(QString, errorMessage);

    QScriptSyntaxCheckResult result = QScriptEngine::checkSyntax(code);
    QCOMPARE(result.state(), QScriptSyntaxCheckResult::State(expectedState));
    QCOMPARE(result.errorLineNumber(), errorLineNumber);
    QCOMPARE(result.errorColumnNumber(), errorColumnNumber);
    QCOMPARE(result.errorMessage(), errorMessage);
}

void tst_QScriptEngine::canEvaluate_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<bool>("expectSuccess");

    QTest::newRow("") << QString("") << true;
    QTest::newRow("0") << QString("0") << true;
    QTest::newRow("!") << QString("!\n") << false;
    QTest::newRow("if (") << QString("if (\n") << false;
    QTest::newRow("if (10) //") << QString("if (10) //\n") << false;
    QTest::newRow("a = 1; if (") << QString("a = 1;\nif (\n") << false;
    QTest::newRow("./test.js") << QString("./test.js\n") << true;
    QTest::newRow("if (0) print(1)") << QString("if (0)\nprint(1)\n") << true;
    QTest::newRow("0 = ") << QString("0 = \n") << false;
    QTest::newRow("0 = 0") << QString("0 = 0\n") << true;
    QTest::newRow("foo[") << QString("foo[") << true; // automatic semicolon will be inserted
    QTest::newRow("foo[") << QString("foo[\n") << false;
    QTest::newRow("foo['bar']") << QString("foo['bar']") << true;

    QTest::newRow("/*") << QString("/*") << false;
    QTest::newRow("/*\nMy comment") << QString("/*\nMy comment") << false;
    QTest::newRow("/*\nMy comment */\nfoo = 10") << QString("/*\nMy comment */\nfoo = 10") << true;
    QTest::newRow("foo = 10 /*") << QString("foo = 10 /*") << false;
    QTest::newRow("foo = 10; /*") << QString("foo = 10; /*") << false;
    QTest::newRow("foo = 10 /* My comment */") << QString("foo = 10 /* My comment */") << true;

    QTest::newRow("/=/") << QString("/=/") << true;
    QTest::newRow("/=/g") << QString("/=/g") << true;
    QTest::newRow("/a/") << QString("/a/") << true;
    QTest::newRow("/a/g") << QString("/a/g") << true;
}

void tst_QScriptEngine::canEvaluate()
{
    QFETCH(QString, code);
    QFETCH(bool, expectSuccess);

    QScriptEngine eng;
    QCOMPARE(eng.canEvaluate(code), expectSuccess);
}

void tst_QScriptEngine::evaluate_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<int>("lineNumber");
    QTest::addColumn<bool>("expectHadError");
    QTest::addColumn<int>("expectErrorLineNumber");

    QTest::newRow("(newline)") << QString("\n") << -1 << false << -1;
    QTest::newRow("0 //")   << QString("0 //") << -1 << false << -1;
    QTest::newRow("/* */")   << QString("/* */") << -1 << false << -1;
    QTest::newRow("//") << QString("//") << -1 << false << -1;
    QTest::newRow("(spaces)")  << QString("  ") << -1 << false << -1;
    QTest::newRow("(empty)")   << QString("") << -1 << false << -1;
    QTest::newRow("0")     << QString("0")       << -1 << false << -1;
    QTest::newRow("0=1")   << QString("\n0=1\n") << -1 << true  << 2;
    QTest::newRow("a=1")   << QString("a=1\n")   << -1 << false << -1;
    QTest::newRow("a=1;K") << QString("a=1;\nK") << -1 << true  << 2;

    QTest::newRow("f()") << QString("function f()\n"
                                    "{\n"
                                    "  var a;\n"
                                    "  var b=\";\n" // here's the error
                                    "}\n"
                                    "f();\n")
                         << -1 << true << 4;

    QTest::newRow("0")     << QString("0")       << 10 << false << -1;
    QTest::newRow("0=1")   << QString("\n\n0=1\n") << 10 << true  << 12;
    QTest::newRow("a=1")   << QString("a=1\n")   << 10 << false << -1;
    QTest::newRow("a=1;K") << QString("a=1;\n\nK") << 10 << true  << 12;

    QTest::newRow("f()") << QString("function f()\n"
                                    "{\n"
                                    "  var a;\n"
                                    "\n\n"
                                    "  var b=\";\n" // here's the error
                                    "}\n"
                                    "f();\n")
                         << 10 << true << 15;
    QTest::newRow("functionThatDoesntExist()")
        << QString(";\n;\n;\nfunctionThatDoesntExist()")
        << -1 << true << 4;
    QTest::newRow("for (var p in this) { continue labelThatDoesntExist; }")
        << QString("for (var p in this) {\ncontinue labelThatDoesntExist; }")
        << 4 << true << 5;
    QTest::newRow("duplicateLabel: { duplicateLabel: ; }")
        << QString("duplicateLabel: { duplicateLabel: ; }")
        << 12 << true << 12;

    QTest::newRow("/=/") << QString("/=/") << -1 << false << -1;
    QTest::newRow("/=/g") << QString("/=/g") << -1 << false << -1;
    QTest::newRow("/a/") << QString("/a/") << -1 << false << -1;
    QTest::newRow("/a/g") << QString("/a/g") << -1 << false << -1;
    QTest::newRow("/a/gim") << QString("/a/gim") << -1 << false << -1;
    QTest::newRow("/a/gimp") << QString("/a/gimp") << 1 << true << 1;
}

void tst_QScriptEngine::evaluate()
{
    QFETCH(QString, code);
    QFETCH(int, lineNumber);
    QFETCH(bool, expectHadError);
    QFETCH(int, expectErrorLineNumber);

    QScriptEngine eng;
    QScriptValue ret;
    if (lineNumber != -1)
        ret = eng.evaluate(code, /*fileName =*/QString(), lineNumber);
    else
        ret = eng.evaluate(code);
    QCOMPARE(eng.hasUncaughtException(), expectHadError);
    QCOMPARE(eng.uncaughtExceptionLineNumber(), expectErrorLineNumber);
    if (eng.hasUncaughtException() && ret.isError())
        QVERIFY(ret.property("lineNumber").strictlyEquals(QScriptValue(&eng, expectErrorLineNumber)));
    else
        QVERIFY(eng.uncaughtExceptionBacktrace().isEmpty());
}

static QScriptValue eval_nested(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue result = eng->newObject();
    result.setProperty("thisObjectIdBefore", ctx->thisObject().property("id"));
    QScriptValue evaluatedThisObject = eng->evaluate("this");
    result.setProperty("thisObjectIdAfter", ctx->thisObject().property("id"));
    result.setProperty("evaluatedThisObjectId", evaluatedThisObject.property("id"));
    return result;
}

void tst_QScriptEngine::nestedEvaluate()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("fun", eng.newFunction(eval_nested));
    QScriptValue result = eng.evaluate("o = { id:'foo'}; o.fun = fun; o.fun()");
    QCOMPARE(result.property("thisObjectIdBefore").toString(), QString("foo"));
    QCOMPARE(result.property("thisObjectIdAfter").toString(), QString("foo"));
    QCOMPARE(result.property("evaluatedThisObjectId").toString(), QString("foo"));
}

void tst_QScriptEngine::uncaughtException()
{
    QScriptEngine eng;
    QScriptValue fun = eng.newFunction(myFunction);
    QScriptValue throwFun = eng.newFunction(myThrowingFunction);
    for (int x = 0; x < 2; ++x) {
        {
            QScriptValue ret = eng.evaluate("a = 10;\nb = 20;\n0 = 0;\n", /*fileName=*/QString(), /*lineNumber=*/x);
            QVERIFY(eng.hasUncaughtException());
            QCOMPARE(eng.uncaughtExceptionLineNumber(), x+2);
            QVERIFY(eng.uncaughtException().strictlyEquals(ret));
            (void)ret.toString();
            QVERIFY(eng.hasUncaughtException());
            QVERIFY(eng.uncaughtException().strictlyEquals(ret));
            QVERIFY(fun.call().isNull());
            QVERIFY(eng.hasUncaughtException());
            QCOMPARE(eng.uncaughtExceptionLineNumber(), x+2);
            QVERIFY(eng.uncaughtException().strictlyEquals(ret));
            eng.clearExceptions();
            QVERIFY(!eng.hasUncaughtException());
            QCOMPARE(eng.uncaughtExceptionLineNumber(), x+2);
            QVERIFY(!eng.uncaughtException().isValid());

            eng.evaluate("2 = 3");
            QVERIFY(eng.hasUncaughtException());
            QScriptValue ret2 = throwFun.call();
            QVERIFY(ret2.isError());
            QVERIFY(eng.hasUncaughtException());
            QVERIFY(eng.uncaughtException().strictlyEquals(ret2));
            QCOMPARE(eng.uncaughtExceptionLineNumber(), -1);
            eng.clearExceptions();
            QVERIFY(!eng.hasUncaughtException());
            eng.evaluate("1 + 2");
            QVERIFY(!eng.hasUncaughtException());
        }
        {
            QScriptValue ret = eng.evaluate("a = 10");
            QVERIFY(!eng.hasUncaughtException());
            QVERIFY(!eng.uncaughtException().isValid());
        }
        {
            QScriptValue ret = eng.evaluate("1 = 2");
            QVERIFY(eng.hasUncaughtException());
            eng.clearExceptions();
            QVERIFY(!eng.hasUncaughtException());
        }
        {
            eng.globalObject().setProperty("throwFun", throwFun);
            eng.evaluate("1;\nthrowFun();");
            QVERIFY(eng.hasUncaughtException());
            QCOMPARE(eng.uncaughtExceptionLineNumber(), 2);
            eng.clearExceptions();
            QVERIFY(!eng.hasUncaughtException());
        }
    }
}

struct Foo {
public:
    int x, y;
    Foo() : x(-1), y(-1) { }
};

Q_DECLARE_METATYPE(Foo)
Q_DECLARE_METATYPE(Foo*)

void tst_QScriptEngine::getSetDefaultPrototype()
{
    QScriptEngine eng;
    {
        QScriptValue object = eng.newObject();
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<int>()).isValid(), false);
        eng.setDefaultPrototype(qMetaTypeId<int>(), object);
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<int>()).strictlyEquals(object), true);
        QScriptValue value = eng.newVariant(int(123));
        QCOMPARE(value.prototype().isObject(), true);
        QCOMPARE(value.prototype().strictlyEquals(object), true);

        eng.setDefaultPrototype(qMetaTypeId<int>(), QScriptValue());
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<int>()).isValid(), false);
        QScriptValue value2 = eng.newVariant(int(123));
        QCOMPARE(value2.prototype().strictlyEquals(object), false);
    }
    {
        QScriptValue object = eng.newObject();
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<Foo>()).isValid(), false);
        eng.setDefaultPrototype(qMetaTypeId<Foo>(), object);
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<Foo>()).strictlyEquals(object), true);
        QScriptValue value = eng.newVariant(qVariantFromValue(Foo()));
        QCOMPARE(value.prototype().isObject(), true);
        QCOMPARE(value.prototype().strictlyEquals(object), true);

        eng.setDefaultPrototype(qMetaTypeId<Foo>(), QScriptValue());
        QCOMPARE(eng.defaultPrototype(qMetaTypeId<Foo>()).isValid(), false);
        QScriptValue value2 = eng.newVariant(qVariantFromValue(Foo()));
        QCOMPARE(value2.prototype().strictlyEquals(object), false);
    }
}

static QScriptValue fooToScriptValue(QScriptEngine *eng, const Foo &foo)
{
    QScriptValue obj = eng->newObject();
    obj.setProperty("x", QScriptValue(eng, foo.x));
    obj.setProperty("y", QScriptValue(eng, foo.y));
    return obj;
}

static void fooFromScriptValue(const QScriptValue &value, Foo &foo)
{
    foo.x = value.property("x").toInt32();
    foo.y = value.property("y").toInt32();
}

static QScriptValue fooToScriptValueV2(QScriptEngine *eng, const Foo &foo)
{
    return QScriptValue(eng, foo.x);
}

static void fooFromScriptValueV2(const QScriptValue &value, Foo &foo)
{
    foo.x = value.toInt32();
}

Q_DECLARE_METATYPE(QLinkedList<QString>)
Q_DECLARE_METATYPE(QList<Foo>)
Q_DECLARE_METATYPE(QVector<QChar>)
Q_DECLARE_METATYPE(QStack<int>)
Q_DECLARE_METATYPE(QQueue<char>)
Q_DECLARE_METATYPE(QLinkedList<QStack<int> >)

void tst_QScriptEngine::valueConversion()
{
    QScriptEngine eng;
    {
        QScriptValue num = qScriptValueFromValue(&eng, 123);
        QCOMPARE(num.isNumber(), true);
        QCOMPARE(num.strictlyEquals(QScriptValue(&eng, 123)), true);

        int inum = qScriptValueToValue<int>(num);
        QCOMPARE(inum, 123);

        QString snum = qScriptValueToValue<QString>(num);
        QCOMPARE(snum, QLatin1String("123"));
    }
#ifndef QT_NO_MEMBER_TEMPLATES
    {
        QScriptValue num = eng.toScriptValue(123);
        QCOMPARE(num.isNumber(), true);
        QCOMPARE(num.strictlyEquals(QScriptValue(&eng, 123)), true);

        int inum = eng.fromScriptValue<int>(num);
        QCOMPARE(inum, 123);

        QString snum = eng.fromScriptValue<QString>(num);
        QCOMPARE(snum, QLatin1String("123"));
    }
#endif
    {
        QScriptValue num(&eng, 123);
        QCOMPARE(qScriptValueToValue<char>(num), char(123));
        QCOMPARE(qScriptValueToValue<unsigned char>(num), (unsigned char)(123));
        QCOMPARE(qScriptValueToValue<short>(num), short(123));
        QCOMPARE(qScriptValueToValue<unsigned short>(num), (unsigned short)(123));
        QCOMPARE(qScriptValueToValue<float>(num), float(123));
        QCOMPARE(qScriptValueToValue<double>(num), double(123));
        QCOMPARE(qScriptValueToValue<qlonglong>(num), qlonglong(123));
        QCOMPARE(qScriptValueToValue<qulonglong>(num), qulonglong(123));
    }
    {
        QScriptValue num(123);
        QCOMPARE(qScriptValueToValue<char>(num), char(123));
        QCOMPARE(qScriptValueToValue<unsigned char>(num), (unsigned char)(123));
        QCOMPARE(qScriptValueToValue<short>(num), short(123));
        QCOMPARE(qScriptValueToValue<unsigned short>(num), (unsigned short)(123));
        QCOMPARE(qScriptValueToValue<float>(num), float(123));
        QCOMPARE(qScriptValueToValue<double>(num), double(123));
        QCOMPARE(qScriptValueToValue<qlonglong>(num), qlonglong(123));
        QCOMPARE(qScriptValueToValue<qulonglong>(num), qulonglong(123));
    }

    {
        QScriptValue num = qScriptValueFromValue(&eng, Q_INT64_C(0x100000000));
        QCOMPARE(qScriptValueToValue<qlonglong>(num), Q_INT64_C(0x100000000));
        QCOMPARE(qScriptValueToValue<qulonglong>(num), Q_UINT64_C(0x100000000));
    }

    {
        QChar c = QLatin1Char('c');
        QScriptValue str = QScriptValue(&eng, "ciao");
        QCOMPARE(qScriptValueToValue<QChar>(str), c);
        QScriptValue code = QScriptValue(&eng, c.unicode());
        QCOMPARE(qScriptValueToValue<QChar>(code), c);
        QCOMPARE(qScriptValueToValue<QChar>(qScriptValueFromValue(&eng, c)), c);
    }

    {
        // a type that we don't have built-in conversion of
        // (it's stored as a variant)
        QTime tm(1, 2, 3, 4);
        QScriptValue val = qScriptValueFromValue(&eng, tm);
        QCOMPARE(qScriptValueToValue<QTime>(val), tm);
    }

    {
        Foo foo;
        foo.x = 12;
        foo.y = 34;
        QScriptValue fooVal = qScriptValueFromValue(&eng, foo);
        QCOMPARE(fooVal.isVariant(), true);

        Foo foo2 = qScriptValueToValue<Foo>(fooVal);
        QCOMPARE(foo2.x, foo.x);
        QCOMPARE(foo2.y, foo.y);
    }

    qScriptRegisterMetaType<Foo>(&eng, fooToScriptValue, fooFromScriptValue);

    {
        Foo foo;
        foo.x = 12;
        foo.y = 34;
        QScriptValue fooVal = qScriptValueFromValue(&eng, foo);
        QCOMPARE(fooVal.isObject(), true);
        QVERIFY(fooVal.prototype().strictlyEquals(eng.evaluate("Object.prototype")));
        QCOMPARE(fooVal.property("x").strictlyEquals(QScriptValue(&eng, 12)), true);
        QCOMPARE(fooVal.property("y").strictlyEquals(QScriptValue(&eng, 34)), true);
        fooVal.setProperty("x", QScriptValue(&eng, 56));
        fooVal.setProperty("y", QScriptValue(&eng, 78));

        Foo foo2 = qScriptValueToValue<Foo>(fooVal);
        QCOMPARE(foo2.x, 56);
        QCOMPARE(foo2.y, 78);

        QScriptValue fooProto = eng.newObject();
        eng.setDefaultPrototype(qMetaTypeId<Foo>(), fooProto);
        QScriptValue fooVal2 = qScriptValueFromValue(&eng, foo2);
        QVERIFY(fooVal2.prototype().strictlyEquals(fooProto));
        QVERIFY(fooVal2.property("x").strictlyEquals(QScriptValue(&eng, 56)));
        QVERIFY(fooVal2.property("y").strictlyEquals(QScriptValue(&eng, 78)));
    }

    qScriptRegisterSequenceMetaType<QLinkedList<QString> >(&eng);

    {
        QLinkedList<QString> lst;
        lst << QLatin1String("foo") << QLatin1String("bar");
        QScriptValue lstVal = qScriptValueFromValue(&eng, lst);
        QCOMPARE(lstVal.isArray(), true);
        QCOMPARE(lstVal.property("length").toInt32(), 2);
        QCOMPARE(lstVal.property("0").isString(), true);
        QCOMPARE(lstVal.property("0").toString(), QLatin1String("foo"));
        QCOMPARE(lstVal.property("1").isString(), true);
        QCOMPARE(lstVal.property("1").toString(), QLatin1String("bar"));
    }

    qScriptRegisterSequenceMetaType<QList<Foo> >(&eng);
    qScriptRegisterSequenceMetaType<QStack<int> >(&eng);
    qScriptRegisterSequenceMetaType<QVector<QChar> >(&eng);
    qScriptRegisterSequenceMetaType<QQueue<char> >(&eng);
    qScriptRegisterSequenceMetaType<QLinkedList<QStack<int> > >(&eng);

    {
        QLinkedList<QStack<int> > lst;
        QStack<int> first; first << 13 << 49; lst << first;
        QStack<int> second; second << 99999;lst << second;
        QScriptValue lstVal = qScriptValueFromValue(&eng, lst);
        QCOMPARE(lstVal.isArray(), true);
        QCOMPARE(lstVal.property("length").toInt32(), 2);
        QCOMPARE(lstVal.property("0").isArray(), true);
        QCOMPARE(lstVal.property("0").property("length").toInt32(), 2);
        QCOMPARE(lstVal.property("0").property("0").toInt32(), first.at(0));
        QCOMPARE(lstVal.property("0").property("1").toInt32(), first.at(1));
        QCOMPARE(lstVal.property("1").isArray(), true);
        QCOMPARE(lstVal.property("1").property("length").toInt32(), 1);
        QCOMPARE(lstVal.property("1").property("0").toInt32(), second.at(0));
        QCOMPARE(qscriptvalue_cast<QStack<int> >(lstVal.property("0")), first);
        QCOMPARE(qscriptvalue_cast<QStack<int> >(lstVal.property("1")), second);
        QCOMPARE(qscriptvalue_cast<QLinkedList<QStack<int> > >(lstVal), lst);
    }

    // pointers
    {
        Foo foo;
        {
            QScriptValue v = qScriptValueFromValue(&eng, &foo);
            Foo *pfoo = qscriptvalue_cast<Foo*>(v);
            QCOMPARE(pfoo, &foo);
        }
        {
            Foo *pfoo = 0;
            QScriptValue v = qScriptValueFromValue(&eng, pfoo);
            QCOMPARE(v.isNull(), true);
            QVERIFY(qscriptvalue_cast<Foo*>(v) == 0);
        }
    }

    // QList<int> and QObjectList should be converted from/to arrays by default
    {
        QList<int> lst;
        lst << 1 << 2 << 3;
        QScriptValue val = qScriptValueFromValue(&eng, lst);
        QVERIFY(val.isArray());
        QCOMPARE(val.property("length").toInt32(), lst.size());
        QCOMPARE(val.property(0).toInt32(), lst.at(0));
        QCOMPARE(val.property(1).toInt32(), lst.at(1));
        QCOMPARE(val.property(2).toInt32(), lst.at(2));

        QCOMPARE(qscriptvalue_cast<QList<int> >(val), lst);
    }
    {
        QObjectList lst;
        lst << this;
        QScriptValue val = qScriptValueFromValue(&eng, lst);
        QVERIFY(val.isArray());
        QCOMPARE(val.property("length").toInt32(), lst.size());
        QCOMPARE(val.property(0).toQObject(), (QObject *)this);

        QCOMPARE(qscriptvalue_cast<QObjectList>(val), lst);
    }

    // qScriptValueFromValue() should be "smart" when the argument is a QVariant
    {
        QScriptValue val = qScriptValueFromValue(&eng, QVariant());
        QVERIFY(!val.isVariant());
        QVERIFY(val.isUndefined());
    }
    {
        QScriptValue val = qScriptValueFromValue(&eng, QVariant(true));
        QVERIFY(!val.isVariant());
        QVERIFY(val.isBoolean());
        QCOMPARE(val.toBoolean(), true);
    }
    {
        QScriptValue val = qScriptValueFromValue(&eng, QVariant(int(123)));
        QVERIFY(!val.isVariant());
        QVERIFY(val.isNumber());
        QCOMPARE(val.toNumber(), qsreal(123));
    }
    {
        QScriptValue val = qScriptValueFromValue(&eng, QVariant(qsreal(1.25)));
        QVERIFY(!val.isVariant());
        QVERIFY(val.isNumber());
        QCOMPARE(val.toNumber(), qsreal(1.25));
    }
    {
        QString str = QString::fromLatin1("ciao");
        QScriptValue val = qScriptValueFromValue(&eng, QVariant(str));
        QVERIFY(!val.isVariant());
        QVERIFY(val.isString());
        QCOMPARE(val.toString(), str);
    }
    {
        QScriptValue val = qScriptValueFromValue(&eng, qVariantFromValue((QObject*)this));
        QVERIFY(!val.isVariant());
        QVERIFY(val.isQObject());
        QCOMPARE(val.toQObject(), (QObject*)this);
    }
    {
        QVariant var = qVariantFromValue(QPoint(123, 456));
        QScriptValue val = qScriptValueFromValue(&eng, var);
        QVERIFY(val.isVariant());
        QCOMPARE(val.toVariant(), var);
    }

    // task 248802
    qScriptRegisterMetaType<Foo>(&eng, fooToScriptValueV2, fooFromScriptValueV2);
    {
        QScriptValue num(&eng, 123);
        Foo foo = qScriptValueToValue<Foo>(num);
        QCOMPARE(foo.x, 123);
    }
    {
        QScriptValue num(123);
        Foo foo = qScriptValueToValue<Foo>(num);
        QCOMPARE(foo.x, -1);
    }
    {
        QScriptValue str(&eng, "123");
        Foo foo = qScriptValueToValue<Foo>(str);
        QCOMPARE(foo.x, 123);
    }
}

static QScriptValue __import__(QScriptContext *ctx, QScriptEngine *eng)
{
    return eng->importExtension(ctx->argument(0).toString());
}

void tst_QScriptEngine::importExtension()
{
    QStringList libPaths = QCoreApplication::instance()->libraryPaths();
    QCoreApplication::instance()->setLibraryPaths(QStringList() << ".");

    QStringList availableExtensions;
    {
        QScriptEngine eng;
        QVERIFY(eng.importedExtensions().isEmpty());
        QStringList ret = eng.availableExtensions();
        QCOMPARE(ret.size(), 4);
        QCOMPARE(ret.at(0), QString::fromLatin1("com"));
        QCOMPARE(ret.at(1), QString::fromLatin1("com.trolltech"));
        QCOMPARE(ret.at(2), QString::fromLatin1("com.trolltech.recursive"));
        QCOMPARE(ret.at(3), QString::fromLatin1("com.trolltech.syntaxerror"));
        availableExtensions = ret;
    }

    // try to import something that doesn't exist
    {
        QScriptEngine eng;
        QScriptValue ret = eng.importExtension("this.extension.does.not.exist");
        QCOMPARE(eng.hasUncaughtException(), true);
        QCOMPARE(ret.isError(), true);
    }

    {
        QScriptEngine eng;
        for (int x = 0; x < 2; ++x) {
            QCOMPARE(eng.globalObject().property("com").isValid(), x == 1);
            QScriptValue ret = eng.importExtension("com.trolltech");
            QCOMPARE(eng.hasUncaughtException(), false);
            QCOMPARE(ret.isUndefined(), true);

            QScriptValue com = eng.globalObject().property("com");
            QCOMPARE(com.isObject(), true);
            QCOMPARE(com.property("wasDefinedAlready")
                     .strictlyEquals(QScriptValue(&eng, false)), true);
            QCOMPARE(com.property("name")
                     .strictlyEquals(QScriptValue(&eng, "com")), true);
            QCOMPARE(com.property("level")
                     .strictlyEquals(QScriptValue(&eng, 1)), true);

            QScriptValue trolltech = com.property("trolltech");
            QCOMPARE(trolltech.isObject(), true);
            QCOMPARE(trolltech.property("wasDefinedAlready")
                     .strictlyEquals(QScriptValue(&eng, false)), true);
            QCOMPARE(trolltech.property("name")
                     .strictlyEquals(QScriptValue(&eng, "com.trolltech")), true);
            QCOMPARE(trolltech.property("level")
                     .strictlyEquals(QScriptValue(&eng, 2)), true);
        }
        QStringList imp = eng.importedExtensions();
        QCOMPARE(imp.size(), 2);
        QCOMPARE(imp.at(0), QString::fromLatin1("com"));
        QCOMPARE(imp.at(1), QString::fromLatin1("com.trolltech"));
        QCOMPARE(eng.availableExtensions(), availableExtensions);
    }

    // recursive import should throw an error
    {
        QScriptEngine eng;
        QVERIFY(eng.importedExtensions().isEmpty());
        eng.globalObject().setProperty("__import__", eng.newFunction(__import__));
        QScriptValue ret = eng.importExtension("com.trolltech.recursive");
        QCOMPARE(eng.hasUncaughtException(), true);
        QStringList imp = eng.importedExtensions();
        QCOMPARE(imp.size(), 2);
        QCOMPARE(imp.at(0), QString::fromLatin1("com"));
        QCOMPARE(imp.at(1), QString::fromLatin1("com.trolltech"));
        QCOMPARE(eng.availableExtensions(), availableExtensions);
    }

    {
        QScriptEngine eng;
        eng.globalObject().setProperty("__import__", eng.newFunction(__import__));
        for (int x = 0; x < 2; ++x) {
            if (x == 0)
                QVERIFY(eng.importedExtensions().isEmpty());
            QScriptValue ret = eng.importExtension("com.trolltech.syntaxerror");
            QVERIFY(eng.hasUncaughtException());
            QCOMPARE(eng.uncaughtExceptionLineNumber(), 4);
            QVERIFY(ret.isError());
            QCOMPARE(ret.property("message").toString(), QLatin1String("invalid assignment lvalue"));
        }
        QStringList imp = eng.importedExtensions();
        QCOMPARE(imp.size(), 2);
        QCOMPARE(imp.at(0), QString::fromLatin1("com"));
        QCOMPARE(imp.at(1), QString::fromLatin1("com.trolltech"));
        QCOMPARE(eng.availableExtensions(), availableExtensions);
    }

    QCoreApplication::instance()->setLibraryPaths(libPaths);
}

static QScriptValue recurse(QScriptContext *ctx, QScriptEngine *eng)
{
    Q_UNUSED(eng);
    return ctx->callee().call();
}

static QScriptValue recurse2(QScriptContext *ctx, QScriptEngine *eng)
{
    Q_UNUSED(eng);
    return ctx->callee().construct();
}

void tst_QScriptEngine::infiniteRecursion()
{
    QSKIP("Can cause C stack overflow (task 241294)", SkipAll);

    QScriptEngine eng;
    {
        QScriptValue ret = eng.evaluate("function foo() { foo(); }; foo();");
        QCOMPARE(ret.isError(), true);
        QCOMPARE(ret.toString(), QLatin1String("Error: call stack overflow"));
    }
    {
        QScriptValue fun = eng.newFunction(recurse);
        QScriptValue ret = fun.call();
        QCOMPARE(ret.isError(), true);
        QCOMPARE(ret.toString(), QLatin1String("Error: call stack overflow"));
    }
    {
        QScriptValue fun = eng.newFunction(recurse2);
        QScriptValue ret = fun.construct();
        QCOMPARE(ret.isError(), true);
        QCOMPARE(ret.toString(), QLatin1String("Error: call stack overflow"));
    }
}

struct Bar {
    int a;
};

struct Baz : public Bar {
    int b;
};

Q_DECLARE_METATYPE(Bar*)
Q_DECLARE_METATYPE(Baz*)

Q_DECLARE_METATYPE(QGradient)
Q_DECLARE_METATYPE(QGradient*)
Q_DECLARE_METATYPE(QLinearGradient)

class Zoo : public QObject
{
    Q_OBJECT
public:
    Zoo() { }
public slots:
    Baz *toBaz(Bar *b) { return reinterpret_cast<Baz*>(b); }
};

void tst_QScriptEngine::castWithPrototypeChain()
{
    QScriptEngine eng;
    Bar bar;
    Baz baz;
    QScriptValue barProto = qScriptValueFromValue(&eng, &bar);
    QScriptValue bazProto = qScriptValueFromValue(&eng, &baz);
    eng.setDefaultPrototype(qMetaTypeId<Bar*>(), barProto);
    eng.setDefaultPrototype(qMetaTypeId<Baz*>(), bazProto);

    Baz baz2;
    baz2.a = 123;
    baz2.b = 456;
    QScriptValue baz2Value = qScriptValueFromValue(&eng, &baz2);
    {
        Baz *pbaz = qscriptvalue_cast<Baz*>(baz2Value);
        QVERIFY(pbaz != 0);
        QCOMPARE(pbaz->b, baz2.b);

        Zoo zoo;
        QScriptValue scriptZoo = eng.newQObject(&zoo);
        QScriptValue toBaz = scriptZoo.property("toBaz");
        QVERIFY(toBaz.isFunction());

        // no relation between Bar and Baz's proto --> casting fails
        {
            Bar *pbar = qscriptvalue_cast<Bar*>(baz2Value);
            QVERIFY(pbar == 0);
        }

        {
            QScriptValue ret = toBaz.call(scriptZoo, QScriptValueList() << baz2Value);
            QVERIFY(ret.isError());
            QCOMPARE(ret.toString(), QLatin1String("TypeError: incompatible type of argument(s) in call to toBaz(); candidates were\n    toBaz(Bar*)"));
        }

        // establish chain -- now casting should work
        bazProto.setPrototype(barProto);

        {
            Bar *pbar = qscriptvalue_cast<Bar*>(baz2Value);
            QVERIFY(pbar != 0);
            QCOMPARE(pbar->a, baz2.a);
        }

        {
            QScriptValue ret = toBaz.call(scriptZoo, QScriptValueList() << baz2Value);
            QVERIFY(!ret.isError());
            QCOMPARE(qscriptvalue_cast<Baz*>(ret), pbaz);
        }
    }

    bazProto.setPrototype(barProto.prototype()); // kill chain
    {
        Baz *pbaz = qscriptvalue_cast<Baz*>(baz2Value);
        QVERIFY(pbaz != 0);
        // should not work anymore
        Bar *pbar = qscriptvalue_cast<Bar*>(baz2Value);
        QVERIFY(pbar == 0);
    }

    bazProto.setPrototype(eng.newQObject(this));
    {
        Baz *pbaz = qscriptvalue_cast<Baz*>(baz2Value);
        QVERIFY(pbaz != 0);
        // should not work now either
        Bar *pbar = qscriptvalue_cast<Bar*>(baz2Value);
        QVERIFY(pbar == 0);
    }

    {
        QScriptValue b = qScriptValueFromValue(&eng, QBrush());
        b.setPrototype(barProto);
        // this shows that a "wrong" cast is possible, if you
        // don't play by the rules (the pointer is actually a QBrush*)...
        Bar *pbar = qscriptvalue_cast<Bar*>(b);
        QVERIFY(pbar != 0);
    }

    {
        QScriptValue gradientProto = qScriptValueFromValue(&eng, QGradient());
        QScriptValue linearGradientProto = qScriptValueFromValue(&eng, QLinearGradient());
        linearGradientProto.setPrototype(gradientProto);
        QLinearGradient lg(10, 20, 30, 40);
        QScriptValue linearGradient = qScriptValueFromValue(&eng, lg);
        {
            QGradient *pgrad = qscriptvalue_cast<QGradient*>(linearGradient);
            QVERIFY(pgrad == 0);
        }
        linearGradient.setPrototype(linearGradientProto);
        {
            QGradient *pgrad = qscriptvalue_cast<QGradient*>(linearGradient);
            QVERIFY(pgrad != 0);
            QCOMPARE(pgrad->type(), QGradient::LinearGradient);
            QLinearGradient *plingrad = static_cast<QLinearGradient*>(pgrad);
            QCOMPARE(plingrad->start(), lg.start());
            QCOMPARE(plingrad->finalStop(), lg.finalStop());
        }
    }
}

class Klazz : public QWidget,
              public QStandardItem,
              public QGraphicsItem
{
    Q_OBJECT
public:
    Klazz(QWidget *parent = 0) : QWidget(parent) { }
    virtual QRectF boundingRect() const { return QRectF(); }
    virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) { }
};

Q_DECLARE_METATYPE(Klazz*)
Q_DECLARE_METATYPE(QStandardItem*)

void tst_QScriptEngine::castWithMultipleInheritance()
{
    QScriptEngine eng;
    Klazz klz;
    QScriptValue v = eng.newQObject(&klz);

    QCOMPARE(qscriptvalue_cast<Klazz*>(v), &klz);
    QCOMPARE(qscriptvalue_cast<QWidget*>(v), (QWidget *)&klz);
    QCOMPARE(qscriptvalue_cast<QObject*>(v), (QObject *)&klz);
    QCOMPARE(qscriptvalue_cast<QStandardItem*>(v), (QStandardItem *)&klz);
    QCOMPARE(qscriptvalue_cast<QGraphicsItem*>(v), (QGraphicsItem *)&klz);
}

void tst_QScriptEngine::collectGarbage()
{
    QScriptEngine eng;
    eng.evaluate("a = new Object(); a = new Object(); a = new Object()");
    QScriptValue a = eng.newObject();
    a = eng.newObject();
    a = eng.newObject();
    QPointer<QObject> ptr = new QObject();
    QVERIFY(ptr != 0);
    {
        QScriptValue v = eng.newQObject(ptr, QScriptEngine::ScriptOwnership);
    }
    eng.collectGarbage();
    QVERIFY(ptr == 0);
}

void tst_QScriptEngine::gcWithNestedDataStructure()
{
    QScriptEngine eng;
    eng.evaluate(
        "function makeList(size)"
        "{"
        "  var head = { };"
        "  var l = head;"
        "  for (var i = 0; i < size; ++i) {"
        "    l.data = i + \"\";"
        "    l.next = { }; l = l.next;"
        "  }"
        "  l.next = null;"
        "  return head;"
        "}");
    QCOMPARE(eng.hasUncaughtException(), false);
    const int size = 200;
    QScriptValue head = eng.evaluate(QString::fromLatin1("makeList(%0)").arg(size));
    QCOMPARE(eng.hasUncaughtException(), false);
    for (int x = 0; x < 2; ++x) {
        if (x == 1)
            eng.evaluate("gc()");
        QScriptValue l = head;
        for (int i = 0; i < 200; ++i) {
            QCOMPARE(l.property("data").toString(), QString::number(i));
            l = l.property("next");
        }
    }
}

class EventReceiver : public QObject
{
public:
    EventReceiver() {
        received = false;
    }

    bool event(QEvent *e) {
        received |= (e->type() == QEvent::User + 1);
        return QObject::event(e);
    }

    bool received;
};

void tst_QScriptEngine::processEventsWhileRunning()
{
    for (int x = 0; x < 2; ++x) {
        QScriptEngine eng;
        if (x == 0)
            eng.pushContext();

        QString script = QString::fromLatin1(
            "var end = Number(new Date()) + 1000;"
            "var x = 0;"
            "while (Number(new Date()) < end) {"
            "    ++x;"
            "}");

        EventReceiver receiver;
        QCoreApplication::postEvent(&receiver, new QEvent(QEvent::Type(QEvent::User+1)));

        eng.evaluate(script);
        QVERIFY(!eng.hasUncaughtException());
        QVERIFY(!receiver.received);

        QCOMPARE(eng.processEventsInterval(), -1);
        eng.setProcessEventsInterval(100);
        eng.evaluate(script);
        QVERIFY(!eng.hasUncaughtException());
        QVERIFY(receiver.received);

        if (x == 0)
            eng.popContext();
    }
}

class EventReceiver2 : public QObject
{
public:
    EventReceiver2(QScriptEngine *eng) {
        engine = eng;
    }

    bool event(QEvent *e) {
        if (e->type() == QEvent::User + 1) {
            engine->currentContext()->throwError("Killed");
        }
        return QObject::event(e);
    }

    QScriptEngine *engine;
};

void tst_QScriptEngine::throwErrorFromProcessEvents()
{
    QScriptEngine eng;

    EventReceiver2 receiver(&eng);
    QCoreApplication::postEvent(&receiver, new QEvent(QEvent::Type(QEvent::User+1)));

    eng.setProcessEventsInterval(100);
    QScriptValue ret = eng.evaluate(QString::fromLatin1("while (1) { }"));
    QVERIFY(ret.isError());
    QCOMPARE(ret.toString(), QString::fromLatin1("Error: Killed"));
}

void tst_QScriptEngine::stacktrace()
{
    QString script = QString::fromLatin1(
        "function foo(counter) {\n"
        "    switch (counter) {\n"
        "        case 0: foo(counter+1); break;\n"
        "        case 1: foo(counter+1); break;\n"
        "        case 2: foo(counter+1); break;\n"
        "        case 3: foo(counter+1); break;\n"
        "        case 4: foo(counter+1); break;\n"
        "        default:\n"
        "        throw new Error('blah');\n"
        "    }\n"
        "}\n"
        "foo(0);");

    const QString fileName("testfile");

    QStringList backtrace;
    backtrace << "foo(5)@testfile:9"
              << "foo(4)@testfile:7"
              << "foo(3)@testfile:6"
              << "foo(2)@testfile:5"
              << "foo(1)@testfile:4"
              << "foo(0)@testfile:3"
              << "<global>()@testfile:12";

    QScriptEngine eng;
    QScriptValue result = eng.evaluate(script, fileName);
    QVERIFY(eng.hasUncaughtException());
    QVERIFY(result.isError());

    QCOMPARE(eng.uncaughtExceptionBacktrace(), backtrace);
    QVERIFY(eng.hasUncaughtException());
    QVERIFY(result.strictlyEquals(eng.uncaughtException()));

    QCOMPARE(result.property("fileName").toString(), fileName);
    QCOMPARE(result.property("lineNumber").toInt32(), 9);

    QScriptValue stack = result.property("stack");
    QVERIFY(stack.isArray());

    QCOMPARE(stack.property("length").toInt32(), 7);

    QScriptValueIterator it(stack);
    int counter = 5;
    while (it.hasNext()) {
        it.next();
        QScriptValue obj = it.value();
        QScriptValue frame = obj.property("frame");

        QCOMPARE(obj.property("fileName").toString(), fileName);
        if (counter >= 0) {
            QScriptValue callee = frame.property("arguments").property("callee");
            QVERIFY(callee.strictlyEquals(eng.globalObject().property("foo")));
            QCOMPARE(obj.property("functionName").toString(), QString("foo"));
            int line = obj.property("lineNumber").toInt32();
            if (counter == 5)
                QCOMPARE(line, 9);
            else
                QCOMPARE(line, 3 + counter);
        } else {
            QVERIFY(frame.strictlyEquals(eng.globalObject()));
            QVERIFY(obj.property("functionName").toString().isEmpty());
        }

        --counter;
    }

    {
        QScriptValue bt = result.property("backtrace").call(result);
        QCOMPARE(qscriptvalue_cast<QStringList>(bt), backtrace);
    }

    // throw something that isn't an Error object
    eng.clearExceptions();
    QVERIFY(eng.uncaughtExceptionBacktrace().isEmpty());
    QString script2 = QString::fromLatin1(
        "function foo(counter) {\n"
        "    switch (counter) {\n"
        "        case 0: foo(counter+1); break;\n"
        "        case 1: foo(counter+1); break;\n"
        "        case 2: foo(counter+1); break;\n"
        "        case 3: foo(counter+1); break;\n"
        "        case 4: foo(counter+1); break;\n"
        "        default:\n"
        "        throw 'just a string';\n"
        "    }\n"
        "}\n"
        "foo(0);");

    QScriptValue result2 = eng.evaluate(script2, fileName);
    QVERIFY(eng.hasUncaughtException());
    QVERIFY(!result2.isError());
    QVERIFY(result2.isString());

    QCOMPARE(eng.uncaughtExceptionBacktrace(), backtrace);
    QVERIFY(eng.hasUncaughtException());

    eng.clearExceptions();
    QVERIFY(!eng.hasUncaughtException());
    QVERIFY(eng.uncaughtExceptionBacktrace().isEmpty());
}

void tst_QScriptEngine::numberParsing_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<qsreal>("expect");

    QTest::newRow("decimal 0") << QString("0") << qsreal(0);
    QTest::newRow("octal 0") << QString("00") << qsreal(00);
    QTest::newRow("hex 0") << QString("0x0") << qsreal(0x0);
    QTest::newRow("decimal 100") << QString("100") << qsreal(100);
    QTest::newRow("hex 100") << QString("0x100") << qsreal(0x100);
    QTest::newRow("octal 100") << QString("0100") << qsreal(0100);
    QTest::newRow("decimal 4G") << QString("4294967296") << qsreal(Q_UINT64_C(4294967296));
    QTest::newRow("hex 4G") << QString("0x100000000") << qsreal(Q_UINT64_C(0x100000000));
    QTest::newRow("octal 4G") << QString("040000000000") << qsreal(Q_UINT64_C(040000000000));
    QTest::newRow("0.5") << QString("0.5") << qsreal(0.5);
    QTest::newRow("1.5") << QString("1.5") << qsreal(1.5);
    QTest::newRow("1e2") << QString("1e2") << qsreal(100);
}

void tst_QScriptEngine::numberParsing()
{
    QFETCH(QString, string);
    QFETCH(qsreal, expect);

    QScriptEngine eng;
    QScriptValue ret = eng.evaluate(string);
    QVERIFY(ret.isNumber());
    qsreal actual = ret.toNumber();
    QCOMPARE(actual, expect);
}

// see ECMA-262, section 7.9
void tst_QScriptEngine::automaticSemicolonInsertion()
{
    QScriptEngine eng;
    {
        QScriptValue ret = eng.evaluate("{ 1 2 } 3");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("SyntaxError: Expected `;', `;'"));
    }
    {
        QScriptValue ret = eng.evaluate("{ 1\n2 } 3");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    {
        QScriptValue ret = eng.evaluate("for (a; b\n)");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("SyntaxError: Expected `;'"));
    }
    {
        QScriptValue ret = eng.evaluate("(function() { return\n1 + 2 })()");
        QVERIFY(ret.isUndefined());
    }
    {
        eng.evaluate("c = 2; b = 1");
        QScriptValue ret = eng.evaluate("a = b\n++c");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    {
        QScriptValue ret = eng.evaluate("if (a > b)\nelse c = d");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("SyntaxError"));
    }
    {
        eng.evaluate("function c() { return { foo: function() { return 5; } } }");
        eng.evaluate("b = 1; d = 2; e = 3");
        QScriptValue ret = eng.evaluate("a = b + c\n(d + e).foo()");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 6);
    }
    {
        QScriptValue ret = eng.evaluate("throw\n1");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("SyntaxError"));
    }
    {
        QScriptValue ret = eng.evaluate("a = Number(1)\n++a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 2);
    }

    // "a semicolon is never inserted automatically if the semicolon
    // would then be parsed as an empty statement"
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if (0)\n ++a; a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if (0)\n --a; a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if ((0))\n ++a; a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if ((0))\n --a; a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if (0\n)\n ++a; a");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if (0\n ++a; a");
        QVERIFY(ret.isError());
    }
    {
        eng.evaluate("a = 123");
        QScriptValue ret = eng.evaluate("if (0))\n ++a; a");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("n = 0; for (i = 0; i < 10; ++i)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
    }
    {
        QScriptValue ret = eng.evaluate("n = 30; for (i = 0; i < 10; ++i)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 20);
    }
    {
        QScriptValue ret = eng.evaluate("n = 0; for (var i = 0; i < 10; ++i)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
    }
    {
        QScriptValue ret = eng.evaluate("n = 30; for (var i = 0; i < 10; ++i)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 20);
    }
    {
        QScriptValue ret = eng.evaluate("n = 0; i = 0; while (i++ < 10)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
    }
    {
        QScriptValue ret = eng.evaluate("n = 30; i = 0; while (i++ < 10)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 20);
    }
    {
        QScriptValue ret = eng.evaluate("o = { a: 0, b: 1, c: 2 }; n = 0; for (i in o)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    {
        QScriptValue ret = eng.evaluate("o = { a: 0, b: 1, c: 2 }; n = 9; for (i in o)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 6);
    }
    {
        QScriptValue ret = eng.evaluate("o = { a: 0, b: 1, c: 2 }; n = 0; for (var i in o)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    {
        QScriptValue ret = eng.evaluate("o = { a: 0, b: 1, c: 2 }; n = 9; for (var i in o)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 6);
    }
    {
        QScriptValue ret = eng.evaluate("o = { n: 3 }; n = 5; with (o)\n ++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 5);
    }
    {
        QScriptValue ret = eng.evaluate("o = { n: 3 }; n = 10; with (o)\n --n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
    }
    {
        QScriptValue ret = eng.evaluate("n = 5; i = 0; do\n ++n; while (++i < 10); n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 15);
    }
    {
        QScriptValue ret = eng.evaluate("n = 20; i = 0; do\n --n; while (++i < 10); n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
    }

    {
        QScriptValue ret = eng.evaluate("n = 1; i = 0; if (n) i\n++n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 2);
    }
    {
        QScriptValue ret = eng.evaluate("n = 1; i = 0; if (n) i\n--n; n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 0);
    }

    {
        QScriptValue ret = eng.evaluate("if (0)");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("while (0)");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("for (;;)");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("for (p in this)");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("with (this)");
        QVERIFY(ret.isError());
    }
    {
        QScriptValue ret = eng.evaluate("do");
        QVERIFY(ret.isError());
    }
}

class EventReceiver3 : public QObject
{
public:
    enum AbortionResult {
        None = 0,
        String = 1,
        Error = 2
    };

    EventReceiver3(QScriptEngine *eng) {
        engine = eng;
        resultType = None;
    }

    bool event(QEvent *e) {
        if (e->type() == QEvent::User + 1) {
            switch (resultType) {
            case None:
                engine->abortEvaluation();
                break;
            case String:
                engine->abortEvaluation(QScriptValue(engine, QString::fromLatin1("Aborted")));
                break;
            case Error:
                engine->abortEvaluation(engine->currentContext()->throwError("AbortedWithError"));
                break;
            }
        }
        return QObject::event(e);
    }

    AbortionResult resultType;
    QScriptEngine *engine;
};

static QScriptValue myFunctionAbortingEvaluation(QScriptContext *, QScriptEngine *eng)
{
    eng->abortEvaluation();
    return eng->nullValue(); // should be ignored
}

void tst_QScriptEngine::abortEvaluation()
{
    QScriptEngine eng;

    eng.abortEvaluation();
    QVERIFY(!eng.hasUncaughtException());

    EventReceiver3 receiver(&eng);

    eng.setProcessEventsInterval(100);
    for (int x = 0; x < 3; ++x) {
        QCoreApplication::postEvent(&receiver, new QEvent(QEvent::Type(QEvent::User+1)));
        receiver.resultType = EventReceiver3::AbortionResult(x);
        QScriptValue ret = eng.evaluate(QString::fromLatin1("while (1) { }"));
        switch (receiver.resultType) {
        case EventReceiver3::None:
            QVERIFY(!eng.hasUncaughtException());
            QVERIFY(!ret.isValid());
            break;
        case EventReceiver3::String:
            QVERIFY(!eng.hasUncaughtException());
            QVERIFY(ret.isString());
            QCOMPARE(ret.toString(), QString::fromLatin1("Aborted"));
            break;
        case EventReceiver3::Error:
            QVERIFY(eng.hasUncaughtException());
            QVERIFY(ret.isError());
            QCOMPARE(ret.toString(), QString::fromLatin1("Error: AbortedWithError"));
            break;
        }
    }

    // scripts cannot intercept the abortion with try/catch
    for (int y = 0; y < 3; ++y) {
        QCoreApplication::postEvent(&receiver, new QEvent(QEvent::Type(QEvent::User+1)));
        receiver.resultType = EventReceiver3::AbortionResult(y);
        QScriptValue ret = eng.evaluate(QString::fromLatin1(
                                            "while (1) {\n"
                                            "  try {\n"
                                            "    (function() { while (1) { } })();\n"
                                            "  } catch (e) {\n"
                                            "    ;\n"
                                            "  }\n"
                                            "}"));
        switch (receiver.resultType) {
        case EventReceiver3::None:
            QVERIFY(!eng.hasUncaughtException());
            QVERIFY(!ret.isValid());
            break;
        case EventReceiver3::String:
            QVERIFY(!eng.hasUncaughtException());
            QVERIFY(ret.isString());
            QCOMPARE(ret.toString(), QString::fromLatin1("Aborted"));
            break;
        case EventReceiver3::Error:
            QVERIFY(eng.hasUncaughtException());
            QVERIFY(ret.isError());
            break;
        }
    }

    {
        QScriptValue fun = eng.newFunction(myFunctionAbortingEvaluation);
        eng.globalObject().setProperty("myFunctionAbortingEvaluation", fun);
        QScriptValue ret = eng.evaluate("myFunctionAbortingEvaluation()");
        QVERIFY(!ret.isValid());
    }
}

static QScriptValue myFunctionReturningIsEvaluating(QScriptContext *, QScriptEngine *eng)
{
    return QScriptValue(eng, eng->isEvaluating());
}

class EventReceiver4 : public QObject
{
public:
    EventReceiver4(QScriptEngine *eng) {
        engine = eng;
        wasEvaluating = false;
    }

    bool event(QEvent *e) {
        if (e->type() == QEvent::User + 1) {
            wasEvaluating = engine->isEvaluating();
        }
        return QObject::event(e);
    }

    QScriptEngine *engine;
    bool wasEvaluating;
};

void tst_QScriptEngine::isEvaluating()
{
    QScriptEngine eng;

    QVERIFY(!eng.isEvaluating());

    eng.evaluate("");
    QVERIFY(!eng.isEvaluating());
    eng.evaluate("123");
    QVERIFY(!eng.isEvaluating());
    eng.evaluate("0 = 0");
    QVERIFY(!eng.isEvaluating());

    {
        QScriptValue fun = eng.newFunction(myFunctionReturningIsEvaluating);
        eng.globalObject().setProperty("myFunctionReturningIsEvaluating", fun);
        QScriptValue ret = eng.evaluate("myFunctionReturningIsEvaluating()");
        QVERIFY(ret.isBoolean());
        QVERIFY(ret.toBoolean());
    }

    {
        EventReceiver4 receiver(&eng);
        QCoreApplication::postEvent(&receiver, new QEvent(QEvent::Type(QEvent::User+1)));

        QString script = QString::fromLatin1(
            "var end = Number(new Date()) + 1000;"
            "var x = 0;"
            "while (Number(new Date()) < end) {"
            "    ++x;"
            "}");

        eng.setProcessEventsInterval(100);
        eng.evaluate(script);
        QVERIFY(receiver.wasEvaluating);
    }
}

void tst_QScriptEngine::printThrowsException()
{
    QScriptEngine eng;
    QScriptValue ret = eng.evaluate("print({ toString: function() { throw 'foo'; } });");
    QVERIFY(eng.hasUncaughtException());
    QVERIFY(ret.strictlyEquals(QScriptValue(&eng, QLatin1String("foo"))));
}

void tst_QScriptEngine::errorConstructors()
{
    QScriptEngine eng;
    QStringList prefixes;
    prefixes << "" << "Eval" << "Range" << "Reference" << "Syntax" << "Type" << "URI";
    for (int x = 0; x < 2; ++x) {
        for (int i = 0; i < prefixes.size(); ++i) {
            QString name = prefixes.at(i) + QLatin1String("Error");
            QString code = QString(i+1, QLatin1Char('\n'));
            if (x == 0)
                code += QLatin1String("new ");
            code += name + QLatin1String("()");
            QScriptValue ret = eng.evaluate(code);
            QVERIFY(ret.isError());
            QVERIFY(!eng.hasUncaughtException());
            QCOMPARE(ret.toString(), name);
            QCOMPARE(ret.property("lineNumber").toInt32(), i+2);
        }
    }
}

void tst_QScriptEngine::argumentsProperty()
{
    {
        QScriptEngine eng;
        QVERIFY(eng.evaluate("arguments").isUndefined());
        eng.evaluate("arguments = 10");
        QScriptValue ret = eng.evaluate("arguments");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 10);
        QVERIFY(!eng.evaluate("delete arguments").toBoolean());
    }
    {
        QScriptEngine eng;
        eng.evaluate("o = { arguments: 123 }");
        QScriptValue ret = eng.evaluate("with (o) { arguments; }");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 123);
    }
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("(function() { arguments = 456; return arguments; })()");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 456);
        QVERIFY(eng.evaluate("arguments").isUndefined());
    }
}

void tst_QScriptEngine::numberClass()
{
    QScriptEngine eng;

    QScriptValue ctor = eng.globalObject().property("Number");
    QVERIFY(ctor.property("length").isNumber());
    QCOMPARE(ctor.property("length").toNumber(), qsreal(1));
    QScriptValue proto = ctor.property("prototype");
    QVERIFY(proto.isObject());
    {
        QScriptValue::PropertyFlags flags = QScriptValue::SkipInEnumeration
                                            | QScriptValue::Undeletable
                                            | QScriptValue::ReadOnly;
        QCOMPARE(ctor.propertyFlags("prototype"), flags);
        QVERIFY(ctor.property("MAX_VALUE").isNumber());
        QCOMPARE(ctor.propertyFlags("MAX_VALUE"), flags);
        QVERIFY(ctor.property("MIN_VALUE").isNumber());
        QCOMPARE(ctor.propertyFlags("MIN_VALUE"), flags);
        QVERIFY(ctor.property("NaN").isNumber());
        QCOMPARE(ctor.propertyFlags("NaN"), flags);
        QVERIFY(ctor.property("NEGATIVE_INFINITY").isNumber()); 
        QCOMPARE(ctor.propertyFlags("NEGATIVE_INFINITY"), flags);
        QVERIFY(ctor.property("POSITIVE_INFINITY").isNumber());
        QCOMPARE(ctor.propertyFlags("POSITIVE_INFINITY"), flags);
    }
    QVERIFY(proto.instanceOf(eng.globalObject().property("Object")));
    QCOMPARE(proto.toNumber(), qsreal(0));
    QVERIFY(proto.property("constructor").strictlyEquals(ctor));

    {
        QScriptValue ret = eng.evaluate("Number()");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toNumber(), qsreal(0));
    }
    {
        QScriptValue ret = eng.evaluate("Number(123)");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toNumber(), qsreal(123));
    }
    {
        QScriptValue ret = eng.evaluate("Number('456')");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toNumber(), qsreal(456));
    }
    {
        QScriptValue ret = eng.evaluate("new Number()");
        QVERIFY(!ret.isNumber());
        QVERIFY(ret.isObject());
        QCOMPARE(ret.toNumber(), qsreal(0));
    }
    {
        QScriptValue ret = eng.evaluate("new Number(123)");
        QVERIFY(!ret.isNumber());
        QVERIFY(ret.isObject());
        QCOMPARE(ret.toNumber(), qsreal(123));
    }
    {
        QScriptValue ret = eng.evaluate("new Number('456')");
        QVERIFY(!ret.isNumber());
        QVERIFY(ret.isObject());
        QCOMPARE(ret.toNumber(), qsreal(456));
    }

    QVERIFY(proto.property("toString").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).toString()");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("123"));
    }
    {
        QScriptValue ret = eng.evaluate("new Number(123).toString(8)");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("173"));
    }
    {
        QScriptValue ret = eng.evaluate("new Number(123).toString(16)");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("7b"));
    }
    QVERIFY(proto.property("toLocaleString").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).toLocaleString()");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("123"));
    }
    QVERIFY(proto.property("valueOf").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).valueOf()");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toNumber(), qsreal(123));
    }
    QVERIFY(proto.property("toExponential").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).toExponential()");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("1e+02"));
    }
    QVERIFY(proto.property("toFixed").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).toFixed()");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("123"));
    }
    QVERIFY(proto.property("toPrecision").isFunction());
    {
        QScriptValue ret = eng.evaluate("new Number(123).toPrecision()");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("1e+02"));
    }
}

void tst_QScriptEngine::forInStatement()
{
    QScriptEngine eng;
    {
        QScriptValue ret = eng.evaluate("o = { }; r = []; for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QVERIFY(lst.isEmpty());
    }
    {
        QScriptValue ret = eng.evaluate("o = { p: 123 }; r = [];"
                                        "for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 1);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
    }
    {
        QScriptValue ret = eng.evaluate("o = { p: 123, q: 456 }; r = [];"
                                        "for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 2);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
        QCOMPARE(lst.at(1), QString::fromLatin1("q"));
    }

    // properties in prototype
    {
        QScriptValue ret = eng.evaluate("o = { }; o.__proto__ = { p: 123 }; r = [];"
                                        "for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 1);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
    }
    {
        QScriptValue ret = eng.evaluate("o = { p: 123 }; o.__proto__ = { q: 456 }; r = [];"
                                        "for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 2);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
        QCOMPARE(lst.at(1), QString::fromLatin1("q"));
    }
    {
        // shadowed property
        QScriptValue ret = eng.evaluate("o = { p: 123 }; o.__proto__ = { p: 456 }; r = [];"
                                        "for (var p in o) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 1);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
    }

    // deleting property during enumeration
    {
        QScriptValue ret = eng.evaluate("o = { p: 123 }; r = [];"
                                        "for (var p in o) { r[r.length] = p; delete r[p]; } r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 1);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
    }
    {
        QScriptValue ret = eng.evaluate("o = { p: 123, q: 456 }; r = [];"
                                        "for (var p in o) { r[r.length] = p; delete o.q; } r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 1);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
    }

    // adding property during enumeration
    {
        QScriptValue ret = eng.evaluate("o = { p: 123 }; r = [];"
                                        "for (var p in o) { r[r.length] = p; o.q = 456; } r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 2);
        QCOMPARE(lst.at(0), QString::fromLatin1("p"));
        QCOMPARE(lst.at(1), QString::fromLatin1("q"));
    }

    // arrays
    {
        QScriptValue ret = eng.evaluate("a = [123, 456]; r = [];"
                                        "for (var p in a) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 2);
        QCOMPARE(lst.at(0), QString::fromLatin1("0"));
        QCOMPARE(lst.at(1), QString::fromLatin1("1"));
    }
    {
        QScriptValue ret = eng.evaluate("a = [123, 456]; a.foo = 'bar'; r = [];"
                                        "for (var p in a) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 3);
        QCOMPARE(lst.at(0), QString::fromLatin1("foo"));
        QCOMPARE(lst.at(1), QString::fromLatin1("0"));
        QCOMPARE(lst.at(2), QString::fromLatin1("1"));
    }
    {
        QScriptValue ret = eng.evaluate("a = [123, 456]; a.foo = 'bar';"
                                        "b = [111, 222, 333]; b.bar = 'baz';"
                                        "a.__proto__ = b; r = [];"
                                        "for (var p in a) r[r.length] = p; r");
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), 5);
        QCOMPARE(lst.at(0), QString::fromLatin1("foo"));
        QCOMPARE(lst.at(1), QString::fromLatin1("0"));
        QCOMPARE(lst.at(2), QString::fromLatin1("1"));
        QCOMPARE(lst.at(3), QString::fromLatin1("bar"));
    }

    // null and undefined
    // according to the spec, we should throw an exception; however, for
    // compability with the real world, we don't
    {
        QScriptValue ret = eng.evaluate("r = true; for (var p in undefined) r = false; r");
        QVERIFY(ret.isBool());
        QVERIFY(ret.toBool());
    }
    {
        QScriptValue ret = eng.evaluate("r = true; for (var p in null) r = false; r");
        QVERIFY(ret.isBool());
        QVERIFY(ret.toBool());
    }
}

void tst_QScriptEngine::functionExpression()
{
    // task 175679
    QScriptEngine eng;
    QVERIFY(!eng.globalObject().property("bar").isValid());
    eng.evaluate("function foo(arg) {\n"
                 "  if (arg == 'bar')\n"
                 "    function bar() { return 'bar'; }\n"
                 "  else\n"
                 "    function baz() { return 'baz'; }\n"
                 "  return (arg == 'bar') ? bar : baz;\n"
                 "}"); 
    QVERIFY(!eng.globalObject().property("bar").isValid());
    QVERIFY(!eng.globalObject().property("baz").isValid());
    QVERIFY(eng.evaluate("foo").isFunction());
    {
        QScriptValue ret = eng.evaluate("foo('bar')");
        QVERIFY(ret.isFunction());
        QScriptValue ret2 = ret.call(QScriptValue());
        QCOMPARE(ret2.toString(), QString::fromLatin1("bar"));
        QVERIFY(!eng.globalObject().property("bar").isValid());
        QVERIFY(!eng.globalObject().property("baz").isValid());
    }
    {
        QScriptValue ret = eng.evaluate("foo('baz')");
        QVERIFY(ret.isFunction());
        QScriptValue ret2 = ret.call(QScriptValue());
        QCOMPARE(ret2.toString(), QString::fromLatin1("baz"));
        QVERIFY(!eng.globalObject().property("bar").isValid());
        QVERIFY(!eng.globalObject().property("baz").isValid());
    }
}

void tst_QScriptEngine::stringObjects()
{
    QScriptEngine eng;
    QString str("ciao");
    // in C++
    {
        QScriptValue obj = QScriptValue(&eng, str).toObject();
        QCOMPARE(obj.property("length").toInt32(), str.length());
        QCOMPARE(obj.propertyFlags("length"), QScriptValue::PropertyFlags(QScriptValue::Undeletable | QScriptValue::SkipInEnumeration | QScriptValue::ReadOnly));
        for (int i = 0; i < str.length(); ++i) {
            QString pname = QString::number(i);
            QVERIFY(obj.property(pname).isString());
            QCOMPARE(obj.property(pname).toString(), QString(str.at(i)));
            QCOMPARE(obj.propertyFlags(pname), QScriptValue::PropertyFlags(QScriptValue::Undeletable | QScriptValue::ReadOnly));
            obj.setProperty(pname, QScriptValue());
            obj.setProperty(pname, QScriptValue(&eng, 123));
            QVERIFY(obj.property(pname).isString());
            QCOMPARE(obj.property(pname).toString(), QString(str.at(i)));
        }
        QVERIFY(!obj.property("-1").isValid());
        QVERIFY(!obj.property(QString::number(str.length())).isValid());

        QScriptValue val(&eng, 123);
        obj.setProperty("-1", val);
        QVERIFY(obj.property("-1").strictlyEquals(val));
        obj.setProperty("100", val);
        QVERIFY(obj.property("100").strictlyEquals(val));
    }

    // in script
    {
        QScriptValue ret = eng.evaluate("s = new String('ciao'); r = []; for (var p in s) r.push(p); r");
        QVERIFY(ret.isArray());
        QStringList lst = qscriptvalue_cast<QStringList>(ret);
        QCOMPARE(lst.size(), str.length());
        for (int i = 0; i < str.length(); ++i)
            QCOMPARE(lst.at(i), QString::number(i));

        QScriptValue ret2 = eng.evaluate("s[0] = 123; s[0]");
        QVERIFY(ret2.isString());
        QCOMPARE(ret2.toString().length(), 1);
        QCOMPARE(ret2.toString().at(0), str.at(0));

        QScriptValue ret3 = eng.evaluate("s[-1] = 123; s[-1]");
        QVERIFY(ret3.isNumber());
        QCOMPARE(ret3.toInt32(), 123);

        QScriptValue ret4 = eng.evaluate("s[s.length] = 456; s[s.length]");
        QVERIFY(ret4.isNumber());
        QCOMPARE(ret4.toInt32(), 456);

        QScriptValue ret5 = eng.evaluate("delete s[0]");
        QVERIFY(ret5.isBoolean());
        QVERIFY(!ret5.toBoolean());

        QScriptValue ret6 = eng.evaluate("delete s[-1]");
        QVERIFY(ret6.isBoolean());
        QVERIFY(ret6.toBoolean());

        QScriptValue ret7 = eng.evaluate("delete s[s.length]");
        QVERIFY(ret7.isBoolean());
        QVERIFY(ret7.toBoolean());
    }

    // task 212440
    {
        QScriptValue ret = eng.evaluate("replace_args = []; \"a a a\".replace(/(a)/g, function() { replace_args.push(arguments); }); replace_args");
        QVERIFY(ret.isArray());
        int len = ret.property("length").toInt32();
        QCOMPARE(len, 3);
        for (int i = 0; i < len; ++i) {
            QScriptValue args = ret.property(i);
            QCOMPARE(args.property("length").toInt32(), 4);
            QCOMPARE(args.property(0).toString(), QString::fromLatin1("a")); // matched string
            QCOMPARE(args.property(1).toString(), QString::fromLatin1("a")); // capture
            QVERIFY(args.property(2).isNumber());
            QCOMPARE(args.property(2).toInt32(), i*2); // index of match
            QCOMPARE(args.property(3).toString(), QString::fromLatin1("a a a"));
        }
    }
    // task 212501
    {
        QScriptValue ret = eng.evaluate("\"foo\".replace(/a/g, function() {})");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("foo"));
    }
}

void tst_QScriptEngine::getterSetterThisObject()
{
    // Global Object
    {
        QScriptEngine eng;
        // read
        eng.evaluate("__defineGetter__('x', function() { return this; });");
        {
            QScriptValue ret = eng.evaluate("x");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("(function() { return x; })()");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("with (this) x");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("with ({}) x");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("(function() { with ({}) return x; })()");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        // write
        eng.evaluate("__defineSetter__('x', function() { return this; });");
        {
            QScriptValue ret = eng.evaluate("x = 'foo'");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("(function() { return x = 'foo'; })()");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("with (this) x = 'foo'");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("with ({}) x = 'foo'");
            QVERIFY(ret.equals(eng.globalObject()));
        }
        {
            QScriptValue ret = eng.evaluate("(function() { with ({}) return x = 'foo'; })()");
            QVERIFY(ret.equals(eng.globalObject()));
        }
    }

    // other object
    {
        QScriptEngine eng;
        eng.evaluate("o = {}");
        // read
        eng.evaluate("o.__defineGetter__('x', function() { return this; })");
        QVERIFY(eng.evaluate("o.x === o").toBoolean());
        QVERIFY(eng.evaluate("with (o) x").equals(eng.evaluate("o")));
        QVERIFY(eng.evaluate("(function() { with (o) return x; })() === o").toBoolean());
        eng.evaluate("q = {}; with (o) with (q) x").equals(eng.evaluate("o"));
        // write
        eng.evaluate("o.__defineSetter__('x', function() { return this; });");
        QVERIFY(eng.evaluate("(o.x = 'foo') === o").toBoolean());
        QVERIFY(eng.evaluate("with (o) x = 'foo'").equals(eng.evaluate("o")));
        QVERIFY(eng.evaluate("with (o) with (q) x = 'foo'").equals(eng.evaluate("o")));
    }

    // getter+setter in prototype chain
    {
        QScriptEngine eng;
        eng.evaluate("o = {}; p = {}; o.__proto__ = p");
        // read
        eng.evaluate("p.__defineGetter__('x', function() { return this; })");
        QVERIFY(eng.evaluate("o.x === o").toBoolean());
        QVERIFY(eng.evaluate("with (o) x").equals(eng.evaluate("o")));
        QVERIFY(eng.evaluate("(function() { with (o) return x; })() === o").toBoolean());
        eng.evaluate("q = {}; with (o) with (q) x").equals(eng.evaluate("o"));
        eng.evaluate("with (q) with (o) x").equals(eng.evaluate("o"));
        // write
        eng.evaluate("o.__defineSetter__('x', function() { return this; });");
        QVERIFY(eng.evaluate("(o.x = 'foo') === o").toBoolean());
        QVERIFY(eng.evaluate("with (o) x = 'foo'").equals(eng.evaluate("o")));
        QVERIFY(eng.evaluate("with (o) with (q) x = 'foo'").equals(eng.evaluate("o")));
    }

    // getter+setter in activation
    {
        QScriptEngine eng;
        QScriptContext *ctx = eng.pushContext();
        QScriptValue act = ctx->activationObject();
        act.setProperty("act", act);
        // read
        eng.evaluate("act.__defineGetter__('x', function() { return this; })");
        QVERIFY(eng.evaluate("x === act").toBoolean());
        QVERIFY(eng.evaluate("with (act) x").equals(eng.evaluate("act")));
        QVERIFY(eng.evaluate("(function() { with (act) return x; })() === act").toBoolean());
        eng.evaluate("q = {}; with (act) with (q) x").equals(eng.evaluate("act"));
        eng.evaluate("with (q) with (act) x").equals(eng.evaluate("act"));
        // write
        eng.evaluate("act.__defineSetter__('x', function() { return this; });");
        QVERIFY(eng.evaluate("(x = 'foo') === act").toBoolean());
        QVERIFY(eng.evaluate("with (act) x = 'foo'").equals(eng.evaluate("act")));
        QVERIFY(eng.evaluate("with (act) with (q) x = 'foo'").equals(eng.evaluate("act")));
        eng.popContext();
    }
}

void tst_QScriptEngine::continueInSwitch()
{
    QScriptEngine eng;
    // switch - continue
    {
        QScriptValue ret = eng.evaluate("switch (1) { default: continue; }");
        QVERIFY(ret.isError());
    }
    // for - switch - case - continue
    {
        QScriptValue ret = eng.evaluate("j = 0; for (i = 0; i < 100000; ++i) {\n"
                                        "  switch (i) {\n"
                                        "    case 1: ++j; continue;\n"
                                        "    case 100: ++j; continue;\n"
                                        "    case 1000: ++j; continue;\n"
                                        "  }\n"
                                        "}; j");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    // for - switch - case - default - continue
    {
        QScriptValue ret = eng.evaluate("j = 0; for (i = 0; i < 100000; ++i) {\n"
                                        "  switch (i) {\n"
                                        "    case 1: ++j; continue;\n"
                                        "    case 100: ++j; continue;\n"
                                        "    case 1000: ++j; continue;\n"
                                        "    default: if (i < 50000) break; else continue;\n"
                                        "  }\n"
                                        "}; j");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
    // switch - for - continue
    {
        QScriptValue ret = eng.evaluate("j = 123; switch (j) {\n"
                                        "  case 123:\n"
                                        "  for (i = 0; i < 100000; ++i) {\n"
                                        "    continue;\n"
                                        "  }\n"
                                        "}; i\n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 100000);
    }
    // switch - switch - continue
    {
        QScriptValue ret = eng.evaluate("i = 1; switch (i) { default: switch (i) { case 1: continue; } }");
        QVERIFY(ret.isError());
    }
    // for - switch - switch - continue
    {
        QScriptValue ret = eng.evaluate("j = 0; for (i = 0; i < 100000; ++i) {\n"
                                        "  switch (i) {\n"
                                        "    case 1:\n"
                                        "    switch (i) {\n"
                                        "      case 1: ++j; continue;\n"
                                        "    }\n"
                                        "  }\n"
                                        "}; j");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 1);
    }
    // switch - for - switch - continue
    {
        QScriptValue ret = eng.evaluate("j = 123; switch (j) {\n"
                                        "  case 123:\n"
                                        "  for (i = 0; i < 100000; ++i) {\n"
                                        "    switch (i) {\n"
                                        "      case 1:\n"
                                        "      ++j; continue;\n"
                                        "    }\n"
                                        "  }\n"
                                        "}; i\n");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 100000);
    }
}

void tst_QScriptEngine::readOnlyPrototypeProperty()
{
    QScriptEngine eng;
    QCOMPARE(eng.evaluate("o = {}; o.__proto__ = parseInt; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length = 4; o.length").toInt32(), 2);
    QVERIFY(!eng.evaluate("o.hasOwnProperty('length')").toBoolean());
    QCOMPARE(eng.evaluate("o.length *= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length /= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length %= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length += 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length -= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length <<= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length >>= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length >>>= 2; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length &= 0; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length ^= 255; o.length").toInt32(), 2);
    QCOMPARE(eng.evaluate("o.length |= 255; o.length").toInt32(), 2);

    {
        QScriptValue ret = eng.evaluate("o.__defineGetter__('length', function() {})");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("Error: cannot redefine read-only property"));
    }
    {
        QScriptValue ret = eng.evaluate("o.__defineSetter__('length', function() {})");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("Error: cannot redefine read-only property"));
    }
}

void tst_QScriptEngine::toObject()
{
    QScriptEngine eng;

    QVERIFY(!eng.toObject(eng.undefinedValue()).isValid());

    QVERIFY(!eng.toObject(eng.nullValue()).isValid());

    QScriptValue falskt(false);
    {
        QScriptValue tmp = eng.toObject(falskt);
        QVERIFY(tmp.isObject());
        QCOMPARE(tmp.toNumber(), falskt.toNumber());
    }

    QScriptValue sant(true);
    {
        QScriptValue tmp = eng.toObject(sant);
        QVERIFY(tmp.isObject());
        QCOMPARE(tmp.toNumber(), sant.toNumber());
    }

    QScriptValue number(123.0);
    {
        QScriptValue tmp = eng.toObject(number);
        QVERIFY(tmp.isObject());
        QCOMPARE(tmp.toNumber(), number.toNumber());
    }

    QScriptValue str = QScriptValue(&eng, QString("ciao"));
    {
        QScriptValue tmp = eng.toObject(str);
        QVERIFY(tmp.isObject());
        QCOMPARE(tmp.toString(), str.toString());
    }

    QScriptValue object = eng.newObject();
    {
        QScriptValue tmp = eng.toObject(object);
        QVERIFY(tmp.isObject());
        QVERIFY(tmp.strictlyEquals(object));
    }

    QScriptValue qobject = eng.newQObject(this);
    QVERIFY(eng.toObject(qobject).strictlyEquals(qobject));

    QVERIFY(!eng.toObject(QScriptValue()).isValid());
}

void tst_QScriptEngine::reservedWords_data()
{
    QTest::addColumn<QString>("word");
    QTest::newRow("break") << QString("break");
    QTest::newRow("case") << QString("case");
    QTest::newRow("catch") << QString("catch");
    QTest::newRow("continue") << QString("continue");
    QTest::newRow("default") << QString("default");
    QTest::newRow("delete") << QString("delete");
    QTest::newRow("do") << QString("do");
    QTest::newRow("else") << QString("else");
    QTest::newRow("false") << QString("false");
    QTest::newRow("finally") << QString("finally");
    QTest::newRow("for") << QString("for");
    QTest::newRow("function") << QString("function");
    QTest::newRow("if") << QString("if");
    QTest::newRow("in") << QString("in");
    QTest::newRow("instanceof") << QString("instanceof");
    QTest::newRow("new") << QString("new");
    QTest::newRow("null") << QString("null");
    QTest::newRow("return") << QString("return");
    QTest::newRow("switch") << QString("switch");
    QTest::newRow("this") << QString("this");
    QTest::newRow("throw") << QString("throw");
    QTest::newRow("true") << QString("true");
    QTest::newRow("try") << QString("try");
    QTest::newRow("typeof") << QString("typeof");
    QTest::newRow("var") << QString("var");
    QTest::newRow("void") << QString("void");
    QTest::newRow("while") << QString("while");
    QTest::newRow("with") << QString("with");
}

void tst_QScriptEngine::reservedWords()
{
    QFETCH(QString, word);
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate(word + " = 123");
        QVERIFY(ret.isError());
        QVERIFY(ret.toString().startsWith("SyntaxError"));
    }
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("var " + word + " = 123");
        QVERIFY(ret.isError());
        QVERIFY(ret.toString().startsWith("SyntaxError"));
    }
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("o = {}; o." + word + " = 123");
        QVERIFY(!ret.isError());
        QVERIFY(ret.strictlyEquals(eng.evaluate("o." + word)));
    }
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("o = { " + word + ": 123 }");
        QVERIFY(!ret.isError());
        QVERIFY(ret.property(word).isNumber());
    }
    {
        // SpiderMonkey allows this, but we don't
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("function " + word + "() {}");
        QVERIFY(ret.isError());
        QVERIFY(ret.toString().startsWith("SyntaxError"));
    }
}

void tst_QScriptEngine::futureReservedWords_data()
{
    QTest::addColumn<QString>("word");
    QTest::newRow("abstract") << QString("abstract");
    QTest::newRow("boolean") << QString("boolean");
    QTest::newRow("byte") << QString("byte");
    QTest::newRow("char") << QString("char");
    QTest::newRow("class") << QString("class");
    QTest::newRow("const") << QString("const");
    QTest::newRow("debugger") << QString("debugger");
    QTest::newRow("double") << QString("double");
    QTest::newRow("enum") << QString("enum");
    QTest::newRow("export") << QString("export");
    QTest::newRow("extends") << QString("extends");
    QTest::newRow("final") << QString("final");
    QTest::newRow("float") << QString("float");
    QTest::newRow("goto") << QString("goto");
    QTest::newRow("implements") << QString("implements");
    QTest::newRow("import") << QString("import");
    QTest::newRow("int") << QString("int");
    QTest::newRow("interface") << QString("interface");
    QTest::newRow("long") << QString("long");
    QTest::newRow("native") << QString("native");
    QTest::newRow("package") << QString("package");
    QTest::newRow("private") << QString("private");
    QTest::newRow("protected") << QString("protected");
    QTest::newRow("public") << QString("public");
    QTest::newRow("short") << QString("short");
    QTest::newRow("static") << QString("static");
    QTest::newRow("super") << QString("super");
    QTest::newRow("synchronized") << QString("synchronized");
    QTest::newRow("throws") << QString("throws");
    QTest::newRow("transient") << QString("transient");
    QTest::newRow("volatile") << QString("volatile");
}

void tst_QScriptEngine::futureReservedWords()
{
    QFETCH(QString, word);
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate(word + " = 123");
        QVERIFY(ret.isError());
        QVERIFY(ret.toString().startsWith("SyntaxError"));
    }
    {
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("var " + word + " = 123");
        QVERIFY(ret.isError());
        QVERIFY(ret.toString().startsWith("SyntaxError"));
    }
    {
        // this should probably be allowed (see task 162567)
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("o = {}; o." + word + " = 123");
        QVERIFY(ret.isNumber());
    }
    {
        // this should probably be allowed (see task 162567)
        QScriptEngine eng;
        QScriptValue ret = eng.evaluate("o = { " + word + ": 123 }");
        QVERIFY(!ret.isError());
        QVERIFY(ret.isObject());
    }
}

void tst_QScriptEngine::throwInsideWithStatement()
{
    // task 209988
    QScriptEngine eng;
    {
        QScriptValue ret = eng.evaluate(
            "try {"
            "  o = { bad : \"bug\" };"
            "  with (o) {"
            "    throw 123;"
            "  }"
            "} catch (e) {"
            "  bad;"
            "}");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("ReferenceError: bad is not defined"));
    }
    {
        QScriptValue ret = eng.evaluate(
            "try {"
            "  o = { bad : \"bug\" };"
            "  with (o) {"
            "    throw 123;"
            "  }"
            "} finally {"
            "  bad;"
            "}");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("ReferenceError: bad is not defined"));
    }
    {
        QScriptValue ret = eng.evaluate(
            "o = { bug : \"no bug\" };"
            "with (o) {"
            "  try {"
            "    throw 123;"
            "  } finally {"
            "    bug;"
            "  }"
            "}");
        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("no bug"));
    }
    {
        QScriptValue ret = eng.evaluate(
            "o = { bug : \"no bug\" };"
            "with (o) {"
            "    throw 123;"
            "}");
        QVERIFY(ret.isNumber());
        QScriptValue ret2 = eng.evaluate("bug");
        QVERIFY(ret2.isError());
        QCOMPARE(ret2.toString(), QString::fromLatin1("ReferenceError: bug is not defined"));
    }
}

class TestAgent : public QScriptEngineAgent
{
public:
    TestAgent(QScriptEngine *engine) : QScriptEngineAgent(engine) {}
};

void tst_QScriptEngine::getSetAgent()
{
    QScriptEngine eng;
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)0);
    TestAgent agent(&eng);
    eng.setAgent(&agent);
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)&agent);
    eng.setAgent(0);
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)0);
}

void tst_QScriptEngine::reentrancy()
{
    {
        QScriptEngine eng1;
        QScriptEngine eng2;
        QScriptString s1 = eng1.toStringHandle("foo");
        QScriptString s2 = eng2.toStringHandle("foo");
        QVERIFY(s1 != s2);
    }
    {
        QScriptEngine eng1;
        QScriptEngine eng2;
        eng1.setProcessEventsInterval(123);
        QCOMPARE(eng2.processEventsInterval(), -1);
        eng2.setProcessEventsInterval(456);
        QCOMPARE(eng1.processEventsInterval(), 123);
    }
    {
        QScriptEngine eng1;
        QScriptEngine eng2;
        qScriptRegisterMetaType<Foo>(&eng1, fooToScriptValue, fooFromScriptValue);
        Foo foo;
        foo.x = 12;
        foo.y = 34;
        {
            QScriptValue fooVal = qScriptValueFromValue(&eng1, foo);
            QVERIFY(fooVal.isObject());
            QVERIFY(!fooVal.isVariant());
            QCOMPARE(fooVal.property("x").toInt32(), 12);
            QCOMPARE(fooVal.property("y").toInt32(), 34);
            fooVal.setProperty("x", 56);
            fooVal.setProperty("y", 78);

            Foo foo2 = qScriptValueToValue<Foo>(fooVal);
            QCOMPARE(foo2.x, 56);
            QCOMPARE(foo2.y, 78);
        }
        {
            QScriptValue fooVal = qScriptValueFromValue(&eng2, foo);
            QVERIFY(fooVal.isVariant());

            Foo foo2 = qScriptValueToValue<Foo>(fooVal);
            QCOMPARE(foo2.x, 12);
            QCOMPARE(foo2.y, 34);
        }
        QVERIFY(!eng1.defaultPrototype(qMetaTypeId<Foo>()).isValid());
        QVERIFY(!eng2.defaultPrototype(qMetaTypeId<Foo>()).isValid());
        QScriptValue proto1 = eng1.newObject();
        eng1.setDefaultPrototype(qMetaTypeId<Foo>(), proto1);
        QVERIFY(!eng2.defaultPrototype(qMetaTypeId<Foo>()).isValid());
        QScriptValue proto2 = eng2.newObject();
        eng2.setDefaultPrototype(qMetaTypeId<Foo>(), proto2);
        QVERIFY(eng2.defaultPrototype(qMetaTypeId<Foo>()).isValid());
        QVERIFY(eng1.defaultPrototype(qMetaTypeId<Foo>()).strictlyEquals(proto1));
    }
    {
        QScriptEngine eng1;
        QScriptEngine eng2;
        QVERIFY(!eng2.globalObject().property("a").isValid());
        eng1.evaluate("a = 10");
        QVERIFY(eng1.globalObject().property("a").isNumber());
        QVERIFY(!eng2.globalObject().property("a").isValid());
        eng2.evaluate("a = 20");
        QVERIFY(eng2.globalObject().property("a").isNumber());
        QCOMPARE(eng1.globalObject().property("a").toInt32(), 10);
    }
    // weird bug with JSC backend
    {
        QScriptEngine eng;
        QCOMPARE(eng.evaluate("Array()").toString(), QString());
        eng.evaluate("Array.prototype.toString");
        QCOMPARE(eng.evaluate("Array()").toString(), QString());
    }
    {
        QScriptEngine eng;
        QCOMPARE(eng.evaluate("Array()").toString(), QString());
    }
}

void tst_QScriptEngine:: incDecNonObjectProperty()
{
    QScriptEngine eng;
    {
        QScriptValue ret = eng.evaluate("var a; a.n++");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a; a.n--");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a = null; a.n++");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a = null; a.n--");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a; ++a.n");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a; --a.n");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a; a.n += 1");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a; a.n -= 1");
        QVERIFY(ret.isError());
        QCOMPARE(ret.toString(), QString::fromLatin1("TypeError: not an object"));
    }
    {
        QScriptValue ret = eng.evaluate("var a = 'ciao'; a.length++");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 4);
    }
    {
        QScriptValue ret = eng.evaluate("var a = 'ciao'; a.length--");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 4);
    }
    {
        QScriptValue ret = eng.evaluate("var a = 'ciao'; ++a.length");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 5);
    }
    {
        QScriptValue ret = eng.evaluate("var a = 'ciao'; --a.length");
        QVERIFY(ret.isNumber());
        QCOMPARE(ret.toInt32(), 3);
    }
}

QTEST_MAIN(tst_QScriptEngine)
#include "tst_qscriptengine.moc"
