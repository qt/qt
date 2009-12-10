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

#include "qmlopenmetaobject_p.h"
#include "qmlpropertycache_p.h"
#include "qmldeclarativedata_p.h"
#include <qmetaobjectbuilder_p.h>
#include <QDebug>

QT_BEGIN_NAMESPACE


class QmlOpenMetaObjectTypePrivate
{
public:
    QmlOpenMetaObjectTypePrivate() : mem(0), engine(0) {}

    void init(QObject *obj);

    int propertyOffset;
    int signalOffset;
    QHash<QByteArray, int> names;
    QMetaObjectBuilder mob;
    QMetaObject *mem;
    QmlEngine *engine;
};

QmlOpenMetaObjectType::QmlOpenMetaObjectType(QmlEngine *engine)
    : d(new QmlOpenMetaObjectTypePrivate)
{
    d->engine = engine;
}

QmlOpenMetaObjectType::~QmlOpenMetaObjectType()
{
    qFree(d->mem);
}

void QmlOpenMetaObjectTypePrivate::init(QObject *obj)
{
    if (!mem) {
        mob.setSuperClass(obj->metaObject());
        mob.setClassName(obj->metaObject()->className());
        mob.setFlags(QMetaObjectBuilder::DynamicMetaObject);

        QObjectPrivate *op = QObjectPrivate::get(obj);
        if (op->metaObject)
            mob.setSuperClass(op->metaObject);
        mem = mob.toMetaObject();

        propertyOffset = mem->propertyOffset();
        signalOffset = mem->methodOffset();
    }
}

class QmlOpenMetaObjectPrivate
{
public:
    QmlOpenMetaObjectPrivate(QmlOpenMetaObject *_q) : q(_q), parent(0), type(0), ownType(false) {}

    inline QVariant &getData(int idx) {
        while (data.count() <= idx)
            data << QPair<QVariant, bool>(QVariant(), false);
        QPair<QVariant, bool> &prop = data[idx];
        if (!prop.second) {
            prop.first = q->initialValue(idx);
            prop.second = true;
        }
        return prop.first;
    }

    inline void writeData(int idx, const QVariant &value) {
        while (data.count() <= idx)
            data << QPair<QVariant, bool>(QVariant(), false);
        QPair<QVariant, bool> &prop = data[idx];
        prop.first = value;
        prop.second = true;
    }

    bool autoCreate;
    QmlOpenMetaObject *q;
    QAbstractDynamicMetaObject *parent;
    QList<QPair<QVariant, bool> > data;
    QObject *object;
    QmlOpenMetaObjectType *type;
    bool ownType;
};

QmlOpenMetaObject::QmlOpenMetaObject(QObject *obj, bool automatic)
: d(new QmlOpenMetaObjectPrivate(this))
{
    d->autoCreate = automatic;
    d->object = obj;

    d->type = new QmlOpenMetaObjectType(0);
    d->ownType = true;
    d->type->d->init(obj);

    QObjectPrivate *op = QObjectPrivate::get(obj);
    *static_cast<QMetaObject *>(this) = *d->type->d->mem;
    op->metaObject = this;
}

QmlOpenMetaObject::QmlOpenMetaObject(QObject *obj, QmlOpenMetaObjectType *type, bool automatic)
: d(new QmlOpenMetaObjectPrivate(this))
{
    d->autoCreate = automatic;
    d->object = obj;

    d->type = type;
    d->type->d->init(obj);

    QObjectPrivate *op = QObjectPrivate::get(obj);
    *static_cast<QMetaObject *>(this) = *d->type->d->mem;
    op->metaObject = this;
}

QmlOpenMetaObject::~QmlOpenMetaObject()
{
    if (d->parent)
        delete d->parent;
    if (d->ownType)
        delete d->type;
    delete d;
}

int QmlOpenMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (( c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty)
            && id >= d->type->d->propertyOffset) {
        int propId = id - d->type->d->propertyOffset;
        if (c == QMetaObject::ReadProperty) {
            propertyRead(propId);
            *reinterpret_cast<QVariant *>(a[0]) = d->getData(propId);
        } else if (c == QMetaObject::WriteProperty) {
            if (d->data[propId].first != *reinterpret_cast<QVariant *>(a[0]))  {
                propertyWrite(propId);
                d->writeData(propId, *reinterpret_cast<QVariant *>(a[0]));
                activate(d->object, d->type->d->signalOffset + propId, 0);
            }
        } 
        return -1;
    } else {
        if (d->parent)
            return d->parent->metaCall(c, id, a);
        else
            return d->object->qt_metacall(c, id, a);
    }
}

QVariant QmlOpenMetaObject::value(int id) const
{
    return d->getData(id);
}

void QmlOpenMetaObject::setValue(int id, const QVariant &value)
{
    d->writeData(id, value);
    activate(d->object, id + d->type->d->signalOffset, 0);
}

QVariant QmlOpenMetaObject::value(const QByteArray &name) const
{
    QHash<QByteArray, int>::ConstIterator iter = d->type->d->names.find(name);
    if (iter == d->type->d->names.end())
        return QVariant();

    return d->getData(*iter);
}

QVariant &QmlOpenMetaObject::operator[](const QByteArray &name)
{
    QHash<QByteArray, int>::ConstIterator iter = d->type->d->names.find(name);
    Q_ASSERT(iter != d->type->d->names.end());

    return d->getData(*iter);
}

void QmlOpenMetaObject::setValue(const QByteArray &name, const QVariant &val)
{
    QHash<QByteArray, int>::ConstIterator iter = d->type->d->names.find(name);

    int id = -1;
    if (iter == d->type->d->names.end()) {
        id = doCreateProperty(name.constData()) - d->type->d->propertyOffset;
    } else {
        id = *iter;
    }

    QVariant &dataVal = d->getData(id);
    if (dataVal == val)
        return;

    dataVal = val;
    activate(d->object, id + d->type->d->signalOffset, 0);
}

int QmlOpenMetaObject::createProperty(const char *name, const char *)
{
    if (d->autoCreate)
        return doCreateProperty(name);
    else
        return -1;
}

int QmlOpenMetaObject::doCreateProperty(const char *name)
{
    int id = d->type->d->mob.propertyCount();
    d->type->d->mob.addSignal("__" + QByteArray::number(id) + "()");
    QMetaPropertyBuilder build = d->type->d->mob.addProperty(name, "QVariant", id);
    build.setDynamic(true);
    propertyCreated(id, build);
    qFree(d->type->d->mem);
    d->type->d->mem = d->type->d->mob.toMetaObject();
    *static_cast<QMetaObject *>(this) = *d->type->d->mem;
    d->type->d->names.insert(name, id);

    return d->type->d->propertyOffset + id;
}

void QmlOpenMetaObject::propertyRead(int)
{
}

void QmlOpenMetaObject::propertyWrite(int)
{
}

void QmlOpenMetaObject::propertyCreated(int id, QMetaPropertyBuilder &)
{
}

QVariant QmlOpenMetaObject::initialValue(int)
{
    return QVariant();
}

int QmlOpenMetaObject::count() const
{
    return d->type->d->names.count();
}

QByteArray QmlOpenMetaObject::name(int idx) const
{
    Q_ASSERT(idx >= 0 && idx < d->type->d->names.count());

    return d->type->d->mob.property(idx).name();
}

QObject *QmlOpenMetaObject::object() const
{
    return d->object;
}

QT_END_NAMESPACE
