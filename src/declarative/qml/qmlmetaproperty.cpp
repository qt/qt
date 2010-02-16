/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlmetaproperty.h"
#include "qmlmetaproperty_p.h"

#include "qmlcompositetypedata_p.h"
#include "qml.h"
#include "qmlbinding.h"
#include "qmlcontext.h"
#include "qmlcontext_p.h"
#include "qmlboundsignal_p.h"
#include "qmlengine.h"
#include "qmlengine_p.h"
#include "qmldeclarativedata_p.h"
#include "qmlstringconverters_p.h"

#include <qfxperf_p_p.h>

#include <QStringList>
#include <QtCore/qdebug.h>

#include <math.h>

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

/*!
    \class QmlMetaProperty
    \brief The QmlMetaProperty class abstracts accessing QML properties.
    \internal
 */

/*!
    Create an invalid QmlMetaProperty.
*/
QmlMetaProperty::QmlMetaProperty()
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
}

/*!  \internal */
QmlMetaProperty::~QmlMetaProperty()
{
    delete d; d = 0;
}

/*!
    Creates a QmlMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QmlMetaProperty will be created.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj)
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
    d->initDefault(obj);
}

/*!
    \internal
    Creates a QmlMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QmlMetaProperty will be created.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
    d->context = ctxt;
    d->initDefault(obj);
}

/*!
    Initialize from the default property of \a obj
*/
void QmlMetaPropertyPrivate::initDefault(QObject *obj)
{
    if (!obj)
        return;

    QMetaProperty p = QmlMetaType::defaultProperty(obj);
    core.load(p);
    if (core.isValid()) {
        isDefaultProperty = true;
        object = obj;
    }
}

/*!
    Creates a QmlMetaProperty for the property \a name of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, const QString &name)
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
    d->initProperty(obj, name);
    if (!isValid()) d->object = 0;
}

/*!
    \internal
    Creates a QmlMetaProperty for the property \a name of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, const QString &name, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
    d->context = ctxt;
    d->initProperty(obj, name);
    if (!isValid()) { d->object = 0; d->context = 0; }
}

void QmlMetaPropertyPrivate::initProperty(QObject *obj, const QString &name)
{
    QmlEnginePrivate *enginePrivate = 0;
    if (context && context->engine())
        enginePrivate = QmlEnginePrivate::get(context->engine());

    object = obj;

    if (name.isEmpty() || !obj)
        return;

    if (enginePrivate && name.at(0).isUpper()) {
        // Attached property
        // ### What about qualified types?
        QmlTypeNameCache *tnCache = QmlContextPrivate::get(context)->imports;
        if (tnCache) {
            QmlTypeNameCache::Data *d = tnCache->data(name);
            if (d && d->type && d->type->attachedPropertiesFunction()) {
                attachedFunc = d->type->index();
            }
        }
        return;

    } else if (name.count() >= 3 && 
               name.at(0) == QChar(QLatin1Char('o')) && 
               name.at(1) == QChar(QLatin1Char('n')) && 
               name.at(2).isUpper()) {
        // Signal
        QString signalName = name.mid(2);
        signalName[0] = signalName.at(0).toLower();

        QMetaMethod method = findSignal(obj, signalName);
        if (method.signature()) {
            core.load(method);
            return;
        }
    } 

    // Property
    QmlPropertyCache::Data local;
    QmlPropertyCache::Data *property = 
        QmlPropertyCache::property(context?context->engine():0, obj, name, local);
    if (property && !(property->flags & QmlPropertyCache::Data::IsFunction)) 
        core = *property;
}

/*!
    Create a copy of \a other.
*/
QmlMetaProperty::QmlMetaProperty(const QmlMetaProperty &other)
: d(new QmlMetaPropertyPrivate(*other.d))
{
    d->q = this;
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
    uint type = q->type();

    if (type & QmlMetaProperty::ValueTypeProperty) {
        return QmlMetaProperty::Normal;
    } else if (type & QmlMetaProperty::Attached) {
        return QmlMetaProperty::Object;
    } else if (type & QmlMetaProperty::Property) {
        int type = propertyType();
        if (type == QVariant::Invalid)
            return QmlMetaProperty::InvalidProperty;
        else if ((uint)type < QVariant::UserType)
            return QmlMetaProperty::Normal;
        else if (type == qMetaTypeId<QmlBinding *>())
            return QmlMetaProperty::Bindable;
        else if (core.flags & QmlPropertyCache::Data::IsQObjectDerived)
            return QmlMetaProperty::Object;
        else if (core.flags & QmlPropertyCache::Data::IsQmlList)
            return QmlMetaProperty::QmlList;
        else if (core.flags & QmlPropertyCache::Data::IsQList)
            return QmlMetaProperty::List;
        else 
            return QmlMetaProperty::Normal;
    } else {
        return QmlMetaProperty::InvalidProperty;
    }
}

