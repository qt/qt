#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qcolor.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlparserstatus.h>
#include <QtDeclarative/qmlpropertyvaluesource.h>

class MyInterface 
{
public:
    MyInterface() : id(913) {}
    int id;
};
Q_DECLARE_INTERFACE(MyInterface, "com.trolltech.Qt.Test.MyInterface");
QML_DECLARE_INTERFACE(MyInterface);

struct MyCustomVariantType
{
    MyCustomVariantType() : a(0) {}
    int a;
};
Q_DECLARE_METATYPE(MyCustomVariantType);

static QVariant myCustomVariantTypeConverter(const QString &data)
{
    MyCustomVariantType rv;
    rv.a = data.toInt();
    return QVariant::fromValue(rv);
}

class MyAttachedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
public:
    MyAttachedObject(QObject *parent) : QObject(parent), m_value(0) {}

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

private:
    int m_value;
};

class MyQmlObject : public QObject, public MyInterface, public QmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(QString readOnlyString READ readOnlyString)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QMatrix matrix READ matrix WRITE setMatrix)  //assumed to be unsupported by QML
    Q_PROPERTY(MyInterface *interface READ interface WRITE setInterface)
    Q_PROPERTY(int onLiteralSignal READ onLiteralSignal WRITE setOnLiteralSignal);
    Q_PROPERTY(MyCustomVariantType customType READ customType WRITE setCustomType);
    Q_INTERFACES(MyInterface QmlParserStatus)
public:
    MyQmlObject() : m_value(-1), m_interface(0) { qRegisterMetaType<MyCustomVariantType>("MyCustomVariantType"); }

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

    static MyAttachedObject *qmlAttachedProperties(QObject *other) {
        return new MyAttachedObject(other);
    }
    Q_CLASSINFO("DefaultMethod", "basicSlot()");

    int onLiteralSignal() const { return m_value; }
    void setOnLiteralSignal(int v) { m_value = v; }

    MyCustomVariantType customType() const { return m_custom; }
    void setCustomType(const MyCustomVariantType &v)  { m_custom = v; }
public slots:
    void basicSlot() { qWarning("MyQmlObject::basicSlot"); }

signals:
    void basicSignal();

private:
    friend class tst_qmlparser;
    int m_value;
    MyInterface *m_interface;
    MyCustomVariantType m_custom;
};
QML_DECLARE_TYPE(MyQmlObject);

class MyTypeObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(MyEnum)
    Q_FLAGS(MyFlags)

    Q_PROPERTY(QString id READ id WRITE setId);
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty);
    Q_PROPERTY(QmlComponent *componentProperty READ componentProperty WRITE setComponentProperty);
    Q_PROPERTY(MyFlags flagProperty READ flagProperty WRITE setFlagProperty);
    Q_PROPERTY(MyEnum enumProperty READ enumProperty WRITE setEnumProperty);
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty);
    Q_PROPERTY(uint uintProperty READ uintProperty WRITE setUintProperty);
    Q_PROPERTY(int intProperty READ intProperty WRITE setIntProperty);
    Q_PROPERTY(qreal realProperty READ realProperty WRITE setRealProperty);
    Q_PROPERTY(double doubleProperty READ doubleProperty WRITE setDoubleProperty);
    Q_PROPERTY(QColor colorProperty READ colorProperty WRITE setColorProperty);
    Q_PROPERTY(QDate dateProperty READ dateProperty WRITE setDateProperty);
    Q_PROPERTY(QTime timeProperty READ timeProperty WRITE setTimeProperty);
    Q_PROPERTY(QDateTime dateTimeProperty READ dateTimeProperty WRITE setDateTimeProperty);
    Q_PROPERTY(QPoint pointProperty READ pointProperty WRITE setPointProperty);
    Q_PROPERTY(QPointF pointFProperty READ pointFProperty WRITE setPointFProperty);
    Q_PROPERTY(QSize sizeProperty READ sizeProperty WRITE setSizeProperty);
    Q_PROPERTY(QSizeF sizeFProperty READ sizeFProperty WRITE setSizeFProperty);
    Q_PROPERTY(QRect rectProperty READ rectProperty WRITE setRectProperty);
    Q_PROPERTY(QRectF rectFProperty READ rectFProperty WRITE setRectFProperty);
    Q_PROPERTY(bool boolProperty READ boolProperty WRITE setBoolProperty);
    Q_PROPERTY(QVariant variantProperty READ variantProperty WRITE setVariantProperty);

