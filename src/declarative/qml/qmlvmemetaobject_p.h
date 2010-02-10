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

#ifndef QMLVMEMETAOBJECT_P_H
#define QMLVMEMETAOBJECT_P_H

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

#include "qml.h"

#include <QtCore/QMetaObject>
#include <QtCore/QBitArray>
#include <QtCore/QPair>

#include <private/qobject_p.h>

#include "qmlguard_p.h"
#include "qmlcompiler_p.h"

QT_BEGIN_NAMESPACE

#define QML_ALIAS_FLAG_PTR 0x00000001

struct QmlVMEMetaData
{
    short propertyCount;
    short aliasCount;
    short signalCount;
    short methodCount;

    struct AliasData {
        int contextIdx;
        int propertyIdx;
        int flags;
    };
    
    struct PropertyData {
        int propertyType;
    };

    struct MethodData {
        int parameterCount;
        int bodyOffset;
        int bodyLength;
        int scriptProgram;
    };

    PropertyData *propertyData() const {
        return (PropertyData *)(((const char *)this) + sizeof(QmlVMEMetaData));
    }

    AliasData *aliasData() const {
        return (AliasData *)(propertyData() + propertyCount);
    }

    MethodData *methodData() const {
        return (MethodData *)(aliasData() + aliasCount);
    }
};

class QmlRefCount;
class QmlVMEMetaObject : public QAbstractDynamicMetaObject
{
public:
    QmlVMEMetaObject(QObject *obj, const QMetaObject *other, const QmlVMEMetaData *data,
                     QmlCompiledData *compiledData);
    ~QmlVMEMetaObject();

    void registerInterceptor(int index, int valueIndex, QmlPropertyValueInterceptor *interceptor);
    QScriptValue vmeMethod(int index);
protected:
    virtual int metaCall(QMetaObject::Call _c, int _id, void **_a);

private:
    QObject *object;
    QmlCompiledData *compiledData;
    QmlGuard<QmlContext> ctxt;

    const QmlVMEMetaData *metaData;
    int propOffset;
    int methodOffset;

    QVariant *data;
    QBitArray aConnected;
    QBitArray aInterceptors;
    QHash<int, QPair<int, QmlPropertyValueInterceptor*> > interceptors;

    QScriptValue *methods;
    QScriptValue method(int);

    QAbstractDynamicMetaObject *parent;

    void listChanged(int);
    class List : public QmlConcreteList<QObject*>
    {
    public:
        List(QmlVMEMetaObject *p, int propIdx) 
            : parent(p), parentProperty(propIdx) { }

        virtual void append(QObject *v) { 
            QmlConcreteList<QObject*>::append(v); 
            parent->listChanged(parentProperty);
        }
        virtual void insert(int i, QObject *v) { 
            QmlConcreteList<QObject*>::insert(i, v); 
            parent->listChanged(parentProperty);
        }
        virtual void clear() { 
            QmlConcreteList<QObject*>::clear();
            parent->listChanged(parentProperty);
        }
        virtual void removeAt(int i) { 
            QmlConcreteList<QObject*>::removeAt(i);
            parent->listChanged(parentProperty);
        }
    private:
        QmlVMEMetaObject *parent;
        int parentProperty;
    };
    QList<List *> listProperties;
};

QT_END_NAMESPACE

#endif // QMLVMEMETAOBJECT_P_H
