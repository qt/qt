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

#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalueiterator.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptValueIterator : public QObject
{
    Q_OBJECT

public:
    tst_QScriptValueIterator();
    virtual ~tst_QScriptValueIterator();

private slots:
    void iterateForward_data();
    void iterateForward();
    void iterateBackward_data();
    void iterateBackward();
    void iterateArray();
    void iterateBackAndForth();
    void setValue();
    void remove();
    void iterateString();
    void iterateGetterSetter();
    void iterateArgumentsObject();
    void assignObjectToIterator();
    void undefinedBehavior();
};

tst_QScriptValueIterator::tst_QScriptValueIterator()
{
}

tst_QScriptValueIterator::~tst_QScriptValueIterator()
{
}

void tst_QScriptValueIterator::iterateForward_data()
{
    QTest::addColumn<QStringList>("propertyNames");
    QTest::addColumn<QStringList>("propertyValues");

    QTest::newRow("no properties")
        << QStringList() << QStringList();
    QTest::newRow("foo=bar")
        << (QStringList() << "foo")
        << (QStringList() << "bar");
    QTest::newRow("foo=bar, baz=123")
        << (QStringList() << "foo" << "baz")
        << (QStringList() << "bar" << "123");
    QTest::newRow("foo=bar, baz=123, rab=oof")
        << (QStringList() << "foo" << "baz" << "rab")
        << (QStringList() << "bar" << "123" << "oof");
}

void tst_QScriptValueIterator::iterateForward()
{
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, propertyValues);
    QMap<QString, QString> pmap;
    Q_ASSERT(propertyNames.size() == propertyValues.size());

    QScriptEngine engine;
    QScriptValue object = engine.newObject();
    for (int i = 0; i < propertyNames.size(); ++i) {
        QString name = propertyNames.at(i);
        QString value = propertyValues.at(i);
        pmap.insert(name, value);
        object.setProperty(name, QScriptValue(&engine, value));
    }
    QScriptValue otherObject = engine.newObject();
    otherObject.setProperty("foo", QScriptValue(&engine, 123456));
    otherObject.setProperty("protoProperty", QScriptValue(&engine, 654321));
    object.setPrototype(otherObject); // should not affect iterator

    QStringList lst;
    QScriptValueIterator it(object);
    while (!pmap.isEmpty()) {
        QCOMPARE(it.hasNext(), true);
        QCOMPARE(it.hasNext(), true);
        it.next();
        QString name = it.name();
        QCOMPARE(pmap.contains(name), true);
        QCOMPARE(it.name(), name);
        QCOMPARE(it.flags(), object.propertyFlags(name));
        QCOMPARE(it.value().strictlyEquals(QScriptValue(&engine, pmap.value(name))), true);
        QCOMPARE(it.scriptName(), engine.toStringHandle(name));
        pmap.remove(name);
        lst.append(name);
    }

    QCOMPARE(it.hasNext(), false);
    QCOMPARE(it.hasNext(), false);

    it.toFront();
    for (int i = 0; i < lst.count(); ++i) {
        QCOMPARE(it.hasNext(), true);
        it.next();
        QCOMPARE(it.name(), lst.at(i));
    }

    for (int i = 0; i < lst.count(); ++i) {
        QCOMPARE(it.hasPrevious(), true);
        it.previous();
        QCOMPARE(it.name(), lst.at(lst.count()-1-i));
    }
    QCOMPARE(it.hasPrevious(), false);
}

void tst_QScriptValueIterator::iterateBackward_data()
{
    iterateForward_data();
}

void tst_QScriptValueIterator::iterateBackward()
{
    QFETCH(QStringList, propertyNames);
    QFETCH(QStringList, propertyValues);
    QMap<QString, QString> pmap;
    Q_ASSERT(propertyNames.size() == propertyValues.size());

    QScriptEngine engine;
    QScriptValue object = engine.newObject();
    for (int i = 0; i < propertyNames.size(); ++i) {
        QString name = propertyNames.at(i);
        QString value = propertyValues.at(i);
        pmap.insert(name, value);
        object.setProperty(name, QScriptValue(&engine, value));
    }

    QStringList lst;
    QScriptValueIterator it(object);
    it.toBack();
    while (!pmap.isEmpty()) {
        QCOMPARE(it.hasPrevious(), true);
        QCOMPARE(it.hasPrevious(), true);
        it.previous();
        QString name = it.name();
        QCOMPARE(pmap.contains(name), true);
        QCOMPARE(it.name(), name);
        QCOMPARE(it.flags(), object.propertyFlags(name));
        QCOMPARE(it.value().strictlyEquals(QScriptValue(&engine, pmap.value(name))), true);
        pmap.remove(name);
        lst.append(name);
    }

    QCOMPARE(it.hasPrevious(), false);
    QCOMPARE(it.hasPrevious(), false);

    it.toBack();
    for (int i = 0; i < lst.count(); ++i) {
        QCOMPARE(it.hasPrevious(), true);
        it.previous();
        QCOMPARE(it.name(), lst.at(i));
    }

    for (int i = 0; i < lst.count(); ++i) {
        QCOMPARE(it.hasNext(), true);
        it.next();
        QCOMPARE(it.name(), lst.at(lst.count()-1-i));
    }
    QCOMPARE(it.hasNext(), false);
}

