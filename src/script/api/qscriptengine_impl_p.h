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

#ifndef QSCRIPTENGINE_IMPL_P_H
#define QSCRIPTENGINE_IMPL_P_H

#include "qscriptengine_p.h"
#include "qscriptengineagent_p.h"
#include "qscriptqobject_p.h"

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
    v8::TryCatch tryCatch;
    v8::ScriptOrigin scriptOrigin(QScriptConverter::toString(fileName), v8::Integer::New(lineNumber - 1));
    v8::Handle<v8::Script> script;
    // This is a risk management problem. CompileEval function is our extension to V8, so it introduces
    // constant risk that it is or will be broken. It is safer to use the standard function instead.
    // Of course it doesn't mean that we shouldn't fix problems in CompileEval, we want to reduce
    // potential bug impact.
    // FIXME: CompileEval introduces instability in V8 debugger which affects QSEAgent. This check
    // may be removed for testing QSEA to test other code paths.
    // http://bugreports.qt.nokia.com/browse/QTBUG-17878
    if (m_baseQsContext.data() == m_currentQsContext // no pushContext
            && m_currentQsContext->scopes.empty()    // no pushScope
            && m_originalGlobalObject.strictlyEquals(globalObject()) // no setGlobalObject
            ) {
        script = v8::Script::Compile(QScriptConverter::toString(program), &scriptOrigin);
    } else {
        script = v8::Script::CompileEval(QScriptConverter::toString(program), &scriptOrigin);
    }
    if (script.IsEmpty()) {
        // TODO: Why don't we get the exception, as with Script::Compile()?
        // Q_ASSERT(tryCatch.HasCaught());
        v8::Handle<v8::Value> error = v8::Exception::SyntaxError(v8::String::New(""));
        setException(error);
        return new QScriptValuePrivate(this, error);
    }
    if (m_currentAgent)
        m_currentAgent->scriptLoad(script, program, fileName, lineNumber);

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
        return InvalidValue();
    return evaluate(program->m_program, program->m_fileName, program->m_line);
}

inline bool QScriptEnginePrivate::isEvaluating() const
{
    return m_state == Evaluating;
}

inline bool QScriptEnginePrivate::isDestroyed() const
{
    return m_state == Destroyed;
}

void QScriptEnginePrivate::collectGarbage()
{
    v8::V8::LowMemoryNotification();
    while (!v8::V8::IdleNotification()) {}
}

