/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qscriptvalue.h"

#ifndef QT_NO_SCRIPT

#include "qscriptvalue_p.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"

#include "JSArray.h"
#include "JSGlobalObject.h"
#include "JSImmediate.h"
#include "JSObject.h"
#include "JSValue.h"
#include "JSFunction.h"
#include "DateInstance.h"
#include "ErrorInstance.h"
#include "RegExpObject.h"
#include "Identifier.h"
#include "Operations.h"
#include "Arguments.h"

#include <QtCore/qdatetime.h>
#include <QtCore/qregexp.h>
#include <QtCore/qvariant.h>
#include <QtCore/qnumeric.h>

#include "utils/qscriptdate_p.h"
#include "bridge/qscriptvariant_p.h"
#include "bridge/qscriptqobject_p.h"

QT_BEGIN_NAMESPACE

/*!
  \since 4.3
  \class QScriptValue

  \brief The QScriptValue class acts as a container for the Qt Script data types.

  \ingroup script
  \mainclass

  QScriptValue supports the types defined in the \l{ECMA-262}
  standard: The primitive types, which are Undefined, Null, Boolean,
  Number, and String; and the Object type. Additionally, Qt Script
  has built-in support for QVariant, QObject and QMetaObject.

  For the object-based types (including Date and RegExp), use the
  newT() functions in QScriptEngine (e.g. QScriptEngine::newObject())
  to create a QScriptValue of the desired type. For the primitive types,
  use one of the QScriptValue constructor overloads.

  The methods named isT() (e.g. isBool(), isUndefined()) can be
  used to test if a value is of a certain type. The methods named
  toT() (e.g. toBool(), toString()) can be used to convert a
  QScriptValue to another type. You can also use the generic
  qscriptvalue_cast() function.

  Object values have zero or more properties which are themselves
  QScriptValues. Use setProperty() to set a property of an object, and
  call property() to retrieve the value of a property.

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 0

  Each property can have a set of attributes; these are specified as
  the third (optional) argument to setProperty(). The attributes of a
  property can be queried by calling the propertyFlags() function. The
  following code snippet creates a property that cannot be modified by
  script code:

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 1

  If you want to iterate over the properties of a script object, use
  the QScriptValueIterator class.

  Object values have an internal \c{prototype} property, which can be
  accessed with prototype() and setPrototype(). Properties added to a
  prototype are shared by all objects having that prototype; this is
  referred to as prototype-based inheritance. In practice, it means
  that (by default) the property() function will automatically attempt
  to look up look the property in the prototype() (and in the
  prototype of the prototype(), and so on), if the object itself does
  not have the requested property. Note that this prototype-based
  lookup is not performed by setProperty(); setProperty() will always
  create the property in the script object itself.  For more
  information, see the \l{QtScript} documentation.

  Function objects (objects for which isFunction() returns true) can
  be invoked by calling call(). Constructor functions can be used to
  construct new objects by calling construct().

  Use equals(), strictlyEquals() and lessThan() to compare a QScriptValue
  to another.

  Object values can have custom data associated with them; see the
  setData() and data() functions. By default, this data is not
  accessible to scripts; it can be used to store any data you want to
  associate with the script object. Typically this is used by custom
  class objects (see QScriptClass) to store a C++ type that contains
  the "native" object data.

  Note that a QScriptValue for which isObject() is true only carries a
  reference to an actual object; copying the QScriptValue will only
  copy the object reference, not the object itself. If you want to
  clone an object (i.e. copy an object's properties to another
  object), you can do so with the help of a \c{for-in} statement in
  script code, or QScriptValueIterator in C++.

  \sa QScriptEngine, QScriptValueIterator
*/

/*!
    \enum QScriptValue::SpecialValue

    This enum is used to specify a single-valued type.

    \value UndefinedValue An undefined value.

    \value NullValue A null value.
*/

/*!
    \enum QScriptValue::PropertyFlag

    This enum describes the attributes of a property.

    \value ReadOnly The property is read-only. Attempts by Qt Script code to write to the property will be ignored.

    \value Undeletable Attempts by Qt Script code to \c{delete} the property will be ignored.

    \value SkipInEnumeration The property is not to be enumerated by a \c{for-in} enumeration.

    \value PropertyGetter The property is defined by a function which will be called to get the property value.

    \value PropertySetter The property is defined by a function which will be called to set the property value.

    \value QObjectMember This flag is used to indicate that an existing property is a QObject member (a property or method).

    \value KeepExistingFlags This value is used to indicate to setProperty() that the property's flags should be left unchanged. If the property doesn't exist, the default flags (0) will be used.

    \value UserRange Flags in this range are not used by Qt Script, and can be used for custom purposes.
*/

/*!
    \enum QScriptValue::ResolveFlag

    This enum specifies how to look up a property of an object.

    \value ResolveLocal Only check the object's own properties.

    \value ResolvePrototype Check the object's own properties first, then search the prototype chain. This is the default.

    \value ResolveScope Check the object's own properties first, then search the scope chain.

    \value ResolveFull Check the object's own properties first, then search the prototype chain, and finally search the scope chain.
*/

// ### move

#include <QtCore/qnumeric.h>
#include <math.h>

namespace QScript
{

static const qsreal D32 = 4294967296.0;

qint32 ToInt32(qsreal n)
{
    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D32);
    const double D31 = D32 / 2.0;

    if (sign == -1 && n < -D31)
        n += D32;

    else if (sign != -1 && n >= D31)
        n -= D32;

    return qint32 (n);
}

quint32 ToUint32(qsreal n)
{
    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D32);

    if (n < 0)
        n += D32;

    return quint32 (n);
}

quint16 ToUint16(qsreal n)
{
    static const qsreal D16 = 65536.0;

    if (qIsNaN(n) || qIsInf(n) || (n == 0))
        return 0;

    double sign = (n < 0) ? -1.0 : 1.0;
    qsreal abs_n = fabs(n);

    n = ::fmod(sign * ::floor(abs_n), D16);

    if (n < 0)
        n += D16;

    return quint16 (n);
}

qsreal ToInteger(qsreal n)
{
    if (qIsNaN(n))
        return 0;

    if (n == 0 || qIsInf(n))
        return n;

    int sign = n < 0 ? -1 : 1;
    return sign * ::floor(::fabs(n));
}

} // namespace QScript

QScriptValuePrivate::QScriptValuePrivate() : engine(this), valid(true), id(-1)
{
    ref = 0;
}

QScriptValuePrivate::~QScriptValuePrivate()
{
}

QScriptValuePrivate::QScriptValueAutoRegister::QScriptValueAutoRegister(QScriptValuePrivate *value,const QScriptEngine *engine):
    val(value), ptr(const_cast<QScriptEngine*>(engine))
{
    QScriptEnginePrivate::get(ptr)->registerScriptValue(val);
}

QScriptValuePrivate::QScriptValueAutoRegister::~QScriptValueAutoRegister()
{
    if (ptr) QScriptEnginePrivate::get(ptr)->unregisterScriptValue(val);
}

QScriptValuePrivate::QScriptValueAutoRegister& QScriptValuePrivate::QScriptValueAutoRegister::operator=(const QScriptEngine *pointer)
{
    if (ptr)
        QScriptEnginePrivate::get(ptr)->unregisterScriptValue(val);
    ptr = const_cast<QScriptEngine*> (pointer);
    if (ptr)
        QScriptEnginePrivate::get(ptr)->registerScriptValue(val);
    return *this;
};


void QScriptValuePrivate::initFromJSCValue(JSC::JSValue value)
{
    type = JSC;
    jscValue = value;
    if (value.isCell()) {
        JSC::JSCell *cell = value.asCell();
        Q_ASSERT(engine != 0);
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        if (value != eng_p->globalObject) {
            if (!eng_p->keepAliveValues.contains(cell))
                eng_p->keepAliveValues[cell] = 0;
            eng_p->keepAliveValues[cell].ref();
        }
    }
}

void QScriptValuePrivate::initFromNumber(double value)
{
    type = Number;
    numberValue = value;
}

void QScriptValuePrivate::initFromString(const QString &value)
{
    type = String;
    stringValue = new QString(value);
}

