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
#include <private/qmldeclarativedata_p.h>

QT_BEGIN_NAMESPACE

struct ObjectData {
    ObjectData(QObject *o) : object(o) {}
    QGuard<QObject> object;
};

static QScriptValue QmlObjectToString(QScriptContext *context, QScriptEngine *engine)
{
    QObject* obj = context->thisObject().data().toQObject();
    QString ret = QLatin1String("Qml Object, ");
    if(obj){
        //###Should this be designer or developer details? Dev for now.
        //TODO: Can we print the id too?
        ret += QLatin1String("\"");
        ret += obj->objectName();
        ret += QLatin1String("\" ");
        ret += QLatin1String(obj->metaObject()->className());
        ret += QLatin1String("(0x");
        ret += QString::number((quintptr)obj,16);
        ret += QLatin1String(")");
    }else{
        ret += QLatin1String("null");
    }
    return engine->newVariant(ret);
}

static QScriptValue QmlObjectDestroy(QScriptContext *context, QScriptEngine *engine)
{
    QObject* obj = context->thisObject().toQObject();
    if(obj){
        int delay = 0;
        if(context->argumentCount() > 0)
            delay = context->argument(0).toInt32();
        obj->deleteLater();
        //### Should this be delayed as well?
        context->thisObject().setData(QScriptValue(engine, 0));
    }
    return engine->nullValue();
}

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

    m_destroy = scriptEngine->newFunction(QmlObjectDestroy);
    m_destroyId = createPersistentIdentifier<Dummy>(QLatin1String("destroy"));
}

QmlObjectScriptClass::~QmlObjectScriptClass()
{
    delete m_destroyId;
}

QScriptValue QmlObjectScriptClass::newQObject(QObject *object) 
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, (Object)new ObjectData(object));
}

QObject *QmlObjectScriptClass::toQObject(const QScriptValue &value) const
{
    return value.toQObject();
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(const Object &object, const Identifier &name, 
                                    QScriptClass::QueryFlags flags)
{
    return queryProperty(toQObject(object), name, flags);
}

QScriptClass::QueryFlags 
QmlObjectScriptClass::queryProperty(QObject *obj, const Identifier &name, 
                                    QScriptClass::QueryFlags flags)
{
    Q_UNUSED(flags);
    lastData = 0;

    if (name == m_destroyId->identifier)
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
        if (ddata) { cache->addref(); ddata->propertyCache = cache; }
    }

    if (cache) {
        QmlPropertyCache::Data *property = cache->property(name);
        if (!property) return 0;

        if (flags == QScriptClass::HandlesReadAccess) {
            lastData = property;
            return QScriptClass::HandlesReadAccess;
        } else if (property->propType > 0 && property->propType < QVariant::UserType) {
            lastData = property;
            return flags;
        }
    }

    // Fallback
    return QmlEnginePrivate::get(engine)->queryObject(toString(name), &m_id, obj);
}

QScriptValue QmlObjectScriptClass::property(const Object &object, const Identifier &name)
{
    return property(toQObject(object), name);
}

QScriptValue QmlObjectScriptClass::property(QObject *obj, const Identifier &name)
{
    if (name == m_destroyId->identifier)
        return m_destroy;

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    if (!obj) {
        return QScriptValue();
    } else if (lastData) {

        if (lastData->flags & QmlPropertyCache::Data::IsFunction) {
            // ### Optimize
            QScriptValue sobj = scriptEngine->newQObject(obj);
            return sobj.property(toString(name));
        } else {
            QVariant var = obj->metaObject()->property(lastData->coreIndex).read(obj);
            if (!(lastData->flags & QmlPropertyCache::Data::IsConstant)) {
                enginePriv->capturedProperties << 
                    QmlEnginePrivate::CapturedProperty(obj, lastData->coreIndex, 
                                                       lastData->notifyIndex);
            }

            if (lastData->flags & QmlPropertyCache::Data::IsQObjectDerived) {
                QObject *rv = *(QObject **)var.constData();
                return newQObject(rv);
            } else {
                return qScriptValueFromValue(scriptEngine, var);
            }
        }

    } else {
        return QmlEnginePrivate::get(engine)->propertyObject(toString(name), obj, m_id);
    }
}

void QmlObjectScriptClass::setProperty(const Object &object, 
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
    Q_UNUSED(object);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    if (!obj) {
        return;
    } else if (lastData) {
        switch (lastData->propType) {
        case 1:
            {
                bool b = value.toBoolean();
                void *a[1];
                a[0] = &b;
                QMetaObject::metacall(obj, QMetaObject::WriteProperty, lastData->coreIndex, a);
            }
            break;

        case 2:
            {
                int b = value.toInteger();
                void *a[1];
                a[0] = &b;
                QMetaObject::metacall(obj, QMetaObject::WriteProperty, lastData->coreIndex, a);
            }
            break;

        case 6:
            {
                double b = value.toNumber();
                void *a[1];
                a[0] = &b;
                QMetaObject::metacall(obj, QMetaObject::WriteProperty, lastData->coreIndex, a);
            }
            break;

        default:
            {
                QMetaProperty p = obj->metaObject()->property(lastData->coreIndex);
                p.write(obj, value.toVariant());
            }
        }
    } else {
        QmlEnginePrivate::get(engine)->setPropertyObject(value, m_id);
    }
}

QObject *QmlObjectScriptClass::toQObject(const Object &object, bool *ok)
{
    if (ok) *ok = true;

    ObjectData *data = (ObjectData*)object;
    return data->object.data();
}

void QmlObjectScriptClass::destroyed(const Object &object)
{
    ObjectData *data = (ObjectData*)object;
    delete data;
}

QT_END_NAMESPACE
