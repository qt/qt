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
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtDeclarative/qdeclarativelist.h>
#include <QtCore/qrect.h>
#include <QtGui/qmatrix.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qdatetime.h>
#include <QtScript/qscriptvalue.h>
#include <QtDeclarative/qdeclarativescriptstring.h>
#include <QtDeclarative/qdeclarativecomponent.h>

class MyQmlAttachedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value CONSTANT)
    Q_PROPERTY(int value2 READ value2 WRITE setValue2)
public:
    MyQmlAttachedObject(QObject *parent) : QObject(parent), m_value2(0) {}

    int value() const { return 19; }
    int value2() const { return m_value2; }
    void setValue2(int v) { m_value2 = v; }

    void emitMySignal() { emit mySignal(); }

signals:
    void mySignal();

private:
    int m_value2;
};

class MyQmlObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(MyEnum)
    Q_ENUMS(MyEnum2)
    Q_PROPERTY(int deleteOnSet READ deleteOnSet WRITE setDeleteOnSet)
    Q_PROPERTY(bool trueProperty READ trueProperty CONSTANT)
    Q_PROPERTY(bool falseProperty READ falseProperty CONSTANT)
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(int console READ console CONSTANT)
    Q_PROPERTY(QString stringProperty READ stringProperty WRITE setStringProperty NOTIFY stringChanged)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty NOTIFY objectChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> objectListProperty READ objectListProperty CONSTANT)
    Q_PROPERTY(int resettableProperty READ resettableProperty WRITE setResettableProperty RESET resetProperty)
    Q_PROPERTY(QRegExp regExp READ regExp WRITE setRegExp)

public:
    MyQmlObject(): m_methodCalled(false), m_methodIntCalled(false), m_object(0), m_value(0), m_resetProperty(13) {}

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

    QDeclarativeListProperty<QObject> objectListProperty() { return QDeclarativeListProperty<QObject>(this, m_objectQList); }

    bool methodCalled() const { return m_methodCalled; }
    bool methodIntCalled() const { return m_methodIntCalled; }

    QString string() const { return m_string; }

    static MyQmlAttachedObject *qmlAttachedProperties(QObject *o) {
        return new MyQmlAttachedObject(o);
    }

    int deleteOnSet() const { return 1; }
    void setDeleteOnSet(int v) { if(v) delete this; }

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; }

    int resettableProperty() const { return m_resetProperty; }
    void setResettableProperty(int v) { m_resetProperty = v; }
    void resetProperty() { m_resetProperty = 13; }

    QRegExp regExp() { return m_regExp; }
    void setRegExp(const QRegExp &regExp) { m_regExp = regExp; }

    int console() const { return 11; }
signals:
    void basicSignal();
    void argumentSignal(int a, QString b, qreal c);
    void stringChanged();
    void objectChanged();
    void anotherBasicSignal();
    void thirdBasicSignal();

public slots:
    void deleteMe() { delete this; }
    void methodNoArgs() { m_methodCalled = true; }
    void method(int a) { if(a == 163) m_methodIntCalled = true; }
    void setString(const QString &s) { m_string = s; }

private:
    friend class tst_qdeclarativeecmascript;
    bool m_methodCalled;
    bool m_methodIntCalled;

    QObject *m_object;
    QString m_string;
    QList<QObject *> m_objectQList;
    int m_value;
    int m_resetProperty;
    QRegExp m_regExp;
};

QML_DECLARE_TYPEINFO(MyQmlObject, QML_HAS_ATTACHED_PROPERTIES)

class MyQmlContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<MyQmlObject> children READ children CONSTANT)
public:
    MyQmlContainer() {}

    QDeclarativeListProperty<MyQmlObject> children() { return QDeclarativeListProperty<MyQmlObject>(this, m_children); }

private:
    QList<MyQmlObject*> m_children;
};


class MyExpression : public QDeclarativeExpression
{
    Q_OBJECT
public:
    MyExpression(QDeclarativeContext *ctxt, const QString &expr)
        : QDeclarativeExpression(ctxt, 0, expr), changed(false)
    {
        QObject::connect(this, SIGNAL(valueChanged()), this, SLOT(expressionValueChanged()));
        setNotifyOnValueChanged(true);
    }

    bool changed;

public slots:
    void expressionValueChanged() {
        changed = true;
    }
};


