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

#include "qmlvmemetaobject_p.h"
#include <qml.h>
#include <private/qmlrefcount_p.h>
#include <QColor>
#include <QDate>
#include <QtCore/qlist.h>
#include <QtCore/qdebug.h>
#include <qmlexpression.h>
#include <private/qmlcontext_p.h>

QT_BEGIN_NAMESPACE

QmlVMEMetaObject::QmlVMEMetaObject(QObject *obj,
                                   const QMetaObject *other, 
                                   const QmlVMEMetaData *meta,
                                   QmlRefCount *rc)
: object(obj), ref(rc), metaData(meta), parent(0)
{
    if (ref)
        ref->addref();

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

    // ### Optimize
    for (int ii = 0; ii < metaData->propertyCount; ++ii) {
        int t = (metaData->propertyData() + ii)->propertyType;
        if (t != -1)
            data[ii] = QVariant((QVariant::Type)t);
    }
}

QmlVMEMetaObject::~QmlVMEMetaObject()
{
    if (ref)
        ref->release();
    if (parent)
        delete parent;
    delete [] data;
}

int QmlVMEMetaObject::metaCall(QMetaObject::Call c, int _id, void **a)
{
    int id = _id;
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
                        default:
                            break;
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

                QmlContext *ctxt = qmlContext(object);

                if (!ctxt) return -1;
                QmlVMEMetaData::AliasData *d = metaData->aliasData() + id;
                QmlContextPrivate *ctxtPriv = 
                    (QmlContextPrivate *)QObjectPrivate::get(ctxt);

                QObject *target = 
                    *(QObject **)ctxtPriv->propertyValues[d->contextIdx].data();
                if (!target) return -1;

                if (c == QMetaObject::ReadProperty && !aConnected.testBit(id)) {
                    int sigIdx = methodOffset + id + metaData->propertyCount;
                    QMetaObject::connect(ctxt, d->contextIdx + ctxtPriv->notifyIndex, object, sigIdx);

                    QMetaProperty prop = 
                        target->metaObject()->property(d->propertyIdx);
                    if (prop.hasNotifySignal())
                        QMetaObject::connect(target, prop.notifySignalIndex(), 
                                             object, sigIdx);
                    aConnected.setBit(id);
                }
                return QMetaObject::metacall(target, c, d->propertyIdx, a);

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
                QmlVMEMetaData::MethodData *data = metaData->methodData() + id;
                const QChar *body = 
                    (const QChar *)(((const char*)metaData) + data->bodyOffset);

                QString code = QString::fromRawData(body, data->bodyLength);
                QmlContext *ctxt = qmlContext(object);

                if (0 == (metaData->methodData() + id)->parameterCount) {
                    QmlExpression expr(ctxt, code, object);
                    expr.setTrackChange(false);
                    expr.value();
                } else {
                    QmlContext newCtxt(ctxt);
                    QMetaMethod m = method(_id);
                    QList<QByteArray> names = m.parameterNames(); 
                    for (int ii = 0; ii < names.count(); ++ii) 
                        newCtxt.setContextProperty(names.at(ii), *(QVariant *)a[ii + 1]);
                    QmlExpression expr(&newCtxt, code, object);
                    expr.setTrackChange(false);
                    expr.value();
                }
            }
            return -1;
        }
    }

    if (parent)
        return parent->metaCall(c, _id, a);
    else
        return object->qt_metacall(c, _id, a);
}

QT_END_NAMESPACE
