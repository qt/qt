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
#include "qmlcompositetypedata_p.h"
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
#include <private/qmldeclarativedata_p.h>

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

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

    object = obj;
    QMetaProperty p = QmlMetaType::defaultProperty(obj);
    core.load(p);
    if (core.isValid())
        isDefaultProperty = true;
}

/*!
    \internal

    Creates a QmlMetaProperty for the property at index \a idx of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, int idx, QmlContext *ctxt)
: d(new QmlMetaPropertyPrivate)
{
    Q_ASSERT(obj);

    d->q = this;
    d->context = ctxt;
    d->object = obj;
    d->core.load(obj->metaObject()->property(idx));
}

/*!
    Creates a QmlMetaProperty for the property \a name of \a obj.
 */
QmlMetaProperty::QmlMetaProperty(QObject *obj, const QString &name)
: d(new QmlMetaPropertyPrivate)
{
    d->q = this;
    d->initProperty(obj, name);
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
        //### needs to be done in a better way
        QmlCompositeTypeData *typeData =
            enginePrivate->typeManager.get(context->baseUrl());

        if (typeData) {
            QmlType *t = 0;
            enginePrivate->resolveType(typeData->imports, name.toLatin1(), &t, 0, 0, 0, 0);
            if (t && t->attachedPropertiesFunction()) {
                attachedFunc = t->index();
            }
            typeData->release();
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
    QmlPropertyCache *cache = 0;
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
    if (ddata)
        cache = ddata->propertyCache;
    if (!cache)
        cache = enginePrivate?enginePrivate->cache(obj):0;

    if (cache) {
        QmlPropertyCache::Data *data = cache->property(name);

        if (data && !(data->flags & QmlPropertyCache::Data::IsFunction)) 
            core = *data;

    } else {
        // No cache available
        QMetaProperty p = QmlMetaType::property(obj, name.toUtf8().constData());
        core.load(p);
    }
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

        const char *rv = valueType->metaObject()->property(d->valueTypeCoreIdx).typeName();

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
           d->valueTypeCoreIdx == other.d->valueTypeCoreIdx &&
           d->valueTypePropType == other.d->valueTypePropType &&
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
        return valueTypePropType;
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
    else if (d->valueTypeCoreIdx != -1)
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

    d->valueTypeCoreIdx = other.d->valueTypeCoreIdx;
    d->valueTypePropType = other.d->valueTypePropType;

    d->attachedFunc = other.d->attachedFunc;
    return *this;
}

/*!
    Returns true if the property is writable, otherwise false.
*/
bool QmlMetaProperty::isWritable() const
{
    QmlMetaProperty::PropertyCategory category = propertyCategory();

    if (category == List || category == QmlList)
        return true;
    else if (type() & SignalProperty)
        return true;
    else if (d->core.isValid() && d->object)
        return d->object->metaObject()->property(d->core.coreIndex).isWritable();
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
    // ### What is this used for?
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
    if (type() & ValueTypeProperty) {
        QString rv = d->core.name + QLatin1String(".");

        QmlEnginePrivate *ep = d->context?QmlEnginePrivate::get(d->context->engine()):0;
        QmlValueType *valueType = 0;
        if (ep) valueType = ep->valueTypes[d->core.propType];
        else valueType = QmlValueTypeFactory::valueType(d->core.propType);
        Q_ASSERT(valueType);

        rv += QLatin1String(valueType->metaObject()->property(d->valueTypeCoreIdx).name());

        if (!ep) delete valueType;

        return rv;
    } else {
        return d->core.name;
    }
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
*/
QmlAbstractBinding *
QmlMetaProperty::setBinding(QmlAbstractBinding *newBinding) const
{
    if (!isProperty() || (type() & Attached) || !d->object)
        return 0;

    d->setBinding(d->object, d->core, newBinding);
}

QmlAbstractBinding *
QmlMetaPropertyPrivate::setBinding(QObject *object, const QmlPropertyCache::Data &core, 
                                   QmlAbstractBinding *newBinding)
{
    QmlDeclarativeData *data = QmlDeclarativeData::get(object, 0 != newBinding);

    if (data && data->hasBindingBit(core.coreIndex)) {
        QmlAbstractBinding *binding = data->bindings;
        while (binding) {
            // ### This wont work for value types
            if (binding->propertyIndex() == core.coreIndex) {
                binding->setEnabled(false);

                if (newBinding) 
                    newBinding->setEnabled(true);

                return binding; // ### QmlAbstractBinding;
            }

            binding = binding->m_nextBinding;
        }
    } 

    if (newBinding)
        newBinding->setEnabled(true);

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
    if (!(type() & SignalProperty))
        return 0;

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
        QString methodName = QLatin1String(method.signature());
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

        const QObjectList &children = object()->children();

        for (int ii = 0; ii < children.count(); ++ii) {
            QmlBoundSignal *sig = QmlBoundSignal::cast(children.at(ii));
            if (sig && sig->index() == d->core.coreIndex) 
                return sig->expression()->expression();
        }

    } else if (type() & Property) {

        return d->readValueProperty();

    }
    return QVariant();
}

void QmlMetaPropertyPrivate::writeSignalProperty(const QVariant &value)
{
    QString expr = value.toString();
    const QObjectList &children = object->children();

    for (int ii = 0; ii < children.count(); ++ii) {
        QmlBoundSignal *sig = QmlBoundSignal::cast(children.at(ii));
        if (sig && sig->index() == core.coreIndex) {
            if (expr.isEmpty()) {
                sig->disconnect();
                sig->deleteLater();
            } else {
                sig->expression()->setExpression(expr);
            }
            return;
        }
    }

    if (!expr.isEmpty()) {
        // XXX scope
        (void *)new QmlBoundSignal(qmlContext(object), expr, object, q->method(), object);
    }
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
            valueType->metaObject()->property(valueTypeCoreIdx).read(valueType);

        if (!ep) delete valueType;
        return rv;

    } else {

        return object->metaObject()->property(core.coreIndex).read(object.data());

    }
}

