/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlobjectscriptclass_p.h"

#include "qmlengine_p.h"
#include "qmlcontext_p.h"
#include "qmldeclarativedata_p.h"
#include "qmltypenamescriptclass_p.h"
#include "qmllistscriptclass_p.h"
#include "qmlbinding.h"
#include "qmlguard_p.h"
#include "qmlvmemetaobject_p.h"

#include <QtCore/qtimer.h>
#include <QtCore/qvarlengtharray.h>

Q_DECLARE_METATYPE(QScriptValue);

QT_BEGIN_NAMESPACE

struct ObjectData : public QScriptDeclarativeClass::Object {
    ObjectData(QObject *o, int t) : object(o), type(t) {}
    QmlGuard<QObject> object;
    int type;
};

/*
    The QmlObjectScriptClass handles property access for QObjects
    via QtScript. It is also used to provide a more useful API in
    QtScript for QML.
 */
QmlObjectScriptClass::QmlObjectScriptClass(QmlEngine *bindEngine)
: QmlScriptClass(QmlEnginePrivate::getScriptEngine(bindEngine)), 
#if (QT_VERSION > QT_VERSION_CHECK(4, 6, 2))
  methods(bindEngine),
#endif
  lastData(0), engine(bindEngine)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    m_destroy = scriptEngine->newFunction(destroy);
    m_destroyId = createPersistentIdentifier(QLatin1String("destroy"));
    m_toString = scriptEngine->newFunction(tostring);
    m_toStringId = createPersistentIdentifier(QLatin1String("toString"));
}

QmlObjectScriptClass::~QmlObjectScriptClass()
{
}

QScriptValue QmlObjectScriptClass::newQObject(QObject *object, int type) 
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!object)
        return newObject(scriptEngine, this, new ObjectData(object, type));

    QmlDeclarativeData *ddata = QmlDeclarativeData::get(object, true);

    if (!ddata->scriptValue.isValid()) {
        ddata->scriptValue = newObject(scriptEngine, this, new ObjectData(object, type));
        return ddata->scriptValue;
    } else if (ddata->scriptValue.engine() == QmlEnginePrivate::getScriptEngine(engine)) {
        return ddata->scriptValue;
    } else {
        return newObject(scriptEngine, this, new ObjectData(object, type));
    }
}

QObject *QmlObjectScriptClass::toQObject(const QScriptValue &value) const
{
    return value.toQObject();
}

