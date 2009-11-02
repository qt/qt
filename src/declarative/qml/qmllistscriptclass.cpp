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
#include <private/qmlengine_p.h>

QT_BEGIN_NAMESPACE

struct ListData : public QScriptDeclarativeClass::Object {
    QGuard<QObject> object;
    int propertyIdx;
    QmlListScriptClass::ListType type;
};

QmlListScriptClass::QmlListScriptClass(QmlEngine *e)
: QScriptDeclarativeClass(QmlEnginePrivate::getScriptEngine(e)), engine(e)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    m_lengthId = createPersistentIdentifier(QLatin1String("length"));
}

QmlListScriptClass::~QmlListScriptClass()
{
}

QScriptValue QmlListScriptClass::newList(QObject *object, int propId, ListType type)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    if (!object || propId == -1)
        return scriptEngine->nullValue();

    ListData *data = new ListData;
    data->object = object;
    data->propertyIdx = propId;
    data->type = type;

    return newObject(scriptEngine, this, data);
}

QScriptClass::QueryFlags 
QmlListScriptClass::queryProperty(Object *object, const Identifier &name, 
                                  QScriptClass::QueryFlags flags)
{
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

QScriptValue QmlListScriptClass::property(Object *obj, const Identifier &name)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);

    ListData *data = (ListData *)obj;
    if (!data->object) 
        return scriptEngine->undefinedValue();

    void *list = 0;
    void *args[] = { &list, 0 };
    QMetaObject::metacall(data->object, QMetaObject::ReadProperty, 
                          data->propertyIdx, args);

    if (!list)
        return scriptEngine->undefinedValue();

    if (data->type == QListPtr) {
        const QList<QObject *> &qlist = *((QList<QObject *>*)list);

        if (name == m_lengthId.identifier) 
            return qlist.count();
        else if (lastIndex < qlist.count())
            return enginePriv->objectClass->newQObject(qlist.at(lastIndex));
        else
            return scriptEngine->undefinedValue();

    } else {
        Q_ASSERT(data->type == QmlListPtr);
        const QmlList<QObject *> &qmllist = *((QmlList<QObject *>*)list);

        int count = qmllist.count();

        if (name == m_lengthId.identifier) 
            return count;
        else if (lastIndex < count) 
            return enginePriv->objectClass->newQObject(qmllist.at(lastIndex));
        else
            return scriptEngine->undefinedValue();
    }
}

QT_END_NAMESPACE

