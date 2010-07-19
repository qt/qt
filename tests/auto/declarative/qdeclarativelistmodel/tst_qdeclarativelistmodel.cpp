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
#include <QtDeclarative/private/qdeclarativeitem_p.h>
#include <QtDeclarative/private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativeengine_p.h>
#include <QtDeclarative/private/qdeclarativelistmodel_p.h>
#include <QtDeclarative/private/qdeclarativeexpression_p.h>
#include <QDeclarativeComponent>

#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtranslator.h>

#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativelistmodel : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativelistmodel() {}

private:
    QScriptValue nestedListValue(QScriptEngine *eng) const;
    QDeclarativeItem *createWorkerTest(QDeclarativeEngine *eng, QDeclarativeComponent *component, QDeclarativeListModel *model);
    void waitForWorker(QDeclarativeItem *item);

private slots:
    void static_types();
    void static_types_data();
    void static_i18n();
    void static_nestedElements();
    void static_nestedElements_data();
    void dynamic_data();
    void dynamic();
    void dynamic_worker_data();
    void dynamic_worker();
    void convertNestedToFlat_fail();
    void convertNestedToFlat_fail_data();
    void convertNestedToFlat_ok();
    void convertNestedToFlat_ok_data();
    void enumerate();
    void error_data();
    void error();
    void set();
};

QScriptValue tst_qdeclarativelistmodel::nestedListValue(QScriptEngine *eng) const
{
    QScriptValue list = eng->newArray();
    list.setProperty(0, eng->newObject());
    list.setProperty(1, eng->newObject());
    QScriptValue sv = eng->newObject();
    sv.setProperty("foo", list);
    return sv;
}

QDeclarativeItem *tst_qdeclarativelistmodel::createWorkerTest(QDeclarativeEngine *eng, QDeclarativeComponent *component, QDeclarativeListModel *model)
{
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component->create());
    QDeclarativeEngine::setContextForObject(model, eng->rootContext());
    if (item)
        item->setProperty("model", qVariantFromValue(model)); 
    return item;
}

void tst_qdeclarativelistmodel::waitForWorker(QDeclarativeItem *item)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    QDeclarativeProperty prop(item, "done");
    QVERIFY(prop.isValid());
    QVERIFY(prop.connectNotifySignal(&loop, SLOT(quit())));
    timer.start(10000);
    loop.exec();
    QVERIFY(timer.isActive());
}

void tst_qdeclarativelistmodel::static_i18n()
{
    QString expect = QString::fromUtf8("na\303\257ve");

    QString componentStr = "import Qt 4.7\nListModel { ListElement { prop1: \""+expect+"\"; prop2: QT_TR_NOOP(\""+expect+"\") } }";
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toUtf8(), QUrl::fromLocalFile(""));
    QDeclarativeListModel *obj = qobject_cast<QDeclarativeListModel*>(component.create());
    QVERIFY(obj != 0);
    QString prop1 = obj->get(0).property(QLatin1String("prop1")).toString();
    QCOMPARE(prop1,expect);
    QString prop2 = obj->get(0).property(QLatin1String("prop2")).toString();
    QCOMPARE(prop2,expect); // (no, not translated, QT_TR_NOOP is a no-op)
    delete obj;
}

void tst_qdeclarativelistmodel::static_nestedElements()
{
    QFETCH(int, elementCount);

    QStringList elements;
    for (int i=0; i<elementCount; i++) 
        elements.append("ListElement { a: 1; b: 2 }");
    QString elementsStr = elements.join(",\n") + "\n";

    QString componentStr = 
        "import Qt 4.7\n"
        "ListModel {\n"
        "   ListElement {\n"
        "       attributes: [\n";
    componentStr += elementsStr.toUtf8().constData();
    componentStr += 
        "       ]\n"
        "   }\n"
        "}";

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toUtf8(), QUrl::fromLocalFile(""));

    QDeclarativeListModel *obj = qobject_cast<QDeclarativeListModel*>(component.create());
    QVERIFY(obj != 0);

    QScriptValue prop = obj->get(0).property(QLatin1String("attributes")).property(QLatin1String("count"));
    QVERIFY(prop.isNumber());
    QCOMPARE(prop.toInt32(), qint32(elementCount));

    delete obj;
}

void tst_qdeclarativelistmodel::static_nestedElements_data()
{
    QTest::addColumn<int>("elementCount");

    QTest::newRow("0 items") << 0;
    QTest::newRow("1 item") << 1;
    QTest::newRow("2 items") << 2;
    QTest::newRow("many items") << 5;
}

