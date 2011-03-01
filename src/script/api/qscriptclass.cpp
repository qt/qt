/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptisolate_p.h"
#include "qscriptclass.h"
#include "qscriptclass_p.h"
#include "qscriptclasspropertyiterator.h"
#include "qscriptengine_p.h"
#include "qscriptstring_p.h"
#include "qscriptvalue_p.h"
#include "qscriptcontext_p.h"
#include <v8.h>
#include "qscriptv8objectwrapper_p.h"
#include "qscript_impl_p.h"

Q_DECLARE_METATYPE(QScriptContext *)

QT_BEGIN_NAMESPACE

static v8::Handle<v8::Value> QtScriptClassToStringCallback(const v8::Arguments& args)
{
    QScriptClassObject *data = QScriptV8ObjectWrapperHelper::getDataPointer<QScriptClassObject>(args);
    QString result = QString::fromAscii("[object %1]").arg(data->scriptClass()->userCallback()->name());
    return QScriptConverter::toString(result);
}

v8::Handle<v8::FunctionTemplate> QScriptClassPrivate::createToStringTemplate()
{
    return v8::FunctionTemplate::New(QtScriptClassToStringCallback);
}

v8::Handle<v8::Value> QScriptClassObject::property(v8::Handle<v8::String> property)
{
    v8::HandleScope handleScope;
    if (!m_scriptclass || (!m_original.IsEmpty() && !m_original->IsUndefined())) {
        // For compatibility with the old back-end, normal JS properties are queried first.
        v8::Handle<v8::Value> originalResult = m_original->Get(property);
        if (!m_scriptclass || (!originalResult.IsEmpty() && !originalResult->IsUndefined())) {
            if (originalResult->IsUndefined() && !m_original->Has(property))
                return handleScope.Close(v8::ThrowException(v8::Exception::ReferenceError(property)));
            return handleScope.Close(originalResult);
        }
    }

    Q_ASSERT(m_scriptclass);

    QScriptString str = QScriptStringPrivate::get(new QScriptStringPrivate(engine, property));
    QScriptValue that = engine->scriptValueFromInternal(engine->currentContext()->thisObject());

    uint id = 0;
    QScriptClass::QueryFlags userFlags =
            m_scriptclass->userCallback()->queryProperty(that, str, QScriptClass::HandlesReadAccess, &id);

    if (!(userFlags & QScriptClass::HandlesReadAccess)) {
        v8::Handle<v8::String> toStringProp = v8::String::New("toString");
        if (property->Equals(toStringProp)) {
            v8::Handle<v8::Object> proto = v8::Handle<v8::Object>::Cast(engine->currentContext()->thisObject()->GetPrototype());
            if (engine->getOwnProperty(proto, toStringProp).IsEmpty()) {
                return handleScope.Close(engine->scriptClassToStringTemplate()->GetFunction());
            }
        }
        return handleScope.Close(v8::Handle<v8::Value>());
    }

    QScriptValue userResult = m_scriptclass->userCallback()->property(that, str, id);
    QScriptValuePrivate* result = QScriptValuePrivate::get(userResult);
    return handleScope.Close(static_cast<v8::Handle<v8::Value> >(result->asV8Value(m_scriptclass->engine())));
}

v8::Handle<v8::Value> QScriptClassObject::property(uint32_t index)
{
    v8::HandleScope handleScope;
    // FIXME it could be faster
    v8::Handle<v8::String> str = QScriptConverter::toString(QString::number(index));
    return handleScope.Close(property(str));
}


v8::Handle<v8::Value> QScriptClassObject::setProperty(v8::Handle<v8::String> property, v8::Local<v8::Value> value)
{
    if (!m_scriptclass) {
        Q_ASSERT(!m_original.IsEmpty());
        bool ret = m_original->Set(property, value);
        return ret ? value : v8::Handle<v8::Value>();
    }

    v8::HandleScope handleScope;

    QScriptString str = QScriptStringPrivate::get(new QScriptStringPrivate(engine, property));
    QScriptValue that = engine->scriptValueFromInternal(engine->currentContext()->thisObject());

    uint id = 0;
    QScriptClass::QueryFlags userFlags =
        m_scriptclass->userCallback()->queryProperty(that, str, QScriptClass::HandlesWriteAccess, &id);

    if (!(userFlags & QScriptClass::HandlesWriteAccess)) {
        if (m_original.IsEmpty())
            setOriginal(v8::Object::New());
        bool ret = m_original->Set(property, value);
        return ret ? value : v8::Handle<v8::Value>();
    }

    m_scriptclass->userCallback()->setProperty(that, str, id, QScriptValuePrivate::get(new QScriptValuePrivate(m_scriptclass->engine(), value)));
    return handleScope.Close(value);
}

