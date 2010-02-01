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

#include "qmlvaluetypescriptclass_p.h"

#include "qmlengine_p.h"
#include "qmlguard_p.h"

QT_BEGIN_NAMESPACE

struct QmlValueTypeReference : public QScriptDeclarativeClass::Object {
    QmlValueType *type;
    QmlGuard<QObject> object;
    int property;
};

QmlValueTypeScriptClass::QmlValueTypeScriptClass(QmlEngine *bindEngine)
: QmlScriptClass(QmlEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine)
{
}

QmlValueTypeScriptClass::~QmlValueTypeScriptClass()
{
}

QScriptValue QmlValueTypeScriptClass::newObject(QObject *object, int coreIndex, QmlValueType *type)
{
    QmlValueTypeReference *ref = new QmlValueTypeReference;
    ref->type = type;
    ref->object = object;
    ref->property = coreIndex;
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    return QScriptDeclarativeClass::newObject(scriptEngine, this, ref);
}

QScriptClass::QueryFlags 
QmlValueTypeScriptClass::queryProperty(Object *obj, const Identifier &name, 
                                       QScriptClass::QueryFlags)
{
    QmlValueTypeReference *ref = static_cast<QmlValueTypeReference *>(obj);

    m_lastIndex = -1;

    if (!ref->object)
        return 0;

    QByteArray propName = toString(name).toUtf8();

    m_lastIndex = ref->type->metaObject()->indexOfProperty(propName.constData());
    if (m_lastIndex == -1)
        return 0;

    QMetaProperty prop = ref->object->metaObject()->property(m_lastIndex);

    QScriptClass::QueryFlags rv =
        QScriptClass::HandlesReadAccess;
    if (prop.isWritable())
        rv |= QScriptClass::HandlesWriteAccess;

    return rv;
}

QmlValueTypeScriptClass::ScriptValue QmlValueTypeScriptClass::property(Object *obj, const Identifier &)
{
    QmlValueTypeReference *ref = static_cast<QmlValueTypeReference *>(obj);

    QMetaProperty p = ref->type->metaObject()->property(m_lastIndex);
    ref->type->read(ref->object, ref->property);
    QVariant rv = p.read(ref->type);

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    return Value(scriptEngine, static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine))->scriptValueFromVariant(rv));
}

void QmlValueTypeScriptClass::setProperty(Object *obj, const Identifier &, 
                                          const QScriptValue &value)
{
    QmlValueTypeReference *ref = static_cast<QmlValueTypeReference *>(obj);

    QVariant v = QmlScriptClass::toVariant(engine, value);

    ref->type->read(ref->object, ref->property);
    QMetaProperty p = ref->type->metaObject()->property(m_lastIndex);
    p.write(ref->type, v);
    ref->type->write(ref->object, ref->property, 0);
}

QVariant QmlValueTypeScriptClass::toVariant(Object *obj, bool *ok)
{
    QmlValueTypeReference *ref = static_cast<QmlValueTypeReference *>(obj);

    if (ok) *ok = true;

    if (ref->object) {
        ref->type->read(ref->object, ref->property);
        return ref->type->value();
    } else {
        return QVariant();
    }
}

QVariant QmlValueTypeScriptClass::toVariant(const QScriptValue &value)
{
    Q_ASSERT(scriptClass(value) == this);

    return toVariant(object(value), 0);
}

QT_END_NAMESPACE

