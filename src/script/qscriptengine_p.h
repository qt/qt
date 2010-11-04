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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <QtCore/qset.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>

#include "qscriptengine.h"
#include "qscriptconverter_p.h"
#include "qscriptshareddata_p.h"

#include "qscriptoriginalglobalobject_p.h"
#include "qscriptvalue.h"
#include "qscriptprogram_p.h"
#include <v8.h>

Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QObjectList)
Q_DECLARE_METATYPE(QList<int>)


QT_BEGIN_NAMESPACE

class QDateTime;
class QScriptEngine;
class QScriptContextPrivate;
class QScriptClassPrivate;

class QScriptEnginePrivate
    : public QScriptSharedData
{
    class Exception
    {
        v8::Persistent<v8::Value> m_value;
        v8::Persistent<v8::Message> m_message;
        Q_DISABLE_COPY(Exception)
    public:
        inline Exception();
        inline ~Exception();
        inline void set(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message);
        inline void clear();
        inline operator bool() const;
        inline operator v8::Handle<v8::Value>() const;
        inline int lineNumber() const;
        inline QStringList backtrace() const;
    };

    Q_DECLARE_PUBLIC(QScriptEngine)
public:
    static QScriptEnginePrivate* get(QScriptEngine* q) { Q_ASSERT(q); return q->d_func(); }
    static QScriptEngine* get(QScriptEnginePrivate* d) { Q_ASSERT(d); return d->q_func(); }

    QScriptEnginePrivate(QScriptEngine*, QScriptEngine::ContextOwnership ownership = QScriptEngine::CreateNewContext);
    ~QScriptEnginePrivate();

    inline QScriptPassPointer<QScriptValuePrivate> evaluate(const QString &program, const QString &fileName = QString(), int lineNumber = 1);
    inline QScriptPassPointer<QScriptValuePrivate> evaluate(QScriptProgramPrivate* program);
    QScriptPassPointer<QScriptValuePrivate> evaluate(v8::Handle<v8::Script> script, v8::TryCatch& tryCatch);
    inline bool isEvaluating() const;
    inline void collectGarbage();
    inline void reportAdditionalMemoryCost(int cost);
    v8::Handle<v8::Object> globalObject() const;
    void setGlobalObject(QScriptValuePrivate* newGlobalObjectValue);

    QScriptPassPointer<QScriptValuePrivate> newArray(uint length);
    QScriptPassPointer<QScriptValuePrivate> newObject();
    QScriptPassPointer<QScriptValuePrivate> newObject(QScriptClassPrivate* scriptclass, QScriptValuePrivate* data);
    QScriptPassPointer<QScriptValuePrivate> newFunction(QScriptEngine::FunctionSignature fun, QScriptValuePrivate *prototype, int length);
    QScriptPassPointer<QScriptValuePrivate> newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg);

    v8::Handle<v8::Object> newQObject(
        QObject *object, QScriptEngine::ValueOwnership own = QScriptEngine::QtOwnership,
        const QScriptEngine::QObjectWrapOptions &opt = 0);

    v8::Handle<v8::Object> newVariant(const QVariant &value);
    v8::Handle<v8::Object> newQMetaObject(const QMetaObject* mo, const QScriptValue &ctor);

    v8::Handle<v8::FunctionTemplate> createMetaObjectTemplate();
    v8::Handle<v8::FunctionTemplate> createVariantTemplate();

    v8::Handle<v8::FunctionTemplate> qtClassTemplate(const QMetaObject *);
    v8::Handle<v8::FunctionTemplate> qobjectTemplate();

    inline v8::Handle<v8::Value> makeJSValue(bool value);
    inline v8::Handle<v8::Value> makeJSValue(int value);
    inline v8::Handle<v8::Value> makeJSValue(uint value);
    inline v8::Handle<v8::Value> makeJSValue(qsreal value);
    inline v8::Handle<v8::Value> makeJSValue(QScriptValue::SpecialValue value);
    inline v8::Handle<v8::Value> makeJSValue(const QString& value);
    inline v8::Local<v8::Array> getOwnPropertyNames(v8::Handle<v8::Object> object) const;
    inline QScriptValue::PropertyFlags getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode);
    inline v8::Local<v8::Value> getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    inline v8::Local<v8::Value> getOwnProperty(v8::Handle<v8::Object> object, uint32_t index) const;

    QDateTime qtDateTimeFromJS(v8::Handle<v8::Date> jsDate);
    v8::Handle<v8::Value> qtDateTimeToJS(const QDateTime &dt);

