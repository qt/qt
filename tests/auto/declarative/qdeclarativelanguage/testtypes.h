/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtCore/qdatetime.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeparserstatus.h>
#include <QtDeclarative/qdeclarativepropertyvaluesource.h>
#include <QtDeclarative/qdeclarativescriptstring.h>
#include <QtDeclarative/qdeclarativeproperty.h>

#include <private/qdeclarativecustomparser_p.h>

QVariant myCustomVariantTypeConverter(const QString &data);

class MyInterface 
{
public:
    MyInterface() : id(913) {}
    int id;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(MyInterface, "com.trolltech.Qt.Test.MyInterface");
QT_END_NAMESPACE
QML_DECLARE_INTERFACE(MyInterface);

struct MyCustomVariantType
{
    MyCustomVariantType() : a(0) {}
    int a;
};
Q_DECLARE_METATYPE(MyCustomVariantType);

class MyAttachedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int value2 READ value2 WRITE setValue2)
public:
    MyAttachedObject(QObject *parent) : QObject(parent), m_value(0), m_value2(0) {}

    int value() const { return m_value; }
    void setValue(int v) { if (m_value != v) { m_value = v; emit valueChanged(); } }

    int value2() const { return m_value2; }
    void setValue2(int v) { m_value2 = v; }

signals:
    void valueChanged();

private:
    int m_value;
    int m_value2;
};

class MyQmlObject : public QObject, public MyInterface
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue FINAL)
    Q_PROPERTY(QString readOnlyString READ readOnlyString)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QMatrix matrix READ matrix WRITE setMatrix)  //assumed to be unsupported by QML
    Q_PROPERTY(MyInterface *interfaceProperty READ interface WRITE setInterface)
    Q_PROPERTY(int onLiteralSignal READ onLiteralSignal WRITE setOnLiteralSignal)
    Q_PROPERTY(MyCustomVariantType customType READ customType WRITE setCustomType)
    Q_PROPERTY(MyQmlObject *qmlobjectProperty READ qmlobject WRITE setQmlobject)
    Q_PROPERTY(int propertyWithNotify READ propertyWithNotify WRITE setPropertyWithNotify NOTIFY oddlyNamedNotifySignal)
    Q_PROPERTY(int nonScriptable READ nonScriptable WRITE setNonScriptable SCRIPTABLE false);

    Q_INTERFACES(MyInterface)
public:
    MyQmlObject() : m_value(-1), m_interface(0), m_qmlobject(0) { qRegisterMetaType<MyCustomVariantType>("MyCustomVariantType"); }

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
    Q_CLASSINFO("DefaultMethod", "basicSlot()")

    int onLiteralSignal() const { return m_value; }
    void setOnLiteralSignal(int v) { m_value = v; }

    MyQmlObject *qmlobject() const { return m_qmlobject; }
    void setQmlobject(MyQmlObject *o) { m_qmlobject = o; }

    MyCustomVariantType customType() const { return m_custom; }
    void setCustomType(const MyCustomVariantType &v)  { m_custom = v; }

    int propertyWithNotify() const { return m_propertyWithNotify; }
    void setPropertyWithNotify(int i) { m_propertyWithNotify = i; emit oddlyNamedNotifySignal(); }

    int nonScriptable() const { return 0; }
    void setNonScriptable(int) {}
public slots:
    void basicSlot() { qWarning("MyQmlObject::basicSlot"); }
    void basicSlotWithArgs(int v) { qWarning("MyQmlObject::basicSlotWithArgs(%d)", v); }

signals:
    void basicSignal();
    void basicParameterizedSignal(int parameter);
    void oddlyNamedNotifySignal();

private:
    friend class tst_qdeclarativelanguage;
    int m_value;
    MyInterface *m_interface;
    MyQmlObject *m_qmlobject;
    MyCustomVariantType m_custom;
    int m_propertyWithNotify;
};
QML_DECLARE_TYPE(MyQmlObject)
QML_DECLARE_TYPEINFO(MyQmlObject, QML_HAS_ATTACHED_PROPERTIES)

class MyGroupedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeScriptString script READ script WRITE setScript)
    Q_PROPERTY(int value READ value WRITE setValue)
public:
    QDeclarativeScriptString script() const { return m_script; }
    void setScript(const QDeclarativeScriptString &s) { m_script = s; }

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

private:
    int m_value;
    QDeclarativeScriptString m_script;
};


class MyTypeObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(MyEnum)
    Q_FLAGS(MyFlags)

    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty)
    Q_PROPERTY(QDeclarativeComponent *componentProperty READ componentProperty WRITE setComponentProperty)
    Q_PROPERTY(MyFlags flagProperty READ flagProperty WRITE setFlagProperty)
    Q_PROPERTY(MyEnum enumProperty READ enumProperty WRITE setEnumProperty)
    Q_PROPERTY(MyEnum readOnlyEnumProperty READ readOnlyEnumProperty)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty)
    Q_PROPERTY(uint uintProperty READ uintProperty WRITE setUintProperty)
    Q_PROPERTY(int intProperty READ intProperty WRITE setIntProperty)
    Q_PROPERTY(qreal realProperty READ realProperty WRITE setRealProperty)
    Q_PROPERTY(double doubleProperty READ doubleProperty WRITE setDoubleProperty)
    Q_PROPERTY(float floatProperty READ floatProperty WRITE setFloatProperty)
    Q_PROPERTY(QColor colorProperty READ colorProperty WRITE setColorProperty)
    Q_PROPERTY(QDate dateProperty READ dateProperty WRITE setDateProperty)
    Q_PROPERTY(QTime timeProperty READ timeProperty WRITE setTimeProperty)
    Q_PROPERTY(QDateTime dateTimeProperty READ dateTimeProperty WRITE setDateTimeProperty)
    Q_PROPERTY(QPoint pointProperty READ pointProperty WRITE setPointProperty)
    Q_PROPERTY(QPointF pointFProperty READ pointFProperty WRITE setPointFProperty)
    Q_PROPERTY(QSize sizeProperty READ sizeProperty WRITE setSizeProperty)
    Q_PROPERTY(QSizeF sizeFProperty READ sizeFProperty WRITE setSizeFProperty)
    Q_PROPERTY(QRect rectProperty READ rectProperty WRITE setRectProperty NOTIFY rectPropertyChanged)
    Q_PROPERTY(QRect rectProperty2 READ rectProperty2 WRITE setRectProperty2)
    Q_PROPERTY(QRectF rectFProperty READ rectFProperty WRITE setRectFProperty)
    Q_PROPERTY(bool boolProperty READ boolProperty WRITE setBoolProperty)
    Q_PROPERTY(QVariant variantProperty READ variantProperty WRITE setVariantProperty)
    Q_PROPERTY(QVector3D vectorProperty READ vectorProperty WRITE setVectorProperty)
    Q_PROPERTY(QUrl urlProperty READ urlProperty WRITE setUrlProperty)

    Q_PROPERTY(QDeclarativeScriptString scriptProperty READ scriptProperty WRITE setScriptProperty)
    Q_PROPERTY(MyGroupedObject *grouped READ grouped CONSTANT)
    Q_PROPERTY(MyGroupedObject *nullGrouped READ nullGrouped CONSTANT)

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

    QDeclarativeComponent *componentPropertyValue;
    QDeclarativeComponent *componentProperty() const {
        return componentPropertyValue;
    }
    void setComponentProperty(QDeclarativeComponent *v) {
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

    MyEnum readOnlyEnumProperty() const {
        return EnumVal1;
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

    float floatPropertyValue;
    float floatProperty() const {
       return floatPropertyValue;
    }
    void setFloatProperty(const float &v) {
        floatPropertyValue = v;
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
        emit rectPropertyChanged();
    }

    QRect rectPropertyValue2;
    QRect rectProperty2() const {
       return rectPropertyValue2;
    }
    void setRectProperty2(const QRect &v) {
        rectPropertyValue2 = v;
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

    QVector3D vectorPropertyValue;
    QVector3D vectorProperty() const {
        return vectorPropertyValue;
    }
    void setVectorProperty(const QVector3D &v) {
        vectorPropertyValue = v;
    }

    QUrl urlPropertyValue;
    QUrl urlProperty() const {
        return urlPropertyValue;
    }
    void setUrlProperty(const QUrl &v) {
        urlPropertyValue = v;
    }

    QDeclarativeScriptString scriptPropertyValue;
    QDeclarativeScriptString scriptProperty() const {
        return scriptPropertyValue;
    }
    void setScriptProperty(const QDeclarativeScriptString &v) {
        scriptPropertyValue = v;
    }

    MyGroupedObject groupedValue;
    MyGroupedObject *grouped() { return &groupedValue; }

    MyGroupedObject *nullGrouped() { return 0; }

    void doAction() { emit action(); }
signals:
    void action();
    void rectPropertyChanged();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MyTypeObject::MyFlags)


class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
    Q_PROPERTY(QDeclarativeListProperty<MyContainer> containerChildren READ containerChildren)
    Q_PROPERTY(QDeclarativeListProperty<MyInterface> qlistInterfaces READ qlistInterfaces)
    Q_CLASSINFO("DefaultProperty", "children")
public:
    MyContainer() {}

    QDeclarativeListProperty<QObject> children() { return QDeclarativeListProperty<QObject>(this, m_children); }
    QDeclarativeListProperty<MyContainer> containerChildren() { return QDeclarativeListProperty<MyContainer>(this, m_containerChildren); }
    QList<QObject *> *getChildren() { return &m_children; }
    QDeclarativeListProperty<MyInterface> qlistInterfaces() { return QDeclarativeListProperty<MyInterface>(this, m_interfaces); }
    QList<MyInterface *> *getQListInterfaces() { return &m_interfaces; }

    QList<MyContainer*> m_containerChildren;
    QList<QObject*> m_children;
    QList<MyInterface *> m_interfaces;
};


class MyPropertyValueSource : public QObject, public QDeclarativePropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativePropertyValueSource)
public:
    MyPropertyValueSource()
        : QDeclarativePropertyValueSource() {}

    QDeclarativeProperty prop;
    virtual void setTarget(const QDeclarativeProperty &p)
    {
        prop = p;
    }
};