void tst_QScriptValueIterator::iterateArray()
{
    QScriptEngine engine;
    QScriptValue array = engine.newArray();
    array.setProperty("0", QScriptValue(&engine, 123));
    array.setProperty("1", QScriptValue(&engine, 456));
    array.setProperty("2", QScriptValue(&engine, 789));
    int length = array.property("length").toInt32();
    QCOMPARE(length, 3);
    QScriptValueIterator it(array);
    for (int i = 0; i < length; ++i) {
        QCOMPARE(it.hasNext(), true);
        QString indexStr = QScriptValue(&engine, i).toString();
        it.next();
        QCOMPARE(it.name(), indexStr);
        QCOMPARE(it.flags(), array.propertyFlags(indexStr));
        QCOMPARE(it.value().strictlyEquals(array.property(indexStr)), true);
    }
    QCOMPARE(it.hasNext(), false);

    for (int i = length - 1; i >= 0; --i) {
        it.previous();
        QString indexStr = QScriptValue(&engine, i).toString();
        QCOMPARE(it.name(), indexStr);
        QCOMPARE(it.value().strictlyEquals(array.property(indexStr)), true);
        QCOMPARE(it.hasPrevious(), i > 0);
    }
    QCOMPARE(it.hasPrevious(), false);

    // hasNext() and hasPrevious() cache their result; verify that the result is in sync
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("0"));
    QVERIFY(it.hasNext());
    it.previous();
    QCOMPARE(it.name(), QString::fromLatin1("0"));
    QVERIFY(!it.hasPrevious());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("0"));
    QVERIFY(it.hasPrevious());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("1"));
}

void tst_QScriptValueIterator::iterateBackAndForth()
{
    QScriptEngine engine;
    {
        QScriptValue object = engine.newObject();
        object.setProperty("foo", QScriptValue(&engine, "bar"));
        object.setProperty("rab", QScriptValue(&engine, "oof"),
                           QScriptValue::SkipInEnumeration); // should not affect iterator
        QScriptValueIterator it(object);
        it.next();
        QCOMPARE(it.name(), QLatin1String("foo"));
        it.previous();
        QCOMPARE(it.name(), QLatin1String("foo"));
        it.next();
        QCOMPARE(it.name(), QLatin1String("foo"));
        it.previous();
        QCOMPARE(it.name(), QLatin1String("foo"));
        it.next();
        QCOMPARE(it.name(), QLatin1String("foo"));
        it.next();
        QCOMPARE(it.name(), QLatin1String("rab"));
        it.previous();
        QCOMPARE(it.name(), QLatin1String("rab"));
        it.next();
        QCOMPARE(it.name(), QLatin1String("rab"));
        it.previous();
        QCOMPARE(it.name(), QLatin1String("rab"));
    }
    {
        // hasNext() and hasPrevious() cache their result; verify that the result is in sync
        QScriptValue object = engine.newObject();
        object.setProperty("foo", QScriptValue(&engine, "bar"));
        object.setProperty("rab", QScriptValue(&engine, "oof"));
        QScriptValueIterator it(object);
        QVERIFY(it.hasNext());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QVERIFY(it.hasNext());
        it.previous();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QVERIFY(!it.hasPrevious());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QVERIFY(it.hasPrevious());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("rab"));
    }
}

void tst_QScriptValueIterator::setValue()
{
    QScriptEngine engine;
    QScriptValue object = engine.newObject();
    object.setProperty("foo", QScriptValue(&engine, "bar"));
    QScriptValueIterator it(object);
    it.next();
    QCOMPARE(it.name(), QLatin1String("foo"));
    it.setValue(QScriptValue(&engine, "baz"));
    QCOMPARE(it.value().strictlyEquals(QScriptValue(&engine, QLatin1String("baz"))), true);
    QCOMPARE(object.property("foo").toString(), QLatin1String("baz"));
    it.setValue(QScriptValue(&engine, "zab"));
    QCOMPARE(it.value().strictlyEquals(QScriptValue(&engine, QLatin1String("zab"))), true);
    QCOMPARE(object.property("foo").toString(), QLatin1String("zab"));
}

