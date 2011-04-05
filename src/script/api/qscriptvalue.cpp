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
#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptstring.h"
#include "qscriptvalue.h"
#include "qscriptvalue_p.h"
#include "qscriptclass_p.h"
#include "qscriptdeclarativeclassobject_p.h"
#include "qscript_impl_p.h"
#include "qscriptshareddata_p.h"
#include <QtCore/qregexp.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

/*!
    Constructs an invalid value.
*/
QScriptValue::QScriptValue()
    : d_ptr(InvalidValue())
{
}

/*!
  Constructs a new QScriptValue with a boolean \a value.
*/
QScriptValue::QScriptValue(bool value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
    \enum QScriptValue::PropertyFlag

    This enum describes the attributes of a property.

    \value ReadOnly The property is read-only. Attempts by Qt Script code to write to the property will be ignored.

    \value Undeletable Attempts by Qt Script code to \c{delete} the property will be ignored.

    \value SkipInEnumeration The property is not to be enumerated by a \c{for-in} enumeration.

    \value PropertyGetter The property is defined by a function which will be called to get the property value.

    \value PropertySetter The property is defined by a function which will be called to set the property value.

    \omitvalue QObjectMember This flag is used to indicate that an existing property is a QObject member (a property or method).

    \value KeepExistingFlags This value is used to indicate to setProperty() that the property's flags should be left unchanged. If the property doesn't exist, the default flags (0) will be used.

    \omitvalue UserRange Flags in this range are not used by Qt Script, and can be used for custom purposes.
*/

/*!
  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(int value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(uint value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a number \a value.
*/
QScriptValue::QScriptValue(qsreal value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QString& value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a special \a value.
*/
QScriptValue::QScriptValue(SpecialValue value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const QLatin1String &value)
    : d_ptr(new QScriptValuePrivate(value))
{
}

/*!
  Constructs a new QScriptValue with a string \a value.
*/
QScriptValue::QScriptValue(const char* value)
    : d_ptr(new QScriptValuePrivate(QString::fromUtf8(value)))
{
}

/*!
    Block automatic convertion to bool
    \internal
*/
QScriptValue::QScriptValue(void* d)
{
    Q_UNUSED(d);
    Q_ASSERT(false);
}

/*!
    Constructs a new QScriptValue from private
    \internal
*/
QScriptValue::QScriptValue(QScriptValuePrivate* d)
    : d_ptr(d)
{
}

/*!
    Constructs a new QScriptValue from private
    \internal
*/
QScriptValue::QScriptValue(QScriptPassPointer<QScriptValuePrivate> d)
    : d_ptr(d.give())
{
}

/*!
  \obsolete

  Constructs a new QScriptValue with the boolean \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, bool value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the integer \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, int value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the unsigned integer \a value and
  registers it with the script \a engine.
 */
QScriptValue::QScriptValue(QScriptEngine* engine, uint value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the qsreal \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, qsreal value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, const QString& value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the string \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, const char* value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), QString::fromUtf8(value));
    } else {
        d_ptr = new QScriptValuePrivate(QString::fromUtf8(value));
    }
}

/*!
  \obsolete

  Constructs a new QScriptValue with the special \a value and
  registers it with the script \a engine.
*/
QScriptValue::QScriptValue(QScriptEngine* engine, SpecialValue value)
{
    if (engine) {
        QScriptIsolate api(QScriptEnginePrivate::get(engine), QScriptIsolate::NotNullEngine);
        d_ptr = new QScriptValuePrivate(QScriptEnginePrivate::get(engine), value);
    } else {
        d_ptr = new QScriptValuePrivate(value);
    }
}

/*!
  Constructs a new QScriptValue that is a copy of \a other.

  Note that if \a other is an object (i.e., isObject() would return
  true), then only a reference to the underlying object is copied into
  the new script value (i.e., the object itself is not copied).
*/
QScriptValue::QScriptValue(const QScriptValue& other)
    : d_ptr(other.d_ptr)
{
}

/*!
    Destroys this QScriptValue.
*/
QScriptValue::~QScriptValue()
{
}

/*!
  Returns true if this QScriptValue is valid; otherwise returns
  false.
*/
bool QScriptValue::isValid() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isValid();
}

