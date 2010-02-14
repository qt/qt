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

#ifndef QMLMETAPROPERTY_P_H
#define QMLMETAPROPERTY_P_H

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

#include "qmlmetaproperty.h"

#include "qmlpropertycache_p.h"
#include "qmlguard_p.h"

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlContext;
class QmlEnginePrivate;
class QmlMetaPropertyPrivate
{
public:
    QmlMetaPropertyPrivate()
        : q(0), context(0), object(0), isDefaultProperty(false), isNameCached(false),
          attachedFunc(-1) {}

    QmlMetaPropertyPrivate(const QmlMetaPropertyPrivate &other)
        : q(0), context(other.context), object(other.object), 
          isDefaultProperty(other.isDefaultProperty), isNameCached(other.isNameCached),
          core(other.core), nameCache(other.nameCache),
          valueType(other.valueType), attachedFunc(other.attachedFunc) {}

    QmlMetaProperty *q;
    QmlContext *context;
    QmlGuard<QObject> object;

    bool isDefaultProperty:1;
    bool isNameCached:1;
    QmlPropertyCache::Data core;
    QString nameCache;

    // Describes the "virtual" value-type sub-property.  
    QmlPropertyCache::ValueTypeData valueType;

    // The attached property accessor
    int attachedFunc;

    void initProperty(QObject *obj, const QString &name);
    void initDefault(QObject *obj);

    QObject *attachedObject() const;
    QMetaMethod findSignal(QObject *, const QString &);

    int propertyType() const;
    QmlMetaProperty::PropertyCategory propertyCategory() const;

    QVariant readValueProperty();
    bool writeValueProperty(const QVariant &, QmlMetaProperty::WriteFlags);

    static const QMetaObject *rawMetaObjectForType(QmlEnginePrivate *, int);
    static bool writeEnumProperty(const QMetaProperty &prop, int idx, QObject *object, const QVariant &value, int flags);
    static bool write(QObject *, const QmlPropertyCache::Data &, const QVariant &, QmlContext *,
                      QmlMetaProperty::WriteFlags flags = 0);
    static QmlAbstractBinding *setBinding(QObject *, const QmlPropertyCache::Data &, QmlAbstractBinding *,
                                          QmlMetaProperty::WriteFlags flags = QmlMetaProperty::DontRemoveBinding);

    static QByteArray saveValueType(const QMetaObject *, int, int, int);
    static QByteArray saveProperty(const QMetaObject *, int);
    static QmlMetaProperty restore(const QByteArray &, QObject *, QmlContext * = 0);

    static bool equal(const QMetaObject *, const QMetaObject *);
    static bool canConvert(const QMetaObject *from, const QMetaObject *to);
};

QT_END_NAMESPACE

#endif // QMLMETAPROPERTY_P_H
