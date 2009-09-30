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

#include "qmlpropertycache_p.h"
#include "qmlengine_p.h"

QmlPropertyCache::QmlPropertyCache()
{
}

QmlPropertyCache::~QmlPropertyCache()
{
    for (int ii = 0; ii < indexCache.count(); ++ii) 
        indexCache.at(ii)->release();

    for (StringCache::ConstIterator iter = stringCache.begin(); 
            iter != stringCache.end(); ++iter)
        (*iter)->release();

    for (IdentifierCache::ConstIterator iter = identifierCache.begin(); 
            iter != identifierCache.end(); ++iter)
        (*iter)->release();
}

// ### Optimize - check engine for the parent meta object etc.
QmlPropertyCache *QmlPropertyCache::create(QmlEngine *engine, const QMetaObject *metaObject)
{
    Q_ASSERT(engine);
    Q_ASSERT(metaObject);

    QmlPropertyCache *cache = new QmlPropertyCache;

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    // ### The properties/methods should probably be spliced on a per-metaobject basis
    int propCount = metaObject->propertyCount();

    cache->indexCache.resize(propCount);
    for (int ii = propCount - 1; ii >= 0; --ii) {
        QMetaProperty p = metaObject->property(ii);
        QString propName = QLatin1String(p.name());

        QScriptDeclarativeClass::PersistentIdentifier<Data> *data = 
            enginePriv->objectClass2->createPersistentIdentifier<Data>(propName);
        data->propType = p.userType();
        data->coreIndex = ii;
        data->name = propName;

        cache->indexCache[ii] = data;

        if (cache->stringCache.contains(propName))
            continue;

        cache->stringCache.insert(propName, data);
        cache->identifierCache.insert(data->identifier, data);
        data->addref();
        data->addref();
    }

    int methodCount = metaObject->methodCount();
    for (int ii = methodCount - 1; ii >= 0; --ii) {
        QMetaMethod m = metaObject->method(ii);
        QString methodName = QLatin1String(m.signature());

        int parenIdx = methodName.indexOf(QLatin1Char('('));
        Q_ASSERT(parenIdx != -1);
        methodName = methodName.left(parenIdx);

        if (cache->stringCache.contains(methodName))
            continue;

        QScriptDeclarativeClass::PersistentIdentifier<Data> *data = 
            enginePriv->objectClass2->createPersistentIdentifier<Data>(methodName);
        cache->stringCache.insert(methodName, data);
        cache->identifierCache.insert(data->identifier, data);
        data->addref();
        data->addref();

        data->isFunction = true;
    }

    return cache;
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