void QScriptValuePrivate::initFromJSCValue(QScriptValue &result,
                                           QScriptEngine *engine,
                                           JSC::JSValue value)
{
    Q_ASSERT(!result.isValid());
    result.d_ptr = new QScriptValuePrivate();
    result.d_ptr->engine = engine;
    result.d_ptr->initFromJSCValue(value);
    result.d_ptr->ref.ref();
}

bool QScriptValuePrivate::isJSC() const
{
    return (type == JSC);
}

QScriptValuePrivate *QScriptValuePrivate::get(const QScriptValue &q)
{
    return q.d_ptr;
}

namespace QScript
{
JSC::UString qtStringToJSCUString(const QString &str);
QString qtStringFromJSCUString(const JSC::UString &str);
bool isFunction(JSC::JSValue value);
}

QScriptValue QScriptValuePrivate::property(const QString &name, int resolveMode) const
{
    Q_ASSERT(type == JSC);
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::UString jscName = QScript::qtStringToJSCUString(name);
    JSC::Identifier id = JSC::Identifier(exec, jscName);
    JSC::JSObject *object = jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;
    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
    } else if ((resolveMode & QScriptValue::ResolvePrototype)
          && const_cast<JSC::JSObject*>(object)->getPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
    }
    return eng_p->scriptValueFromJSCValue(result);
}

QScriptValue QScriptValuePrivate::property(quint32 index, int resolveMode) const
{
    Q_ASSERT(type == JSC);
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::JSObject *object = jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;
    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, index, slot)) {
        result = slot.getValue(exec, index);
    } else if ((resolveMode & QScriptValue::ResolvePrototype)
          && const_cast<JSC::JSObject*>(object)->getPropertySlot(exec, index, slot)) {
        result = slot.getValue(exec, index);
    }
    return eng_p->scriptValueFromJSCValue(result);
}

QVariant &QScriptValuePrivate::variantValue() const
{
    return static_cast<QScript::QVariantWrapperObject*>(JSC::asObject(jscValue))->value();
}

void QScriptValuePrivate::setVariantValue(const QVariant &value)
{
    static_cast<QScript::QVariantWrapperObject*>(JSC::asObject(jscValue))->setValue(value);
}

/*!
  Constructs an invalid QScriptValue.
*/
QScriptValue::QScriptValue()
    : d_ptr(0)
{
}

/*!
  Destroys this QScriptValue.
*/
QScriptValue::~QScriptValue()
{
    if (d_ptr && !d_ptr->ref.deref()) {
        if (d_ptr->engine && d_ptr->isJSC()
            && !JSC::JSImmediate::isImmediate(d_ptr->jscValue)) {
            QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d_ptr->engine);
            if (d_ptr->jscValue != eng_p->globalObject)
                eng_p->releaseJSCValue(d_ptr->jscValue);
        }
        delete d_ptr;
        d_ptr = 0;
    }
}

/*!
  Constructs a new QScriptValue that is a copy of \a other.

  Note that if \a other is an object (i.e., isObject() would return
  true), then only a reference to the underlying object is copied into
  the new script value (i.e., the object itself is not copied).
*/
QScriptValue::QScriptValue(const QScriptValue &other)
    : d_ptr(other.d_ptr)
{
    if (d_ptr)
        d_ptr->ref.ref();
}

/*!
  \obsolete 

  Constructs a new QScriptValue with the special \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, QScriptValue::SpecialValue value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    switch (value) {
    case NullValue:
        d_ptr->initFromJSCValue(JSC::jsNull());
        break;
    case UndefinedValue:
        d_ptr->initFromJSCValue(JSC::jsUndefined());
        break;
    }
    d_ptr->ref.ref();
}

/*!
  \obsolete

  \fn QScriptValue::QScriptValue(QScriptEngine *engine, bool value)

  Constructs a new QScriptValue with the boolean \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, bool val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    d_ptr->initFromJSCValue(JSC::jsBoolean(val));
    d_ptr->ref.ref();
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, int value)
  \obsolete

  Constructs a new QScriptValue with the integer \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, int val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        d_ptr->initFromJSCValue(JSC::jsNumber(exec, val));
    } else {
        JSC::JSValue immediate = JSC::JSImmediate::from(val);
        if (immediate)
            d_ptr->initFromJSCValue(immediate);
        else
            d_ptr->initFromNumber(val);
    }
    d_ptr->ref.ref();
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, uint value)
  \obsolete

  Constructs a new QScriptValue with the unsigned integer \a value and
  registers it with the script \a engine.
 */
QScriptValue::QScriptValue(QScriptEngine *engine, uint val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        d_ptr->initFromJSCValue(JSC::jsNumber(exec, val));
    } else {
        JSC::JSValue immediate = JSC::JSImmediate::from(val);
        if (immediate)
            d_ptr->initFromJSCValue(immediate);
        else
            d_ptr->initFromNumber(val);
    }
    d_ptr->ref.ref();
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, qsreal value)
  \obsolete

  Constructs a new QScriptValue with the qsreal \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, qsreal val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        d_ptr->initFromJSCValue(JSC::jsNumber(exec, val));
    } else {
        JSC::JSValue immediate = JSC::JSImmediate::from(val);
        if (immediate)
            d_ptr->initFromJSCValue(immediate);
        else
            d_ptr->initFromNumber(val);
    }
    d_ptr->ref.ref();
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, const QString &value)
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine *engine, const QString &val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        JSC::UString jscVal = QScript::qtStringToJSCUString(val);
        d_ptr->initFromJSCValue(JSC::jsString(exec, jscVal));
    } else {
        d_ptr->initFromString(val);
    }
    d_ptr->ref.ref();
}

