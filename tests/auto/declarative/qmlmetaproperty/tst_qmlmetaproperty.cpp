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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlmetaproperty.h>
#include <private/qguard_p.h>
#include <QtDeclarative/qmlbinding.h>
#include <QtGui/QLineEdit>

class MyQmlObject : public QObject
{
    Q_OBJECT
public:
    MyQmlObject() {}
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(Test,1,0,MyQmlObject,MyQmlObject);

class MyAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int foo READ foo WRITE setFoo)
public:
    MyAttached(QObject *parent) : QObject(parent), m_foo(13) {}

    int foo() const { return m_foo; }
    void setFoo(int f) { m_foo = f; }

private:
    int m_foo;
};

class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<MyQmlObject*>* children READ children)
    Q_PROPERTY(QmlList<MyQmlObject*>* qmlChildren READ qmlChildren)
public:
    MyContainer() {}

    QList<MyQmlObject*> *children() { return &m_children; }
    QmlConcreteList<MyQmlObject *> *qmlChildren() { return &m_qmlChildren; }

    static MyAttached *qmlAttachedProperties(QObject *o) {
        return new MyAttached(o);
    }

private:
    QList<MyQmlObject*> m_children;
    QmlConcreteList<MyQmlObject *> m_qmlChildren;
};

QML_DECLARE_TYPE(MyContainer);
QML_DEFINE_TYPE(Test,1,0,MyContainer,MyContainer);
QML_DECLARE_TYPEINFO(MyContainer, QML_HAS_ATTACHED_PROPERTIES)

class tst_qmlmetaproperty : public QObject
{
    Q_OBJECT
public:
    tst_qmlmetaproperty() {}

private slots:

    // Constructors
    void qmlmetaproperty();
    void qmlmetaproperty_object();
    void qmlmetaproperty_object_string();
    void qmlmetaproperty_object_context();
    void qmlmetaproperty_object_string_context();

    // Methods
    void name();
    void read();
    void write();

    // Functionality
    void writeObjectToList();
    void writeListToList();
    void writeObjectToQmlList();

    //writeToReadOnly();

private:
    QmlEngine engine;
};

void tst_qmlmetaproperty::qmlmetaproperty()
{
    QmlMetaProperty prop;

    QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
    QVERIFY(binding != 0);
    QGuard<QmlExpression> expression(new QmlExpression());
    QVERIFY(expression != 0);

    QObject *obj = new QObject;

    QCOMPARE(prop.name(), QString());
    QCOMPARE(prop.read(), QVariant());
    QCOMPARE(prop.write(QVariant()), false);
    QCOMPARE(prop.hasChangedNotifier(), false);
    QCOMPARE(prop.needsChangedNotifier(), false);
    QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
    QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
    QCOMPARE(prop.connectNotifier(obj, 0), false);
    QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
    QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
    QCOMPARE(prop.connectNotifier(obj, -1), false);
    QVERIFY(prop.method().signature() == 0);
    QCOMPARE(prop.type(), QmlMetaProperty::Invalid);
    QCOMPARE(prop.isProperty(), false);
    QCOMPARE(prop.isDefault(), false);
    QCOMPARE(prop.isWritable(), false);
    QCOMPARE(prop.isDesignable(), false);
    QCOMPARE(prop.isValid(), false);
    QCOMPARE(prop.object(), (QObject *)0);
    QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
    QCOMPARE(prop.propertyType(), 0);
    QCOMPARE(prop.propertyTypeName(), (const char *)0);
    QVERIFY(prop.property().name() == 0);
    QVERIFY(prop.binding() == 0);
    QVERIFY(prop.setBinding(binding) == 0);
    QVERIFY(binding == 0);
    QVERIFY(prop.signalExpression() == 0);
    QVERIFY(prop.setSignalExpression(expression) == 0);
    QVERIFY(expression == 0);
    QCOMPARE(prop.coreIndex(), -1);
    QCOMPARE(prop.valueTypeCoreIndex(), -1);

    delete obj;
}

class PropertyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int defaultProperty READ defaultProperty);
    Q_PROPERTY(QRect rectProperty READ rectProperty);
    Q_PROPERTY(QRect wrectProperty READ wrectProperty WRITE setWRectProperty);
    Q_PROPERTY(QUrl url READ url WRITE setUrl);

    Q_CLASSINFO("DefaultProperty", "defaultProperty");
