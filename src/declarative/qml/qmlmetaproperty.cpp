/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlmetaproperty.h"
#include "qmlmetaproperty_p.h"
#include <qml.h>
#include <qfxperf.h>
#include <QStringList>
#include <qmlbindablevalue.h>
#include <qmlcontext.h>
#include "qmlboundsignal_p.h"
#include <math.h>
#include <QtCore/qdebug.h>


QT_BEGIN_NAMESPACE

class QMetaPropertyEx : public QMetaProperty
{
public:
    QMetaPropertyEx()
        : propertyType(-1) {}

    QMetaPropertyEx(const QMetaProperty &p)
        : QMetaProperty(p), propertyType(p.userType()) {}

    QMetaPropertyEx(const QMetaPropertyEx &o)
        : QMetaProperty(o),
          propertyType(o.propertyType) {}

    QMetaPropertyEx &operator=(const QMetaPropertyEx &o)
    {
        static_cast<QMetaProperty *>(this)->operator=(o);
        propertyType = o.propertyType;
        return *this;
    }

    private:
    friend class QmlMetaProperty;
    int propertyType;
};


/*!
    \class QmlMetaProperty
    \brief The QmlMetaProperty class abstracts accessing QML properties.
 */

/*!
    Create an invalid QmlMetaProperty.
*/
QmlMetaProperty::QmlMetaProperty()
: d(new QmlMetaPropertyPrivate)
{
}

/*!
  The destructor deletes its heap data.
 */
QmlMetaProperty::~QmlMetaProperty()
{
    delete d; d = 0;
}

struct CachedPropertyData {
    CachedPropertyData(const QString &n, int pt, int ci)
    : name(n), propType(pt), coreIdx(ci) {}
    QString name;
    int propType;
    int coreIdx;
};

// ### not thread safe
static QHash<const QMetaObject *, CachedPropertyData> qmlCacheDefProp;

/*!
    Creates a QmlMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QmlMetaProperty will be created.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj)
{
    initDefault(obj);
}

/*!
    \internal
    Creates a QmlMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QmlMetaProperty will be created.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->context = ctxt;
    initDefault(obj);
}

void QmlMetaProperty::initDefault(QObject *obj)
{
    if (!obj)
        return;

    d->object = obj;
    QHash<const QMetaObject *, CachedPropertyData>::ConstIterator iter =
        qmlCacheDefProp.find(obj->metaObject());
    if (iter != qmlCacheDefProp.end()) {
        d->name = iter->name;
        d->propType = iter->propType;
        d->coreIdx = iter->coreIdx;
    } else {
        QMetaPropertyEx p(QmlMetaType::defaultProperty(obj));
        d->name = QLatin1String(p.name());
        d->propType = p.propertyType;
        d->coreIdx = p.propertyIndex();
        if (!QObjectPrivate::get(obj)->metaObject)
            qmlCacheDefProp.insert(obj->metaObject(), CachedPropertyData(d->name, d->propType, d->coreIdx));
    }
    if (!d->name.isEmpty()) {
        d->type = Property | Default;
    }
}

/*!
    \internal

    Creates a QmlMetaProperty for the property at index \a idx of \a obj.

    The QmlMetaProperty is assigned category \a cat.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, int idx, PropertyCategory cat, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->context = ctxt;
    d->object = obj;
    d->type = Property;
    d->category = cat;
    QMetaPropertyEx p(obj->metaObject()->property(idx));
    d->propType = p.propertyType;
    d->coreIdx = idx;
    if (p.name() != 0)
        d->name = QLatin1String(p.name());
}

// ### Not thread safe!!!!
static QHash<const QMetaObject *, QHash<QString, CachedPropertyData> > qmlCacheProps;
/*!
    Creates a QmlMetaProperty for the property \a name of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, const QString &name)
: d(new QmlMetaPropertyPrivate)
{
    initProperty(obj, name);
}

/*!
    \internal
    Creates a QmlMetaProperty for the property \a name of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, const QString &name, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->context = ctxt;
    initProperty(obj, name);
}

void QmlMetaProperty::initProperty(QObject *obj, const QString &name)
{
    d->name = name;
    d->object = obj;
    if (name.isEmpty() || !obj)
        return;

    if (name.at(0).isUpper()) {
        // Attached property
        d->attachedFunc = QmlMetaType::attachedPropertiesFuncId(name.toLatin1());
        if (d->attachedFunc != -1)
            d->type  = Property | Attached;
        return;
    } else if (name.count() >= 3 && name.startsWith(QLatin1String("on")) && name.at(2).isUpper()) {
        // Signal
        QString signalName = name.mid(2);
        signalName[0] = signalName.at(0).toLower();

        d->findSignalInt(obj, signalName);
        if (d->signal.signature() != 0) {
            d->type = SignalProperty;
            return;
        }
    } 

    // Property
    QHash<QString, CachedPropertyData> &props = qmlCacheProps[obj->metaObject()];
    QHash<QString, CachedPropertyData>::ConstIterator iter = props.find(name);
    if (iter != props.end()) {
        d->name = iter->name;
        d->propType = iter->propType;
        d->coreIdx = iter->coreIdx;
    } else {
        QMetaPropertyEx p = QmlMetaType::property(obj, name.toLatin1().constData());
        d->name = QLatin1String(p.name());
        d->propType = p.propertyType;
        d->coreIdx = p.propertyIndex();
        if (!QObjectPrivate::get(obj)->metaObject)
            props.insert(name, CachedPropertyData(d->name, d->propType, d->coreIdx));
    }
    if (!d->name.isEmpty())
        d->type = Property;

    if (d->type == Invalid) {
        int sig = findSignal(obj, name.toLatin1());
        if (sig != -1) {
            d->signal = obj->metaObject()->method(sig);
            d->type = Signal;
            d->coreIdx = sig;
        } 
    }
}

/*!
    Create a copy of \a other.
*/
QmlMetaProperty::QmlMetaProperty(const QmlMetaProperty &other)
: d(new QmlMetaPropertyPrivate(*other.d))
{
}