/*!
  \fn QScriptValue::QScriptValue(QScriptEngine *engine, const char *value)
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/

#ifndef QT_NO_CAST_FROM_ASCII
QScriptValue::QScriptValue(QScriptEngine *engine, const char *val)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = engine;
    if (engine) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        d_ptr->initFromJSCValue(JSC::jsString(exec, val));
    } else {
        d_ptr->initFromString(QString::fromAscii(val));
    }
    d_ptr->ref.ref();
}
#endif

/*!
  \since 4.5

  Constructs a new QScriptValue with a special \a value.
*/
QScriptValue::QScriptValue(SpecialValue value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    switch (value) {
    case NullValue:
        d_ptr->initFromJSCValue(JSC::jsNull());
        break;
    case UndefinedValue:
        d_ptr->initFromJSCValue(JSC::jsUndefined());
        break;
    }
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a boolean \a value.
*/
QScriptValue::QScriptValue(bool value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    d_ptr->initFromJSCValue(JSC::jsBoolean(value));
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(int value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    JSC::JSValue immediate = JSC::JSImmediate::from(value);
    if (immediate)
        d_ptr->initFromJSCValue(immediate);
    else
        d_ptr->initFromNumber(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(uint value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    JSC::JSValue immediate = JSC::JSImmediate::from(value);
    if (immediate)
        d_ptr->initFromJSCValue(immediate);
    else
        d_ptr->initFromNumber(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(qsreal value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    JSC::JSValue immediate = JSC::JSImmediate::from(value);
    if (immediate)
        d_ptr->initFromJSCValue(immediate);
    else
        d_ptr->initFromNumber(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QString &value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    d_ptr->initFromString(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QLatin1String &value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    d_ptr->initFromString(value);
    d_ptr->ref.ref();
}

/*!
  \since 4.5

  Constructs a new QScriptValue with a string \a value.
*/

#ifndef QT_NO_CAST_FROM_ASCII
QScriptValue::QScriptValue(const char *value)
    : d_ptr(new QScriptValuePrivate)
{
    d_ptr->engine = 0;
    d_ptr->initFromString(QString::fromAscii(value));
    d_ptr->ref.ref();
}
#endif

/*!
  Assigns the \a other value to this QScriptValue.

  Note that if \a other is an object (isObject() returns true),
  only a reference to the underlying object will be assigned;
  the object itself will not be copied.
*/
QScriptValue &QScriptValue::operator=(const QScriptValue &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        if (d_ptr->engine && d_ptr->isJSC()
            && !JSC::JSImmediate::isImmediate(d_ptr->jscValue)) {
            QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d_ptr->engine);
            eng_p->releaseJSCValue(d_ptr->jscValue);
        }
        delete d_ptr;
    }
    d_ptr = other.d_ptr;
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns true if this QScriptValue is an object of the Error class;
  otherwise returns false.

  \sa QScriptContext::throwError()
*/
bool QScriptValue::isError() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return d->jscValue.isObject(&JSC::ErrorInstance::info);
}

/*!
  Returns true if this QScriptValue is an object of the Array class;
  otherwise returns false.

  \sa QScriptEngine::newArray()
*/
bool QScriptValue::isArray() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return d->jscValue.isObject(&JSC::JSArray::info);
}

/*!
  Returns true if this QScriptValue is an object of the Date class;
  otherwise returns false.

  \sa QScriptEngine::newDate()
*/
bool QScriptValue::isDate() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return d->jscValue.isObject(&JSC::DateInstance::info);
}

/*!
  Returns true if this QScriptValue is an object of the RegExp class;
  otherwise returns false.

  \sa QScriptEngine::newRegExp()
*/
bool QScriptValue::isRegExp() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return d->jscValue.isObject(&JSC::RegExpObject::info);
}

/*!
  If this QScriptValue is an object, returns the internal prototype
  (\c{__proto__} property) of this object; otherwise returns an
  invalid QScriptValue.

  \sa setPrototype(), isObject()
*/
QScriptValue QScriptValue::prototype() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return QScriptValue();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    return eng_p->scriptValueFromJSCValue(JSC::asObject(d->jscValue)->prototype());
}

/*!
  If this QScriptValue is an object, sets the internal prototype
  (\c{__proto__} property) of this object to be \a prototype;
  otherwise does nothing.

  The internal prototype should not be confused with the public
  property with name "prototype"; the public prototype is usually
  only set on functions that act as constructors.

  \sa prototype(), isObject()
*/
void QScriptValue::setPrototype(const QScriptValue &prototype)
{
    Q_D(QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    if (prototype.isValid() && prototype.engine()
        && (prototype.engine() != engine())) {
        qWarning("QScriptValue::setPrototype() failed: "
                 "cannot set a prototype created in "
                 "a different engine");
        return;
    }
    // ### check for cycle
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::JSValue other = eng_p->scriptValueToJSCValue(prototype);
    JSC::asObject(d->jscValue)->setPrototype(other);
}

/*!
  \internal
*/
QScriptValue QScriptValue::scope() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return QScriptValue();
    // ### make hidden property
    return d->property(QLatin1String("__qt_scope__"), QScriptValue::ResolveLocal);
}

/*!
  \internal
*/
void QScriptValue::setScope(const QScriptValue &scope)
{
    Q_D(QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    if (scope.isValid() && scope.engine()
        && (scope.engine() != engine())) {
        qWarning("QScriptValue::setScope() failed: "
                 "cannot set a scope object created in "
                 "a different engine");
        return;
    }
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::JSValue other = eng_p->scriptValueToJSCValue(scope);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::Identifier id = JSC::Identifier(exec, "__qt_scope__");
    if (!scope.isValid()) {
        JSC::asObject(d->jscValue)->removeDirect(id);
    } else {
        // ### make hidden property
        JSC::asObject(d->jscValue)->putDirect(id, other);
    }
}

/*!
  Returns true if this QScriptValue is an instance of
  \a other; otherwise returns false.

  This QScriptValue is considered to be an instance of \a other if
  \a other is a function and the value of the \c{prototype}
  property of \a other is in the prototype chain of this
  QScriptValue.
*/
bool QScriptValue::instanceOf(const QScriptValue &other) const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject() || !other.isObject())
        return false;
    if (other.engine() != engine()) {
        qWarning("QScriptValue::instanceof: "
                 "cannot perform operation on a value created in "
                 "a different engine");
        return false;
    }
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::JSValue jscProto = eng_p->scriptValueToJSCValue(other.property(QLatin1String("prototype")));
    if (!jscProto)
        jscProto = JSC::jsUndefined();
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::JSValue jscOther = eng_p->scriptValueToJSCValue(other);
    return JSC::asObject(jscOther)->hasInstance(exec, d->jscValue, jscProto);
}

// ### move

namespace QScript
{

enum Type {
    Undefined,
    Null,
    Boolean,
    String,
    Number,
    Object
};

static Type type(const QScriptValue &v)
{
    if (v.isUndefined())
        return Undefined;
    else if (v.isNull())
        return Null;
    else if (v.isBoolean())
        return Boolean;
    else if (v.isString())
        return String;
    else if (v.isNumber())
        return Number;
    Q_ASSERT(v.isObject());
    return Object;
}

QScriptValue ToPrimitive(const QScriptValue &object, JSC::PreferredPrimitiveType hint = JSC::NoPreference)
{
    Q_ASSERT(object.isObject());
    QScriptValuePrivate *pp = QScriptValuePrivate::get(object);
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(pp->engine);
    JSC::ExecState *exec = eng_p->currentFrame;
    return eng_p->scriptValueFromJSCValue(JSC::asObject(pp->jscValue)->toPrimitive(exec, hint));
}

static bool IsNumerical(const QScriptValue &value)
{
    return value.isNumber() || value.isBool();
}

static bool LessThan(QScriptValue lhs, QScriptValue rhs)
{
    if (type(lhs) == type(rhs)) {
        switch (type(lhs)) {
        case Undefined:
        case Null:
            return false;

        case Number:
#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
            if (qIsNaN(lhs.toNumber()) || qIsNaN(rhs.toNumber()))
                return false;
#endif
            return lhs.toNumber() < rhs.toNumber();

        case Boolean:
            return lhs.toBool() < rhs.toBool();

        case String:
            return lhs.toString() < rhs.toString();

        case Object:
            break;
        } // switch
    }

    if (lhs.isObject())
        lhs = ToPrimitive(lhs, JSC::PreferNumber);

    if (rhs.isObject())
        rhs = ToPrimitive(rhs, JSC::PreferNumber);

    if (lhs.isString() && rhs.isString())
        return lhs.toString() < rhs.toString();

    qsreal n1 = lhs.toNumber();
    qsreal n2 = rhs.toNumber();
#if defined Q_CC_MSVC && !defined Q_CC_MSVC_NET
    if (qIsNaN(n1) || qIsNaN(n2))
        return false;
#endif
    return n1 < n2;
}

static bool Equals(QScriptValue lhs, QScriptValue rhs)
{
    if (type(lhs) == type(rhs)) {
        switch (type(lhs)) {
        case QScript::Undefined:
        case QScript::Null:
            return true;

        case QScript::Number:
            return lhs.toNumber() == rhs.toNumber();

        case QScript::Boolean:
            return lhs.toBool() == rhs.toBool();

        case QScript::String:
            return lhs.toString() == rhs.toString();

        case QScript::Object:
            if (lhs.isVariant())
                return lhs.strictlyEquals(rhs) || (lhs.toVariant() == rhs.toVariant());
#ifndef QT_NO_QOBJECT
            else if (lhs.isQObject())
                return (lhs.strictlyEquals(rhs)) || (lhs.toQObject() == rhs.toQObject());
#endif
            else
                return lhs.strictlyEquals(rhs);
        }
    }

    if (lhs.isNull() && rhs.isUndefined())
        return true;

    else if (lhs.isUndefined() && rhs.isNull())
        return true;

    else if (IsNumerical(lhs) && rhs.isString())
        return lhs.toNumber() == rhs.toNumber();

    else if (lhs.isString() && IsNumerical(rhs))
        return lhs.toNumber() == rhs.toNumber();

    else if (lhs.isBool())
        return Equals(lhs.toNumber(), rhs);

    else if (rhs.isBool())
        return Equals(lhs, rhs.toNumber());

    else if (lhs.isObject() && !rhs.isNull()) {
        lhs = ToPrimitive(lhs);

        if (lhs.isValid() && !lhs.isObject())
            return Equals(lhs, rhs);
    }

    else if (rhs.isObject() && ! lhs.isNull()) {
        rhs = ToPrimitive(rhs);
        if (rhs.isValid() && !rhs.isObject())
            return Equals(lhs, rhs);
    }

    return false;
}

} // namespace QScript

/*!
  Returns true if this QScriptValue is less than \a other, otherwise
  returns false.  The comparison follows the behavior described in
  \l{ECMA-262} section 11.8.5, "The Abstract Relational Comparison
  Algorithm".

  Note that if this QScriptValue or the \a other value are objects,
  calling this function has side effects on the script engine, since
  the engine will call the object's valueOf() function (and possibly
  toString()) in an attempt to convert the object to a primitive value
  (possibly resulting in an uncaught script exception).

  \sa equals()
*/
bool QScriptValue::lessThan(const QScriptValue &other) const
{
    // no equivalent function in JSC? There's a jsLess() in VM/Machine.cpp
    if (!isValid() || !other.isValid())
        return false;
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::lessThan: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    return QScript::LessThan(*this, other);
}

/*!
  Returns true if this QScriptValue is equal to \a other, otherwise
  returns false. The comparison follows the behavior described in
  \l{ECMA-262} section 11.9.3, "The Abstract Equality Comparison
  Algorithm".

  This function can return true even if the type of this QScriptValue
  is different from the type of the \a other value; i.e. the
  comparison is not strict.  For example, comparing the number 9 to
  the string "9" returns true; comparing an undefined value to a null
  value returns true; comparing a \c{Number} object whose primitive
  value is 6 to a \c{String} object whose primitive value is "6"
  returns true; and comparing the number 1 to the boolean value
  \c{true} returns true. If you want to perform a comparison
  without such implicit value conversion, use strictlyEquals().

  Note that if this QScriptValue or the \a other value are objects,
  calling this function has side effects on the script engine, since
  the engine will call the object's valueOf() function (and possibly
  toString()) in an attempt to convert the object to a primitive value
  (possibly resulting in an uncaught script exception).

  \sa strictlyEquals(), lessThan()
*/
bool QScriptValue::equals(const QScriptValue &other) const
{
    Q_D(const QScriptValue);
    if (!d || !other.d_ptr)
        return (d == other.d_ptr);
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::equals: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    if (d->isJSC() && other.d_ptr->isJSC()) {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        if (!eng_p)
            eng_p = QScriptEnginePrivate::get(other.d_ptr->engine);
        if (eng_p) {
            JSC::ExecState *exec = eng_p->currentFrame;
            if (JSC::JSValue::equal(exec, d->jscValue, other.d_ptr->jscValue))
                return true;
        }
    }
    return QScript::Equals(*this, other);
}

/*!
  Returns true if this QScriptValue is equal to \a other using strict
  comparison (no conversion), otherwise returns false. The comparison
  follows the behavior described in \l{ECMA-262} section 11.9.6, "The
  Strict Equality Comparison Algorithm".

  If the type of this QScriptValue is different from the type of the
  \a other value, this function returns false. If the types are equal,
  the result depends on the type, as shown in the following table:

    \table
    \header \o Type \o Result
    \row    \o Undefined  \o true
    \row    \o Null       \o true
    \row    \o Boolean    \o true if both values are true, false otherwise
    \row    \o Number     \o false if either value is NaN (Not-a-Number); true if values are equal, false otherwise
    \row    \o String     \o true if both values are exactly the same sequence of characters, false otherwise
    \row    \o Object     \o true if both values refer to the same object, false otherwise
    \endtable
  
  \sa equals()
*/
bool QScriptValue::strictlyEquals(const QScriptValue &other) const
{
    Q_D(const QScriptValue);
    if (!d || !other.d_ptr)
        return (d == other.d_ptr);
    if (other.engine() && engine() && (other.engine() != engine())) {
        qWarning("QScriptValue::strictlyEquals: "
                 "cannot compare to a value created in "
                 "a different engine");
        return false;
    }
    if (d->type != other.d_ptr->type)
        return false;
    switch (d->type) {
    case QScriptValuePrivate::JSC:
        return JSC::JSValue::strictEqual(d->jscValue, other.d_ptr->jscValue);
    case QScriptValuePrivate::Number:
        return (d->numberValue == other.d_ptr->numberValue);
    case QScriptValuePrivate::String:
        return (*d->stringValue == *other.d_ptr->stringValue);
    }
    return false;
}

/*!
  Returns the string value of this QScriptValue, as defined in
  \l{ECMA-262} section 9.8, "ToString".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's toString() function (and possibly valueOf()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isString()
*/
QString QScriptValue::toString() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QString();
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        JSC::UString str = d->jscValue.toString(exec);
        return QString(reinterpret_cast<const QChar*>(str.data()), str.size());
    }
    case QScriptValuePrivate::Number:
        return QScript::qtStringFromJSCUString(JSC::UString::from(d->numberValue));
    case QScriptValuePrivate::String:
        return *d->stringValue;
    }
    return QString();
}