void tst_qdeclarativelistmodel::dynamic_data()
{
    QTest::addColumn<QString>("script");
    QTest::addColumn<int>("result");
    QTest::addColumn<QString>("warning");

    // Simple flat model

    QTest::newRow("count") << "count" << 0 << "";

    QTest::newRow("get1") << "{get(0) === undefined}" << 1 << "";
    QTest::newRow("get2") << "{get(-1) === undefined}" << 1 << "";
    QTest::newRow("get3") << "{append({'foo':123});get(0) != undefined}" << 1 << "";

    QTest::newRow("append1") << "{append({'foo':123});count}" << 1 << "";
    QTest::newRow("append2") << "{append({'foo':123,'bar':456});count}" << 1 << "";
    QTest::newRow("append3a") << "{append({'foo':123});append({'foo':456});get(0).foo}" << 123 << "";
    QTest::newRow("append3b") << "{append({'foo':123});append({'foo':456});get(1).foo}" << 456 << "";
    QTest::newRow("append4a") << "{append(123)}" << 0 << "<Unknown File>: QML ListModel: append: value is not an object";
    QTest::newRow("append4b") << "{append([1,2,3])}" << 0 << "<Unknown File>: QML ListModel: append: value is not an object";

    QTest::newRow("clear1") << "{append({'foo':456});clear();count}" << 0 << "";
    QTest::newRow("clear2") << "{append({'foo':123});append({'foo':456});clear();count}" << 0 << "";
    QTest::newRow("clear3") << "{append({'foo':123});clear()}" << 0 << "";

    QTest::newRow("remove1") << "{append({'foo':123});remove(0);count}" << 0 << "";
    QTest::newRow("remove2a") << "{append({'foo':123});append({'foo':456});remove(0);count}" << 1 << "";
    QTest::newRow("remove2b") << "{append({'foo':123});append({'foo':456});remove(0);get(0).foo}" << 456 << "";
    QTest::newRow("remove2c") << "{append({'foo':123});append({'foo':456});remove(1);get(0).foo}" << 123 << "";
    QTest::newRow("remove3") << "{append({'foo':123});remove(0)}" << 0 << "";
    QTest::newRow("remove3a") << "{append({'foo':123});remove(-1);count}" << 1 << "<Unknown File>: QML ListModel: remove: index -1 out of range";
    QTest::newRow("remove4a") << "{remove(0)}" << 0 << "<Unknown File>: QML ListModel: remove: index 0 out of range";
    QTest::newRow("remove4b") << "{append({'foo':123});remove(0);remove(0);count}" << 0 << "<Unknown File>: QML ListModel: remove: index 0 out of range";
    QTest::newRow("remove4c") << "{append({'foo':123});remove(1);count}" << 1 << "<Unknown File>: QML ListModel: remove: index 1 out of range";

    QTest::newRow("insert1") << "{insert(0,{'foo':123});count}" << 1 << "";
    QTest::newRow("insert2") << "{insert(1,{'foo':123});count}" << 0 << "<Unknown File>: QML ListModel: insert: index 1 out of range";
    QTest::newRow("insert3a") << "{append({'foo':123});insert(1,{'foo':456});count}" << 2 << "";
    QTest::newRow("insert3b") << "{append({'foo':123});insert(1,{'foo':456});get(0).foo}" << 123 << "";
    QTest::newRow("insert3c") << "{append({'foo':123});insert(1,{'foo':456});get(1).foo}" << 456 << "";
    QTest::newRow("insert3d") << "{append({'foo':123});insert(0,{'foo':456});get(0).foo}" << 456 << "";
    QTest::newRow("insert3e") << "{append({'foo':123});insert(0,{'foo':456});get(1).foo}" << 123 << "";
    QTest::newRow("insert4") << "{append({'foo':123});insert(-1,{'foo':456});count}" << 1 << "<Unknown File>: QML ListModel: insert: index -1 out of range";
    QTest::newRow("insert5a") << "{insert(0,123)}" << 0 << "<Unknown File>: QML ListModel: insert: value is not an object";
    QTest::newRow("insert5b") << "{insert(0,[1,2,3])}" << 0 << "<Unknown File>: QML ListModel: insert: value is not an object";

    QTest::newRow("set1") << "{append({'foo':123});set(0,{'foo':456});count}" << 1 << "";
    QTest::newRow("set2") << "{append({'foo':123});set(0,{'foo':456});get(0).foo}" << 456 << "";
    QTest::newRow("set3a") << "{append({'foo':123,'bar':456});set(0,{'foo':999});get(0).foo}" << 999 << "";
    QTest::newRow("set3b") << "{append({'foo':123,'bar':456});set(0,{'foo':999});get(0).bar}" << 456 << "";
    QTest::newRow("set4a") << "{set(0,{'foo':456})}" << 0 << "<Unknown File>: QML ListModel: set: index 0 out of range";
    QTest::newRow("set4c") << "{set(-1,{'foo':456})}" << 0 << "<Unknown File>: QML ListModel: set: index -1 out of range";
    QTest::newRow("set5a") << "{append({'foo':123,'bar':456});set(0,123);count}" << 1 << "<Unknown File>: QML ListModel: set: value is not an object";
    QTest::newRow("set5b") << "{append({'foo':123,'bar':456});set(0,[1,2,3]);count}" << 1 << "<Unknown File>: QML ListModel: set: value is not an object";
    QTest::newRow("set6") << "{append({'foo':123});set(1,{'foo':456});count}" << 2 << "";

    QTest::newRow("setprop1") << "{append({'foo':123});setProperty(0,'foo',456);count}" << 1 << "";
    QTest::newRow("setprop2") << "{append({'foo':123});setProperty(0,'foo',456);get(0).foo}" << 456 << "";
    QTest::newRow("setprop3a") << "{append({'foo':123,'bar':456});setProperty(0,'foo',999);get(0).foo}" << 999 << "";
    QTest::newRow("setprop3b") << "{append({'foo':123,'bar':456});setProperty(0,'foo',999);get(0).bar}" << 456 << "";
    QTest::newRow("setprop4a") << "{setProperty(0,'foo',456)}" << 0 << "<Unknown File>: QML ListModel: set: index 0 out of range";
    QTest::newRow("setprop4b") << "{setProperty(-1,'foo',456)}" << 0 << "<Unknown File>: QML ListModel: set: index -1 out of range";
    QTest::newRow("setprop4c") << "{append({'foo':123,'bar':456});setProperty(1,'foo',456);count}" << 1 << "<Unknown File>: QML ListModel: set: index 1 out of range";
    QTest::newRow("setprop5") << "{append({'foo':123,'bar':456});append({'foo':111});setProperty(1,'bar',222);get(1).bar}" << 222 << "";

    QTest::newRow("move1a") << "{append({'foo':123});append({'foo':456});move(0,1,1);count}" << 2 << "";
    QTest::newRow("move1b") << "{append({'foo':123});append({'foo':456});move(0,1,1);get(0).foo}" << 456 << "";
    QTest::newRow("move1c") << "{append({'foo':123});append({'foo':456});move(0,1,1);get(1).foo}" << 123 << "";
    QTest::newRow("move1d") << "{append({'foo':123});append({'foo':456});move(1,0,1);get(0).foo}" << 456 << "";
    QTest::newRow("move1e") << "{append({'foo':123});append({'foo':456});move(1,0,1);get(1).foo}" << 123 << "";
    QTest::newRow("move2a") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);count}" << 3 << "";
    QTest::newRow("move2b") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(0).foo}" << 789 << "";
    QTest::newRow("move2c") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(1).foo}" << 123 << "";
    QTest::newRow("move2d") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(2).foo}" << 456 << "";
    QTest::newRow("move3a") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(1,0,3);count}" << 3 << "<Unknown File>: QML ListModel: move: out of range";
    QTest::newRow("move3b") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(1,-1,1);count}" << 3 << "<Unknown File>: QML ListModel: move: out of range";
    QTest::newRow("move3c") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(1,0,-1);count}" << 3 << "<Unknown File>: QML ListModel: move: out of range";
    QTest::newRow("move3d") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,3,1);count}" << 3 << "<Unknown File>: QML ListModel: move: out of range";

    // Nested models

    QTest::newRow("nested-append1") << "{append({'foo':123,'bars':[{'a':1},{'a':2},{'a':3}]});count}" << 1 << "";
    QTest::newRow("nested-append2") << "{append({'foo':123,'bars':[{'a':1},{'a':2},{'a':3}]});get(0).bars.get(1).a}" << 2 << "";
    QTest::newRow("nested-append3") << "{append({'foo':123,'bars':[{'a':1},{'a':2},{'a':3}]});get(0).bars.append({'a':4});get(0).bars.get(3).a}" << 4 << "";

    QTest::newRow("nested-insert") << "{append({'foo':123});insert(0,{'bars':[{'a':1},{'b':2},{'c':3}]});get(0).bars.get(0).a}" << 1 << "";
    QTest::newRow("nested-set") << "{append({'foo':123});set(0,{'foo':[{'x':123}]});get(0).foo.get(0).x}" << 123 << "";

    // XXX
    //QTest::newRow("nested-setprop") << "{append({'foo':123});setProperty(0,'foo',[{'x':123}]);get(0).foo.get(0).x}" << 123 << "";
}

