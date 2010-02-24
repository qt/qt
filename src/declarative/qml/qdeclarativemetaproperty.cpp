/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativemetaproperty.h"
#include "qdeclarativemetaproperty_p.h"

#include "qdeclarativecompositetypedata_p.h"
#include "qdeclarative.h"
#include "qdeclarativebinding_p.h"
#include "qdeclarativecontext.h"
#include "qdeclarativecontext_p.h"
#include "qdeclarativeboundsignal_p.h"
#include "qdeclarativeengine.h"
#include "qdeclarativeengine_p.h"
#include "qdeclarativedeclarativedata_p.h"
#include "qdeclarativestringconverters_p.h"
#include "qdeclarativelist_p.h"

#include <QStringList>
#include <QtCore/qdebug.h>

#include <math.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDeclarativeMetaProperty
    \brief The QDeclarativeMetaProperty class abstracts accessing QML properties.
    \internal
 */

/*!
    Create an invalid QDeclarativeMetaProperty.
*/
QDeclarativeMetaProperty::QDeclarativeMetaProperty()
: d(new QDeclarativeMetaPropertyPrivate)
{
    d->q = this;
}

/*!  \internal */
QDeclarativeMetaProperty::~QDeclarativeMetaProperty()
{
    delete d; d = 0;
}

/*!
    Creates a QDeclarativeMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QDeclarativeMetaProperty will be created.
 */
QDeclarativeMetaProperty::QDeclarativeMetaProperty(QObject *obj)
: d(new QDeclarativeMetaPropertyPrivate)
{
    d->q = this;
    d->initDefault(obj);
}

/*!
    \internal
    Creates a QDeclarativeMetaProperty for the default property of \a obj. If there is no
    default property, an invalid QDeclarativeMetaProperty will be created.
 */
QDeclarativeMetaProperty::QDeclarativeMetaProperty(QObject *obj, QDeclarativeContext *ctxt)
: d(new QDeclarativeMetaPropertyPrivate)
{
    d->q = this;
    d->context = ctxt;
    d->initDefault(obj);
}

/*!
    Initialize from the default property of \a obj
*/
void QDeclarativeMetaPropertyPrivate::initDefault(QObject *obj)
{
    if (!obj)
        return;

    QMetaProperty p = QDeclarativeMetaType::defaultProperty(obj);
    core.load(p);
    if (core.isValid()) {
        isDefaultProperty = true;
        object = obj;
    }
}

/*!
    Creates a QDeclarativeMetaProperty for the property \a name of \a obj.
 */
QDeclarativeMetaProperty::QDeclarativeMetaProperty(QObject *obj, const QString &name)
: d(new QDeclarativeMetaPropertyPrivate)
{
    d->q = this;
    d->initProperty(obj, name);
    if (!isValid()) d->object = 0;
}

/*!
    \internal
    Creates a QDeclarativeMetaProperty for the property \a name of \a obj.
 */
QDeclarativeMetaProperty::QDeclarativeMetaProperty(QObject *obj, const QString &name, QDeclarativeContext *ctxt)
: d(new QDeclarativeMetaPropertyPrivate)
{
    d->q = this;
    d->context = ctxt;
    d->initProperty(obj, name);
    if (!isValid()) { d->object = 0; d->context = 0; }
}

