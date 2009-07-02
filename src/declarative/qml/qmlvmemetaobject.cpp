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
                                   QList<QString> *strData,
                                   int slotData,
                                   const QByteArray &alias,
                                   QmlRefCount *rc)
: object(obj), ref(rc), slotData(strData), slotDataIdx(slotData), parent(0),
  aliasData(alias), aliases(0), aliasArray(0)
{
    if (ref)
        ref->addref();

    *static_cast<QMetaObject *>(this) = *other;
    this->d.superdata = obj->metaObject();

    QObjectPrivate *op = QObjectPrivate::get(obj);
    if (op->metaObject)
        parent = static_cast<QAbstractDynamicMetaObject*>(op->metaObject);
    op->metaObject = this;

    if (!aliasData.isEmpty()) {
        aliases = (Aliases *)aliasData.constData();
        aliasArray = (AliasArray *)(aliasData.constData() + 3 * sizeof(int));
        aConnected.resize(aliases->aliasCount);
    }

    baseProp = propertyOffset();
    baseSig = methodOffset();
    int propCount = propertyCount() - (aliases?aliases->aliasCount:0);
    data = new QVariant[propCount - baseProp];
    vTypes.resize(propCount - baseProp);

    // ### Optimize
    for (int ii = baseProp; ii < propCount; ++ii) {
        QMetaProperty prop = property(ii);
        if ((int)prop.type() != -1) {
            data[ii - baseProp] = QVariant((QVariant::Type)prop.userType());
        } else {
            vTypes.setBit(ii - baseProp, true);
        }
    }

    baseSlot = -1;
    slotCount = 0;
    for (int ii = baseSig; ii < methodCount(); ++ii) {
        QMetaMethod m = method(ii);
        if (m.methodType() == QMetaMethod::Slot) {
            if (baseSlot == -1)
                baseSlot = ii;
        } else {
            if (baseSlot != -1) {
                slotCount = ii - baseSlot;
                break;
            }
        }
    }
    if(baseSlot != -1 && !slotCount)
        slotCount = methodCount() - baseSlot;
}

QmlVMEMetaObject::~QmlVMEMetaObject()
{
    if (ref)
        ref->release();
    if (parent)
        delete parent;
    delete [] data;
}

int QmlVMEMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if(c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty) {
        if (id >= baseProp) {
            int propId = id - baseProp;
            bool needActivate = false;

            if (aliases && propId >= aliases->propCount) {
                QmlContext *ctxt = qmlContext(object);

                if (!ctxt) return -1;
                int aliasId = propId - aliases->propCount;
                AliasArray *d = aliasArray + aliasId;
                QmlContextPrivate *ctxtPriv = 
                    (QmlContextPrivate *)QObjectPrivate::get(ctxt);

                QObject *target = *(QObject **)ctxtPriv->propertyValues[d->contextIdx].data();
                if (!target) return -1;

                if (c == QMetaObject::ReadProperty && 
                    !aConnected.testBit(aliasId)) {

                    int mySigIdx = baseSig + aliasId + aliases->signalOffset;
                    QMetaObject::connect(ctxt, d->contextIdx + ctxtPriv->notifyIndex, object, mySigIdx);

                    QMetaProperty prop = target->metaObject()->property(d->propIdx);
                    if (prop.hasNotifySignal())
                        QMetaObject::connect(target, prop.notifySignalIndex(), 
                                             object, mySigIdx);
                    aConnected.setBit(aliasId);

                }
                return QMetaObject::metacall(target, c, d->propIdx, a);

            } else if (vTypes.testBit(propId)) {
                if (c == QMetaObject::ReadProperty) {
                    *reinterpret_cast<QVariant *>(a[0]) = data[propId];
                } else if (c == QMetaObject::WriteProperty) {
                    needActivate = 
                        (data[propId] != *reinterpret_cast<QVariant *>(a[0]));
                    data[propId] = *reinterpret_cast<QVariant *>(a[0]);
                }
            } else {
                if (c == QMetaObject::ReadProperty) {
                    switch(data[propId].type()) {
                    case QVariant::Int:
                        *reinterpret_cast<int *>(a[0]) = data[propId].toInt();
                        break;
                    case QVariant::Bool:
                        *reinterpret_cast<bool *>(a[0]) = data[propId].toBool();
                        break;
                    case QVariant::Double:
                        *reinterpret_cast<double *>(a[0]) = data[propId].toDouble();
                        break;
                    case QVariant::String:
                        *reinterpret_cast<QString *>(a[0]) = data[propId].toString();
                        break;
                    case QVariant::Url:
                        *reinterpret_cast<QUrl *>(a[0]) = data[propId].toUrl();
                        break;
                    case QVariant::Color:
                        *reinterpret_cast<QColor *>(a[0]) = data[propId].value<QColor>();
                        break;
                    case QVariant::Date:
                        *reinterpret_cast<QDate *>(a[0]) = data[propId].toDate();
                        break;
                    default:
                        qFatal("Unknown type");
                        break;
                    }
                } else if (c == QMetaObject::WriteProperty) {

                    QVariant value = QVariant((QVariant::Type)data[propId].type(), a[0]); 
                    needActivate = (data[propId] != value);
                    data[propId] = value;
                }
            }

            if (c == QMetaObject::WriteProperty && needActivate) {
                activate(object, baseSig + propId, 0);
            }

            return id;
        } 
    } else if(c == QMetaObject::InvokeMetaMethod) {
        if (id >= baseSig && (aliases && id >= baseSig + aliases->signalOffset)) {
            QMetaObject::activate(object, id, a);
            return id;
        } else if (id >= baseSig && (baseSlot == -1 || id < baseSlot)) {
            QMetaObject::activate(object, id, a);
            return id;
        } else if (id >= baseSlot && id < (baseSlot + slotCount)) {
            int idx = id - baseSlot + slotDataIdx;
            QmlContext *ctxt = qmlContext(object);
            QmlExpression expr(ctxt, slotData->at(idx), object);
            expr.setTrackChange(false);
            expr.value();
            return id;
        }
    }

    if (parent)
        return parent->metaCall(c, id, a);
    else
        return object->qt_metacall(c, id, a);
}

QT_END_NAMESPACE