/*!
  Returns the number value of this QScriptValue, as defined in
  \l{ECMA-262} section 9.3, "ToNumber".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isNumber(), toInteger(), toInt32(), toUInt32(), toUInt16()
*/
qsreal QScriptValue::toNumber() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toNumber(exec);
    }
    case QScriptValuePrivate::Number:
        return d->numberValue;
    case QScriptValuePrivate::String:
        return QScript::qtStringToJSCUString(*d->stringValue).toDouble();
    }
    return 0;
}

/*!
  \obsolete

  Use toBool() instead.
*/
bool QScriptValue::toBoolean() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toBoolean(exec);
    }
    case QScriptValuePrivate::Number:
        return (d->numberValue != 0) && !qIsNaN(d->numberValue);
    case QScriptValuePrivate::String:
        return (d->stringValue->length() != 0);
    }
    return false;
}

/*!
  \since 4.5

  Returns the boolean value of this QScriptValue, using the conversion
  rules described in \l{ECMA-262} section 9.2, "ToBoolean".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa isBool()
*/
bool QScriptValue::toBool() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toBoolean(exec);
    }
    case QScriptValuePrivate::Number:
        return (d->numberValue != 0) && !qIsNaN(d->numberValue);
    case QScriptValuePrivate::String:
        return (d->stringValue->length() != 0);
    }
    return false;
}

/*!
  Returns the signed 32-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.5, "ToInt32".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber(), toUInt32()
*/
qint32 QScriptValue::toInt32() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toInt32(exec);
    }
    case QScriptValuePrivate::Number:
        return QScript::ToInt32(d->numberValue);
    case QScriptValuePrivate::String:
        return QScript::ToInt32(QScript::qtStringToJSCUString(*d->stringValue).toDouble());
    }
    return 0;
}

/*!
  Returns the unsigned 32-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.6, "ToUint32".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber(), toInt32()
*/
quint32 QScriptValue::toUInt32() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toUInt32(exec);
    }
    case QScriptValuePrivate::Number:
        return QScript::ToUint32(d->numberValue);
    case QScriptValuePrivate::String:
        return QScript::ToUint32(QScript::qtStringToJSCUString(*d->stringValue).toDouble());
    }
    return 0;
}

/*!
  Returns the unsigned 16-bit integer value of this QScriptValue, using
  the conversion rules described in \l{ECMA-262} section 9.7, "ToUint16".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber()
*/
quint16 QScriptValue::toUInt16() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        // no equivalent function in JSC
        return QScript::ToUint16(toNumber());
    }
    case QScriptValuePrivate::Number:
        return QScript::ToUint16(d->numberValue);
    case QScriptValuePrivate::String:
        return QScript::ToUint16(QScript::qtStringToJSCUString(*d->stringValue).toDouble());
    }
    return 0;
}

/*!
  Returns the integer value of this QScriptValue, using the conversion
  rules described in \l{ECMA-262} section 9.4, "ToInteger".

  Note that if this QScriptValue is an object, calling this function
  has side effects on the script engine, since the engine will call
  the object's valueOf() function (and possibly toString()) in an
  attempt to convert the object to a primitive value (possibly
  resulting in an uncaught script exception).

  \sa toNumber()
*/
qsreal QScriptValue::toInteger() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p ? eng_p->currentFrame : 0;
        return d->jscValue.toInteger(exec);
    }
    case QScriptValuePrivate::Number:
        return QScript::ToInteger(d->numberValue);
    case QScriptValuePrivate::String:
        return QScript::ToInteger(QScript::qtStringToJSCUString(*d->stringValue).toDouble());
    }
    return 0;
}

