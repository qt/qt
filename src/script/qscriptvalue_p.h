/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSCRIPTVALUE_P_H
#define QSCRIPTVALUE_P_H

#include <v8.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qmath.h>
#include <QtCore/qvarlengtharray.h>
#include <qdebug.h>

#include "qscriptconverter_p.h"
#include "qscriptshareddata_p.h"
#include "qscriptvalue.h"
#include "qscriptstring_p.h"

QT_BEGIN_NAMESPACE

class QScriptClassPrivate;
/*
  \internal
  \class QScriptValuePrivate
  TODO we need to preallocate some values
    // TODO: bring back these!
    //
    //    inline void* operator new(size_t, QScriptEnginePrivate*);
    //    inline void operator delete(void*);
*/
class QScriptValuePrivate : public QScriptSharedData
{
public:

    inline static QScriptValuePrivate* get(const QScriptValue& q);
    inline static QScriptValue get(const QScriptValuePrivate* d);
    inline static QScriptValue get(QScriptValuePrivate* d);
    inline static QScriptValue get(QScriptPassPointer<QScriptValuePrivate> d);
    inline ~QScriptValuePrivate();

    inline QScriptValuePrivate();
    inline QScriptValuePrivate(bool value);
    inline QScriptValuePrivate(int value);
    inline QScriptValuePrivate(uint value);
    inline QScriptValuePrivate(qsreal value);
    inline QScriptValuePrivate(const QString& value);
    inline QScriptValuePrivate(QScriptValue::SpecialValue value);

    inline QScriptValuePrivate(QScriptEnginePrivate* engine, bool value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, int value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, uint value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, qsreal value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, const QString& value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, QScriptValue::SpecialValue value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, v8::Handle<v8::Value>);
    inline void reinitialize();
    inline void reinitialize(QScriptEnginePrivate* engine, v8::Handle<v8::Value>);

    inline bool toBool() const;
    inline qsreal toNumber() const;
    inline QScriptPassPointer<QScriptValuePrivate> toObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> toObject(QScriptEnginePrivate* engine) const;
    inline QString toString() const;
    inline qsreal toInteger() const;
    inline qint32 toInt32() const;
    inline quint32 toUInt32() const;
    inline quint16 toUInt16() const;
    inline QDateTime toDataTime() const;
    inline QRegExp toRegExp() const;
    inline QObject *toQObject() const;
    inline QVariant toVariant() const;
    inline const QMetaObject *toQMetaObject() const;

    inline bool isArray() const;
    inline bool isBool() const;
    inline bool isCallable() const;
    inline bool isError() const;
    inline bool isFunction() const;
    inline bool isNull() const;
    inline bool isNumber() const;
    inline bool isObject() const;
    inline bool isString() const;
    inline bool isUndefined() const;
    inline bool isValid() const;
    inline bool isVariant() const;
    inline bool isDate() const;
    inline bool isRegExp() const;
    inline bool isQObject() const;
    inline bool isQMetaObject() const;

    inline bool equals(QScriptValuePrivate* other);
    inline bool strictlyEquals(QScriptValuePrivate* other);
    inline bool lessThan(QScriptValuePrivate *other) const;
    inline bool instanceOf(QScriptValuePrivate*) const;
    inline bool instanceOf(v8::Handle<v8::Object> other) const;

    inline QScriptPassPointer<QScriptValuePrivate> prototype() const;
    inline void setPrototype(QScriptValuePrivate* prototype);
    QScriptClassPrivate* scriptClass() const;
    void setScriptClass(QScriptClassPrivate* scriptclass);

