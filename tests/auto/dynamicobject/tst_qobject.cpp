/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#define QT3_SUPPORT
#include <QtTest/QtTest>


#include <qcoreapplication.h>

#include <qmetaobject.h>

#include "qobject.h"
#include "private/qobject_p.h"
#include "qmetaobjectbuilder.h"
#include "qdynamicmetaobject.h"


//TESTED_CLASS=
//TESTED_FILES=

class tst_QObject : public QObject
{
    Q_OBJECT

public:
    tst_QObject();
    virtual ~tst_QObject();


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void addSignal();
    void addSlot();
    void addProperty();
    void dynamicProperty();
};

//===========================================================================

class MyDynamicMetaObject : public QDynamicMetaObject
{
public:
    MyDynamicMetaObject(const QMetaObject *base);

    int createProperty(const QObject *object, const char *, const char *);

    virtual void callSlot(QObject *object, int id, const QList<QVariant> &args);

    virtual void writeProperty(QObject *, int id, const QVariant &value) {
        props[property(id).name()] = value;
    }

    virtual QVariant readProperty(QObject *, int id) const {
        return props[property(id).name()];
    }

    QString calledSlot;
    QString string;

private:
    QMap<QString,QVariant> props;
};

MyDynamicMetaObject::MyDynamicMetaObject(const QMetaObject *_base)
    : QDynamicMetaObject(_base)
{
}

void MyDynamicMetaObject::callSlot(QObject *object, int id, const QList<QVariant> &args)
{
    calledSlot = method(id).signature();
    qDebug() << "Handled slot" << id << calledSlot << args;
    if (calledSlot == "string(QString)") {
        string = args[0].toString();
    }
}

int MyDynamicMetaObject::createProperty(const QObject *, const char *name, const char *type)
{
    qDebug() << "createProperty" << name;
    if (name == QString("dynamicProp")) {
        addProperty(name, type);
        int id = indexOfProperty(name);
        return id;
    }

    return -1;
}

//===========================================================================

class MyDynamicObject : public QObject
{
    Q_OBJECT
public:
    MyDynamicObject();

    void addSignal(const char *sigName) {
        dynamicMeta->addSignal(sigName);
    }

    void addSignal(const char *sigName, const QList<QByteArray> &parameterNames) {
        dynamicMeta->addSignal(sigName, parameterNames);
    }

    void emitSignal(const char *sigName) {
        int idx = dynamicMeta->indexOfSignal(sigName);
        if (idx >= 0)
            dynamicMeta->activate(this, idx, 0);
    }

    void emitSignal(const char *sigName, QGenericArgument val0
            , QGenericArgument val1=QGenericArgument(0)
            , QGenericArgument val2=QGenericArgument(0)
            , QGenericArgument val3=QGenericArgument(0)
            , QGenericArgument val4=QGenericArgument(0)
            , QGenericArgument val5=QGenericArgument(0)
            , QGenericArgument val6=QGenericArgument(0)
            , QGenericArgument val7=QGenericArgument(0))
    {
        int idx = dynamicMeta->indexOfSignal(sigName);
        if (idx >= 0) {
            QGenericReturnArgument rv;
            dynamicMeta->method(idx).invoke(this, Qt::DirectConnection, rv,
                    val0, val1, val2, val3, val4, val5, val6, val7);
        }
    }

    void addSlot(const char *slotName) {
        dynamicMeta->addSlot(slotName);
    }

    void addSlot(const char *slotName, const QList<QByteArray> &parameterNames) {
        dynamicMeta->addSlot(slotName, parameterNames);
    }

    void addProperty(const char *name, const char *type, const char *notifier) {
        dynamicMeta->addProperty(name, type, notifier);
    }

    QString calledSlot() const { return dynamicMeta->calledSlot; }

    void emitNormalSignal() { emit normalSignal(); }

    MyDynamicMetaObject *dynamicMeta;

public slots:
    void normalSlot() {}

signals:
    void normalSignal();

};

MyDynamicObject::MyDynamicObject()
    : QObject()
{
    dynamicMeta = new MyDynamicMetaObject(&staticMetaObject);
    setDynamicMetaObject(dynamicMeta);
}

//===========================================================================

class TestObject : public QObject
{
    Q_OBJECT
public:
    TestObject() : slot1Called(false), slot2Called(false) {}

