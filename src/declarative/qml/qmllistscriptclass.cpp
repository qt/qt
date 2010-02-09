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

#include "qmllistscriptclass_p.h"

#include "qmlengine_p.h"
#include "qmlguard_p.h"

QT_BEGIN_NAMESPACE

struct ListData : public QScriptDeclarativeClass::Object {
    QmlGuard<QObject> object;
    int propertyIdx;
    QmlListScriptClass::ListCategory type;
    int propertyType;
};

QmlListScriptClass::QmlListScriptClass(QmlEngine *e)
: QmlScriptClass(QmlEnginePrivate::getScriptEngine(e)), engine(e)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    Q_UNUSED(scriptEngine);

    m_lengthId = createPersistentIdentifier(QLatin1String("length"));
}

QmlListScriptClass::~QmlListScriptClass()
{
}

QScriptValue QmlListScriptClass::newList(QObject *object, int propId, ListCategory type, int propType)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!object || propId == -1)
        return scriptEngine->nullValue();

    ListData *data = new ListData;
    data->object = object;
    data->propertyIdx = propId;
    data->type = type;
    data->propertyType = propType;

    return newObject(scriptEngine, this, data);
}

QScriptClass::QueryFlags 
QmlListScriptClass::queryProperty(Object *object, const Identifier &name, 
                                  QScriptClass::QueryFlags flags)
{
    Q_UNUSED(object);
    Q_UNUSED(flags);
    if (name == m_lengthId.identifier)
        return QScriptClass::HandlesReadAccess;

    bool ok = false;
    quint32 idx = toArrayIndex(name, &ok);

    if (ok) {
        lastIndex = idx;
        return QScriptClass::HandlesReadAccess;
    } else {
        return 0;
    }
}

QmlListScriptClass::ScriptValue QmlListScriptClass::property(Object *obj, const Identifier &name)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    ListData *data = (ListData *)obj;
    if (!data->object) 
        return Value();

    if (data->type == ListProperty) {
        QmlListProperty<QObject> list;
        void *args[] = { &list, 0 };

        QMetaObject::metacall(data->object, QMetaObject::ReadProperty, 
                data->propertyIdx, args);

        quint32 count = list.count(&list);

        if (name == m_lengthId.identifier)
            return Value(scriptEngine, count);
        else if (lastIndex < count)
            return Value(scriptEngine, enginePriv->objectClass->newQObject(list.at(&list, lastIndex)));
        else
            return Value();

    } else {
        void *list = 0;
        void *args[] = { &list, 0 };
        QMetaObject::metacall(data->object, QMetaObject::ReadProperty, 
                data->propertyIdx, args);

        if (!list)
            return Value();

        Q_ASSERT(data->type == QmlListPtr);
        const QmlList<QObject *> &qmllist = *((QmlList<QObject *>*)list);

        quint32 count = qmllist.count();

        if (name == m_lengthId.identifier)
            return Value(scriptEngine, count);
        else if (lastIndex < count) 
            return Value(scriptEngine, enginePriv->objectClass->newQObject(qmllist.at(lastIndex)));
        else
            return Value();
    }
}

QVariant QmlListScriptClass::toVariant(Object *obj, bool *ok)
{
    ListData *data = (ListData *)obj;

    if (!data->object) {
        if (ok) *ok = false;
        return QVariant();
    }

    if (data->type == QmlListScriptClass::QmlListPtr) {
        void *list = 0;
        void *args[] = { &list, 0 };
        QMetaObject::metacall(data->object, QMetaObject::ReadProperty, 
                              data->propertyIdx, args);

        if (!list) {
            if (ok) *ok = false;
            return QVariant();
        }

        if (ok) *ok = true;
        return QVariant(data->propertyType, &list);
    } else {
        QmlListProperty<QObject> list;
        void *args[] = { &list, 0 };

        QMetaObject::metacall(data->object, QMetaObject::ReadProperty, 
                              data->propertyIdx, args);

        if (ok) *ok = true;
        return QVariant(data->propertyType, &list);
    }
}

QT_END_NAMESPACE

