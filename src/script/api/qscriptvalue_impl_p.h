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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QSCRIPTVALUE_IMPL_P_H
#define QSCRIPTVALUE_IMPL_P_H

#include "qscriptvalue_p.h"
#include "qscriptclass_p.h"
#include "qscriptdeclarativeclassobject_p.h"
#include "qscriptengine_p.h"
#include "qscriptqobject_p.h"
#include "qscriptisolate_p.h"

QT_BEGIN_NAMESPACE


QScriptValuePrivate* QScriptValuePrivate::get(const QScriptValue& q) { Q_ASSERT(q.d_ptr.data()); return q.d_ptr.data(); }

QScriptValue QScriptValuePrivate::get(const QScriptValuePrivate* d)
{
    Q_ASSERT(d);
    return QScriptValue(const_cast<QScriptValuePrivate*>(d));
}

QScriptValue QScriptValuePrivate::get(QScriptPassPointer<QScriptValuePrivate> d)
{
    Q_ASSERT(d);
    return QScriptValue(d);
}

QScriptValue QScriptValuePrivate::get(QScriptValuePrivate* d)
{
    Q_ASSERT(d);
    return QScriptValue(d);
}

QScriptValuePrivate::QScriptValuePrivate()
    : m_engine(0), m_state(Invalid)
{
}

QScriptValuePrivate::QScriptValuePrivate(bool value)
    : m_engine(0), m_state(CBool), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(int value)
    : m_engine(0), m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(uint value)
    : m_engine(0), m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(qsreal value)
    : m_engine(0), m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(const QString& value)
    : m_engine(0), m_state(CString), u(new QString(value))
{
}

QScriptValuePrivate::QScriptValuePrivate(QScriptValue::SpecialValue value)
    : m_engine(0), m_state(value == QScriptValue::NullValue ? CNull : CUndefined)
{
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, bool value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, int value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, uint value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, qsreal value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, const QString& value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, QScriptValue::SpecialValue value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
    m_engine->registerValue(this);
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Value> value)
    : m_engine(engine), m_state(JSValue), m_value(v8::Persistent<v8::Value>::New(value))
{
    Q_ASSERT(engine);
    // It shouldn't happen, v8 shows errors by returning an empty handler. This is important debug
    // information and it can't be simply ignored.
    Q_ASSERT(!value.IsEmpty());
    m_engine->registerValue(this);
}

QScriptValuePrivate::~QScriptValuePrivate()
{
    if (isJSBased()) {
        m_engine->unregisterValue(this);
        QScriptIsolate api(m_engine);
        m_value.Dispose();
    } else if (isStringBased()) {
        delete u.m_string;
    }
}

bool QScriptValuePrivate::toBool() const
{
    switch (m_state) {
    case JSValue:
        {
            v8::HandleScope scope;
            return m_value->ToBoolean()->Value();
        }
    case CNumber:
        return !(qIsNaN(u.m_number) || !u.m_number);
    case CBool:
        return u.m_bool;
    case Invalid:
    case CNull:
    case CUndefined:
        return false;
    case CString:
        return u.m_string->length();
    }

    Q_ASSERT_X(false, "toBool()", "Not all states are included in the previous switch statement.");
    return false; // Avoid compiler warning.
}

qsreal QScriptValuePrivate::toNumber() const
{
    switch (m_state) {
    case JSValue:
    {
        v8::HandleScope scope;
        return m_value->ToNumber()->Value();
    }
    case CNumber:
        return u.m_number;
    case CBool:
        return u.m_bool ? 1 : 0;
    case CNull:
    case Invalid:
        return 0;
    case CUndefined:
        return qQNaN();
    case CString:
        bool ok;
        qsreal result = u.m_string->toDouble(&ok);
        if (ok)
            return result;
        result = u.m_string->toInt(&ok, 0); // Try other bases.
        if (ok)
            return result;
        if (*u.m_string == QLatin1String("Infinity"))
            return qInf();
        if (*u.m_string == QLatin1String("-Infinity"))
            return -qInf();
        return u.m_string->length() ? qQNaN() : 0;
    }

    Q_ASSERT_X(false, "toNumber()", "Not all states are included in the previous switch statement.");
    return 0; // Avoid compiler warning.
}

QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::toObject(QScriptEnginePrivate* engine) const
{
    Q_ASSERT(engine);
    if (this->engine() && engine != this->engine()) {
        qWarning("QScriptEngine::toObject: cannot convert value created in a different engine");
        return InvalidValue();
    }

    v8::HandleScope scope;
    switch (m_state) {
    case Invalid:
    case CNull:
    case CUndefined:
        return new QScriptValuePrivate;
    case CString:
        return new QScriptValuePrivate(engine, engine->makeJSValue(*u.m_string)->ToObject());
    case CNumber:
        return new QScriptValuePrivate(engine, engine->makeJSValue(u.m_number)->ToObject());
    case CBool:
        return new QScriptValuePrivate(engine, engine->makeJSValue(u.m_bool)->ToObject());
    case JSValue:
        if (m_value->IsObject())
            return const_cast<QScriptValuePrivate*>(this);
        if (m_value->IsNull() || m_value->IsUndefined()) // avoid "Uncaught TypeError: Cannot convert null to object"
            return InvalidValue();
        return new QScriptValuePrivate(engine, m_value->ToObject());
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not all states are included in this switch");
        return InvalidValue();
    }
}