int QmlObjectScriptClass::objectType(const QScriptValue &value) const
{
    if (scriptClass(value) != this)
        return QVariant::Invalid;

    Object *o = object(value);
    return ((ObjectData*)(o))->type;
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(Object *object, const Identifier &name, 
                                    QScriptClass::QueryFlags flags)
{
    return queryProperty(toQObject(object), name, flags, 0);
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(QObject *obj, const Identifier &name, 
                                    QScriptClass::QueryFlags flags, QmlContext *evalContext,
                                    QueryHints hints)
{
    Q_UNUSED(flags);
    lastData = 0;
    lastTNData = 0;

    if (name == m_destroyId.identifier ||
        name == m_toStringId.identifier)
        return QScriptClass::HandlesReadAccess;

    if (!obj)
        return 0;

    QmlEnginePrivate *enginePrivate = QmlEnginePrivate::get(engine);

    QmlPropertyCache *cache = 0;
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
    if (ddata)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePrivate->cache(obj);
        if (cache && ddata) { cache->addref(); ddata->propertyCache = cache; }
    }

    if (cache) {
        lastData = cache->property(name);
    } else {
        local = QmlPropertyCache::create(obj->metaObject(), toString(name));
        if (local.isValid())
            lastData = &local;
    }

    if (lastData)
        return QScriptClass::HandlesReadAccess | QScriptClass::HandlesWriteAccess; 

    if (!(hints & SkipAttachedProperties)) {
        if (!evalContext && context()) {
            // Global object, QScriptContext activation object, QmlContext object
            QScriptValue scopeNode = scopeChainValue(context(), -3);         
            Q_ASSERT(scopeNode.isValid());
            Q_ASSERT(scriptClass(scopeNode) == enginePrivate->contextClass);

            evalContext = enginePrivate->contextClass->contextFromValue(scopeNode);
        }

        if (evalContext) {
            QmlContextPrivate *cp = QmlContextPrivate::get(evalContext);

            if (cp->imports) {
                QmlTypeNameCache::Data *data = cp->imports->data(name);
                if (data) {
                    lastTNData = data;
                    return QScriptClass::HandlesReadAccess;
                }
            }
        }
    }

    if (!(hints & ImplicitObject)) {
        local.coreIndex = -1;
        lastData = &local;
        return QScriptClass::HandlesReadAccess | QScriptClass::HandlesWriteAccess;
    }

    return 0;
}

QmlObjectScriptClass::ScriptValue
QmlObjectScriptClass::property(Object *object, const Identifier &name)
{
    return property(toQObject(object), name);
}

QmlObjectScriptClass::ScriptValue
QmlObjectScriptClass::property(QObject *obj, const Identifier &name)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (name == m_destroyId.identifier)
        return Value(scriptEngine, m_destroy);
    else if (name == m_toStringId.identifier)
        return Value(scriptEngine, m_toString);

    if (lastData && !lastData->isValid())
        return Value();

    Q_ASSERT(obj);

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    if (lastTNData) {

        if (lastTNData->type)
            return Value(scriptEngine, enginePriv->typeNameClass->newObject(obj, lastTNData->type));
        else
            return Value(scriptEngine, enginePriv->typeNameClass->newObject(obj, lastTNData->typeNamespace));

    } else if (lastData->flags & QmlPropertyCache::Data::IsFunction) {
        if (lastData->flags & QmlPropertyCache::Data::IsVMEFunction) {
            return Value(scriptEngine, ((QmlVMEMetaObject *)(obj->metaObject()))->vmeMethod(lastData->coreIndex));
        } else {
#if (QT_VERSION > QT_VERSION_CHECK(4, 6, 2))
            // Uncomment to use QtScript method call logic
            // QScriptValue sobj = scriptEngine->newQObject(obj);
            // return Value(scriptEngine, sobj.property(toString(name)));
            return Value(scriptEngine, methods.newMethod(obj, lastData));
#else
            QScriptValue sobj = scriptEngine->newQObject(obj);
            return Value(scriptEngine, sobj.property(toString(name)));
#endif
        }
    } else {
        if (enginePriv->captureProperties && !(lastData->flags & QmlPropertyCache::Data::IsConstant)) {
            enginePriv->capturedProperties << 
                QmlEnginePrivate::CapturedProperty(obj, lastData->coreIndex, lastData->notifyIndex);
        }

        if ((uint)lastData->propType < QVariant::UserType) {
            QmlValueType *valueType = enginePriv->valueTypes[lastData->propType];
            if (valueType)
                return Value(scriptEngine, enginePriv->valueTypeClass->newObject(obj, lastData->coreIndex, valueType));
        }

        if (lastData->flags & QmlPropertyCache::Data::IsQList) {
            return Value(scriptEngine, enginePriv->listClass->newList(obj, lastData->coreIndex, QmlListScriptClass::QListPtr, lastData->propType));
        } else if (lastData->flags & QmlPropertyCache::Data::IsQmlList) {
            return Value(scriptEngine, enginePriv->listClass->newList(obj, lastData->coreIndex, QmlListScriptClass::QmlListPtr, lastData->propType));
        } else if (lastData->flags & QmlPropertyCache::Data::IsQObjectDerived) {
            QObject *rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, newQObject(rv, lastData->propType));
        } else if (lastData->flags & QmlPropertyCache::Data::IsQScriptValue) {
            QScriptValue rv = scriptEngine->nullValue();
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::QReal) {
            qreal rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::Int) {
            int rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::Bool) {
            bool rv = false;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::QString) {
            QString rv;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::UInt) {
            uint rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::Float) {
            float rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::Double) {
            double rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else {
            QVariant var = obj->metaObject()->property(lastData->coreIndex).read(obj);
            return Value(scriptEngine, enginePriv->scriptValueFromVariant(var));
        }

    }
}

void QmlObjectScriptClass::setProperty(Object *object, 
                                       const Identifier &name, 
                                       const QScriptValue &value)
{
    return setProperty(toQObject(object), name, value);
}