v8::Handle<v8::Value> QScriptClassObject::setProperty(uint32_t index, v8::Local<v8::Value> value)
{
    v8::HandleScope handleScope;
    // FIXME it could be faster
    v8::Handle<v8::String> str = QScriptConverter::toString(QString::number(index));
    return handleScope.Close(setProperty(str, value));
}

v8::Handle<v8::Boolean> QScriptClassObject::removeProperty(v8::Handle<v8::String> property)
{
    if (!m_scriptclass) {
        Q_ASSERT(!m_original.IsEmpty());
        bool ret = m_original->Delete(property);
        return v8::Boolean::New(ret);
    }

    v8::HandleScope handleScope;
    QScriptString str = QScriptStringPrivate::get(new QScriptStringPrivate(engine, property));
    QScriptValue that = engine->scriptValueFromInternal(engine->currentContext()->thisObject());

    uint id = 0;
    QScriptClass::QueryFlags userFlags =
        m_scriptclass->userCallback()->queryProperty(that, str, QScriptClass::HandlesWriteAccess, &id);

    if (!(userFlags & QScriptClass::HandlesWriteAccess)) {
        if (m_original.IsEmpty())
            setOriginal(v8::Object::New());
        bool ret = m_original->Delete(property);
        return v8::Boolean::New(ret);
    }

    m_scriptclass->userCallback()->setProperty(that, str, id, QScriptValue());
    return v8::True();
}

v8::Handle<v8::Boolean> QScriptClassObject::removeProperty(uint32_t index)
{
    v8::HandleScope handleScope;
    // FIXME it could be faster
    v8::Handle<v8::String> str = QScriptConverter::toString(QString::number(index));
    return handleScope.Close(removeProperty(str));
}

v8::Handle<v8::Integer> QScriptClassObject::propertyFlags(v8::Handle<v8::String> property)
{
    v8::HandleScope handleScope;
    if (!m_scriptclass)  {
        Q_ASSERT(!m_original.IsEmpty());
        if (m_original->Has(property))
            return v8::Integer::New(QScriptConverter::toPropertyAttributes(engine->getPropertyFlags(m_original, property, QScriptValue::ResolvePrototype)));
        return handleScope.Close(v8::Handle<v8::Integer>());
    }

    QScriptString str = QScriptStringPrivate::get(new QScriptStringPrivate(engine, property));
    QScriptValue that = engine->scriptValueFromInternal(engine->currentContext()->thisObject());

    uint id = 0;
    QScriptClass::QueryFlags userFlags =
            m_scriptclass->userCallback()->queryProperty(that, str, QScriptClass::HandlesReadAccess, &id);

    if (!(userFlags & QScriptClass::HandlesReadAccess))
        return handleScope.Close(v8::Handle<v8::Integer>());
    QScriptValue::PropertyFlags userResult = m_scriptclass->userCallback()->propertyFlags(that, str, id);
    return handleScope.Close(v8::Integer::New(QScriptConverter::toPropertyAttributes(userResult)));
}

v8::Handle<v8::Integer> QScriptClassObject::propertyFlags(uint32_t index)
{
    v8::HandleScope handleScope;
    // FIXME it could be faster
    v8::Handle<v8::String> str = QScriptConverter::toString(QString::number(index));
    return handleScope.Close(propertyFlags(str));
}

v8::Handle<v8::Array> QScriptClassObject::enumerate()
{
    if (m_original.IsEmpty() || !engine) {
        // FIXME Is it possible?
        Q_UNIMPLEMENTED();
        return v8::Handle<v8::Array>();
    }

    v8::HandleScope handleScope;
    v8::Handle<v8::Array> originalNames = engine->getOwnPropertyNames(m_original);
    v8::Handle<v8::Array> names;
    uint32_t idx = 0;
    if (m_scriptclass) {
        QScriptValue that = engine->scriptValueFromInternal(engine->currentContext()->thisObject());
        QScopedPointer<QScriptClassPropertyIterator> iter(m_scriptclass->userCallback()->newIterator(that));
        if (iter) {
            names = v8::Array::New(originalNames->Length()); // Length will be at least equal to that (or bigger).
            while (iter->hasNext()) {
                iter->next();
                QScriptString name = iter->name();
                names->Set(idx++, QScriptStringPrivate::get(name)->asV8Value());
            }
        } else {
            // The value is a script class instance but custom property iterator is not created, so
            // only js properties should be returned.
            return handleScope.Close(originalNames);
        }
    } else
        return handleScope.Close(originalNames);

    // add original names and custom ones
    for (uint32_t i = 0; i < originalNames->Length(); ++i)
        names->Set(idx + i, originalNames->Get(i));
    return handleScope.Close(names);
}