/*!
  This method is created only for QScriptValue::toObject() purpose which is obsolete.
  \internal
 */
QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::toObject() const
{
    if (isJSBased())
        return toObject(engine());

    // Without an engine there is not much we can do.
    return new QScriptValuePrivate;
}

QString QScriptValuePrivate::toString() const
{
    switch (m_state) {
    case Invalid:
        return QString();
    case CBool:
        return u.m_bool ? QString::fromLatin1("true") : QString::fromLatin1("false");
    case CString:
        return *u.m_string;
    case CNumber:
        return QScriptConverter::toString(u.m_number);
    case CNull:
        return QString::fromLatin1("null");
    case CUndefined:
        return QString::fromLatin1("undefined");
    case JSValue:
        Q_ASSERT(!m_value.IsEmpty());
        v8::HandleScope handleScope;
        v8::TryCatch tryCatch;
        v8::Local<v8::String> result = m_value->ToString();
        if (result.IsEmpty()) {
            result = tryCatch.Exception()->ToString();
            m_engine->setException(tryCatch.Exception(), tryCatch.Message());
        }
        return QScriptConverter::toString(result);
    }

    Q_ASSERT_X(false, "toString()", "Not all states are included in the previous switch statement.");
    return QString(); // Avoid compiler warning.
}

QVariant QScriptValuePrivate::toVariant() const
{
    switch (m_state) {
        case Invalid:
            return QVariant();
        case CBool:
            return QVariant(u.m_bool);
        case CString:
            return QVariant(*u.m_string);
        case CNumber:
            return QVariant(u.m_number);
        case CNull:
            return QVariant();
        case CUndefined:
            return QVariant();
        case JSValue:
            break;
    }

    Q_ASSERT(m_state == JSValue);
    Q_ASSERT(!m_value.IsEmpty());
    Q_ASSERT(m_engine);

    v8::HandleScope handleScope;
    return m_engine->variantFromJS(m_value);
}

inline QDateTime QScriptValuePrivate::toDataTime() const
{
    if (!isDate())
        return QDateTime();

    v8::HandleScope handleScope;
    return engine()->qtDateTimeFromJS(v8::Handle<v8::Date>::Cast(m_value));

}

inline QRegExp QScriptValuePrivate::toRegExp() const
{
    if (!isRegExp())
        return QRegExp();

    v8::HandleScope handleScope;
    return QScriptConverter::toRegExp(v8::Handle<v8::RegExp>::Cast(m_value));
}

QObject* QScriptValuePrivate::toQObject() const
{
    if (!isJSBased())
        return 0;

    v8::HandleScope handleScope;
    return engine()->qtObjectFromJS(m_value);
}

const QMetaObject *QScriptValuePrivate::toQMetaObject() const
{
    if (!isQMetaObject())
        return 0;
    v8::HandleScope handleScope;
    QtMetaObjectData *data = QtMetaObjectData::get(*this);
    Q_ASSERT(data);
    return data->metaObject();
}

qsreal QScriptValuePrivate::toInteger() const
{
    qsreal result = toNumber();
    if (qIsNaN(result))
        return 0;
    if (qIsInf(result))
        return result;
    return (result > 0) ? qFloor(result) : -1 * qFloor(-result);
}

qint32 QScriptValuePrivate::toInt32() const
{
    qsreal result = toInteger();
    // Orginaly it should look like that (result == 0 || qIsInf(result) || qIsNaN(result)), but
    // some of these operation are invoked in toInteger subcall.
    if (qIsInf(result))
        return 0;
    return result;
}

quint32 QScriptValuePrivate::toUInt32() const
{
    qsreal result = toInteger();
    // Orginaly it should look like that (result == 0 || qIsInf(result) || qIsNaN(result)), but
    // some of these operation are invoked in toInteger subcall.
    if (qIsInf(result))
        return 0;
    return result;
}

quint16 QScriptValuePrivate::toUInt16() const
{
    return toInt32();
}

inline bool QScriptValuePrivate::isArray() const
{
    return isJSBased() && m_value->IsArray();
}

inline bool QScriptValuePrivate::isBool() const
{
    return m_state == CBool || (isJSBased() && m_value->IsBoolean());
}

inline bool QScriptValuePrivate::isCallable() const
{
    if (isFunction())
        return true;
    if (isObject()) {
        // Our C++ wrappers register function handlers but not always act as callables.
        QScriptDeclarativeClass *declarativeClass = QScriptDeclarativeClassObject::declarativeClass(this);
        if (declarativeClass)
            return declarativeClass->supportsCall();
        QScriptClassObject *scriptClassObject = QScriptClassObject::safeGet(this);
        if (scriptClassObject && scriptClassObject->scriptClass())
            return scriptClassObject->scriptClass()->userCallback()->supportsExtension(QScriptClass::Callable);
        return v8::Object::Cast(*m_value)->IsCallable();
    }
    return false;
}