#ifndef QT_NO_REGEXP
    QScriptPassPointer<QScriptValuePrivate> newRegExp(const QRegExp &regexp);
    QScriptPassPointer<QScriptValuePrivate> newRegExp(const QString &pattern, const QString &flags);
#endif

    v8::Handle<v8::Array> stringListToJS(const QStringList &lst);
    QStringList stringListFromJS(v8::Handle<v8::Array> jsArray);

    v8::Handle<v8::Array> variantListToJS(const QVariantList &lst);
    QVariantList variantListFromJS(v8::Handle<v8::Array> jsArray);

    v8::Handle<v8::Object> variantMapToJS(const QVariantMap &vmap);
    QVariantMap variantMapFromJS(v8::Handle<v8::Object> jsObject);

    v8::Handle<v8::Value> variantToJS(const QVariant &value);
    QVariant variantFromJS(v8::Handle<v8::Value> value);

    v8::Handle<v8::Value> metaTypeToJS(int type, const void *data);
    bool metaTypeFromJS(v8::Handle<v8::Value> value, int type, void *data);

    bool isQtObject(v8::Handle<v8::Value> value);
    QObject *qtObjectFromJS(v8::Handle<v8::Value> value);
    bool convertToNativeQObject(v8::Handle<v8::Value> value,
                                const QByteArray &targetType,
                                void **result);

    inline bool isQtVariant(v8::Handle<v8::Value> value) const;
    inline bool isQtMetaObject(v8::Handle<v8::Value> value) const;
    QVariant &variantValue(v8::Handle<v8::Value> value);

    void installTranslatorFunctions(QScriptValuePrivate* object);
    void installTranslatorFunctions(v8::Handle<v8::Value> object);

    QScriptValue scriptValueFromInternal(v8::Handle<v8::Value>);

    inline operator v8::Handle<v8::Context>();
    inline void clearExceptions();
    inline void setException(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message = v8::Handle<v8::Message>());
    inline v8::Handle<v8::Value> throwException(v8::Handle<v8::Value> value);
    inline bool hasUncaughtException() const;
    inline int uncaughtExceptionLineNumber() const;
    inline QStringList uncaughtExceptionBacktrace() const;
    QScriptPassPointer<QScriptValuePrivate> uncaughtException() const;

    v8::Handle<v8::String> qtDataId();

    inline void enterIsolate() const;
    inline void exitIsolate() const;

    void pushScope(QScriptValuePrivate* value);
    QScriptPassPointer<QScriptValuePrivate> popScope();

    void registerCustomType(int type, QScriptEngine::MarshalFunction mf, QScriptEngine::DemarshalFunction df, const QScriptValuePrivate *prototype);
    void setDefaultPrototype(int metaTypeId, const QScriptValuePrivate *prototype);
    QScriptPassPointer<QScriptValuePrivate> defaultPrototype(int metaTypeId);

    inline QScriptContextPrivate *setCurrentQSContext(QScriptContextPrivate *ctx);
    inline QScriptContextPrivate *currentContext() { return m_currentQsContext; }
    QScriptContextPrivate *pushContext();
    void popContext();
    v8::Handle<v8::Value> securityToken() { return m_v8Context->GetSecurityToken(); }
    void emitSignalHandlerException();

    v8::Persistent<v8::FunctionTemplate> declarativeClassTemplate;
    v8::Persistent<v8::FunctionTemplate> scriptClassTemplate;
    v8::Persistent<v8::FunctionTemplate> metaMethodTemplate;
    v8::Persistent<v8::FunctionTemplate> signalTemplate;

    class TypeInfos
    {
    public:
        struct TypeInfo
        {
            QScriptEngine::MarshalFunction marshal;
            QScriptEngine::DemarshalFunction demarshal;
            // This is a persistent and it should be deleted in ~TypeInfos
            v8::Handle<v8::Object> prototype;
        };

        inline TypeInfos() {};
        inline ~TypeInfos();
        inline void clear();
        inline TypeInfo value(int type) const;
        inline void registerCustomType(int type, QScriptEngine::MarshalFunction mf, QScriptEngine::DemarshalFunction df, v8::Handle<v8::Object> prototype = v8::Handle<v8::Object>());
    private:
        Q_DISABLE_COPY(TypeInfos)
        QHash<int, TypeInfo> m_infos;
    };
