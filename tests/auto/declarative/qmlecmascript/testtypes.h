#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>

class MyQmlAttachedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value CONSTANT)
public:
    MyQmlAttachedObject(QObject *parent) : QObject(parent) {}

    int value() const { return 19; }
};

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(MyEnum)
    Q_ENUMS(MyEnum2)
    Q_PROPERTY(int deleteOnSet READ deleteOnSet WRITE setDeleteOnSet);
    Q_PROPERTY(bool trueProperty READ trueProperty CONSTANT)
    Q_PROPERTY(bool falseProperty READ falseProperty CONSTANT)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty NOTIFY stringChanged)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty NOTIFY objectChanged);
public:
    MyQmlObject(): m_methodCalled(false), m_methodIntCalled(false), m_object(0) {}

    enum MyEnum { EnumValue1 = 0, EnumValue2 = 1 };
    enum MyEnum2 { EnumValue3 = 2, EnumValue4 = 3 };

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

    static MyQmlAttachedObject *qmlAttachedProperties(QObject *o) {
        return new MyQmlAttachedObject(o);
    }

    int deleteOnSet() const { return 1; }
    void setDeleteOnSet(int v) { if(v) delete this; }
signals:
    void basicSignal();
    void argumentSignal(int a, QString b, qreal c);
    void stringChanged();
    void objectChanged();

public slots:
    void deleteMe() { delete this; }
    void method() { m_methodCalled = true; }
    void method(int a) { if(a == 163) m_methodIntCalled = true; }
    void setString(const QString &s) { m_string = s; }

private:
    friend class tst_qmlecmascript;
    bool m_methodCalled;
    bool m_methodIntCalled;

    QObject *m_object;
    QString m_string;
};

QML_DECLARE_TYPE(MyQmlObject);

class MyQmlContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<MyQmlObject*>* children READ children)
public:
    MyQmlContainer() {}

    QList<MyQmlObject*> *children() { return &m_children; }

private:
    QList<MyQmlObject*> m_children;
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
    Q_PROPERTY(int horseLegs READ horseLegs CONSTANT);
    Q_PROPERTY(int antLegs READ antLegs CONSTANT);
public:
    int horseLegs() const { return 4; }
    int antLegs() const { return 6; }
};

class MyDefaultObject2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int antLegs READ antLegs CONSTANT);
    Q_PROPERTY(int emuLegs READ emuLegs CONSTANT);
public:
    int antLegs() const { return 5; } // Had an accident
    int emuLegs() const { return 2; }
};

class MyDefaultObject3 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int antLegs READ antLegs CONSTANT);
    Q_PROPERTY(int humanLegs READ humanLegs CONSTANT);
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

class MyTypeObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPoint pointProperty READ pointProperty WRITE setPointProperty);
    Q_PROPERTY(QPointF pointFProperty READ pointFProperty WRITE setPointFProperty);
    Q_PROPERTY(QSize sizeProperty READ sizeProperty WRITE setSizeProperty);
    Q_PROPERTY(QSizeF sizeFProperty READ sizeFProperty WRITE setSizeFProperty);
    Q_PROPERTY(QRect rectProperty READ rectProperty WRITE setRectProperty NOTIFY rectPropertyChanged);
    Q_PROPERTY(QRectF rectFProperty READ rectFProperty WRITE setRectFProperty);
    
public:
    MyTypeObject() {}

    QPoint pointPropertyValue;
    QPoint pointProperty() const {
       return pointPropertyValue;
    }
    void setPointProperty(const QPoint &v) {
        pointPropertyValue = v;
    }

    QPointF pointFPropertyValue;
    QPointF pointFProperty() const {
       return pointFPropertyValue;
    }
    void setPointFProperty(const QPointF &v) {
        pointFPropertyValue = v;
    }

    QSize sizePropertyValue;
    QSize sizeProperty() const {
       return sizePropertyValue;
    }
    void setSizeProperty(const QSize &v) {
        sizePropertyValue = v;
    }

    QSizeF sizeFPropertyValue;
    QSizeF sizeFProperty() const {
       return sizeFPropertyValue;
    }
    void setSizeFProperty(const QSizeF &v) {
        sizeFPropertyValue = v;
    }

    QRect rectPropertyValue;
    QRect rectProperty() const {
       return rectPropertyValue;
    }
    void setRectProperty(const QRect &v) {
        rectPropertyValue = v;
    }

    QRectF rectFPropertyValue;
    QRectF rectFProperty() const {
       return rectFPropertyValue;
    }
    void setRectFProperty(const QRectF &v) {
        rectFPropertyValue = v;
    }

};
QML_DECLARE_TYPE(MyTypeObject);

#endif // TESTTYPES_H