inline bool QScriptValuePrivate::isError() const
{
    if (!isJSBased())
        return false;
    v8::HandleScope handleScope;
    return m_value->IsError();
}

inline bool QScriptValuePrivate::isFunction() const
{
    return isJSBased() && m_value->IsFunction();
}

inline bool QScriptValuePrivate::isNull() const
{
    return m_state == CNull || (isJSBased() && m_value->IsNull());
}

inline bool QScriptValuePrivate::isNumber() const
{
    return m_state == CNumber || (isJSBased() && m_value->IsNumber());
}

inline bool QScriptValuePrivate::isObject() const
{
    return isJSBased() && m_value->IsObject();
}

inline bool QScriptValuePrivate::isString() const
{
    return m_state == CString || (isJSBased() && m_value->IsString());
}

inline bool QScriptValuePrivate::isUndefined() const
{
    return m_state == CUndefined || (isJSBased() && m_value->IsUndefined());
}

inline bool QScriptValuePrivate::isValid() const
{
    return m_state != Invalid;
}

inline bool QScriptValuePrivate::isVariant() const
{
    return isJSBased() && m_engine->isQtVariant(m_value);
}

bool QScriptValuePrivate::isDate() const
{
    return (isJSBased() && m_value->IsDate());
}

bool QScriptValuePrivate::isRegExp() const
{
    return (isJSBased() && m_value->IsRegExp());
}

bool QScriptValuePrivate::isQObject() const
{
    return isJSBased() && engine()->isQtObject(m_value);
}

bool QScriptValuePrivate::isQMetaObject() const
{
    if (!isJSBased() || !m_value->IsObject())
        return false;
    return m_engine->isQtMetaObject(m_value);
}

inline bool QScriptValuePrivate::equals(QScriptValuePrivate* other)
{
    if (!isValid())
        return !other->isValid();

    if (!other->isValid())
        return false;

    if (!isJSBased() && !other->isJSBased()) {
        switch (m_state) {
        case CNull:
        case CUndefined:
            return other->isUndefined() || other->isNull();
        case CNumber:
            switch (other->m_state) {
            case CBool:
            case CString:
                return u.m_number == other->toNumber();
            case CNumber:
                return u.m_number == other->u.m_number;
            default:
                return false;
            }
        case CBool:
            switch (other->m_state) {
            case CBool:
                return u.m_bool == other->u.m_bool;
            case CNumber:
                return toNumber() == other->u.m_number;
            case CString:
                return toNumber() == other->toNumber();
            default:
                return false;
            }
        case CString:
            switch (other->m_state) {
            case CBool:
                return toNumber() == other->toNumber();
            case CNumber:
                return toNumber() == other->u.m_number;
            case CString:
                return *u.m_string == *other->u.m_string;
            default:
                return false;
            }
        default:
            Q_ASSERT_X(false, "QScriptValue::equals", "Not all states are included in the previous switch statement.");
        }
    }

    v8::HandleScope handleScope;
    if (isJSBased() && !other->isJSBased()) {
        if (!other->assignEngine(engine())) {
            qWarning("QScriptValue::equals: cannot compare to a value created in a different engine");
            return false;
        }
    } else if (!isJSBased() && other->isJSBased()) {
        if (!assignEngine(other->engine())) {
            qWarning("QScriptValue::equals: cannot compare to a value created in a different engine");
            return false;
        }
    }

    Q_ASSERT(this->engine() && other->engine());
    if (this->engine() != other->engine()) {
        qWarning("QScriptValue::equals: cannot compare to a value created in a different engine");
        return false;
    }

    // The next line does not work because it fails to compare the global object
    // http://code.google.com/p/v8/issues/detail?id=1078 and http://code.google.com/p/v8/issues/detail?id=1082
    //return m_value->Equals(other->m_value);

    // FIXME: equal can throw an exception which will be dropped by this code:
    m_engine->saveException();
    QScriptSharedDataPointer<QScriptValuePrivate> cmp(m_engine->evaluate(
        QString::fromLatin1("(function(a,b, global){"
            "return (a == b) || (a == global && b == this) || (b == global && a == this);})")));
    Q_ASSERT(cmp->isFunction());
    v8::Handle<v8::Value> args[3] = { m_value, other->m_value, m_engine->globalObject() };
    QScriptSharedDataPointer<QScriptValuePrivate> resultValue(cmp->call(0, 3, args));
    bool result = resultValue->toBool();
    m_engine->restoreException();
    return result;
}