private:
    Q_DISABLE_COPY(QScriptEnginePrivate)
    QScriptEngine* q_ptr;
    v8::Isolate *m_isolate;
    v8::Persistent<v8::Context> m_v8Context;
    QVarLengthArray<v8::Persistent<v8::Context>, 8> m_v8Contexts;
    Exception m_exception;
    QScriptOriginalGlobalObject m_originalGlobalObject;
    v8::Persistent<v8::String> m_qtDataId;

    QHash<const QMetaObject *, v8::Persistent<v8::FunctionTemplate> > m_qtClassTemplates;
    v8::Persistent<v8::FunctionTemplate> m_variantTemplate;
    v8::Persistent<v8::FunctionTemplate> m_metaObjectTemplate;
    v8::Persistent<v8::ObjectTemplate> m_globalObjectTemplate;
    QScriptContextPrivate *m_currentQsContext;
    QScopedPointer<QScriptContextPrivate> m_baseQsContext;
    QSet<int> visitedConversionObjects;
    bool m_isEvaluating;
    TypeInfos m_typeInfos;

    QSet<QString> importedExtensions;
    QSet<QString> extensionsBeingImported;
};

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(bool value)
{
    return value ? v8::True() : v8::False();
}

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(int value)
{
    return v8::Integer::New(value);
}

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(uint value)
{
    return v8::Integer::NewFromUnsigned(value);
}

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(qsreal value)
{
    return v8::Number::New(value);
}

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(QScriptValue::SpecialValue value) {
    if (value == QScriptValue::NullValue)
        return v8::Null();
    return v8::Undefined();
}

v8::Handle<v8::Value> QScriptEnginePrivate::makeJSValue(const QString& value)
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
    return m_originalGlobalObject.getPropertyFlags(object, property, mode);
}

inline v8::Local<v8::Value> QScriptEnginePrivate::getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const
{
    return m_originalGlobalObject.getOwnProperty(object, property);
}

inline v8::Local<v8::Value> QScriptEnginePrivate::getOwnProperty(v8::Handle<v8::Object> object, uint32_t index) const
{
    return m_originalGlobalObject.getOwnProperty(object, v8::Integer::New(index));
}

QScriptPassPointer<QScriptValuePrivate> QScriptEnginePrivate::evaluate(const QString& program, const QString& fileName, int lineNumber)
{
    Q_UNUSED(lineNumber);
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> script = v8::Script::CompileEval(QScriptConverter::toString(program), QScriptConverter::toString(fileName));
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
QScriptContextPrivate* QScriptEnginePrivate::setCurrentQSContext(QScriptContextPrivate *ctx)
{
    qSwap(ctx, m_currentQsContext);
    return ctx;
}

inline v8::Handle<v8::Object> QScriptEnginePrivate::globalObject() const
{
    return m_v8Context->Global();
}

QScriptEnginePrivate::Exception::Exception() {}

QScriptEnginePrivate::Exception::~Exception() { clear(); }

void QScriptEnginePrivate::Exception::set(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message)
{
    clear();
    m_value = v8::Persistent<v8::Value>::New(value);
    m_message = v8::Persistent<v8::Message>::New(message);
}

void QScriptEnginePrivate::Exception::clear()
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

QScriptEnginePrivate::Exception::operator bool() const
{
    return !m_value.IsEmpty();
}

QScriptEnginePrivate::Exception::operator v8::Handle<v8::Value>() const
{
    Q_ASSERT(*this);
    return m_value;
}

int QScriptEnginePrivate::Exception::lineNumber() const
{
    if (m_message.IsEmpty())
        return -1;
    return m_message->GetLineNumber();
}

QStringList QScriptEnginePrivate::Exception::backtrace() const
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

QT_END_NAMESPACE

#endif
