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

#include "private/qdeclarativepropertycache_p.h"

#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativebinding_p.h"
#include <QtCore/qdebug.h>

Q_DECLARE_METATYPE(QScriptValue)

QT_BEGIN_NAMESPACE

QDeclarativePropertyCache::Data::Flags QDeclarativePropertyCache::Data::flagsForProperty(const QMetaProperty &p, QDeclarativeEngine *engine) 
{
    int propType = p.userType();

    Flags flags;

    if (p.isConstant())
        flags |= Data::IsConstant;
    if (p.isWritable())
        flags |= Data::IsWritable;
    if (p.isResettable())
        flags |= Data::IsResettable;

    if (propType == qMetaTypeId<QDeclarativeBinding *>()) {
        flags |= Data::IsQmlBinding;
    } else if (propType == qMetaTypeId<QScriptValue>()) {
        flags |= Data::IsQScriptValue;
    } else if (p.isEnumType()) {
        flags |= Data::IsEnumType;
    } else {
        QDeclarativeMetaType::TypeCategory cat = engine ? QDeclarativeEnginePrivate::get(engine)->typeCategory(propType)
                                               : QDeclarativeMetaType::typeCategory(propType);
        if (cat == QDeclarativeMetaType::Object)
            flags |= Data::IsQObjectDerived;
        else if (cat == QDeclarativeMetaType::List)
            flags |= Data::IsQList;
    }

    return flags;
}

void QDeclarativePropertyCache::Data::load(const QMetaProperty &p, QDeclarativeEngine *engine)
{
    propType = p.userType();
    if (QVariant::Type(propType) == QVariant::LastType)
        propType = qMetaTypeId<QVariant>();
    coreIndex = p.propertyIndex();
    notifyIndex = p.notifySignalIndex();
    flags = flagsForProperty(p, engine);
}

void QDeclarativePropertyCache::Data::load(const QMetaMethod &m)
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


/*!
Creates a new empty QDeclarativePropertyCache.
*/
QDeclarativePropertyCache::QDeclarativePropertyCache(QDeclarativeEngine *e)
: QDeclarativeCleanup(e), engine(e)
{
    Q_ASSERT(engine);
}

/*!
Creates a new QDeclarativePropertyCache of \a metaObject.
*/
QDeclarativePropertyCache::QDeclarativePropertyCache(QDeclarativeEngine *e, const QMetaObject *metaObject)
: QDeclarativeCleanup(e), engine(e)
{
    Q_ASSERT(engine);
    Q_ASSERT(metaObject);

    update(engine, metaObject);
}

QDeclarativePropertyCache::~QDeclarativePropertyCache()
{
    clear();
}

void QDeclarativePropertyCache::clear()
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

QDeclarativePropertyCache::Data QDeclarativePropertyCache::create(const QMetaObject *metaObject, 
                                                                  const QString &property)
{
    Q_ASSERT(metaObject);

    QDeclarativePropertyCache::Data rv;
    int idx = metaObject->indexOfProperty(property.toUtf8());
    if (idx != -1) {
        rv.load(metaObject->property(idx));
        return rv;
    }

    int methodCount = metaObject->methodCount();
    for (int ii = methodCount - 1; ii >= 2; --ii) { // >=2 to block the destroyed signal
        QMetaMethod m = metaObject->method(ii);
        if (m.access() == QMetaMethod::Private)
            continue;
        QString methodName = QString::fromUtf8(m.signature());

        int parenIdx = methodName.indexOf(QLatin1Char('('));
        Q_ASSERT(parenIdx != -1);
        QStringRef methodNameRef = methodName.leftRef(parenIdx);

        if (methodNameRef == property) {
            rv.load(m);
            return rv;
        }
    }

    return rv;
}

QDeclarativePropertyCache *QDeclarativePropertyCache::copy() const
{
    QDeclarativePropertyCache *cache = new QDeclarativePropertyCache(engine);
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

void QDeclarativePropertyCache::append(QDeclarativeEngine *engine, const QMetaObject *metaObject, 
                              Data::Flag propertyFlags, Data::Flag methodFlags)
{
    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);

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
    int methodOffset = qMax(2, metaObject->methodOffset()); // 2 to block the destroyed signal
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

void QDeclarativePropertyCache::update(QDeclarativeEngine *engine, const QMetaObject *metaObject)
{
    Q_ASSERT(engine);
    Q_ASSERT(metaObject);
    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);

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
    for (int ii = methodCount - 1; ii >= 2; --ii) { // >=2 to block the destroyed signal
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

QDeclarativePropertyCache::Data *
QDeclarativePropertyCache::property(int index) const
{
    if (index < 0 || index >= indexCache.count())
        return 0;

    return indexCache.at(index);
}

QDeclarativePropertyCache::Data *
QDeclarativePropertyCache::property(const QString &str) const
{
    return stringCache.value(str);
}

QString QDeclarativePropertyCache::Data::name(QObject *object)
{
    if (!object)
        return QString();

    return name(object->metaObject());
}

QString QDeclarativePropertyCache::Data::name(const QMetaObject *metaObject)
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

QStringList QDeclarativePropertyCache::propertyNames() const
{
    return stringCache.keys();
}

QDeclarativePropertyCache::Data *QDeclarativePropertyCache::property(QDeclarativeEngine *engine, QObject *obj, 
                                                   const QScriptDeclarativeClass::Identifier &name, Data &local)
{
    QDeclarativePropertyCache::Data *rv = 0;

    QDeclarativeEnginePrivate *enginePrivate = QDeclarativeEnginePrivate::get(engine);

    QDeclarativePropertyCache *cache = 0;
    QDeclarativeData *ddata = QDeclarativeData::get(obj);
    if (ddata && ddata->propertyCache && ddata->propertyCache->qmlEngine() == engine)
        cache = ddata->propertyCache;
    if (!cache) {
        cache = enginePrivate->cache(obj);
        if (cache && ddata && !ddata->propertyCache) { cache->addref(); ddata->propertyCache = cache; }
    }

    if (cache) {
        rv = cache->property(name);
    } else {
        local = QDeclarativePropertyCache::create(obj->metaObject(), enginePrivate->objectClass->toString(name));
        if (local.isValid())
            rv = &local;
    }

    return rv;
}

QDeclarativePropertyCache::Data *QDeclarativePropertyCache::property(QDeclarativeEngine *engine, QObject *obj, 
                                                   const QString &name, Data &local)
{
    QDeclarativePropertyCache::Data *rv = 0;

    if (!engine) {
        local = QDeclarativePropertyCache::create(obj->metaObject(), name);
        if (local.isValid())
            rv = &local;
    } else {
        QDeclarativeEnginePrivate *enginePrivate = QDeclarativeEnginePrivate::get(engine);

        QDeclarativePropertyCache *cache = 0;
        QDeclarativeData *ddata = QDeclarativeData::get(obj);
        if (ddata && ddata->propertyCache && ddata->propertyCache->qmlEngine() == engine)
            cache = ddata->propertyCache;
        if (!cache) {
            cache = enginePrivate->cache(obj);
            if (cache && ddata && !ddata->propertyCache) { cache->addref(); ddata->propertyCache = cache; }
        }

        if (cache) {
            rv = cache->property(name);
        } else {
            local = QDeclarativePropertyCache::create(obj->metaObject(), name);
            if (local.isValid())
                rv = &local;
        }
    }

    return rv;
}

QT_END_NAMESPACE