class MyDefaultObject1 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int horseLegs READ horseLegs CONSTANT)
    Q_PROPERTY(int antLegs READ antLegs CONSTANT)
    Q_PROPERTY(int emuLegs READ emuLegs CONSTANT)
public:
    int horseLegs() const { return 4; }
    int antLegs() const { return 6; }
    int emuLegs() const { return 2; }
};

class MyDefaultObject3 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int antLegs READ antLegs CONSTANT)
    Q_PROPERTY(int humanLegs READ humanLegs CONSTANT)
public:
    int antLegs() const { return 7; } // Mutant
    int humanLegs() const { return 2; }
    int millipedeLegs() const { return 1000; }
};

class MyDeferredObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QObject *objectProperty READ objectProperty WRITE setObjectProperty)
    Q_PROPERTY(QObject *objectProperty2 READ objectProperty2 WRITE setObjectProperty2)
    Q_CLASSINFO("DeferredPropertyNames", "value,objectProperty,objectProperty2")

public:
    MyDeferredObject() : m_value(0), m_object(0), m_object2(0) {}

    int value() const { return m_value; }
    void setValue(int v) { m_value = v; emit valueChanged(); }

    QObject *objectProperty() const { return m_object; }
    void setObjectProperty(QObject *obj) { m_object = obj; }

    QObject *objectProperty2() const { return m_object2; }
    void setObjectProperty2(QObject *obj) { m_object2 = obj; }

signals:
    void valueChanged();

private:
    int m_value;
    QObject *m_object;
    QObject *m_object2;
};

class MyBaseExtendedObject : public QObject
{
Q_OBJECT
Q_PROPERTY(int baseProperty READ baseProperty WRITE setBaseProperty)
public:
    MyBaseExtendedObject() : m_value(0) {}

    int baseProperty() const { return m_value; }
    void setBaseProperty(int v) { m_value = v; }

private:
    int m_value;
};

class MyExtendedObject : public MyBaseExtendedObject
{
Q_OBJECT
Q_PROPERTY(int coreProperty READ coreProperty WRITE setCoreProperty)
public:
    MyExtendedObject() : m_value(0) {}

    int coreProperty() const { return m_value; }
    void setCoreProperty(int v) { m_value = v; }

private:
    int m_value;
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

    void doAction() { emit action(); }
signals:
    void action();
    void rectPropertyChanged();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(MyTypeObject::MyFlags)

Q_DECLARE_METATYPE(QScriptValue);
class MyInvokableObject : public QObject
{
    Q_OBJECT
    Q_ENUMS(TestEnum)
public:
    enum TestEnum { EnumValue1, EnumValue2 };
    MyInvokableObject() { reset(); }

    int invoked() const { return m_invoked; }
    bool error() const { return m_invokedError; }
    const QVariantList &actuals() const { return m_actuals; }
    void reset() { m_invoked = -1; m_invokedError = false; m_actuals.clear(); }

    Q_INVOKABLE QPointF method_get_QPointF() { return QPointF(99.3, -10.2); }
    Q_INVOKABLE QPoint method_get_QPoint() { return QPoint(9, 12); }

    Q_INVOKABLE void method_NoArgs() { invoke(0); }
    Q_INVOKABLE int method_NoArgs_int() { invoke(1); return 6; }
    Q_INVOKABLE qreal method_NoArgs_real() { invoke(2); return 19.75; }
    Q_INVOKABLE QPointF method_NoArgs_QPointF() { invoke(3); return QPointF(123, 4.5); }
    Q_INVOKABLE QObject *method_NoArgs_QObject() { invoke(4); return this; }
    Q_INVOKABLE MyInvokableObject *method_NoArgs_unknown() { invoke(5); return this; }
    Q_INVOKABLE QScriptValue method_NoArgs_QScriptValue() { invoke(6); return QScriptValue("Hello world"); }
    Q_INVOKABLE QVariant method_NoArgs_QVariant() { invoke(7); return QVariant("QML rocks"); }

