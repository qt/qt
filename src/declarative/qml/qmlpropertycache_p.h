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

#ifndef QMLPROPERTYCACHE_P_H
#define QMLPROPERTYCACHE_P_H

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

#include <private/qmlrefcount_p.h>
#include <private/qscriptdeclarativeclass_p.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QmlEngine;
class QMetaProperty;
class QmlPropertyCache : public QmlRefCount
{
public:
    QmlPropertyCache();
    virtual ~QmlPropertyCache();

    struct Data {
        inline Data(); 
        inline bool operator==(const Data &);

        enum Flag { 
                    // Can apply to all properties, except IsFunction
                    IsConstant = 0x00000004,

                    // These are mutually exclusive
                    IsFunction = 0x00000001, 
                    IsQObjectDerived = 0x00000002,
                    IsEnumType = 0x00000008,
                    IsQmlList = 0x00000010,
                    IsQList = 0x00000020,
                    IsQmlBinding = 0x00000040 
        };
        Q_DECLARE_FLAGS(Flags, Flag)
                        
        bool isValid() const { return coreIndex != -1; } 

        Flags flags;
        int propType;
        int coreIndex;
        int notifyIndex;
        QString name;

        void load(const QMetaProperty &);
        void load(const QMetaMethod &);
    };

#if 0
    struct ValueTypeData {
        int valueTypeCoreIdx;  // The prop index of the access property on the value type wrapper
        int valueTypePropType; // The QVariant::Type of access property on the value type wrapper
    };
#endif

    static QmlPropertyCache *create(QmlEngine *, const QMetaObject *);

    inline Data *property(const QScriptDeclarativeClass::Identifier &id) const;
    Data *property(const QString &) const;
    Data *property(int) const;

private:
    struct RData : public Data, public QmlRefCount {};

    typedef QVector<QScriptDeclarativeClass::PersistentIdentifier<RData> *> IndexCache;
    typedef QHash<QString, QScriptDeclarativeClass::PersistentIdentifier<RData> *> StringCache;
    typedef QHash<QScriptDeclarativeClass::Identifier, QScriptDeclarativeClass::PersistentIdentifier<RData> *> IdentifierCache;

    IndexCache indexCache;
    StringCache stringCache;
    IdentifierCache identifierCache;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QmlPropertyCache::Data::Flags);
  
QmlPropertyCache::Data::Data()
: flags(0), propType(0), coreIndex(-1), notifyIndex(-1) 
{
}

bool QmlPropertyCache::Data::operator==(const QmlPropertyCache::Data::Data &other)
{
    return flags == other.flags &&
           propType == other.propType &&
           coreIndex == other.coreIndex &&
           notifyIndex == other.notifyIndex &&
           name == other.name;
}

QmlPropertyCache::Data *
QmlPropertyCache::property(const QScriptDeclarativeClass::Identifier &id) const 
{
    return identifierCache.value(id);
}

QT_END_NAMESPACE

#endif // QMLPROPERTYCACHE_P_H