public:
    int defaultProperty() { return 10; }
    QRect rectProperty() { return QRect(10, 10, 1, 209); }

    QRect wrectProperty() { return m_rect; }
    void setWRectProperty(const QRect &r) { m_rect = r; }

    QUrl url() { return m_url; }
    void setUrl(const QUrl &u) { m_url = u; }

signals:
    void clicked();

private:
    QRect m_rect;
    QUrl m_url;
};

void tst_qmlmetaproperty::qmlmetaproperty_object()
{
    QObject object; // Has no default property
    PropertyObject dobject; // Has default property

    {
        QmlMetaProperty prop(&object);

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString());
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Invalid);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), false);
        QCOMPARE(prop.object(), (QObject *)0);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QVERIFY(prop.property().name() == 0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), -1);
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject);

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("defaultProperty"));
        QCOMPARE(prop.read(), QVariant(10));
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), true);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), (QmlMetaProperty::Type)(QmlMetaProperty::Property | QmlMetaProperty::Default));
        QCOMPARE(prop.isProperty(), true);
        QCOMPARE(prop.isDefault(), true);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), true);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::Normal);
        QCOMPARE(prop.propertyType(), (int)QVariant::Int);
        QCOMPARE(prop.propertyTypeName(), "int");
        QCOMPARE(QString(prop.property().name()), QString("defaultProperty"));
        QVERIFY(prop.binding() == 0);
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>:-1: Unable to assign null to int");
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding != 0);
        QVERIFY(prop.binding() == binding);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfProperty("defaultProperty"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }
}

void tst_qmlmetaproperty::qmlmetaproperty_object_string()
{
    QObject object; 
    PropertyObject dobject; 

    {
        QmlMetaProperty prop(&object, QString("defaultProperty"));

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString());
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Invalid);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), false);
        QCOMPARE(prop.object(), (QObject *)0);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QVERIFY(prop.property().name() == 0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), -1);
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject, QString("defaultProperty"));

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("defaultProperty"));
        QCOMPARE(prop.read(), QVariant(10));
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), true);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Property);
        QCOMPARE(prop.isProperty(), true);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), true);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::Normal);
        QCOMPARE(prop.propertyType(), (int)QVariant::Int);
        QCOMPARE(prop.propertyTypeName(), "int");
        QCOMPARE(QString(prop.property().name()), QString("defaultProperty"));
        QVERIFY(prop.binding() == 0);
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>:-1: Unable to assign null to int");
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding != 0);
        QVERIFY(prop.binding() == binding);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfProperty("defaultProperty"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject, QString("onClicked"));

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("onClicked"));
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant("Hello")), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QCOMPARE(QString(prop.method().signature()), QString("clicked()"));
        QCOMPARE(prop.type(), QmlMetaProperty::SignalProperty);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QCOMPARE(prop.property().name(), (const char *)0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression != 0);
        QVERIFY(prop.signalExpression() == expression);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfMethod("clicked()"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }
}

void tst_qmlmetaproperty::qmlmetaproperty_object_context()
{
    QObject object; // Has no default property
    PropertyObject dobject; // Has default property

    {
        QmlMetaProperty prop(&object, engine.rootContext());

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString());
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Invalid);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), false);
        QCOMPARE(prop.object(), (QObject *)0);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QVERIFY(prop.property().name() == 0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), -1);
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject, engine.rootContext());

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("defaultProperty"));
        QCOMPARE(prop.read(), QVariant(10));
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), true);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), (QmlMetaProperty::Type)(QmlMetaProperty::Property | QmlMetaProperty::Default));
        QCOMPARE(prop.isProperty(), true);
        QCOMPARE(prop.isDefault(), true);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), true);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::Normal);
        QCOMPARE(prop.propertyType(), (int)QVariant::Int);
        QCOMPARE(prop.propertyTypeName(), "int");
        QCOMPARE(QString(prop.property().name()), QString("defaultProperty"));
        QVERIFY(prop.binding() == 0);
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>:-1: Unable to assign null to int");
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding != 0);
        QVERIFY(prop.binding() == binding);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfProperty("defaultProperty"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }
}