/*!
  Returns the QVariant value of this QScriptValue, if it can be
  converted to a QVariant; otherwise returns an invalid QVariant.
  The conversion is performed according to the following table:

    \table
    \header \o Input Type \o Result
    \row    \o Undefined  \o An invalid QVariant.
    \row    \o Null       \o An invalid QVariant.
    \row    \o Boolean    \o A QVariant containing the value of the boolean.
    \row    \o Number     \o A QVariant containing the value of the number.
    \row    \o String     \o A QVariant containing the value of the string.
    \row    \o QVariant Object \o The result is the QVariant value of the object (no conversion).
    \row    \o QObject Object \o A QVariant containing a pointer to the QObject.
    \row    \o Date Object \o A QVariant containing the date value (toDateTime()).
    \row    \o RegExp Object \o A QVariant containing the regular expression value (toRegExp()).
    \row    \o Array Object \o The array is converted to a QVariantList.
    \row    \o Object     \o If the value is primitive, then the result is converted to a QVariant according to the above rules; otherwise, an invalid QVariant is returned.
    \endtable

  \sa isVariant()
*/
QVariant QScriptValue::toVariant() const
{
    Q_D(const QScriptValue);
    if (!d)
        return QVariant();
    switch (d->type) {
    case QScriptValuePrivate::JSC:
        if (isObject()) {
            if (isVariant())
                return d->variantValue();
#ifndef QT_NO_QOBJECT
            else if (isQObject())
                return qVariantFromValue(toQObject());
#endif
            else if (isDate())
                return QVariant(toDateTime());
#ifndef QT_NO_REGEXP
            else if (isRegExp())
                return QVariant(toRegExp());
#endif
            else if (isArray())
                return QScriptEnginePrivate::variantListFromArray(*this);
            // try to convert to primitive
            QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(engine());
            JSC::ExecState *exec = eng_p->currentFrame;
            JSC::JSValue prim = d->jscValue.toPrimitive(exec);
            if (!prim.isObject())
                return eng_p->scriptValueFromJSCValue(prim).toVariant();
        } else if (isNumber()) {
            return QVariant(toNumber());
        } else if (isString()) {
            return QVariant(toString());
        } else if (isBool()) {
            return QVariant(toBool());
        }
        return QVariant();
    case QScriptValuePrivate::Number:
        return QVariant(d->numberValue);
    case QScriptValuePrivate::String:
        return QVariant(*d->stringValue);
    }
    return QVariant();
}

/*!
  \obsolete

  This function is obsolete; use QScriptEngine::toObject() instead.
*/
QScriptValue QScriptValue::toObject() const
{
    Q_D(const QScriptValue);
    if (!d || !d->engine)
        return QScriptValue();
    switch (d->type) {
    case QScriptValuePrivate::JSC: {
        if (JSC::JSImmediate::isUndefinedOrNull(d->jscValue))
            return QScriptValue();
        QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
        JSC::ExecState *exec = eng_p->currentFrame;
        return eng_p->scriptValueFromJSCValue(d->jscValue.toObject(exec));
    }
    case QScriptValuePrivate::Number:
    case QScriptValuePrivate::String:
        Q_ASSERT_X(false, Q_FUNC_INFO, "you should not get here");
        break;
    }
    return QScriptValue();
}

/*!
  Returns a QDateTime representation of this value, in local time.
  If this QScriptValue is not a date, or the value of the date is NaN
  (Not-a-Number), an invalid QDateTime is returned.

  \sa isDate()
*/
QDateTime QScriptValue::toDateTime() const
{
    Q_D(const QScriptValue);
    if (!isDate())
        return QDateTime();
    qsreal t = static_cast<JSC::DateInstance*>(JSC::asObject(d->jscValue))->internalNumber();
    return QScript::ToDateTime(t, Qt::LocalTime);
}

#ifndef QT_NO_REGEXP
/*!
  Returns the QRegExp representation of this value.
  If this QScriptValue is not a regular expression, an empty
  QRegExp is returned.

  \sa isRegExp()
*/
QRegExp QScriptValue::toRegExp() const
{
    Q_D(const QScriptValue);
    if (!isRegExp())
        return QRegExp();
    QString pattern = d->property(QLatin1String("source"), QScriptValue::ResolvePrototype).toString();
    Qt::CaseSensitivity kase = Qt::CaseSensitive;
    if (d->property(QLatin1String("ignoreCase"), QScriptValue::ResolvePrototype).toBool())
        kase = Qt::CaseInsensitive;
    return QRegExp(pattern, kase, QRegExp::RegExp2);
}
#endif // QT_NO_REGEXP

/*!
  If this QScriptValue is a QObject, returns the QObject pointer
  that the QScriptValue represents; otherwise, returns 0.

  If the QObject that this QScriptValue wraps has been deleted,
  this function returns 0 (i.e. it is possible for toQObject()
  to return 0 even when isQObject() returns true).

  \sa isQObject()
*/
QObject *QScriptValue::toQObject() const
{
    Q_D(const QScriptValue);
    if (isQObject()) {
        return static_cast<QScript::QObjectWrapperObject*>(JSC::asObject(d->jscValue))->value();
    } else if (isVariant()) {
        QVariant var = toVariant();
        int type = var.userType();
        if ((type == QMetaType::QObjectStar) || (type == QMetaType::QWidgetStar))
            return *reinterpret_cast<QObject* const *>(var.constData());
    }
    return 0;
}

/*!
  If this QScriptValue is a QMetaObject, returns the QMetaObject pointer
  that the QScriptValue represents; otherwise, returns 0.

  \sa isQMetaObject()
*/
const QMetaObject *QScriptValue::toQMetaObject() const
{
    Q_D(const QScriptValue);
    if (isQMetaObject())
        return static_cast<QScript::QMetaObjectWrapperObject*>(JSC::asObject(d->jscValue))->value();
    return 0;
}