/*!
  Returns true if this QScriptValue is of the primitive type Boolean;
  otherwise returns false.

  \sa toBool()
*/
bool QScriptValue::isBool() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isBool();
}

/*!
  \obsolete

  Use isBool() instead.
  Returns true if this QScriptValue is of the primitive type Boolean;
  otherwise returns false.
*/
bool QScriptValue::isBoolean() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isBool();
}

/*!
  Returns true if this QScriptValue is of the primitive type Number;
  otherwise returns false.

  \sa toNumber()
*/
bool QScriptValue::isNumber() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isNumber();
}

/*!
  Returns true if this QScriptValue is of the primitive type Null;
  otherwise returns false.

  \sa QScriptEngine::nullValue()
*/
bool QScriptValue::isNull() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isNull();
}

/*!
  Returns true if this QScriptValue is of the primitive type String;
  otherwise returns false.

  \sa toString()
*/
bool QScriptValue::isString() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isString();
}

/*!
  Returns true if this QScriptValue is of the primitive type Undefined;
  otherwise returns false.

  \sa QScriptEngine::undefinedValue()
*/
bool QScriptValue::isUndefined() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isUndefined();
}

/*!
  Returns true if this QScriptValue is an object of the Error class;
  otherwise returns false.

  \sa QScriptContext::throwError()
*/
bool QScriptValue::isError() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isError();
}

/*!
  Returns true if this QScriptValue is an object of the Array class;
  otherwise returns false.

  \sa QScriptEngine::newArray()
*/
bool QScriptValue::isArray() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isArray();
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
    QScriptIsolate api(d->engine());
    return d->isObject();
}

/*!
  Returns true if this QScriptValue is a function; otherwise returns
  false.

  \sa call()
*/
bool QScriptValue::isFunction() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isCallable();
}

/*!
  Returns true if this QScriptValue is a variant value;
  otherwise returns false.

  \sa toVariant(), QScriptEngine::newVariant()
*/
bool QScriptValue::isVariant() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isVariant();
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
    QScriptIsolate api(d->engine());
    return d->toString();
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
    QScriptIsolate api(d->engine());
    return d->toNumber();
}

/*!
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
    QScriptIsolate api(d->engine());
    return d->toBool();
}

/*!
  \obsolete

  Use toBool() instead.
*/
bool QScriptValue::toBoolean() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->toBool();
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
    QScriptIsolate api(d->engine());
    return d->toInteger();
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
    QScriptIsolate api(d->engine());
    return d->toInt32();
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
    QScriptIsolate api(d->engine());
    return d->toUInt32();
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
    QScriptIsolate api(d->engine());
    return d->toUInt16();
}

/*!
  \obsolete

  This function is obsolete; use QScriptEngine::toObject() instead.
*/
QScriptValue QScriptValue::toObject() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->toObject());
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
    \row    \o Array Object \o The array is converted to a QVariantList. Each element is converted to a QVariant, recursively; cyclic references are not followed.
    \row    \o Object     \o The object is converted to a QVariantMap. Each property is converted to a QVariant, recursively; cyclic references are not followed.
    \endtable

  \sa isVariant()
*/
QVariant QScriptValue::toVariant() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    QScriptDeclarativeClass *cls = QScriptDeclarativeClassObject::declarativeClass(d);
    if (cls)
        return cls->toVariant(QScriptDeclarativeClassObject::object(d));
    return d->toVariant();
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
QScriptValue QScriptValue::call(const QScriptValue& thisObject, const QScriptValueList& args)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    return d->call(QScriptValuePrivate::get(thisObject), args);
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
QScriptValue QScriptValue::call(const QScriptValue &thisObject, const QScriptValue &arguments)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    return d->call(QScriptValuePrivate::get(thisObject), arguments);
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
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->construct(args));
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
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->construct(arguments));
}


/*!
  Returns the QScriptEngine that created this QScriptValue,
  or 0 if this QScriptValue is invalid or the value is not
  associated with a particular engine.
*/
QScriptEngine* QScriptValue::engine() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    QScriptEnginePrivate* engine = d->engine();
    if (engine)
        return QScriptEnginePrivate::get(engine);
    return 0;
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
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->prototype());
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
void QScriptValue::setPrototype(const QScriptValue& prototype)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    d->setPrototype(QScriptValuePrivate::get(prototype));
}

