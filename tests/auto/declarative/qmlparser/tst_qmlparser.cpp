#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>

class MyInterface 
{
public:
    MyInterface() : id(913) {}
    int id;
};

Q_DECLARE_INTERFACE(MyInterface, "com.trolltech.Qt.Test.MyInterface");
QML_DECLARE_INTERFACE(MyInterface);
QML_DEFINE_INTERFACE(MyInterface);

class MyQmlObject : public QObject, public MyInterface
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QString readOnlyString READ readOnlyString)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QMatrix matrix READ matrix WRITE setMatrix)  //assumed to be unsupported by QML
    Q_PROPERTY(MyInterface *interface READ interface WRITE setInterface)
    Q_INTERFACES(MyInterface)
public:
    MyQmlObject() : m_value(-1), m_interface(0) {}

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

    QString readOnlyString() const { return QLatin1String(""); }

    bool enabled() const { return false; }
    void setEnabled(bool) {}

    QRect rect() const { return QRect(); }
    void setRect(const QRect&) {}

    QMatrix matrix() const { return QMatrix(); }
    void setMatrix(const QMatrix&) {}

    MyInterface *interface() const { return m_interface; }
    void setInterface(MyInterface *iface) { m_interface = iface; }

    Q_CLASSINFO("defaultMethod", "basicSlot()");

public slots:
    void basicSlot() { qWarning("MyQmlObject::basicSlot"); }

signals:
    void basicSignal();

private:
    friend class tst_qmlparser;
    int m_value;
    MyInterface *m_interface;
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(MyQmlObject,MyQmlObject);

class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QObject*>* children READ children)
    Q_PROPERTY(QList<MyInterface*>* qlistInterfaces READ qlistInterfaces)
    Q_PROPERTY(QmlList<MyInterface*>* qmllistInterfaces READ qmllistInterfaces)
    Q_CLASSINFO("DefaultProperty", "children");
public:
    MyContainer() {}

    QList<QObject*> *children() { return &m_children; }
    QList<MyInterface *> *qlistInterfaces() { return &m_interfaces; }
    QmlList<MyInterface *> *qmllistInterfaces() { return &m_qmlinterfaces; }
    const QmlConcreteList<MyInterface *> &qmllistAccessor() const { return m_qmlinterfaces; }

private:
    QList<QObject*> m_children;
    QList<MyInterface *> m_interfaces;
    QmlConcreteList<MyInterface *> m_qmlinterfaces;
};

QML_DECLARE_TYPE(MyContainer);
QML_DEFINE_TYPE(MyContainer,MyContainer);

class MyDotPropertyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MyQmlObject *obj READ obj)
    Q_PROPERTY(MyQmlObject *readWriteObj READ readWriteObj WRITE setReadWriteObj)
public:
    MyDotPropertyObject() : m_rwobj(0), m_ownRWObj(false) {}
    ~MyDotPropertyObject()
    {
        if (m_ownRWObj)
            delete m_rwobj;
    }

    MyQmlObject *obj() { return 0; }

    MyQmlObject *readWriteObj()
    {
        if (!m_rwobj) {
            m_rwobj = new MyQmlObject;
            m_ownRWObj = true;
        }
        return m_rwobj;
    }

    void setReadWriteObj(MyQmlObject *obj)
    {
        if (m_ownRWObj) {
            delete m_rwobj;
            m_ownRWObj = false;
        }

        m_rwobj = obj;
    }

private:
    MyQmlObject *m_rwobj;
    bool m_ownRWObj;
};

QML_DECLARE_TYPE(MyDotPropertyObject);
QML_DEFINE_TYPE(MyDotPropertyObject,MyDotPropertyObject);

class tst_qmlparser : public QObject
{
    Q_OBJECT
public:
    tst_qmlparser() {}

private slots:
    void simpleObject();
    void simpleContainer();
    void unregisteredObject();
    void nonexistantProperty();
    void unsupportedProperty();
    //void setProperties();
    void wrongType();
    void readOnly();
    //void dotProperties();
    void nullDotProperty();
    void fakeDotProperty();
    //void readWriteDotProperty();
    void emptyInput();
    void missingObject();
    //void invalidXML();
    void duplicateIDs();
    void invalidID();
    void interfaceProperty();
    void interfaceQmlList();
    void interfaceQList();
    //void cannotAssignBindingToSignal();
    void assignObjectToSignal();

    // regression tests for crashes
    void crash1();

private:
    QmlEngine engine;
};