void tst_QScriptValueIterator::remove()
{
    QScriptEngine engine;
    QScriptValue object = engine.newObject();
    object.setProperty("foo", QScriptValue(&engine, "bar"),
                       QScriptValue::SkipInEnumeration); // should not affect iterator
    object.setProperty("rab", QScriptValue(&engine, "oof"));
    QScriptValueIterator it(object);
    it.next();
    QCOMPARE(it.name(), QLatin1String("foo"));
    it.remove();
    QCOMPARE(it.hasPrevious(), false);
    QCOMPARE(object.property("foo").isValid(), false);
    QCOMPARE(object.property("rab").toString(), QLatin1String("oof"));
    it.next();
    QCOMPARE(it.name(), QLatin1String("rab"));
    QCOMPARE(it.value().toString(), QLatin1String("oof"));
    QCOMPARE(it.hasNext(), false);
    it.remove();
    QCOMPARE(object.property("rab").isValid(), false);
    QCOMPARE(it.hasPrevious(), false);
    QCOMPARE(it.hasNext(), false);
}

void tst_QScriptValueIterator::iterateString()
{
    QScriptEngine engine;
    QScriptValue str = QScriptValue(&engine, QString::fromLatin1("ciao"));
    QVERIFY(str.isString());
    QScriptValue obj = str.toObject();
    int length = obj.property("length").toInt32();
    QCOMPARE(length, 4);
    QScriptValueIterator it(obj);
    for (int i = 0; i < length; ++i) {
        QCOMPARE(it.hasNext(), true);
        QString indexStr = QScriptValue(&engine, i).toString();
        it.next();
        QCOMPARE(it.name(), indexStr);
        QCOMPARE(it.flags(), obj.propertyFlags(indexStr));
        QCOMPARE(it.value().strictlyEquals(obj.property(indexStr)), true);
    }
    QCOMPARE(it.hasNext(), false);

    for (int i = length - 1; i >= 0; --i) {
        it.previous();
        QString indexStr = QScriptValue(&engine, i).toString();
        QCOMPARE(it.name(), indexStr);
        QCOMPARE(it.flags(), obj.propertyFlags(indexStr));
        QCOMPARE(it.value().strictlyEquals(obj.property(indexStr)), true);
        QCOMPARE(it.hasPrevious(), i > 0);
    }
    QCOMPARE(it.hasPrevious(), false);
}

static QScriptValue myGetterSetter(QScriptContext *ctx, QScriptEngine *)
{
    if (ctx->argumentCount() == 1)
        ctx->thisObject().setProperty("bar", ctx->argument(0));
    return ctx->thisObject().property("bar");
}

static QScriptValue myGetter(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->thisObject().property("bar");
}

static QScriptValue mySetter(QScriptContext *ctx, QScriptEngine *)
{
    ctx->thisObject().setProperty("bar", ctx->argument(0));
    return ctx->argument(0);
}

void tst_QScriptValueIterator::iterateGetterSetter()
{
    // unified getter/setter function
    {
        QScriptEngine eng;
        QScriptValue obj = eng.newObject();
        obj.setProperty("foo", eng.newFunction(myGetterSetter),
                        QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
        QScriptValue val(&eng, 123);
        obj.setProperty("foo", val);
        QVERIFY(obj.property("bar").strictlyEquals(val));
        QVERIFY(obj.property("foo").strictlyEquals(val));

        QScriptValueIterator it(obj);
        QVERIFY(it.hasNext());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QCOMPARE(it.flags(), QScriptValue::PropertyFlags(QScriptValue::PropertyGetter | QScriptValue::PropertySetter));
        QVERIFY(it.value().strictlyEquals(val));
        QScriptValue val2(&eng, 456);
        it.setValue(val2);
        QVERIFY(obj.property("bar").strictlyEquals(val2));
        QVERIFY(obj.property("foo").strictlyEquals(val2));

        QVERIFY(it.hasNext());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("bar"));
        QVERIFY(!it.hasNext());

        QVERIFY(it.hasPrevious());
        it.previous();
        QCOMPARE(it.name(), QString::fromLatin1("bar"));
        QVERIFY(it.hasPrevious());
        it.previous();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QCOMPARE(it.flags(), QScriptValue::PropertyFlags(QScriptValue::PropertyGetter | QScriptValue::PropertySetter));
        QVERIFY(it.value().strictlyEquals(val2));
        it.setValue(val);
        QVERIFY(obj.property("bar").strictlyEquals(val));
        QVERIFY(obj.property("foo").strictlyEquals(val));
    }
    // separate getter/setter function
    for (int x = 0; x < 2; ++x) {
        QScriptEngine eng;
        QScriptValue obj = eng.newObject();
        if (x == 0) {
            obj.setProperty("foo", eng.newFunction(myGetter), QScriptValue::PropertyGetter);
            obj.setProperty("foo", eng.newFunction(mySetter), QScriptValue::PropertySetter);
        } else {
            obj.setProperty("foo", eng.newFunction(mySetter), QScriptValue::PropertySetter);
            obj.setProperty("foo", eng.newFunction(myGetter), QScriptValue::PropertyGetter);
        }
        QScriptValue val(&eng, 123);
        obj.setProperty("foo", val);
        QVERIFY(obj.property("bar").strictlyEquals(val));
        QVERIFY(obj.property("foo").strictlyEquals(val));

        QScriptValueIterator it(obj);
        QVERIFY(it.hasNext());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QVERIFY(it.value().strictlyEquals(val));
        QScriptValue val2(&eng, 456);
        it.setValue(val2);
        QVERIFY(obj.property("bar").strictlyEquals(val2));
        QVERIFY(obj.property("foo").strictlyEquals(val2));

        QVERIFY(it.hasNext());
        it.next();
        QCOMPARE(it.name(), QString::fromLatin1("bar"));
        QVERIFY(!it.hasNext());

        QVERIFY(it.hasPrevious());
        it.previous();
        QCOMPARE(it.name(), QString::fromLatin1("bar"));
        QVERIFY(it.hasPrevious());
        it.previous();
        QCOMPARE(it.name(), QString::fromLatin1("foo"));
        QVERIFY(it.value().strictlyEquals(val2));
        it.setValue(val);
        QVERIFY(obj.property("bar").strictlyEquals(val));
        QVERIFY(obj.property("foo").strictlyEquals(val));
    }
}