    inline void setProperty(const QScriptStringPrivate *name, QScriptValuePrivate *value, v8::PropertyAttribute attribs = v8::None);
    inline void setProperty(const QString& name, QScriptValuePrivate *value, v8::PropertyAttribute attribs = v8::None);
    inline void setProperty(v8::Handle<v8::String> name, QScriptValuePrivate *value, v8::PropertyAttribute attribs = v8::None);
    inline void setProperty(quint32 index, QScriptValuePrivate *value, v8::PropertyAttribute attribs = v8::None);
    inline QScriptPassPointer<QScriptValuePrivate> property(const QString& name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(quint32 index, const QScriptValue::ResolveFlags& mode) const;
    template<typename T>
    inline QScriptPassPointer<QScriptValuePrivate> property(T name, const QScriptValue::ResolveFlags& mode) const;
    inline bool deleteProperty(const QString& name);
    inline QScriptValue::PropertyFlags propertyFlags(const QString& name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptValue::PropertyFlags propertyFlags(const QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptValue::PropertyFlags propertyFlags(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const;
    inline void setData(QScriptValuePrivate* value) const;
    inline QScriptPassPointer<QScriptValuePrivate> data() const;

    inline int convertArguments(QVarLengthArray<v8::Handle<v8::Value>, 8> *argv, const QScriptValue& arguments);

    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, const QScriptValueList& args);
    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, const QScriptValue& arguments);
    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, int argc, v8::Handle< v8::Value >* argv);
    inline QScriptPassPointer<QScriptValuePrivate> construct(int argc, v8::Handle<v8::Value> *argv);
    inline QScriptPassPointer<QScriptValuePrivate> construct(const QScriptValueList& args);
    inline QScriptPassPointer<QScriptValuePrivate> construct(const QScriptValue& arguments);

    inline bool assignEngine(QScriptEnginePrivate* engine);
    inline QScriptEnginePrivate* engine() const;

    inline operator v8::Handle<v8::Value>() const;
    inline operator v8::Handle<v8::Object>() const;
    inline v8::Handle<v8::Value> asV8Value(QScriptEnginePrivate* engine);
private:
    QScriptSharedDataPointer<QScriptEnginePrivate> m_engine;

    // Please, update class documentation when you change the enum.
    enum State {
        Invalid = 0,
        CString = 0x1000,
        CNumber,
        CBool,
        CNull,
        CUndefined,
        JSValue = 0x2000, // V8 values are equal or higher then this value.
        // JSPrimitive,
        // JSObject
    } m_state;

    union CValue {
        bool m_bool;
        qsreal m_number;
        QString* m_string;

        CValue() : m_number(0) {}
        CValue(bool value) : m_bool(value) {}
        CValue(int number) : m_number(number) {}
        CValue(uint number) : m_number(number) {}
        CValue(qsreal number) : m_number(number) {}
        CValue(QString* string) : m_string(string) {}
    } u;

public: // FIXME it shouldn't be public it is an implementation detail.
    // v8::Persistent is not a POD, so can't be part of the union.
    v8::Persistent<v8::Value> m_value;
private:
    Q_DISABLE_COPY(QScriptValuePrivate)
    inline bool isJSBased() const;
    inline bool isNumberBased() const;
    inline bool isStringBased() const;
    inline bool prepareArgumentsForCall(v8::Handle<v8::Value> argv[], const QScriptValueList& arguments) const;
};

QT_BEGIN_INCLUDE_NAMESPACE
#include "qscriptengine_p.h"
#include "qscriptqobject_p.h"
QT_END_INCLUDE_NAMESPACE

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
    : m_state(Invalid)
{
}

QScriptValuePrivate::QScriptValuePrivate(bool value)
    : m_state(CBool), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(int value)
    : m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(uint value)
    : m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(qsreal value)
    : m_state(CNumber), u(value)
{
}

QScriptValuePrivate::QScriptValuePrivate(const QString& value)
    : m_state(CString), u(new QString(value))
{
}

QScriptValuePrivate::QScriptValuePrivate(QScriptValue::SpecialValue value)
    : m_state(value == QScriptValue::NullValue ? CNull : CUndefined)
{
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, bool value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, int value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, uint value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, qsreal value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, const QString& value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate* engine, QScriptValue::SpecialValue value)
    : m_engine(engine), m_state(JSValue)
{
    Q_ASSERT(engine);
    v8::HandleScope handleScope;
    m_value = v8::Persistent<v8::Value>::New(m_engine->makeJSValue(value));
}

QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate *engine, v8::Handle<v8::Value> value)
    : m_engine(engine), m_state(JSValue), m_value(v8::Persistent<v8::Value>::New(value))
{
    Q_ASSERT(engine);
    // It shouldn't happen, v8 shows errors by returning an empty handler. This is important debug
    // information and it can't be simply ignored.
    Q_ASSERT(!value.IsEmpty());
}

