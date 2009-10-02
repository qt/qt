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

#include "qmlvaluetypescriptclass_p.h"
#include <private/qmlengine_p.h>

QT_BEGIN_NAMESPACE

struct QmlValueTypeReference {
    QmlValueType *type;
    QGuard<QObject> object;
    int property;
};
Q_DECLARE_METATYPE(QmlValueTypeReference);

QmlValueTypeScriptClass::QmlValueTypeScriptClass(QmlEngine *bindEngine)
: QScriptClass(QmlEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine)
{
}

QmlValueTypeScriptClass::~QmlValueTypeScriptClass()
{
}

QScriptValue QmlValueTypeScriptClass::newObject(QObject *object, int coreIndex, QmlValueType *type)
{
    QmlValueTypeReference ref = { type, object, coreIndex };
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    return scriptEngine->newObject(this, scriptEngine->newVariant(qVariantFromValue(ref)));
}

QmlValueTypeScriptClass::QueryFlags
QmlValueTypeScriptClass::queryProperty(const QScriptValue &object,
                                       const QScriptString &name,
                                       QueryFlags flags, uint *id)
{
    Q_UNUSED(flags);
    QmlValueTypeReference ref =
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return 0;

    QByteArray propName = name.toString().toUtf8();

    int idx = ref.type->metaObject()->indexOfProperty(propName.constData());
    if (idx == -1)
        return 0;
    *id = idx;

    QMetaProperty prop = ref.object->metaObject()->property(idx);

    QmlValueTypeScriptClass::QueryFlags rv =
        QmlValueTypeScriptClass::HandlesReadAccess;
    if (prop.isWritable())
        rv |= QmlValueTypeScriptClass::HandlesWriteAccess;

    return rv;
}

QScriptValue QmlValueTypeScriptClass::property(const QScriptValue &object,
                                               const QScriptString &name,
                                               uint id)
{
    Q_UNUSED(name);
    QmlValueTypeReference ref =
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return QScriptValue();

    ref.type->read(ref.object, ref.property);

    QMetaProperty p = ref.type->metaObject()->property(id);
    QVariant rv = p.read(ref.type);

    return static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine))->scriptEngine.newVariant(rv);
}

void QmlValueTypeScriptClass::setProperty(QScriptValue &object,
                                          const QScriptString &name,
                                          uint id,
                                          const QScriptValue &value)
{
    Q_UNUSED(name);
    QmlValueTypeReference ref =
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return;

    QVariant v = QmlScriptClass::toVariant(engine, value);

    ref.type->read(ref.object, ref.property);
    QMetaProperty p = ref.type->metaObject()->property(id);
    p.write(ref.type, v);
    ref.type->write(ref.object, ref.property);
}

QVariant QmlValueTypeScriptClass::toVariant(const QScriptValue &val)
{
    QmlValueTypeReference ref =
        qvariant_cast<QmlValueTypeReference>(val.data().toVariant());

    if (!ref.object)
        return QVariant();

    QMetaProperty p = ref.object->metaObject()->property(ref.property);
    return p.read(ref.object);
}

QT_END_NAMESPACE