public:
    MyTypeObject()
        : objectPropertyValue(0), componentPropertyValue(0) {}

    QString idValue;
    QString id() const {
        return idValue;
    }
    void setId(const QString &v) {
        idValue = v;
    }

    QObject *objectPropertyValue;
    QObject *objectProperty() const {
        return objectPropertyValue;
    }
    void setObjectProperty(QObject *v) {
        objectPropertyValue = v;
    }

    QmlComponent *componentPropertyValue;
    QmlComponent *componentProperty() const {
        return componentPropertyValue;
    }
    void setComponentProperty(QmlComponent *v) {
        componentPropertyValue = v;
    }

    enum MyFlag { FlagVal1 = 0x01, FlagVal2 = 0x02, FlagVal3 = 0x04 };
    Q_DECLARE_FLAGS(MyFlags, MyFlag)
    MyFlags flagPropertyValue;
    MyFlags flagProperty() const {
        return flagPropertyValue;
    }
    void setFlagProperty(MyFlags v) {
        flagPropertyValue = v;
    }

    enum MyEnum { EnumVal1, EnumVal2 };
    MyEnum enumPropertyValue;
    MyEnum enumProperty() const {
        return enumPropertyValue;
    }
    void setEnumProperty(MyEnum v) {
        enumPropertyValue = v;
    }

    QString stringPropertyValue;
    QString stringProperty() const {
       return stringPropertyValue;
    }
    void setStringProperty(const QString &v) {
        stringPropertyValue = v;
    }

    uint uintPropertyValue;
    uint uintProperty() const {
       return uintPropertyValue;
    }
    void setUintProperty(const uint &v) {
        uintPropertyValue = v;
    }

    int intPropertyValue;
    int intProperty() const {
       return intPropertyValue;
    }
    void setIntProperty(const int &v) {
        intPropertyValue = v;
    }

    qreal realPropertyValue;
    qreal realProperty() const {
       return realPropertyValue;
    }
    void setRealProperty(const qreal &v) {
        realPropertyValue = v;
    }

    double doublePropertyValue;
    double doubleProperty() const {
       return doublePropertyValue;
    }
    void setDoubleProperty(const double &v) {
        doublePropertyValue = v;
    }

    QColor colorPropertyValue;
    QColor colorProperty() const {
       return colorPropertyValue;
    }
    void setColorProperty(const QColor &v) {
        colorPropertyValue = v;
    }

    QDate datePropertyValue;
    QDate dateProperty() const {
       return datePropertyValue;
    }
    void setDateProperty(const QDate &v) {
        datePropertyValue = v;
    }

    QTime timePropertyValue;
    QTime timeProperty() const {
       return timePropertyValue;
    }
    void setTimeProperty(const QTime &v) {
        timePropertyValue = v;
    }

    QDateTime dateTimePropertyValue;
    QDateTime dateTimeProperty() const {
       return dateTimePropertyValue;
    }
    void setDateTimeProperty(const QDateTime &v) {
        dateTimePropertyValue = v;
    }

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

    bool boolPropertyValue;
    bool boolProperty() const {
       return boolPropertyValue;
    }
    void setBoolProperty(const bool &v) {
        boolPropertyValue = v;
    }

    QVariant variantPropertyValue;
    QVariant variantProperty() const {
       return variantPropertyValue;
    }
    void setVariantProperty(const QVariant &v) {
        variantPropertyValue = v;
    }
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MyTypeObject::MyFlags)
QML_DECLARE_TYPE(MyTypeObject);

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

class MyPropertyValueSource : public QmlPropertyValueSource
{
    Q_OBJECT
public:
    MyPropertyValueSource()
        : QmlPropertyValueSource(0) {}

    QmlMetaProperty prop;
    virtual void setTarget(const QmlMetaProperty &p)
    {
        prop = p;
    }
};
QML_DECLARE_TYPE(MyPropertyValueSource);

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



#endif // TESTTYPES_H