void tst_qdeclarativelistmodel::dynamic()
{
    QFETCH(QString, script);
    QFETCH(int, result);
    QFETCH(QString, warning);

    QDeclarativeEngine engine;
    QDeclarativeListModel model;
    QDeclarativeEngine::setContextForObject(&model,engine.rootContext());
    engine.rootContext()->setContextObject(&model);
    QDeclarativeExpression e(engine.rootContext(), &model, script);
    if (!warning.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, warning.toLatin1());

    int actual = e.evaluate().toInt();
    if (e.hasError())
        qDebug() << e.error(); // errors not expected

    QCOMPARE(actual,result);
}

void tst_qdeclarativelistmodel::dynamic_worker_data()
{
    dynamic_data();
}

void tst_qdeclarativelistmodel::dynamic_worker()
{
    QFETCH(QString, script);
    QFETCH(int, result);
    QFETCH(QString, warning);

    // This is same as dynamic() except it applies the test to a ListModel called 
    // from a WorkerScript (i.e. testing the internal NestedListModel class)

    QDeclarativeListModel model;
    QDeclarativeEngine eng;
    QDeclarativeComponent component(&eng, QUrl::fromLocalFile(SRCDIR "/data/model.qml"));
    QDeclarativeItem *item = createWorkerTest(&eng, &component, &model);
    QVERIFY(item != 0);

    if (script[0] == QLatin1Char('{') && script[script.length()-1] == QLatin1Char('}'))
        script = script.mid(1, script.length() - 2);
    QVariantList operations;
    foreach (const QString &s, script.split(';')) {
        if (!s.isEmpty())
            operations << s;
    }

    if (!warning.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, warning.toLatin1());

    if (operations.count() == 1) {
        // test count(), get() return the correct default values in the worker list model
        QVERIFY(QMetaObject::invokeMethod(item, "evalExpressionViaWorker",
                Q_ARG(QVariant, operations)));
        waitForWorker(item);
        QCOMPARE(QDeclarativeProperty(item, "result").read().toInt(), result);
    } else {
        // execute a set of commands on the worker list model, then check the
        // changes are reflected in the list model in the main thread
        if (QByteArray(QTest::currentDataTag()).startsWith("nested"))
            QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML ListModel: Cannot add nested list values when modifying or after modification from a worker script");

        QVERIFY(QMetaObject::invokeMethod(item, "evalExpressionViaWorker", 
                Q_ARG(QVariant, operations.mid(0, operations.length()-1))));
        waitForWorker(item);

        QDeclarativeExpression e(eng.rootContext(), &model, operations.last().toString());
        if (QByteArray(QTest::currentDataTag()).startsWith("nested"))
            QVERIFY(e.evaluate().toInt() != result);
        else
            QCOMPARE(e.evaluate().toInt(), result);
    }

    delete item;
    qApp->processEvents();
}

