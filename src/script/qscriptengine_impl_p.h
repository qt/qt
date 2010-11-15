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

#ifndef QSCRIPTENGINE_IMPL_P_H
#define QSCRIPTENGINE_IMPL_P_H

#include "qscriptengine_p.h"
#include "qscriptvalue_impl_p.h"

QT_BEGIN_NAMESPACE

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(bool value)
{
    return value ? v8::True() : v8::False();
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(int value)
{
    return v8::Integer::New(value);
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(uint value)
{
    return v8::Integer::NewFromUnsigned(value);
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(qsreal value)
{
    return v8::Number::New(value);
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(QScriptValue::SpecialValue value) {
    if (value == QScriptValue::NullValue)
        return v8::Null();
    return v8::Undefined();
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(const QString& value)
{
    return QScriptConverter::toString(value);
}

inline QScriptEnginePrivate::operator v8::Handle<v8::Context>()
{
    return m_v8Context;
}

/*!
  \internal
  returns all property names of an object (same as ECMA getOwnPropertyNames)
*/
inline v8::Local<v8::Array> QScriptEnginePrivate::getOwnPropertyNames(v8::Handle<v8::Object> object) const
{
    return m_originalGlobalObject.getOwnPropertyNames(object);
}

/*!
  \internal
  \note property can be index (v8::Integer) or a property (v8::String) name, according to ECMA script
  property would be converted to a string.
*/
inline QScriptValue::PropertyFlags QScriptEnginePrivate::getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode)
{
    QScriptValue::PropertyFlags flags = m_originalGlobalObject.getPropertyFlags(object, property, mode);
    if (!flags && hasInstance(scriptClassTemplate, object)) {
        flags = getPropertyFlagsFromScriptClassInstance(object, property, mode);
    }
    return flags;
}
inline v8::Local<v8::Value> QScriptEnginePrivate::getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> propertyName) const
{
    v8::Local<v8::Value> property = m_originalGlobalObject.getOwnProperty(object, propertyName);
    if (property.IsEmpty()) {
        // Check if the object is not an instance of a script class.
        if (hasInstance(scriptClassTemplate, object)) {
            property = getOwnPropertyFromScriptClassInstance(object, propertyName);
        }
    }
    return property;
}

inline v8::Local<v8::Value> QScriptEnginePrivate::getOwnProperty(v8::Handle<v8::Object> object, uint32_t index) const
{
    return getOwnProperty(object, v8::Integer::New(index));
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::evaluate(const QString& program, const QString& fileName, int lineNumber)
{
    Q_UNUSED(lineNumber);
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> script = v8::Script::CompileEval(QScriptConverter::toString(program), QScriptConverter::toString(fileName));
    if (script.IsEmpty()) {
        // TODO: Why don't we get the exception, as with Script::Compile()?
        // Q_ASSERT(tryCatch.HasCaught());
        v8::Handle<v8::Value> error = v8::Exception::SyntaxError(v8::String::New(""));
        setException(error);
        return new QScriptValuePrivate(this, error);
    }
    return evaluate(script, tryCatch);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::evaluate(QScriptProgramPrivate* program)
{
    // FIXME: We cannot use v8::Script::Compile() because it compiles
    // the script in the current (global) context, and uses _the
    // original_ context when the script is evaluated later; the
    // semantics of evaluate(QScriptProgram) is that it should
    // evaluate the program in the current context, even if that is
    // different from the one where the program was evaluated the
    // first time.
    //
    // We cannot use v8::Script::New() because it compiles the script
    // as if it's evaluated in a (not necessarily the current) global
    // context. We need something like v8::Script::NewEval().
    //
    // For now, fall back to evaluating the program from source every
    // time to enforce the right semantics.

//    v8::TryCatch tryCatch;
//    v8::Handle<v8::Script> script = program->compiled(this);
//    return evaluate(script, tryCatch);
    if (program->isNull())
        return new QScriptValuePrivate();
    return evaluate(program->m_program, program->m_fileName, program->m_line);
}

inline bool QScriptEnginePrivate::isEvaluating() const
{
    return m_isEvaluating;
}

void QScriptEnginePrivate::collectGarbage()
{
    v8::V8::LowMemoryNotification();
}

void QScriptEnginePrivate::reportAdditionalMemoryCost(int cost)
{
    /*// The check is needed only for compatibility.
    if (cost > 0)
        v8::V8::AdjustAmountOfExternalAllocatedMemory(cost);*/
    // in V8, AdjustAmountOfExternalAllocatedMemory need to be balanced
    // by a negative number when the memory is released, else
    // the garbage collector will think it still has lot of memory and
    // will be run too often.
    if (cost > 0)
        Q_UNIMPLEMENTED();
}

inline void QScriptEnginePrivate::setException(v8::Handle<v8::Value> value, v8::Handle<v8::Message> msg)
{
    m_exception.set(value, msg);
}

inline v8::Handle<v8::Value> QScriptEnginePrivate::throwException(v8::Handle<v8::Value> value)
{
    setException(value);
    v8::ThrowException(value);
    return value;
}

inline void QScriptEnginePrivate::clearExceptions()
{
    m_exception.clear();
}

inline bool QScriptEnginePrivate::hasUncaughtException() const
{
    return m_exception;
}

inline int QScriptEnginePrivate::uncaughtExceptionLineNumber() const
{
    return m_exception.lineNumber();
}

inline QStringList QScriptEnginePrivate::uncaughtExceptionBacktrace() const
{
    return m_exception.backtrace();
}

inline void QScriptEnginePrivate::enterIsolate() const
{
    m_isolate->Enter();
}

inline void QScriptEnginePrivate::exitIsolate() const
{
    m_isolate->Exit();
}

inline bool QScriptEnginePrivate::isQtVariant(v8::Handle<v8::Value> value) const
{
    return m_variantTemplate->HasInstance(value);
}

inline bool QScriptEnginePrivate::isQtMetaObject(v8::Handle<v8::Value> value) const
{
    return m_metaObjectTemplate->HasInstance(value);
}

/* set the current QScriptContext, and return the old value */
inline QScriptContextPrivate* QScriptEnginePrivate::setCurrentQSContext(QScriptContextPrivate *ctx)
{
    qSwap(ctx, m_currentQsContext);
    return ctx;
}

inline v8::Handle<v8::Object> QScriptEnginePrivate::globalObject() const
{
    return m_v8Context->Global();
}

inline QScriptEnginePrivate::Exception::Exception() {}

inline QScriptEnginePrivate::Exception::~Exception() { clear(); }

inline void QScriptEnginePrivate::Exception::set(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message)
{
    clear();
    m_value = v8::Persistent<v8::Value>::New(value);
    m_message = v8::Persistent<v8::Message>::New(message);
}

inline void QScriptEnginePrivate::Exception::clear()
{
    if (!m_value.IsEmpty()) {
        m_value.Dispose();
        m_value.Clear();
    }
    if (!m_message.IsEmpty()) {
        m_message.Dispose();
        m_message.Clear();
    }
}

inline QScriptEnginePrivate::Exception::operator bool() const
{
    return !m_value.IsEmpty();
}

inline QScriptEnginePrivate::Exception::operator v8::Handle<v8::Value>() const
{
    Q_ASSERT(*this);
    return m_value;
}

inline int QScriptEnginePrivate::Exception::lineNumber() const
{
    if (m_message.IsEmpty())
        return -1;
    return m_message->GetLineNumber();
}

inline QStringList QScriptEnginePrivate::Exception::backtrace() const
{
    if (m_message.IsEmpty())
        return QStringList();

    QStringList backtrace;
    v8::Handle<v8::StackTrace> trace = m_message->GetStackTrace();
    if (trace.IsEmpty())
        // FIXME it should not happen (SetCaptureStackTraceForUncaughtExceptions is called).
        return QStringList();

    for (int i = 0; i < trace->GetFrameCount(); ++i) {
        v8::Local<v8::StackFrame> frame = trace->GetFrame(i);
        backtrace.append(QScriptConverter::toString(frame->GetFunctionName()));
        backtrace.append(QScriptConverter::toString(frame->GetFunctionName()));
        backtrace.append(QString::fromAscii("()@"));
        backtrace.append(QScriptConverter::toString(frame->GetScriptName()));
        backtrace.append(QString::fromAscii(":"));
        backtrace.append(QString::number(frame->GetLineNumber()));
    }
    return backtrace;
}

inline QScriptEnginePrivate::TypeInfos::~TypeInfos()
{
    clear();
}

inline void QScriptEnginePrivate::TypeInfos::registerCustomType(int type, QScriptEngine::MarshalFunction mf, QScriptEngine::DemarshalFunction df, v8::Handle<v8::Object> prototype)
{
    TypeInfo &info = m_infos[type];
    static_cast<v8::Persistent<v8::Object> >(info.prototype).Dispose();

    Q_ASSERT(prototype.IsEmpty() || prototype->IsObject());
    info.marshal = mf;
    info.demarshal = df;
    info.prototype = v8::Persistent<v8::Object>::New(prototype);

}

inline QScriptEnginePrivate::TypeInfos::TypeInfo QScriptEnginePrivate::TypeInfos::value(int type) const
{
    return m_infos.value(type);
}

inline void QScriptEnginePrivate::TypeInfos::clear()
{
    QList<TypeInfo> values = m_infos.values();
    QList<TypeInfo>::const_iterator i = values.constBegin();
    for (; i != values.constEnd(); ++i)
        static_cast<v8::Persistent<v8::Object> >(i->prototype).Dispose();
    m_infos.clear();
}

inline bool QScriptEnginePrivate::hasInstance(v8::Handle<v8::FunctionTemplate> fun, v8::Handle<v8::Value> value) const
{
    Q_ASSERT(!value.IsEmpty());
    return !fun.IsEmpty() && fun->HasInstance(value);
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newQObject(
        QObject *object, QScriptEngine::ValueOwnership own,
        const QScriptEngine::QObjectWrapOptions &opt)
{
    return new QScriptValuePrivate(this, makeQtObject(object, own, opt));
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newQObject(QScriptValuePrivate *scriptObject,
                                                                         QObject *qtobject,
                                                                         QScriptEngine::ValueOwnership ownership,
                                                                         const QScriptEngine::QObjectWrapOptions &options)
{
    if (!scriptObject->isObject())
        return newQObject(qtobject, ownership, options);

    v8::Handle<v8::Object> jsobject = *scriptObject;
    if (scriptObject->isQObject()) {
        // scriptObject is a wrapper of an qt object.
        Q_ASSERT(jsobject->InternalFieldCount() == 1);
        QtInstanceData *data = reinterpret_cast<QtInstanceData*>(jsobject->GetPointerFromInternalField(0));
        Q_ASSERT(data);
        delete data;
        data = new QtInstanceData(this, qtobject, ownership, options);
        jsobject->SetPointerInInternalField(0, data);
        return scriptObject;
    }

    // FIXME it create a new instance instead of reusing this one. It doesn't replace existing references in JS.
    // Similar problem is in QSV::setScriptClass.
    // Q_UNIMPLEMENTED();
    QScriptPassPointer<QScriptValuePrivate> obj(newQObject(qtobject, ownership, options));
    QScriptPassPointer<QScriptValuePrivate> proto(scriptObject->prototype());
    scriptObject->reinitialize(this, *obj.give());
    scriptObject->setPrototype(proto.give());

    return scriptObject;
}
QT_END_NAMESPACE

#endif