v8::Handle<v8::Value> QScriptClassObject::call(const v8::Arguments& args)
{
    v8::HandleScope handleScope;
    QScriptClassObject *data = QScriptV8ObjectWrapperHelper::getDataPointer<QScriptClassObject>(args.Holder());

    // ### Does it make sense to consider call the original object when there's no scriptclass?
    if (!data->scriptClass()) {
        Q_UNIMPLEMENTED();
        return handleScope.Close(v8::Handle<v8::Value>());
    }

    QScriptClass *userCallback = data->scriptClass()->userCallback();
    if (!userCallback->supportsExtension(QScriptClass::Callable))
        return handleScope.Close(v8::ThrowException(v8::Exception::TypeError(v8::String::New("QScriptClass for object doesn't support Callable extension"))));

    v8::Handle<v8::Object> thisObject;

    // v8 doesn't create a new Object to put in This() when the constructor call came from
    // a callback registered with SetCallAsFunctionHandler().
    if (args.IsConstructCall()) {
        thisObject = v8::Object::New();
        v8::Handle<v8::Value> proto = data->property(v8::String::New("prototype"));
        if (!proto.IsEmpty() && proto->IsObject())
            thisObject->SetPrototype(proto);
    }

    QScriptContextPrivate qScriptContext(data->engine, &args, args.Holder(), thisObject);
    QScriptContext *ctx = &qScriptContext;
    Q_ASSERT(ctx == data->engine->currentContext());

    v8::Handle<v8::Value> result = data->engine->variantToJS(userCallback->extension(QScriptClass::Callable, qVariantFromValue(ctx)));
    if (args.IsConstructCall() && (result.IsEmpty() || !result->IsObject()))
        return handleScope.Close(thisObject);
    return handleScope.Close(result);
}

v8::Handle<v8::FunctionTemplate> QScriptClassObject::createFunctionTemplate(QScriptEnginePrivate *engine)
{
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
    v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
    instTempl->SetInternalFieldCount(1);

    //FIXME: fully implement both!
    instTempl->SetNamedPropertyHandler(QScriptV8ObjectWrapperHelper::namedPropertyGetter<QScriptClassObject>,
                                       QScriptV8ObjectWrapperHelper::namedPropertySetter<QScriptClassObject>,
                                       QScriptV8ObjectWrapperHelper::namedPropertyQuery<QScriptClassObject>,
                                       QScriptV8ObjectWrapperHelper::namedPropertyDeleter<QScriptClassObject>,
                                       QScriptV8ObjectWrapperHelper::namedPropertyEnumerator<QScriptClassObject>);

    instTempl->SetIndexedPropertyHandler(QScriptV8ObjectWrapperHelper::indexedPropertyGetter<QScriptClassObject>,
                                        QScriptV8ObjectWrapperHelper::indexedPropertySetter<QScriptClassObject>,
                                        QScriptV8ObjectWrapperHelper::indexedPropertyQuery<QScriptClassObject>,
                                        QScriptV8ObjectWrapperHelper::indexedPropertyDeleter<QScriptClassObject>,
                                        QScriptV8ObjectWrapperHelper::indexedPropertyEnumerator<QScriptClassObject>);

    instTempl->SetCallAsFunctionHandler(QScriptClassObject::call);

    return handleScope.Close(funcTempl);
}


v8::Handle<v8::Value> QScriptClassObject::newInstance(QScriptClassPrivate* scriptclass, v8::Handle<v8::Object> previousValue, QScriptEnginePrivate* engine)
{
    QScriptClassObject *data = new QScriptClassObject;
    data->engine = engine;
    data->m_scriptclass = scriptclass;
    if (!previousValue.IsEmpty() && !previousValue->IsUndefined())
        data->setOriginal(previousValue);

    v8::Handle<v8::Object> instance = createInstance(data);

    QScriptSharedDataPointer<QScriptValuePrivate> prototype(QScriptValuePrivate::get(QScriptClassPrivate::get(scriptclass)->prototype()));
    if (prototype->isValid() && (prototype->isObject() || prototype->isNull())) {
        if (!prototype->engine() || prototype->engine() == engine) {
            instance->SetPrototype(prototype->asV8Value(engine));
        } else {
            qWarning("QScriptValue::setPrototype() failed: cannot set a prototype created in a different engine");
        }
    }
    return instance;
}