/*!
  Sets the value of this QScriptValue's property with the given \a name to
  the given \a value.

  If this QScriptValue is not an object, this function does nothing.

  If this QScriptValue does not already have a property with name \a name,
  a new property is created; the given \a flags then specify how this
  property may be accessed by script code.

  If \a value is invalid, the property is removed.

  If the property is implemented using a setter function (i.e. has the
  PropertySetter flag set), calling setProperty() has side-effects on
  the script engine, since the setter function will be called with the
  given \a value as argument (possibly resulting in an uncaught script
  exception).

  Note that you cannot specify custom getter or setter functions for
  built-in properties, such as the \c{length} property of Array objects
  or meta properties of QObject objects.

  \sa property()
*/
void QScriptValue::setProperty(const QString &name, const QScriptValue &value,
                               const PropertyFlags &flags)
{
    Q_D(QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    if (value.engine() && (value.engine() != engine())) {
        qWarning("QScriptValue::setProperty(%s) failed: "
                 "cannot set value created in a different engine",
                 qPrintable(name));
        return;
    }
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::JSValue jscValue = eng_p->scriptValueToJSCValue(value);
    JSC::UString jscName = QScript::qtStringToJSCUString(name);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::Identifier id = JSC::Identifier(exec, jscName);
    if (!jscValue) {
        JSC::asObject(d->jscValue)->deleteProperty(exec, id);
    } else {
        if ((flags & QScriptValue::PropertyGetter) || (flags & QScriptValue::PropertySetter)) {
            if (jscValue.isObject()) {
                if (flags & QScriptValue::PropertyGetter)
                    JSC::asObject(d->jscValue)->defineGetter(exec, id, JSC::asObject(jscValue));
                if (flags & QScriptValue::PropertySetter)
                    JSC::asObject(d->jscValue)->defineSetter(exec, id, JSC::asObject(jscValue));
            }
        } else {
            if (flags != QScriptValue::KeepExistingFlags) {
                if (JSC::asObject(d->jscValue)->hasOwnProperty(exec, id))
                    JSC::asObject(d->jscValue)->deleteProperty(exec, id);
                unsigned attribs = 0;
                if (flags & QScriptValue::ReadOnly)
                    attribs |= JSC::ReadOnly;
                if (flags & QScriptValue::SkipInEnumeration)
                    attribs |= JSC::DontEnum;
                if (flags & QScriptValue::Undeletable)
                    attribs |= JSC::DontDelete;
                attribs |= flags & QScriptValue::UserRange;
                JSC::asObject(d->jscValue)->putWithAttributes(exec, id, jscValue, attribs);
            } else {
                JSC::PutPropertySlot slot;
                JSC::asObject(d->jscValue)->put(exec, id, jscValue, slot);
            }
        }
    }
}

/*!
  Returns the value of this QScriptValue's property with the given \a name,
  using the given \a mode to resolve the property.

  If no such property exists, an invalid QScriptValue is returned.

  If the property is implemented using a getter function (i.e. has the
  PropertyGetter flag set), calling property() has side-effects on the
  script engine, since the getter function will be called (possibly
  resulting in an uncaught script exception). If an exception
  occurred, property() returns the value that was thrown (typically
  an \c{Error} object).

  \sa setProperty(), propertyFlags(), QScriptValueIterator
*/
QScriptValue QScriptValue::property(const QString &name,
                                    const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return QScriptValue();
    return d->property(name, mode);
}

/*!
  \overload

  Returns the property at the given \a arrayIndex, using the given \a
  mode to resolve the property.

  This function is provided for convenience and performance when
  working with array objects.

  If this QScriptValue is not an Array object, this function behaves
  as if property() was called with the string representation of \a
  arrayIndex.
*/
QScriptValue QScriptValue::property(quint32 arrayIndex,
                                    const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return QScriptValue();
    return d->property(arrayIndex, mode);
}

/*!
  \overload

  Sets the property at the given \a arrayIndex to the given \a value.

  This function is provided for convenience and performance when
  working with array objects.

  If this QScriptValue is not an Array object, this function behaves
  as if setProperty() was called with the string representation of \a
  arrayIndex.
*/
void QScriptValue::setProperty(quint32 arrayIndex, const QScriptValue &value,
                               const PropertyFlags &flags)
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    if (value.engine() && (value.engine() != engine())) {
        qWarning("QScriptValue::setProperty() failed: "
                 "cannot set value created in a different engine");
        return;
    }
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::JSValue jscValue = eng_p->scriptValueToJSCValue(value);
    if (!jscValue) {
        JSC::asObject(d->jscValue)->deleteProperty(exec, arrayIndex);
    } else {
        if ((flags & QScriptValue::PropertyGetter) || (flags & QScriptValue::PropertySetter)) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "property getters and setters not implemented");
        } else {
            if (flags != QScriptValue::KeepExistingFlags) {
//                if (JSC::asObject(d->jscValue)->hasOwnProperty(exec, arrayIndex))
//                    JSC::asObject(d->jscValue)->deleteProperty(exec, arrayIndex);
                unsigned attribs = 0;
                if (flags & QScriptValue::ReadOnly)
                    attribs |= JSC::ReadOnly;
                if (flags & QScriptValue::SkipInEnumeration)
                    attribs |= JSC::DontEnum;
                if (flags & QScriptValue::Undeletable)
                    attribs |= JSC::DontDelete;
                attribs |= flags & QScriptValue::UserRange;
                JSC::asObject(d->jscValue)->putWithAttributes(exec, arrayIndex, jscValue, attribs);
            } else {
                JSC::asObject(d->jscValue)->put(exec, arrayIndex, jscValue);
            }
        }
    }
}

/*! 
  \since 4.4

  Returns the value of this QScriptValue's property with the given \a name,
  using the given \a mode to resolve the property.

  This overload of property() is useful when you need to look up the
  same property repeatedly, since the lookup can be performed faster
  when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle(), setProperty()
*/
QScriptValue QScriptValue::property(const QScriptString &name,
                                    const ResolveFlags &mode) const
{
    // ### simplified for now
    return property(name.toString(), mode);
}

/*!
  \since 4.4

  Sets the value of this QScriptValue's property with the given \a
  name to the given \a value. The given \a flags specify how this
  property may be accessed by script code.

  This overload of setProperty() is useful when you need to set the
  same property repeatedly, since the operation can be performed
  faster when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle()
*/
void QScriptValue::setProperty(const QScriptString &name,
                               const QScriptValue &value,
                               const PropertyFlags &flags)
{
    // ### simplified for now
    setProperty(name.toString(), value, flags);
}

/*!
  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QString &name,
                                                        const ResolveFlags &mode) const
{
    Q_D(const QScriptValue);
    if (!isObject())
        return 0;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::UString jscName = QScript::qtStringToJSCUString(name);
    JSC::Identifier id = JSC::Identifier(exec, jscName);
    unsigned attribs = 0;
    if (!JSC::asObject(d->jscValue)->getPropertyAttributes(exec, id, attribs))
        return 0;
    QScriptValue::PropertyFlags result = 0;
    if (attribs & JSC::ReadOnly)
        result |= QScriptValue::ReadOnly;
    if (attribs & JSC::DontEnum)
        result |= QScriptValue::SkipInEnumeration;
    if (attribs & JSC::DontDelete)
        result |= QScriptValue::Undeletable;
    if (attribs & JSC::Getter)
        result |= QScriptValue::PropertyGetter;
    if (attribs & JSC::Setter)
        result |= QScriptValue::PropertySetter;
    if (attribs & QScript::QObjectMemberAttribute)
        result |= QScriptValue::QObjectMember;

    result |= QScriptValue::PropertyFlag(attribs & QScriptValue::UserRange);
    return result;
}

/*!
  \since 4.4

  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QScriptString &name,
                                                        const ResolveFlags &mode) const
{
    // ### simplified for now
    return propertyFlags(name.toString(), mode);
}

/*!
  Calls this QScriptValue as a function, using \a thisObject as
  the `this' object in the function call, and passing \a args
  as arguments to the function. Returns the value returned from
  the function.

  If this QScriptValue is not a function, call() does nothing
  and returns an invalid QScriptValue.

  Note that if \a thisObject is not an object, the global object
  (see \l{QScriptEngine::globalObject()}) will be used as the
  `this' object.

  Calling call() can cause an exception to occur in the script engine;
  in that case, call() returns the value that was thrown (typically an
  \c{Error} object). You can call
  QScriptEngine::hasUncaughtException() to determine if an exception
  occurred.

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 2

  \sa construct()
*/
QScriptValue QScriptValue::call(const QScriptValue &thisObject,
                                const QScriptValueList &args)
{
    Q_D(const QScriptValue);
    if (!isFunction())
        return QScriptValue();

    if (thisObject.engine() && (thisObject.engine() != engine())) {
        qWarning("QScriptValue::call() failed: "
                 "cannot call function with thisObject created in "
                 "a different engine");
        return QScriptValue();
    }

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;

    JSC::JSValue jscThisObject = eng_p->scriptValueToJSCValue(thisObject);
    if (!jscThisObject || !jscThisObject.isObject())
        jscThisObject = eng_p->globalObject;

    QVector<JSC::JSValue> argsVector;
    argsVector.resize(args.size());
    for (int i = 0; i < args.size(); ++i) {
        const QScriptValue &arg = args.at(i);
        if (!arg.isValid()) {
            argsVector[i] = JSC::jsUndefined();
        } else if (arg.engine() && (arg.engine() != engine())) {
            qWarning("QScriptValue::call() failed: "
                     "cannot call function with argument created in "
                     "a different engine");
            return QScriptValue();
        } else {
            argsVector[i] = eng_p->scriptValueToJSCValue(arg);
        }
    }
    JSC::ArgList jscArgs(argsVector.data(), argsVector.size());

    JSC::JSValue callee = d->jscValue;
    JSC::CallData callData;
    JSC::CallType callType = callee.getCallData(callData);
    JSC::JSValue result = JSC::call(exec, callee, callType, callData, jscThisObject, jscArgs);
    if (exec->hadException())
        eng_p->uncaughtException = exec->exception();
    return eng_p->scriptValueFromJSCValue(result);
}

