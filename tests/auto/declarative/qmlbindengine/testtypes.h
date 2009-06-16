#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlexpression.h>

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool trueProperty READ trueProperty)
    Q_PROPERTY(bool falseProperty READ falseProperty)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty NOTIFY stringChanged)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty NOTIFY objectChanged);
public:
    MyQmlObject(): m_methodCalled(false), m_methodIntCalled(false), m_object(0) {}

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

    QObject *objectProperty() const { return m_object; }
    void setObjectProperty(QObject *obj) { 
        if (obj == m_object)
            return;
        m_object = obj;
        emit objectChanged();
    }

    bool methodCalled() const { return m_methodCalled; }
    bool methodIntCalled() const { return m_methodIntCalled; }

    QString string() const { return m_string; }
signals:
    void basicSignal();
    void argumentSignal(int a, QString b, qreal c);
    void stringChanged();
    void objectChanged();

public slots:
    void method() { m_methodCalled = true; }
    void method(int a) { if(a == 163) m_methodIntCalled = true; }
    void setString(const QString &s) { m_string = s; }

private:
    friend class tst_qmlbindengine;
    bool m_methodCalled;
    bool m_methodIntCalled;

    QObject *m_object;
    QString m_string;
};

QML_DECLARE_TYPE(MyQmlObject);

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

class MyDeferredObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty);
    Q_PROPERTY(QObject *objectProperty2 READ objectProperty2 WRITE setObjectProperty2);
    Q_CLASSINFO("DeferredPropertyNames", "value,objectProperty,objectProperty2");

public:
    MyDeferredObject() : m_value(0), m_object(0), m_object2(0) {}

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

    QObject *objectProperty() const { return m_object; }
    void setObjectProperty(QObject *obj) { m_object = obj; }

    QObject *objectProperty2() const { return m_object2; }
    void setObjectProperty2(QObject *obj) { m_object2 = obj; }
private:
    int m_value;
    QObject *m_object;
    QObject *m_object2;
};
QML_DECLARE_TYPE(MyDeferredObject);

class MyBaseExtendedObject : public QObject
{
Q_OBJECT
Q_PROPERTY(int baseProperty READ baseProperty WRITE setBaseProperty);
public:
    MyBaseExtendedObject() : m_value(0) {}

    int baseProperty() const { return m_value; }
    void setBaseProperty(int v) { m_value = v; }

private:
    int m_value;
};
QML_DECLARE_TYPE(MyBaseExtendedObject);

class MyExtendedObject : public MyBaseExtendedObject
{
Q_OBJECT
Q_PROPERTY(int coreProperty READ coreProperty WRITE setCoreProperty);
public:
    MyExtendedObject() : m_value(0) {}

    int coreProperty() const { return m_value; }
    void setCoreProperty(int v) { m_value = v; }

private:
    int m_value;
};
QML_DECLARE_TYPE(MyExtendedObject);

#endif // TESTTYPES_H