/*!
  Assigns the \a other value to this QScriptValue.

  Note that if \a other is an object (isObject() returns true),
  only a reference to the underlying object will be assigned;
  the object itself will not be copied.
*/
QScriptValue& QScriptValue::operator=(const QScriptValue& other)
{
    d_ptr = other.d_ptr;
    return *this;
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
bool QScriptValue::equals(const QScriptValue& other) const
{
    Q_D(const QScriptValue);
    QScriptValuePrivate* otherValue = QScriptValuePrivate::get(other);
    QScriptIsolate api(d->engine() ? d->engine() : otherValue->engine());
    return d_ptr->equals(otherValue);
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
bool QScriptValue::strictlyEquals(const QScriptValue& other) const
{
    Q_D(const QScriptValue);
    QScriptValuePrivate* o = QScriptValuePrivate::get(other);
    QScriptIsolate api(d->engine() ? d->engine() : o->engine());
    return d_ptr->strictlyEquals(o);
}

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
    Q_D(const QScriptValue);
    QScriptValuePrivate *o = QScriptValuePrivate::get(other);
    QScriptIsolate api(d->engine() ? d->engine() : o->engine());
    return d->lessThan(o);
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
    QScriptIsolate api(d->engine());
    return d->instanceOf(QScriptValuePrivate::get(other));
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
QScriptValue QScriptValue::property(const QString& name, const ResolveFlags& mode) const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->property(name, mode));
}

/*!
  \overload

  Returns the value of this QScriptValue's property with the given \a name,
  using the given \a mode to resolve the property.

  This overload of property() is useful when you need to look up the
  same property repeatedly, since the lookup can be performed faster
  when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle(), setProperty()
*/
QScriptValue QScriptValue::property(const QScriptString& name, const ResolveFlags& mode) const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->property(QScriptStringPrivate::get(name), mode));
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
QScriptValue QScriptValue::property(quint32 arrayIndex, const ResolveFlags& mode) const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->property(arrayIndex, mode));
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
void QScriptValue::setProperty(const QString& name, const QScriptValue& value, const PropertyFlags& flags)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    d->setProperty(name, QScriptValuePrivate::get(value), QScriptConverter::toPropertyAttributes(flags));
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
void QScriptValue::setProperty(quint32 arrayIndex, const QScriptValue& value, const PropertyFlags& flags)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    d->setProperty(arrayIndex, QScriptValuePrivate::get(value), QScriptConverter::toPropertyAttributes(flags));
}

/*!
  Sets the value of this QScriptValue's property with the given \a
  name to the given \a value. The given \a flags specify how this
  property may be accessed by script code.

  This overload of setProperty() is useful when you need to set the
  same property repeatedly, since the operation can be performed
  faster when the name is represented as an interned string.

  \sa QScriptEngine::toStringHandle()
*/
void QScriptValue::setProperty(const QScriptString& name, const QScriptValue& value, const PropertyFlags& flags)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    d->setProperty(QScriptStringPrivate::get(name), QScriptValuePrivate::get(value), QScriptConverter::toPropertyAttributes(flags));
}

/*!
  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QString& name, const ResolveFlags& mode) const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->propertyFlags(name, mode);
}

/*!
  Returns the flags of the property with the given \a name, using the
  given \a mode to resolve the property.

  \sa property()
*/
QScriptValue::PropertyFlags QScriptValue::propertyFlags(const QScriptString& name, const ResolveFlags& mode) const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->propertyFlags(QScriptStringPrivate::get(name), mode);
}

/*!
 * If this QScriptValue is a QObject, returns the QObject pointer
 * that the QScriptValue represents; otherwise, returns 0.
 *
 * If the QObject that this QScriptValue wraps has been deleted,
 * this function returns 0 (i.e. it is possible for toQObject()
 * to return 0 even when isQObject() returns true).
 *
 * \sa isQObject()
 */
QObject *QScriptValue::toQObject() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    QScriptDeclarativeClass *cls = QScriptDeclarativeClassObject::declarativeClass(d);
    if (cls)
        return cls->toQObject(QScriptDeclarativeClassObject::object(d));
    return d->toQObject();
}

