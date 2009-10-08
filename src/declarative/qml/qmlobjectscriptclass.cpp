/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlobjectscriptclass_p.h"
#include <private/qmlengine_p.h>
#include <private/qguard_p.h>
#include <private/qmlcontext_p.h>
#include <private/qmldeclarativedata_p.h>
#include <private/qmltypenamescriptclass_p.h>
#include <QtDeclarative/qmlbinding.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE

struct ObjectData : public QScriptDeclarativeClass::Object {
    ObjectData(QObject *o) : object(o) {}
    QGuard<QObject> object;
};

/*
    The QmlObjectScriptClass handles property access for QObjects
    via QtScript. It is also used to provide a more useful API in
    QtScript for QML.
 */
QmlObjectScriptClass::QmlObjectScriptClass(QmlEngine *bindEngine)
: QScriptDeclarativeClass(QmlEnginePrivate::getScriptEngine(bindEngine)), lastData(0),
  engine(bindEngine)
{
    engine = bindEngine;
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(bindEngine);

    m_destroy = scriptEngine->newFunction(destroy);
    m_destroyId = createPersistentIdentifier(QLatin1String("destroy"));
    m_toString = scriptEngine->newFunction(tostring);
    m_toStringId = createPersistentIdentifier(QLatin1String("toString"));
}

QmlObjectScriptClass::~QmlObjectScriptClass()
{
}

QScriptValue QmlObjectScriptClass::newQObject(QObject *object) 
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!object)
        return newObject(scriptEngine, this, new ObjectData(object));

    QmlDeclarativeData *ddata = QmlDeclarativeData::get(object, true);

    if (!ddata->scriptValue.isValid()) {
        ddata->scriptValue = newObject(scriptEngine, this, new ObjectData(object));
        return ddata->scriptValue;
    } else if (ddata->scriptValue.engine() == QmlEnginePrivate::getScriptEngine(engine)) {
        return ddata->scriptValue;
    } else {
        return newObject(scriptEngine, this, new ObjectData(object));
    }
}

QObject *QmlObjectScriptClass::toQObject(const QScriptValue &value) const
{
    return value.toQObject();
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(Object *object, const Identifier &name, 
                                    QScriptClass::QueryFlags flags)
{
    return queryProperty(toQObject(object), name, flags, 0);
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(QObject *obj, const Identifier &name, 
                                    QScriptClass::QueryFlags flags, QmlContext *evalContext)
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
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

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

    if (lastData) {
        QScriptClass::QueryFlags rv = QScriptClass::HandlesReadAccess;
        if (lastData->flags & QmlPropertyCache::Data::IsWritable)
            rv |= QScriptClass::HandlesWriteAccess;
        return rv;
    } 

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

    return 0;
}

QScriptValue QmlObjectScriptClass::property(Object *object, const Identifier &name)
{
    return property(toQObject(object), name);
}

QScriptValue QmlObjectScriptClass::property(QObject *obj, const Identifier &name)
{
    if (name == m_destroyId.identifier)
        return m_destroy;
    else if (name == m_toStringId.identifier)
        return m_toString;

    Q_ASSERT(obj);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    if (lastTNData) {

        if (lastTNData->type)
            return enginePriv->typeNameClass->newObject(obj, lastTNData->type);
        else
            return enginePriv->typeNameClass->newObject(obj, lastTNData->typeNamespace);

    } else if (lastData->flags & QmlPropertyCache::Data::IsFunction) {
        // ### Optimize
        QScriptValue sobj = scriptEngine->newQObject(obj);
        return sobj.property(toString(name));
    } else {
        if (!(lastData->flags & QmlPropertyCache::Data::IsConstant)) {
            enginePriv->capturedProperties << 
                QmlEnginePrivate::CapturedProperty(obj, lastData->coreIndex, lastData->notifyIndex);
        }

        if ((uint)lastData->propType < QVariant::UserType) {
            QmlValueType *valueType = enginePriv->valueTypes[lastData->propType];
            if (valueType)
                return enginePriv->valueTypeClass->newObject(obj, lastData->coreIndex, valueType);
        }

        QVariant var = obj->metaObject()->property(lastData->coreIndex).read(obj);

        if (lastData->flags & QmlPropertyCache::Data::IsQObjectDerived) {
            QObject *rv = *(QObject **)var.constData();
            return newQObject(rv);
        } else {
            return enginePriv->scriptValueFromVariant(var);
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
                                       const QScriptValue &value)
{
    Q_UNUSED(name);

    Q_ASSERT(obj);
    Q_ASSERT(lastData);

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    // ### Can well known types be optimized?
    QVariant v = QmlScriptClass::toVariant(engine, value);
    delete QmlMetaPropertyPrivate::setBinding(obj, *lastData, 0);
    QmlMetaPropertyPrivate::write(obj, *lastData, v, enginePriv->currentExpression->context());
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

        ret += QLatin1String(obj->metaObject()->className());
        ret += QLatin1String("(0x");
        ret += QString::number((quintptr)obj,16);

        if (!objectName.isEmpty()) {
            ret += QLatin1String(", \"");
            ret += objectName;
            ret += QLatin1String("\"");
        }

        ret += QLatin1String(")");
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

QT_END_NAMESPACE