/*!
    Returns the type name of the property, or 0 if the property has no type
    name.
*/
const char *QmlMetaProperty::propertyTypeName() const
{
    if (type() & ValueTypeProperty) {

        QmlEnginePrivate *ep = d->context?QmlEnginePrivate::get(d->context->engine()):0;
        QmlValueType *valueType = 0;
        if (ep) valueType = ep->valueTypes[d->core.propType];
        else valueType = QmlValueTypeFactory::valueType(d->core.propType);
        Q_ASSERT(valueType);

        const char *rv = valueType->metaObject()->property(d->valueType.valueTypeCoreIdx).typeName();

        if (!ep) delete valueType;

        return rv;
    } else if (d->object && type() & Property && d->core.isValid()) {
        return d->object->metaObject()->property(d->core.coreIndex).typeName();
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
    // category is intentially omitted here as it is generated 
    // from the other members
    return d->object == other.d->object &&
           d->core == other.d->core &&
           d->valueType == other.d->valueType &&
           d->attachedFunc == other.d->attachedFunc;
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
    uint type = q->type();
    if (type & QmlMetaProperty::ValueTypeProperty) {
        return valueType.valueTypePropType;
    } else if (type & QmlMetaProperty::Attached) {
        return qMetaTypeId<QObject *>();
    } else if (type & QmlMetaProperty::Property) {
        if (core.propType == (int)QVariant::LastType)
            return qMetaTypeId<QVariant>();
        else
            return core.propType;
    } else {
        return QVariant::Invalid;
    }
}

/*!
    Returns the type of the property.
*/
QmlMetaProperty::Type QmlMetaProperty::type() const
{
    if (d->core.flags & QmlPropertyCache::Data::IsFunction)
        return SignalProperty;
    else if (d->attachedFunc != -1)
        return Attached;
    else if (d->valueType.valueTypeCoreIdx != -1)
        return (Type)(Property | ValueTypeProperty);
    else if (d->core.isValid())
        return (Type)(Property | ((d->isDefaultProperty)?Default:0));
    else
        return Invalid;
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
    d->context = other.d->context;
    d->object = other.d->object;

    d->isDefaultProperty = other.d->isDefaultProperty;
    d->core = other.d->core;

    d->valueType = other.d->valueType;

    d->attachedFunc = other.d->attachedFunc;
    return *this;
}

/*!
    Returns true if the property is writable, otherwise false.
*/
bool QmlMetaProperty::isWritable() const
{
    QmlMetaProperty::PropertyCategory category = propertyCategory();

    if (!d->object)
        return false;
    if (category == List || category == QmlList)
        return true;
    else if (type() & SignalProperty)
        return false;
    else if (d->core.isValid() && d->object)
        return d->core.flags & QmlPropertyCache::Data::IsWritable;
    else
        return false;
}

/*!
    Returns true if the property is designable, otherwise false.
*/
bool QmlMetaProperty::isDesignable() const
{
    if (type() & Property && d->core.isValid() && d->object)
        return d->object->metaObject()->property(d->core.coreIndex).isDesignable();
    else
        return false;
}

/*!
    Returns true if the property is resettable, otherwise false.
*/
bool QmlMetaProperty::isResettable() const
{
    if (type() & Property && d->core.isValid() && d->object)
        return d->core.flags & QmlPropertyCache::Data::IsResettable;
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
    Return the name of this QML property.
*/
QString QmlMetaProperty::name() const
{
    if (!d->isNameCached) {
        // ###
        if (!d->object) {
        } else if (type() & ValueTypeProperty) {
            QString rv = d->core.name(d->object) + QLatin1Char('.');

            QmlEnginePrivate *ep = d->context?QmlEnginePrivate::get(d->context->engine()):0;
            QmlValueType *valueType = 0;
            if (ep) valueType = ep->valueTypes[d->core.propType];
            else valueType = QmlValueTypeFactory::valueType(d->core.propType);
            Q_ASSERT(valueType);

            rv += QString::fromUtf8(valueType->metaObject()->property(d->valueType.valueTypeCoreIdx).name());

            if (!ep) delete valueType;

            d->nameCache = rv;
        } else if (type() & SignalProperty) {
            QString name = QLatin1String("on") + d->core.name(d->object);
            name[2] = name.at(2).toUpper();
            d->nameCache = name;
        } else {
            d->nameCache = d->core.name(d->object);
        }
        d->isNameCached = true;
    }

    return d->nameCache;
}

/*!
  Returns the \l{QMetaProperty} {Qt property} associated with
  this QML property.
 */
QMetaProperty QmlMetaProperty::property() const
{
    if (type() & Property && d->core.isValid() && d->object)
        return d->object->metaObject()->property(d->core.coreIndex);
    else
        return QMetaProperty();
}

/*!
    Return the QMetaMethod for this property if it is a SignalProperty, 
    otherwise returns an invalid QMetaMethod.
*/
QMetaMethod QmlMetaProperty::method() const
{
    if (type() & SignalProperty && d->object)
        return d->object->metaObject()->method(d->core.coreIndex);
    else
        return QMetaMethod();
}


/*!
    Returns the binding associated with this property, or 0 if no binding 
    exists.
*/
QmlAbstractBinding *QmlMetaProperty::binding() const
{
    if (!isProperty() || (type() & Attached) || !d->object)
        return 0;

    QmlDeclarativeData *data = QmlDeclarativeData::get(d->object);
    if (!data) 
        return 0;

    if (!data->hasBindingBit(d->core.coreIndex))
        return 0;

    QmlAbstractBinding *binding = data->bindings;
    while (binding) {
        // ### This wont work for value types
        if (binding->propertyIndex() == d->core.coreIndex)
            return binding; 
        binding = binding->m_nextBinding;
    }
    return 0;
}

/*!
    Set the binding associated with this property to \a newBinding.  Returns
    the existing binding (if any), otherwise 0.

    \a newBinding will be enabled, and the returned binding (if any) will be
    disabled.

    Ownership of \a newBinding transfers to QML.  Ownership of the return value
    is assumed by the caller.

    \a flags is passed through to the binding and is used for the initial update (when
    the binding sets the intial value, it will use these flags for the write).
*/
QmlAbstractBinding *
QmlMetaProperty::setBinding(QmlAbstractBinding *newBinding, QmlMetaProperty::WriteFlags flags) const
{
    if (!isProperty() || (type() & Attached) || !d->object) {
        if (newBinding)
            newBinding->destroy();
        return 0;
    }

    return d->setBinding(d->object, d->core, newBinding, flags);
}

QmlAbstractBinding *
QmlMetaPropertyPrivate::setBinding(QObject *object, const QmlPropertyCache::Data &core, 
                                   QmlAbstractBinding *newBinding, QmlMetaProperty::WriteFlags flags)
{
    QmlDeclarativeData *data = QmlDeclarativeData::get(object, 0 != newBinding);

    if (data && data->hasBindingBit(core.coreIndex)) {
        QmlAbstractBinding *binding = data->bindings;
        while (binding) {
            // ### This wont work for value types
            if (binding->propertyIndex() == core.coreIndex) {
                binding->setEnabled(false);

                if (newBinding) 
                    newBinding->setEnabled(true, flags);

                return binding; // ### QmlAbstractBinding;
            }

            binding = binding->m_nextBinding;
        }
    } 

    if (newBinding)
        newBinding->setEnabled(true, flags);

    return 0;
}
/*!
    Returns the expression associated with this signal property, or 0 if no 
    signal expression exists.
*/
QmlExpression *QmlMetaProperty::signalExpression() const
{
    if (!(type() & SignalProperty))
        return 0;

    const QObjectList &children = d->object->children();
    
    for (int ii = 0; ii < children.count(); ++ii) {
        QObject *child = children.at(ii);

        QmlBoundSignal *signal = QmlBoundSignal::cast(child);
        if (signal && signal->index() == coreIndex()) 
            return signal->expression();
    }

    return 0;
}

/*!
    Set the signal expression associated with this signal property to \a expr.
    Returns the existing signal expression (if any), otherwise 0.

    Ownership of \a expr transfers to QML.  Ownership of the return value is
    assumed by the caller.
*/
QmlExpression *QmlMetaProperty::setSignalExpression(QmlExpression *expr) const
{
    if (!(type() & SignalProperty)) {
        delete expr;
        return 0;
    }

    const QObjectList &children = d->object->children();
    
    for (int ii = 0; ii < children.count(); ++ii) {
        QObject *child = children.at(ii);

        QmlBoundSignal *signal = QmlBoundSignal::cast(child);
        if (signal && signal->index() == coreIndex()) 
            return signal->setExpression(expr);
    }

    if (expr) {
        QmlBoundSignal *signal = new QmlBoundSignal(d->object, method(), d->object);
        return signal->setExpression(expr);
    } else {
        return 0;
    }
}

QMetaMethod QmlMetaPropertyPrivate::findSignal(QObject *obj, const QString &name)
{
    const QMetaObject *mo = obj->metaObject();

    int methods = mo->methodCount();
    for (int ii = methods - 1; ii >= 0; --ii) {
        QMetaMethod method = mo->method(ii);
        QString methodName = QString::fromUtf8(method.signature());
        int idx = methodName.indexOf(QLatin1Char('('));
        methodName = methodName.left(idx);

        if (methodName == name) 
            return method;
    }
    return QMetaMethod();
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

        return QVariant();

    } else if (type() & Property || type() & Attached) {

        return d->readValueProperty();

    }
    return QVariant();
}

QVariant QmlMetaPropertyPrivate::readValueProperty()
{
    uint type = q->type();
    if (type & QmlMetaProperty::Attached) {

        return QVariant::fromValue(attachedObject());

    } else if(type & QmlMetaProperty::ValueTypeProperty) {

        QmlEnginePrivate *ep = context?QmlEnginePrivate::get(context->engine()):0;
        QmlValueType *valueType = 0;
        if (ep) valueType = ep->valueTypes[core.propType];
        else valueType = QmlValueTypeFactory::valueType(core.propType);
        Q_ASSERT(valueType);

        valueType->read(object, core.coreIndex);

        QVariant rv =
            valueType->metaObject()->property(this->valueType.valueTypeCoreIdx).read(valueType);

        if (!ep) delete valueType;
        return rv;

    } else {

        return object->metaObject()->property(core.coreIndex).read(object.data());

    }
}

//###
//writeEnumProperty MIRRORS the relelvant bit of QMetaProperty::write AND MUST BE KEPT IN SYNC!
//###
bool QmlMetaPropertyPrivate::writeEnumProperty(const QMetaProperty &prop, int idx, QObject *object, const QVariant &value, int flags)
{
    if (!object || !prop.isWritable())
        return false;

    QVariant v = value;
    if (prop.isEnumType()) {
        QMetaEnum menum = prop.enumerator();
        if (v.userType() == QVariant::String
#ifdef QT3_SUPPORT
            || v.userType() == QVariant::CString
#endif
            ) {
            if (prop.isFlagType())
                v = QVariant(menum.keysToValue(value.toByteArray()));
            else
                v = QVariant(menum.keyToValue(value.toByteArray()));
        } else if (v.userType() != QVariant::Int && v.userType() != QVariant::UInt) {
            int enumMetaTypeId = QMetaType::type(QByteArray(menum.scope()) + "::" + menum.name());
            if ((enumMetaTypeId == 0) || (v.userType() != enumMetaTypeId) || !v.constData())
                return false;
            v = QVariant(*reinterpret_cast<const int *>(v.constData()));
        }
        v.convert(QVariant::Int);
    }

    // the status variable is changed by qt_metacall to indicate what it did
    // this feature is currently only used by QtDBus and should not be depended
    // upon. Don't change it without looking into QDBusAbstractInterface first
    // -1 (unchanged): normal qt_metacall, result stored in argv[0]
    // changed: result stored directly in value, return the value of status
    int status = -1;
    void *argv[] = { v.data(), &v, &status, &flags };
    QMetaObject::metacall(object, QMetaObject::WriteProperty, idx, argv);
    return status;
}

bool QmlMetaPropertyPrivate::writeValueProperty(const QVariant &value,
                                                QmlMetaProperty::WriteFlags flags)
{
    // Remove any existing bindings on this property
    if (!(flags & QmlMetaProperty::DontRemoveBinding)) {
        QmlAbstractBinding *binding = q->setBinding(0);
        if (binding) binding->destroy();
    }

    bool rv = false;
    uint type = q->type();
    if (type & QmlMetaProperty::ValueTypeProperty) {
        QmlEnginePrivate *ep = 
            context?static_cast<QmlEnginePrivate *>(QObjectPrivate::get(context->engine())):0;

        QmlValueType *writeBack = 0;
        if (ep) {
            writeBack = ep->valueTypes[core.propType];
        } else {
            writeBack = QmlValueTypeFactory::valueType(core.propType);
        }

        writeBack->read(object, core.coreIndex);

        QmlPropertyCache::Data data = core;
        data.coreIndex = valueType.valueTypeCoreIdx;
        data.propType = valueType.valueTypePropType;
        rv = write(writeBack, data, value, context, flags);

        writeBack->write(object, core.coreIndex, flags);
        if (!ep) delete writeBack;

    } else {

        rv = write(object, core, value, context, flags);

    }

    return rv;
}

bool QmlMetaPropertyPrivate::write(QObject *object, const QmlPropertyCache::Data &property, 
                                   const QVariant &value, QmlContext *context, 
                                   QmlMetaProperty::WriteFlags flags)
{
    int coreIdx = property.coreIndex;
    int status = -1;    //for dbus

    if (property.flags & QmlPropertyCache::Data::IsEnumType) {
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);
        QVariant v = value;
        // Enum values come through the script engine as doubles
        if (value.userType() == QVariant::Double) { 
            double integral;
            double fractional = modf(value.toDouble(), &integral);
            if (qFuzzyIsNull(fractional))
                v.convert(QVariant::Int);
        }
        return writeEnumProperty(prop, coreIdx, object, v, flags);
    }

    int t = property.propType;
    int vt = value.userType();

    QmlEnginePrivate *enginePriv = 0;
    if (context && context->engine())
        enginePriv = QmlEnginePrivate::get(context->engine());

    if (t == QVariant::Url) {

        QUrl u;
        bool found = false;
        if (vt == QVariant::Url) {
            u = value.toUrl();
            found = true;
        } else if (vt == QVariant::ByteArray) {
            u = QUrl(QString::fromUtf8(value.toByteArray()));
            found = true;
        } else if (vt == QVariant::String) {
            u = QUrl(value.toString());
            found = true;
        }

        if (!found)
            return false;

        if (context && u.isRelative() && !u.isEmpty())
            u = context->resolvedUrl(u);
        int status = -1;
        void *argv[] = { &u, 0, &status, &flags };
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, argv);

    } else if (vt == t) {

        void *a[] = { (void *)value.constData(), 0, &status, &flags };
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (qMetaTypeId<QVariant>() == t) {

        void *a[] = { (void *)&value, 0, &status, &flags };
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (property.flags & QmlPropertyCache::Data::IsQObjectDerived) {

        const QMetaObject *valMo = rawMetaObjectForType(enginePriv, value.userType());
        
        if (!valMo)
            return false;

        QObject *o = *(QObject **)value.constData();
        const QMetaObject *propMo = rawMetaObjectForType(enginePriv, t);

        if (o) valMo = o->metaObject();

        if (canConvert(valMo, propMo)) {
            void *args[] = { &o, 0, &status, &flags };
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, 
                                  args);
        } else if (!o && canConvert(propMo, valMo)) {
            // In the case of a null QObject, we assign the null if there is 
            // any change that the null variant type could be up or down cast to 
            // the property type.
            void *args[] = { &o, 0, &status, &flags };
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, 
                                  args);
        } else {
            return false;
        }

    } else if (property.flags & QmlPropertyCache::Data::IsQList) {

        int listType = QmlMetaType::listType(t);
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);

        if (value.userType() == qMetaTypeId<QList<QObject *> >()) {
            const QList<QObject *> &list =
                qvariant_cast<QList<QObject *> >(value);
            QVariant listVar = prop.read(object);
            QmlMetaType::clear(listVar);
            for (int ii = 0; ii < list.count(); ++ii) {
                QVariant v = QmlMetaType::qmlType(listType)->fromObject(list.at(ii)); 
                QmlMetaType::append(listVar, v);
            }

        } else if (vt == listType ||
                  value.userType() == listType) {
            QVariant listVar = prop.read(object);
            QmlMetaType::append(listVar, value);
        }

    } else if (property.flags & QmlPropertyCache::Data::IsQmlList) {

        // XXX - optimize!
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);
        QVariant list = prop.read(object);
        QmlPrivate::ListInterface *li =
            *(QmlPrivate::ListInterface **)list.constData();

        int type = li->type();

        if (QObject *obj = QmlMetaType::toQObject(value)) {
            const QMetaObject *mo = rawMetaObjectForType(enginePriv, type);

            const QMetaObject *objMo = obj->metaObject();
            bool found = false;
            while(!found && objMo) {
                if (equal(objMo, mo))
                    found = true;
                else
                    objMo = objMo->superClass();
            }

            if (!found) 
                return false;

            // NOTE: This assumes a cast to QObject does not alter
            // the object pointer
            void *d = (void *)&obj;
            li->append(d);
        }
    } else {
        Q_ASSERT(vt != t);

        QVariant v = value;
        if (v.convert((QVariant::Type)t)) {
            void *a[] = { (void *)v.constData(), 0, &status, &flags};
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
        } else if ((uint)t >= QVariant::UserType && vt == QVariant::String) {
            QmlMetaType::StringConverter con = QmlMetaType::customStringConverter(t);
            if (!con)
                return false;

            QVariant v = con(value.toString());
            if (v.userType() == t) {
                void *a[] = { (void *)v.constData(), 0, &status, &flags};
                QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
            }
        } else if (vt == QVariant::String) {
            bool ok = false;
            QVariant v = QmlStringConverters::variantFromString(value.toString(), t, &ok);
            if (!ok)
                return false;

            void *a[] = { (void *)v.constData(), 0, &status, &flags};
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
        } else {
            return false;
        }
    }

    return true;
}

