/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlpropertycache_p.h"

#include "qmlengine_p.h"
#include "qmlbinding.h"
#include "qdebug.h"

Q_DECLARE_METATYPE(QScriptValue);

QT_BEGIN_NAMESPACE

void QmlPropertyCache::Data::load(const QMetaProperty &p, QmlEngine *engine)
{
    propType = p.userType();
    if (QVariant::Type(propType) == QVariant::LastType)
        propType = qMetaTypeId<QVariant>();
    coreIndex = p.propertyIndex();
    notifyIndex = p.notifySignalIndex();

    if (p.isConstant())
        flags |= Data::IsConstant;
    if (p.isWritable())
        flags |= Data::IsWritable;
    if (p.isResettable())
        flags |= Data::IsResettable;

    if (propType == qMetaTypeId<QmlBinding *>()) {
        flags |= Data::IsQmlBinding;
    } else if (propType == qMetaTypeId<QScriptValue>()) {
        flags |= Data::IsQScriptValue;
    } else if (p.isEnumType()) {
        flags |= Data::IsEnumType;
    } else {
        QmlMetaType::TypeCategory cat = engine ? QmlEnginePrivate::get(engine)->typeCategory(propType)
                                               : QmlMetaType::typeCategory(propType);
        if (cat == QmlMetaType::Object)
            flags |= Data::IsQObjectDerived;
        else if (cat == QmlMetaType::List)
            flags |= Data::IsQList;
        else if (cat == QmlMetaType::QmlList)
            flags |= Data::IsQmlList;
    }
}

void QmlPropertyCache::Data::load(const QMetaMethod &m)
{
    coreIndex = m.methodIndex();
    flags |= Data::IsFunction;
    propType = QVariant::Invalid;

    const char *returnType = m.typeName();
    if (returnType) 
        propType = QMetaType::type(returnType);

    QList<QByteArray> params = m.parameterTypes();
    if (!params.isEmpty())
        flags |= Data::HasArguments;
}


QmlPropertyCache::QmlPropertyCache(QmlEngine *e)
: QmlCleanup(e), engine(e)
{
}

QmlPropertyCache::~QmlPropertyCache()
{
    clear();
}

void QmlPropertyCache::clear()
{
    for (int ii = 0; ii < indexCache.count(); ++ii) 
        indexCache.at(ii)->release();

    for (StringCache::ConstIterator iter = stringCache.begin(); 
            iter != stringCache.end(); ++iter)
        (*iter)->release();

    for (IdentifierCache::ConstIterator iter = identifierCache.begin(); 
            iter != identifierCache.end(); ++iter)
        (*iter)->release();

    indexCache.clear();
    stringCache.clear();
    identifierCache.clear();
}

QmlPropertyCache::Data QmlPropertyCache::create(const QMetaObject *metaObject, 
                                                const QString &property)
{
    Q_ASSERT(metaObject);

    QmlPropertyCache::Data rv;
    int idx = metaObject->indexOfProperty(property.toUtf8());
    if (idx != -1) {
        rv.load(metaObject->property(idx));
        return rv;
    }

    int methodCount = metaObject->methodCount();
    for (int ii = methodCount - 1; ii >= 0; --ii) {
        QMetaMethod m = metaObject->method(ii);
        if (m.access() == QMetaMethod::Private)
            continue;
        QString methodName = QString::fromUtf8(m.signature());

        int parenIdx = methodName.indexOf(QLatin1Char('('));
        Q_ASSERT(parenIdx != -1);
        methodName = methodName.left(parenIdx);

        if (methodName == property) {
            rv.load(m);
            return rv;
        }
    }

    return rv;
}

QmlPropertyCache *QmlPropertyCache::copy() const
{
    QmlPropertyCache *cache = new QmlPropertyCache(engine);
    cache->indexCache = indexCache;
    cache->stringCache = stringCache;
    cache->identifierCache = identifierCache;

    for (int ii = 0; ii < indexCache.count(); ++ii)
        indexCache.at(ii)->addref();
    for (StringCache::ConstIterator iter = stringCache.begin(); iter != stringCache.end(); ++iter)
        (*iter)->addref();
    for (IdentifierCache::ConstIterator iter = identifierCache.begin(); iter != identifierCache.end(); ++iter)
        (*iter)->addref();

    return cache;
}

void QmlPropertyCache::append(QmlEngine *engine, const QMetaObject *metaObject, 
                              Data::Flag propertyFlags, Data::Flag methodFlags)
{
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    int propCount = metaObject->propertyCount();
    int propOffset = metaObject->propertyOffset();

    indexCache.resize(propCount);
    for (int ii = propOffset; ii < propCount; ++ii) {
        QMetaProperty p = metaObject->property(ii);
        QString propName = QString::fromUtf8(p.name());

        RData *data = new RData;
        data->identifier = enginePriv->objectClass->createPersistentIdentifier(propName);

        data->load(p, engine);
        data->flags |= propertyFlags;

        indexCache[ii] = data;

        if (stringCache.contains(propName)) {
            stringCache[propName]->release();
            identifierCache[data->identifier.identifier]->release();
        }

        stringCache.insert(propName, data);
        identifierCache.insert(data->identifier.identifier, data);
        data->addref();
        data->addref();
    }

    int methodCount = metaObject->methodCount();
    int methodOffset = metaObject->methodOffset();
    for (int ii = methodOffset; ii < methodCount; ++ii) {
        QMetaMethod m = metaObject->method(ii);
        if (m.access() == QMetaMethod::Private)
            continue;
        QString methodName = QString::fromUtf8(m.signature());

        int parenIdx = methodName.indexOf(QLatin1Char('('));
        Q_ASSERT(parenIdx != -1);
        methodName = methodName.left(parenIdx);

        RData *data = new RData;
        data->identifier = enginePriv->objectClass->createPersistentIdentifier(methodName);

        if (stringCache.contains(methodName)) {
            stringCache[methodName]->release();
            identifierCache[data->identifier.identifier]->release();
        }

        data->load(m);
        if (m.methodType() == QMetaMethod::Slot || m.methodType() == QMetaMethod::Method) 
            data->flags |= methodFlags;

        stringCache.insert(methodName, data);
        identifierCache.insert(data->identifier.identifier, data);
        data->addref();
    }
}