void QmlObjectScriptClass::setProperty(QObject *obj, 
                                       const Identifier &name, 
                                       const QScriptValue &value,
                                       QmlContext *evalContext)
{
    Q_UNUSED(name);

    Q_ASSERT(obj);
    Q_ASSERT(lastData);

    if (!lastData->isValid()) {
        QString error = QLatin1String("Cannot assign to non-existant property \"") +
                        toString(name) + QLatin1Char('\"');
        if (context())
            context()->throwError(error);
        return;
    }

    if (!(lastData->flags & QmlPropertyCache::Data::IsWritable)) {
        QString error = QLatin1String("Cannot assign to read-only property \"") +
                        toString(name) + QLatin1Char('\"');
        if (context())
            context()->throwError(error);
        return;
    }

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    if (!evalContext && context()) {
        // Global object, QScriptContext activation object, QmlContext object
        QScriptValue scopeNode = scopeChainValue(context(), -3);         
        Q_ASSERT(scopeNode.isValid());
        Q_ASSERT(scriptClass(scopeNode) == enginePriv->contextClass);

        evalContext = enginePriv->contextClass->contextFromValue(scopeNode);
    }

    QmlAbstractBinding *delBinding = QmlMetaPropertyPrivate::setBinding(obj, *lastData, 0);
    if (delBinding)
        delBinding->destroy();

    if (value.isUndefined() && lastData->flags & QmlPropertyCache::Data::IsResettable) {
        void *a[] = { 0 };
        QMetaObject::metacall(obj, QMetaObject::ResetProperty, lastData->coreIndex, a);
    } else {
        // ### Can well known types be optimized?
        QVariant v = QmlScriptClass::toVariant(engine, value);
        QmlMetaPropertyPrivate::write(obj, *lastData, v, evalContext);
    }
}

bool QmlObjectScriptClass::isQObject() const
{
    return true;
}

QObject *QmlObjectScriptClass::toQObject(Object *object, bool *ok)
{
    if (ok) *ok = true;

    ObjectData *data = (ObjectData*)object;
    return data->object.data();
}

QScriptValue QmlObjectScriptClass::tostring(QScriptContext *context, QScriptEngine *)
{
    QObject* obj = context->thisObject().toQObject();

    QString ret;
    if(obj){
        QString objectName = obj->objectName();

        ret += QString::fromUtf8(obj->metaObject()->className());
        ret += QLatin1String("(0x");
        ret += QString::number((quintptr)obj,16);

        if (!objectName.isEmpty()) {
            ret += QLatin1String(", \"");
            ret += objectName;
            ret += QLatin1Char('\"');
        }

        ret += QLatin1Char(')');
    }else{
        ret += QLatin1String("null");
    }
    return QScriptValue(ret);
}

QScriptValue QmlObjectScriptClass::destroy(QScriptContext *context, QScriptEngine *engine)
{
    QObject* obj = context->thisObject().toQObject();
    if(obj){
        int delay = 0;
        if(context->argumentCount() > 0)
            delay = context->argument(0).toInt32();
        if (delay > 0)
            QTimer::singleShot(delay, obj, SLOT(deleteLater()));
        else
            obj->deleteLater();
    }
    return engine->nullValue();
}

QStringList QmlObjectScriptClass::propertyNames(Object *object)
{
    QObject *obj = toQObject(object);
    if (!obj)
        return QStringList();

    QmlEnginePrivate *enginePrivate = QmlEnginePrivate::get(engine);

    QmlPropertyCache *cache = 0;
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
    if (ddata)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePrivate->cache(obj);
        if (cache && ddata) { cache->addref(); ddata->propertyCache = cache; }
    }

    if (!cache)
        return QStringList();

    return cache->propertyNames();
}

#if (QT_VERSION > QT_VERSION_CHECK(4, 6, 2))

struct MethodData : public QScriptDeclarativeClass::Object {
    MethodData(QObject *o, const QmlPropertyCache::Data &d) : object(o), data(d) {}

    QmlGuard<QObject> object;
    QmlPropertyCache::Data data;
};

