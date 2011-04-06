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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <QtCore/qset.h>
#include <QtCore/qstack.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmutex.h>

#include <private/qobject_p.h>

#include "qscriptengine.h"
#include "qscriptconverter_p.h"
#include "qscriptshareddata_p.h"

#include "qscriptoriginalglobalobject_p.h"
#include "qscriptvalue.h"
#include "qscriptprogram_p.h"
#include "qscripttools_p.h"
#include "qscriptengineagent_p.h"
#include <v8.h>

Q_DECLARE_METATYPE(QScriptValue)
Q_DECLARE_METATYPE(QObjectList)
Q_DECLARE_METATYPE(QList<int>)

QT_BEGIN_NAMESPACE

class QDateTime;
class QScriptContextPrivate;
class QScriptClassPrivate;
class QScriptablePrivate;
class QtDataBase;
class QScriptEngineAgentPrivate;

class QScriptEnginePrivate
    : public QObjectPrivate
{
    class Exception
    {
        typedef QPair<v8::Persistent<v8::Value>, v8::Persistent<v8::Message> > ValueMessagePair;

        v8::Persistent<v8::Value> m_value;
        v8::Persistent<v8::Message> m_message;
        QStack<ValueMessagePair> m_stack;

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

        inline void push();
        inline void pop();
    };

    // FIXME: This can go away as bug http://code.google.com/p/v8/issues/detail?id=1205
    // will be resolved and we can store QScriptEnginePrivate* in v8::Isolate
    class Isolates {
    public:
        static v8::Isolate *createEnterIsolate(QScriptEnginePrivate *engine);
        static QScriptEnginePrivate *engine(v8::Isolate *isolate);
    private:
        Q_GLOBAL_STATIC(Isolates, isolates);

        typedef QHash<v8::Isolate*, QScriptEnginePrivate*> QHashIsolateEngine;
        QHashIsolateEngine m_mapping;
        QMutex m_protector;
    };

    Q_DECLARE_PUBLIC(QScriptEngine)
public:
    static QScriptEnginePrivate* get(QScriptEngine* q) { Q_ASSERT(q); return q->d_func(); }
    static QScriptEngine* get(QScriptEnginePrivate* d) { Q_ASSERT(d); return d->q_func(); }

    QScriptEnginePrivate(QScriptEngine::ContextOwnership ownership = QScriptEngine::CreateNewContext);
    ~QScriptEnginePrivate();

    inline QScriptPassPointer<QScriptValuePrivate> evaluate(const QString &program, const QString &fileName = QString(), int lineNumber = 1);
    inline QScriptPassPointer<QScriptValuePrivate> evaluate(QScriptProgramPrivate* program);
    QScriptPassPointer<QScriptValuePrivate> evaluate(v8::Handle<v8::Script> script, v8::TryCatch& tryCatch);
    inline bool isEvaluating() const;
    inline bool isDestroyed() const;
    inline void collectGarbage();
    static void GCEpilogueCallback(v8::GCType type, v8::GCCallbackFlags flags);
    inline void reportAdditionalMemoryCost(int cost);
    inline void abortEvaluation(v8::Handle<v8::Value> result);
    inline void updateGlobalObjectCache();
    inline v8::Handle<v8::Object> globalObject() const;
    void setGlobalObject(QScriptValuePrivate* newGlobalObjectValue);

    inline void setAgent(QScriptEngineAgentPrivate *agent);
    inline QScriptEngineAgentPrivate *agent() const;

    QScriptPassPointer<QScriptValuePrivate> newArray(uint length);
    QScriptPassPointer<QScriptValuePrivate> newObject();
    QScriptPassPointer<QScriptValuePrivate> newObject(QScriptClassPrivate* scriptclass, QScriptValuePrivate* data);
    QScriptPassPointer<QScriptValuePrivate> newFunction(QScriptEngine::FunctionSignature fun, QScriptValuePrivate *prototype, int length);
    QScriptPassPointer<QScriptValuePrivate> newFunction(QScriptEngine::FunctionWithArgSignature fun, void *arg);
    v8::Handle<v8::Object> newVariant(const QVariant &variant);
    QScriptPassPointer<QScriptValuePrivate> newVariant(QScriptValuePrivate* value, const QVariant &variant);
    v8::Handle<v8::Value> newQObject(QObject *object,
                                       QScriptEngine::ValueOwnership own = QScriptEngine::QtOwnership,
                                       const QScriptEngine::QObjectWrapOptions &opt = 0);
    QScriptPassPointer<QScriptValuePrivate> newQObject(QScriptValuePrivate *scriptObject,
                                                       QObject *qtObject,
                                                       QScriptEngine::ValueOwnership ownership,
                                                       const QScriptEngine::QObjectWrapOptions &options);
    v8::Handle<v8::Object> newQMetaObject(const QMetaObject* mo, const QScriptValue &ctor);


    v8::Handle<v8::FunctionTemplate> qtClassTemplate(const QMetaObject *, const QScriptEngine::QObjectWrapOptions &);
    v8::Handle<v8::FunctionTemplate> qobjectTemplate();
    v8::Handle<v8::FunctionTemplate> scriptClassToStringTemplate();

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

    QScriptValue scriptValueFromInternal(v8::Handle<v8::Value>) const;

    v8::Persistent<v8::Object> v8ObjectForConnectedObject(const QObject *o) const;
    void addV8ObjectForConnectedObject(const QObject *o, v8::Persistent<v8::Object> v8Object);
    // private slot
    void _q_removeConnectedObject(QObject*);

    inline operator v8::Handle<v8::Context>();
    inline void clearExceptions();
    inline void setException(v8::Handle<v8::Value> value, v8::Handle<v8::Message> message = v8::Handle<v8::Message>());
    inline v8::Handle<v8::Value> throwException(v8::Handle<v8::Value> value);
    inline bool hasUncaughtException() const;
    inline int uncaughtExceptionLineNumber() const;
    inline QStringList uncaughtExceptionBacktrace() const;
    v8::Handle<v8::Value> uncaughtException() const;
    inline void saveException();
    inline void restoreException();

    v8::Handle<v8::String> qtDataId();

    inline void enterIsolate() const;
    inline void exitIsolate() const;

    void pushScope(QScriptValuePrivate* value);
    QScriptPassPointer<QScriptValuePrivate> popScope();

    void registerCustomType(int type, QScriptEngine::MarshalFunction mf, QScriptEngine::DemarshalFunction df, const QScriptValuePrivate *prototype);
    void setDefaultPrototype(int metaTypeId, const QScriptValuePrivate *prototype);
    QScriptPassPointer<QScriptValuePrivate> defaultPrototype(int metaTypeId);
    v8::Handle<v8::Object> defaultPrototype(const char* metaTypeName);

    inline QScriptContextPrivate *setCurrentQSContext(QScriptContextPrivate *ctx);
    inline QScriptContextPrivate *currentContext() const { return m_currentQsContext; }
    QScriptContextPrivate *pushContext();
    void popContext();
    void emitSignalHandlerException();

    inline bool hasInstance(v8::Handle<v8::FunctionTemplate> fun, v8::Handle<v8::Value> value) const;
    inline const QScriptOriginalGlobalObject *originalGlobalObject() const { return &m_originalGlobalObject; }

    // Additional resources allocated by QSV and kept as weak references can leak if not collected
    // by GC before context destruction. So we need to track them separetly.
    // data should be a QtData instance
    inline void registerAdditionalResources(QtDataBase *data);
    inline void unregisterAdditionalResources(QtDataBase *data);
    inline void deallocateAdditionalResources();

    inline void registerValue(QScriptValuePrivate *data);
    inline void unregisterValue(QScriptValuePrivate *data);
    inline void invalidateAllValues();

    inline void registerString(QScriptStringPrivate *data);
    inline void unregisterString(QScriptStringPrivate *data);
    inline void invalidateAllString();

    inline void registerScriptable(QScriptablePrivate *data);
    inline void unregisterScriptable(QScriptablePrivate *data);
    inline void invalidateAllScriptable();

    inline void registerAgent(QScriptEngineAgentPrivate *data);
    inline void unregisterAgent(QScriptEngineAgentPrivate *data);
    inline void invalidateAllAgents();

    inline void registerScript(QScriptEngineAgentPrivate::UnloadData *data);
    inline void unregisterScript(QScriptEngineAgentPrivate::UnloadData *data);
    inline void invalidateAllScripts();

    bool hasDemarshalFunction(int metaTypeId) const;

    v8::Persistent<v8::FunctionTemplate> declarativeClassTemplate;
    v8::Persistent<v8::FunctionTemplate> scriptClassTemplate;
    v8::Persistent<v8::FunctionTemplate> metaMethodTemplate;
    v8::Persistent<v8::FunctionTemplate> signalTemplate;

    class TypeInfos
    {
    public:
        struct TypeInfo
        {
            TypeInfo() : marshal(0), demarshal(0) { }
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
    struct EvaluateScope {
        QScriptEnginePrivate *engine;
        bool wasEvaluating;
        EvaluateScope(QScriptEnginePrivate *engine);
        ~EvaluateScope();
    };
private:
    Q_DISABLE_COPY(QScriptEnginePrivate)
    v8::Local<v8::Value> getOwnPropertyFromScriptClassInstance(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    QScriptValue::PropertyFlags getPropertyFlagsFromScriptClassInstance(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode);
    v8::Handle<v8::FunctionTemplate> createMetaObjectTemplate();
    v8::Handle<v8::FunctionTemplate> createVariantTemplate();
    v8::Handle<v8::FunctionTemplate> metaObjectTemplate();
    v8::Handle<v8::FunctionTemplate> variantTemplate();

    v8::Isolate *m_isolate;
    v8::Persistent<v8::Context> m_v8Context;
    Exception m_exception;
    QScriptOriginalGlobalObject m_originalGlobalObject;
    v8::Persistent<v8::Object> m_currentGlobalObject;
    v8::Persistent<v8::String> m_qtDataId;
    int m_reportedAddtionalMemoryCost;

    typedef QHash<QPair<const QMetaObject *, QScriptEngine::QObjectWrapOptions>, v8::Persistent<v8::FunctionTemplate> > ClassTemplateHash;
    ClassTemplateHash m_qtClassTemplates;
    v8::Persistent<v8::FunctionTemplate> m_qobjectBaseTemplate;
    v8::Persistent<v8::FunctionTemplate> m_variantTemplate;
    v8::Persistent<v8::FunctionTemplate> m_metaObjectTemplate;
    QScriptContextPrivate *m_currentQsContext;
    QScopedPointer<QScriptContextPrivate> m_baseQsContext;
    QSet<int> visitedConversionObjects;
    TypeInfos m_typeInfos;

    v8::Persistent<v8::FunctionTemplate> m_scriptClassToStringTemplate;

    QSet<QString> importedExtensions;
    QSet<QString> extensionsBeingImported;

    enum State { Idle, Evaluating, Destroyed } m_state;
    QScriptBagContainer<QtDataBase> m_additionalResources;
    QScriptBagContainer<QScriptValuePrivate> m_values;
    QScriptBagContainer<QScriptStringPrivate> m_strings;
    QScriptBagContainer<QScriptablePrivate> m_scriptable;
    QScriptBagContainer<QScriptEngineAgentPrivate> m_agents;
    QScriptBagContainer<QScriptEngineAgentPrivate::UnloadData> m_scripts;
    QHash<const QObject *, v8::Persistent<v8::Object> > m_connectedObjects;

    QScriptEngineAgentPrivate *m_currentAgent;
    class ProcessEventTimeoutThread;
    ProcessEventTimeoutThread *m_processEventTimeoutThread;
    int m_processEventInterval;
    bool m_shouldAbort;
    v8::Persistent<v8::Value> m_abortResult;
};

QT_END_NAMESPACE

#endif