inline bool QScriptValuePrivate::strictlyEquals(QScriptValuePrivate* other)
{
    if (isJSBased()) {
        // We can't compare these two values without binding to the same engine.
        if (!other->isJSBased()) {
            if (other->assignEngine(engine()))
                return m_value->StrictEquals(other->m_value);
            return false;
        }
        if (other->engine() != engine()) {
            qWarning("QScriptValue::strictlyEquals: cannot compare to a value created in a different engine");
            return false;
        }

        // The next line does not work because it fails to compare the global object
        // http://code.google.com/p/v8/issues/detail?id=1078 and http://code.google.com/p/v8/issues/detail?id=1082
        //return m_value->StrictEquals(other->m_value);
        v8::HandleScope handleScope;
        m_engine->saveException();
        QScriptSharedDataPointer<QScriptValuePrivate> cmp(m_engine->evaluate(
            QString::fromLatin1("(function(a,b, global){"
            "return (a === b) || (a === global && b === this) || (b === global && a === this);})")));
        Q_ASSERT(cmp->isFunction());
        v8::Handle<v8::Value> args[3] = { m_value, other->m_value, m_engine->globalObject() };
        QScriptSharedDataPointer<QScriptValuePrivate> resultValue(cmp->call(0, 3, args));
        bool result = resultValue->toBool();
        m_engine->restoreException();
        return result;
    }
    if (isStringBased()) {
        if (other->isStringBased())
            return *u.m_string == *(other->u.m_string);
        if (other->isJSBased()) {
            assignEngine(other->engine());
            return m_value->StrictEquals(other->m_value);
        }
    }
    if (isNumberBased()) {
        if (other->isJSBased()) {
            assignEngine(other->engine());
            return m_value->StrictEquals(other->m_value);
        }
        if (m_state != other->m_state)
            return false;
        if (m_state == CNumber)
            return u.m_number == other->u.m_number;
        Q_ASSERT(m_state == CBool);
        return u.m_bool == other->u.m_bool;
    }

    if (!isValid() && !other->isValid())
        return true;

    return false;
}

inline bool QScriptValuePrivate::lessThan(QScriptValuePrivate *other) const
{
    if (engine() != other->engine() && engine() && other->engine()) {
        qWarning("QScriptValue::lessThan: cannot compare to a value created in a different engine");
        return false;
    }

    if (!isValid() || !other->isValid())
        return false;

    if (isString() && other->isString())
        return toString() < other->toString();

    if (isObject() || other->isObject()) {
        v8::HandleScope handleScope;
        QScriptEnginePrivate *eng = m_engine ? engine() : other->engine();
        // FIXME: lessThan can throw an exception which will be dropped by this code:
        Q_ASSERT(eng);
        eng->saveException();
        QScriptSharedDataPointer<QScriptValuePrivate> cmp(eng->evaluate(QString::fromLatin1("(function(a,b){return a<b})")));
        Q_ASSERT(cmp->isFunction());
        v8::Handle<v8::Value> args[2];
        cmp->prepareArgumentsForCall(args, QScriptValueList() << QScriptValuePrivate::get(this) << QScriptValuePrivate::get(other));
        QScriptSharedDataPointer<QScriptValuePrivate> resultValue(cmp->call(0, 2, args));
        bool result = resultValue->toBool();
        eng->restoreException();
        return result;
    }

    qsreal nthis = toNumber();
    qsreal nother = other->toNumber();
    if (qIsNaN(nthis) || qIsNaN(nother)) {
        // Should return undefined in ECMA standard.
        return false;
    }
    return nthis < nother;
}

inline bool QScriptValuePrivate::instanceOf(QScriptValuePrivate* other) const
{
    if (!isObject() || !other->isFunction())
        return false;
    if (engine() != other->engine()) {
        qWarning("QScriptValue::instanceof: cannot perform operation on a value created in a different engine");
        return false;
    }
    v8::HandleScope handleScope;
    return instanceOf(v8::Handle<v8::Object>::Cast(other->m_value));
}

