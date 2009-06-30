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

#include "qmlopenmetaobject.h"
#include "private/qmetaobjectbuilder_p.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

class QmlOpenMetaObjectPrivate
{
public:
    QmlOpenMetaObjectPrivate() : parent(0), mem(0) {}

    bool autoCreate;
    QAbstractDynamicMetaObject *parent;
    int propertyOffset;
    int signalOffset;
    QList<QVariant> data;
    QHash<QByteArray, int> names;
    QMetaObjectBuilder mob;
    QMetaObject *mem;
    QObject *object;
};

QmlOpenMetaObject::QmlOpenMetaObject(QObject *obj, bool automatic)
: d(new QmlOpenMetaObjectPrivate)
{
    d->autoCreate = automatic;
    d->object = obj;

    d->mob.setSuperClass(obj->metaObject());
    d->mob.setClassName(obj->metaObject()->className());
    d->mob.setFlags(QMetaObjectBuilder::DynamicMetaObject);

    QObjectPrivate *op = QObjectPrivate::get(obj);
    if (op->metaObject)
        d->mob.setSuperClass(op->metaObject);

    d->mem = d->mob.toMetaObject();
    *static_cast<QMetaObject *>(this) = *d->mem;
    op->metaObject = this;
    d->propertyOffset = propertyOffset();
    d->signalOffset = methodOffset();
}

QmlOpenMetaObject::~QmlOpenMetaObject()
{
    if (d->parent)
        delete d->parent;
    qFree(d->mem);
    delete d;
}

int QmlOpenMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (( c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty)
            && id >= d->propertyOffset) {
        int propId = id - d->propertyOffset;
        if (c == QMetaObject::ReadProperty) {
            propertyRead(propId);
            *reinterpret_cast<QVariant *>(a[0]) = d->data[propId];
        } else if (c == QMetaObject::WriteProperty) {
            if (d->data[propId] != *reinterpret_cast<QVariant *>(a[0]))  {
                propertyWrite(propId);
                d->data[propId] = *reinterpret_cast<QVariant *>(a[0]);
                activate(d->object, d->signalOffset + propId, 0);
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
    Q_ASSERT(id >= 0 && id < d->data.count());
    return d->data.at(id);
}

void QmlOpenMetaObject::setValue(int id, const QVariant &value)
{
    Q_ASSERT(id >= 0 && id < d->data.count());
    d->data[id] = value;
    activate(d->object, id + d->signalOffset, 0);
}

QVariant QmlOpenMetaObject::value(const QByteArray &name) const
{
    QHash<QByteArray, int>::ConstIterator iter = d->names.find(name);
    if (iter == d->names.end())
        return QVariant();

    return d->data.at(*iter);
}

void QmlOpenMetaObject::setValue(const QByteArray &name, const QVariant &val)
{
    QHash<QByteArray, int>::ConstIterator iter = d->names.find(name);

    int id = -1;
    if (iter == d->names.end()) {
        id = doCreateProperty(name.constData()) - d->propertyOffset;
    } else {
        id = *iter;
    }

    if (d->data[id] == val)
        return;

    d->data[id] = val;
    activate(d->object, id + d->signalOffset, 0);
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
    int id = d->mob.propertyCount();
    d->mob.addSignal("__" + QByteArray::number(id) + "()");
    QMetaPropertyBuilder build = d->mob.addProperty(name, "QVariant", id);
    build.setDynamic(true);
    d->data << propertyCreated(id, build);
    qFree(d->mem);
    d->mem = d->mob.toMetaObject();
    *static_cast<QMetaObject *>(this) = *d->mem;
    d->names.insert(name, id);

    return d->propertyOffset + id;
}

void QmlOpenMetaObject::propertyRead(int)
{
}

void QmlOpenMetaObject::propertyWrite(int)
{
}

QVariant QmlOpenMetaObject::propertyCreated(int, QMetaPropertyBuilder &)
{
    return QVariant();
}

int QmlOpenMetaObject::count() const
{
    return d->data.count();
}

QByteArray QmlOpenMetaObject::name(int idx) const
{
    Q_ASSERT(idx >= 0 && idx < d->data.count());

    return d->mob.property(idx).name();
}

QObject *QmlOpenMetaObject::object() const
{
    return d->object;
}

QT_END_NAMESPACE
