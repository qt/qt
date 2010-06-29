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

#include <qtest.h>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <private/qdeclarativeengine_p.h>
#include <private/qdeclarativeobjectscriptclass_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <QScriptEngine>
#include <QScriptValue>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_script : public QObject
{
    Q_OBJECT
public:
    tst_script() {}

private slots:
    void initTestCase();

    void property_js();
    void property_getter();
    void property_getter_js();
    void property_getter_qobject();
    void property_getter_qmetaproperty();
    void property_qobject();
    void property_qmlobject();

    void function_js();
    void function_cpp();
    void function_qobject();
    void function_qmlobject();

    void function_args_js();
    void function_args_cpp();
    void function_args_qobject();
    void function_args_qmlobject();

    void signal_unconnected();
    void signal_qml();
    void signal_args();
    void signal_unusedArgs();

    void slot_simple();
    void slot_simple_js();
    void slot_complex();
    void slot_complex_js();

    void block_data();
    void block();

    void global_property_js();
    void global_property_qml();
    void global_property_qml_js();

    void scriptfile_property();
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)

public:
    TestObject(QObject *parent = 0);

    int x();

    void emitMySignal() { emit mySignal(); }
    void emitMySignalWithArgs(int n) { emit mySignalWithArgs(n); }

signals:
    void mySignal();
    void mySignalWithArgs(int n);

public slots:
    int method() {
        return x();
    }

    int methodArgs(int val) {
        return val + x();
    }

private:
    int m_x;
};
QML_DECLARE_TYPE(TestObject);

TestObject::TestObject(QObject *parent)
: QObject(parent), m_x(0)
{
}

int TestObject::x() 
{
    return m_x++;
}

void tst_script::initTestCase()
{
    qmlRegisterType<TestObject>("Qt.test", 1, 0, "TestObject");
}


#define PROPERTY_PROGRAM \
    "(function(testObject) { return (function() { " \
    "    var test = 0; " \
    "    for (var ii = 0; ii < 10000; ++ii) { " \
    "        test += testObject.x; " \
    "    } " \
    "    return test; " \
    "}); })"

void tst_script::property_js()
{
    QScriptEngine engine;

    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("x"), 10);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call().toNumber();
    }
}

static QScriptValue property_getter_method(QScriptContext *, QScriptEngine *engine) 
{
    static int x = 0;
    return QScriptValue(engine,x++);
}

void tst_script::property_getter()
{
    QScriptEngine engine;

    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("x"), engine.newFunction(property_getter_method), 
                  QScriptValue::PropertyGetter);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

static TestObject *property_getter_qobject_object = 0;
static QScriptValue property_getter_qobject_method(QScriptContext *, QScriptEngine *) 
{
    static int idx = -1;
    if (idx == -1) 
        idx = TestObject::staticMetaObject.indexOfProperty("x");

    int value = 0;
    void *args[] = { &value, 0 };
    QMetaObject::metacall(property_getter_qobject_object, QMetaObject::ReadProperty, idx, args);

    return QScriptValue(value);
}

static QScriptValue property_getter_qmetaproperty_method(QScriptContext *, QScriptEngine *) 
{
    static int idx = -1;
    if (idx == -1) 
        idx = TestObject::staticMetaObject.indexOfProperty("x");

    int value = 0;
    value = property_getter_qobject_object->metaObject()->property(idx).read(property_getter_qobject_object).toInt();

    return QScriptValue(value);
}

void tst_script::property_getter_qobject()
{
    QScriptEngine engine;

    TestObject to;
    property_getter_qobject_object = &to;
    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("x"), engine.newFunction(property_getter_qobject_method), 
                  QScriptValue::PropertyGetter);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
    property_getter_qobject_object = 0;
}

void tst_script::property_getter_qmetaproperty()
{
    QScriptEngine engine;

    TestObject to;
    property_getter_qobject_object = &to;
    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("x"), engine.newFunction(property_getter_qmetaproperty_method), 
                  QScriptValue::PropertyGetter);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
    property_getter_qobject_object = 0;
}