/*!
  \enum QmlMetaProperty::PropertyCategory

  This enum specifies a category of QML property.

  \value Unknown The category is unknown.  This will never be returned from propertyCategory()
  \value InvalidProperty The property is invalid.
  \value Bindable The property is a QmlBindableValue.
  \value List The property is a QList pointer
  \value QmlList The property is a QmlList pointer
  \value Object The property is a QObject derived type pointer
  \value Normal The property is none of the above.
 */

/*!
  \enum QmlMetaProperty::Type

  This enum specifies a type of QML property.

  \value Invalid The property is invalid.
  \value Property The property is a regular Qt property.
  \value SignalProperty The property is a signal property.
  \value Signal The property is a signal.
  \value Default The property is the default property.
  \value Attached The property is an attached property.
*/

/*!
    Returns the property category.
*/
QmlMetaProperty::PropertyCategory QmlMetaProperty::propertyCategory() const
{
    return d->propertyCategory();
}

QmlMetaProperty::PropertyCategory 
QmlMetaPropertyPrivate::propertyCategory() const
{
    if (category == QmlMetaProperty::Unknown) {
        int type = propertyType();
        if (type == QmlMetaProperty::Invalid)
            category = QmlMetaProperty::InvalidProperty;
        else if (type == qMetaTypeId<QmlBindableValue *>())
            category = QmlMetaProperty::Bindable;
        else if (QmlMetaType::isList(type))
            category = QmlMetaProperty::List;
        else if (QmlMetaType::isQmlList(type))
            category = QmlMetaProperty::QmlList;
        else if (QmlMetaType::isObject(type))
            category = QmlMetaProperty::Object;
        else 
            category = QmlMetaProperty::Normal;
    }
    return category;
}

/*!
    Returns the property category of \a prop.
*/
QmlMetaProperty::PropertyCategory 
QmlMetaProperty::propertyCategory(const QMetaProperty &prop)
{
    if (prop.name()) {
        int type = 0;
        if (prop.type() == QVariant::LastType)
            type = qMetaTypeId<QVariant>();
        else if (prop.type() == QVariant::UserType)
            type = prop.userType();
        else
            type = prop.type();

        if (type == qMetaTypeId<QmlBindableValue *>())
            return Bindable;
        else if (QmlMetaType::isList(type))
            return List;
        else if (QmlMetaType::isQmlList(type))
            return QmlList;
        else if (QmlMetaType::isObject(type))
            return Object;
        else 
            return Normal;
    } else {
        return InvalidProperty;
    }
}