QmlObjectMethodScriptClass::QmlObjectMethodScriptClass(QmlEngine *bindEngine)
: QScriptDeclarativeClass(QmlEnginePrivate::getScriptEngine(bindEngine)), 
  engine(bindEngine)
{
    setSupportsCall(true);
}

QmlObjectMethodScriptClass::~QmlObjectMethodScriptClass()
{
}

QScriptValue QmlObjectMethodScriptClass::newMethod(QObject *object, const QmlPropertyCache::Data *method)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new MethodData(object, *method));
}

namespace {
struct MetaCallArgument {
    inline MetaCallArgument();
    inline ~MetaCallArgument();
    inline void *dataPtr();

    inline void initAsType(int type, QmlEngine *);
    void fromScriptValue(int type, QmlEngine *, const QScriptValue &);
    inline QScriptDeclarativeClass::Value toValue(QmlEngine *);

private:
    MetaCallArgument(const MetaCallArgument &);
    
    inline void cleanup();

    char *data[16];
    int type;
};
}

MetaCallArgument::MetaCallArgument()
: type(QVariant::Invalid)
{
}

MetaCallArgument::~MetaCallArgument()
{
    cleanup();
}

void MetaCallArgument::cleanup() 
{
    if (type == QMetaType::QString) {
        ((QString *)data)->~QString();
    } else if (type == -1 || type == qMetaTypeId<QVariant>()) {
        ((QVariant *)data)->~QVariant();
    } else if (type == qMetaTypeId<QScriptValue>()) {
        ((QScriptValue *)data)->~QScriptValue();
    }
}

void *MetaCallArgument::dataPtr()
{
    if (type == -1) 
        return ((QVariant *)data)->data();
    else
        return (void *)data;
}

void MetaCallArgument::initAsType(int callType, QmlEngine *e)
{
    if (type != 0) { cleanup(); type = 0; }
    if (callType == 0) return;

    QScriptEngine *engine = QmlEnginePrivate::getScriptEngine(e);
    
    if (callType == qMetaTypeId<QScriptValue>()) {
        new (data) QScriptValue(engine->undefinedValue());
        type = callType;
    } else if (callType == QMetaType::Int ||
               callType == QMetaType::UInt ||
               callType == QMetaType::Bool ||
               callType == QMetaType::Double ||
               callType == QMetaType::Float) {
        type = callType;
    } else if (callType == QMetaType::QObjectStar) {
        *((QObject **)data) = 0;
        type = callType;
    } else if (callType == QMetaType::QString) {
        new (data) QString();
        type = callType;
    } else if (callType == qMetaTypeId<QVariant>()) {
        type = qMetaTypeId<QVariant>();
        new (data) QVariant();
    } else {
        type = -1;
        new (data) QVariant(callType, (void *)0);
    }
}

void MetaCallArgument::fromScriptValue(int callType, QmlEngine *engine, const QScriptValue &value)
{
    if (type != 0) { cleanup(); type = 0; }

    if (callType == qMetaTypeId<QScriptValue>()) {
        new (data) QScriptValue(value);
        type = qMetaTypeId<QScriptValue>();
    } else if (callType == QMetaType::Int) {
        *((int *)data) = int(value.toInt32());
        type = callType;
    } else if (callType == QMetaType::UInt) {
        *((uint *)data) = uint(value.toUInt32());
        type = callType;
    } else if (callType == QMetaType::Bool) {
        *((bool *)data) = value.toBool();
        type = callType;
    } else if (callType == QMetaType::Double) {
        *((double *)data) = double(value.toNumber());
        type = callType;
    } else if (callType == QMetaType::Float) {
        *((float *)data) = float(value.toNumber());
        type = callType;
    } else if (callType == QMetaType::QString) {
        if (value.isNull() || value.isUndefined())
            new (data) QString();
        else
            new (data) QString(value.toString());
        type = callType;
    } else if (callType == QMetaType::QObjectStar) {
        *((QObject **)data) = value.toQObject();
        type = callType;
    } else if (callType == qMetaTypeId<QVariant>()) {
        new (data) QVariant(QmlScriptClass::toVariant(engine, value));
        type = callType;
    } else {
        new (data) QVariant();
        type = -1;

        QVariant v = QmlScriptClass::toVariant(engine, value);
        if (v.userType() == callType) {
            *((QVariant *)data) = v;
        } else if (v.canConvert((QVariant::Type)callType)) {
            *((QVariant *)data) = v;
            ((QVariant *)data)->convert((QVariant::Type)callType);
        } else {
            *((QVariant *)data) = QVariant(callType, (void *)0);
        }
    }
}