void tst_qmlmetaproperty::qmlmetaproperty_object_string_context()
{
    QObject object; 
    PropertyObject dobject; 

    {
        QmlMetaProperty prop(&object, QString("defaultProperty"), engine.rootContext());

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString());
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Invalid);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), false);
        QCOMPARE(prop.object(), (QObject *)0);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QVERIFY(prop.property().name() == 0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), -1);
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject, QString("defaultProperty"), engine.rootContext());

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("defaultProperty"));
        QCOMPARE(prop.read(), QVariant(10));
        QCOMPARE(prop.write(QVariant()), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), true);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QVERIFY(prop.method().signature() == 0);
        QCOMPARE(prop.type(), QmlMetaProperty::Property);
        QCOMPARE(prop.isProperty(), true);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), true);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::Normal);
        QCOMPARE(prop.propertyType(), (int)QVariant::Int);
        QCOMPARE(prop.propertyTypeName(), "int");
        QCOMPARE(QString(prop.property().name()), QString("defaultProperty"));
        QVERIFY(prop.binding() == 0);
        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>:-1: Unable to assign null to int");
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding != 0);
        QVERIFY(prop.binding() == binding);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression == 0);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfProperty("defaultProperty"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }

    {
        QmlMetaProperty prop(&dobject, QString("onClicked"), engine.rootContext());

        QGuard<QmlBinding> binding(new QmlBinding(QString(), 0, 0));
        binding->setTarget(prop);
        QVERIFY(binding != 0);
        QGuard<QmlExpression> expression(new QmlExpression());
        QVERIFY(expression != 0);

        QObject *obj = new QObject;

        QCOMPARE(prop.name(), QString("onClicked"));
        QCOMPARE(prop.read(), QVariant());
        QCOMPARE(prop.write(QVariant("Hello")), false);
        QCOMPARE(prop.hasChangedNotifier(), false);
        QCOMPARE(prop.needsChangedNotifier(), false);
        QCOMPARE(prop.connectNotifier(0, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, SLOT(deleteLater())), false);
        QCOMPARE(prop.connectNotifier(obj, 0), false);
        QCOMPARE(prop.connectNotifier(0, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, obj->metaObject()->indexOfMethod("deleteLater()")), false);
        QCOMPARE(prop.connectNotifier(obj, -1), false);
        QCOMPARE(QString(prop.method().signature()), QString("clicked()"));
        QCOMPARE(prop.type(), QmlMetaProperty::SignalProperty);
        QCOMPARE(prop.isProperty(), false);
        QCOMPARE(prop.isDefault(), false);
        QCOMPARE(prop.isWritable(), false);
        QCOMPARE(prop.isDesignable(), false);
        QCOMPARE(prop.isValid(), true);
        QCOMPARE(prop.object(), &dobject);
        QCOMPARE(prop.propertyCategory(), QmlMetaProperty::InvalidProperty);
        QCOMPARE(prop.propertyType(), 0);
        QCOMPARE(prop.propertyTypeName(), (const char *)0);
        QCOMPARE(prop.property().name(), (const char *)0);
        QVERIFY(prop.binding() == 0);
        QVERIFY(prop.setBinding(binding) == 0);
        QVERIFY(binding == 0);
        QVERIFY(prop.signalExpression() == 0);
        QVERIFY(prop.setSignalExpression(expression) == 0);
        QVERIFY(expression != 0);
        QVERIFY(prop.signalExpression() == expression);
        QCOMPARE(prop.coreIndex(), dobject.metaObject()->indexOfMethod("clicked()"));
        QCOMPARE(prop.valueTypeCoreIndex(), -1);

        delete obj;
    }
}

void tst_qmlmetaproperty::name()
{
    { 
        QmlMetaProperty p;
        QCOMPARE(p.name(), QString());
    }

    {
        PropertyObject o;
        QmlMetaProperty p(&o);
        QCOMPARE(p.name(), QString("defaultProperty"));
    }

    {
        QObject o;
        QmlMetaProperty p(&o, QString("objectName"));
        QCOMPARE(p.name(), QString("objectName"));
    }

    {
        PropertyObject o;
        QmlMetaProperty p(&o, "onClicked");
        QCOMPARE(p.name(), QString("onClicked"));
    }

    {
        QObject o;
        QmlMetaProperty p(&o, "onClicked");
        QCOMPARE(p.name(), QString());
    }

    {
        QObject o;
        QmlMetaProperty p(&o, "foo");
        QCOMPARE(p.name(), QString());
    }

    {
        QmlMetaProperty p(0, "foo");
        QCOMPARE(p.name(), QString());
    }

    {
        PropertyObject o;
        QmlMetaProperty p = QmlMetaProperty::createProperty(&o, "rectProperty");
        QCOMPARE(p.name(), QString("rectProperty"));
    }

    {
        PropertyObject o;
        QmlMetaProperty p = QmlMetaProperty::createProperty(&o, "rectProperty.x");
        QCOMPARE(p.name(), QString("rectProperty.x"));
    }

    {
        PropertyObject o;
        QmlMetaProperty p = QmlMetaProperty::createProperty(&o, "rectProperty.foo");
        QCOMPARE(p.name(), QString());
    }
}

