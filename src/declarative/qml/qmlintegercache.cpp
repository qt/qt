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

#include "qmlintegercache_p.h"
#include <private/qmlengine_p.h>
#include <QtDeclarative/qmlmetatype.h>

QT_BEGIN_NAMESPACE

QmlIntegerCache::QmlIntegerCache(QmlEngine *e)
: QmlCleanup(e), engine(e)
{
}

QmlIntegerCache::~QmlIntegerCache()
{
    clear();
}

void QmlIntegerCache::clear()
{
    qDeleteAll(stringCache);
    stringCache.clear();
    identifierCache.clear();
    engine = 0;
}

void QmlIntegerCache::add(const QString &id, int value)
{
    Q_ASSERT(!stringCache.contains(id));

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    // ### use contextClass
    Data *d = new Data(enginePriv->objectClass->createPersistentIdentifier(id), value);

    stringCache.insert(id, d);
    identifierCache.insert(d->identifier, d);
}

int QmlIntegerCache::value(const QString &id)
{
    Data *d = stringCache.value(id);
    return d?d->value:-1;
}

QmlIntegerCache *QmlIntegerCache::createForEnums(QmlType *type, QmlEngine *engine)
{
    Q_ASSERT(type);
    Q_ASSERT(engine);

    QmlIntegerCache *cache = new QmlIntegerCache(engine);

    const QMetaObject *mo = type->metaObject();

    for (int ii = mo->enumeratorCount() - 1; ii >= 0; --ii) {
        QMetaEnum enumerator = mo->enumerator(ii);

        for (int jj = 0; jj < enumerator.keyCount(); ++jj) {
            QString name = QString::fromUtf8(enumerator.key(jj));
            int value = enumerator.value(jj);

            if (!name.at(0).isUpper())
                continue;

            if (cache->stringCache.contains(name))
                continue;

            cache->add(name, value);
        }
    }

    return cache;
}

QT_END_NAMESPACE