void tst_qdeclarativelistmodel::convertNestedToFlat_fail()
{
    // If a model has nested data, it cannot be used at all from a worker script

    QFETCH(QString, script);

    QDeclarativeListModel model;
    QDeclarativeEngine eng;
    QDeclarativeComponent component(&eng, QUrl::fromLocalFile(SRCDIR "/data/model.qml"));
    QDeclarativeItem *item = createWorkerTest(&eng, &component, &model);
    QVERIFY(item != 0);

    QScriptEngine s_eng;
    QScriptValue plainData = s_eng.newObject();
    plainData.setProperty("foo", QScriptValue(123));
    model.append(plainData);
    model.append(nestedListValue(&s_eng));
    QCOMPARE(model.count(), 2);

    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML ListModel: List contains nested list values and cannot be used from a worker script");
    QVERIFY(QMetaObject::invokeMethod(item, "evalExpressionViaWorker", Q_ARG(QVariant, script)));
    waitForWorker(item);

    QCOMPARE(model.count(), 2);

    delete item;
    qApp->processEvents();
}

void tst_qdeclarativelistmodel::convertNestedToFlat_fail_data()
{
    QTest::addColumn<QString>("script");

    QTest::newRow("clear") << "clear()";
    QTest::newRow("remove") << "remove(0)";
    QTest::newRow("append") << "append({'x':1})";
    QTest::newRow("insert") << "insert(0, {'x':1})";
    QTest::newRow("set") << "set(0, {'foo':1})";
    QTest::newRow("setProperty") << "setProperty(0, 'foo', 1})";
    QTest::newRow("move") << "move(0, 1, 1})";
    QTest::newRow("get") << "get(0)";
}