/*!
    Returns the type name of the property, or 0 if the property has no type
    name.
*/
const char *QmlMetaProperty::propertyTypeName() const
{
    if (!d->name.isEmpty()) {
        return d->object->metaObject()->property(d->coreIdx).typeName();
    } else {
        return 0;
    }
}

/*!
    Returns true if \a other and this QmlMetaProperty represent the same 
    property.
*/
bool QmlMetaProperty::operator==(const QmlMetaProperty &other) const
{
    return d->name == other.d->name &&
           d->signal.signature() == other.d->signal.signature() &&
           d->type == other.d->type &&
           d->object == other.d->object;
}

/*!
    Returns the QVariant type of the property, or QVariant::Invalid if the 
    property has no QVariant type.
*/
int QmlMetaProperty::propertyType() const
{
    return d->propertyType();
}

int QmlMetaPropertyPrivate::propertyType() const
{
    int rv = QVariant::Invalid;

    if (!name.isEmpty()) {
        if (propType == (int)QVariant::LastType)
            rv = qMetaTypeId<QVariant>();
        else
            rv = propType;
    } else if (attachedFunc) {
        rv = qMetaTypeId<QObject *>();
    } 

    return rv;
}

/*!
    Returns the type of the property.
*/
QmlMetaProperty::Type QmlMetaProperty::type() const
{
    return (Type)d->type;
}

/*!
    Returns true if this QmlMetaProperty represents a regular Qt property.
*/
bool QmlMetaProperty::isProperty() const
{
    return type() & Property;
}

/*!
    Returns true if this QmlMetaProperty represents a default property.
*/
bool QmlMetaProperty::isDefault() const
{
    return type() & Default;
}

/*!
    Returns the QmlMetaProperty's QObject.
*/
QObject *QmlMetaProperty::object() const
{
    return d->object;
}

/*!
    Assign \a other to this QmlMetaProperty.
*/
QmlMetaProperty &QmlMetaProperty::operator=(const QmlMetaProperty &other)
{
    d->name = other.d->name;
    d->propType = other.d->propType;
    d->type = other.d->type;
    d->signal = other.d->signal;
    d->coreIdx = other.d->coreIdx;
    d->attachedFunc = other.d->attachedFunc;
    d->object = other.d->object;
    d->category = other.d->category;
    return *this;
}

/*!
    Returns true if the property is writable, otherwise false.
*/
bool QmlMetaProperty::isWritable() const
{
    if (propertyCategory() == List || propertyCategory() == QmlList)
        return true;
    else if (!d->name.isEmpty())
        return d->object->metaObject()->property(d->coreIdx).isWritable();
    else if (type() & SignalProperty)
        return true;
    else
        return false;
}

/*!
    Returns true if the property is designable, otherwise false.
*/
bool QmlMetaProperty::isDesignable() const
{
    if (!d->name.isEmpty())
        return d->object->metaObject()->property(d->coreIdx).isDesignable();
    else
        return false;
}

/*!
    Returns true if the QmlMetaProperty refers to a valid property, otherwise
    false.
*/
bool QmlMetaProperty::isValid() const
{
    return type() != Invalid;
}

/*!
    Returns all of \a obj's Qt properties.
*/
QStringList QmlMetaProperty::properties(QObject *obj)
{
    if (!obj)
        return QStringList();

    QStringList rv;
    const QMetaObject *mo = obj->metaObject();
    for (int ii = 0; ii < mo->propertyCount(); ++ii) {
        QMetaProperty prop = mo->property(ii);
        rv << QLatin1String(prop.name());
    }

    return rv;
}

/*!
    Return the name of this QML property.
*/
QString QmlMetaProperty::name() const
{
    return d->name;
}

/*!
  Returns the \l{QMetaProperty} {Qt property} associated with
  this QML property.
 */
