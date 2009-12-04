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

#ifndef QMLTYPENAMECACHE_P_H
#define QMLTYPENAMECACHE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmlrefcount_p.h"
#include "qmlcleanup_p.h"

#include <private/qscriptdeclarativeclass_p.h>

QT_BEGIN_NAMESPACE

class QmlType;
class QmlEngine;
class QmlTypeNameCache : public QmlRefCount, public QmlCleanup
{
public:
    QmlTypeNameCache(QmlEngine *);
    virtual ~QmlTypeNameCache();

    struct Data {
        inline Data();
        inline ~Data();
        QmlType *type;
        QmlTypeNameCache *typeNamespace;
    };

    void add(const QString &, QmlType *);
    void add(const QString &, QmlTypeNameCache *);

    Data *data(const QString &) const;
    inline Data *data(const QScriptDeclarativeClass::Identifier &id) const;

protected:
    virtual void clear();

private:
    struct RData : public Data { 
        QScriptDeclarativeClass::PersistentIdentifier identifier;
    };
    typedef QHash<QString, RData *> StringCache;
    typedef QHash<QScriptDeclarativeClass::Identifier, RData *> IdentifierCache;

    StringCache stringCache;
    IdentifierCache identifierCache;
    QmlEngine *engine;
};

QmlTypeNameCache::Data::Data()
: type(0), typeNamespace(0)
{
}

QmlTypeNameCache::Data::~Data()
{
    if (typeNamespace) typeNamespace->release();
}

QmlTypeNameCache::Data *QmlTypeNameCache::data(const QScriptDeclarativeClass::Identifier &id) const
{
    return identifierCache.value(id);
}

QT_END_NAMESPACE

#endif // QMLTYPENAMECACHE_P_H

