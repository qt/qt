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
#include <private/qfxperf_p.h>
#include <QStringList>
#include "qmlbinding.h"
#include <qmlcontext.h>
#include "qmlboundsignal_p.h"
#include <math.h>
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlengine.h>
#include <private/qmlengine_p.h>

Q_DECLARE_METATYPE(QList<QObject *>);

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
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, int idx, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->context = ctxt;
    d->object = obj;
    d->type = Property;
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
  \value Bindable The property is a QmlBinding.
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
        else if (type == qMetaTypeId<QmlBinding *>())
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
    Returns the type name of the property, or 0 if the property has no type
    name.
*/
const char *QmlMetaProperty::propertyTypeName() const
{
    if (!d->name.isEmpty() && d->object) {
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
    d->signal = other.d->signal;
    d->context = other.d->context;
    d->coreIdx = other.d->coreIdx;
    d->valueTypeIdx = other.d->valueTypeIdx;
    d->valueTypeId = other.d->valueTypeId;
    d->type = other.d->type;
    d->attachedFunc = other.d->attachedFunc;
    d->object = other.d->object;
    d->propType = other.d->propType;
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
    else if (!d->name.isEmpty() && d->object)
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
    if (!d->name.isEmpty() && d->object)
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
    if (d->object)
        return d->object->metaObject()->property(d->coreIdx);
    else
        return QMetaProperty();
}

/*!
    Returns the binding associated with this property, or 0 if no binding 
    exists.
*/
QmlBinding *QmlMetaProperty::binding() const
{
    if (!isProperty() || (type() & Attached) || !d->object)
        return 0;

    const QObjectList &children = object()->children();
    for (QObjectList::ConstIterator iter = children.begin();
            iter != children.end(); ++iter) {
        QObject *child = *iter;
        if (child->metaObject() == &QmlBinding::staticMetaObject) {
            QmlBinding *v = static_cast<QmlBinding *>(child);
            if (v->property() == *this)
                return v;
        }
    }
    return 0;
}

/*!
    Set the binding associated with this property to \a binding.  Returns
    the existing binding (if any), otherwise 0.

    \a binding will be enabled, and the returned binding (if any) will be
    disabled.
*/
QmlBinding *QmlMetaProperty::setBinding(QmlBinding *binding) const
{
    if (!isProperty() || (type() & Attached) || !d->object)
        return 0;

    const QObjectList &children = object()->children();
    for (QObjectList::ConstIterator iter = children.begin();
            iter != children.end(); ++iter) {
        QObject *child = *iter;
        if (child->metaObject() == &QmlBinding::staticMetaObject) {
            QmlBinding *v = static_cast<QmlBinding *>(child);
            if (v->property() == *this && v->enabled()) {

                v->setEnabled(false);

                if (binding) {
                    binding->setParent(object());
                    binding->setTarget(*this);
                    binding->setEnabled(true);
                    binding->forceUpdate();
                }

                return v;

            }
        }
    }

    if (binding) {
        binding->setParent(object());
        binding->setTarget(*this);
        binding->setEnabled(true);
        binding->forceUpdate();
    }

    return 0;
}

void QmlMetaPropertyPrivate::findSignalInt(QObject *obj, const QString &name)
{
    const QMetaObject *mo = obj->metaObject();

    int methods = mo->methodCount();
    for (int ii = methods - 1; ii >= 0; --ii) {
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
    if (!d->object)
        return QVariant();

    if (type() & SignalProperty) {

        const QObjectList &children = object()->children();

        for (int ii = 0; ii < children.count(); ++ii) {
            QmlBoundSignal *sig = qobject_cast<QmlBoundSignal *>(children.at(ii));
            if (sig && sig->index() == d->coreIdx) 
                return sig->expression();
        }
    } else if (type() & Property) {
        if (type() & Attached) {
            return QVariant::fromValue(d->attachedObject());
        } else if(type() & ValueTypeProperty) {
            QmlEnginePrivate *ep = d->context?static_cast<QmlEnginePrivate *>(QObjectPrivate::get(d->context->engine())):0;
            QmlValueType *valueType = 0;
            if (ep)
                valueType = ep->valueTypes[d->valueTypeId];
            else
                valueType = QmlValueTypeFactory::valueType(d->valueTypeId);
            Q_ASSERT(valueType);

            valueType->read(object(), d->coreIdx);
            QVariant rv =
                valueType->metaObject()->property(d->valueTypeIdx).read(valueType);
            if (!ep)
                delete valueType;
            return rv;

        } else {
            return d->object->metaObject()->property(d->coreIdx).read(object());
        }
    }
    return QVariant();
}

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
        (void *)new QmlBoundSignal(qmlContext(object), expr, object, coreIdx, object);
    }
}