void QmlMetaPropertyPrivate::writeValueProperty(const QVariant &value,
                                                QmlMetaProperty::WriteSource source)
{
    // Remove any existing bindings on this property
    if (source != QmlMetaProperty::Binding) 
        delete q->setBinding(0);

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
        data.coreIndex = valueTypeCoreIdx;
        data.propType = valueTypePropType;
        write(writeBack, data, value, context);

        writeBack->write(object, core.coreIndex);
        if (!ep) delete writeBack;

    } else {

        write(object, core, value, context);

    }
}

void QmlMetaPropertyPrivate::write(QObject *object, const QmlPropertyCache::Data &property, 
                                   const QVariant &value, QmlContext *context)
{
    int coreIdx = property.coreIndex;

    if (property.flags & QmlPropertyCache::Data::IsEnumType) {
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);
        QVariant v = value;
        // Enum values come through the script engine as doubles
        if (value.type() == QVariant::Double) { 
            double integral;
            double fractional = modf(value.toDouble(), &integral);
            if (qFuzzyCompare(fractional, (double)0.0))
                v.convert(QVariant::Int);
        }
        prop.write(object, v);

        return;
    }

    int t = property.propType;
    int vt = value.userType();

    if (t == QVariant::Url) {

        QUrl u;
        bool found = false;
        if (vt == QVariant::Url) {
            u = value.toUrl();
            found = true;
        } else if (vt == QVariant::ByteArray) {
            u = QUrl(QLatin1String(value.toByteArray()));
            found = true;
        } else if (vt == QVariant::String) {
            u = QUrl(value.toString());
            found = true;
        }

        if (found) {
            if (context && u.isRelative() && !u.isEmpty())
                u = context->baseUrl().resolved(u);
            void *a[1];
            a[0] = &u;
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
        }

    } else if (vt == t) {

        void *a[1];
        a[0] = (void *)value.constData();
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (qMetaTypeId<QVariant>() == t) {

        void *a[1];
        a[0] = (void *)&value;
        QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);

    } else if (property.flags & QmlPropertyCache::Data::IsQObjectDerived) {

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

    } else if (property.flags & QmlPropertyCache::Data::IsQList) {

        int listType = QmlMetaType::listType(t);
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);

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

    } else if (property.flags & QmlPropertyCache::Data::IsQmlList) {

        // XXX - optimize!
        QMetaProperty prop = object->metaObject()->property(property.coreIndex);
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
    } else {
        Q_ASSERT(vt != t);

        QVariant v = value;
        if (v.convert((QVariant::Type)t)) {
            void *a[1];
            a[0] = (void *)v.constData();
            QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
        } else if ((uint)t >= QVariant::UserType && vt == QVariant::String) {
            QmlMetaType::StringConverter con = QmlMetaType::customStringConverter(t);
            if (con) {
                QVariant v = con(value.toString());
                if (v.userType() == t) {
                    void *a[1];
                    a[0] = (void *)v.constData();
                    QMetaObject::metacall(object, QMetaObject::WriteProperty, coreIdx, a);
                }
            }
        }
    }
}