/*!
  If this QScriptValue is a QMetaObject, returns the QMetaObject pointer
  that the QScriptValue represents; otherwise, returns 0.

  \sa isQMetaObject()
*/
const QMetaObject *QScriptValue::toQMetaObject() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->toQMetaObject();
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
    QScriptIsolate api(d->engine());
    return d->toDataTime();
}

/*!
  Returns the QRegExp representation of this value.
  If this QScriptValue is not a regular expression, an empty
  QRegExp is returned.

  \sa isRegExp()
*/
QRegExp QScriptValue::toRegExp() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->toRegExp();
}

/*!
  Returns true if this QScriptValue is an object of the Date class;
  otherwise returns false.

  \sa QScriptEngine::newDate()
*/
bool QScriptValue::isDate() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isDate();
}

/*!
  Returns true if this QScriptValue is an object of the RegExp class;
  otherwise returns false.

  \sa QScriptEngine::newRegExp()
*/
bool QScriptValue::isRegExp() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isRegExp();
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
    QScriptIsolate api(d->engine());
    QScriptDeclarativeClass *cls = QScriptDeclarativeClassObject::declarativeClass(d);
    if (cls)
        return cls->isQObject();
    return d->isQObject();
}

/*!
  Returns true if this QScriptValue is a QMetaObject; otherwise returns
  false.

  \sa toQMetaObject(), QScriptEngine::newQMetaObject()
*/
bool QScriptValue::isQMetaObject() const
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    return d->isQMetaObject();
}

/*!
  \internal
*/
QScriptValue QScriptValue::scope() const
{
    // FIXME can it be removed?
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  \internal
*/
void QScriptValue::setScope(const QScriptValue &)
{
    // FIXME can it be removed?
    Q_UNIMPLEMENTED();
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
    QScriptIsolate api(d->engine());
    return QScriptValuePrivate::get(d->data());
}

/*!
  \since 4.4

  Sets the internal \a data of this QScriptValue object. You can use
  this function to set object-specific data that won't be directly
  accessible to scripts, but may be retrieved in C++ using the data()
  function.

  \sa QScriptEngine::reportAdditionalMemoryCost()
*/
void QScriptValue::setData(const QScriptValue &value)
{
    Q_D(const QScriptValue);
    QScriptIsolate api(d->engine());
    d->setData(QScriptValuePrivate::get(value));
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
    QScriptIsolate api(d->engine());
    return QScriptClassPrivate::safeGet(d->scriptClass());
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
void QScriptValue::setScriptClass(QScriptClass *scriptclass)
{
    Q_D(QScriptValue);
    QScriptIsolate api(d->engine());
    d->setScriptClass(QScriptClassPrivate::safeGet(scriptclass));
}

/*!
  \internal
  Get script class if it exists
  \note it can be null
*/
QScriptClassPrivate* QScriptValuePrivate::scriptClass() const
{
    QScriptClassObject *data = QScriptClassObject::safeGet(this);
    if (data)
        return data->scriptClass();
    return 0;
}

void QScriptValuePrivate::setScriptClass(QScriptClassPrivate *scriptclass)
{
    if (!isObject())
        return;

    v8::HandleScope scope;
    // FIXME this algorithm is bad. It creates new value instead to add functionality to exiting one
    // This code would fail
    // engine.evaluate("a = new Object()");
    // QSV obj1 = engine.evaluate("a");
    // QSV obj2 = engine.evaluate("a");
    // obj1.setScriptClass(scriptclass);
    // QVERIFY(obj1.strictlyEquals(obj2);

    QScriptClassObject *data = QScriptClassObject::safeGet(this);
    if (data) {
        data->setScriptClass(scriptclass);
        if (!scriptclass) {
            if (data->original().IsEmpty())
                data->setOriginal(v8::Object::New());
            reinitialize(engine(), data->original());
        }
        return;
    }
    if (!scriptclass)
        return;

    v8::Handle<v8::Object> self = v8::Handle<v8::Object>::Cast(m_value);
    v8::Handle<v8::Value> newObject = QScriptClassObject::newInstance(scriptclass, self, engine());
    reinitialize(engine(), newObject);
}


/*!
  \internal

  Returns the ID of this object, or -1 if this QScriptValue is not an
  object.

  \sa QScriptEngine::objectById()
*/
qint64 QScriptValue::objectId() const
{
    Q_D(const QScriptValue);
    return d->objectId();
}

QT_END_NAMESPACE