void QmlMetaPropertyPrivate::writeValueProperty(const QVariant &value)
{
    QObject *object = this->object;
    int coreIdx = this->coreIdx;
    
    QmlValueType *writeBack = 0;
    QObject *writeBackObj = 0;
    int writeBackIdx = -1;
    bool deleteWriteBack = false;

    if (type & QmlMetaProperty::ValueTypeProperty) {
        QmlEnginePrivate *ep = context?static_cast<QmlEnginePrivate *>(QObjectPrivate::get(context->engine())):0;

        if (ep) {
            writeBack = ep->valueTypes[valueTypeId];
        } else {
            writeBack = QmlValueTypeFactory::valueType(valueTypeId);
            deleteWriteBack = true;
        }

        writeBackObj = this->object;
        writeBackIdx = this->coreIdx;
        writeBack->read(writeBackObj, writeBackIdx);
        object = writeBack;
        coreIdx = valueTypeIdx;
    }

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

        if (writeBack) {
            writeBack->write(writeBackObj, writeBackIdx);
            if (deleteWriteBack) delete writeBack;
        }
        return;
    }

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

        const QMetaObject *valMo = 0;

        if (o) {

            valMo = o->metaObject();
            const QMetaObject *propMo = QmlMetaType::rawMetaObjectForType(t);

            while (valMo) {
                if (valMo == propMo)
                    break;
                valMo = valMo->superClass();
            }

        }

        if (valMo || !o) {

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

        bool found = false;
        switch(t) {
        case QVariant::Double:
            {
                double d;
                if (vt == QVariant::Int) {
                    d = value.toInt();
                    found = true;
                } else if (vt == QVariant::UInt) {
                    d = value.toUInt();
                    found = true;
                } 

                if (found) {
                    void *a[1];
                    a[0] = &d;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
                }
            }
            break;

        case QVariant::Int:
            {
                int i;
                if (vt == QVariant::Double) {
                    i = (int)value.toDouble();
                    found = true;
                } else if (vt == QVariant::UInt) {
                    i = (int)value.toUInt();
                    found = true;
                } 

                if (found) {
                    void *a[1];
                    a[0] = &i;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
                }
            }
            break;

        case QVariant::String:
            {
                QString s;
                if (vt == QVariant::ByteArray) {
                    s = QLatin1String(value.toByteArray());
                    found = true;
                } 

                if (found) {
                    void *a[1];
                    a[0] = &s;
                    QMetaObject::metacall(object, 
                                          QMetaObject::WriteProperty,
                                          coreIdx, a);
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
                        found = true;
                    }
                }
            }
            break;
        }
        if (!found)
            prop.write(object, value);
    }

    if (writeBack) {
        writeBack->write(writeBackObj, writeBackIdx);
        if (deleteWriteBack) delete writeBack;
    }
}

/*!
    Set the property value to \a value.
*/
void QmlMetaProperty::write(const QVariant &value) const
{
    if (!d->object)
        return;

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
    if (type() & Property && !(type() & Attached) && d->object) {
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
    return type() & Property && !(type() & Attached) && 
           !property().isConstant();
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
    if (!(type() & Property) || (type() & Attached) || !d->object)
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
    if (!(type() & Property) || (type() & Attached) || !d->object)
        return false;

    QMetaProperty prop = d->object->metaObject()->property(d->coreIdx);
    if (prop.hasNotifySignal()) {
        QByteArray signal(QByteArray("2") + prop.notifySignal().signature());
        return QObject::connect(d->object, signal.constData(), dest, slot);
    } else  {
        return false;
    }
}

/*!
    Return the Qt metaobject index of the property.
*/
int QmlMetaProperty::coreIndex() const
{
    return d->coreIdx;
}

Q_GLOBAL_STATIC(QmlValueTypeFactory, qmlValueTypes);

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

    Q_ASSERT(rv <= 0x7FF);
    Q_ASSERT(type() <= 0x3F);
    Q_ASSERT(d->valueTypeIdx <= 0x7F);

    rv |= (type() << 18);

    if (type() & ValueTypeProperty)
        rv |= (d->valueTypeIdx << 11);

    return rv;
}