void tst_script::property_getter_js()
{
    QScriptEngine engine;
    
    QScriptValue v = engine.evaluate("(function() { var o = new Object; o._x = 0; o.__defineGetter__(\"x\", function() { return this._x++; }); return o; })").call();

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::property_qobject()
{
    QScriptEngine engine;

    TestObject to;
    QScriptValue v = engine.newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(PROPERTY_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::property_qmlobject()
{
    QDeclarativeEngine qmlengine;

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(&qmlengine);
    TestObject to;

    QScriptValue v = QDeclarativeEnginePrivate::get(&qmlengine)->objectClass->newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine->evaluate(PROPERTY_PROGRAM).call(engine->globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

#define FUNCTION_PROGRAM \
    "(function(testObject) { return (function() { " \
    "    var test = 0; " \
    "    for (var ii = 0; ii < 10000; ++ii) { " \
    "        test += testObject.method(); " \
    "    } " \
    "    return test; " \
    "}); })"

void tst_script::function_js()
{
    QScriptEngine engine;
    
    QScriptValue v = engine.evaluate("(function() { var o = new Object; o._x = 0; o.method = (function() { return this._x++; }); return o; })").call();

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

static QScriptValue function_method(QScriptContext *, QScriptEngine *) 
{
    static int x = 0;
    return QScriptValue(x++);
}

void tst_script::function_cpp()
{
    QScriptEngine engine;

    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("method"), engine.newFunction(function_method));

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::function_qobject()
{
    QScriptEngine engine;

    TestObject to;
    QScriptValue v = engine.newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::function_qmlobject()
{
    QDeclarativeEngine qmlengine;

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(&qmlengine);
    TestObject to;

    QScriptValue v = QDeclarativeEnginePrivate::get(&qmlengine)->objectClass->newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine->evaluate(FUNCTION_PROGRAM).call(engine->globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

#define FUNCTION_ARGS_PROGRAM \
    "(function(testObject) { return (function() { " \
    "    var test = 0; " \
    "    for (var ii = 0; ii < 10000; ++ii) { " \
    "        test += testObject.methodArgs(ii); " \
    "    } " \
    "    return test; " \
    "}); })"

void tst_script::function_args_js()
{
    QScriptEngine engine;
    
    QScriptValue v = engine.evaluate("(function() { var o = new Object; o._x = 0; o.methodArgs = (function(a) { return a + this._x++; }); return o; })").call();

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_ARGS_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

static QScriptValue function_args_method(QScriptContext *ctxt, QScriptEngine *) 
{
    static int x = 0;
    return QScriptValue(ctxt->argument(0).toNumber() + x++);
}

void tst_script::function_args_cpp()
{
    QScriptEngine engine;

    QScriptValue v = engine.newObject();
    v.setProperty(QLatin1String("methodArgs"), engine.newFunction(function_args_method));

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_ARGS_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::function_args_qobject()
{
    QScriptEngine engine;

    TestObject to;
    QScriptValue v = engine.newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine.evaluate(FUNCTION_ARGS_PROGRAM).call(engine.globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::function_args_qmlobject()
{
    QDeclarativeEngine qmlengine;

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(&qmlengine);
    TestObject to;

    QScriptValue v = QDeclarativeEnginePrivate::get(&qmlengine)->objectClass->newQObject(&to);

    QScriptValueList args;
    args << v;
    QScriptValue prog = engine->evaluate(FUNCTION_ARGS_PROGRAM).call(engine->globalObject(), args);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::signal_unconnected()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("signal_unconnected.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::signal_qml()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("signal_qml.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::signal_args()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("signal_args.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignalWithArgs(11);
    }

    delete object;
}

void tst_script::signal_unusedArgs()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("signal_unusedArgs.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignalWithArgs(11);
    }

    delete object;
}

void tst_script::slot_simple()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("slot_simple.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::slot_simple_js()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("slot_simple_js.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::slot_complex()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("slot_complex.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::slot_complex_js()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("slot_complex_js.qml"));
    TestObject *object = qobject_cast<TestObject *>(component.create());
    QVERIFY(object != 0);

    QBENCHMARK {
        object->emitMySignal();
    }

    delete object;
}

void tst_script::block_data()
{
    QTest::addColumn<QString>("methodName");
    QTest::newRow("direct") << "doSomethingDirect()";
    QTest::newRow("localObj") << "doSomethingLocalObj()";
    QTest::newRow("local") << "doSomethingLocal()";
}

void tst_script::block()
{
    QFETCH(QString, methodName);
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("block.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle *>(component.create());
    QVERIFY(rect != 0);

    int index = rect->metaObject()->indexOfMethod(methodName.toUtf8());
    QVERIFY(index != -1);
    QMetaMethod method = rect->metaObject()->method(index);

    QBENCHMARK {
        method.invoke(rect, Qt::DirectConnection);
    }

    delete rect;
}

#define GLOBALPROPERTY_PROGRAM \
    "(function() { " \
    "    for (var ii = 0; ii < 10000; ++ii) { " \
    "        Math.sin(90); " \
    "    } " \
    "})"

void tst_script::global_property_js()
{
    QScriptEngine engine;

    QScriptValue prog = engine.evaluate(GLOBALPROPERTY_PROGRAM);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::global_property_qml()
{
    QDeclarativeEngine qmlengine;

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(&qmlengine);
    QScriptValue prog = engine->evaluate(GLOBALPROPERTY_PROGRAM);
    prog.call();

    QBENCHMARK {
        prog.call();
    }
}

void tst_script::global_property_qml_js()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("global_prop.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle *>(component.create());
    QVERIFY(rect != 0);

    int index = rect->metaObject()->indexOfMethod("triggered()");
    QVERIFY(index != -1);
    QMetaMethod method = rect->metaObject()->method(index);

    QBENCHMARK {
        method.invoke(rect, Qt::DirectConnection);
    }

    delete rect;
}

void tst_script::scriptfile_property()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("global_prop.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle *>(component.create());
    QVERIFY(rect != 0);

    int index = rect->metaObject()->indexOfMethod("incrementTriggered()");
    QVERIFY(index != -1);
    QMetaMethod method = rect->metaObject()->method(index);

    QBENCHMARK {
        method.invoke(rect, Qt::DirectConnection);
    }

    delete rect;
}

QTEST_MAIN(tst_script)

#include "tst_script.moc"