static QScriptValue getArgumentsObject(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->argumentsObject();
}

void tst_QScriptValueIterator::iterateArgumentsObject()
{
    QScriptEngine eng;
    QScriptValue fun = eng.newFunction(getArgumentsObject);
    QScriptValue ret = fun.call(QScriptValue(), QScriptValueList() << QScriptValue(&eng, 123) << QScriptValue(&eng, 456));
    QCOMPARE(ret.property("length").toInt32(), 2);

    QScriptValueIterator it(ret);
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("callee"));
    QVERIFY(it.value().isFunction());
    QVERIFY(it.value().strictlyEquals(fun));
    QVERIFY(it.hasNext());

    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("length"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 2);
    QVERIFY(it.hasNext());

    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("0"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 123);
    QVERIFY(it.hasNext());

    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("1"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 456);
    QVERIFY(!it.hasNext());

    QVERIFY(it.hasPrevious());
    it.previous();
    QCOMPARE(it.name(), QString::fromLatin1("1"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 456);
    QVERIFY(it.hasPrevious());

    it.previous();
    QCOMPARE(it.name(), QString::fromLatin1("0"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 123);
    QVERIFY(it.hasPrevious());

    it.previous();
    QCOMPARE(it.name(), QString::fromLatin1("length"));
    QVERIFY(it.value().isNumber());
    QCOMPARE(it.value().toInt32(), 2);
    QVERIFY(it.hasPrevious());

    it.previous();
    QCOMPARE(it.name(), QString::fromLatin1("callee"));
    QVERIFY(it.value().isFunction());
    QVERIFY(it.value().strictlyEquals(fun));
    QVERIFY(!it.hasPrevious());
}

void tst_QScriptValueIterator::undefinedBehavior()
{
    QScriptEngine eng;
    QScriptValue obj = eng.newObject();
    obj.setProperty("foo", QScriptValue(&eng, 123));

    QScriptValueIterator it(obj);
    QVERIFY(it.hasNext());

    // delete the property
    obj.setProperty("foo", QScriptValue());

    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("foo"));
    QVERIFY(!it.value().isValid());

    QVERIFY(!it.hasNext());
    // add a property
    obj.setProperty("bar", QScriptValue(&eng, 123));
    QVERIFY(it.hasNext());

    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("bar"));
    QVERIFY(it.value().isNumber());
}

void tst_QScriptValueIterator::assignObjectToIterator()
{
    QScriptEngine eng;
    QScriptValue obj1 = eng.newObject();
    obj1.setProperty("foo", 123);
    QScriptValue obj2 = eng.newObject();
    obj2.setProperty("bar", 456);

    QScriptValueIterator it(obj1);
    QVERIFY(it.hasNext());
    it.next();
    it = obj2;
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("bar"));

    it = obj1;
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("foo"));

    it = obj2;
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("bar"));

    it = obj2;
    QVERIFY(it.hasNext());
    it.next();
    QCOMPARE(it.name(), QString::fromLatin1("bar"));
}

QTEST_MAIN(tst_QScriptValueIterator)
#include "tst_qscriptvalueiterator.moc"