const QMetaObject *QmlMetaPropertyPrivate::rawMetaObjectForType(QmlEnginePrivate *engine, int userType)
{
    if (engine) {
        return engine->rawMetaObjectForType(userType);
    } else {
        QmlType *type = QmlMetaType::qmlType(userType);
        return type?type->baseMetaObject():0;
    }
}

/*!
    Set the property value to \a value.
*/
bool QmlMetaProperty::write(const QVariant &value) const
{
    return write(value, 0);
}

/*!
    Resets the property value.
*/
bool QmlMetaProperty::reset() const
{
    if (isResettable()) {
        void *args[] = { 0 };
        QMetaObject::metacall(d->object, QMetaObject::ResetProperty, d->core.coreIndex, args);
        return true;
    } else {
        return false;
    }
}

bool QmlMetaProperty::write(const QVariant &value, QmlMetaProperty::WriteFlags flags) const
{
    if (d->object && type() & Property && d->core.isValid() && isWritable()) 
        return d->writeValueProperty(value, flags);
    else 
        return false;
}

/*!
    Returns true if the property has a change notifier signal, otherwise false.
*/
bool QmlMetaProperty::hasChangedNotifier() const
{
    if (type() & Property && !(type() & Attached) && d->object) {
        return d->object->metaObject()->property(d->core.coreIndex).hasNotifySignal();
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

    QMetaProperty prop = d->object->metaObject()->property(d->core.coreIndex);
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

    QMetaProperty prop = d->object->metaObject()->property(d->core.coreIndex);
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
    return d->core.coreIndex;
}

/*! \internal */
int QmlMetaProperty::valueTypeCoreIndex() const
{
    return d->valueType.valueTypeCoreIdx;
}

Q_GLOBAL_STATIC(QmlValueTypeFactory, qmlValueTypes);


struct SerializedData {
    QmlMetaProperty::Type type;
    QmlPropertyCache::Data core;
};

struct ValueTypeSerializedData : public SerializedData {
    QmlPropertyCache::ValueTypeData valueType;
};

QByteArray QmlMetaPropertyPrivate::saveValueType(const QMetaObject *metaObject, int index, 
                                                 int subIndex, int subType)
{
    ValueTypeSerializedData sd;
    sd.type = QmlMetaProperty::ValueTypeProperty;
    sd.core.load(metaObject->property(index));
    sd.valueType.valueTypeCoreIdx = subIndex;
    sd.valueType.valueTypePropType = subType;

    QByteArray rv((const char *)&sd, sizeof(sd));
    return rv;
}

QByteArray QmlMetaPropertyPrivate::saveProperty(const QMetaObject *metaObject, int index)
{
    SerializedData sd;
    sd.type = QmlMetaProperty::Property;
    sd.core.load(metaObject->property(index));

    QByteArray rv((const char *)&sd, sizeof(sd));
    return rv;
}

QmlMetaProperty 
QmlMetaPropertyPrivate::restore(const QByteArray &data, QObject *object, QmlContext *ctxt)
{
    QmlMetaProperty prop;

    if (data.isEmpty())
        return prop;

    prop.d->object = object;
    prop.d->context = ctxt;

    const SerializedData *sd = (const SerializedData *)data.constData();
    if (sd->type == QmlMetaProperty::Property) {
        prop.d->core = sd->core;
    } else if(sd->type == QmlMetaProperty::ValueTypeProperty) {
        const ValueTypeSerializedData *vt = (const ValueTypeSerializedData *)sd;
        prop.d->core = vt->core;
        prop.d->valueType = vt->valueType;
    }

    return prop;
}

/*!
    \internal

    Creates a QmlMetaProperty for the property \a name of \a obj. Unlike
    the QmlMetaProperty(QObject*, QString, QmlContext*) constructor, this static function
    will correctly handle dot properties, including value types and attached properties.
*/
QmlMetaProperty QmlMetaProperty::createProperty(QObject *obj, 
                                                const QString &name,
                                                QmlContext *context)
{
    QmlTypeNameCache *typeNameCache = context?QmlContextPrivate::get(context)->imports:0;

    QStringList path = name.split(QLatin1Char('.'));
    QObject *object = obj;

    for (int jj = 0; jj < path.count() - 1; ++jj) {
        const QString &pathName = path.at(jj);

        if (QmlTypeNameCache::Data *data = typeNameCache?typeNameCache->data(pathName):0) {
            if (data->type) {
                QmlAttachedPropertiesFunc func = data->type->attachedPropertiesFunction();
                if (!func) 
                    return QmlMetaProperty();
                object = qmlAttachedPropertiesObjectById(data->type->index(), object);
                if (!object)
                    return QmlMetaProperty();
                continue;
            } else {
                Q_ASSERT(data->typeNamespace);
                ++jj;
                data = data->typeNamespace->data(path.at(jj));
                if (!data || !data->type)
                    return QmlMetaProperty();
                QmlAttachedPropertiesFunc func = data->type->attachedPropertiesFunction();
                if (!func) 
                    return QmlMetaProperty();
                object = qmlAttachedPropertiesObjectById(data->type->index(), object);
                if (!object)
                    return QmlMetaProperty();
                continue;
            }
        }

        QmlMetaProperty prop(object, pathName, context);

        if (jj == path.count() - 2 && prop.propertyType() < (int)QVariant::UserType &&
            qmlValueTypes()->valueTypes[prop.propertyType()]) {
            // We're now at a value type property.  We can use a global valuetypes array as we 
            // never actually use the objects, just look up their properties.
            QObject *typeObject = 
                qmlValueTypes()->valueTypes[prop.propertyType()];
            int idx = typeObject->metaObject()->indexOfProperty(path.last().toUtf8().constData());
            if (idx == -1)
                return QmlMetaProperty();
            QMetaProperty vtProp = typeObject->metaObject()->property(idx);

            QmlMetaProperty p = prop;
            p.d->valueType.valueTypeCoreIdx = idx;
            p.d->valueType.valueTypePropType = vtProp.userType();
            return p;
        }

        QObject *objVal = QmlMetaType::toQObject(prop.read());
        if (!objVal)
            return QmlMetaProperty();
        object = objVal;
    }

    const QString &propName = path.last();
    QmlMetaProperty prop(object, propName, context);
    if (!prop.isValid())
        return QmlMetaProperty();
    else
        return prop;
}

/*!
    Returns true if lhs and rhs refer to the same metaobject data
*/
bool QmlMetaPropertyPrivate::equal(const QMetaObject *lhs, const QMetaObject *rhs)
{
    return lhs == rhs || (1 && lhs && rhs && lhs->d.stringdata == rhs->d.stringdata);
}

/*!
    Returns true if from inherits to.
*/
bool QmlMetaPropertyPrivate::canConvert(const QMetaObject *from, const QMetaObject *to)
{
    if (from && to == &QObject::staticMetaObject)
        return true;

    while (from) {
        if (equal(from, to))
            return true;
        from = from->superClass();
    }
    
    return false;
}

QT_END_NAMESPACE
