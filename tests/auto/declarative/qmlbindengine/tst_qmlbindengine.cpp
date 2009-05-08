#include <qtest.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlengine.h>

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool trueProperty READ trueProperty)
    Q_PROPERTY(bool falseProperty READ falseProperty)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty)
public:
    MyQmlObject(): m_methodCalled(false), m_methodIntCalled(false) {}

    bool trueProperty() const { return true; }
    bool falseProperty() const { return false; }

    QString stringProperty() const { return m_string; }
    void setStringProperty(const QString &s) { m_string = s; }

    bool methodCalled() const { return m_methodCalled; }
    bool methodIntCalled() const { return m_methodIntCalled; }

    QString string() const { return m_string; }
signals:
    void basicSignal();
    void argumentSignal(int a, QString b, qreal c);

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

class tst_qmlbindengine : public QObject
{
    Q_OBJECT
public:
    tst_qmlbindengine() {}

private slots:
    void boolPropertiesEvaluateAsBool();
    void methods();
    void signalAssignment();

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

QTEST_MAIN(tst_qmlbindengine)

#include "tst_qmlbindengine.moc"
