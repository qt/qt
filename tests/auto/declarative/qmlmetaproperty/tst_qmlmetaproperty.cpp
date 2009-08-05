#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlmetaproperty.h>
#include <QtGui/QLineEdit>

class MyQmlObject : public QObject
{
    Q_OBJECT
public:
    MyQmlObject() {}
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(Test,1,0,0,MyQmlObject,MyQmlObject);

class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<MyQmlObject*>* children READ children)
    Q_PROPERTY(QmlList<MyQmlObject*>* qmlChildren READ qmlChildren)
public:
    MyContainer() {}

    QList<MyQmlObject*> *children() { return &m_children; }
    QmlConcreteList<MyQmlObject *> *qmlChildren() { return &m_qmlChildren; }

private:
    QList<MyQmlObject*> m_children;
    QmlConcreteList<MyQmlObject *> m_qmlChildren;
};

QML_DECLARE_TYPE(MyContainer);
QML_DEFINE_TYPE(Test,1,0,0,MyContainer,MyContainer);

class tst_QmlMetaProperty : public QObject
{
    Q_OBJECT
public:
    tst_QmlMetaProperty() {}

private slots:
    void writeObjectToList();
    void writeListToList();
    void writeObjectToQmlList();

    //writeToReadOnly();

private:
    QmlEngine engine;
};

void tst_QmlMetaProperty::writeObjectToList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
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
void tst_QmlMetaProperty::writeListToList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
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

void tst_QmlMetaProperty::writeObjectToQmlList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { qmlChildren: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmlChildren()->size() == 1);

    MyQmlObject *object = new MyQmlObject;
    QmlMetaProperty prop(container, "qmlChildren");
    prop.write(qVariantFromValue(object));
    QCOMPARE(container->qmlChildren()->size(), 2);
    QCOMPARE(container->qmlChildren()->at(1), object);
}

QTEST_MAIN(tst_QmlMetaProperty)

#include "tst_qmlmetaproperty.moc"
