/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativevaluetypescriptclass_p.h"

#include "private/qdeclarativebinding_p.h"
#include "private/qdeclarativeproperty_p.h"
#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

struct QDeclarativeValueTypeObject : public QScriptDeclarativeClass::Object {
    enum Type { Reference, Copy };
    QDeclarativeValueTypeObject(Type t) : objectType(t) {}
    Type objectType;
    QDeclarativeValueType *type;
};

struct QDeclarativeValueTypeReference : public QDeclarativeValueTypeObject {
    QDeclarativeValueTypeReference() : QDeclarativeValueTypeObject(Reference) {}
    QDeclarativeGuard<QObject> object;
    int property;
};

struct QDeclarativeValueTypeCopy : public QDeclarativeValueTypeObject {
    QDeclarativeValueTypeCopy() : QDeclarativeValueTypeObject(Copy) {}
    QVariant value;
};

QDeclarativeValueTypeScriptClass::QDeclarativeValueTypeScriptClass(QDeclarativeEngine *bindEngine)
: QScriptDeclarativeClass(QDeclarativeEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine)
{
}

QDeclarativeValueTypeScriptClass::~QDeclarativeValueTypeScriptClass()
{
}

QScriptValue QDeclarativeValueTypeScriptClass::newObject(QObject *object, int coreIndex, QDeclarativeValueType *type)
{
    QDeclarativeValueTypeReference *ref = new QDeclarativeValueTypeReference;
    ref->type = type;
    ref->object = object;
    ref->property = coreIndex;
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);
    return QScriptDeclarativeClass::newObject(scriptEngine, this, ref);
}

QScriptValue QDeclarativeValueTypeScriptClass::newObject(const QVariant &v, QDeclarativeValueType *type)
{
    QDeclarativeValueTypeCopy *copy = new QDeclarativeValueTypeCopy;
    copy->type = type;
    copy->value = v;
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);
    return QScriptDeclarativeClass::newObject(scriptEngine, this, copy);
}

QScriptClass::QueryFlags 
QDeclarativeValueTypeScriptClass::queryProperty(Object *obj, const Identifier &name, 
                                                QScriptClass::QueryFlags)
{
    QDeclarativeValueTypeObject *o = static_cast<QDeclarativeValueTypeObject *>(obj);

    m_lastIndex = -1;

    QByteArray propName = toString(name).toUtf8();

    m_lastIndex = o->type->metaObject()->indexOfProperty(propName.constData());
    if (m_lastIndex == -1)
        return 0;

    QScriptClass::QueryFlags rv = 0;

    if (o->objectType == QDeclarativeValueTypeObject::Reference) {
        QDeclarativeValueTypeReference *ref = static_cast<QDeclarativeValueTypeReference *>(o);

        if (!ref->object)
            return 0;

        QMetaProperty prop = ref->object->metaObject()->property(m_lastIndex);

        rv = QScriptClass::HandlesReadAccess;
        if (prop.isWritable())
            rv |= QScriptClass::HandlesWriteAccess;
    } else {
        rv = QScriptClass::HandlesReadAccess | QScriptClass::HandlesWriteAccess;
    }

    return rv;
}

QDeclarativeValueTypeScriptClass::Value QDeclarativeValueTypeScriptClass::property(Object *obj, const Identifier &)
{
    QDeclarativeValueTypeObject *o = static_cast<QDeclarativeValueTypeObject *>(obj);

    QVariant rv;
    if (o->objectType == QDeclarativeValueTypeObject::Reference) {
        QDeclarativeValueTypeReference *ref = static_cast<QDeclarativeValueTypeReference *>(obj);

        QMetaProperty p = ref->type->metaObject()->property(m_lastIndex);
        ref->type->read(ref->object, ref->property);
        rv = p.read(ref->type);
    } else {
        QDeclarativeValueTypeCopy *copy = static_cast<QDeclarativeValueTypeCopy *>(obj);

        QMetaProperty p = copy->type->metaObject()->property(m_lastIndex);
        copy->type->setValue(copy->value);
        rv = p.read(copy->type);
    }

    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);
    return Value(scriptEngine, static_cast<QDeclarativeEnginePrivate *>(QObjectPrivate::get(engine))->scriptValueFromVariant(rv));
}

void QDeclarativeValueTypeScriptClass::setProperty(Object *obj, const Identifier &, 
                                                   const QScriptValue &value)
{
    QDeclarativeValueTypeObject *o = static_cast<QDeclarativeValueTypeObject *>(obj);

    QVariant v = QDeclarativeEnginePrivate::get(engine)->scriptValueToVariant(value);

    if (o->objectType == QDeclarativeValueTypeObject::Reference) {
        QDeclarativeValueTypeReference *ref = static_cast<QDeclarativeValueTypeReference *>(obj);

        QDeclarativeAbstractBinding *delBinding = 
            QDeclarativePropertyPrivate::setBinding(ref->object, ref->property, m_lastIndex, 0);
        if (delBinding) 
            delBinding->destroy();

        ref->type->read(ref->object, ref->property);
        QMetaProperty p = ref->type->metaObject()->property(m_lastIndex);
        if (p.isEnumType() && (QMetaType::Type)v.type() == QMetaType::QReal) 
            v = v.toInt();
        p.write(ref->type, v);
        ref->type->write(ref->object, ref->property, 0);
    } else {
        QDeclarativeValueTypeCopy *copy = static_cast<QDeclarativeValueTypeCopy *>(obj);
        copy->type->setValue(copy->value);
        QMetaProperty p = copy->type->metaObject()->property(m_lastIndex);
        p.write(copy->type, v);
        copy->value = copy->type->value();
    }
}

QVariant QDeclarativeValueTypeScriptClass::toVariant(Object *obj, bool *ok)
{
    QDeclarativeValueTypeObject *o = static_cast<QDeclarativeValueTypeObject *>(obj);

    if (o->objectType == QDeclarativeValueTypeObject::Reference) {
        QDeclarativeValueTypeReference *ref = static_cast<QDeclarativeValueTypeReference *>(obj);

        if (ok) *ok = true;

        if (ref->object) {
            ref->type->read(ref->object, ref->property);
            return ref->type->value();
        }
    } else {
        QDeclarativeValueTypeCopy *copy = static_cast<QDeclarativeValueTypeCopy *>(obj);
        
        if (ok) *ok = true;

        return copy->value;
    }

    return QVariant();
}

QVariant QDeclarativeValueTypeScriptClass::toVariant(const QScriptValue &value)
{
    Q_ASSERT(scriptClass(value) == this);

    return toVariant(object(value), 0);
}

QT_END_NAMESPACE