QMetaProperty QmlMetaProperty::property() const
{
    return d->object->metaObject()->property(d->coreIdx);
}

/*!
    Returns the binding associated with this property, or 0 if no binding 
    exists.
*/
QmlBindableValue *QmlMetaProperty::binding()
{
    if (!isProperty() || type() & Attached)
        return 0;

    const QObjectList &children = object()->children();
    for (QObjectList::ConstIterator iter = children.begin();
            iter != children.end(); ++iter) {
        QObject *child = *iter;
        if (child->metaObject() == &QmlBindableValue::staticMetaObject) {
            QmlBindableValue *v = static_cast<QmlBindableValue *>(child);
            if (v->property() == *this)
                return v;
        }
    }
    return 0;
}

/*! \internal */
int QmlMetaProperty::findSignal(const QObject *obj, const char *name)
{
    const QMetaObject *mo = obj->metaObject();
    int methods = mo->methodCount();
    for (int ii = 0; ii < methods; ++ii) {
        QMetaMethod method = mo->method(ii);
        if (method.methodType() != QMetaMethod::Signal)
            continue;

        QByteArray methodName = method.signature();
        int idx = methodName.indexOf('(');
        methodName = methodName.left(idx);

        if (methodName == name) 
            return ii;
    }
    return -1;
}

void QmlMetaPropertyPrivate::findSignalInt(QObject *obj, const QString &name)
{
    const QMetaObject *mo = obj->metaObject();

    int methods = mo->methodCount();
    for (int ii = 0; ii < methods; ++ii) {
        QMetaMethod method = mo->method(ii);
        QString methodName = QLatin1String(method.signature());
        int idx = methodName.indexOf(QLatin1Char('('));
        methodName = methodName.left(idx);

        if (methodName == name) {
            signal = method;
            coreIdx = ii;
            return;
        }
    }
}

QObject *QmlMetaPropertyPrivate::attachedObject() const
{
    if (attachedFunc == -1)
        return 0;
    else
        return qmlAttachedPropertiesObjectById(attachedFunc, object);
}

/*!
    Returns the property value.
*/
QVariant QmlMetaProperty::read() const
{
    if (type() & SignalProperty) {

        const QObjectList &children = object()->children();

        for (int ii = 0; ii < children.count(); ++ii) {
            QmlBoundSignal *sig = qobject_cast<QmlBoundSignal *>(children.at(ii));
            if (sig && sig->index() == d->coreIdx) 
                return sig->expression();
        }
    } else if (type() & Property) {
        if (type() & Attached) 
            return QVariant::fromValue(d->attachedObject());
        else
            return d->object->metaObject()->property(d->coreIdx).read(object());
    }
    return QVariant();
}

Q_DECLARE_METATYPE(QList<QObject *>);

void QmlMetaPropertyPrivate::writeSignalProperty(const QVariant &value)
{
    QString expr = value.toString();
    const QObjectList &children = object->children();

    for (int ii = 0; ii < children.count(); ++ii) {
        QmlBoundSignal *sig = qobject_cast<QmlBoundSignal *>(children.at(ii));
        if (sig && sig->index() == coreIdx) {
            if (expr.isEmpty()) {
                sig->disconnect();
                sig->deleteLater();
            } else {
                sig->setExpression(expr);
            }
            return;
        }
    }

    if (!expr.isEmpty()) {
        // XXX scope
        (void *)new QmlBoundSignal(QmlContext::activeContext(), expr, object, 
                                   coreIdx, object);
    }
}