void tst_qmlmetaproperty::read()
{
    // Invalid 
    {
        QmlMetaProperty p;
        QCOMPARE(p.read(), QVariant());
    }

    // Default prop
    {
        PropertyObject o;
        QmlMetaProperty p(&o);
        QCOMPARE(p.read(), QVariant(10));
    }

    // Invalid default prop
    {
        QObject o;
        QmlMetaProperty p(&o);
        QCOMPARE(p.read(), QVariant());
    }

    // Value prop by name
    {
        QObject o;

        QmlMetaProperty p(&o, "objectName");
        QCOMPARE(p.read(), QVariant(QString()));

        o.setObjectName("myName");

        QCOMPARE(p.read(), QVariant("myName"));
    }

    // Value-type prop
    {
        PropertyObject o;
        QmlMetaProperty p = QmlMetaProperty::createProperty(&o, "rectProperty.x");
        QCOMPARE(p.read(), QVariant(10));
    }

    // Invalid value-type prop
    {
        PropertyObject o;
        QmlMetaProperty p = QmlMetaProperty::createProperty(&o, "rectProperty.foo");
        QCOMPARE(p.read(), QVariant());
    }

    // Signal property
    {
        PropertyObject o;
        QmlMetaProperty p(&o, "onClicked");
        QCOMPARE(p.read(), QVariant());

        QVERIFY(0 == p.setSignalExpression(new QmlExpression()));
        QVERIFY(0 != p.signalExpression());

        QCOMPARE(p.read(), QVariant());
    }

    // Deleted object
    {
        PropertyObject *o = new PropertyObject;
        QmlMetaProperty p = QmlMetaProperty::createProperty(o, "rectProperty.x");
        QCOMPARE(p.read(), QVariant(10));
        delete o;
        QCOMPARE(p.read(), QVariant());
    }

    // Attached property
    {
        QmlComponent component(&engine);
        component.setData("import Test 1.0\nMyContainer { }", QUrl());
        QObject *object = component.create();
        QVERIFY(object != 0);

        QmlMetaProperty p = QmlMetaProperty::createProperty(object, "MyContainer.foo", qmlContext(object));
        QCOMPARE(p.read(), QVariant(13));
        delete object;
    }
    {
        QmlComponent component(&engine);
        component.setData("import Test 1.0\nMyContainer { MyContainer.foo: 10 }", QUrl());
        QObject *object = component.create();
        QVERIFY(object != 0);

        QmlMetaProperty p = QmlMetaProperty::createProperty(object, "MyContainer.foo", qmlContext(object));
        QCOMPARE(p.read(), QVariant(10));
        delete object;
    }
    {
        QmlComponent component(&engine);
        component.setData("import Test 1.0 as Foo\nFoo.MyContainer { Foo.MyContainer.foo: 10 }", QUrl());
        QObject *object = component.create();
        QVERIFY(object != 0);

        QmlMetaProperty p = QmlMetaProperty::createProperty(object, "Foo.MyContainer.foo", qmlContext(object));
        QCOMPARE(p.read(), QVariant(10));
        delete object;
    }
}