QScriptValuePrivate::~QScriptValuePrivate()
{
    if (isJSBased()) {
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
        return new QScriptValuePrivate();
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
            return new QScriptValuePrivate();
        return new QScriptValuePrivate(engine, m_value->ToObject());
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Not all states are included in this switch");
        return new QScriptValuePrivate();
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
    if (!isObject())
        return 0;
    return toQtObject(m_engine.data(), m_value->ToObject());
}

const QMetaObject *QScriptValuePrivate::toQMetaObject() const
{
    if (!isQMetaObject())
        return 0;
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> object = m_value->ToObject();
    Q_ASSERT(object->InternalFieldCount() == 1);
    QtMetaObjectData *data = static_cast<QtMetaObjectData *>(object->GetPointerFromInternalField(0));
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
    return isFunction() || (isObject() && v8::Object::Cast(*m_value)->IsCallable());
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
    if (!isJSBased() || !m_value->IsObject())
        return false;
    return toQtObject(m_engine.data(), m_value->ToObject());
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
            Q_ASSERT_X(false, "equals()", "Not all states are included in the previous switch statement.");
        }
    }

    v8::HandleScope handleScope;
    if (isJSBased() && !other->isJSBased()) {
        if (!other->assignEngine(engine())) {
            qWarning("equals(): Cannot compare to a value created in a different engine");
            return false;
        }
    } else if (!isJSBased() && other->isJSBased()) {
        if (!assignEngine(other->engine())) {
            qWarning("equals(): Cannot compare to a value created in a different engine");
            return false;
        }
    }

    Q_ASSERT(this->engine() && other->engine());
    return m_value->Equals(other->m_value);
    return false;
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
            qWarning("strictlyEquals(): Cannot compare to a value created in a different engine");
            return false;
        }
        return m_value->StrictEquals(other->m_value);
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
        Q_ASSERT(eng);
        QScriptSharedDataPointer<QScriptValuePrivate> cmp(eng->evaluate(QString::fromLatin1("(function(a,b){return a<b})")));
        Q_ASSERT(cmp->isFunction());
        v8::Handle<v8::Value> args[2];
        cmp->prepareArgumentsForCall(args, QScriptValueList() << QScriptValuePrivate::get(this) << QScriptValuePrivate::get(other));
        QScriptSharedDataPointer<QScriptValuePrivate> result(cmp->call(0, 2, args));
        return result->toBool();
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
    if (isJSBased() && m_value->IsObject()) {
        v8::HandleScope handleScope;
        return new QScriptValuePrivate(engine(), v8::Handle<v8::Object>::Cast(m_value)->GetPrototype());
    }
    return new QScriptValuePrivate();
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

inline void QScriptValuePrivate::setProperty(const QScriptStringPrivate *name, QScriptValuePrivate* value, v8::PropertyAttribute attribs)
{
    if (name->isValid())
        setProperty(name->asV8Value(), value, attribs);
}

inline void QScriptValuePrivate::setProperty(const QString& name, QScriptValuePrivate* value, v8::PropertyAttribute attribs)
{
    v8::HandleScope handleScope;
    setProperty(QScriptConverter::toString(name), value, attribs);
}

inline void QScriptValuePrivate::setProperty(v8::Handle<v8::String> name, QScriptValuePrivate* value, v8::PropertyAttribute attribs)
{
    if (!isObject())
        return;

    if (!value->isJSBased())
        value->assignEngine(engine());

    if (!value->isValid()) {
        // Remove the property.
        v8::Object::Cast(*m_value)->Delete(name);
        return;
    }

    if (engine() != value->engine()) {
        QStringList msg;
        msg << QString::fromLatin1("QScriptValue::setProperty(")
            << QScriptConverter::toString(name)
            << QString::fromLatin1(") failed: cannot set value created in a different engine");
        qWarning(msg.join(QString()).toLatin1().constData());
        return;
    }

    v8::Object::Cast(*m_value)->Set(name, value->m_value, attribs);
}