void QmlMetaPropertyPrivate::writeValueProperty(const QVariant &value)
{
    QMetaProperty prop = object->metaObject()->property(coreIdx);
    if (prop.isEnumType()) {
        QVariant v = value;
        if (value.type() == QVariant::Double) {   //enum values come through the script engine as doubles
            double integral;
            double fractional = modf(value.toDouble(), &integral);
            if (qFuzzyCompare(fractional, (double)0.0))
                v.convert(QVariant::Int);
        }
        prop.write(object, v);
        return;
    }

    if (!value.isValid())
        return;

    int t = propertyType();
    int vt = value.userType();
    int category = propertyCategory();

    if (vt == t) {

        void *a[1];
        a[0] = (void *)value.constData();
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (qMetaTypeId<QVariant>() == t) {

        prop.write(object, value);

    } else if (category == QmlMetaProperty::Object) {

        QObject *o = QmlMetaType::toQObject(value);

        if (!o)
            return;

        const QMetaObject *valMo = o->metaObject();
        const QMetaObject *propMo = QmlMetaType::rawMetaObjectForType(t);

        while (valMo) {
            if (valMo == propMo)
                break;
            valMo = valMo->superClass();
        }

        if (valMo) {

            void *args[] = { &o, 0 };
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, 
                                  args);

        }

    } else if (category == QmlMetaProperty::List) {

        int listType = QmlMetaType::listType(t);

        if (value.userType() == qMetaTypeId<QList<QObject *> >()) {
            const QList<QObject *> &list =
                qvariant_cast<QList<QObject *> >(value);
            QVariant listVar = prop.read(object);
            QmlMetaType::clear(listVar);
            for (int ii = 0; ii < list.count(); ++ii) {
                QVariant v = QmlMetaType::fromObject(list.at(ii), listType);
                QmlMetaType::append(listVar, v);
            }

        } else if (vt == listType ||
                  value.userType() == listType) {
            QVariant listVar = prop.read(object);
            QmlMetaType::clear(listVar);
            QmlMetaType::append(listVar, value);
        }
    } else if (category == QmlMetaProperty::QmlList) {

        // XXX - optimize!
        QVariant list = prop.read(object);
        QmlPrivate::ListInterface *li =
            *(QmlPrivate::ListInterface **)list.constData();

        int type = li->type();

        if (QObject *obj = QmlMetaType::toQObject(value)) {
            const QMetaObject *mo =
                QmlMetaType::rawMetaObjectForType(type);

            const QMetaObject *objMo = obj->metaObject();
            bool found = false;
            while(!found && objMo) {
                if (objMo == mo)
                    found = true;
                else
                    objMo = objMo->superClass();
            }

            if (!found) {
                qWarning() << "Unable to assign object to list";
                return;
            }

            // NOTE: This assumes a cast to QObject does not alter
            // the object pointer
            void *d = (void *)&obj;
            li->append(d);
        }
    } else if (category == QmlMetaProperty::Normal) {

        switch(t) {
        case QVariant::Double:
            {
                double d;
                bool found = true;
                if (vt == QVariant::Int) {
                    d = value.toInt();
                } else if (vt == QVariant::UInt) {
                    d = value.toUInt();
                } else {
                    found = false;
                }

                if (found) {
                    void *a[1];
                    a[0] = &d;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
                    return;
                }
            }
            break;

        case QVariant::Int:
            {
                int i;
                bool found = true;
                if (vt == QVariant::Double) {
                    i = (int)value.toDouble();
                } else if (vt == QVariant::UInt) {
                    i = (int)value.toUInt();
                } else {
                    found = false;
                }

                if (found) {
                    void *a[1];
                    a[0] = &i;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
                    return;
                }
            }
            break;

        case QVariant::String:
            {
                QString s;
                bool found = true;
                if (vt == QVariant::ByteArray) {
                    s = QLatin1String(value.toByteArray());
                } else {
                    found = false;
                }

                if (found) {
                    void *a[1];
                    a[0] = &s;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
                    return;
                }
            }
            break;


        default:
            {
                if ((uint)t >= QVariant::UserType && vt == QVariant::String) {
                    QmlMetaType::StringConverter con = QmlMetaType::customStringConverter(t);
                    if (con) {
                        QVariant v = con(value.toString());
                        prop.write(object, v);
                        return;
                    }
                }
            }
            break;
        }
        prop.write(object, value);
    }

}

/*!
    Set the property value to \a value.
*/
void QmlMetaProperty::write(const QVariant &value) const
{
    QMetaProperty prop = d->object->metaObject()->property(d->coreIdx);
    if (type() & SignalProperty) {

        d->writeSignalProperty(value);

    } else if (prop.name()) {

        d->writeValueProperty(value);

    }
}