void tst_qdeclarativelistmodel::convertNestedToFlat_ok()
{
    // If a model only has plain data, it can be modified from a worker script. However,
    // once the model is used from a worker script, it no longer accepts nested data

    QFETCH(QString, script);

    QDeclarativeListModel model;
    QDeclarativeEngine eng;
    QDeclarativeComponent component(&eng, QUrl::fromLocalFile(SRCDIR "/data/model.qml"));
    QDeclarativeItem *item = createWorkerTest(&eng, &component, &model);
    QVERIFY(item != 0);

    QScriptEngine s_eng;
    QScriptValue plainData = s_eng.newObject();
    plainData.setProperty("foo", QScriptValue(123));
    model.append(plainData);
    QCOMPARE(model.count(), 1);

    QVERIFY(QMetaObject::invokeMethod(item, "evalExpressionViaWorker", Q_ARG(QVariant, script)));
    waitForWorker(item);

    // can still add plain data
    int count = model.count();
    model.append(plainData);
    QCOMPARE(model.count(), count+1);

    QScriptValue nested = nestedListValue(&s_eng);
    const char *warning = "<Unknown File>: QML ListModel: Cannot add nested list values when modifying or after modification from a worker script";

    QTest::ignoreMessage(QtWarningMsg, warning);
    model.append(nested);

    QTest::ignoreMessage(QtWarningMsg, warning);
    model.insert(0, nested);

    QTest::ignoreMessage(QtWarningMsg, warning);
    model.set(0, nested);

    QCOMPARE(model.count(), count+1);

    delete item;
    qApp->processEvents();
}

void tst_qdeclarativelistmodel::convertNestedToFlat_ok_data()
{
    convertNestedToFlat_fail_data();
}

void tst_qdeclarativelistmodel::static_types_data()
{
    QTest::addColumn<QString>("qml");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("string")
        << "ListElement { foo: \"bar\" }"
        << QVariant(QString("bar"));

    QTest::newRow("real")
        << "ListElement { foo: 10.5 }"
        << QVariant(10.5);

    QTest::newRow("real0")
        << "ListElement { foo: 0 }"
        << QVariant(double(0));

    QTest::newRow("bool")
        << "ListElement { foo: false }"
        << QVariant(false);

    QTest::newRow("bool")
        << "ListElement { foo: true }"
        << QVariant(true);

    QTest::newRow("enum")
        << "ListElement { foo: Text.AlignHCenter }"
        << QVariant(double(QDeclarativeText::AlignHCenter));
}

void tst_qdeclarativelistmodel::static_types()
{
    QFETCH(QString, qml);
    QFETCH(QVariant, value);

    qml = "import Qt 4.7\nListModel { " + qml + " }";

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData(qml.toUtf8(),
                      QUrl::fromLocalFile(QString("dummy.qml")));

    if (value.toString().startsWith("QTBUG-"))
        QEXPECT_FAIL("",value.toString().toLatin1(),Abort);

    QVERIFY(!component.isError());

    QDeclarativeListModel *obj = qobject_cast<QDeclarativeListModel*>(component.create());
    QVERIFY(obj != 0);

    QScriptValue actual = obj->get(0).property(QLatin1String("foo"));

    QCOMPARE(actual.isString(), value.type() == QVariant::String);
    QCOMPARE(actual.isBoolean(), value.type() == QVariant::Bool);
    QCOMPARE(actual.isNumber(), value.type() == QVariant::Double);

    QCOMPARE(actual.toString(), value.toString());

    delete obj;
}