// ### Optimize - check engine for the parent meta object etc.
QmlPropertyCache *QmlPropertyCache::create(QmlEngine *engine, const QMetaObject *metaObject)
{
    Q_ASSERT(engine);
    Q_ASSERT(metaObject);

    QmlPropertyCache *cache = new QmlPropertyCache(engine);
    cache->update(engine, metaObject);
    return cache;
}

void QmlPropertyCache::update(QmlEngine *engine, const QMetaObject *metaObject)
{
    Q_ASSERT(engine);
    Q_ASSERT(metaObject);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    clear();

    // ### The properties/methods should probably be spliced on a per-metaobject basis
    int propCount = metaObject->propertyCount();

    indexCache.resize(propCount);
    for (int ii = propCount - 1; ii >= 0; --ii) {
        QMetaProperty p = metaObject->property(ii);
        QString propName = QString::fromUtf8(p.name());

        RData *data = new RData;
        data->identifier = enginePriv->objectClass->createPersistentIdentifier(propName);

        data->load(p, engine);

        indexCache[ii] = data;

        if (stringCache.contains(propName))
            continue;

        stringCache.insert(propName, data);
        identifierCache.insert(data->identifier.identifier, data);
        data->addref();
        data->addref();
    }

    int methodCount = metaObject->methodCount();
    for (int ii = methodCount - 1; ii >= 0; --ii) {
        QMetaMethod m = metaObject->method(ii);
        if (m.access() == QMetaMethod::Private)
            continue;
        QString methodName = QString::fromUtf8(m.signature());

        int parenIdx = methodName.indexOf(QLatin1Char('('));
        Q_ASSERT(parenIdx != -1);
        methodName = methodName.left(parenIdx);

        if (stringCache.contains(methodName))
            continue;

        RData *data = new RData;
        data->identifier = enginePriv->objectClass->createPersistentIdentifier(methodName);

        data->load(m);

        stringCache.insert(methodName, data);
        identifierCache.insert(data->identifier.identifier, data);
        data->addref();
    }
}

QmlPropertyCache::Data *
QmlPropertyCache::property(int index) const
{
    if (index < 0 || index >= indexCache.count())
        return 0;

    return indexCache.at(index);
}

QmlPropertyCache::Data *
QmlPropertyCache::property(const QString &str) const
{
    return stringCache.value(str);
}

QString QmlPropertyCache::Data::name(QObject *object)
{
    if (!object)
        return QString();

    return name(object->metaObject());
}

QString QmlPropertyCache::Data::name(const QMetaObject *metaObject)
{
    if (!metaObject || coreIndex == -1)
        return QString();

    if (flags & IsFunction) {
        QMetaMethod m = metaObject->method(coreIndex);

        QString name = QString::fromUtf8(m.signature());
        int parenIdx = name.indexOf(QLatin1Char('('));
        if (parenIdx != -1)
            name = name.left(parenIdx);
        return name;
    } else {
        QMetaProperty p = metaObject->property(coreIndex);
        return QString::fromUtf8(p.name());
    }
}

QStringList QmlPropertyCache::propertyNames() const
{
    return stringCache.keys();
}

QmlPropertyCache::Data *QmlPropertyCache::property(QmlEngine *engine, QObject *obj, 
                                                   const QScriptDeclarativeClass::Identifier &name, Data &local)
{
    QmlPropertyCache::Data *rv = 0;

    QmlEnginePrivate *enginePrivate = QmlEnginePrivate::get(engine);

    QmlPropertyCache *cache = 0;
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
    if (ddata && ddata->propertyCache && ddata->propertyCache->qmlEngine() == engine)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePrivate->cache(obj);
        if (cache && ddata && !ddata->propertyCache) { cache->addref(); ddata->propertyCache = cache; }
    }

    if (cache) {
        rv = cache->property(name);
    } else {
        local = QmlPropertyCache::create(obj->metaObject(), enginePrivate->objectClass->toString(name));
        if (local.isValid())
            rv = &local;
    }

    return rv;
}

QmlPropertyCache::Data *QmlPropertyCache::property(QmlEngine *engine, QObject *obj, 
                                                   const QString &name, Data &local)
{
    QmlPropertyCache::Data *rv = 0;

    if (!engine) {
        local = QmlPropertyCache::create(obj->metaObject(), name);
        if (local.isValid())
            rv = &local;
    } else {
        QmlEnginePrivate *enginePrivate = QmlEnginePrivate::get(engine);

        QmlPropertyCache *cache = 0;
        QmlDeclarativeData *ddata = QmlDeclarativeData::get(obj);
        if (ddata && ddata->propertyCache && ddata->propertyCache->qmlEngine() == engine)
            cache = ddata->propertyCache;
        if (!cache) {
            cache = enginePrivate->cache(obj);
            if (cache && ddata && !ddata->propertyCache) { cache->addref(); ddata->propertyCache = cache; }
        }

        if (cache) {
            rv = cache->property(name);
        } else {
            local = QmlPropertyCache::create(obj->metaObject(), name);
            if (local.isValid())
                rv = &local;
        }
    }

    return rv;
}

QT_END_NAMESPACE
