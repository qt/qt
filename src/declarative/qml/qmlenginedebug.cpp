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

#include "qmlenginedebug_p.h"
#include <QtCore/qdebug.h>
#include <QtCore/qmetaobject.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlmetatype.h>
#include <qmlmetaproperty.h>
#include <qmlbinding.h>
#include "qmlcontext_p.h"

QT_BEGIN_NAMESPACE

QList<QmlEngine *> QmlEngineDebugServer::m_engines;
QmlEngineDebugServer::QmlEngineDebugServer(QObject *parent)
: QmlDebugService(QLatin1String("QmlEngine"), parent)
{
}

QDataStream &operator<<(QDataStream &ds, 
                        const QmlEngineDebugServer::QmlObjectData &data)
{
    ds << data.url << data.lineNumber << data.columnNumber << data.objectName
       << data.objectType << data.objectId;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, 
                        QmlEngineDebugServer::QmlObjectData &data)
{
    ds >> data.url >> data.lineNumber >> data.columnNumber >> data.objectName
       >> data.objectType >> data.objectId;
    return ds;
}

QDataStream &operator<<(QDataStream &ds, 
                        const QmlEngineDebugServer::QmlObjectProperty &data)
{
    ds << (int)data.type << data.name << data.value << data.binding;
    return ds;
}

QDataStream &operator>>(QDataStream &ds,  
                        QmlEngineDebugServer::QmlObjectProperty &data)
{
    int type;
    ds >> type >> data.name >> data.value >> data.binding;
    data.type = (QmlEngineDebugServer::QmlObjectProperty::Type)type;
    return ds;
}

QmlEngineDebugServer::QmlObjectProperty 
QmlEngineDebugServer::propertyData(QObject *obj, int propIdx)
{
    QmlObjectProperty rv;

    QMetaProperty prop = obj->metaObject()->property(propIdx);

    rv.type = QmlObjectProperty::Unknown;
    rv.name = prop.name();
    QmlAbstractBinding *binding = QmlMetaProperty(obj, rv.name).binding();
    if (binding)
        rv.binding = binding->expression();

    if (prop.type() < QVariant::UserType) {
        rv.type = QmlObjectProperty::Basic;
        rv.value = prop.read(obj);
    } else if (QmlMetaType::isObject(prop.userType()))  {
        rv.type = QmlObjectProperty::Object;
    } else if (QmlMetaType::isList(prop.userType()) ||
               QmlMetaType::isQmlList(prop.userType())) {
        rv.type = QmlObjectProperty::List;
    }
    return rv;
}

void QmlEngineDebugServer::buildObjectDump(QDataStream &message, 
                                           QObject *object, bool recur)
{
    message << objectData(object);
    message << object->metaObject()->propertyCount();

    for (int ii = 0; ii < object->metaObject()->propertyCount(); ++ii) 
        message << propertyData(object, ii);

    QObjectList children = object->children();
    message << children.count() << recur;

    for (int ii = 0; ii < children.count(); ++ii) {
        QObject *child = children.at(ii);
        if (recur)
            buildObjectDump(message, child, recur);
        else
            message << objectData(child);
    }
}

void QmlEngineDebugServer::buildObjectList(QDataStream &message, 
                                           QmlContext *ctxt)
{
    QmlContextPrivate *p = (QmlContextPrivate *)QObjectPrivate::get(ctxt);

    QString ctxtName = ctxt->objectName();
    int ctxtId = QmlDebugService::idForObject(ctxt);

    message << ctxtName << ctxtId; 
    
    int count = 0;

    for (QSet<QmlContext *>::ConstIterator iter = p->childContexts.begin();
            iter != p->childContexts.end(); ++iter) {
        QmlContextPrivate *p = (QmlContextPrivate *)QObjectPrivate::get(*iter);
        if (p->isInternal)
            continue;
        ++count;
    }

    message << count;

    for (QSet<QmlContext *>::ConstIterator iter = p->childContexts.begin();
            iter != p->childContexts.end(); ++iter) {
        QmlContextPrivate *p = (QmlContextPrivate *)QObjectPrivate::get(*iter);
        if (p->isInternal)
            continue;
        buildObjectList(message, *iter);
    }

    // Clean deleted objects
    for (int ii = 0; ii < p->instances.count(); ++ii) {
        if (!p->instances.at(ii)) {
            p->instances.removeAt(ii);
            --ii;
        }
    }

    message << p->instances.count();
    for (int ii = 0; ii < p->instances.count(); ++ii) {
        message << objectData(p->instances.at(ii));
    }
}

QmlEngineDebugServer::QmlObjectData 
QmlEngineDebugServer::objectData(QObject *object)
{
    QmlObjectData rv;
    rv.lineNumber = -1;
    rv.columnNumber = -1;

    rv.objectName = object->objectName();
    rv.objectType = object->metaObject()->className();
    rv.objectId = QmlDebugService::idForObject(object);

    return rv;
}

void QmlEngineDebugServer::messageReceived(const QByteArray &message)
{
    QDataStream ds(message);

    QByteArray type;
    ds >> type;

    if (type == "LIST_ENGINES") {
        int queryId;
        ds >> queryId;

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("LIST_ENGINES_R");
        rs << queryId << m_engines.count();

        for (int ii = 0; ii < m_engines.count(); ++ii) {
            QmlEngine *engine = m_engines.at(ii);

            QString engineName = engine->objectName();
            int engineId = QmlDebugService::idForObject(engine);

            rs << engineName << engineId;
        }

        sendMessage(reply);
    } else if (type == "LIST_OBJECTS") {
        int queryId;
        int engineId = -1;
        ds >> queryId >> engineId;

        QmlEngine *engine = 
            qobject_cast<QmlEngine *>(QmlDebugService::objectForId(engineId));

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("LIST_OBJECTS_R") << queryId;

        if (engine)
            buildObjectList(rs, engine->rootContext());

        sendMessage(reply);
    } else if (type == "FETCH_OBJECT") {
        int queryId;
        int objectId;
        bool recurse;

        ds >> queryId >> objectId >> recurse;

        QObject *object = QmlDebugService::objectForId(objectId);

        QByteArray reply;
        QDataStream rs(&reply, QIODevice::WriteOnly);
        rs << QByteArray("FETCH_OBJECT_R") << queryId;

        if (object) 
            buildObjectDump(rs, object, recurse);

        sendMessage(reply);
    }
}

void QmlEngineDebugServer::addEngine(QmlEngine *engine)
{
    Q_ASSERT(engine);
    Q_ASSERT(!m_engines.contains(engine));

    m_engines.append(engine);
}

void QmlEngineDebugServer::remEngine(QmlEngine *engine)
{
    Q_ASSERT(engine);
    Q_ASSERT(m_engines.contains(engine));

    m_engines.removeAll(engine);
}

QT_END_NAMESPACE
