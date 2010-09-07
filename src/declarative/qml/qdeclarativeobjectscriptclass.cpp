/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativeobjectscriptclass_p.h"

#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativecontext_p.h"
#include "private/qdeclarativedata_p.h"
#include "private/qdeclarativetypenamescriptclass_p.h"
#include "private/qdeclarativelistscriptclass_p.h"
#include "private/qdeclarativebinding_p.h"
#include "private/qdeclarativeguard_p.h"
#include "private/qdeclarativevmemetaobject_p.h"

#include <QtCore/qtimer.h>
#include <QtCore/qvarlengtharray.h>
#include <QtScript/qscriptcontextinfo.h>

Q_DECLARE_METATYPE(QScriptValue);

QT_BEGIN_NAMESPACE

struct ObjectData : public QScriptDeclarativeClass::Object {
    ObjectData(QObject *o, int t) : object(o), type(t) {
        if (o) {
            QDeclarativeData *ddata = QDeclarativeData::get(object, true);
            if (ddata) ddata->objectDataRefCount++;
        }
    }

    virtual ~ObjectData() {
        if (object && !object->parent()) {
            QDeclarativeData *ddata = QDeclarativeData::get(object, false);
            if (ddata && !ddata->indestructible && 0 == --ddata->objectDataRefCount)
                object->deleteLater();
        }
    }

    QDeclarativeGuard<QObject> object;
    int type;
};

/*
    The QDeclarativeObjectScriptClass handles property access for QObjects
    via QtScript. It is also used to provide a more useful API in
    QtScript for QML.
 */
QDeclarativeObjectScriptClass::QDeclarativeObjectScriptClass(QDeclarativeEngine *bindEngine)
: QScriptDeclarativeClass(QDeclarativeEnginePrivate::getScriptEngine(bindEngine)),
  methods(bindEngine), lastData(0), engine(bindEngine)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    m_destroy = scriptEngine->newFunction(destroy);
    m_destroyId = createPersistentIdentifier(QLatin1String("destroy"));
    m_toString = scriptEngine->newFunction(tostring);
    m_toStringId = createPersistentIdentifier(QLatin1String("toString"));
}

QDeclarativeObjectScriptClass::~QDeclarativeObjectScriptClass()
{
}

QScriptValue QDeclarativeObjectScriptClass::newQObject(QObject *object, int type)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (!object)
        return scriptEngine->nullValue();
//        return newObject(scriptEngine, this, new ObjectData(object, type));

    if (QObjectPrivate::get(object)->wasDeleted)
       return scriptEngine->undefinedValue();

    QDeclarativeData *ddata = QDeclarativeData::get(object, true);

    if (!ddata) {
       return scriptEngine->undefinedValue();
    } else if (!ddata->indestructible && !object->parent()) {
        return newObject(scriptEngine, this, new ObjectData(object, type));
    } else if (!ddata->scriptValue) {
        ddata->scriptValue = new QScriptValue(newObject(scriptEngine, this, new ObjectData(object, type)));
        return *ddata->scriptValue;
    } else if (ddata->scriptValue->engine() == QDeclarativeEnginePrivate::getScriptEngine(engine)) {
        return *ddata->scriptValue;
    } else {
        return newObject(scriptEngine, this, new ObjectData(object, type));
    }
}

QObject *QDeclarativeObjectScriptClass::toQObject(const QScriptValue &value) const
{
    return value.toQObject();
}

int QDeclarativeObjectScriptClass::objectType(const QScriptValue &value) const
{
    if (scriptClass(value) != this)
        return QVariant::Invalid;

    Object *o = object(value);
    return ((ObjectData*)(o))->type;
}

QScriptClass::QueryFlags
QDeclarativeObjectScriptClass::queryProperty(Object *object, const Identifier &name,
                                    QScriptClass::QueryFlags flags)
{
    return queryProperty(toQObject(object), name, flags, 0);
}