/*!
    Returns true if the property has a change notifier signal, otherwise false.
*/
bool QmlMetaProperty::hasChangedNotifier() const
{
    if (type() & Property && !(type() & Attached)) {
        return d->object->metaObject()->property(d->coreIdx).hasNotifySignal();
    }
    return false;
}

/*!
    Returns true if the property needs a change notifier signal for bindings
    to remain upto date, false otherwise.

    Some properties, such as attached properties or those whose value never 
    changes, do not require a change notifier.
*/
bool QmlMetaProperty::needsChangedNotifier() const
{
    return type() & Property && !(type() & Attached);
}

/*!
    Connects the property's change notifier signal to the
    specified \a method of the \a dest object and returns
    true. Returns false if this metaproperty does not
    represent a regular Qt property or if it has no
    change notifier signal, or if the \a dest object does
    not have the specified \a method.
*/
bool QmlMetaProperty::connectNotifier(QObject *dest, int method) const
{
    if (!(type() & Property) || type() & Attached)
        return false;

    QMetaProperty prop = d->object->metaObject()->property(d->coreIdx);
    if (prop.hasNotifySignal()) {
        return QMetaObject::connect(d->object, prop.notifySignalIndex(), dest, method, Qt::DirectConnection);
    } else {
        return false;
    }
}

/*!
    Connects the property's change notifier signal to the
    specified \a slot of the \a dest object and returns
    true. Returns false if this metaproperty does not
    represent a regular Qt property or if it has no
    change notifier signal, or if the \a dest object does
    not have the specified \a slot.
*/
bool QmlMetaProperty::connectNotifier(QObject *dest, const char *slot) const
{
    if (!(type() & Property) || type() & Attached)
        return false;

    QMetaProperty prop = d->object->metaObject()->property(d->coreIdx);
    if (prop.hasNotifySignal()) {
        QByteArray signal(QByteArray("2") + prop.notifySignal().signature());
        return QObject::connect(d->object, signal.constData(), dest, slot);
    } else  {
        return false;
    }
}

/*! \internal */
void QmlMetaProperty::emitSignal()
{
    if (type() & Signal) {
        if (d->signal.parameterTypes().isEmpty())
            d->object->metaObject()->activate(d->object, d->coreIdx, 0);
        else
            qWarning() << "QmlMetaProperty: Cannot emit signal with parameters";
    }
}

/*!
    Return the Qt metaobject index of the property.
*/
int QmlMetaProperty::coreIndex() const
{
    return d->coreIdx;
}

/*!
    Returns the property information serialized into a single integer.  
    QmlMetaProperty uses the bottom 24 bits only.
*/
quint32 QmlMetaProperty::save() const
{
    quint32 rv = 0;
    if (type() & Attached) {
        rv = d->attachedFunc;
    } else if (type() != Invalid) {
        rv = d->coreIdx;
    }

    Q_ASSERT(rv <= 0xFFFF);
    Q_ASSERT(type() <= 0xFF);
    rv |= (type() << 16);

    return rv;
}

/*!
  Restore a QmlMetaProperty from a previously saved \a id.
  \a obj must be the same object as used in the previous call
  to QmlMetaProperty::save().  Only the bottom 24-bits are
  used, the high bits can be set to any value.
*/
void QmlMetaProperty::restore(quint32 id, QObject *obj)
{
    *this = QmlMetaProperty();
    d->object = obj;

    id &= 0xFFFFFF;
    d->type = id >> 16;
    id &= 0xFFFF;

    if (d->type & Attached) {
        d->attachedFunc = id;
    } else if (d->type & Property) {
        QMetaPropertyEx p(obj->metaObject()->property(id));
        d->name = QLatin1String(p.name());
        d->propType = p.propertyType;
        d->coreIdx = id;
    } else if (d->type & SignalProperty || d->type & Signal) {
        d->signal = obj->metaObject()->method(id);
        d->coreIdx = id;
    } 
}

/*!
    Return the QMetaMethod for this property if it is a SignalProperty, 
    otherwise returns an invalid QMetaMethod.
*/
QMetaMethod QmlMetaProperty::method() const
{
    return d->signal;
}

QT_END_NAMESPACE