void QDeclarativeMetaPropertyPrivate::initProperty(QObject *obj, const QString &name)
{
    QDeclarativeEnginePrivate *enginePrivate = 0;
    if (context && context->engine())
        enginePrivate = QDeclarativeEnginePrivate::get(context->engine());

    object = obj;

    if (name.isEmpty() || !obj)
        return;

    if (enginePrivate && name.at(0).isUpper()) {
        // Attached property
        // ### What about qualified types?
        QDeclarativeTypeNameCache *tnCache = QDeclarativeContextPrivate::get(context)->imports;
        if (tnCache) {
            QDeclarativeTypeNameCache::Data *d = tnCache->data(name);
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
    QDeclarativePropertyCache::Data local;
    QDeclarativePropertyCache::Data *property = 
        QDeclarativePropertyCache::property(context?context->engine():0, obj, name, local);
    if (property && !(property->flags & QDeclarativePropertyCache::Data::IsFunction)) 
        core = *property;
}

/*!
    Create a copy of \a other.
*/
QDeclarativeMetaProperty::QDeclarativeMetaProperty(const QDeclarativeMetaProperty &other)
: d(new QDeclarativeMetaPropertyPrivate(*other.d))
{
    d->q = this;
}

/*!
  \enum QDeclarativeMetaProperty::PropertyCategory

  This enum specifies a category of QML property.

  \value Unknown The category is unknown.  This will never be returned from propertyCategory()
  \value InvalidProperty The property is invalid.
  \value Bindable The property is a QDeclarativeBinding.
  \value List The property is a QList pointer
  \value Object The property is a QObject derived type pointer
  \value Normal The property is none of the above.
 */

/*!
  \enum QDeclarativeMetaProperty::Type

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
QDeclarativeMetaProperty::PropertyCategory QDeclarativeMetaProperty::propertyCategory() const
{
    return d->propertyCategory();
}

QDeclarativeMetaProperty::PropertyCategory 
QDeclarativeMetaPropertyPrivate::propertyCategory() const
{
    uint type = q->type();

    if (type & QDeclarativeMetaProperty::ValueTypeProperty) {
        return QDeclarativeMetaProperty::Normal;
    } else if (type & QDeclarativeMetaProperty::Attached) {
        return QDeclarativeMetaProperty::Object;
    } else if (type & QDeclarativeMetaProperty::Property) {
        int type = propertyType();
        if (type == QVariant::Invalid)
            return QDeclarativeMetaProperty::InvalidProperty;
        else if ((uint)type < QVariant::UserType)
            return QDeclarativeMetaProperty::Normal;
        else if (type == qMetaTypeId<QDeclarativeBinding *>())
            return QDeclarativeMetaProperty::Bindable;
        else if (core.flags & QDeclarativePropertyCache::Data::IsQObjectDerived)
            return QDeclarativeMetaProperty::Object;
        else if (core.flags & QDeclarativePropertyCache::Data::IsQList)
            return QDeclarativeMetaProperty::List;
        else 
            return QDeclarativeMetaProperty::Normal;
    } else {
        return QDeclarativeMetaProperty::InvalidProperty;
    }
}

/*!
    Returns the type name of the property, or 0 if the property has no type
    name.
*/
const char *QDeclarativeMetaProperty::propertyTypeName() const
{
    if (type() & ValueTypeProperty) {

        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(d->context);
        QDeclarativeValueType *valueType = 0;
        if (ep) valueType = ep->valueTypes[d->core.propType];
        else valueType = QDeclarativeValueTypeFactory::valueType(d->core.propType);
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
    Returns true if \a other and this QDeclarativeMetaProperty represent the same 
    property.
*/
bool QDeclarativeMetaProperty::operator==(const QDeclarativeMetaProperty &other) const
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
int QDeclarativeMetaProperty::propertyType() const
{
    return d->propertyType();
}

int QDeclarativeMetaPropertyPrivate::propertyType() const
{
    uint type = q->type();
    if (type & QDeclarativeMetaProperty::ValueTypeProperty) {
        return valueType.valueTypePropType;
    } else if (type & QDeclarativeMetaProperty::Attached) {
        return qMetaTypeId<QObject *>();
    } else if (type & QDeclarativeMetaProperty::Property) {
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
QDeclarativeMetaProperty::Type QDeclarativeMetaProperty::type() const
{
    if (d->core.flags & QDeclarativePropertyCache::Data::IsFunction)
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
    Returns true if this QDeclarativeMetaProperty represents a regular Qt property.
*/
bool QDeclarativeMetaProperty::isProperty() const
{
    return type() & Property;
}

/*!
    Returns true if this QDeclarativeMetaProperty represents a default property.
*/
bool QDeclarativeMetaProperty::isDefault() const
{
    return type() & Default;
}

/*!
    Returns the QDeclarativeMetaProperty's QObject.
*/
QObject *QDeclarativeMetaProperty::object() const
{
    return d->object;
}

/*!
    Assign \a other to this QDeclarativeMetaProperty.
*/
QDeclarativeMetaProperty &QDeclarativeMetaProperty::operator=(const QDeclarativeMetaProperty &other)
{
    d->context = other.d->context;
    d->object = other.d->object;

    d->isDefaultProperty = other.d->isDefaultProperty;
    d->isNameCached = other.d->isNameCached;
    d->core = other.d->core;
    d->nameCache = other.d->nameCache;

    d->valueType = other.d->valueType;

    d->attachedFunc = other.d->attachedFunc;
    return *this;
}

/*!
    Returns true if the property is writable, otherwise false.
*/
bool QDeclarativeMetaProperty::isWritable() const
{
    QDeclarativeMetaProperty::PropertyCategory category = propertyCategory();

    if (!d->object)
        return false;
    if (category == List)
        return true;
    else if (type() & SignalProperty)
        return false;
    else if (d->core.isValid() && d->object)
        return d->core.flags & QDeclarativePropertyCache::Data::IsWritable;
    else
        return false;
}

/*!
    Returns true if the property is designable, otherwise false.
*/
bool QDeclarativeMetaProperty::isDesignable() const
{
    if (type() & Property && d->core.isValid() && d->object)
        return d->object->metaObject()->property(d->core.coreIndex).isDesignable();
    else
        return false;
}

/*!
    Returns true if the property is resettable, otherwise false.
*/
bool QDeclarativeMetaProperty::isResettable() const
{
    if (type() & Property && d->core.isValid() && d->object)
        return d->core.flags & QDeclarativePropertyCache::Data::IsResettable;
    else
        return false;
}

/*!
    Returns true if the QDeclarativeMetaProperty refers to a valid property, otherwise
    false.
*/
bool QDeclarativeMetaProperty::isValid() const
{
    return type() != Invalid;
}

/*!
    Return the name of this QML property.
*/
QString QDeclarativeMetaProperty::name() const
{
    if (!d->isNameCached) {
        // ###
        if (!d->object) {
        } else if (type() & ValueTypeProperty) {
            QString rv = d->core.name(d->object) + QLatin1Char('.');

            QDeclarativeEnginePrivate *ep = d->context?QDeclarativeEnginePrivate::get(d->context->engine()):0;
            QDeclarativeValueType *valueType = 0;
            if (ep) valueType = ep->valueTypes[d->core.propType];
            else valueType = QDeclarativeValueTypeFactory::valueType(d->core.propType);
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
QMetaProperty QDeclarativeMetaProperty::property() const
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
QMetaMethod QDeclarativeMetaProperty::method() const
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
QDeclarativeAbstractBinding *QDeclarativeMetaProperty::binding() const
{
    if (!isProperty() || (type() & Attached) || !d->object)
        return 0;

    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(d->object);
    if (!data) 
        return 0;

    if (!data->hasBindingBit(d->core.coreIndex))
        return 0;

    QDeclarativeAbstractBinding *binding = data->bindings;
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
QDeclarativeAbstractBinding *
QDeclarativeMetaProperty::setBinding(QDeclarativeAbstractBinding *newBinding, QDeclarativeMetaProperty::WriteFlags flags) const
{
    if (!isProperty() || (type() & Attached) || !d->object) {
        if (newBinding)
            newBinding->destroy();
        return 0;
    }

    return d->setBinding(d->object, d->core, newBinding, flags);
}

QDeclarativeAbstractBinding *
QDeclarativeMetaPropertyPrivate::setBinding(QObject *object, const QDeclarativePropertyCache::Data &core, 
                                   QDeclarativeAbstractBinding *newBinding, QDeclarativeMetaProperty::WriteFlags flags)
{
    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(object, 0 != newBinding);

    if (data && data->hasBindingBit(core.coreIndex)) {
        QDeclarativeAbstractBinding *binding = data->bindings;
        while (binding) {
            // ### This wont work for value types
            if (binding->propertyIndex() == core.coreIndex) {
                binding->setEnabled(false);

                if (newBinding) 
                    newBinding->setEnabled(true, flags);

                return binding; // ### QDeclarativeAbstractBinding;
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
QDeclarativeExpression *QDeclarativeMetaProperty::signalExpression() const
{
    if (!(type() & SignalProperty))
        return 0;

    const QObjectList &children = d->object->children();
    
    for (int ii = 0; ii < children.count(); ++ii) {
        QObject *child = children.at(ii);

        QDeclarativeBoundSignal *signal = QDeclarativeBoundSignal::cast(child);
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
QDeclarativeExpression *QDeclarativeMetaProperty::setSignalExpression(QDeclarativeExpression *expr) const
{
    if (!(type() & SignalProperty)) {
        delete expr;
        return 0;
    }

    const QObjectList &children = d->object->children();
    
    for (int ii = 0; ii < children.count(); ++ii) {
        QObject *child = children.at(ii);

        QDeclarativeBoundSignal *signal = QDeclarativeBoundSignal::cast(child);
        if (signal && signal->index() == coreIndex()) 
            return signal->setExpression(expr);
    }

    if (expr) {
        QDeclarativeBoundSignal *signal = new QDeclarativeBoundSignal(d->object, method(), d->object);
        return signal->setExpression(expr);
    } else {
        return 0;
    }
}

QMetaMethod QDeclarativeMetaPropertyPrivate::findSignal(QObject *obj, const QString &name)
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

QObject *QDeclarativeMetaPropertyPrivate::attachedObject() const
{
    if (attachedFunc == -1)
        return 0;
    else
        return qmlAttachedPropertiesObjectById(attachedFunc, object);
}

/*!
    Returns the property value.
*/
QVariant QDeclarativeMetaProperty::read() const
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

QVariant QDeclarativeMetaPropertyPrivate::readValueProperty()
{
    uint type = q->type();
    if (type & QDeclarativeMetaProperty::Attached) {

        return QVariant::fromValue(attachedObject());

    } else if(type & QDeclarativeMetaProperty::ValueTypeProperty) {

        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(context);
        QDeclarativeValueType *valueType = 0;
        if (ep) valueType = ep->valueTypes[core.propType];
        else valueType = QDeclarativeValueTypeFactory::valueType(core.propType);
        Q_ASSERT(valueType);

        valueType->read(object, core.coreIndex);

        QVariant rv =
            valueType->metaObject()->property(this->valueType.valueTypeCoreIdx).read(valueType);

        if (!ep) delete valueType;
        return rv;

    } else if(core.flags & QDeclarativePropertyCache::Data::IsQList) {

        QDeclarativeListProperty<QObject> prop;
        void *args[] = { &prop, 0 };
        QMetaObject::metacall(object, QMetaObject::ReadProperty, core.coreIndex, args);
        return QVariant::fromValue(QDeclarativeListReferencePrivate::init(prop, core.propType, context?context->engine():0));

    } else {

        return object->metaObject()->property(core.coreIndex).read(object.data());

    }
}

//###
//writeEnumProperty MIRRORS the relelvant bit of QMetaProperty::write AND MUST BE KEPT IN SYNC!
//###
bool QDeclarativeMetaPropertyPrivate::writeEnumProperty(const QMetaProperty &prop, int idx, QObject *object, const QVariant &value, int flags)
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

bool QDeclarativeMetaPropertyPrivate::writeValueProperty(const QVariant &value,
                                                QDeclarativeMetaProperty::WriteFlags flags)
{
    // Remove any existing bindings on this property
    if (!(flags & QDeclarativeMetaProperty::DontRemoveBinding)) {
        QDeclarativeAbstractBinding *binding = q->setBinding(0);
        if (binding) binding->destroy();
    }

    bool rv = false;
    uint type = q->type();
    if (type & QDeclarativeMetaProperty::ValueTypeProperty) {
        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(context);

        QDeclarativeValueType *writeBack = 0;
        if (ep) {
            writeBack = ep->valueTypes[core.propType];
        } else {
            writeBack = QDeclarativeValueTypeFactory::valueType(core.propType);
        }

        writeBack->read(object, core.coreIndex);

        QDeclarativePropertyCache::Data data = core;
        data.flags = valueType.flags;
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

bool QDeclarativeMetaPropertyPrivate::write(QObject *object, const QDeclarativePropertyCache::Data &property, 
                                   const QVariant &value, QDeclarativeContext *context, 
                                   QDeclarativeMetaProperty::WriteFlags flags)
{
    int coreIdx = property.coreIndex;
    int status = -1;    //for dbus

    if (property.flags & QDeclarativePropertyCache::Data::IsEnumType) {
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

    int propertyType = property.propType;
    int variantType = value.userType();

    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(context);

    if (propertyType == QVariant::Url) {

        QUrl u;
        bool found = false;
        if (variantType == QVariant::Url) {
            u = value.toUrl();
            found = true;
        } else if (variantType == QVariant::ByteArray) {
            u = QUrl(QString::fromUtf8(value.toByteArray()));
            found = true;
        } else if (variantType == QVariant::String) {
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

    } else if (variantType == propertyType) {

        void *a[] = { (void *)value.constData(), 0, &status, &flags };
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (qMetaTypeId<QVariant>() == propertyType) {

        void *a[] = { (void *)&value, 0, &status, &flags };
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (property.flags & QDeclarativePropertyCache::Data::IsQObjectDerived) {

        const QMetaObject *valMo = rawMetaObjectForType(enginePriv, value.userType());
        
        if (!valMo)
            return false;

        QObject *o = *(QObject **)value.constData();
        const QMetaObject *propMo = rawMetaObjectForType(enginePriv, propertyType);

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

    } else if (property.flags & QDeclarativePropertyCache::Data::IsQList) {

        const QMetaObject *listType = 0;
        if (enginePriv) {
            listType = enginePriv->rawMetaObjectForType(enginePriv->listType(property.propType));
        } else {
            QDeclarativeType *type = QDeclarativeMetaType::qmlType(QDeclarativeMetaType::listType(property.propType));
            if (!type) return false;
            listType = type->baseMetaObject();
        }
        if (!listType) return false;

        QDeclarativeListProperty<void> prop;
        void *args[] = { &prop, 0 };
        QMetaObject::metacall(object, QMetaObject::ReadProperty, coreIdx, args);

        if (!prop.clear) return false;

        prop.clear(&prop);

        if (value.userType() == qMetaTypeId<QList<QObject *> >()) {
            const QList<QObject *> &list = qvariant_cast<QList<QObject *> >(value);

            for (int ii = 0; ii < list.count(); ++ii) {
                QObject *o = list.at(ii);
                if (!canConvert(o->metaObject(), listType))
                    o = 0;
                prop.append(&prop, (void *)o);
            }
        } else {
            QObject *o = enginePriv?enginePriv->toQObject(value):QDeclarativeMetaType::toQObject(value);
            if (!canConvert(o->metaObject(), listType))
                o = 0;
            prop.append(&prop, (void *)o);
        }

    } else {
        Q_ASSERT(variantType != propertyType);

        QVariant v = value;
        if (v.convert((QVariant::Type)propertyType)) {
            void *a[] = { (void *)v.constData(), 0, &status, &flags};
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
        } else if ((uint)propertyType >= QVariant::UserType && variantType == QVariant::String) {
            QDeclarativeMetaType::StringConverter con = QDeclarativeMetaType::customStringConverter(propertyType);
            if (!con)
                return false;

            QVariant v = con(value.toString());
            if (v.userType() == propertyType) {
                void *a[] = { (void *)v.constData(), 0, &status, &flags};
                QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
            }
        } else if (variantType == QVariant::String) {
            bool ok = false;
            QVariant v = QDeclarativeStringConverters::variantFromString(value.toString(), propertyType, &ok);
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

const QMetaObject *QDeclarativeMetaPropertyPrivate::rawMetaObjectForType(QDeclarativeEnginePrivate *engine, int userType)
{
    if (engine) {
        return engine->rawMetaObjectForType(userType);
    } else {
        QDeclarativeType *type = QDeclarativeMetaType::qmlType(userType);
        return type?type->baseMetaObject():0;
    }
}

/*!
    Set the property value to \a value.
*/
bool QDeclarativeMetaProperty::write(const QVariant &value) const
{
    return write(value, 0);
}

/*!
    Resets the property value.
*/
bool QDeclarativeMetaProperty::reset() const
{
    if (isResettable()) {
        void *args[] = { 0 };
        QMetaObject::metacall(d->object, QMetaObject::ResetProperty, d->core.coreIndex, args);
        return true;
    } else {
        return false;
    }
}

bool QDeclarativeMetaProperty::write(const QVariant &value, QDeclarativeMetaProperty::WriteFlags flags) const
{
    if (d->object && type() & Property && d->core.isValid() && isWritable()) 
        return d->writeValueProperty(value, flags);
    else 
        return false;
}

/*!
    Returns true if the property has a change notifier signal, otherwise false.
*/
bool QDeclarativeMetaProperty::hasChangedNotifier() const
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
bool QDeclarativeMetaProperty::needsChangedNotifier() const
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
bool QDeclarativeMetaProperty::connectNotifier(QObject *dest, int method) const
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
bool QDeclarativeMetaProperty::connectNotifier(QObject *dest, const char *slot) const
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
int QDeclarativeMetaProperty::coreIndex() const
{
    return d->core.coreIndex;
}

/*! \internal */
int QDeclarativeMetaProperty::valueTypeCoreIndex() const
{
    return d->valueType.valueTypeCoreIdx;
}

Q_GLOBAL_STATIC(QDeclarativeValueTypeFactory, qmlValueTypes);


struct SerializedData {
    QDeclarativeMetaProperty::Type type;
    QDeclarativePropertyCache::Data core;
};

struct ValueTypeSerializedData : public SerializedData {
    QDeclarativePropertyCache::ValueTypeData valueType;
};

QByteArray QDeclarativeMetaPropertyPrivate::saveValueType(const QMetaObject *metaObject, int index, 
                                                 const QMetaObject *subObject, int subIndex)
{
    QMetaProperty prop = metaObject->property(index);
    QMetaProperty subProp = subObject->property(subIndex);

    ValueTypeSerializedData sd;
    sd.type = QDeclarativeMetaProperty::ValueTypeProperty;
    sd.core.load(metaObject->property(index));
    sd.valueType.flags = QDeclarativePropertyCache::Data::flagsForProperty(subProp);
    sd.valueType.valueTypeCoreIdx = subIndex;
    sd.valueType.valueTypePropType = subProp.userType();

    QByteArray rv((const char *)&sd, sizeof(sd));

    return rv;
}

QByteArray QDeclarativeMetaPropertyPrivate::saveProperty(const QMetaObject *metaObject, int index)
{
    SerializedData sd;
    sd.type = QDeclarativeMetaProperty::Property;
    sd.core.load(metaObject->property(index));

    QByteArray rv((const char *)&sd, sizeof(sd));
    return rv;
}

QDeclarativeMetaProperty 
QDeclarativeMetaPropertyPrivate::restore(const QByteArray &data, QObject *object, QDeclarativeContext *ctxt)
{
    QDeclarativeMetaProperty prop;

    if (data.isEmpty())
        return prop;

    prop.d->object = object;
    prop.d->context = ctxt;

    const SerializedData *sd = (const SerializedData *)data.constData();
    if (sd->type == QDeclarativeMetaProperty::Property) {
        prop.d->core = sd->core;
    } else if(sd->type == QDeclarativeMetaProperty::ValueTypeProperty) {
        const ValueTypeSerializedData *vt = (const ValueTypeSerializedData *)sd;
        prop.d->core = vt->core;
        prop.d->valueType = vt->valueType;
    }

    return prop;
}

/*!
    \internal

    Creates a QDeclarativeMetaProperty for the property \a name of \a obj. Unlike
    the QDeclarativeMetaProperty(QObject*, QString, QDeclarativeContext*) constructor, this static function
    will correctly handle dot properties, including value types and attached properties.
*/
QDeclarativeMetaProperty QDeclarativeMetaProperty::createProperty(QObject *obj, 
                                                const QString &name,
                                                QDeclarativeContext *context)
{
    QDeclarativeTypeNameCache *typeNameCache = context?QDeclarativeContextPrivate::get(context)->imports:0;

    QStringList path = name.split(QLatin1Char('.'));
    QObject *object = obj;

    for (int jj = 0; jj < path.count() - 1; ++jj) {
        const QString &pathName = path.at(jj);

        if (QDeclarativeTypeNameCache::Data *data = typeNameCache?typeNameCache->data(pathName):0) {
            if (data->type) {
                QDeclarativeAttachedPropertiesFunc func = data->type->attachedPropertiesFunction();
                if (!func) 
                    return QDeclarativeMetaProperty();
                object = qmlAttachedPropertiesObjectById(data->type->index(), object);
                if (!object)
                    return QDeclarativeMetaProperty();
                continue;
            } else {
                Q_ASSERT(data->typeNamespace);
                ++jj;
                data = data->typeNamespace->data(path.at(jj));
                if (!data || !data->type)
                    return QDeclarativeMetaProperty();
                QDeclarativeAttachedPropertiesFunc func = data->type->attachedPropertiesFunction();
                if (!func) 
                    return QDeclarativeMetaProperty();
                object = qmlAttachedPropertiesObjectById(data->type->index(), object);
                if (!object)
                    return QDeclarativeMetaProperty();
                continue;
            }
        }

        QDeclarativeMetaProperty prop(object, pathName, context);

        if (jj == path.count() - 2 && prop.propertyType() < (int)QVariant::UserType &&
            qmlValueTypes()->valueTypes[prop.propertyType()]) {
            // We're now at a value type property.  We can use a global valuetypes array as we 
            // never actually use the objects, just look up their properties.
            QObject *typeObject = 
                qmlValueTypes()->valueTypes[prop.propertyType()];
            int idx = typeObject->metaObject()->indexOfProperty(path.last().toUtf8().constData());
            if (idx == -1)
                return QDeclarativeMetaProperty();
            QMetaProperty vtProp = typeObject->metaObject()->property(idx);

            QDeclarativeMetaProperty p = prop;
            p.d->valueType.valueTypeCoreIdx = idx;
            p.d->valueType.valueTypePropType = vtProp.userType();
            return p;
        }

        QObject *objVal = QDeclarativeMetaType::toQObject(prop.read());
        if (!objVal)
            return QDeclarativeMetaProperty();
        object = objVal;
    }

    const QString &propName = path.last();
    QDeclarativeMetaProperty prop(object, propName, context);
    if (!prop.isValid())
        return QDeclarativeMetaProperty();
    else
        return prop;
}

/*!
    Returns true if lhs and rhs refer to the same metaobject data
*/
bool QDeclarativeMetaPropertyPrivate::equal(const QMetaObject *lhs, const QMetaObject *rhs)
{
    return lhs == rhs || (1 && lhs && rhs && lhs->d.stringdata == rhs->d.stringdata);
}

/*!
    Returns true if from inherits to.
*/
bool QDeclarativeMetaPropertyPrivate::canConvert(const QMetaObject *from, const QMetaObject *to)
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