quint32 QmlMetaPropertyPrivate::saveValueType(int core, int valueType)
{
    Q_ASSERT(core <= 0x7FF);
    Q_ASSERT(valueType <= 0x7F);
    quint32 rv = 0;
    rv = (QmlMetaProperty::ValueTypeProperty | QmlMetaProperty::Property) << 18;
    rv |= core;
    rv |= valueType << 11;
    return rv;
}

quint32 QmlMetaPropertyPrivate::saveProperty(int core)
{
    Q_ASSERT(core <= 0x7FF);
    quint32 rv = 0;
    rv = (QmlMetaProperty::Property) << 18;
    rv |= core;
    return rv;
}

/*!
  Restore a QmlMetaProperty from a previously saved \a id.
  \a obj must be the same object as used in the previous call
  to QmlMetaProperty::save().  Only the bottom 24-bits are
  used, the high bits can be set to any value.
*/
void QmlMetaProperty::restore(quint32 id, QObject *obj, QmlContext *ctxt)
{
    d->object = obj;
    d->context = ctxt;

    id &= 0xFFFFFF;
    d->type = id >> 18;
    id &= 0xFFFF;

    if (d->type & Attached) {
        d->attachedFunc = id;
    } else if (d->type & ValueTypeProperty) {
        int coreIdx = id & 0x7FF;
        int valueTypeIdx = id >> 11;

        QMetaProperty p(obj->metaObject()->property(coreIdx));
        Q_ASSERT(p.type() < QVariant::UserType);

        QmlValueType *valueType = qmlValueTypes()->valueTypes[p.type()];

        QMetaPropertyEx p2(valueType->metaObject()->property(valueTypeIdx));

        d->name = QLatin1String(p2.name());
        d->propType = p2.propertyType;
        d->coreIdx = coreIdx;
        d->valueTypeIdx = valueTypeIdx;
        d->valueTypeId = p.type();

    } else if (d->type & Property) {
        QMetaPropertyEx p(obj->metaObject()->property(id));
        d->name = QLatin1String(p.name());
        d->propType = p.propertyType;
        d->coreIdx = id;
    } else if (d->type & SignalProperty) {
        d->signal = obj->metaObject()->method(id);
        d->coreIdx = id;
    } else {
        *this = QmlMetaProperty();
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

/*!
    \internal

    Creates a QmlMetaProperty for the property \a name of \a obj. Unlike
    the QmlMetaProperty(QObject*, QString) constructor, this static function
    will correctly handle dot properties.
*/
QmlMetaProperty QmlMetaProperty::createProperty(QObject *obj, 
                                                const QString &name)
{
    QStringList path = name.split(QLatin1Char('.'));

    QObject *object = obj;

    for (int jj = 0; jj < path.count() - 1; ++jj) {
        const QString &pathName = path.at(jj);
        QmlMetaProperty prop(object, pathName);

        if (jj == path.count() - 2 && 
            prop.propertyType() < QVariant::UserType &&
            qmlValueTypes()->valueTypes[prop.propertyType()]) {
            // We're now at a value type property
            QObject *typeObject = 
                qmlValueTypes()->valueTypes[prop.propertyType()];
            int idx = typeObject->metaObject()->indexOfProperty(path.last().toUtf8().constData());
            if (idx == -1)
                return QmlMetaProperty();

            QmlMetaProperty p;
            p.d->name = pathName + QLatin1String(".") + path.last();
            p.d->context = 0;
            p.d->coreIdx = prop.coreIndex();
            p.d->valueTypeIdx = idx;
            p.d->valueTypeId = prop.propertyType();
            p.d->type = QmlMetaProperty::ValueTypeProperty | 
                        QmlMetaProperty::Property;
            p.d->object = obj;
            p.d->propType = typeObject->metaObject()->property(idx).userType();
            return p;
        }

        QObject *objVal = QmlMetaType::toQObject(prop.read());
        if (!objVal)
            return QmlMetaProperty();
        object = objVal;
    }

    const QString &propName = path.last();
    QmlMetaProperty prop(object, propName);
    if (!prop.isValid())
        return QmlMetaProperty();
    else
        return prop;
}

QT_END_NAMESPACE