QScriptDeclarativeClass::Value MetaCallArgument::toValue(QmlEngine *e)
{
    QScriptEngine *engine = QmlEnginePrivate::getScriptEngine(e);
    
    if (type == qMetaTypeId<QScriptValue>()) {
        return QScriptDeclarativeClass::Value(engine, *((QScriptValue *)data));
    } else if (type == QMetaType::Int) {
        return QScriptDeclarativeClass::Value(engine, *((int *)data));
    } else if (type == QMetaType::UInt) {
        return QScriptDeclarativeClass::Value(engine, *((uint *)data));
    } else if (type == QMetaType::Bool) {
        return QScriptDeclarativeClass::Value(engine, *((bool *)data));
    } else if (type == QMetaType::Double) {
        return QScriptDeclarativeClass::Value(engine, *((double *)data));
    } else if (type == QMetaType::Float) {
        return QScriptDeclarativeClass::Value(engine, *((float *)data));
    } else if (type == QMetaType::QString) {
        return QScriptDeclarativeClass::Value(engine, *((QString *)data));
    } else if (type == QMetaType::QObjectStar) {
        return QScriptDeclarativeClass::Value(engine, QmlEnginePrivate::get(e)->objectClass->newQObject(*((QObject **)data)));
    } else if (type == -1 || type == qMetaTypeId<QVariant>()) {
        return QScriptDeclarativeClass::Value(engine, QmlEnginePrivate::get(e)->scriptValueFromVariant(*((QVariant *)data)));
    } else {
        return QScriptDeclarativeClass::Value();
    }
}

QmlObjectMethodScriptClass::Value QmlObjectMethodScriptClass::call(Object *o, QScriptContext *ctxt)
{
    MethodData *method = static_cast<MethodData *>(o);

    if (method->data.flags & QmlPropertyCache::Data::HasArguments) {

        QMetaMethod m = method->object->metaObject()->method(method->data.coreIndex);
        QList<QByteArray> argTypeNames = m.parameterTypes();
        QVarLengthArray<int, 9> argTypes(argTypeNames.count());

        // ### Cache
        for (int ii = 0; ii < argTypeNames.count(); ++ii) {
            argTypes[ii] = QMetaType::type(argTypeNames.at(ii));
            if (argTypes[ii] == QVariant::Invalid) 
                return Value(ctxt, ctxt->throwError(QString(QLatin1String("Unknown method parameter type: %1")).arg(QLatin1String(argTypeNames.at(ii)))));
        }

        if (argTypes.count() > ctxt->argumentCount()) 
            return Value(ctxt, ctxt->throwError("Insufficient arguments"));

        QVarLengthArray<MetaCallArgument, 9> args(argTypes.count() + 1);
        args[0].initAsType(method->data.propType, engine);

        for (int ii = 0; ii < argTypes.count(); ++ii) 
            args[ii + 1].fromScriptValue(argTypes[ii], engine, ctxt->argument(ii));

        QVarLengthArray<void *, 9> argData(args.count());
        for (int ii = 0; ii < args.count(); ++ii) 
            argData[ii] = args[ii].dataPtr();

        QMetaObject::metacall(method->object, QMetaObject::InvokeMetaMethod, method->data.coreIndex, argData.data());

        return args[0].toValue(engine);

    } else if (method->data.propType != 0) {

        MetaCallArgument arg;
        arg.initAsType(method->data.propType, engine);

        void *args[] = { arg.dataPtr() };

        QMetaObject::metacall(method->object, QMetaObject::InvokeMetaMethod, method->data.coreIndex, args);

        return arg.toValue(engine);

    } else {

        void *args[] = { 0 };
        QMetaObject::metacall(method->object, QMetaObject::InvokeMetaMethod, method->data.coreIndex, args);
        return Value();

    }
    return Value();
}

#endif

QT_END_NAMESPACE