inline void QScriptValuePrivate::setProperty(quint32 index, QScriptValuePrivate* value, v8::PropertyAttribute attribs)
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
        v8::Object::Cast(*m_value)->Delete(index);
        return;
    }

    if (engine() != value->engine()) {
        qWarning("QScriptValue::setProperty() failed: cannot set value created in a different engine");
        return;
    }

    v8::HandleScope handleScope;
    v8::Object::Cast(*m_value)->Set(index, value->m_value);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(const QString& name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject() || !name.length())
        return new QScriptValuePrivate();

    v8::HandleScope handleScope;
    return property(QScriptConverter::toString(name), mode);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::property(QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const
{
    if (!isObject() || !name->isValid())
        return new QScriptValuePrivate();

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
        return new QScriptValuePrivate();
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
    // FIXME: Result may be empty if a property accessor throw an exception, but for some reasons
    // tryCatch.HasCaught() returns false.
    if (result.IsEmpty() || (result->IsUndefined() && !self->Has(name))) {
        // In QtScript we make a distinction between a property that exists and has value undefined,
        // and a property that doesn't exist; in the latter case, we should return an invalid value.
        return new QScriptValuePrivate();
    }
    if ((mode == QScriptValue::ResolveLocal) && engine()->getOwnProperty(self, name).IsEmpty())
        return new QScriptValuePrivate();

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
        return new QScriptValuePrivate();
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> self(v8::Object::Cast(*m_value));
    v8::Handle<v8::Value> value = self->GetHiddenValue(engine()->qtDataId());
    if (value.IsEmpty())
        return new QScriptValuePrivate();
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

inline int QScriptValuePrivate::convertArguments(QVarLengthArray<v8::Handle<v8::Value>, 8> *argv, const QScriptValue& arguments)
{
    // Convert all arguments and bind to the engine.
    QScriptValuePrivate *args = QScriptValuePrivate::get(arguments);

    if (!args->isJSBased() && !args->assignEngine(engine()))
        return -1;

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
        return new QScriptValuePrivate();

    v8::HandleScope handleScope;

    // Convert all arguments and bind to the engine.
    int argc = args.size();
    QVarLengthArray<v8::Handle<v8::Value>, 8> argv(argc);
    if (!prepareArgumentsForCall(argv.data(), args)) {
        qWarning("QScriptValue::call() failed: cannot call function with argument created in a different engine");
        return new QScriptValuePrivate();
    }

    return call(thisObject, argc, argv.data());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::call(QScriptValuePrivate* thisObject, const QScriptValue& arguments)
{
    if (!isCallable())
        return new QScriptValuePrivate();

    v8::HandleScope handleScope;

    QVarLengthArray<v8::Handle<v8::Value>, 8> argv;
    int argc = convertArguments(&argv, arguments);
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
            return new QScriptValuePrivate();
        }

        recv = v8::Handle<v8::Object>(v8::Object::Cast(*thisObject->m_value));
    }

    if (argc < 0) {
        v8::Local<v8::Value> exeption = v8::Exception::TypeError(v8::String::New("Arguments must be an array"));
        e->setException(exeption);
        return new QScriptValuePrivate(e, exeption);
    }

    v8::TryCatch tryCatch;

    v8::Handle<v8::Value> result = v8::Object::Cast(*m_value)->Call(recv, argc, argv);

    if (result.IsEmpty()) {
        result = tryCatch.Exception();
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

    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = v8::Handle<v8::Function>::Cast(m_value)->NewInstance(argc, argv);

    if (result.IsEmpty()) {
        result = tryCatch.Exception();
        e->setException(result, tryCatch.Message());
    }

    return new QScriptValuePrivate(e, result);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::construct(const QScriptValueList& args)
{
    if (isQMetaObject()) {
        QtMetaObjectData *data = static_cast<QtMetaObjectData *>(m_value->ToObject()->GetPointerFromInternalField(0));
        Q_ASSERT(data);
        return QScriptValuePrivate::get(data->constructor())->construct(args);
    }
    if (!isFunction())
        return new QScriptValuePrivate();

    v8::HandleScope handleScope;

    // Convert all arguments and bind to the engine.
    int argc = args.size();
    QVarLengthArray<v8::Handle<v8::Value>, 8> argv(argc);
    if (!prepareArgumentsForCall(argv.data(), args)) {
        qWarning("QScriptValue::construct() failed: cannot call function with values created in a different engine");
        return new QScriptValuePrivate();
    }

    return construct(argc, argv.data());
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptValuePrivate::construct(const QScriptValue& arguments)
{
    if (!isFunction())
        return new QScriptValuePrivate();

    v8::HandleScope handleScope;

    QVarLengthArray<v8::Handle<v8::Value>, 8> argv;
    int argc = convertArguments(&argv, arguments);

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
    return true;
}

/*!
  \internal
  reinitialize this value to an invalid
*/
void QScriptValuePrivate::reinitialize()
{
    if (isJSBased()) {
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
  reinitialize this value to an JSValue or invalid (if the given handle point to invalid)
*/
void QScriptValuePrivate::reinitialize(QScriptEnginePrivate* engine, v8::Handle<v8::Value> value)
{
    if (isJSBased()) {
        m_value.Dispose();
    } else if (isStringBased()) {
        delete u.m_string;
    }
    m_engine = engine;
    m_state = JSValue;
    m_value = v8::Persistent<v8::Value>::New(value);
}

QScriptEnginePrivate* QScriptValuePrivate::engine() const
{
    // As long as m_engine is an autoinitializated pointer we can safely return it without
    // checking current state.
    return const_cast<QScriptEnginePrivate*>(m_engine.constData());
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
