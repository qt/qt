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

#ifndef QDECLARATIVEMETAPROPERTY_H
#define QDECLARATIVEMETAPROPERTY_H

#include <QtCore/qmetaobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QObject;
class QDeclarativeAbstractBinding;
class QDeclarativeExpression;
class QStringList;
class QVariant;
struct QMetaObject;
class QDeclarativeContext;
class QDeclarativeEngine;

class QDeclarativeMetaPropertyPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeMetaProperty
{
public:
    enum PropertyCategory {
        Unknown,
        InvalidProperty,
        Bindable,
        List,
        Object,
        Normal
    };
    QDeclarativeMetaProperty();
    QDeclarativeMetaProperty(QObject *);
    QDeclarativeMetaProperty(QObject *, const QString &);
    QDeclarativeMetaProperty(QObject *, QDeclarativeContext *);
    QDeclarativeMetaProperty(QObject *, const QString &, QDeclarativeContext *);
    QDeclarativeMetaProperty(const QDeclarativeMetaProperty &);
    QDeclarativeMetaProperty &operator=(const QDeclarativeMetaProperty &);
    ~QDeclarativeMetaProperty();

    QString name() const;

    QVariant read() const;
    bool write(const QVariant &) const;
    enum WriteFlag { BypassInterceptor = 0x01, DontRemoveBinding = 0x02 };
    Q_DECLARE_FLAGS(WriteFlags, WriteFlag)
    bool write(const QVariant &, QDeclarativeMetaProperty::WriteFlags) const;
    bool reset() const;

    bool hasChangedNotifier() const;
    bool needsChangedNotifier() const;
    bool connectNotifier(QObject *dest, const char *slot) const;
    bool connectNotifier(QObject *dest, int method) const;

    QMetaMethod method() const;

    enum Type { Invalid = 0x00, 
                Property = 0x01, 
                SignalProperty = 0x02,
                Default = 0x08,
                Attached = 0x10,
                ValueTypeProperty = 0x20 };

    Type type() const;
    bool isProperty() const;
    bool isDefault() const;
    bool isWritable() const;
    bool isDesignable() const;
    bool isResettable() const;
    bool isValid() const;
    QObject *object() const;

    PropertyCategory propertyCategory() const;

    int propertyType() const;
    const char *propertyTypeName() const;

    bool operator==(const QDeclarativeMetaProperty &) const;

    QMetaProperty property() const;

    int coreIndex() const;
    int valueTypeCoreIndex() const;
private:
    friend class QDeclarativeEnginePrivate;
    friend class QDeclarativeMetaPropertyPrivate;
    QDeclarativeMetaPropertyPrivate *d;
};
typedef QList<QDeclarativeMetaProperty> QDeclarativeMetaProperties;
 Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeMetaProperty::WriteFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEMETAPROPERTY_H