void tst_qdeclarativelistmodel::enumerate()
{
    QDeclarativeEngine eng;
    QDeclarativeComponent component(&eng, QUrl::fromLocalFile(SRCDIR "/data/enumerate.qml"));
    QVERIFY(!component.isError());
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QStringList r = item->property("result").toString().split(":");
    QCOMPARE(r[0],QLatin1String("val1=1Y"));
    QCOMPARE(r[1],QLatin1String("val2=2Y"));
    QCOMPARE(r[2],QLatin1String("val3=strY"));
    QCOMPARE(r[3],QLatin1String("val4=falseN"));
    QCOMPARE(r[4],QLatin1String("val5=trueY"));
    delete item;
}


void tst_qdeclarativelistmodel::error_data()
{
    QTest::addColumn<QString>("qml");
    QTest::addColumn<QString>("error");

    QTest::newRow("id not allowed in ListElement")
        << "import Qt 4.7\nListModel { ListElement { id: fred } }"
        << "ListElement: cannot use reserved \"id\" property";

    QTest::newRow("id allowed in ListModel")
        << "import Qt 4.7\nListModel { id:model }"
        << "";

    QTest::newRow("random properties not allowed in ListModel")
        << "import Qt 4.7\nListModel { foo:123 }"
        << "ListModel: undefined property 'foo'";

    QTest::newRow("random properties allowed in ListElement")
        << "import Qt 4.7\nListModel { ListElement { foo:123 } }"
        << "";

    QTest::newRow("bindings not allowed in ListElement")
        << "import Qt 4.7\nRectangle { id: rect; ListModel { ListElement { foo: rect.color } } }"
        << "ListElement: cannot use script for property value";

    QTest::newRow("random object list properties allowed in ListElement")
        << "import Qt 4.7\nListModel { ListElement { foo: [ ListElement { bar: 123 } ] } }"
        << "";

    QTest::newRow("default properties not allowed in ListElement")
        << "import Qt 4.7\nListModel { ListElement { Item { } } }"
        << "ListElement: cannot contain nested elements";

    QTest::newRow("QML elements not allowed in ListElement")
        << "import Qt 4.7\nListModel { ListElement { a: Item { } } }"
        << "ListElement: cannot contain nested elements";

    QTest::newRow("qualified ListElement supported")
        << "import Qt 4.7 as Foo\nFoo.ListModel { Foo.ListElement { a: 123 } }"
        << "";

    QTest::newRow("qualified ListElement required")
        << "import Qt 4.7 as Foo\nFoo.ListModel { ListElement { a: 123 } }"
        << "ListElement is not a type";

    QTest::newRow("unknown qualified ListElement not allowed")
        << "import Qt 4.7\nListModel { Foo.ListElement { a: 123 } }"
        << "Foo.ListElement - Foo is not a namespace";
}

void tst_qdeclarativelistmodel::error()
{
    QFETCH(QString, qml);
    QFETCH(QString, error);

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData(qml.toUtf8(),
                      QUrl::fromLocalFile(QString("dummy.qml")));
    if (error.isEmpty()) {
        QVERIFY(!component.isError());
    } else {
        QVERIFY(component.isError());
        QList<QDeclarativeError> errors = component.errors();
        QCOMPARE(errors.count(),1);
        QCOMPARE(errors.at(0).description(),error);
    }
}

void tst_qdeclarativelistmodel::set()
{
    QDeclarativeEngine engine;
    QDeclarativeListModel model;
    QDeclarativeEngine::setContextForObject(&model,engine.rootContext());
    engine.rootContext()->setContextObject(&model);
    QScriptEngine *seng = QDeclarativeEnginePrivate::getScriptEngine(&engine);

    QScriptValue sv = seng->newObject();
    sv.setProperty("test", QScriptValue(false));
    model.append(sv);

    sv.setProperty("test", QScriptValue(true));
    model.set(0, sv);
    QCOMPARE(model.get(0).property("test").toBool(), true); // triggers creation of model cache
    QCOMPARE(model.data(0, model.roles()[0]), qVariantFromValue(true)); 

    sv.setProperty("test", QScriptValue(false));
    model.set(0, sv);
    QCOMPARE(model.get(0).property("test").toBool(), false); // tests model cache is updated
    QCOMPARE(model.data(0, model.roles()[0]), qVariantFromValue(false)); 
}


QTEST_MAIN(tst_qdeclarativelistmodel)

#include "tst_qdeclarativelistmodel.moc"