void QScriptEnginePrivate::reportAdditionalMemoryCost(int cost)
{
    // In V8, AdjustAmountOfExternalAllocatedMemory need to be balanced
    // by a negative number when the memory is released, else
    // the garbage collector will think it still has lot of memory and
    // will be run too often.

    // The check is needed only for compatibility.
    if (cost > 0) {
        int currentCost = m_reportedAddtionalMemoryCost;
        if (currentCost == - 1) {
            // Fist time call, add a gc callback.
            // AddGCEpilogueCallback works per Isolate, it means that we have to install
            // one callback per each engine instance.
            v8::V8::AddGCEpilogueCallback(GCEpilogueCallback);
        }
        v8::V8::AdjustAmountOfExternalAllocatedMemory(cost);
        m_reportedAddtionalMemoryCost += cost;
    }
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

/*!
  \internal
  Save the current exception on stack so it can be set again later.
  \sa QScriptEnginePrivate::restoreException
*/
inline void QScriptEnginePrivate::saveException()
{
    m_exception.push();
}

/*!
  \internal
  Load a saved exception from stack. Current exception, if exists will be dropped
  \sa QScriptEnginePrivate::saveException
*/
inline void QScriptEnginePrivate::restoreException()
{
    m_exception.pop();
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
    return hasInstance(m_variantTemplate, value);
}

inline bool QScriptEnginePrivate::isQtMetaObject(v8::Handle<v8::Value> value) const
{
    return hasInstance(m_metaObjectTemplate, value);
}

/* set the current QScriptContext, and return the old value */
inline QScriptContextPrivate* QScriptEnginePrivate::setCurrentQSContext(QScriptContextPrivate *ctx)
{
    qSwap(ctx, m_currentQsContext);
    return ctx;
}

inline void QScriptEnginePrivate::updateGlobalObjectCache()
{
    m_currentGlobalObject.Dispose();
    //in V8, the Global object is a proxy to the prototype, which is the real global object.
    // See http://code.google.com/p/v8/issues/detail?id=1078
    m_currentGlobalObject = v8::Persistent<v8::Object>::New(v8::Handle<v8::Object>::Cast(m_v8Context->Global()->GetPrototype()));
}

inline v8::Handle<v8::Object> QScriptEnginePrivate::globalObject() const
{
    Q_ASSERT_X(!m_currentGlobalObject.IsEmpty(), Q_FUNC_INFO, "Global Object handle hasn't been initialized");
    return m_currentGlobalObject;
}

inline QScriptEnginePrivate::Exception::Exception() {}

inline QScriptEnginePrivate::Exception::~Exception()
{
    Q_ASSERT_X(m_stack.isEmpty(), Q_FUNC_INFO, "Some saved exceptions left. Asymetric pop/push found.");
    clear();
}

inline void QScriptEnginePrivate::Exception::set(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message)
{
    Q_ASSERT_X(!value.IsEmpty(), Q_FUNC_INFO, "Throwing an empty value handle is highly suspected");
    clear();
    m_value = v8::Persistent<v8::Value>::New(value);
    m_message = v8::Persistent<v8::Message>::New(message);
}

inline void QScriptEnginePrivate::Exception::clear()
{
    m_value.Dispose();
    m_value.Clear();
    m_message.Dispose();
    m_message.Clear();
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

inline void QScriptEnginePrivate::Exception::push()
{
    m_stack.push(qMakePair(m_value, m_message));
    m_value.Clear();
    m_message.Clear();
}

inline void QScriptEnginePrivate::Exception::pop()
{
    Q_ASSERT_X(!m_stack.empty(), Q_FUNC_INFO, "Attempt to load unsaved exception found");
    ValueMessagePair pair = m_stack.pop();
    clear();
    m_value = pair.first;
    m_message = pair.second;
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

inline void QScriptEnginePrivate::registerAdditionalResources(QtDataBase *data)
{
    m_additionalResources.insert(data);
}

inline void QScriptEnginePrivate::unregisterAdditionalResources(QtDataBase *data)
{
    m_additionalResources.remove(data);
}

class QtScriptBagCleaner
{
public:
    template<class T>
    void operator () (T* value) const
    {
        value->reinitialize();
    }

    void operator() (QScriptEngineAgentPrivate *agent) const
    {
        agent->kill();
    }

    void operator () (QtDataBase *data) const
    {
        delete data;
    }

    void operator () (QScriptEngineAgentPrivate::UnloadData *data) const
    {
        delete data;
    }
};

inline void QScriptEnginePrivate::deallocateAdditionalResources()
{
    QtScriptBagCleaner deleter;
    m_additionalResources.forEach(deleter);
    m_additionalResources.clear();
}

inline void QScriptEnginePrivate::registerValue(QScriptValuePrivate *data)
{
    m_values.insert(data);
}

inline void QScriptEnginePrivate::unregisterValue(QScriptValuePrivate *data)
{
    m_values.remove(data);
}

inline void QScriptEnginePrivate::registerString(QScriptStringPrivate *data)
{
    m_strings.insert(data);
}

inline void QScriptEnginePrivate::unregisterString(QScriptStringPrivate *data)
{
    m_strings.remove(data);
}

inline void QScriptEnginePrivate::registerScriptable(QScriptablePrivate *data)
{
    m_scriptable.insert(data);
}

inline void QScriptEnginePrivate::unregisterScriptable(QScriptablePrivate *data)
{
    m_scriptable.remove(data);
}

inline void QScriptEnginePrivate::registerAgent(QScriptEngineAgentPrivate *data)
{
    m_agents.insert(data);
}

inline void QScriptEnginePrivate::unregisterAgent(QScriptEngineAgentPrivate *data)
{
    m_agents.remove(data);
    if (m_currentAgent == data)
        m_currentAgent = 0;
}

inline void QScriptEnginePrivate::registerScript(QScriptEngineAgentPrivate::UnloadData *data)
{
    m_scripts.insert(data);
}

inline void QScriptEnginePrivate::unregisterScript(QScriptEngineAgentPrivate::UnloadData *data)
{
    m_scripts.remove(data);
}

inline void QScriptEnginePrivate::invalidateAllValues()
{
    QtScriptBagCleaner invalidator;
    m_values.forEach(invalidator);
    m_values.clear();
}

inline void QScriptEnginePrivate::invalidateAllString()
{
    QtScriptBagCleaner invalidator;
    m_strings.forEach(invalidator);
    m_strings.clear();
}

inline void QScriptEnginePrivate::invalidateAllScriptable()
{
    QtScriptBagCleaner invalidator;
    m_scriptable.forEach(invalidator);
    m_scriptable.clear();
}

inline void QScriptEnginePrivate::invalidateAllAgents()
{
    QtScriptBagCleaner killer;
    while (!m_agents.isEmpty()) {
        // action of deleting agents potentially can add new agents.
        m_agents.forEach(killer);
    }
}

inline void QScriptEnginePrivate::invalidateAllScripts()
{
    QtScriptBagCleaner deleter;
    m_scripts.forEach(deleter);
}

inline bool QScriptEnginePrivate::hasDemarshalFunction(int metaTypeId) const
{
    return m_typeInfos.value(metaTypeId).demarshal;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newQObject(QScriptValuePrivate *scriptObject,
                                                                         QObject *qtobject,
                                                                         QScriptEngine::ValueOwnership ownership,
                                                                         const QScriptEngine::QObjectWrapOptions &options)
{
    if (!scriptObject->isObject())
        return new QScriptValuePrivate(this, newQObject(qtobject, ownership, options));

    if (scriptObject->isQObject()) {
        v8::Handle<v8::Object> jsobject = *scriptObject;
        // scriptObject is a wrapper of an qt object.
        QScriptQObjectData::set(jsobject, new QScriptQObjectData(this, qtobject, ownership, options));
        return scriptObject;
    }

    // FIXME it create a new instance instead of reusing this one. It doesn't replace existing references in JS.
    // Similar problem is in QSV::setScriptClass.
    // Q_UNIMPLEMENTED();
    QScriptSharedDataPointer<QScriptValuePrivate> obj(new QScriptValuePrivate(this, newQObject(qtobject, ownership, options)));
    QScriptSharedDataPointer<QScriptValuePrivate> proto(scriptObject->prototype());
    scriptObject->reinitialize(this, *obj);
    scriptObject->setPrototype(proto.data());

    return scriptObject;
}

inline v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::metaObjectTemplate()
{
    if (m_metaObjectTemplate.IsEmpty())
        m_metaObjectTemplate = v8::Persistent<v8::FunctionTemplate>::New(createMetaObjectTemplate());
    return m_metaObjectTemplate;
}

inline v8::Handle<v8::FunctionTemplate> QScriptEnginePrivate::variantTemplate()
{
    if (m_variantTemplate.IsEmpty())
        m_variantTemplate = v8::Persistent<v8::FunctionTemplate>::New(createVariantTemplate());
    return m_variantTemplate;
}

inline QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::newVariant(QScriptValuePrivate* object, const QVariant &value)
{

    if (!object->isObject())
        return new QScriptValuePrivate(this, newVariant(value));

    if (object->isVariant()) {
        // object is a wrapper of a qvariant.
        QtVariantData::set(*object, new QtVariantData(this, value));
        return object;
    }
    // FIXME it create a new instance instead of reusing this one. It doesn't replace existing references in JS.
    // Similar problem is in QSV::setScriptClass and QSE::newQObject.
    // Q_UNIMPLEMENTED();
    QScriptSharedDataPointer<QScriptValuePrivate> obj(new QScriptValuePrivate(this, newVariant(value)));
    QScriptSharedDataPointer<QScriptValuePrivate> proto(object->prototype());
    object->reinitialize(this, *obj);
    object->setPrototype(proto.data());
    return object;
}

inline void QScriptEnginePrivate::setAgent(QScriptEngineAgentPrivate *agent)
{
    if (agent && (agent->engine() != this)) {
        qWarning("QScriptEngine::setAgent(): cannot set agent belonging to different engine");
        return;
    }
    m_currentAgent = agent;
}

inline QScriptEngineAgentPrivate *QScriptEnginePrivate::agent() const
{
    return m_currentAgent;
}

void QScriptEnginePrivate::abortEvaluation(v8::Handle< v8::Value > result)
{
    if (!isEvaluating())
        return;
    m_shouldAbort = true;
    m_abortResult.Dispose();
    m_abortResult = v8::Persistent<v8::Value>::New(result);
    v8::V8::TerminateExecution();
}

QT_END_NAMESPACE

#endif