/*!
  Calls this QScriptValue as a function, using \a thisObject as
  the `this' object in the function call, and passing \a arguments
  as arguments to the function. Returns the value returned from
  the function.

  If this QScriptValue is not a function, call() does nothing
  and returns an invalid QScriptValue.

  \a arguments can be an arguments object, an array, null or
  undefined; any other type will cause a TypeError to be thrown.

  Note that if \a thisObject is not an object, the global object
  (see \l{QScriptEngine::globalObject()}) will be used as the
  `this' object.

  One common usage of this function is to forward native function
  calls to another function:

  \snippet doc/src/snippets/code/src_script_qscriptvalue.cpp 3

  \sa construct(), QScriptContext::argumentsObject()
*/
QScriptValue QScriptValue::call(const QScriptValue &thisObject,
                                const QScriptValue &arguments)
{
    Q_D(QScriptValue);
    if (!isFunction())
        return QScriptValue();

    if (thisObject.engine() && (thisObject.engine() != engine())) {
        qWarning("QScriptValue::call() failed: "
                 "cannot call function with thisObject created in "
                 "a different engine");
        return QScriptValue();
    }

    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;

    JSC::JSValue jscThisObject = eng_p->scriptValueToJSCValue(thisObject);
    if (!jscThisObject || !jscThisObject.isObject())
        jscThisObject = eng_p->globalObject;

    JSC::JSValue array = eng_p->scriptValueToJSCValue(arguments);
    // copied from runtime/FunctionPrototype.cpp, functionProtoFuncApply()
    JSC::MarkedArgumentBuffer applyArgs;
    if (!array.isUndefinedOrNull()) {
        if (!array.isObject()) {
            return eng_p->scriptValueFromJSCValue(JSC::throwError(exec, JSC::TypeError));
        }
        if (JSC::asObject(array)->classInfo() == &JSC::Arguments::info)
            JSC::asArguments(array)->fillArgList(exec, applyArgs);
        else if (JSC::isJSArray(&exec->globalData(), array))
            JSC::asArray(array)->fillArgList(exec, applyArgs);
        else if (JSC::asObject(array)->inherits(&JSC::JSArray::info)) {
            unsigned length = JSC::asArray(array)->get(exec, exec->propertyNames().length).toUInt32(exec);
            for (unsigned i = 0; i < length; ++i)
                applyArgs.append(JSC::asArray(array)->get(exec, i));
        } else {
            Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
//            return JSC::throwError(exec, JSC::TypeError);
        }
    }

    JSC::JSValue callee = d->jscValue;
    JSC::JSValue result;
    JSC::CallData callData;
    JSC::CallType callType = callee.getCallData(callData);
    if (callType == JSC::CallTypeJS) {
        result = JSC::asFunction(callee)->call(exec, jscThisObject, applyArgs);
    } else if (callType == JSC::CallTypeHost) {
        // ### avoid copy+paste of other call() overload
        JSC::ScopeChainNode* scopeChain = exec->scopeChain();
        JSC::Interpreter *interp = exec->interpreter();
        JSC::Register *oldEnd = interp->registerFile().end();
        int argc = 1 + applyArgs.size(); // implicit "this" parameter
        if (!interp->registerFile().grow(oldEnd + argc + JSC::RegisterFile::CallFrameHeaderSize)) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "stack overflow");
        }
        JSC::CallFrame* newCallFrame = JSC::CallFrame::create(oldEnd);
        size_t dst = 0;
        newCallFrame[0] = jscThisObject;
        JSC::MarkedArgumentBuffer::const_iterator it;
        for (it = applyArgs.begin(); it != applyArgs.end(); ++it)
            newCallFrame[++dst] = *it;
        newCallFrame += argc + JSC::RegisterFile::CallFrameHeaderSize;
        // ### dst?
        newCallFrame->init(0, /*vPC=*/0, scopeChain, exec, dst, argc, JSC::asObject(callee));
        result = callData.native.function(newCallFrame, JSC::asObject(callee), jscThisObject, applyArgs);
        interp->registerFile().shrink(oldEnd);
    }
    if (exec->hadException()) {
        eng_p->uncaughtException = exec->exception();
        result = exec->exception();
    }
    return eng_p->scriptValueFromJSCValue(result);
}

/*!
  Creates a new \c{Object} and calls this QScriptValue as a
  constructor, using the created object as the `this' object and
  passing \a args as arguments. If the return value from the
  constructor call is an object, then that object is returned;
  otherwise the default constructed object is returned.

  If this QScriptValue is not a function, construct() does nothing
  and returns an invalid QScriptValue.

  Calling construct() can cause an exception to occur in the script
  engine; in that case, construct() returns the value that was thrown
  (typically an \c{Error} object). You can call
  QScriptEngine::hasUncaughtException() to determine if an exception
  occurred.

  \sa call(), QScriptEngine::newObject()
*/
QScriptValue QScriptValue::construct(const QScriptValueList &args)
{
    Q_D(const QScriptValue);
    if (!isFunction())
        return QScriptValue();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;

    QVector<JSC::JSValue> argsVector;
    argsVector.resize(args.size());
    for (int i = 0; i < args.size(); ++i) {
        if (!args.at(i).isValid())
            argsVector[i] = JSC::jsUndefined();
        else
            argsVector[i] = eng_p->scriptValueToJSCValue(args.at(i));
    }
    JSC::ArgList jscArgs(argsVector.data(), argsVector.size());

    JSC::JSValue callee = d->jscValue;
    JSC::ConstructData constructData;
    JSC::ConstructType constructType = callee.getConstructData(constructData);
    JSC::JSObject *result = JSC::construct(exec, callee, constructType, constructData, jscArgs);
    if (exec->hadException())
        eng_p->uncaughtException = exec->exception();
    return eng_p->scriptValueFromJSCValue(result);
}

/*!
  Creates a new \c{Object} and calls this QScriptValue as a
  constructor, using the created object as the `this' object and
  passing \a arguments as arguments. If the return value from the
  constructor call is an object, then that object is returned;
  otherwise the default constructed object is returned.

  If this QScriptValue is not a function, construct() does nothing
  and returns an invalid QScriptValue.

  \a arguments can be an arguments object, an array, null or
  undefined. Any other type will cause a TypeError to be thrown.

  \sa call(), QScriptEngine::newObject(), QScriptContext::argumentsObject()
*/
QScriptValue QScriptValue::construct(const QScriptValue &arguments)
{
    Q_D(QScriptValue);
    if (!isFunction())
        return QScriptValue();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::ExecState *exec = eng_p->currentFrame;

    JSC::JSValue array = eng_p->scriptValueToJSCValue(arguments);
    // copied from runtime/FunctionPrototype.cpp, functionProtoFuncApply()
    JSC::MarkedArgumentBuffer applyArgs;
    if (!array.isUndefinedOrNull()) {
        if (!array.isObject()) {
            return eng_p->scriptValueFromJSCValue(JSC::throwError(exec, JSC::TypeError, "Arguments must be an array"));
        }
        if (JSC::asObject(array)->classInfo() == &JSC::Arguments::info)
            JSC::asArguments(array)->fillArgList(exec, applyArgs);
        else if (JSC::isJSArray(&exec->globalData(), array))
            JSC::asArray(array)->fillArgList(exec, applyArgs);
        else if (JSC::asObject(array)->inherits(&JSC::JSArray::info)) {
            unsigned length = JSC::asArray(array)->get(exec, exec->propertyNames().length).toUInt32(exec);
            for (unsigned i = 0; i < length; ++i)
                applyArgs.append(JSC::asArray(array)->get(exec, i));
        } else {
            return eng_p->scriptValueFromJSCValue(JSC::throwError(exec, JSC::TypeError, "Arguments must be an array"));
        }
    }

    JSC::JSValue callee = d->jscValue;
    JSC::JSValue result;
    JSC::ConstructData constructData;
    JSC::ConstructType constructType = callee.getConstructData(constructData);
    if (constructType == JSC::ConstructTypeJS) {
        result = JSC::asFunction(callee)->construct(exec, applyArgs);
    } else if (constructType == JSC::ConstructTypeHost) {
        JSC::Structure* structure;
        JSC::JSValue prototype = callee.get(exec, exec->propertyNames().prototype);
        if (prototype.isObject())
            structure = asObject(prototype)->inheritorID();
        else
            structure = exec->lexicalGlobalObject()->emptyObjectStructure();
        JSC::JSObject* thisObj = new (exec) JSC::JSObject(structure);
        // ### avoid copy+paste of call()
        JSC::ScopeChainNode* scopeChain = exec->scopeChain();
        JSC::Interpreter *interp = exec->interpreter();
        JSC::Register *oldEnd = interp->registerFile().end();
        int argc = 1 + applyArgs.size(); // implicit "this" parameter
        if (!interp->registerFile().grow(oldEnd + argc + JSC::RegisterFile::CallFrameHeaderSize)) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "stack overflow");
        }
        JSC::CallFrame* newCallFrame = JSC::CallFrame::create(oldEnd);
        size_t dst = 0;
        newCallFrame[0] = JSC::JSValue(thisObj);
        JSC::MarkedArgumentBuffer::const_iterator it;
        for (it = applyArgs.begin(); it != applyArgs.end(); ++it)
            newCallFrame[++dst] = *it;
        newCallFrame += argc + JSC::RegisterFile::CallFrameHeaderSize;
        // ### dst?
        newCallFrame->init(0, /*vPC=*/0, scopeChain, exec, dst, argc, JSC::asObject(callee));
        result = constructData.native.function(newCallFrame, JSC::asObject(callee), applyArgs);
        interp->registerFile().shrink(oldEnd);
    }
    if (exec->hadException()) {
        eng_p->uncaughtException = exec->exception();
        result = exec->exception();
    }
    return eng_p->scriptValueFromJSCValue(result);
}