inline bool QScriptValuePrivate::instanceOf(v8::Handle<v8::Object> other) const
{
    Q_ASSERT(isObject());
    Q_ASSERT(other->IsFunction());

    v8::Handle<v8::Object> self = v8::Handle<v8::Object>::Cast(m_value);
    v8::Handle<v8::Value> selfPrototype = self->GetPrototype();
    v8::Handle<v8::Value> otherPrototype = other->Get(v8::String::New("prototype"));

    while (!selfPrototype->IsNull()) {
        if (selfPrototype->StrictEquals(otherPrototype))
            return true;
        // In general a prototype can be an object or null, but in the loop it can't be null, so
        // we can cast it safely.
        selfPrototype = v8::Handle<v8::Object>::Cast(selfPrototype)->GetPrototype();
    }
    return false;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::prototype() const
{
    if (isObject()) {
        v8::HandleScope handleScope;
        return new QScriptValuePrivate(engine(), v8::Handle<v8::Object>::Cast(m_value)->GetPrototype());
    }
    return InvalidValue();
}

inline void QScriptValuePrivate::setPrototype(QScriptValuePrivate* prototype)
{
    if (isObject() && (prototype->isObject() || prototype->isNull())) {
        if (engine() != prototype->engine()) {
            if (prototype->engine()) {
                qWarning("QScriptValue::setPrototype() failed: cannot set a prototype created in a different engine");
                return;
            }
            prototype->assignEngine(engine());
        }
        v8::HandleScope handleScope;
        if (!v8::Handle<v8::Object>::Cast(m_value)->SetPrototype(*prototype))
            qWarning("QScriptValue::setPrototype() failed: cyclic prototype value");
    }
}

inline void QScriptValuePrivate::setProperty(const QScriptStringPrivate *name, QScriptValuePrivate* value, uint attribs)
{
    if (name->isValid())
        setProperty(name->asV8Value(), value, attribs);
}

inline void QScriptValuePrivate::setProperty(const QString& name, QScriptValuePrivate* value, uint attribs)
{
    if (!isObject())
        return;
    v8::HandleScope handleScope;
    setProperty(QScriptConverter::toString(name), value, attribs);
}

inline void QScriptValuePrivate::setProperty(v8::Handle<v8::String> name, QScriptValuePrivate* value, uint attribs)
{
    if (!isObject())
        return;

    if (!value->isJSBased())
        value->assignEngine(engine());

    if (!value->isValid()) {
        // Remove the property.
        v8::HandleScope handleScope;
        v8::TryCatch tryCatch;
        v8::Handle<v8::Object> recv(v8::Object::Cast(*m_value));
        if (attribs & QScriptValue::PropertyGetter && !(attribs & QScriptValue::PropertySetter)) {
            v8::Local<v8::Object> descriptor = engine()->originalGlobalObject()->getOwnPropertyDescriptor(recv, name);
            if (!descriptor.IsEmpty()) {
                v8::Local<v8::Value> setter = descriptor->Get(v8::String::New("set"));
                if (!setter.IsEmpty() && !setter->IsUndefined()) {
                    recv->Delete(name);
                    engine()->originalGlobalObject()->defineGetterOrSetter(recv, name, setter, QScriptValue::PropertySetter);
                    if (tryCatch.HasCaught())
                        engine()->setException(tryCatch.Exception(), tryCatch.Message());
                    return;
                }
            }
        } else if (attribs & QScriptValue::PropertySetter && !(attribs & QScriptValue::PropertyGetter)) {
            v8::Local<v8::Object> descriptor = engine()->originalGlobalObject()->getOwnPropertyDescriptor(recv, name);
            if (!descriptor.IsEmpty()) {
                v8::Local<v8::Value> getter = descriptor->Get(v8::String::New("get"));
                if (!getter.IsEmpty() && !getter->IsUndefined()) {
                    recv->Delete(name);
                    engine()->originalGlobalObject()->defineGetterOrSetter(recv, name, getter, QScriptValue::PropertyGetter);
                    if (tryCatch.HasCaught())
                        engine()->setException(tryCatch.Exception(), tryCatch.Message());
                    return;
                }
            }
        }
        recv->Delete(name);
        if (tryCatch.HasCaught())
            engine()->setException(tryCatch.Exception(), tryCatch.Message());
        return;
    }

    if (engine() != value->engine()) {
        qWarning("QScriptValue::setProperty(%s) failed: "
                 "cannot set value created in a different engine",
                 qPrintable(QScriptConverter::toString(name)));
        return;
    }

    v8::TryCatch tryCatch;
    if (attribs & (QScriptValue::PropertyGetter | QScriptValue::PropertySetter)) {
        engine()->originalGlobalObject()->defineGetterOrSetter(*this, name, value->m_value, attribs);
    } else {
        v8::Object::Cast(*m_value)->Set(name, value->m_value, v8::PropertyAttribute(attribs & QScriptConverter::PropertyAttributeMask));
    }
    if (tryCatch.HasCaught())
        engine()->setException(tryCatch.Exception(), tryCatch.Message());
}

inline void QScriptValuePrivate::setProperty(quint32 index, QScriptValuePrivate* value, uint attribs)
{
    // FIXME this method should by integrated with other overloads to use the same code patch.
    // for now it is not possible as v8 doesn't allow to set property attributes using index based api.

    if (!isObject())
        return;

    if (attribs) {
        // FIXME we dont need to convert index to a string.
        //Object::Set(int,value) do not take attributes.
        setProperty(QString::number(index), value, attribs);
        return;
    }

    if (!value->isJSBased())
        value->assignEngine(engine());

    if (!value->isValid()) {
        // Remove the property.
        v8::HandleScope handleScope;
        v8::TryCatch tryCatch;
        v8::Object::Cast(*m_value)->Delete(index);
        if (tryCatch.HasCaught())
            engine()->setException(tryCatch.Exception(), tryCatch.Message());
        return;
    }

    if (engine() != value->engine()) {
        qWarning("QScriptValue::setProperty() failed: cannot set value created in a different engine");
        return;
    }

    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Object::Cast(*m_value)->Set(index, value->m_value);
    if (tryCatch.HasCaught())
        engine()->setException(tryCatch.Exception(), tryCatch.Message());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(const QString& name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject() || !name.length())
        return InvalidValue();

    v8::HandleScope handleScope;
    return property(QScriptConverter::toString(name), mode);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject() || !name->isValid())
        return InvalidValue();

    v8::HandleScope handleScope;
    return property(name->asV8Value(), mode);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const
{
    Q_ASSERT(!name.IsEmpty());
    Q_ASSERT(isObject());
    return property<>(name, mode);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(quint32 index, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject())
        return InvalidValue();
    return property<>(index, mode);
}

template<typename T>
inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(T name, const QScriptValue::ResolveFlags& mode) const
{
    Q_ASSERT(isObject());
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> self(v8::Object::Cast(*m_value));

    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = self->Get(name);
    if (tryCatch.HasCaught()) {
        result = tryCatch.Exception();
        engine()->setException(result, tryCatch.Message());
        return new QScriptValuePrivate(engine(), result);
    }
    if (result.IsEmpty() || (result->IsUndefined() && !self->Has(name))) {
        // In QtScript we make a distinction between a property that exists and has value undefined,
        // and a property that doesn't exist; in the latter case, we should return an invalid value.
        return InvalidValue();
    }
    if (!(mode & QScriptValue::ResolvePrototype) && engine()->getOwnProperty(self, name).IsEmpty())
        return InvalidValue();

    return new QScriptValuePrivate(engine(), result);
}

inline bool QScriptValuePrivate::deleteProperty(const QString& name)
{
    if (!isObject())
        return false;

    v8::HandleScope handleScope;
    v8::Handle<v8::Object> self(v8::Handle<v8::Object>::Cast(m_value));
    return self->Delete(QScriptConverter::toString(name));
}

inline QScriptValue::PropertyFlags QScriptValuePrivate::propertyFlags(const QString& name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject())
        return QScriptValue::PropertyFlags(0);

    v8::HandleScope handleScope;
    return engine()->getPropertyFlags(v8::Handle<v8::Object>::Cast(m_value), QScriptConverter::toString(name), mode);
}

