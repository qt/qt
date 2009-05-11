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
    void contextPropertiesTriggerReeval();

private:
    QmlEngine engine;
};

void tst_qmlbindengine::boolPropertiesEvaluateAsBool()
{
    {
        QmlComponent component(&engine, "MyQmlObject { stringProperty: trueProperty?'pass':'fail' }");
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
    {
        QmlComponent component(&engine, "MyQmlObject { stringProperty: falseProperty?'fail':'pass' }");
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->stringProperty(), QLatin1String("pass"));
    }
}

void tst_qmlbindengine::signalAssignment()
{
    {
        QmlComponent component(&engine, "MyQmlObject { onBasicSignal: setString('pass') }");
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->string(), QString());
        emit object->basicSignal();
        QCOMPARE(object->string(), QString("pass"));
    }

    {
        QmlComponent component(&engine, "MyQmlObject { onArgumentSignal: setString('pass ' + a + ' ' + b + ' ' + c) }");
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
        QmlComponent component(&engine, "MyQmlObject { id: MyObject; onBasicSignal: MyObject.method() }");
        MyQmlObject *object = qobject_cast<MyQmlObject *>(component.create());
        QVERIFY(object != 0);
        QCOMPARE(object->methodCalled(), false);
        QCOMPARE(object->methodIntCalled(), false);
        emit object->basicSignal();
        QCOMPARE(object->methodCalled(), true);
        QCOMPARE(object->methodIntCalled(), false);
    }

    {
        QmlComponent component(&engine, "MyQmlObject { id: MyObject; onBasicSignal: MyObject.method(163) }");
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
    QmlComponent component(&engine, "MyQmlContainer { children : [ "\
                                    "MyQmlObject { id: Object1; stringProperty: \"hello\" + Object2.stringProperty }, "\
                                    "MyQmlObject { id: Object2; stringProperty: \"hello\" + Object1.stringProperty } ] }");
    //### ignoreMessage doesn't seem to work here
    //QTest::ignoreMessage(QtWarningMsg, "QML MyQmlObject (unknown location): Binding loop detected for property \"stringProperty\"");
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