/*!
  Returns the QScriptEngine that created this QScriptValue,
  or 0 if this QScriptValue is invalid or the value is not
  associated with a particular engine.
*/
QScriptEngine *QScriptValue::engine() const
{
    Q_D(const QScriptValue);
    if (!d)
        return 0;
    return d->engine;
}

/*!
  \obsolete

  Use isBool() instead.
*/
bool QScriptValue::isBoolean() const
{
    Q_D(const QScriptValue);
    return d && d->isJSC() && d->jscValue.isBoolean();
}

/*!
  \since 4.5

  Returns true if this QScriptValue is of the primitive type Boolean;
  otherwise returns false.

  \sa toBool()
*/
bool QScriptValue::isBool() const
{
    Q_D(const QScriptValue);
    return d && d->isJSC() && d->jscValue.isBoolean();
}

/*!
  Returns true if this QScriptValue is of the primitive type Number;
  otherwise returns false.

  \sa toNumber()
*/
bool QScriptValue::isNumber() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    switch (d->type) {
    case QScriptValuePrivate::JSC:
        return d->jscValue.isNumber();
    case QScriptValuePrivate::Number:
        return true;
    case QScriptValuePrivate::String:
        return false;
    }
    return false;
}

/*!
  Returns true if this QScriptValue is of the primitive type String;
  otherwise returns false.

  \sa toString()
*/
bool QScriptValue::isString() const
{
    Q_D(const QScriptValue);
    if (!d)
        return false;
    switch (d->type) {
    case QScriptValuePrivate::JSC:
        return d->jscValue.isString();
    case QScriptValuePrivate::Number:
        return false;
    case QScriptValuePrivate::String:
        return true;
    }
    return false;
}

/*!
  Returns true if this QScriptValue is a function; otherwise returns
  false.

  \sa call()
*/
bool QScriptValue::isFunction() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC())
        return false;
    return QScript::isFunction(d->jscValue);
}

/*!
  Returns true if this QScriptValue is of the primitive type Null;
  otherwise returns false.

  \sa QScriptEngine::nullValue()
*/
bool QScriptValue::isNull() const
{
    Q_D(const QScriptValue);
    return d && d->isJSC() && d->jscValue.isNull();
}

/*!
  Returns true if this QScriptValue is of the primitive type Undefined;
  otherwise returns false.

  \sa QScriptEngine::undefinedValue()
*/
bool QScriptValue::isUndefined() const
{
    Q_D(const QScriptValue);
    return d && d->isJSC() && d->jscValue.isUndefined();
}

/*!
  Returns true if this QScriptValue is of the Object type; otherwise
  returns false.

  Note that function values, variant values, and QObject values are
  objects, so this function returns true for such values.

  \sa toObject(), QScriptEngine::newObject()
*/
bool QScriptValue::isObject() const
{
    Q_D(const QScriptValue);
    return d && d->isJSC() && d->jscValue.isObject();
}

/*!
  Returns true if this QScriptValue is a variant value;
  otherwise returns false.

  \sa toVariant(), QScriptEngine::newVariant()
*/
bool QScriptValue::isVariant() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return JSC::asObject(d->jscValue)->isObject(&QScript::QVariantWrapperObject::info);
}

/*!
  Returns true if this QScriptValue is a QObject; otherwise returns
  false.

  Note: This function returns true even if the QObject that this
  QScriptValue wraps has been deleted.

  \sa toQObject(), QScriptEngine::newQObject()
*/
bool QScriptValue::isQObject() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return JSC::asObject(d->jscValue)->isObject(&QScript::QObjectWrapperObject::info);
}

/*!
  Returns true if this QScriptValue is a QMetaObject; otherwise returns
  false.

  \sa toQMetaObject(), QScriptEngine::newQMetaObject()
*/
bool QScriptValue::isQMetaObject() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return false;
    return JSC::asObject(d->jscValue)->isObject(&QScript::QMetaObjectWrapperObject::info);
    return false;
}

/*!
  Returns true if this QScriptValue is valid; otherwise returns
  false.
*/
bool QScriptValue::isValid() const
{
    Q_D(const QScriptValue);
    if (d)
        return d->isValid();
    else
        return false;
}

/*!
  \since 4.4

  Returns the internal data of this QScriptValue object. QtScript uses
  this property to store the primitive value of Date, String, Number
  and Boolean objects. For other types of object, custom data may be
  stored using setData().
*/
QScriptValue QScriptValue::data() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return QScriptValue();
    // ### make hidden property
    return d->property(QLatin1String("__qt_data__"), QScriptValue::ResolveLocal);
}

/*!
  \since 4.4

  Sets the internal \a data of this QScriptValue object. You can use
  this function to set object-specific data that won't be directly
  accessible to scripts, but may be retrieved in C++ using the data()
  function.
*/
void QScriptValue::setData(const QScriptValue &data)
{
    Q_D(QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(d->engine);
    JSC::JSValue other = eng_p->scriptValueToJSCValue(data);
    JSC::ExecState *exec = eng_p->currentFrame;
    JSC::Identifier id = JSC::Identifier(exec, "__qt_data__");
    if (!data.isValid()) {
        JSC::asObject(d->jscValue)->removeDirect(id);
    } else {
        // ### make hidden property
        JSC::asObject(d->jscValue)->putDirect(id, other);
    }
}

/*!
  \since 4.4

  Returns the custom script class that this script object is an
  instance of, or 0 if the object is not of a custom class.

  \sa setScriptClass()
*/
QScriptClass *QScriptValue::scriptClass() const
{
    Q_D(const QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return 0;
    if (!d->jscValue.isObject(&QScript::ClassObject::info))
        return 0;
    QScript::ClassObject *instance = static_cast<QScript::ClassObject*>(JSC::asObject(d->jscValue));
    return instance->scriptClass();
}

/*!
  \since 4.4

  Sets the custom script class of this script object to \a scriptClass.
  This can be used to "promote" a plain script object (e.g. created
  by the "new" operator in a script, or by QScriptEngine::newObject() in C++)
  to an object of a custom type.

  If \a scriptClass is 0, the object will be demoted to a plain
  script object.

  \sa scriptClass(), setData()
*/
void QScriptValue::setScriptClass(QScriptClass *scriptClass)
{
    Q_D(QScriptValue);
    if (!d || !d->isJSC() || !d->jscValue.isObject())
        return;
    if (!d->jscValue.isObject(&QScript::ClassObject::info)) {
        qWarning("QScriptValue::setScriptClass() not implemented");
        return;
    }
    QScript::ClassObject *instance = static_cast<QScript::ClassObject*>(JSC::asObject(d->jscValue));
    instance->setScriptClass(scriptClass);
}

/*!
  \internal

  Returns the ID of this object, or -1 if this QScriptValue is not an
  object.

  \sa QScriptEngine::objectById()
*/
qint64 QScriptValue::objectId() const
{
    return d_ptr?d_ptr->id:-1;
}
QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
