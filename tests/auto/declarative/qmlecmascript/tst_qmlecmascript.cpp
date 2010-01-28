/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdebug.h>
#include <QtCore/private/qguard_p.h>
#include <QtCore/qdir.h>
#include <QtCore/qnumeric.h>
#include <private/qmlengine_p.h>
#include <private/qmlglobalscriptclass_p.h>
#include "testtypes.h"

/*
This test covers evaluation of ECMAScript expressions and bindings from within
QML.  This does not include static QML language issues.

Static QML language issues are covered in qmllanguage
*/
inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath("data/" + filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}

class tst_qmlecmascript : public QObject
{
    Q_OBJECT
public:
    tst_qmlecmascript() {}

private slots:
    void assignBasicTypes();
    void idShortcutInvalidates();
    void boolPropertiesEvaluateAsBool();
    void methods();
    void signalAssignment();
    void bindingLoop();
    void basicExpressions();
    void basicExpressions_data();
    void arrayExpressions();
    void contextPropertiesTriggerReeval();
    void objectPropertiesTriggerReeval();
    void deferredProperties();
    void extensionObjects();
    void attachedProperties();
    void enums();
    void valueTypeFunctions();
    void constantsOverrideBindings();
    void outerBindingOverridesInnerBinding();
    void aliasPropertyAndBinding();
    void nonExistantAttachedObject();
    void scope();
    void signalParameterTypes();
    void objectsCompareAsEqual();
    void scriptAccess();
    void dynamicCreation_data();
    void dynamicCreation();
    void dynamicDestruction();
    void objectToString();
    void selfDeletingBinding();
    void extendedObjectPropertyLookup();
    void scriptErrors();
    void signalTriggeredBindings();
    void listProperties();
    void exceptionClearsOnReeval();
    void exceptionSlotProducesWarning();
    void exceptionBindingProducesWarning();
    void transientErrors();
    void shutdownErrors();
    void externalScript();
    void compositePropertyType();
    void jsObject();
    void undefinedResetsProperty();

    void bug1();

    void callQtInvokables();
private:
    QmlEngine engine;
};

