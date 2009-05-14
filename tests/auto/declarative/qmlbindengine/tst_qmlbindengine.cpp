#include <qtest.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlcontext.h>

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool trueProperty READ trueProperty)
    Q_PROPERTY(bool falseProperty READ falseProperty)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty NOTIFY stringChanged)
public:
    MyQmlObject(): m_methodCalled(false), m_methodIntCalled(false) {}

    bool trueProperty() const { return true; }
    bool falseProperty() const { return false; }

    QString stringProperty() const { return m_string; }
    void setStringProperty(const QString &s)
    {
        if (s == m_string)
            return;
        m_string = s;
        emit stringChanged();
    }

    bool methodCalled() const { return m_methodCalled; }
    bool methodIntCalled() const { return m_methodIntCalled; }

    QString string() const { return m_string; }
signals:
    void basicSignal();
    void argumentSignal(int a, QString b, qreal c);
    void stringChanged();

public slots:
    void method() { m_methodCalled = true; }
    void method(int a) { if(a == 163) m_methodIntCalled = true; }
    void setString(const QString &s) { m_string = s; }

private:
    friend class tst_qmlbindengine;
    bool m_methodCalled;
    bool m_methodIntCalled;

    QString m_string;
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(MyQmlObject,MyQmlObject);

class MyQmlContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<MyQmlContainer*>* children READ children)
public:
    MyQmlContainer() {}

    QList<MyQmlContainer*> *children() { return &m_children; }

private:
    QList<MyQmlContainer*> m_children;
};

QML_DECLARE_TYPE(MyQmlContainer);
QML_DEFINE_TYPE(MyQmlContainer,MyQmlContainer);

#define TEST_FILE(filename) \
    QUrl::fromLocalFile(QApplication::applicationDirPath() + "/" + filename)

class tst_qmlbindengine : public QObject
{
    Q_OBJECT
public:
    tst_qmlbindengine() {}

private slots:
    void boolPropertiesEvaluateAsBool();
    void methods();
    void signalAssignment();
    void bindingLoop();
    void basicExpressions();
    void basicExpressions_data();
    void contextPropertiesTriggerReeval();

private:
    QmlEngine engine;
};

void tst_qmlbindengine::boolPropertiesEvaluateAsBool()
{
    {
        QmlComponent component(&engine, TEST_FILE("boolPropertiesEvaluateAsBool.1.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
    {
        QmlComponent component(&engine, TEST_FILE("boolPropertiesEvaluateAsBool.2.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
}

void tst_qmlbindengine::signalAssignment()
{
    {
        QmlComponent component(&engine, TEST_FILE("signalAssignment.1.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->string(), QString());
        emit object->basicSignal();
        QCOMPARE(object->string(), QString("pass"));
    }

    {
        QmlComponent component(&engine, TEST_FILE("signalAssignment.2.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->string(), QString());
        emit object->argumentSignal(19, "Hello world!", 10.3);
        QCOMPARE(object->string(), QString("pass 19 Hello world! 10.3"));
    }
}

void tst_qmlbindengine::methods()
{
    {
        QmlComponent component(&engine, TEST_FILE("methods.1.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), false);
        emit object->basicSignal();
        QCOMPARE(object->methodCalled(), true);
        QCOMPARE(object->methodIntCalled(), false);
    }

    {
        QmlComponent component(&engine, TEST_FILE("methods.2.txt"));
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), false);
        emit object->basicSignal();
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), true);
    }
}

void tst_qmlbindengine::bindingLoop()
{
    QmlComponent component(&engine, TEST_FILE("bindingLoop.txt"));
    QTest::ignoreMessage(QtWarningMsg, "QML MyQmlObject (unknown location): Binding loop detected for property \"stringProperty\" ");
    QObject *object = component.create();
    QVERIFY(object != 0);
}

class MyExpression : public QmlExpression
{
public:
    MyExpression(QmlContext *ctxt, const QString &expr)
        : QmlExpression(ctxt, expr, 0), changed(false)
    {
    }

    virtual void valueChanged() {
        changed = true;
    }
    bool changed;
};


class MyDefaultObject1 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int horseLegs READ horseLegs);
    Q_PROPERTY(int antLegs READ antLegs);
public:
    int horseLegs() const { return 4; }
    int antLegs() const { return 6; }
};

class MyDefaultObject2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int antLegs READ antLegs);
    Q_PROPERTY(int emuLegs READ emuLegs);
public:
    int antLegs() const { return 5; } // Had an accident
    int emuLegs() const { return 2; }
};

class MyDefaultObject3 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int antLegs READ antLegs);
    Q_PROPERTY(int humanLegs READ humanLegs);
public:
    int antLegs() const { return 7; } // Mutant
    int humanLegs() const { return 2; }
    int millipedeLegs() const { return 1000; }
};

void tst_qmlbindengine::basicExpressions_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QVariant>("result");
    QTest::addColumn<bool>("nest");

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

void tst_qmlbindengine::basicExpressions()
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

// Tests that modifying a context property will reevaluate expressions
void tst_qmlbindengine::contextPropertiesTriggerReeval()
{
    QmlContext context(engine.rootContext());
    MyQmlObject object1;
    MyQmlObject object2;

    object1.setStringProperty("Hello");
    object2.setStringProperty("World");

    context.setContextProperty("testProp", QVariant(1));
    context.setContextProperty("testObj", &object1);

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
}

QTEST_MAIN(tst_qmlbindengine)

#include "tst_qmlbindengine.moc"