class UnavailableType : public QObject
{
    Q_OBJECT
public:
    UnavailableType() {}
};

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


namespace MyNamespace {
    class MyNamespacedType : public QObject
    {
        Q_OBJECT
    };

    class MySecondNamespacedType : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QDeclarativeListProperty<MyNamespace::MyNamespacedType> list READ list)
    public:
        QDeclarativeListProperty<MyNamespacedType> list() { return QDeclarativeListProperty<MyNamespacedType>(this, m_list); }

    private:
        QList<MyNamespacedType *> m_list;
    };
}

class MyCustomParserType : public QObject
{
    Q_OBJECT
};

class MyCustomParserTypeParser : public QDeclarativeCustomParser
{
public:
    QByteArray compile(const QList<QDeclarativeCustomParserProperty> &) { return QByteArray(); }
    void setCustomData(QObject *, const QByteArray &) {}
};

class MyParserStatus : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
public:
    MyParserStatus() : m_cbc(0), m_ccc(0) {}

    int classBeginCount() const { return m_cbc; }
    int componentCompleteCount() const { return m_ccc; }

    virtual void classBegin() { m_cbc++; }
    virtual void componentComplete() { m_ccc++; }
private:
    int m_cbc;
    int m_ccc;
};

void registerTypes();

#endif // TESTTYPES_H
