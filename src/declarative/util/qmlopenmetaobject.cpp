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
#include <QDebug>


QT_BEGIN_NAMESPACE
QmlOpenMetaObject::QmlOpenMetaObject(QObject *obj, bool automatic)
: autoCreate(automatic), parent(0), mem(0), _object(obj)
{
    mob.setSuperClass(obj->metaObject());
    mob.setFlags(QMetaObjectBuilder::DynamicMetaObject);

    QObjectPrivate *op = QObjectPrivate::get(obj);
    if (op->metaObject)
        mob.setSuperClass(op->metaObject);

    mem = mob.toMetaObject();
    *static_cast<QMetaObject *>(this) = *mem;
    op->metaObject = this;
    _propertyOffset = propertyOffset();
    _signalOffset = methodOffset();
}

QmlOpenMetaObject::~QmlOpenMetaObject()
{
    if (parent)
        delete parent;
    qFree(mem);
}

int QmlOpenMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (( c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty)
            && id >= _propertyOffset) {
        int propId = id - _propertyOffset;
        if (c == QMetaObject::ReadProperty) {
            propertyRead(propId);
            *reinterpret_cast<QVariant *>(a[0]) = data[propId];
        } else if (c == QMetaObject::WriteProperty) {
            if (data[propId] != *reinterpret_cast<QVariant *>(a[0]))  {
                propertyWrite(propId);
                data[propId] = *reinterpret_cast<QVariant *>(a[0]);
                activate(_object, _signalOffset + propId, 0);
            }
        } 
        return -1;
    } else {
        if (parent)
            return parent->metaCall(c, id, a);
        else
            return _object->qt_metacall(c, id, a);
    }
}

QVariant QmlOpenMetaObject::value(int id) const
{
    Q_ASSERT(id >= 0 && id < data.count());
    return data.at(id);
}

void QmlOpenMetaObject::setValue(int id, const QVariant &value)
{
    Q_ASSERT(id >= 0 && id < data.count());
    data[id] = value;
    activate(_object, id + _signalOffset, 0);
}

QVariant QmlOpenMetaObject::value(const QByteArray &name) const
{
    QHash<QByteArray, int>::ConstIterator iter = names.find(name);
    if (iter == names.end())
        return QVariant();

    return data.at(*iter);
}

void QmlOpenMetaObject::setValue(const QByteArray &name, const QVariant &val)
{
    QHash<QByteArray, int>::ConstIterator iter = names.find(name);

    int id = -1;
    if (iter == names.end()) {
        id = doCreateProperty(name.constData()) - _propertyOffset;
    } else {
        id = *iter;
    }

    if (data[id] == val)
        return;

    data[id] = val;
    activate(_object, id + _signalOffset, 0);
}

int QmlOpenMetaObject::createProperty(const char *name, const char *)
{
    if (autoCreate) 
        return doCreateProperty(name);
    else
        return -1;
}

int QmlOpenMetaObject::doCreateProperty(const char *name)
{
    int id = mob.propertyCount();
    mob.addSignal("__" + QByteArray::number(id) + "()");
    QMetaPropertyBuilder build = mob.addProperty(name, "QVariant", id);
    build.setDynamic(true);
    data << propertyCreated(id, build);
    qFree(mem);
    mem = mob.toMetaObject();
    *static_cast<QMetaObject *>(this) = *mem;
    names.insert(name, id);

    return _propertyOffset + id;
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
    return data.count();
}

QByteArray QmlOpenMetaObject::name(int idx) const
{
    Q_ASSERT(idx >= 0 && idx < data.count());

    return mob.property(idx).name();
}

QObject *QmlOpenMetaObject::object() const
{
    return _object;
}

QT_END_NAMESPACE