inline QScriptValue::PropertyFlags QScriptValuePrivate::propertyFlags(const QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject())
        return QScriptValue::PropertyFlags(0);

    v8::HandleScope handleScope;
    return engine()->getPropertyFlags(v8::Handle<v8::Object>::Cast(m_value), static_cast<v8::Handle<v8::String> >(*name), mode);
}

inline QScriptValue::PropertyFlags QScriptValuePrivate::propertyFlags(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject())
        return QScriptValue::PropertyFlags(0);

    v8::HandleScope handleScope;
    return engine()->getPropertyFlags(v8::Handle<v8::Object>::Cast(m_value), name, mode);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::data() const
{
    if (!isObject())
        return InvalidValue();
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> self(v8::Object::Cast(*m_value));
    v8::Handle<v8::Value> value = self->GetHiddenValue(engine()->qtDataId());
    if (value.IsEmpty())
        return InvalidValue();
    return new QScriptValuePrivate(engine(), value);
}

inline void QScriptValuePrivate::setData(QScriptValuePrivate* data) const
{
    if (!isObject())
        return;
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> self(v8::Object::Cast(*m_value));
    v8::Handle<v8::String> dataId = engine()->qtDataId();
    if (!data->assignEngine(engine()))
        self->DeleteHiddenValue(dataId);
    else
        self->SetHiddenValue(dataId, data->m_value);
}

