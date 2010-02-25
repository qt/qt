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

#include "qdeclarativevmemetaobject_p.h"

#include "qdeclarative.h"
#include "qdeclarativerefcount_p.h"
#include "qdeclarativeexpression.h"
#include "qdeclarativeexpression_p.h"
#include "qdeclarativecontext_p.h"

#include <QColor>
#include <QDate>
#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QDeclarativeVMEMetaObject::QDeclarativeVMEMetaObject(QObject *obj,
                                   const QMetaObject *other, 
                                   const QDeclarativeVMEMetaData *meta,
                                   QDeclarativeCompiledData *cdata)
: object(obj), compiledData(cdata), ctxt(qmlContext(obj)), metaData(meta), methods(0),
  parent(0)
{
    compiledData->addref();

    *static_cast<QMetaObject *>(this) = *other;
    this->d.superdata = obj->metaObject();

    QObjectPrivate *op = QObjectPrivate::get(obj);
    if (op->metaObject)
        parent = static_cast<QAbstractDynamicMetaObject*>(op->metaObject);
    op->metaObject = this;

    propOffset = QAbstractDynamicMetaObject::propertyOffset();
    methodOffset = QAbstractDynamicMetaObject::methodOffset();

    data = new QVariant[metaData->propertyCount];
    aConnected.resize(metaData->aliasCount);

    int list_type = qMetaTypeId<QDeclarativeListProperty<QObject> >();
    // ### Optimize
    for (int ii = 0; ii < metaData->propertyCount; ++ii) {
        int t = (metaData->propertyData() + ii)->propertyType;
        if (t == list_type) {
            listProperties.append(new List(methodOffset + ii));
            data[ii] = QVariant::fromValue(QDeclarativeListProperty<QObject>(obj, listProperties.last(), list_append,
                                                                    list_count, list_at, list_clear));
        } else if (t != -1) {
            data[ii] = QVariant((QVariant::Type)t);
        }
    }
}

QDeclarativeVMEMetaObject::~QDeclarativeVMEMetaObject()
{
    compiledData->release();
    delete parent;
    qDeleteAll(listProperties);
    delete [] data;
    delete [] methods;
}

int QDeclarativeVMEMetaObject::metaCall(QMetaObject::Call c, int _id, void **a)
{
    int id = _id;
    if(c == QMetaObject::WriteProperty) {
        int flags = *reinterpret_cast<int*>(a[3]);
        if (!(flags & QDeclarativeMetaPropertyPrivate::BypassInterceptor)
            && !aInterceptors.isEmpty()
            && aInterceptors.testBit(id)) {
            QPair<int, QDeclarativePropertyValueInterceptor*> pair = interceptors.value(id);
            int valueIndex = pair.first;
            QDeclarativePropertyValueInterceptor *vi = pair.second;
            int type = property(id).userType();

            if (type != QVariant::Invalid) {
                if (valueIndex != -1) {
                    QDeclarativeEnginePrivate *ep = ctxt?QDeclarativeEnginePrivate::get(ctxt->engine()):0;
                    QDeclarativeValueType *valueType = 0;
                    if (ep) valueType = ep->valueTypes[type];
                    else valueType = QDeclarativeValueTypeFactory::valueType(type);
                    Q_ASSERT(valueType);

                    valueType->setValue(QVariant(type, a[0]));
                    QMetaProperty valueProp = valueType->metaObject()->property(valueIndex);
                    vi->write(valueProp.read(valueType));

                    if (!ep) delete valueType;
                    return -1;
                } else {
                    vi->write(QVariant(type, a[0]));
                    return -1;
                }
            }
        }
    }
    if(c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty) {
        if (id >= propOffset) {
            id -= propOffset;

            if (id < metaData->propertyCount) {
                int t = (metaData->propertyData() + id)->propertyType;
                bool needActivate = false;

                if (t == -1) {

                    if (c == QMetaObject::ReadProperty) {
                        *reinterpret_cast<QVariant *>(a[0]) = data[id];
                    } else if (c == QMetaObject::WriteProperty) {
                        needActivate = 
                            (data[id] != *reinterpret_cast<QVariant *>(a[0]));
                        data[id] = *reinterpret_cast<QVariant *>(a[0]);
                    }

                } else {

                    if (c == QMetaObject::ReadProperty) {
                        switch(t) {
                        case QVariant::Int:
                            *reinterpret_cast<int *>(a[0]) = data[id].toInt();
                            break;
                        case QVariant::Bool:
                            *reinterpret_cast<bool *>(a[0]) = data[id].toBool();
                            break;
                        case QVariant::Double:
                            *reinterpret_cast<double *>(a[0]) = data[id].toDouble();
                            break;
                        case QVariant::String:
                            *reinterpret_cast<QString *>(a[0]) = data[id].toString();
                            break;
                        case QVariant::Url:
                            *reinterpret_cast<QUrl *>(a[0]) = data[id].toUrl();
                            break;
                        case QVariant::Color:
                            *reinterpret_cast<QColor *>(a[0]) = data[id].value<QColor>();
                            break;
                        case QVariant::Date:
                            *reinterpret_cast<QDate *>(a[0]) = data[id].toDate();
                            break;
                        case QMetaType::QObjectStar:
                            *reinterpret_cast<QObject **>(a[0]) = data[id].value<QObject*>();
                            break;
                        default:
                            break;
                        }
                        if (t == qMetaTypeId<QDeclarativeListProperty<QObject> >()) {
                            *reinterpret_cast<QDeclarativeListProperty<QObject> *>(a[0]) = 
                                data[id].value<QDeclarativeListProperty<QObject> >();
                        }

                    } else if (c == QMetaObject::WriteProperty) {

                        QVariant value = QVariant((QVariant::Type)data[id].type(), a[0]); 
                        needActivate = (data[id] != value);
                        data[id] = value;
                    }

                }

                if (c == QMetaObject::WriteProperty && needActivate) {
                    activate(object, methodOffset + id, 0);
                }

                return -1;
            }

            id -= metaData->propertyCount;

            if (id < metaData->aliasCount) {

                QDeclarativeVMEMetaData::AliasData *d = metaData->aliasData() + id;

                if (d->flags & QML_ALIAS_FLAG_PTR && c == QMetaObject::ReadProperty) 
                        *reinterpret_cast<void **>(a[0]) = 0;

                if (!ctxt) return -1;
                QDeclarativeContextPrivate *ctxtPriv = 
                    (QDeclarativeContextPrivate *)QObjectPrivate::get(ctxt);

                QObject *target = ctxtPriv->idValues[d->contextIdx].data();
                if (!target) 
                    return -1;

                if (c == QMetaObject::ReadProperty && !aConnected.testBit(id)) {
                    int sigIdx = methodOffset + id + metaData->propertyCount;
                    QMetaObject::connect(ctxt, d->contextIdx + ctxtPriv->notifyIndex, object, sigIdx);

                    if (d->propertyIdx != -1) {
                        QMetaProperty prop = 
                            target->metaObject()->property(d->propertyIdx);
                        if (prop.hasNotifySignal())
                            QMetaObject::connect(target, prop.notifySignalIndex(), 
                                                 object, sigIdx);
                    }
                    aConnected.setBit(id);
                }

                if (d->propertyIdx == -1) {
                    *reinterpret_cast<QObject **>(a[0]) = target;
                    return -1;
                } else {
                    return QMetaObject::metacall(target, c, d->propertyIdx, a);
                }

            }
            return -1;

        }

    } else if(c == QMetaObject::InvokeMetaMethod) {

        if (id >= methodOffset) {

            id -= methodOffset;
            int plainSignals = metaData->signalCount + metaData->propertyCount +
                               metaData->aliasCount;
            if (id < plainSignals) {
                QMetaObject::activate(object, _id, a);
                return -1;
            }

            id -= plainSignals;

            if (id < metaData->methodCount) {
                if (!ctxt->engine())
                    return -1; // We can't run the method

                QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(ctxt->engine());

                QScriptValue function = method(id);

                QScriptValueList args;
                QDeclarativeVMEMetaData::MethodData *data = metaData->methodData() + id;
                if (data->parameterCount) {
                    for (int ii = 0; ii < data->parameterCount; ++ii) {
                        args << ep->scriptValueFromVariant(*(QVariant *)a[ii + 1]);
                    }
                }
                QScriptValue rv = function.call(ep->objectClass->newQObject(object), args);

                if (a[0]) *reinterpret_cast<QVariant *>(a[0]) = ep->scriptValueToVariant(rv);

                return -1;
            }
            return -1;
        }
    }

    if (parent)
        return parent->metaCall(c, _id, a);
    else
        return object->qt_metacall(c, _id, a);
}