void tst_qmlparser::simpleObject()
{
    QmlComponent component(&engine, "MyQmlObject {}");
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_qmlparser::simpleContainer()
{
    QmlComponent component(&engine, "MyContainer {\nMyQmlObject{}\nMyQmlObject{}\n}");
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QCOMPARE(container->children()->count(),2);
}

void tst_qmlparser::unregisteredObject()
{
    QmlComponent component(&engine, "UnRegisteredObject {}", QUrl("myprogram.qml"));
    QTest::ignoreMessage(QtWarningMsg, "Unable to create object of type 'UnRegisteredObject' @myprogram.qml:1");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

void tst_qmlparser::nonexistantProperty()
{
    //NOTE: these first 3 should all have the same error message
    {
        QmlComponent component(&engine, "MyQmlObject { something: 24 }");
        QTest::ignoreMessage(QtWarningMsg, "Unknown property 'something' @<unspecified file>:1");
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    {
        QmlComponent component(&engine, "MyQmlObject {\n something: 24\n}");
        QTest::ignoreMessage(QtWarningMsg, "Unknown property 'something' @<unspecified file>:2");
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    //non-existant using binding
    {
        QmlComponent component(&engine, "MyQmlObject { something: 1 + 1 }");
        QTest::ignoreMessage(QtWarningMsg, "Unknown property 'something' @<unspecified file>:1");
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    {
        QmlComponent component(&engine, "MyQmlObject { something: }");
        QObject *object = component.create();
        QVERIFY(object != 0);
    }

    //non-existant value-type default property
    {
        QmlComponent component(&engine, "MyQmlObject {\n24\n}");
        QTest::ignoreMessage(QtWarningMsg, "Unable to resolve default property @<unspecified file>:3");
        // XXX would 2 be a better line number in this case? Message should also be improved.
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    //non-existant object-type default property
    {
        QmlComponent component(&engine, "MyQmlObject {\nMyQmlObject{}\n}");
        QTest::ignoreMessage(QtWarningMsg, "Unable to assign to non-existant property  @<unspecified file>:2");
        // XXX Message needs to be improved (and should be closer to value-type message).
        QObject *object = component.create();
        QVERIFY(object == 0);
    }
}

void tst_qmlparser::unsupportedProperty()
{
    QTest::ignoreMessage(QtWarningMsg, "Property 'matrix' is of an unknown type @<unspecified file>:1");
    QmlComponent component(&engine, "MyQmlObject { matrix: \"1,0,0,0,1,0,0,0,1\" }");
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object == 0);
}

void tst_qmlparser::wrongType()
{
    //string for int
    {
        QTest::ignoreMessage(QtWarningMsg, "Can't assign value 'hello' to property 'value' @<unspecified file>:1");
        QmlComponent component(&engine, "MyQmlObject { value: \"hello\" }");
        MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
        QVERIFY(object == 0);
    }

    //int for bool
    {
        QTest::ignoreMessage(QtWarningMsg, "Can't assign value '5' to property 'enabled' @<unspecified file>:1");
        QmlComponent component(&engine, "MyQmlObject { enabled: 5 }");
        MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
        QVERIFY(object == 0);
    }

    //bad format for rect
    {
        QTest::ignoreMessage(QtWarningMsg, "Can't assign value '5,5x10' to property 'rect' @<unspecified file>:1");
        QmlComponent component(&engine, "MyQmlObject { rect: \"5,5x10\" }");
        MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
        QVERIFY(object == 0);
    }

    //TODO: more types (including float-to-int, complex types, etc)
}

void tst_qmlparser::readOnly()
{
    {
        QTest::ignoreMessage(QtWarningMsg, "Can't assign value 'hello' to property 'readOnlyString' because 'readOnlyString' is read-only @<unspecified file>:1");
        QmlComponent component(&engine, "MyQmlObject { readOnlyString: \"hello\" }");
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    {
        QTest::ignoreMessage(QtWarningMsg, "Can't assign a binding to property 'readOnlyString' because 'readOnlyString' is read-only @<unspecified file>:1");
        QmlComponent component(&engine, "MyQmlObject { readOnlyString: {'hello'} }");
        QObject *object = component.create();
        QVERIFY(object == 0);
    }
}

void tst_qmlparser::nullDotProperty()
{
    QTest::ignoreMessage(QtWarningMsg, "Can't set properties on 'obj' because it is null @<unspecified file>:1");
    QmlComponent component(&engine, "MyDotPropertyObject { obj.value: 1 }");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

void tst_qmlparser::fakeDotProperty()
{
    QTest::ignoreMessage(QtWarningMsg, "Can't set properties on 'value' because it isn't a known object type @<unspecified file>:1");
    QmlComponent component(&engine, "MyQmlObject { value.something: \"hello\" }");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

//XXX need to define correct behavior first
/*void tst_qmlparser::readWriteDotProperty()
{
    QmlComponent component(&engine, "MyDotPropertyObject { readWriteObj.value: 1 }");
    MyDotPropertyObject *object = qobject_cast<MyDotPropertyObject*>(component.create());
    QVERIFY(object != 0);
    QCOMPARE(object->readWriteObj()->value(),1);

    {
        QmlComponent component(&engine, "MyContainer { MyQmlObject { id: Obj value: 1 } MyDotPropertyObject { readWriteObj: Obj } }");
        MyContainer *object = qobject_cast<MyContainer*>(component.create());
        QVERIFY(object != 0);
        MyDotPropertyObject *dpo = qobject_cast<MyDotPropertyObject *>(object->children()->at(1));
        QCOMPARE(dpo->readWriteObj()->value(),1);
    }
}*/

void tst_qmlparser::emptyInput()
{
    QTest::ignoreMessage(QtCriticalMsg, "No Qml was specified for parsing.");
    QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @<unspecified file>:-1");
    QmlComponent component(&engine, "");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

void tst_qmlparser::missingObject()
{
    QTest::ignoreMessage(QtCriticalMsg, "Can't have a property with no object @<unspecified file>:1");
    QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @<unspecified file>:-1");
    QmlComponent component(&engine, "something:");
    QObject *object = component.create();
    QVERIFY(object == 0);
}


/*void tst_qmlparser::invalidXML()
{
    //extra stuff on end
    {
        QTest::ignoreMessage(QtCriticalMsg, "Extra content at end of document. @myprogram.qml:1");
        QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @myprogram.qml:-1");
        QmlComponent component(&engine, "<MyQmlObject/><something/>", QUrl("myprogram.qml"));
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    //mismatched tags
    {
        QTest::ignoreMessage(QtCriticalMsg, "Opening and ending tag mismatch. @myprogram.qml:2");
        QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @myprogram.qml:-1");
        QmlComponent component(&engine, "<MyQmlObject>\n</MyContainer>", QUrl("myprogram.qml"));
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    {
        QTest::ignoreMessage(QtCriticalMsg, "Expected '>' or '/', but got '<'. @myprogram.qml:1");
        QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @myprogram.qml:-1");
        QmlComponent component(&engine, "<MyQmlObject < />", QUrl("myprogram.qml"));
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

    {
        QTest::ignoreMessage(QtCriticalMsg, "Premature end of document. @myprogram.qml:1");
        QTest::ignoreMessage(QtWarningMsg, "Can't compile because of earlier errors @myprogram.qml:-1");
        QmlComponent component(&engine, "<MyQmlObject something=\" />", QUrl("myprogram.qml"));
        QObject *object = component.create();
        QVERIFY(object == 0);
    }

}*/

void tst_qmlparser::duplicateIDs()
{
    QTest::ignoreMessage(QtWarningMsg, "An id (\"MyID\") is not unique within its scope. @<unspecified file>:3");
    QmlComponent component(&engine, "MyContainer {\nMyQmlObject { id: MyID }\nMyQmlObject { id: MyID }\n}");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

void tst_qmlparser::invalidID()
{
    QTest::ignoreMessage(QtWarningMsg, "'1' is not a valid id @<unspecified file>:1");
    QmlComponent component(&engine, "MyQmlObject { id: 1 }");
    QObject *object = component.create();
    QVERIFY(object == 0);
}

void tst_qmlparser::interfaceProperty()
{
    QmlComponent component(&engine, "MyQmlObject { interface: MyQmlObject }");
    MyQmlObject *object = qobject_cast<MyQmlObject*>(component.create());
    QVERIFY(object != 0);
    QVERIFY(object->interface());
    QVERIFY(object->interface()->id == 913);
}

void tst_qmlparser::interfaceQmlList()
{
    QmlComponent component(&engine, "MyContainer { qmllistInterfaces: MyQmlObject {} }");
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmllistAccessor().count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qmllistAccessor().at(ii)->id == 913);
}

void tst_qmlparser::interfaceQList()
{
    QmlComponent component(&engine, "MyContainer { qlistInterfaces: MyQmlObject {} }");
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container != 0);
    QVERIFY(container->qlistInterfaces()->count() == 2);
    for(int ii = 0; ii < 2; ++ii)
        QVERIFY(container->qlistInterfaces()->at(ii)->id == 913);
}

/*void tst_qmlparser::cannotAssignBindingToSignal()
{
    QTest::ignoreMessage(QtWarningMsg, "Cannot assign binding to signal property @<unspecified file>:1");
    QmlComponent component(&engine, "<MyQmlObject onBasicSignal=\"{print(1921)}\" />");
    MyContainer *container= qobject_cast<MyContainer*>(component.create());
    QVERIFY(container == 0);
}*/

void tst_qmlparser::assignObjectToSignal()
{
    QmlComponent component(&engine, "MyQmlObject { onBasicSignal: MyQmlObject {} }");
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
    QVERIFY(object != 0);
    QTest::ignoreMessage(QtWarningMsg, "MyQmlObject::basicSlot");
    emit object->basicSignal();
}

void tst_qmlparser::crash1()
{
    QmlComponent component(&engine, "Component {}");
    MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
}

QTEST_MAIN(tst_qmlparser)

#include "tst_qmlparser.moc"