/*!
    Set the property value to \a value.
*/
void QmlMetaProperty::write(const QVariant &value) const
{
    write(value, Other);
}

void QmlMetaProperty::write(const QVariant &value, WriteSource source) const
{
    if (!d->object)
        return;

    if (type() & SignalProperty) {

        d->writeSignalProperty(value);

    } else if (d->core.isValid()) {

        d->writeValueProperty(value, source);

    }
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
        rv = d->core.coreIndex;
    }

    Q_ASSERT(rv <= 0x7FF);
    Q_ASSERT(type() <= 0x3F);
    Q_ASSERT(d->valueTypeCoreIdx <= 0x7F);

    rv |= (type() << 18);

    if (type() & ValueTypeProperty)
        rv |= (d->valueTypeCoreIdx << 11);

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
    QmlEnginePrivate *enginePrivate = 0;
    if (ctxt && ctxt->engine())
        enginePrivate = QmlEnginePrivate::get(ctxt->engine());

    d->object = obj;
    d->context = ctxt;

    id &= 0xFFFFFF;
    uint type = id >> 18;
    id &= 0xFFFF;

    if (type & Attached) {
        d->attachedFunc = id;
    } else if (type & ValueTypeProperty) {
        int coreIdx = id & 0x7FF;
        int valueTypeIdx = id >> 11;

        QMetaProperty p(obj->metaObject()->property(coreIdx));
        Q_ASSERT(p.type() < QVariant::UserType);

        QmlValueType *valueType = qmlValueTypes()->valueTypes[p.type()];

        QMetaProperty p2(valueType->metaObject()->property(valueTypeIdx));

        d->core.load(p);
        d->valueTypeCoreIdx = valueTypeIdx;
        d->valueTypePropType = p2.userType();
    } else if (type & Property) {

        QmlPropertyCache *cache = enginePrivate?enginePrivate->cache(obj):0;

        if (cache) {
            QmlPropertyCache::Data *data = cache->property(id);
            if (data) d->core = *data;
        } else {
            QMetaProperty p(obj->metaObject()->property(id));
            d->core.load(p);
        }

    } else if (type & SignalProperty) {

        QMetaMethod method = obj->metaObject()->method(id);
        d->core.load(method);
    } else {
        *this = QmlMetaProperty();
    }
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
            prop.propertyType() < (int)QVariant::UserType &&
            qmlValueTypes()->valueTypes[prop.propertyType()]) {
            // We're now at a value type property
            QObject *typeObject = 
                qmlValueTypes()->valueTypes[prop.propertyType()];
            int idx = typeObject->metaObject()->indexOfProperty(path.last().toUtf8().constData());
            if (idx == -1)
                return QmlMetaProperty();
            QMetaProperty vtProp = typeObject->metaObject()->property(idx);

            QmlMetaProperty p = prop;
            p.d->valueTypeCoreIdx = idx;
            p.d->valueTypePropType = vtProp.userType();
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
