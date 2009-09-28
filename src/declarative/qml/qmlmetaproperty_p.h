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
#include <private/qobject_p.h>
#include <private/qmlpropertycache_p.h>

QT_BEGIN_NAMESPACE

class QmlContext;
class QmlMetaPropertyPrivate
{
public:
    QmlMetaPropertyPrivate()
        : q(0), context(0), object(0), isDefaultProperty(false), valueTypeCoreIdx(-1), 
          valueTypePropType(0), attachedFunc(-1) {}

    QmlMetaPropertyPrivate(const QmlMetaPropertyPrivate &other)
        : q(0), context(other.context), object(other.object), 
          isDefaultProperty(other.isDefaultProperty), core(other.core), 
          valueTypeCoreIdx(other.valueTypeCoreIdx), 
          valueTypePropType(other.valueTypePropType), attachedFunc(other.attachedFunc) {}

    QmlMetaProperty *q;
    QmlContext *context;
    QGuard<QObject> object;

    bool isDefaultProperty;
    QmlPropertyCache::Data core;

    // Describes the "virtual" value-type sub-property.  
    int valueTypeCoreIdx;  // The prop index of the access property on the value type wrapper
    int valueTypePropType; // The QVariant::Type of access property on the value type wrapper

    // The attached property accessor
    int attachedFunc;

    void initProperty(QObject *obj, const QString &name);
    void initDefault(QObject *obj);

    QObject *attachedObject() const;
    QMetaMethod findSignal(QObject *, const QString &);

    int propertyType() const;
    QmlMetaProperty::PropertyCategory propertyCategory() const;

    void writeSignalProperty(const QVariant &);

    QVariant readValueProperty();
    void writeValueProperty(const QVariant &, QmlMetaProperty::WriteFlags);
    static bool writeEnumProperty(const QMetaProperty &prop, int idx, QObject *object, const QVariant &value, int flags);
    static void write(QObject *, const QmlPropertyCache::Data &, const QVariant &, QmlContext *,
                      QmlMetaProperty::WriteFlags flags = 0);
    static QmlAbstractBinding *setBinding(QObject *, const QmlPropertyCache::Data &, QmlAbstractBinding *,
                                          QmlMetaProperty::WriteFlags flags = QmlMetaProperty::DontRemoveBinding);

    static quint32 saveValueType(int, int);
    static quint32 saveProperty(int);
};

QT_END_NAMESPACE

#endif // QMLMETAPROPERTY_P_H