/*!
  \since 4.4
  \class QScriptClass

  \brief The QScriptClass class provides an interface for defining custom behavior of (a class of) Qt Script objects.

  \ingroup script
  \mainclass

  The QScriptClass class defines an interface for handling various
  aspects of interaction with the Qt Script objects associated with
  the class. Such objects are created by calling
  QScriptEngine::newObject(), passing a pointer to the QScriptClass as
  argument.

  By subclassing QScriptClass, you can define precisely how access to
  properties of the objects that use your class is handled. This
  enables a fully dynamic handling of properties, e.g. it's more
  powerful than QScriptEngine::newQObject(). For example, you can use
  QScriptClass to implement array-type objects (i.e. objects that
  handle the \c{length} property, and properties whose names are valid
  array indexes, in a special way), or to implement a "live"
  (runtime-defined) proxy to an underlying object.

  If you just need to handle access to a set of properties that are
  known at the time an object is created (i.e. "semi-statically"), you
  might consider using QScriptValue::setProperty() to define
  getter/setter functions for the relevant properties, rather than
  subclassing QScriptClass.

  Reimplement queryProperty() to specify which properties are handled
  in a custom way by your script class (i.e. should be
  \bold{delegated} to the QScriptClass), and which properties should
  be handled just like normal Qt Script object properties.

  Reimplement property() and setProperty() to perform the actual
  access (read or write) to the properties that your class
  handles. Additionally, you can reimplement propertyFlags() to
  specify custom flags for your properties.

  Reimplement newIterator() to provide an iterator for objects of your
  custom class. This is only necessary if objects of your class can
  have custom properties that you want to be reported when an object
  is used together with the QScriptValueIterator class, or when an
  object is used in a for-in enumeration statement in a script.

  When implementing custom classes of objects, you typically use
  QScriptValue::setData() to store instance-specific data as part of
  object initialization; the data won't be accessible from scripts
  directly, but you can access it in e.g. your reimplementations of
  property() and setProperty() (by calling QScriptValue::data()) to
  perform custom processing.

  Reimplement prototype() to provide a custom prototype object for
  your script class.

  Reimplement supportsExtension() and extension() if your custom
  script class supports one or more of the extensions specified by the
  Extension enum.

  \sa QScriptClassPropertyIterator, QScriptEngine::newObject(), {Custom Script Class Example}
*/

/*!
    \enum QScriptClass::Extension

    This enum specifies the possible extensions to a QScriptClass.

    \value Callable Instances of this class can be called as functions.

    \value HasInstance Instances of this class implement [[HasInstance]].

    \sa extension()
*/

/*!
    \enum QScriptClass::QueryFlag

    This enum describes flags that are used to query a QScriptClass
    regarding how access to a property should be handled.

    \value HandlesReadAccess The QScriptClass handles read access to this property.
    \value HandlesWriteAccess The QScriptClass handles write access to this property.

    \sa queryProperty()
*/

/*!
  Constructs a QScriptClass object to be used in the given \a engine.

  The engine does not take ownership of the QScriptClass object.
*/
QScriptClass::QScriptClass(QScriptEngine *engine)
    : d_ptr(new QScriptClassPrivate(engine ? QScriptEnginePrivate::get(engine) : 0, this))
{
}

/*!
  Destroys the QScriptClass object.

  If a QScriptClass object is deleted before the associated engine(),
  any Qt Script objects using the QScriptClass will be "demoted" to
  normal Qt Script objects.
*/
QScriptClass::~QScriptClass()
{
}

/*!
  Returns the engine that this QScriptClass is associated with.
*/
QScriptEngine *QScriptClass::engine() const
{
    return d_ptr->engine() ? QScriptEnginePrivate::get(d_ptr->engine()) : 0;
}

/*!
  Returns the object to be used as the prototype of new instances
  of this class (created with QScriptEngine::newObject()).

  The default implementation returns an invalid QScriptValue, meaning
  that the standard Object prototype will be used.  Reimplement this
  function to provide your own custom prototype.

  Typically you initialize your prototype object in the constructor of
  your class, then return it in this function.

  See the "Making Use of Prototype-Based Inheritance" section in the
  QtScript documentation for more information on how prototypes are
  used.
*/
QScriptValue QScriptClass::prototype() const
{
    return QScriptValue();
}

/*!
  Returns the name of the script class.

  Qt Script uses this name to generate a default string representation
  of objects in case you do not provide a toString function.

  The default implementation returns a null string.
*/
QString QScriptClass::name() const
{
    return QString();
}