QScriptClass::QueryFlags
QDeclarativeObjectScriptClass::queryProperty(QObject *obj, const Identifier &name,
                                    QScriptClass::QueryFlags flags, QDeclarativeContextData *evalContext,
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

    QDeclarativeEnginePrivate *enginePrivate = QDeclarativeEnginePrivate::get(engine);
    lastData = QDeclarativePropertyCache::property(engine, obj, name, local);

    if (lastData)
        return QScriptClass::HandlesReadAccess | QScriptClass::HandlesWriteAccess;

    if (!(hints & SkipAttachedProperties)) {
        if (!evalContext && context()) {
            // Global object, QScriptContext activation object, QDeclarativeContext object
            QScriptValue scopeNode = scopeChainValue(context(), -3);
            if (scopeNode.isValid()) {
                Q_ASSERT(scriptClass(scopeNode) == enginePrivate->contextClass);

                evalContext = enginePrivate->contextClass->contextFromValue(scopeNode);
            }
        }

        if (evalContext && evalContext->imports) {
            QDeclarativeTypeNameCache::Data *data = evalContext->imports->data(name);
            if (data) {
                lastTNData = data;
                return QScriptClass::HandlesReadAccess;
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

QDeclarativeObjectScriptClass::Value
QDeclarativeObjectScriptClass::property(Object *object, const Identifier &name)
{
    return property(toQObject(object), name);
}

QDeclarativeObjectScriptClass::Value
QDeclarativeObjectScriptClass::property(QObject *obj, const Identifier &name)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (name == m_destroyId.identifier)
        return Value(scriptEngine, m_destroy);
    else if (name == m_toStringId.identifier)
        return Value(scriptEngine, m_toString);

    if (lastData && !lastData->isValid())
        return Value();

    Q_ASSERT(obj);

    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);

    if (lastTNData) {

        if (lastTNData->type)
            return Value(scriptEngine, enginePriv->typeNameClass->newObject(obj, lastTNData->type));
        else
            return Value(scriptEngine, enginePriv->typeNameClass->newObject(obj, lastTNData->typeNamespace));

    } else if (lastData->flags & QDeclarativePropertyCache::Data::IsFunction) {
        if (lastData->flags & QDeclarativePropertyCache::Data::IsVMEFunction) {
            return Value(scriptEngine, ((QDeclarativeVMEMetaObject *)(obj->metaObject()))->vmeMethod(lastData->coreIndex));
        } else {
            // Uncomment to use QtScript method call logic
            // QScriptValue sobj = scriptEngine->newQObject(obj);
            // return Value(scriptEngine, sobj.property(toString(name)));
            return Value(scriptEngine, methods.newMethod(obj, lastData));
        }
    } else {
        if (enginePriv->captureProperties && !(lastData->flags & QDeclarativePropertyCache::Data::IsConstant)) {
            enginePriv->capturedProperties <<
                QDeclarativeEnginePrivate::CapturedProperty(obj, lastData->coreIndex, lastData->notifyIndex);
        }

        if (QDeclarativeValueTypeFactory::isValueType((uint)lastData->propType)) {
            QDeclarativeValueType *valueType = enginePriv->valueTypes[lastData->propType];
            if (valueType)
                return Value(scriptEngine, enginePriv->valueTypeClass->newObject(obj, lastData->coreIndex, valueType));
        }

        if (lastData->flags & QDeclarativePropertyCache::Data::IsQList) {
            return Value(scriptEngine, enginePriv->listClass->newList(obj, lastData->coreIndex, lastData->propType));
        } else if (lastData->flags & QDeclarativePropertyCache::Data::IsQObjectDerived) {
            QObject *rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, newQObject(rv, lastData->propType));
        } else if (lastData->flags & QDeclarativePropertyCache::Data::IsQScriptValue) {
            QScriptValue rv = scriptEngine->nullValue();
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::QReal) {
            qreal rv = 0;
            void *args[] = { &rv, 0 };
            QMetaObject::metacall(obj, QMetaObject::ReadProperty, lastData->coreIndex, args);
            return Value(scriptEngine, rv);
        } else if (lastData->propType == QMetaType::Int || lastData->flags & QDeclarativePropertyCache::Data::IsEnumType) {
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

void QDeclarativeObjectScriptClass::setProperty(Object *object,
                                       const Identifier &name,
                                       const QScriptValue &value)
{
    return setProperty(toQObject(object), name, value, context());
}

void QDeclarativeObjectScriptClass::setProperty(QObject *obj,
                                                const Identifier &name,
                                                const QScriptValue &value,
                                                QScriptContext *context,
                                                QDeclarativeContextData *evalContext)
{
    Q_UNUSED(name);

    Q_ASSERT(obj);
    Q_ASSERT(lastData);
    Q_ASSERT(context);

    if (!lastData->isValid()) {
        QString error = QLatin1String("Cannot assign to non-existent property \"") +
                        toString(name) + QLatin1Char('\"');
        context->throwError(error);
        return;
    }

    if (!(lastData->flags & QDeclarativePropertyCache::Data::IsWritable) && 
        !(lastData->flags & QDeclarativePropertyCache::Data::IsQList)) {
        QString error = QLatin1String("Cannot assign to read-only property \"") +
                        toString(name) + QLatin1Char('\"');
        context->throwError(error);
        return;
    }

    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);

    if (!evalContext) {
        // Global object, QScriptContext activation object, QDeclarativeContext object
        QScriptValue scopeNode = scopeChainValue(context, -3);
        if (scopeNode.isValid()) {
            Q_ASSERT(scriptClass(scopeNode) == enginePriv->contextClass);

            evalContext = enginePriv->contextClass->contextFromValue(scopeNode);
        }
    }

    QDeclarativeAbstractBinding *delBinding =
        QDeclarativePropertyPrivate::setBinding(obj, lastData->coreIndex, -1, 0);
    if (delBinding)
        delBinding->destroy();

    if (value.isNull() && lastData->flags & QDeclarativePropertyCache::Data::IsQObjectDerived) {
        QObject *o = 0;
        int status = -1;
        int flags = 0;
        void *argv[] = { &o, 0, &status, &flags };
        QMetaObject::metacall(obj, QMetaObject::WriteProperty, lastData->coreIndex, argv);
    } else if (value.isUndefined() && lastData->flags & QDeclarativePropertyCache::Data::IsResettable) {
        void *a[] = { 0 };
        QMetaObject::metacall(obj, QMetaObject::ResetProperty, lastData->coreIndex, a);
    } else if (value.isUndefined() && lastData->propType == qMetaTypeId<QVariant>()) {
        QDeclarativePropertyPrivate::write(obj, *lastData, QVariant(), evalContext);
    } else if (value.isUndefined()) {
        QString error = QLatin1String("Cannot assign [undefined] to ") +
                        QLatin1String(QMetaType::typeName(lastData->propType));
        context->throwError(error);
    } else if (!value.isRegExp() && value.isFunction()) {
        QString error = QLatin1String("Cannot assign a function to a property.");
        context->throwError(error);
    } else {
        QVariant v;
        if (lastData->flags & QDeclarativePropertyCache::Data::IsQList)
            v = enginePriv->scriptValueToVariant(value, qMetaTypeId<QList<QObject *> >());
        else
            v = enginePriv->scriptValueToVariant(value, lastData->propType);

        if (!QDeclarativePropertyPrivate::write(obj, *lastData, v, evalContext)) {
            const char *valueType = 0;
            if (v.userType() == QVariant::Invalid) valueType = "null";
            else valueType = QMetaType::typeName(v.userType());

            QString error = QLatin1String("Cannot assign ") +
                            QLatin1String(valueType) +
                            QLatin1String(" to ") +
                            QLatin1String(QMetaType::typeName(lastData->propType));
            context->throwError(error);
        }
    }
}

bool QDeclarativeObjectScriptClass::isQObject() const
{
    return true;
}

QObject *QDeclarativeObjectScriptClass::toQObject(Object *object, bool *ok)
{
    if (ok) *ok = true;

    ObjectData *data = (ObjectData*)object;
    return data->object.data();
}

QScriptValue QDeclarativeObjectScriptClass::tostring(QScriptContext *context, QScriptEngine *)
{
    QObject* obj = context->thisObject().toQObject();

    QString ret;
    if(obj){
        QString objectName = obj->objectName();

        ret += QString::fromUtf8(obj->metaObject()->className());
        ret += QLatin1String("(0x");
        ret += QString::number((intptr_t)obj,16);

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

QScriptValue QDeclarativeObjectScriptClass::destroy(QScriptContext *context, QScriptEngine *engine)
{
    QDeclarativeEnginePrivate *p = QDeclarativeEnginePrivate::get(engine);
    QScriptValue that = context->thisObject();

    if (scriptClass(that) != p->objectClass)
        return engine->undefinedValue();

    ObjectData *data = (ObjectData *)p->objectClass->object(that);
    if (!data->object)
        return engine->undefinedValue();

    QDeclarativeData *ddata = QDeclarativeData::get(data->object, false);
    if (!ddata || ddata->indestructible)
        return engine->currentContext()->throwError(QLatin1String("Invalid attempt to destroy() an indestructible object"));

    QObject *obj = data->object;
    int delay = 0;
    if (context->argumentCount() > 0)
        delay = context->argument(0).toInt32();
    if (delay > 0)
        QTimer::singleShot(delay, obj, SLOT(deleteLater()));
    else
        obj->deleteLater();

    return engine->undefinedValue();
}

QStringList QDeclarativeObjectScriptClass::propertyNames(Object *object)
{
    QObject *obj = toQObject(object);
    if (!obj)
        return QStringList();

    QDeclarativeEnginePrivate *enginePrivate = QDeclarativeEnginePrivate::get(engine);

    QDeclarativePropertyCache *cache = 0;
    QDeclarativeData *ddata = QDeclarativeData::get(obj);
    if (ddata)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePrivate->cache(obj);
        if (cache) {
            if (ddata) { cache->addref(); ddata->propertyCache = cache; }
        } else {
            // Not cachable - fall back to QMetaObject (eg. dynamic meta object)
            // XXX QDeclarativeOpenMetaObject has a cache, so this is suboptimal.
            // XXX This is a workaround for QTBUG-9420.
            const QMetaObject *mo = obj->metaObject();
            QStringList r;
            int pc = mo->propertyCount();
            int po = mo->propertyOffset();
            for (int i=po; i<pc; ++i)
                r += QString::fromUtf8(mo->property(i).name());
            return r;
        }
    }
    return cache->propertyNames();
}

bool QDeclarativeObjectScriptClass::compare(Object *o1, Object *o2)
{
    ObjectData *d1 = (ObjectData *)o1;
    ObjectData *d2 = (ObjectData *)o2;

    return d1 == d2 || d1->object == d2->object;
}

struct MethodData : public QScriptDeclarativeClass::Object {
    MethodData(QObject *o, const QDeclarativePropertyCache::Data &d) : object(o), data(d) {}

    QDeclarativeGuard<QObject> object;
    QDeclarativePropertyCache::Data data;
};

QDeclarativeObjectMethodScriptClass::QDeclarativeObjectMethodScriptClass(QDeclarativeEngine *bindEngine)
: QScriptDeclarativeClass(QDeclarativeEnginePrivate::getScriptEngine(bindEngine)),
  engine(bindEngine)
{
    qRegisterMetaType<QList<QObject *> >("QList<QObject *>");

    setSupportsCall(true);

    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    m_connect = scriptEngine->newFunction(connect);
    m_connectId = createPersistentIdentifier(QLatin1String("connect"));
    m_disconnect = scriptEngine->newFunction(disconnect);
    m_disconnectId = createPersistentIdentifier(QLatin1String("disconnect"));
}

QDeclarativeObjectMethodScriptClass::~QDeclarativeObjectMethodScriptClass()
{
}

QScriptValue QDeclarativeObjectMethodScriptClass::newMethod(QObject *object, const QDeclarativePropertyCache::Data *method)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new MethodData(object, *method));
}

QScriptValue QDeclarativeObjectMethodScriptClass::connect(QScriptContext *context, QScriptEngine *engine)
{
    QDeclarativeEnginePrivate *p = QDeclarativeEnginePrivate::get(engine);

    QScriptValue that = context->thisObject();
    if (&p->objectClass->methods != scriptClass(that))
        return engine->undefinedValue();

    MethodData *data = (MethodData *)object(that);

    if (!data->object || context->argumentCount() == 0)
        return engine->undefinedValue();

    QByteArray signal("2");
    signal.append(data->object->metaObject()->method(data->data.coreIndex).signature());

    if (context->argumentCount() == 1) {
        qScriptConnect(data->object, signal.constData(), QScriptValue(), context->argument(0));
    } else {
        qScriptConnect(data->object, signal.constData(), context->argument(0), context->argument(1));
    }

    return engine->undefinedValue();
}

QScriptValue QDeclarativeObjectMethodScriptClass::disconnect(QScriptContext *context, QScriptEngine *engine)
{
    QDeclarativeEnginePrivate *p = QDeclarativeEnginePrivate::get(engine);

    QScriptValue that = context->thisObject();
    if (&p->objectClass->methods != scriptClass(that))
        return engine->undefinedValue();

    MethodData *data = (MethodData *)object(that);

    if (!data->object || context->argumentCount() == 0)
        return engine->undefinedValue();

    QByteArray signal("2");
    signal.append(data->object->metaObject()->method(data->data.coreIndex).signature());

    if (context->argumentCount() == 1) {
        qScriptDisconnect(data->object, signal.constData(), QScriptValue(), context->argument(0));
    } else {
        qScriptDisconnect(data->object, signal.constData(), context->argument(0), context->argument(1));
    }

    return engine->undefinedValue();
}

QScriptClass::QueryFlags
QDeclarativeObjectMethodScriptClass::queryProperty(Object *, const Identifier &name,
                                                   QScriptClass::QueryFlags flags)
{
    Q_UNUSED(flags);
    if (name == m_connectId.identifier || name == m_disconnectId.identifier)
        return QScriptClass::HandlesReadAccess;
    else
        return 0;

}

QDeclarativeObjectMethodScriptClass::Value
QDeclarativeObjectMethodScriptClass::property(Object *, const Identifier &name)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (name == m_connectId.identifier)
        return Value(scriptEngine, m_connect);
    else if (name == m_disconnectId.identifier)
        return Value(scriptEngine, m_disconnect);
    else
        return Value();
}

namespace {
struct MetaCallArgument {
    inline MetaCallArgument();
    inline ~MetaCallArgument();
    inline void *dataPtr();

    inline void initAsType(int type, QDeclarativeEngine *);
    void fromScriptValue(int type, QDeclarativeEngine *, const QScriptValue &);
    inline QScriptDeclarativeClass::Value toValue(QDeclarativeEngine *);

private:
    MetaCallArgument(const MetaCallArgument &);

    inline void cleanup();

    char data[4 * sizeof(void *)];
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
        ((QString *)&data)->~QString();
    } else if (type == -1 || type == qMetaTypeId<QVariant>()) {
        ((QVariant *)&data)->~QVariant();
    } else if (type == qMetaTypeId<QScriptValue>()) {
        ((QScriptValue *)&data)->~QScriptValue();
    } else if (type == qMetaTypeId<QList<QObject *> >()) {
        ((QList<QObject *> *)&data)->~QList<QObject *>();
    }
}

void *MetaCallArgument::dataPtr()
{
    if (type == -1)
        return ((QVariant *)data)->data();
    else
        return (void *)&data;
}

void MetaCallArgument::initAsType(int callType, QDeclarativeEngine *e)
{
    if (type != 0) { cleanup(); type = 0; }
    if (callType == 0) return;

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(e);

    if (callType == qMetaTypeId<QScriptValue>()) {
        new (&data) QScriptValue(engine->undefinedValue());
        type = callType;
    } else if (callType == QMetaType::Int ||
               callType == QMetaType::UInt ||
               callType == QMetaType::Bool ||
               callType == QMetaType::Double ||
               callType == QMetaType::Float) {
        type = callType;
    } else if (callType == QMetaType::QObjectStar) {
        *((QObject **)&data) = 0;
        type = callType;
    } else if (callType == QMetaType::QString) {
        new (&data) QString();
        type = callType;
    } else if (callType == qMetaTypeId<QVariant>()) {
        type = callType;
        new (&data) QVariant();
    } else if (callType == qMetaTypeId<QList<QObject *> >()) {
        type = callType;
        new (&data) QList<QObject *>();
    } else {
        type = -1;
        new (&data) QVariant(callType, (void *)0);
    }
}

void MetaCallArgument::fromScriptValue(int callType, QDeclarativeEngine *engine, const QScriptValue &value)
{
    if (type != 0) { cleanup(); type = 0; }

    if (callType == qMetaTypeId<QScriptValue>()) {
        new (&data) QScriptValue(value);
        type = qMetaTypeId<QScriptValue>();
    } else if (callType == QMetaType::Int) {
        *((int *)&data) = int(value.toInt32());
        type = callType;
    } else if (callType == QMetaType::UInt) {
        *((uint *)&data) = uint(value.toUInt32());
        type = callType;
    } else if (callType == QMetaType::Bool) {
        *((bool *)&data) = value.toBool();
        type = callType;
    } else if (callType == QMetaType::Double) {
        *((double *)&data) = double(value.toNumber());
        type = callType;
    } else if (callType == QMetaType::Float) {
        *((float *)&data) = float(value.toNumber());
        type = callType;
    } else if (callType == QMetaType::QString) {
        if (value.isNull() || value.isUndefined())
            new (&data) QString();
        else
            new (&data) QString(value.toString());
        type = callType;
    } else if (callType == QMetaType::QObjectStar) {
        *((QObject **)&data) = value.toQObject();
        type = callType;
    } else if (callType == qMetaTypeId<QVariant>()) {
        new (&data) QVariant(QDeclarativeEnginePrivate::get(engine)->scriptValueToVariant(value));
        type = callType;
    } else if (callType == qMetaTypeId<QList<QObject*> >()) {
        QList<QObject *> *list = new (&data) QList<QObject *>(); 
        if (value.isArray()) {
            int length = value.property(QLatin1String("length")).toInt32();
            for (int ii = 0; ii < length; ++ii) {
                QScriptValue arrayItem = value.property(ii);
                QObject *d = arrayItem.toQObject();
                list->append(d);
            }
        } else if (QObject *d = value.toQObject()) {
            list->append(d);
        }
        type = callType;
    } else {
        new (&data) QVariant();
        type = -1;

        QVariant v = QDeclarativeEnginePrivate::get(engine)->scriptValueToVariant(value);
        if (v.userType() == callType) {
            *((QVariant *)&data) = v;
        } else if (v.canConvert((QVariant::Type)callType)) {
            *((QVariant *)&data) = v;
            ((QVariant *)&data)->convert((QVariant::Type)callType);
        } else {
            *((QVariant *)&data) = QVariant(callType, (void *)0);
        }
    }
}

QScriptDeclarativeClass::Value MetaCallArgument::toValue(QDeclarativeEngine *e)
{
    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(e);

    if (type == qMetaTypeId<QScriptValue>()) {
        return QScriptDeclarativeClass::Value(engine, *((QScriptValue *)&data));
    } else if (type == QMetaType::Int) {
        return QScriptDeclarativeClass::Value(engine, *((int *)&data));
    } else if (type == QMetaType::UInt) {
        return QScriptDeclarativeClass::Value(engine, *((uint *)&data));
    } else if (type == QMetaType::Bool) {
        return QScriptDeclarativeClass::Value(engine, *((bool *)&data));
    } else if (type == QMetaType::Double) {
        return QScriptDeclarativeClass::Value(engine, *((double *)&data));
    } else if (type == QMetaType::Float) {
        return QScriptDeclarativeClass::Value(engine, *((float *)&data));
    } else if (type == QMetaType::QString) {
        return QScriptDeclarativeClass::Value(engine, *((QString *)&data));
    } else if (type == QMetaType::QObjectStar) {
        QObject *object = *((QObject **)&data);
        if (object)
            QDeclarativeData::get(object, true)->setImplicitDestructible();
        QDeclarativeEnginePrivate *priv = QDeclarativeEnginePrivate::get(e);
        return QScriptDeclarativeClass::Value(engine, priv->objectClass->newQObject(object));
    } else if (type == qMetaTypeId<QList<QObject *> >()) {
        QList<QObject *> &list = *(QList<QObject *>*)&data;
        QScriptValue rv = engine->newArray(list.count());
        QDeclarativeEnginePrivate *priv = QDeclarativeEnginePrivate::get(e);
        for (int ii = 0; ii < list.count(); ++ii) {
            QObject *object = list.at(ii);
            QDeclarativeData::get(object, true)->setImplicitDestructible();
            rv.setProperty(ii, priv->objectClass->newQObject(object));
        }
        return QScriptDeclarativeClass::Value(engine, rv);
    } else if (type == -1 || type == qMetaTypeId<QVariant>()) {
        return QScriptDeclarativeClass::Value(engine, QDeclarativeEnginePrivate::get(e)->scriptValueFromVariant(*((QVariant *)&data)));
    } else {
        return QScriptDeclarativeClass::Value();
    }
}

int QDeclarativeObjectMethodScriptClass::enumType(const QMetaObject *meta, const QString &strname)
{
    QByteArray str = strname.toUtf8();
    QByteArray scope;
    QByteArray name;
    int scopeIdx = str.lastIndexOf("::");
    if (scopeIdx != -1) {
        scope = str.left(scopeIdx);
        name = str.mid(scopeIdx + 2);
    } else { 
        name = str;
    }
    for (int i = meta->enumeratorCount() - 1; i >= 0; --i) {
        QMetaEnum m = meta->enumerator(i);
        if ((m.name() == name) && (scope.isEmpty() || (m.scope() == scope)))
            return QVariant::Int;
    }
    return QVariant::Invalid;
}

QDeclarativeObjectMethodScriptClass::Value QDeclarativeObjectMethodScriptClass::call(Object *o, QScriptContext *ctxt)
{
    MethodData *method = static_cast<MethodData *>(o);

    if (method->data.flags & QDeclarativePropertyCache::Data::HasArguments) {

        QMetaMethod m = method->object->metaObject()->method(method->data.coreIndex);
        QList<QByteArray> argTypeNames = m.parameterTypes();
        QVarLengthArray<int, 9> argTypes(argTypeNames.count());

        // ### Cache
        for (int ii = 0; ii < argTypeNames.count(); ++ii) {
            argTypes[ii] = QMetaType::type(argTypeNames.at(ii));
            if (argTypes[ii] == QVariant::Invalid) 
                argTypes[ii] = enumType(method->object->metaObject(), argTypeNames.at(ii));
            if (argTypes[ii] == QVariant::Invalid) 
                return Value(ctxt, ctxt->throwError(QString::fromLatin1("Unknown method parameter type: %1").arg(QLatin1String(argTypeNames.at(ii)))));
        }

        if (argTypes.count() > ctxt->argumentCount())
            return Value(ctxt, ctxt->throwError(QLatin1String("Insufficient arguments")));

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

QT_END_NAMESPACE