    void emitSignal() { emit triggered(); }
    void setText(const QString &str) { emit textChanged(str); }

    bool slot1Called;
    bool slot2Called;
    QString string;

public slots:
    void slot1() {
        slot1Called = true;
        qDebug() << "Called slot1()";
    }

    void slot2() {
        slot2Called = true;
        qDebug() << "Called slot2()";
    }

    void slotString(QString str) {
        string = str;
        qDebug() << "Called slotString()" << str;
    }

signals:
    void triggered();
    void textChanged(QString);
};


tst_QObject::tst_QObject()
{

}

tst_QObject::~tst_QObject()
{

}

void tst_QObject::initTestCase()
{
}

void tst_QObject::cleanupTestCase()
{
}

void tst_QObject::init()
{
}

void tst_QObject::cleanup()
{
}

void tst_QObject::addSignal()
{
    MyDynamicObject myObj;
    myObj.addSignal("testSignal()");

    TestObject testObj;

    connect(&myObj, SIGNAL(testSignal()), &testObj, SLOT(slot1()));
    connect(&myObj, SIGNAL(normalSignal()), &testObj, SLOT(slot2()));

    myObj.emitSignal("testSignal()");

    QVERIFY(testObj.slot1Called);
    QVERIFY(!testObj.slot2Called);

    testObj.slot1Called = false;

    myObj.emitNormalSignal();

    QVERIFY(!testObj.slot1Called);
    QVERIFY(testObj.slot2Called);

    // With parameter
    myObj.addSignal("stringSignal(QString)");
    connect(&myObj, SIGNAL(stringSignal(QString)), &testObj, SLOT(slotString(QString)));

    QString param("Hello");
    myObj.emitSignal("stringSignal(QString)", Q_ARG(QString, param));

    QCOMPARE(testObj.string, QString("Hello"));
}

void tst_QObject::addSlot()
{
    MyDynamicObject myObj;
    myObj.addSlot("testSlot1()");
    myObj.addSignal("testSignal()");
    myObj.addSlot("testSlot()");

    TestObject testObj;

    connect(&testObj, SIGNAL(triggered()), &myObj, SLOT(testSlot()));
    connect(&testObj, SIGNAL(triggered()), &myObj, SIGNAL(testSignal())); // signal chaining
    connect(&myObj, SIGNAL(testSignal()), &testObj, SLOT(slot1()));

    testObj.emitSignal();

    QCOMPARE(myObj.calledSlot(), QString("testSlot()"));
    QVERIFY(testObj.slot1Called);

    // Verify parameter type and name
    myObj.addSlot("string(QString)", QList<QByteArray>() << "name");
    int idx = myObj.metaObject()->indexOfSlot("string(QString)");
    QVERIFY(idx > 0);
    QMetaMethod method = myObj.metaObject()->method(idx);
    QVERIFY(method.parameterTypes()[0] == "QString");
    QVERIFY(method.parameterNames()[0] == "name");

    connect(&testObj, SIGNAL(textChanged(QString)), &myObj, SLOT(string(QString)));
    testObj.setText("Hello");
    QCOMPARE(myObj.dynamicMeta->string, QString("Hello"));
}

void tst_QObject::addProperty()
{
    MyDynamicObject myObj;
    myObj.addSignal("propChanged()");
    myObj.addProperty("myProp", QVariant::typeToName(QVariant::String), "propChanged()");

    myObj.setProperty("myProp", QString("hello"));
    qDebug() << "property value" << myObj.property("myProp").toString();
    QCOMPARE(myObj.property("myProp").toString(), QString("hello"));
    
    TestObject testObj;
    connect(&myObj, SIGNAL(propChanged()), &testObj, SLOT(slot1()));
    myObj.setProperty("myProp", QString("there"));

    QCOMPARE(myObj.property("myProp").toString(), QString("there"));
    QVERIFY(testObj.slot1Called);
}

void tst_QObject::dynamicProperty()
{
    MyDynamicObject myObj;

    myObj.setProperty("dynamicProp", QString("hello"));
    qDebug() << "property value" << myObj.property("dynamicProp").toString();
    QCOMPARE(myObj.property("dynamicProp").toString(), QString("hello"));
}

QTEST_MAIN(tst_QObject)
#include "tst_qobject.moc"