QScriptValue QDeclarativeVMEMetaObject::method(int index)
{
    if (!methods) 
        methods = new QScriptValue[metaData->methodCount];

    if (!methods[index].isValid()) {
        QDeclarativeVMEMetaData::MethodData *data = metaData->methodData() + index;

        const QChar *body = 
            (const QChar *)(((const char*)metaData) + data->bodyOffset);

        QString code = QString::fromRawData(body, data->bodyLength);

        // XXX Use QScriptProgram
        // XXX We should evaluate all methods in a single big script block to 
        // improve the call time between dynamic methods defined on the same
        // object
        methods[index] = QDeclarativeExpressionPrivate::evalInObjectScope(ctxt, object, code);
    }

    return methods[index];
}

void QDeclarativeVMEMetaObject::listChanged(int id)
{
    activate(object, methodOffset + id, 0);
}

void QDeclarativeVMEMetaObject::list_append(QDeclarativeListProperty<QObject> *prop, QObject *o)
{
    List *list = static_cast<List *>(prop->data);
    list->append(o);
    QMetaObject::activate(prop->object, list->notifyIndex, 0);
}

int QDeclarativeVMEMetaObject::list_count(QDeclarativeListProperty<QObject> *prop)
{
    return static_cast<List *>(prop->data)->count();
}

QObject *QDeclarativeVMEMetaObject::list_at(QDeclarativeListProperty<QObject> *prop, int index)
{
    return static_cast<List *>(prop->data)->at(index);
}

void QDeclarativeVMEMetaObject::list_clear(QDeclarativeListProperty<QObject> *prop)
{
    List *list = static_cast<List *>(prop->data);
    list->clear();
    QMetaObject::activate(prop->object, list->notifyIndex, 0);
}

void QDeclarativeVMEMetaObject::registerInterceptor(int index, int valueIndex, QDeclarativePropertyValueInterceptor *interceptor)
{
    if (aInterceptors.isEmpty())
        aInterceptors.resize(propertyCount() + metaData->propertyCount);
    aInterceptors.setBit(index);
    interceptors.insert(index, qMakePair(valueIndex, interceptor));
}

QScriptValue QDeclarativeVMEMetaObject::vmeMethod(int index)
{
    if (index < methodOffset) {
        Q_ASSERT(parent);
        return static_cast<QDeclarativeVMEMetaObject *>(parent)->vmeMethod(index);
    }
    int plainSignals = metaData->signalCount + metaData->propertyCount + metaData->aliasCount;
    Q_ASSERT(index >= (methodOffset + plainSignals) && index < (methodOffset + plainSignals + metaData->methodCount));
    return method(index - methodOffset - plainSignals);
}

QT_END_NAMESPACE