    Q_INVOKABLE void method_int(int a) { invoke(8); m_actuals << a; }
    Q_INVOKABLE void method_intint(int a, int b) { invoke(9); m_actuals << a << b; }
    Q_INVOKABLE void method_real(qreal a) { invoke(10); m_actuals << a; }
    Q_INVOKABLE void method_QString(QString a) { invoke(11); m_actuals << a; }
    Q_INVOKABLE void method_QPointF(QPointF a) { invoke(12); m_actuals << a; }
    Q_INVOKABLE void method_QObject(QObject *a) { invoke(13); m_actuals << qVariantFromValue(a); }
    Q_INVOKABLE void method_QScriptValue(QScriptValue a) { invoke(14); m_actuals << qVariantFromValue(a); }
    Q_INVOKABLE void method_intQScriptValue(int a, QScriptValue b) { invoke(15); m_actuals << a << qVariantFromValue(b); }
    
    Q_INVOKABLE void method_overload(int a) { invoke(16); m_actuals << a; }
    Q_INVOKABLE void method_overload(int a, int b) { invoke(17); m_actuals << a << b; }

    Q_INVOKABLE void method_with_enum(TestEnum e) { invoke(18); m_actuals << (int)e; }

private:
    void invoke(int idx) { if (m_invoked != -1) m_invokedError = true; m_invoked = idx;}
    int m_invoked;
    bool m_invokedError;
    QVariantList m_actuals;
};

class NumberAssignment : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(qreal test1 READ test1 WRITE setTest1);
    qreal _test1;
    qreal test1() const { return _test1; }
    void setTest1(qreal v) { _test1 = v; }

    Q_PROPERTY(qreal test2 READ test2 WRITE setTest2);
    qreal _test2;
    qreal test2() const { return _test2; }
    void setTest2(qreal v) { _test2 = v; }

    Q_PROPERTY(qreal test3 READ test3 WRITE setTest3);
    qreal _test3;
    qreal test3() const { return _test3; }
    void setTest3(qreal v) { _test3 = v; }

    Q_PROPERTY(qreal test4 READ test4 WRITE setTest4);
    qreal _test4;
    qreal test4() const { return _test4; }
    void setTest4(qreal v) { _test4 = v; }

    Q_PROPERTY(int test5 READ test5 WRITE setTest5);
    int _test5;
    int test5() const { return _test5; }
    void setTest5(int v) { _test5 = v; }

    Q_PROPERTY(int test6 READ test6 WRITE setTest6);
    int _test6;
    int test6() const { return _test6; }
    void setTest6(int v) { _test6 = v; }

    Q_PROPERTY(int test7 READ test7 WRITE setTest7);
    int _test7;
    int test7() const { return _test7; }
    void setTest7(int v) { _test7 = v; }

    Q_PROPERTY(int test8 READ test8 WRITE setTest8);
    int _test8;
    int test8() const { return _test8; }
    void setTest8(int v) { _test8 = v; }

    Q_PROPERTY(unsigned int test9 READ test9 WRITE setTest9);
    unsigned int _test9;
    unsigned int test9() const { return _test9; }
    void setTest9(unsigned int v) { _test9 = v; }

    Q_PROPERTY(unsigned int test10 READ test10 WRITE setTest10);
    unsigned int _test10;
    unsigned int test10() const { return _test10; }
    void setTest10(unsigned int v) { _test10 = v; }

    Q_PROPERTY(unsigned int test11 READ test11 WRITE setTest11);
    unsigned int _test11;
    unsigned int test11() const { return _test11; }
    void setTest11(unsigned int v) { _test11 = v; }

    Q_PROPERTY(unsigned int test12 READ test12 WRITE setTest12);
    unsigned int _test12;
    unsigned int test12() const { return _test12; }
    void setTest12(unsigned int v) { _test12 = v; }
};

class DefaultPropertyExtendedObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *firstProperty READ firstProperty WRITE setFirstProperty)
    Q_PROPERTY(QObject *secondProperty READ secondProperty WRITE setSecondProperty)
public:
    DefaultPropertyExtendedObject(QObject *parent = 0) : QObject(parent), m_firstProperty(0), m_secondProperty(0) {}

    QObject *firstProperty() const { return m_firstProperty; }
    QObject *secondProperty() const { return m_secondProperty; }
    void setFirstProperty(QObject *property) { m_firstProperty = property; }
    void setSecondProperty(QObject *property) { m_secondProperty = property; }
private:
    QObject* m_firstProperty;
    QObject* m_secondProperty;
};

class OverrideDefaultPropertyObject : public DefaultPropertyExtendedObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "secondProperty")
public:
    OverrideDefaultPropertyObject() {}
};

void registerTypes();

#endif // TESTTYPES_H