/*!
  Queries this script class for how access to the property with the
  given \a name of the given \a object should be handled. The given \a
  flags specify the aspects of interest. This function should return a
  subset of \a flags to indicate which aspects of property access
  should be further handled by the script class.

  For example, if the \a flags contain HandlesReadAccess, and you
  would like your class to handle the reading of the property (through
  the property() function), the returned flags should include
  HandlesReadAccess. If the returned flags do not contain
  HandlesReadAccess, the property will be handled as a normal script
  object property.

  You can optionally use the \a id argument to store a value that will
  subsequently be passed on to functions such as property() and
  setProperty().

  The default implementation of this function returns 0.

  Note: This function is only called if the given property isn't
  already a normal property of the object. For example, say you
  advertise that you want to handle read access to property \c{foo},
  but not write access; if \c{foo} is then assigned a value, it will
  become a normal script object property, and subsequently you will no
  longer be queried regarding read access to \c{foo}.

  \sa property()
*/
QScriptClass::QueryFlags QScriptClass::queryProperty(
    const QScriptValue &object, const QScriptString &name,
    QueryFlags flags, uint *id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    Q_UNUSED(id);
    return 0;
}

/*!
  Returns the value of the property with the given \a name of the given
  \a object.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation does nothing and returns an invalid QScriptValue.

  \sa setProperty(), propertyFlags()
*/
QScriptValue QScriptClass::property(const QScriptValue &object,
                                    const QScriptString &name, uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    return QScriptValue();
}

/*!
  Returns the flags of the property with the given \a name of the given
  \a object.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation returns 0.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptClass::propertyFlags(
    const QScriptValue &object, const QScriptString &name, uint id)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    return 0;
}

/*!
  Sets the property with the given \a name of the given \a object to
  the given \a value.

  The \a id argument is only useful if you assigned a value to it in
  queryProperty().

  The default implementation does nothing.

  An invalid \a value represents a request to remove the property.

  \sa property()
*/
void QScriptClass::setProperty(QScriptValue &object, const QScriptString &name,
                               uint id, const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(id);
    Q_UNUSED(value);
}

/*!
  Returns an iterator for traversing custom properties of the given \a
  object.

  The default implementation returns 0, meaning that there are no
  custom properties to traverse.

  Reimplement this function if objects of your script class can have
  one or more custom properties (e.g. those reported to be handled by
  queryProperty()) that you want to appear when an object's properties
  are enumerated (e.g. by a for-in statement in a script).

  Qt Script takes ownership of the new iterator object.

  \sa QScriptValueIterator
*/
QScriptClassPropertyIterator *QScriptClass::newIterator(const QScriptValue &object)
{
    Q_UNUSED(object);
    return 0;
}

/*!
  Returns true if the QScriptClass supports the given \a extension;
  otherwise, false is returned. By default, no extensions
  are supported.

  Reimplement this function to indicate which extensions your custom
  class supports.

  \sa extension()
*/
bool QScriptClass::supportsExtension(Extension extension) const
{
    Q_UNUSED(extension);
    return false;
}

/*!
  This virtual function can be reimplemented in a QScriptClass
  subclass to provide support for extensions. The optional \a argument
  can be provided as input to the \a extension; the result must be
  returned in the form of a QVariant. You can call supportsExtension()
  to check if an extension is supported by the QScriptClass.  By
  default, no extensions are supported, and this function returns an
  invalid QVariant.

  If you implement the Callable extension, Qt Script will call this
  function when an instance of your class is called as a function
  (e.g. from a script or using QScriptValue::call()).  The \a argument
  will contain a pointer to the QScriptContext that represents the
  function call, and you should return a QVariant that holds the
  result of the function call. In the following example the sum of the
  arguments to the script function are added up and returned:

  \snippet doc/src/snippets/code/src_script_qscriptclass.cpp 0

  If you implement the HasInstance extension, Qt Script will call this
  function as part of evaluating the \c{instanceof} operator, as
  described in ECMA-262 Section 11.8.6. The \a argument is a
  QScriptValueList containing two items: The first item is the object
  that HasInstance is being applied to (an instance of your class),
  and the second item can be any value. extension() should return true
  if the value delegates behavior to the object, false otherwise.

  \sa supportsExtension()
*/
QVariant QScriptClass::extension(Extension extension, const QVariant &argument)
{
    Q_UNUSED(extension);
    Q_UNUSED(argument);
    return QVariant();
}

QT_END_NAMESPACE