void tst_qmlmetaproperty::write()
{
    // Invalid
    {
        QmlMetaProperty p;
        QCOMPARE(p.write(QVariant(10)), false);
    }

    // Read-only default prop
    {
        PropertyObject o;
        QmlMetaProperty p(&o);
        QCOMPARE(p.write(QVariant(10)), false);
    }

    // Invalid default prop
    {
        QObject o;
        QmlMetaProperty p(&o);
        QCOMPARE(p.write(QVariant(10)), false);
    }

    // Read-only prop by name
    {
        PropertyObject o;
        QmlMetaProperty p(&o, QString("defaultProperty"));
        QCOMPARE(p.write(QVariant(10)), false);
    }

    // Writable prop by name
    {
        PropertyObject o;
        QmlMetaProperty p(&o, QString("objectName"));
        QCOMPARE(o.objectName(), QString());
        QCOMPARE(p.write(QVariant(QString("myName"))), true);
        QCOMPARE(o.objectName(), QString("myName"));
    }

    // Deleted object
    {
        PropertyObject *o = new PropertyObject;
        QmlMetaProperty p(o, QString("objectName"));
        QCOMPARE(p.write(QVariant(QString("myName"))), true);
        QCOMPARE(o->objectName(), QString("myName"));

        delete o;

        QCOMPARE(p.write(QVariant(QString("myName"))), false);
    }

    // Signal property
    {
        PropertyObject o;
        QmlMetaProperty p(&o, "onClicked");
        QCOMPARE(p.write(QVariant("console.log(1921)")), false);

        QVERIFY(0 == p.setSignalExpression(new QmlExpression()));
        QVERIFY(0 != p.signalExpression());

        QCOMPARE(p.write(QVariant("console.log(1921)")), false);

        QVERIFY(0 != p.signalExpression());
    }

    // Value-type property
    {
        PropertyObject o;
        QmlMetaProperty p(&o, "wrectProperty");

        QCOMPARE(o.wrectProperty(), QRect());
        QCOMPARE(p.write(QRect(1, 13, 99, 8)), true);
        QCOMPARE(o.wrectProperty(), QRect(1, 13, 99, 8));

        QmlMetaProperty p2 = QmlMetaProperty::createProperty(&o, "wrectProperty.x");
        QCOMPARE(p2.read(), QVariant(1));
        QCOMPARE(p2.write(QVariant(6)), true);
        QCOMPARE(p2.read(), QVariant(6));
        QCOMPARE(o.wrectProperty(), QRect(6, 13, 99, 8));
    }

    // URL-property
    {
        PropertyObject o;
        QmlMetaProperty p(&o, "url");

        QCOMPARE(p.write(QUrl("main.qml")), true);
        QCOMPARE(o.url(), QUrl("main.qml"));

        QmlMetaProperty p2(&o, "url", engine.rootContext());

        QUrl result = engine.baseUrl().resolved(QUrl("main.qml"));
        QVERIFY(result != QUrl("main.qml"));

        QCOMPARE(p2.write(QUrl("main.qml")), true);
        QCOMPARE(o.url(), result);
    }

    // Attached property
    {
        QmlComponent component(&engine);
        component.setData("import Test 1.0\nMyContainer { }", QUrl());
        QObject *object = component.create();
        QVERIFY(object != 0);

        QmlMetaProperty p = QmlMetaProperty::createProperty(object, "MyContainer.foo", qmlContext(object));
        p.write(QVariant(99));
        QCOMPARE(p.read(), QVariant(99));
        delete object;
    }
    {
        QmlComponent component(&engine);
        component.setData("import Test 1.0 as Foo\nFoo.MyContainer { Foo.MyContainer.foo: 10 }", QUrl());
        QObject *object = component.create();
        QVERIFY(object != 0);

        QmlMetaProperty p = QmlMetaProperty::createProperty(object, "Foo.MyContainer.foo", qmlContext(object));
        p.write(QVariant(99));
        QCOMPARE(p.read(), QVariant(99));
        delete object;
    }
}

void tst_qmlmetaproperty::writeObjectToList()
{
    QmlComponent containerComponent(&engine);
    containerComponent.setData("import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->children()->size() == 1);

    MyQmlObject *object = new MyQmlObject;
    QmlMetaProperty prop(container, "children");
    prop.write(qVariantFromValue(object));
    QCOMPARE(container->children()->size(), 2);
    QCOMPARE(container->children()->at(1), object);
}

Q_DECLARE_METATYPE(QList<QObject *>);
void tst_qmlmetaproperty::writeListToList()
{
    QmlComponent containerComponent(&engine);
    containerComponent.setData("import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->children()->size() == 1);

    QList<QObject*> objList;
    objList << new MyQmlObject() << new MyQmlObject() << new MyQmlObject() << new MyQmlObject();
    QmlMetaProperty prop(container, "children");
    prop.write(qVariantFromValue(objList));
    QCOMPARE(container->children()->size(), 4);

    //XXX need to try this with read/write prop (for read-only it correctly doesn't write)
    /*QList<MyQmlObject*> typedObjList;
    typedObjList << new MyQmlObject();
    prop.write(qVariantFromValue(&typedObjList));
    QCOMPARE(container->children()->size(), 1);*/
}

void tst_qmlmetaproperty::writeObjectToQmlList()
{
    QmlComponent containerComponent(&engine);
    containerComponent.setData("import Test 1.0\nMyContainer { qmlChildren: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmlChildren()->size() == 1);

    MyQmlObject *object = new MyQmlObject;
    QmlMetaProperty prop(container, "qmlChildren");
    prop.write(qVariantFromValue(object));
    QCOMPARE(container->qmlChildren()->size(), 2);
    QCOMPARE(container->qmlChildren()->at(1), object);
}

QTEST_MAIN(tst_qmlmetaproperty)

#include "tst_qmlmetaproperty.moc"