void tst_qmlecmascript::assignBasicTypes()
{
    {
    QmlComponent component(&engine, TEST_FILE("assignBasicTypes.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->flagProperty(), MyTypeObject::FlagVal1 | MyTypeObject::FlagVal3);
    QCOMPARE(object->enumProperty(), MyTypeObject::EnumVal2);
    QCOMPARE(object->stringProperty(), QString("Hello World!"));
    QCOMPARE(object->uintProperty(), uint(10));
    QCOMPARE(object->intProperty(), -19);
    QCOMPARE((float)object->realProperty(), float(23.2));
    QCOMPARE((float)object->doubleProperty(), float(-19.7));
    QCOMPARE((float)object->floatProperty(), float(8.5));
    QCOMPARE(object->colorProperty(), QColor("red"));
    QCOMPARE(object->dateProperty(), QDate(1982, 11, 25));
    QCOMPARE(object->timeProperty(), QTime(11, 11, 32));
    QCOMPARE(object->dateTimeProperty(), QDateTime(QDate(2009, 5, 12), QTime(13, 22, 1)));
    QCOMPARE(object->pointProperty(), QPoint(99,13));
    QCOMPARE(object->pointFProperty(), QPointF(-10.1, 12.3));
    QCOMPARE(object->sizeProperty(), QSize(99, 13));
    QCOMPARE(object->sizeFProperty(), QSizeF(0.1, 0.2));
    QCOMPARE(object->rectProperty(), QRect(9, 7, 100, 200));
    QCOMPARE(object->rectFProperty(), QRectF(1000.1, -10.9, 400, 90.99));
    QCOMPARE(object->boolProperty(), true);
    QCOMPARE(object->variantProperty(), QVariant("Hello World!"));
    QCOMPARE(object->vectorProperty(), QVector3D(10, 1, 2.2));
    QCOMPARE(object->urlProperty(), component.url().resolved(QUrl("main.qml")));
    delete object;
    }
    {
    QmlComponent component(&engine, TEST_FILE("assignBasicTypes.2.qml"));
    MyTypeObject *object = qobject_cast<MyTypeObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->flagProperty(), MyTypeObject::FlagVal1 | MyTypeObject::FlagVal3);
    QCOMPARE(object->enumProperty(), MyTypeObject::EnumVal2);
    QCOMPARE(object->stringProperty(), QString("Hello World!"));
    QCOMPARE(object->uintProperty(), uint(10));
    QCOMPARE(object->intProperty(), -19);
    QCOMPARE((float)object->realProperty(), float(23.2));
    QCOMPARE((float)object->doubleProperty(), float(-19.7));
    QCOMPARE((float)object->floatProperty(), float(8.5));
    QCOMPARE(object->colorProperty(), QColor("red"));
    QCOMPARE(object->dateProperty(), QDate(1982, 11, 25));
    QCOMPARE(object->timeProperty(), QTime(11, 11, 32));
    QCOMPARE(object->dateTimeProperty(), QDateTime(QDate(2009, 5, 12), QTime(13, 22, 1)));
    QCOMPARE(object->pointProperty(), QPoint(99,13));
    QCOMPARE(object->pointFProperty(), QPointF(-10.1, 12.3));
    QCOMPARE(object->sizeProperty(), QSize(99, 13));
    QCOMPARE(object->sizeFProperty(), QSizeF(0.1, 0.2));
    QCOMPARE(object->rectProperty(), QRect(9, 7, 100, 200));
    QCOMPARE(object->rectFProperty(), QRectF(1000.1, -10.9, 400, 90.99));
    QCOMPARE(object->boolProperty(), true);
    QCOMPARE(object->variantProperty(), QVariant("Hello World!"));
    QCOMPARE(object->vectorProperty(), QVector3D(10, 1, 2.2));
    QCOMPARE(object->urlProperty(), component.url().resolved(QUrl("main.qml")));
    delete object;
    }
}

void tst_qmlecmascript::idShortcutInvalidates()
{
    {
        QmlComponent component(&engine, TEST_FILE("idShortcutInvalidates.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QVERIFY(object->objectProperty() != 0);
        delete object->objectProperty();
        QVERIFY(object->objectProperty() == 0);
    }

    {
        QmlComponent component(&engine, TEST_FILE("idShortcutInvalidates.1.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QVERIFY(object->objectProperty() != 0);
        delete object->objectProperty();
        QVERIFY(object->objectProperty() == 0);
    }
}

void tst_qmlecmascript::boolPropertiesEvaluateAsBool()
{
    {
        QmlComponent component(&engine, TEST_FILE("boolPropertiesEvaluateAsBool.1.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
    {
        QmlComponent component(&engine, TEST_FILE("boolPropertiesEvaluateAsBool.2.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
}

void tst_qmlecmascript::signalAssignment()
{
    {
        QmlComponent component(&engine, TEST_FILE("signalAssignment.1.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->string(), QString());
        emit object->basicSignal();
        QCOMPARE(object->string(), QString("pass"));
    }

    {
        QmlComponent component(&engine, TEST_FILE("signalAssignment.2.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->string(), QString());
        emit object->argumentSignal(19, "Hello world!", 10.3);
        QCOMPARE(object->string(), QString("pass 19 Hello world! 10.3"));
    }
}

void tst_qmlecmascript::methods()
{
    {
        QmlComponent component(&engine, TEST_FILE("methods.1.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), false);
        emit object->basicSignal();
        QCOMPARE(object->methodCalled(), true);
        QCOMPARE(object->methodIntCalled(), false);
    }

    {
        QmlComponent component(&engine, TEST_FILE("methods.2.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), false);
        emit object->basicSignal();
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), true);
    }

    {
        QmlComponent component(&engine, TEST_FILE("methods.3.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);
        QCOMPARE(object->property("test").toInt(), 19);
    }

    {
        QmlComponent component(&engine, TEST_FILE("methods.4.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);
        QCOMPARE(object->property("test").toInt(), 19);
        QCOMPARE(object->property("test2").toInt(), 17);
        QCOMPARE(object->property("test3").toInt(), 16);
    }

    {
        QmlComponent component(&engine, TEST_FILE("methods.5.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);
        QCOMPARE(object->property("test").toInt(), 9);
    }
}

void tst_qmlecmascript::bindingLoop()
{
    QmlComponent component(&engine, TEST_FILE("bindingLoop.qml"));
    QString warning = "QML MyQmlObject (" + component.url().toString() + ":9:9) Binding loop detected for property \"stringProperty\"";
    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1().constData());
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_qmlecmascript::basicExpressions_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QVariant>("result");
    QTest::addColumn<bool>("nest");

    QTest::newRow("Syntax error (self test)") << "{console.log({'a':1'}.a)}" << QVariant() << false;
    QTest::newRow("Context property") << "a" << QVariant(1944) << false;
    QTest::newRow("Context property") << "a" << QVariant(1944) << true;
    QTest::newRow("Context property expression") << "a * 2" << QVariant(3888) << false;
    QTest::newRow("Context property expression") << "a * 2" << QVariant(3888) << true;
    QTest::newRow("Overridden context property") << "b" << QVariant("Milk") << false;
    QTest::newRow("Overridden context property") << "b" << QVariant("Cow") << true;
    QTest::newRow("Object property") << "object.stringProperty" << QVariant("Object1") << false;
    QTest::newRow("Object property") << "object.stringProperty" << QVariant("Object1") << true;
    QTest::newRow("Overridden object property") << "objectOverride.stringProperty" << QVariant("Object2") << false;
    QTest::newRow("Overridden object property") << "objectOverride.stringProperty" << QVariant("Object3") << true;
    QTest::newRow("Default object property") << "horseLegs" << QVariant(4) << false;
    QTest::newRow("Default object property") << "antLegs" << QVariant(6) << false;
    QTest::newRow("Default object property") << "emuLegs" << QVariant(2) << false;
    QTest::newRow("Nested default object property") << "horseLegs" << QVariant(4) << true;
    QTest::newRow("Nested default object property") << "antLegs" << QVariant(7) << true;
    QTest::newRow("Nested default object property") << "emuLegs" << QVariant(2) << true;
    QTest::newRow("Nested default object property") << "humanLegs" << QVariant(2) << true;
    QTest::newRow("Context property override default object property") << "millipedeLegs" << QVariant(100) << true;
}

void tst_qmlecmascript::basicExpressions()
{
    QFETCH(QString, expression);
    QFETCH(QVariant, result);
    QFETCH(bool, nest);

    MyQmlObject object1;
    MyQmlObject object2;
    MyQmlObject object3;
    MyDefaultObject1 default1;
    MyDefaultObject2 default2;
    MyDefaultObject3 default3;
    object1.setStringProperty("Object1");
    object2.setStringProperty("Object2");
    object3.setStringProperty("Object3");

    QmlContext context(engine.rootContext());
    QmlContext nestedContext(&context);

    context.addDefaultObject(&default1);
    context.addDefaultObject(&default2);
    context.setContextProperty("a", QVariant(1944));
    context.setContextProperty("b", QVariant("Milk"));
    context.setContextProperty("object", &object1);
    context.setContextProperty("objectOverride", &object2);
    nestedContext.addDefaultObject(&default3);
    nestedContext.setContextProperty("b", QVariant("Cow"));
    nestedContext.setContextProperty("objectOverride", &object3);
    nestedContext.setContextProperty("millipedeLegs", QVariant(100));

    MyExpression expr(nest?&nestedContext:&context, expression);
    QCOMPARE(expr.value(), result);
}

Q_DECLARE_METATYPE(QList<QObject *>);
void tst_qmlecmascript::arrayExpressions()
{
    QObject obj1;
    QObject obj2;
    QObject obj3;

    QmlContext context(engine.rootContext());
    context.setContextProperty("a", &obj1);
    context.setContextProperty("b", &obj2);
    context.setContextProperty("c", &obj3);

    MyExpression expr(&context, "[a, b, c, 10]");
    QVariant result = expr.value();
    QCOMPARE(result.userType(), qMetaTypeId<QList<QObject *> >());
    QList<QObject *> list = qvariant_cast<QList<QObject *> >(result);
    QCOMPARE(list.count(), 4);
    QCOMPARE(list.at(0), &obj1);
    QCOMPARE(list.at(1), &obj2);
    QCOMPARE(list.at(2), &obj3);
    QCOMPARE(list.at(3), (QObject *)0);
}

// Tests that modifying a context property will reevaluate expressions
void tst_qmlecmascript::contextPropertiesTriggerReeval()
{
    QmlContext context(engine.rootContext());
    MyQmlObject object1;
    MyQmlObject object2;
    MyQmlObject *object3 = new MyQmlObject;

    object1.setStringProperty("Hello");
    object2.setStringProperty("World");

    context.setContextProperty("testProp", QVariant(1));
    context.setContextProperty("testObj", &object1);
    context.setContextProperty("testObj2", object3);

    { 
        MyExpression expr(&context, "testProp + 1");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant(2));

        context.setContextProperty("testProp", QVariant(2));
        QCOMPARE(expr.changed, true);
        QCOMPARE(expr.value(), QVariant(3));
    }

    { 
        MyExpression expr(&context, "testProp + testProp + testProp");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant(6));

        context.setContextProperty("testProp", QVariant(4));
        QCOMPARE(expr.changed, true);
        QCOMPARE(expr.value(), QVariant(12));
    }

    { 
        MyExpression expr(&context, "testObj.stringProperty");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant("Hello"));

        context.setContextProperty("testObj", &object2);
        QCOMPARE(expr.changed, true);
        QCOMPARE(expr.value(), QVariant("World"));
    }

    { 
        MyExpression expr(&context, "testObj.stringProperty /**/");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant("World"));

        context.setContextProperty("testObj", &object1);
        QCOMPARE(expr.changed, true);
        QCOMPARE(expr.value(), QVariant("Hello"));
    }

    { 
        MyExpression expr(&context, "testObj2");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant::fromValue((QObject *)object3));
    }

}

void tst_qmlecmascript::objectPropertiesTriggerReeval()
{
    QmlContext context(engine.rootContext());
    MyQmlObject object1;
    MyQmlObject object2;
    MyQmlObject object3;
    context.setContextProperty("testObj", &object1);

    object1.setStringProperty(QLatin1String("Hello"));
    object2.setStringProperty(QLatin1String("Dog"));
    object3.setStringProperty(QLatin1String("Cat"));

    { 
        MyExpression expr(&context, "testObj.stringProperty");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant("Hello"));

        object1.setStringProperty(QLatin1String("World"));
        QCOMPARE(expr.changed, true);
        QCOMPARE(expr.value(), QVariant("World"));
    }

    { 
        MyExpression expr(&context, "testObj.objectProperty.stringProperty");
        QCOMPARE(expr.changed, false);
        QCOMPARE(expr.value(), QVariant());

        object1.setObjectProperty(&object2);
        QCOMPARE(expr.changed, true);
        expr.changed = false;
        QCOMPARE(expr.value(), QVariant("Dog"));

        object1.setObjectProperty(&object3);
        QCOMPARE(expr.changed, true);
        expr.changed = false;
        QCOMPARE(expr.value(), QVariant("Cat"));

        object1.setObjectProperty(0);
        QCOMPARE(expr.changed, true);
        expr.changed = false;
        QCOMPARE(expr.value(), QVariant());

        object1.setObjectProperty(&object3);
        QCOMPARE(expr.changed, true);
        expr.changed = false;
        QCOMPARE(expr.value(), QVariant("Cat"));

        object3.setStringProperty("Donkey");
        QCOMPARE(expr.changed, true);
        expr.changed = false;
        QCOMPARE(expr.value(), QVariant("Donkey"));
    }
}

void tst_qmlecmascript::deferredProperties()
{
    QmlComponent component(&engine, TEST_FILE("deferredProperties.qml"));
    MyDeferredObject *object = 
        qobject_cast<MyDeferredObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->value(), 0);
    QVERIFY(object->objectProperty() == 0);
    QVERIFY(object->objectProperty2() != 0);
    qmlExecuteDeferred(object);
    QCOMPARE(object->value(), 10);
    QVERIFY(object->objectProperty() != 0);
    MyQmlObject *qmlObject = 
        qobject_cast<MyQmlObject *>(object->objectProperty());
    QVERIFY(qmlObject != 0);
    QCOMPARE(qmlObject->value(), 10);
    object->setValue(19);
    QCOMPARE(qmlObject->value(), 19);
}

void tst_qmlecmascript::extensionObjects()
{
    QmlComponent component(&engine, TEST_FILE("extensionObjects.qml"));
    MyExtendedObject *object = 
        qobject_cast<MyExtendedObject *>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->baseProperty(), 13);
    QCOMPARE(object->coreProperty(), 9);

    object->setProperty("extendedProperty", QVariant(11));
    object->setProperty("baseExtendedProperty", QVariant(92));
    QCOMPARE(object->coreProperty(), 11);
    QCOMPARE(object->baseProperty(), 92);
}

void tst_qmlecmascript::attachedProperties()
{
    QmlComponent component(&engine, TEST_FILE("attachedProperty.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("a").toInt(), 19);
    QCOMPARE(object->property("b").toInt(), 19);
    QCOMPARE(object->property("c").toInt(), 19);
    QCOMPARE(object->property("d").toInt(), 19);

    // ### Need to test attached property assignment
}

void tst_qmlecmascript::enums()
{
    // Existant enums
    {
    QmlComponent component(&engine, TEST_FILE("enums.1.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("a").toInt(), 0);
    QCOMPARE(object->property("b").toInt(), 1);
    QCOMPARE(object->property("c").toInt(), 2);
    QCOMPARE(object->property("d").toInt(), 3);
    QCOMPARE(object->property("e").toInt(), 0);
    QCOMPARE(object->property("f").toInt(), 1);
    QCOMPARE(object->property("g").toInt(), 2);
    QCOMPARE(object->property("h").toInt(), 3);
    QCOMPARE(object->property("i").toInt(), 19);
    QCOMPARE(object->property("j").toInt(), 19);
    }
    // Non-existant enums
    {
    QmlComponent component(&engine, TEST_FILE("enums.2.qml"));

    QString warning1 = component.url().toString() + ":5: Unable to assign [undefined] to int";
    QString warning2 = component.url().toString() + ":6: Unable to assign [undefined] to int";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));

    QObject *object = component.create();
    QVERIFY(object != 0);
    QCOMPARE(object->property("a").toInt(), 0);
    QCOMPARE(object->property("b").toInt(), 0);
    }
}

void tst_qmlecmascript::valueTypeFunctions()
{
    QmlComponent component(&engine, TEST_FILE("valueTypeFunctions.qml"));
    MyTypeObject *obj = qobject_cast<MyTypeObject*>(component.create());
    QVERIFY(obj != 0);
    QCOMPARE(obj->rectProperty(), QRect(0,0,100,100));
    QCOMPARE(obj->rectFProperty(), QRectF(0,0.5,100,99.5));
}

/* 
Tests that writing a constant to a property with a binding on it disables the
binding.
*/
void tst_qmlecmascript::constantsOverrideBindings()
{
    // From ECMAScript
    {
        QmlComponent component(&engine, TEST_FILE("constantsOverrideBindings.1.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("c2").toInt(), 0);
        object->setProperty("c1", QVariant(9));
        QCOMPARE(object->property("c2").toInt(), 9);

        emit object->basicSignal();

        QCOMPARE(object->property("c2").toInt(), 13);
        object->setProperty("c1", QVariant(8));
        QCOMPARE(object->property("c2").toInt(), 13);
    }

    // During construction
    {
        QmlComponent component(&engine, TEST_FILE("constantsOverrideBindings.2.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("c1").toInt(), 0);
        QCOMPARE(object->property("c2").toInt(), 10);
        object->setProperty("c1", QVariant(9));
        QCOMPARE(object->property("c1").toInt(), 9);
        QCOMPARE(object->property("c2").toInt(), 10);
    }

#if 0
    // From C++
    {
        QmlComponent component(&engine, TEST_FILE("constantsOverrideBindings.3.qml"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);

        QCOMPARE(object->property("c2").toInt(), 0);
        object->setProperty("c1", QVariant(9));
        QCOMPARE(object->property("c2").toInt(), 9);

        object->setProperty("c2", QVariant(13));
        QCOMPARE(object->property("c2").toInt(), 13);
        object->setProperty("c1", QVariant(7));
        QCOMPARE(object->property("c1").toInt(), 7);
        QCOMPARE(object->property("c2").toInt(), 13);
    }
#endif
}

/*
Tests that assigning a binding to a property that already has a binding causes
the original binding to be disabled.
*/
void tst_qmlecmascript::outerBindingOverridesInnerBinding()
{
    QmlComponent component(&engine, 
                           TEST_FILE("outerBindingOverridesInnerBinding.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("c1").toInt(), 0);
    QCOMPARE(object->property("c2").toInt(), 0);
    QCOMPARE(object->property("c3").toInt(), 0);

    object->setProperty("c1", QVariant(9));
    QCOMPARE(object->property("c1").toInt(), 9);
    QCOMPARE(object->property("c2").toInt(), 0);
    QCOMPARE(object->property("c3").toInt(), 0);

    object->setProperty("c3", QVariant(8));
    QCOMPARE(object->property("c1").toInt(), 9);
    QCOMPARE(object->property("c2").toInt(), 8);
    QCOMPARE(object->property("c3").toInt(), 8);
}

/*
Access a non-existant attached object.  

Tests for a regression where this used to crash.
*/
void tst_qmlecmascript::nonExistantAttachedObject()
{
    QmlComponent component(&engine, TEST_FILE("nonExistantAttachedObject.qml"));

    QString warning = component.url().toString() + ":4: Unable to assign [undefined] to QString";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));

    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_qmlecmascript::scope()
{
    {
        QmlComponent component(&engine, TEST_FILE("scope.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test1").toInt(), 1);
        QCOMPARE(object->property("test2").toInt(), 2);
        QCOMPARE(object->property("test3").toString(), QString("1Test"));
        QCOMPARE(object->property("test4").toString(), QString("2Test"));
        QCOMPARE(object->property("test5").toInt(), 1);
        QCOMPARE(object->property("test6").toInt(), 1);
        QCOMPARE(object->property("test7").toInt(), 2);
        QCOMPARE(object->property("test8").toInt(), 2);
        QCOMPARE(object->property("test9").toInt(), 1);
        QCOMPARE(object->property("test10").toInt(), 3);
    }

    {
        QmlComponent component(&engine, TEST_FILE("scope.2.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test1").toInt(), 19);
        QCOMPARE(object->property("test2").toInt(), 19);
        QCOMPARE(object->property("test3").toInt(), 11);
        QCOMPARE(object->property("test4").toInt(), 11);
        QCOMPARE(object->property("test5").toInt(), 24);
        QCOMPARE(object->property("test6").toInt(), 24);
    }
}

/*
Tests that "any" type passes through a synthesized signal parameter.  This
is essentially a test of QmlMetaType::copy()
*/
void tst_qmlecmascript::signalParameterTypes()
{
    QmlComponent component(&engine, TEST_FILE("signalParameterTypes.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);

    emit object->basicSignal();

    QCOMPARE(object->property("intProperty").toInt(), 10);
    QCOMPARE(object->property("realProperty").toReal(), 19.2);
    QVERIFY(object->property("colorProperty").value<QColor>() == QColor(255, 255, 0, 255));
    QVERIFY(object->property("variantProperty") == QVariant::fromValue(QColor(255, 0, 255, 255)));
}

/*
Test that two JS objects for the same QObject compare as equal.
*/
void tst_qmlecmascript::objectsCompareAsEqual()
{
    QmlComponent component(&engine, TEST_FILE("objectsCompareAsEqual.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toBool(), true);
    QCOMPARE(object->property("test2").toBool(), true);
    QCOMPARE(object->property("test3").toBool(), true);
    QCOMPARE(object->property("test4").toBool(), true);
    QCOMPARE(object->property("test5").toBool(), true);
}

/*
Confirm bindings and alias properties can coexist.

Tests for a regression where the binding would not reevaluate.
*/
void tst_qmlecmascript::aliasPropertyAndBinding()
{
    QmlComponent component(&engine, TEST_FILE("aliasPropertyAndBinding.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("c2").toInt(), 3);
    QCOMPARE(object->property("c3").toInt(), 3);

    object->setProperty("c2", QVariant(19));

    QCOMPARE(object->property("c2").toInt(), 19);
    QCOMPARE(object->property("c3").toInt(), 19);
}

/*
Tests that only methods of Script {} blocks are exposed.
*/
void tst_qmlecmascript::scriptAccess()
{
    QmlComponent component(&engine, TEST_FILE("scriptAccess.qml"));

    QString warning = component.url().toString() + ":16: Unable to assign [undefined] to int";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toInt(), 10);
    QCOMPARE(object->property("test2").toInt(), 19);
    QCOMPARE(object->property("test3").toInt(), 0);
}

void tst_qmlecmascript::dynamicCreation_data()
{
    QTest::addColumn<QString>("method");
    QTest::addColumn<QString>("createdName");

    QTest::newRow("One") << "createOne" << "objectOne";
    QTest::newRow("Two") << "createTwo" << "objectTwo";
    QTest::newRow("Three") << "createThree" << "objectThree";
}

/*
Test using createQmlObject to dynamically generate an item
Also using createComponent is tested.
*/
void tst_qmlecmascript::dynamicCreation()
{
    QFETCH(QString, method);
    QFETCH(QString, createdName);

    QmlComponent component(&engine, TEST_FILE("dynamicCreation.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);

    QMetaObject::invokeMethod(object, method.toUtf8());
    QObject *created = object->objectProperty();
    QVERIFY(created);
    QCOMPARE(created->objectName(), createdName);
}

/*
   Tests the destroy function
*/
void tst_qmlecmascript::dynamicDestruction()
{
    QmlComponent component(&engine, TEST_FILE("dynamicDeletion.qml"));
    QGuard<MyQmlObject> object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QGuard<QObject> createdQmlObject = 0;

    QMetaObject::invokeMethod(object, "create");
    createdQmlObject = object->objectProperty();
    QVERIFY(createdQmlObject);
    QCOMPARE(createdQmlObject->objectName(), QString("emptyObject"));

    QMetaObject::invokeMethod(object, "killOther");
    QVERIFY(createdQmlObject);
    QTest::qWait(0);
    QCoreApplication::instance()->processEvents(QEventLoop::DeferredDeletion);
    QVERIFY(createdQmlObject);
    QTest::qWait(100);
    QCoreApplication::instance()->processEvents(QEventLoop::DeferredDeletion);
    QVERIFY(!createdQmlObject);

    QMetaObject::invokeMethod(object, "killMe");
    QVERIFY(object);
    QTest::qWait(0);
    QCoreApplication::instance()->processEvents(QEventLoop::DeferredDeletion);
    QVERIFY(!object);
}

/*
   tests that id.toString() works
*/
void tst_qmlecmascript::objectToString()
{
    QmlComponent component(&engine, TEST_FILE("qmlToString.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QMetaObject::invokeMethod(object, "testToString");
    QVERIFY(object->stringProperty().startsWith("MyQmlObject_QML_"));
    QVERIFY(object->stringProperty().endsWith(", \"objName\")"));
}

/*
Tests bindings that indirectly cause their own deletion work.

This test is best run under valgrind to ensure no invalid memory access occur.
*/
void tst_qmlecmascript::selfDeletingBinding()
{
    {
        QmlComponent component(&engine, TEST_FILE("selfDeletingBinding.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);
        object->setProperty("triggerDelete", true);
    }

    {
        QmlComponent component(&engine, TEST_FILE("selfDeletingBinding.2.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);
        object->setProperty("triggerDelete", true);
    }
}

/*
Test that extended object properties can be accessed.

This test a regression where this used to crash.  The issue was specificially
for extended objects that did not include a synthesized meta object (so non-root
and no synthesiszed properties).
*/
void tst_qmlecmascript::extendedObjectPropertyLookup()
{
    QmlComponent component(&engine, TEST_FILE("extendedObjectPropertyLookup.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);
}

/*
Test file/lineNumbers for binding/Script errors.
*/
void tst_qmlecmascript::scriptErrors()
{
    QmlComponent component(&engine, TEST_FILE("scriptErrors.qml"));
    QString url = component.url().toString();

    QString warning1 = url.left(url.length() - 3) + "js:2: Error: Invalid write to global property \"a\"";
    QString warning2 = url + ":7: TypeError: Result of expression 'a' [undefined] is not an object.";
    QString warning3 = url + ":5: Error: Invalid write to global property \"a\"";
    QString warning4 = url + ":12: TypeError: Result of expression 'a' [undefined] is not an object.";
    QString warning5 = url + ":10: TypeError: Result of expression 'a' [undefined] is not an object.";
    QString warning6 = url + ":9: Unable to assign [undefined] to int";
    QString warning7 = url + ":14: Error: Cannot assign to read-only property \"trueProperty\"";
    QString warning8 = url + ":15: Error: Cannot assign to non-existant property \"fakeProperty\"";

    QTest::ignoreMessage(QtWarningMsg, warning1.toLatin1().constData());
    QTest::ignoreMessage(QtWarningMsg, warning2.toLatin1().constData());
    QTest::ignoreMessage(QtWarningMsg, warning3.toLatin1().constData());
    QTest::ignoreMessage(QtWarningMsg, warning5.toLatin1().constData());
    QTest::ignoreMessage(QtWarningMsg, warning6.toLatin1().constData());
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);

    QTest::ignoreMessage(QtWarningMsg, warning4.toLatin1().constData());
    emit object->basicSignal();

    QTest::ignoreMessage(QtWarningMsg, warning7.toLatin1().constData());
    emit object->anotherBasicSignal();

    QTest::ignoreMessage(QtWarningMsg, warning8.toLatin1().constData());
    emit object->thirdBasicSignal();
}

/*
Test bindings still work when the reeval is triggered from within
a signal script.
*/
void tst_qmlecmascript::signalTriggeredBindings()
{
    QmlComponent component(&engine, TEST_FILE("signalTriggeredBindings.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("base").toReal(), 50.);
    QCOMPARE(object->property("test1").toReal(), 50.);
    QCOMPARE(object->property("test2").toReal(), 50.);

    object->basicSignal();

    QCOMPARE(object->property("base").toReal(), 200.);
    QCOMPARE(object->property("test1").toReal(), 200.);
    QCOMPARE(object->property("test2").toReal(), 200.);

    object->argumentSignal(10, QString(), 10);

    QCOMPARE(object->property("base").toReal(), 400.);
    QCOMPARE(object->property("test1").toReal(), 400.);
    QCOMPARE(object->property("test2").toReal(), 400.);
}

/*
Test that list properties can be iterated from ECMAScript
*/
void tst_qmlecmascript::listProperties()
{
    QmlComponent component(&engine, TEST_FILE("listProperties.qml"));
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toInt(), 21);
    QCOMPARE(object->property("test2").toInt(), 2);
    QCOMPARE(object->property("test3").toInt(), 50);
    QCOMPARE(object->property("test4").toInt(), 3);
    QCOMPARE(object->property("test5").toBool(), true);
    QCOMPARE(object->property("test6").toBool(), true);
    QCOMPARE(object->property("test7").toBool(), true);
    QCOMPARE(object->property("test8").toBool(), true);
}

void tst_qmlecmascript::exceptionClearsOnReeval()
{
    QmlComponent component(&engine, TEST_FILE("exceptionClearsOnReeval.qml"));
    QString url = component.url().toString();

    QString warning = url + ":4: TypeError: Result of expression 'objectProperty.objectProperty' [undefined] is not an object.";

    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1().constData());
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), false);

    MyQmlObject object2;
    MyQmlObject object3;
    object2.setObjectProperty(&object3);
    object->setObjectProperty(&object2);

    QCOMPARE(object->property("test").toBool(), true);
}

void tst_qmlecmascript::exceptionSlotProducesWarning()
{
    QmlComponent component(&engine, TEST_FILE("exceptionProducesWarning.qml"));
    QString url = component.url().toString();

    QString warning = component.url().toString() + ":6: Error: JS exception";

    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1().constData());
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
}

void tst_qmlecmascript::exceptionBindingProducesWarning()
{
    QmlComponent component(&engine, TEST_FILE("exceptionProducesWarning2.qml"));
    QString url = component.url().toString();

    QString warning = component.url().toString() + ":5: Error: JS exception";

    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1().constData());
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
}

static int transientErrorsMsgCount = 0;
static void transientErrorsMsgHandler(QtMsgType, const char *)
{
    ++transientErrorsMsgCount;
}

// Check that transient binding errors are not displayed
void tst_qmlecmascript::transientErrors()
{
    QmlComponent component(&engine, TEST_FILE("transientErrors.qml"));

    transientErrorsMsgCount = 0;
    QtMsgHandler old = qInstallMsgHandler(transientErrorsMsgHandler);

    QObject *object = component.create();
    QVERIFY(object != 0);

    qInstallMsgHandler(old);

    QCOMPARE(transientErrorsMsgCount, 0);
}

// Check that errors during shutdown are minimized
void tst_qmlecmascript::shutdownErrors()
{
    QmlComponent component(&engine, TEST_FILE("shutdownErrors.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    transientErrorsMsgCount = 0;
    QtMsgHandler old = qInstallMsgHandler(transientErrorsMsgHandler);

    delete object;

    qInstallMsgHandler(old);
    QCOMPARE(transientErrorsMsgCount, 0);
}

// Check that Script::source property works as expected
void tst_qmlecmascript::externalScript()
{
    {
        QmlComponent component(&engine, TEST_FILE("externalScript.1.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test").toInt(), 92);

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("externalScript.2.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test").toInt(), 92);

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("externalScript.3.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test").toInt(), 92);
        QCOMPARE(object->property("test2").toInt(), 92);
        QCOMPARE(object->property("test3").toBool(), false);

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("externalScript.4.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("test").toInt(), 92);
        QCOMPARE(object->property("test2").toBool(), true);

        delete object;
    }
}

void tst_qmlecmascript::compositePropertyType()
{
    QmlComponent component(&engine, TEST_FILE("compositePropertyType.qml"));
    QTest::ignoreMessage(QtDebugMsg, "hello world");
    QObject *object = qobject_cast<QObject *>(component.create());
    delete object;
}

// QTBUG-5759
void tst_qmlecmascript::jsObject()
{
    QmlComponent component(&engine, TEST_FILE("jsObject.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toInt(), 92);

    delete object;
}

void tst_qmlecmascript::undefinedResetsProperty()
{
    {
    QmlComponent component(&engine, TEST_FILE("undefinedResetsProperty.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("resettableProperty").toInt(), 92);

    object->setProperty("setUndefined", true);

    QCOMPARE(object->property("resettableProperty").toInt(), 13);

    object->setProperty("setUndefined", false);

    QCOMPARE(object->property("resettableProperty").toInt(), 92);

    delete object;
    }
    {
    QmlComponent component(&engine, TEST_FILE("undefinedResetsProperty.2.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("resettableProperty").toInt(), 19);

    QMetaObject::invokeMethod(object, "doReset");

    QCOMPARE(object->property("resettableProperty").toInt(), 13);

    delete object;
    }
}

// QTBUG-6781
void tst_qmlecmascript::bug1()
{
    QmlComponent component(&engine, TEST_FILE("bug.1.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toInt(), 14);

    object->setProperty("a", 11);

    QCOMPARE(object->property("test").toInt(), 3);

    object->setProperty("b", true);

    QCOMPARE(object->property("test").toInt(), 9);

    delete object;
}

void tst_qmlecmascript::callQtInvokables()
{
    MyInvokableObject o;

    QmlEngine qmlengine;
    QmlEnginePrivate *ep = QmlEnginePrivate::get(&qmlengine);
    QScriptEngine *engine = &ep->scriptEngine;
    ep->globalClass->explicitSetProperty("object", ep->objectClass->newQObject(&o));

    // Non-existant methods
    o.reset();
    QCOMPARE(engine->evaluate("object.method_nonexistant()").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QCOMPARE(engine->evaluate("object.method_nonexistant(10, 11)").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    // Insufficient arguments
    o.reset();
    QCOMPARE(engine->evaluate("object.method_int()").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intint(10)").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    // Excessive arguments
    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(10, 11)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(10));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intint(10, 11, 12)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 9);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(10));
    QCOMPARE(o.actuals().at(1), QVariant(11));

    // Test return types
    o.reset();
    QCOMPARE(engine->evaluate("object.method_NoArgs()").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 0);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QVERIFY(engine->evaluate("object.method_NoArgs_int()").strictlyEquals(QScriptValue(engine, 6)));
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 1);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QVERIFY(engine->evaluate("object.method_NoArgs_real()").strictlyEquals(QScriptValue(engine, 19.7)));
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 2);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    {
    QScriptValue ret = engine->evaluate("object.method_NoArgs_QPointF()");
    QVERIFY(ret.isVariant());
    QCOMPARE(ret.toVariant(), QVariant(QPointF(123, 4.5)));
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 3);
    QCOMPARE(o.actuals().count(), 0);
    }

    o.reset();
    {
    QScriptValue ret = engine->evaluate("object.method_NoArgs_QObject()");
    QVERIFY(ret.isQObject());
    QCOMPARE(ret.toQObject(), (QObject *)&o);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 4);
    QCOMPARE(o.actuals().count(), 0);
    }

    o.reset();
    QCOMPARE(engine->evaluate("object.method_NoArgs_unknown()").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 5);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    {
    QScriptValue ret = engine->evaluate("object.method_NoArgs_QScriptValue()");
    QVERIFY(ret.isString());
    QCOMPARE(ret.toString(), QString("Hello world"));
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 6);
    QCOMPARE(o.actuals().count(), 0);
    }

    o.reset();
    QVERIFY(engine->evaluate("object.method_NoArgs_QVariant()").strictlyEquals(QScriptValue(engine, "QML rocks")));
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 7);
    QCOMPARE(o.actuals().count(), 0);

    // Test arg types
    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(94)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(94));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(\"94\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(94));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(\"not a number\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_int(object)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 8);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intint(122, 9)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 9);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(122));
    QCOMPARE(o.actuals().at(1), QVariant(9));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(94.3)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(94.3));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(\"94.3\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(94.3));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(\"not a number\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qIsNaN(o.actuals().at(0).toDouble()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qIsNaN(o.actuals().at(0).toDouble()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_real(object)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 10);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qIsNaN(o.actuals().at(0).toDouble()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QString(\"Hello world\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 11);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant("Hello world"));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QString(19)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 11);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant("19"));

    o.reset();
    {
    QString expected = "MyInvokableObject(0x" + QString::number((intptr_t)&o, 16) + ")";
    QCOMPARE(engine->evaluate("object.method_QString(object)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 11);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(expected));
    }

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QString(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 11);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QString()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QString(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 11);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QString()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(0)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(object)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF()));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(object.method_get_QPointF())").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF(99.3, -10.2)));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QPointF(object.method_get_QPoint())").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 12);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), QVariant(QPointF(9, 12)));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QObject(0)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 13);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), qVariantFromValue((QObject *)0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QObject(\"Hello world\")").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 13);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), qVariantFromValue((QObject *)0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QObject(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 13);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), qVariantFromValue((QObject *)0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QObject(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 13);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), qVariantFromValue((QObject *)0));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QObject(object)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 13);
    QCOMPARE(o.actuals().count(), 1);
    QCOMPARE(o.actuals().at(0), qVariantFromValue((QObject *)&o));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QScriptValue(null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 14);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(0)).isNull());

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QScriptValue(undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 14);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(0)).isUndefined());

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QScriptValue(19)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 14);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(0)).strictlyEquals(QScriptValue(engine, 19)));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_QScriptValue([19, 20])").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 14);
    QCOMPARE(o.actuals().count(), 1);
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(0)).isArray());

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intQScriptValue(4, null)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 15);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(4));
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(1)).isNull());

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intQScriptValue(8, undefined)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 15);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(8));
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(1)).isUndefined());

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intQScriptValue(3, 19)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 15);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(3));
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(1)).strictlyEquals(QScriptValue(engine, 19)));

    o.reset();
    QCOMPARE(engine->evaluate("object.method_intQScriptValue(44, [19, 20])").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 15);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(44));
    QVERIFY(qvariant_cast<QScriptValue>(o.actuals().at(1)).isArray());

    // Test overloads - QML will always invoke the *last* method
    o.reset();
    QCOMPARE(engine->evaluate("object.method_overload()").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QCOMPARE(engine->evaluate("object.method_overload(10)").isError(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), -1);
    QCOMPARE(o.actuals().count(), 0);

    o.reset();
    QCOMPARE(engine->evaluate("object.method_overload(10, 11)").isUndefined(), true);
    QCOMPARE(o.error(), false);
    QCOMPARE(o.invoked(), 17);
    QCOMPARE(o.actuals().count(), 2);
    QCOMPARE(o.actuals().at(0), QVariant(10));
    QCOMPARE(o.actuals().at(1), QVariant(11));
}

QTEST_MAIN(tst_qmlecmascript)

#include "tst_qmlecmascript.moc"