//returns -1 if arguments is not an array, returns -2 if it is not on the same engine
inline int QScriptValuePrivate::convertArguments(QVarLengthArray<v8::Handle<v8::Value>, 8> *argv, const QScriptValue& arguments)
{
    // Convert all arguments and bind to the engine.
    QScriptValuePrivate *args = QScriptValuePrivate::get(arguments);

    if (!args->assignEngine(engine()))
        return -2;

    // argc == -1 will cause a type error to be thrown.
    int argc = -1;
    if (args->isArray()) {
        v8::Handle<v8::Array> array(v8::Array::Cast(*args->m_value));
        argc = array->Length();
        argv->resize(argc);
        for (int i = 0; i < argc; ++i)
            (*argv)[i] = array->Get(i);
    } else if (args->isObject()) {
        // FIXME probably we have to strip an Arguments object, for now there is no way to check
        // it for sure. Anyway lets do our best.
        QScriptSharedDataPointer<QScriptValuePrivate> lengthProp(args->property(v8::String::New("length"), QScriptValue::ResolveLocal));
        argc = lengthProp->toUInt32();
        v8::Handle<v8::Object> obj(v8::Object::Cast(*args->m_value));
        if (argc) {
            argv->resize(argc);
            for (int i = 0; i < argc; ++i)
                (*argv)[i] = obj->Get(i);
        } else
            argc = -1;
    } else if (args->isNull() || args->isUndefined()) {
        argc = 0;
    }

    return argc;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::call(QScriptValuePrivate* thisObject, const QScriptValueList& args)
{
    if (!isCallable())
        return InvalidValue();

    v8::HandleScope handleScope;

    // Convert all arguments and bind to the engine.
    int argc = args.size();
    QVarLengthArray<v8::Handle<v8::Value>, 8> argv(argc);
    if (!prepareArgumentsForCall(argv.data(), args)) {
        qWarning("QScriptValue::call() failed: cannot call function with argument created in a different engine");
        return InvalidValue();
    }

    return call(thisObject, argc, argv.data());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::call(QScriptValuePrivate* thisObject, const QScriptValue& arguments)
{
    if (!isCallable())
        return InvalidValue();

    v8::HandleScope handleScope;

    QVarLengthArray<v8::Handle<v8::Value>, 8> argv;
    int argc = convertArguments(&argv, arguments);
    if (argc == -2) {
        qWarning("QScriptValue::call() failed: cannot call function with thisObject created in a different engine");
        return InvalidValue();
    }
    return call(thisObject, argc, argv.data());
}


QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::call(QScriptValuePrivate* thisObject, int argc, v8::Handle<v8::Value> *argv)
{
    QScriptEnginePrivate *e = engine();

    v8::Handle<v8::Object> recv;

    if (!thisObject || !thisObject->isObject()) {
        recv = v8::Handle<v8::Object>(v8::Object::Cast(*e->globalObject()));
    } else {
        if (!thisObject->assignEngine(e)) {
            qWarning("QScriptValue::call() failed: cannot call function with thisObject created in a different engine");
            return InvalidValue();
        }

        recv = v8::Handle<v8::Object>(v8::Object::Cast(*thisObject->m_value));
    }

    if (argc < 0) {
        v8::Local<v8::Value> exeption = v8::Exception::TypeError(v8::String::New("Arguments must be an array"));
        e->setException(exeption);
        return new QScriptValuePrivate(e, exeption);
    }

    QScriptEnginePrivate::EvaluateScope evaluateScope(e);
    v8::TryCatch tryCatch;

    v8::Handle<v8::Value> result = v8::Object::Cast(*m_value)->Call(recv, argc, argv);

    if (result.IsEmpty()) {
        result = tryCatch.Exception();
        // TODO: figure out why v8 doesn't always produce an exception value.
        //Q_ASSERT(!result.IsEmpty());
        if (result.IsEmpty())
            result = v8::Exception::Error(v8::String::New("missing exception value"));
        e->setException(result, tryCatch.Message());
    }

    return new QScriptValuePrivate(e, result);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::construct(int argc, v8::Handle<v8::Value> *argv)
{
    QScriptEnginePrivate *e = engine();

    if (argc < 0) {
        v8::Local<v8::Value> exeption = v8::Exception::TypeError(v8::String::New("Arguments must be an array"));
        e->setException(exeption);
        return new QScriptValuePrivate(e, exeption);
    }

    QScriptEnginePrivate::EvaluateScope evaluateScope(e);
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = v8::Object::Cast(*m_value)->NewInstance(argc, argv);

    if (result.IsEmpty()) {
        result = tryCatch.Exception();
        e->setException(result, tryCatch.Message());
    }

    return new QScriptValuePrivate(e, result);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::construct(const QScriptValueList& args)
{
    if (isQMetaObject()) {
        QtMetaObjectData *data = QtMetaObjectData::get(*this);
        Q_ASSERT(data);
        QScriptSharedDataPointer<QScriptValuePrivate> ctor(new QScriptValuePrivate(engine(), data->constructor()));
        return ctor->construct(args);
    }
    // ### Should we support calling declarative classes as constructors?
    if (!isCallable() || QScriptDeclarativeClassObject::declarativeClass(this))
        return InvalidValue();

    v8::HandleScope handleScope;

    // Convert all arguments and bind to the engine.
    int argc = args.size();
    QVarLengthArray<v8::Handle<v8::Value>, 8> argv(argc);
    if (!prepareArgumentsForCall(argv.data(), args)) {
        qWarning("QScriptValue::construct() failed: cannot construct function with argument created in a different engine");
        return InvalidValue();
    }

    return construct(argc, argv.data());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::construct(const QScriptValue& arguments)
{
    // ### Should we support calling declarative classes as constructors?
    if (!isCallable() || QScriptDeclarativeClassObject::declarativeClass(this))
        return InvalidValue();

    v8::HandleScope handleScope;

    QVarLengthArray<v8::Handle<v8::Value>, 8> argv;
    int argc = convertArguments(&argv, arguments);
    if (argc == -2) {
        qWarning("QScriptValue::construct() failed: cannot construct function with argument created in a different engine");
        return InvalidValue();
    }

    return construct(argc, argv.data());
}

/*! \internal
 * Make sure this value is associated with a v8 value belogning to this engine.
 * If the value was invalid, or belogning to another engine, return false.
 */
bool QScriptValuePrivate::assignEngine(QScriptEnginePrivate* engine)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    switch (m_state) {
    case Invalid:
        return false;
    case CBool:
        m_value = v8::Persistent<v8::Value>::New(engine->makeJSValue(u.m_bool));
        break;
    case CString:
        m_value = v8::Persistent<v8::Value>::New(engine->makeJSValue(*u.m_string));
        delete u.m_string;
        break;
    case CNumber:
        m_value = v8::Persistent<v8::Value>::New(engine->makeJSValue(u.m_number));
        break;
    case CNull:
        m_value = v8::Persistent<v8::Value>::New(engine->makeJSValue(QScriptValue::NullValue));
        break;
    case CUndefined:
        m_value = v8::Persistent<v8::Value>::New(engine->makeJSValue(QScriptValue::UndefinedValue));
        break;
    default:
        if (this->engine() == engine)
            return true;
        else if (!isJSBased())
            Q_ASSERT_X(!isJSBased(), "assignEngine()", "Not all states are included in the previous switch statement.");
        else
            qWarning("JSValue can't be rassigned to an another engine.");
        return false;
    }
    m_engine = engine;
    m_state = JSValue;

    m_engine->registerValue(this);
    return true;
}

/*!
  \internal
  reinitialize this value to an invalid value.
*/
void QScriptValuePrivate::reinitialize()
{
    if (isJSBased()) {
        m_engine->unregisterValue(this);
        m_value.Dispose();
        m_value.Clear();
    } else if (isStringBased()) {
        delete u.m_string;
    }
    m_engine = 0;
    m_state = Invalid;
}

/*!
  \internal
  reinitialize this value to an JSValue.
*/
void QScriptValuePrivate::reinitialize(QScriptEnginePrivate* engine, v8::Handle<v8::Value> value)
{
    Q_ASSERT_X(this != InvalidValue(), Q_FUNC_INFO, "static invalid can't be reinitialized to a different value");
    if (isJSBased()) {
        m_value.Dispose();
        // avoid double registration
        m_engine->unregisterValue(this);
    } else if (isStringBased()) {
        delete u.m_string;
    }
    m_engine = engine;
    m_state = JSValue;
    m_value = v8::Persistent<v8::Value>::New(value);
    m_engine->registerValue(this);
}

QScriptEnginePrivate* QScriptValuePrivate::engine() const
{
    return m_engine;
}

inline QScriptValuePrivate::operator v8::Handle<v8::Value>() const
{
    Q_ASSERT(isJSBased());
    return m_value;
}

inline QScriptValuePrivate::operator v8::Handle<v8::Object>() const
{
    Q_ASSERT(isObject());
    return v8::Handle<v8::Object>::Cast(m_value);
}

/*!
 * Return a v8::Handle, assign to the engine if needed.
 */
v8::Handle<v8::Value> QScriptValuePrivate::asV8Value(QScriptEnginePrivate* engine)
{
    if (!m_engine) {
        if (!assignEngine(engine))
            return v8::Handle<v8::Value>();
    }
    Q_ASSERT(isJSBased());
    return m_value;
}

qint64 QScriptValuePrivate::objectId() const
{
    // FIXME: Get rid of that! It is really internal; based on QtScript 4.5 implementation detail, used
    // only by debugger and it causes gcc warnings.
    if (!isObject())
        return -1;
    return *reinterpret_cast<quintptr *>(*(m_value));
}

/*!
  \internal
  Returns true if QSV have an engine associated.
*/
bool QScriptValuePrivate::isJSBased() const
{
#ifndef QT_NO_DEBUG
    // internals check.
    if (m_state >= JSValue)
        Q_ASSERT(!m_value.IsEmpty());
    else
        Q_ASSERT(m_value.IsEmpty());
#endif
    return m_state >= JSValue;
}

/*!
  \internal
  Returns true if current value of QSV is placed in m_number.
*/
bool QScriptValuePrivate::isNumberBased() const { return m_state == CNumber || m_state == CBool; }

/*!
  \internal
  Returns true if current value of QSV is placed in m_string.
*/
bool QScriptValuePrivate::isStringBased() const { return m_state == CString; }

/*!
  \internal
  Converts arguments and bind them to the engine.
  \attention argv should be big enough
*/
inline bool QScriptValuePrivate::prepareArgumentsForCall(v8::Handle<v8::Value> argv[], const QScriptValueList& args) const
{
    QScriptValueList::const_iterator i = args.constBegin();
    for (int j = 0; i != args.constEnd(); j++, i++) {
        QScriptValuePrivate* value = QScriptValuePrivate::get(*i);
        if ((value->isJSBased() && engine() != value->engine())
                || (!value->isJSBased() && value->isValid() && !value->assignEngine(engine())))
            // Different engines are not allowed!
            return false;
        if (value->isValid())
            argv[j] = *value;
        else
            argv[j] = engine()->makeJSValue(QScriptValue::UndefinedValue);
    }
    return true;
}

QT_END_NAMESPACE

#endif
