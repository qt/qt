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

#include "qmldebug_p.h"

#include "qmldebugclient_p.h"

#include <qmlenginedebug_p.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlEngineDebugClient : public QmlDebugClient
{
public:
    QmlEngineDebugClient(QmlDebugConnection *client, QmlEngineDebugPrivate *p);

protected:
    virtual void messageReceived(const QByteArray &);

private:
    QmlEngineDebugPrivate *priv;
};

class QmlEngineDebugPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlEngineDebug)
public:
    QmlEngineDebugPrivate(QmlDebugConnection *);

    void message(const QByteArray &);

    QmlEngineDebugClient *client;
    int nextId;
    int getId();

    void decode(QDataStream &, QmlDebugContextReference &);
    void decode(QDataStream &, QmlDebugObjectReference &, bool simple);

    static void remove(QmlEngineDebug *, QmlDebugEnginesQuery *);
    static void remove(QmlEngineDebug *, QmlDebugRootContextQuery *);
    static void remove(QmlEngineDebug *, QmlDebugObjectQuery *);
    static void remove(QmlEngineDebug *, QmlDebugExpressionQuery *);

    QHash<int, QmlDebugEnginesQuery *> enginesQuery;
    QHash<int, QmlDebugRootContextQuery *> rootContextQuery;
    QHash<int, QmlDebugObjectQuery *> objectQuery;
    QHash<int, QmlDebugExpressionQuery *> expressionQuery;

    QHash<int, QmlDebugWatch *> watched;
};

QmlEngineDebugClient::QmlEngineDebugClient(QmlDebugConnection *client,
                                           QmlEngineDebugPrivate *p)
: QmlDebugClient(QLatin1String("QmlEngine"), client), priv(p)
{
    setEnabled(true);
}

void QmlEngineDebugClient::messageReceived(const QByteArray &data)
{
    priv->message(data);
}

QmlEngineDebugPrivate::QmlEngineDebugPrivate(QmlDebugConnection *c)
: client(new QmlEngineDebugClient(c, this)), nextId(0)
{
}

int QmlEngineDebugPrivate::getId()
{
    return nextId++;
}

void QmlEngineDebugPrivate::remove(QmlEngineDebug *c, QmlDebugEnginesQuery *q)
{
    if (c && q) {
        QmlEngineDebugPrivate *p = (QmlEngineDebugPrivate *)QObjectPrivate::get(c);
        p->enginesQuery.remove(q->m_queryId);
    }
}

void QmlEngineDebugPrivate::remove(QmlEngineDebug *c, 
                                   QmlDebugRootContextQuery *q)
{
    if (c && q) {
        QmlEngineDebugPrivate *p = (QmlEngineDebugPrivate *)QObjectPrivate::get(c);
        p->rootContextQuery.remove(q->m_queryId);
    }
}

void QmlEngineDebugPrivate::remove(QmlEngineDebug *c, QmlDebugObjectQuery *q)
{
    if (c && q) {
        QmlEngineDebugPrivate *p = (QmlEngineDebugPrivate *)QObjectPrivate::get(c);
        p->objectQuery.remove(q->m_queryId);
    }
}

void QmlEngineDebugPrivate::remove(QmlEngineDebug *c, QmlDebugExpressionQuery *q)
{
    if (c && q) {
        QmlEngineDebugPrivate *p = (QmlEngineDebugPrivate *)QObjectPrivate::get(c);
        p->expressionQuery.remove(q->m_queryId);
    }
}

void QmlEngineDebugPrivate::decode(QDataStream &ds, QmlDebugObjectReference &o,
                                   bool simple)
{
    QmlEngineDebugServer::QmlObjectData data;
    ds >> data;
    o.m_debugId = data.objectId;
    o.m_class = data.objectType;
    o.m_name = data.objectName;
    o.m_source.m_url = data.url;
    o.m_source.m_lineNumber = data.lineNumber;
    o.m_source.m_columnNumber = data.columnNumber;
    o.m_contextDebugId = data.contextId;

    if (simple)
        return;

    int childCount;
    bool recur;
    ds >> childCount >> recur;

    for (int ii = 0; ii < childCount; ++ii) {
        o.m_children.append(QmlDebugObjectReference());
        decode(ds, o.m_children.last(), !recur);
    }

    int propCount;
    ds >> propCount;

    for (int ii = 0; ii < propCount; ++ii) {
        QmlEngineDebugServer::QmlObjectProperty data;
        ds >> data;
        QmlDebugPropertyReference prop;
        prop.m_objectDebugId = o.m_debugId;
        prop.m_name = data.name;
        prop.m_binding = data.binding;
        prop.m_hasNotifySignal = data.hasNotifySignal;
        prop.m_valueTypeName = data.valueTypeName;
        switch (data.type) {
            case QmlEngineDebugServer::QmlObjectProperty::Basic:
            case QmlEngineDebugServer::QmlObjectProperty::List:
            case QmlEngineDebugServer::QmlObjectProperty::SignalProperty:
            {
                prop.m_value = data.value;
                break;
            }
            case QmlEngineDebugServer::QmlObjectProperty::Object:
            {
                QmlDebugObjectReference obj;
                obj.m_debugId = prop.m_value.toInt();
                prop.m_value = qVariantFromValue(obj);
                break;
            }
            case QmlEngineDebugServer::QmlObjectProperty::Unknown:
                break;
        }
        o.m_properties << prop;
    }
}

void QmlEngineDebugPrivate::decode(QDataStream &ds, QmlDebugContextReference &c)
{
    ds >> c.m_name >> c.m_debugId;

    int contextCount;
    ds >> contextCount;

    for (int ii = 0; ii < contextCount; ++ii) {
        c.m_contexts.append(QmlDebugContextReference());
        decode(ds, c.m_contexts.last());
    }

    int objectCount;
    ds >> objectCount;

    for (int ii = 0; ii < objectCount; ++ii) {
        QmlDebugObjectReference obj;
        decode(ds, obj, true);

        obj.m_contextDebugId = c.m_debugId;
        c.m_objects << obj;
    }
}

void QmlEngineDebugPrivate::message(const QByteArray &data)
{
    QDataStream ds(data);

    QByteArray type;
    ds >> type;

    //qDebug() << "QmlEngineDebugPrivate::message()" << type;

    if (type == "LIST_ENGINES_R") {
        int queryId;
        ds >> queryId;

        QmlDebugEnginesQuery *query = enginesQuery.value(queryId);
        if (!query)
            return;
        enginesQuery.remove(queryId);

        int count;
        ds >> count;

        for (int ii = 0; ii < count; ++ii) {
            QmlDebugEngineReference ref;
            ds >> ref.m_name;
            ds >> ref.m_debugId;
            query->m_engines << ref;
        }

        query->m_client = 0;
        query->setState(QmlDebugQuery::Completed);
    } else if (type == "LIST_OBJECTS_R") {
        int queryId;
        ds >> queryId;

        QmlDebugRootContextQuery *query = rootContextQuery.value(queryId);
        if (!query)
            return;
        rootContextQuery.remove(queryId);

        if (!ds.atEnd()) 
            decode(ds, query->m_context);

        query->m_client = 0;
        query->setState(QmlDebugQuery::Completed);
    } else if (type == "FETCH_OBJECT_R") {
        int queryId;
        ds >> queryId;

        QmlDebugObjectQuery *query = objectQuery.value(queryId);
        if (!query)
            return;
        objectQuery.remove(queryId);

        if (!ds.atEnd())
            decode(ds, query->m_object, false);

        query->m_client = 0;
        query->setState(QmlDebugQuery::Completed);
    } else if (type == "EVAL_EXPRESSION_R") {
        int queryId;
        QVariant result;
        ds >> queryId >> result;

        QmlDebugExpressionQuery *query = expressionQuery.value(queryId);
        if (!query)
            return;
        expressionQuery.remove(queryId);

        query->m_result = result;
        query->m_client = 0;
        query->setState(QmlDebugQuery::Completed);
    } else if (type == "WATCH_PROPERTY_R") {
        int queryId;
        bool ok;
        ds >> queryId >> ok;

        QmlDebugWatch *watch = watched.value(queryId);
        if (!watch)
            return;

        watch->setState(ok ? QmlDebugWatch::Active : QmlDebugWatch::Inactive);
    } else if (type == "WATCH_OBJECT_R") {
        int queryId;
        bool ok;
        ds >> queryId >> ok;

        QmlDebugWatch *watch = watched.value(queryId);
        if (!watch)
            return;

        watch->setState(ok ? QmlDebugWatch::Active : QmlDebugWatch::Inactive);
    } else if (type == "WATCH_EXPR_OBJECT_R") {
        int queryId;
        bool ok;
        ds >> queryId >> ok;

        QmlDebugWatch *watch = watched.value(queryId);
        if (!watch)
            return;

        watch->setState(ok ? QmlDebugWatch::Active : QmlDebugWatch::Inactive);
    } else if (type == "UPDATE_WATCH") {
        int queryId;
        int debugId;
        QByteArray name;
        QVariant value;
        ds >> queryId >> debugId >> name >> value;

        QmlDebugWatch *watch = watched.value(queryId, 0);
        if (!watch)
            return;
        emit watch->valueChanged(name, value);
    }
}

QmlEngineDebug::QmlEngineDebug(QmlDebugConnection *client, QObject *parent)
: QObject(*(new QmlEngineDebugPrivate(client)), parent)
{
}

QmlDebugPropertyWatch *QmlEngineDebug::addWatch(const QmlDebugPropertyReference &property, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugPropertyWatch *watch = new QmlDebugPropertyWatch(parent);
    if (d->client->isConnected()) {
        int queryId = d->getId();
        watch->m_queryId = queryId;
        watch->m_client = this;
        watch->m_objectDebugId = property.objectDebugId();
        watch->m_name = property.name();
        d->watched.insert(queryId, watch);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("WATCH_PROPERTY") << queryId << property.objectDebugId() << property.name().toUtf8();
        d->client->sendMessage(message);
    } else {
        watch->m_state = QmlDebugWatch::Dead;
    }

    return watch;
}

QmlDebugWatch *QmlEngineDebug::addWatch(const QmlDebugContextReference &, const QString &, QObject *)
{
    qWarning("QmlEngineDebug::addWatch(): Not implemented");
    return 0;
}

QmlDebugObjectExpressionWatch *QmlEngineDebug::addWatch(const QmlDebugObjectReference &object, const QString &expr, QObject *parent)
{
    Q_D(QmlEngineDebug);
    QmlDebugObjectExpressionWatch *watch = new QmlDebugObjectExpressionWatch(parent);
    if (d->client->isConnected()) {
        int queryId = d->getId();
        watch->m_queryId = queryId;
        watch->m_client = this;
        watch->m_objectDebugId = object.debugId();
        watch->m_expr = expr;
        d->watched.insert(queryId, watch);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("WATCH_EXPR_OBJECT") << queryId << object.debugId() << expr;
        d->client->sendMessage(message);
    } else {
        watch->m_state = QmlDebugWatch::Dead;
    }
    return watch;
}

QmlDebugWatch *QmlEngineDebug::addWatch(const QmlDebugObjectReference &object, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugWatch *watch = new QmlDebugWatch(parent);
    if (d->client->isConnected()) {
        int queryId = d->getId();
        watch->m_queryId = queryId;
        watch->m_client = this;
        watch->m_objectDebugId = object.debugId();
        d->watched.insert(queryId, watch);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("WATCH_OBJECT") << queryId << object.debugId();
        d->client->sendMessage(message);
    } else {
        watch->m_state = QmlDebugWatch::Dead;
    }

    return watch;
}

QmlDebugWatch *QmlEngineDebug::addWatch(const QmlDebugFileReference &, QObject *)
{
    qWarning("QmlEngineDebug::addWatch(): Not implemented");
    return 0;
}

void QmlEngineDebug::removeWatch(QmlDebugWatch *watch)
{
    Q_D(QmlEngineDebug);

    if (!watch || !watch->m_client)
        return;

    watch->m_client = 0;
    watch->setState(QmlDebugWatch::Inactive);
    
    d->watched.remove(watch->queryId());

    if (d->client && d->client->isConnected()) {
        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("NO_WATCH") << watch->queryId();
        d->client->sendMessage(message);
    }
}

QmlDebugEnginesQuery *QmlEngineDebug::queryAvailableEngines(QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugEnginesQuery *query = new QmlDebugEnginesQuery(parent);
    if (d->client->isConnected()) {
        query->m_client = this;
        int queryId = d->getId();
        query->m_queryId = queryId;
        d->enginesQuery.insert(queryId, query);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("LIST_ENGINES") << queryId;
        d->client->sendMessage(message);
    } else {
        query->m_state = QmlDebugQuery::Error;
    }

    return query;
}

QmlDebugRootContextQuery *QmlEngineDebug::queryRootContexts(const QmlDebugEngineReference &engine, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugRootContextQuery *query = new QmlDebugRootContextQuery(parent);
    if (d->client->isConnected() && engine.debugId() != -1) {
        query->m_client = this;
        int queryId = d->getId();
        query->m_queryId = queryId;
        d->rootContextQuery.insert(queryId, query);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("LIST_OBJECTS") << queryId << engine.debugId();
        d->client->sendMessage(message);
    } else {
        query->m_state = QmlDebugQuery::Error;
    }

    return query;
}

QmlDebugObjectQuery *QmlEngineDebug::queryObject(const QmlDebugObjectReference &object, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugObjectQuery *query = new QmlDebugObjectQuery(parent);
    if (d->client->isConnected() && object.debugId() != -1) {
        query->m_client = this;
        int queryId = d->getId();
        query->m_queryId = queryId;
        d->objectQuery.insert(queryId, query);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("FETCH_OBJECT") << queryId << object.debugId() 
           << false;
        d->client->sendMessage(message);
    } else {
        query->m_state = QmlDebugQuery::Error;
    }

    return query;
}

QmlDebugObjectQuery *QmlEngineDebug::queryObjectRecursive(const QmlDebugObjectReference &object, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugObjectQuery *query = new QmlDebugObjectQuery(parent);
    if (d->client->isConnected() && object.debugId() != -1) {
        query->m_client = this;
        int queryId = d->getId();
        query->m_queryId = queryId;
        d->objectQuery.insert(queryId, query);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("FETCH_OBJECT") << queryId << object.debugId() 
           << true;
        d->client->sendMessage(message);
    } else {
        query->m_state = QmlDebugQuery::Error;
    }

    return query;
}

QmlDebugExpressionQuery *QmlEngineDebug::queryExpressionResult(int objectDebugId, const QString &expr, QObject *parent)
{
    Q_D(QmlEngineDebug);

    QmlDebugExpressionQuery *query = new QmlDebugExpressionQuery(parent);
    if (d->client->isConnected() && objectDebugId != -1) {
        query->m_client = this;
        query->m_expr = expr;
        int queryId = d->getId();
        query->m_queryId = queryId;
        d->expressionQuery.insert(queryId, query);

        QByteArray message;
        QDataStream ds(&message, QIODevice::WriteOnly);
        ds << QByteArray("EVAL_EXPRESSION") << queryId << objectDebugId << expr;
        d->client->sendMessage(message);
    } else {
        query->m_state = QmlDebugQuery::Error;
    }

    return query;
}

QmlDebugWatch::QmlDebugWatch(QObject *parent)
: QObject(parent), m_state(Waiting), m_queryId(-1), m_client(0), m_objectDebugId(-1)
{
}

QmlDebugWatch::~QmlDebugWatch()
{
}

int QmlDebugWatch::queryId() const
{
    return m_queryId;
}

int QmlDebugWatch::objectDebugId() const
{
    return m_objectDebugId;
}

QmlDebugWatch::State QmlDebugWatch::state() const
{
    return m_state;
}

void QmlDebugWatch::setState(State s)
{
    if (m_state == s)
        return;
    m_state = s;
    emit stateChanged(m_state);
}

QmlDebugPropertyWatch::QmlDebugPropertyWatch(QObject *parent)
    : QmlDebugWatch(parent)
{
}

QString QmlDebugPropertyWatch::name() const
{
    return m_name;
}


QmlDebugObjectExpressionWatch::QmlDebugObjectExpressionWatch(QObject *parent)
    : QmlDebugWatch(parent)
{
}

QString QmlDebugObjectExpressionWatch::expression() const
{
    return m_expr;
}


QmlDebugQuery::QmlDebugQuery(QObject *parent)
: QObject(parent), m_state(Waiting)
{
}

QmlDebugQuery::State QmlDebugQuery::state() const
{
    return m_state;
}

bool QmlDebugQuery::isWaiting() const
{
    return m_state == Waiting;
}

void QmlDebugQuery::setState(State s)
{
    if (m_state == s)
        return;
    m_state = s;
    emit stateChanged(m_state);
}

QmlDebugEnginesQuery::QmlDebugEnginesQuery(QObject *parent)
: QmlDebugQuery(parent), m_client(0), m_queryId(-1)
{
}

QmlDebugEnginesQuery::~QmlDebugEnginesQuery()
{
    if (m_client && m_queryId != -1) 
        QmlEngineDebugPrivate::remove(m_client, this);
}

QList<QmlDebugEngineReference> QmlDebugEnginesQuery::engines() const
{
    return m_engines;
}

QmlDebugRootContextQuery::QmlDebugRootContextQuery(QObject *parent)
: QmlDebugQuery(parent), m_client(0), m_queryId(-1)
{
}

QmlDebugRootContextQuery::~QmlDebugRootContextQuery()
{
    if (m_client && m_queryId != -1) 
        QmlEngineDebugPrivate::remove(m_client, this);
}

QmlDebugContextReference QmlDebugRootContextQuery::rootContext() const
{
    return m_context;
}

QmlDebugObjectQuery::QmlDebugObjectQuery(QObject *parent)
: QmlDebugQuery(parent), m_client(0), m_queryId(-1)
{
}

QmlDebugObjectQuery::~QmlDebugObjectQuery()
{
    if (m_client && m_queryId != -1) 
        QmlEngineDebugPrivate::remove(m_client, this);
}

QmlDebugObjectReference QmlDebugObjectQuery::object() const
{
    return m_object;
}

QmlDebugExpressionQuery::QmlDebugExpressionQuery(QObject *parent)
: QmlDebugQuery(parent), m_client(0), m_queryId(-1)
{
}

QmlDebugExpressionQuery::~QmlDebugExpressionQuery()
{
    if (m_client && m_queryId != -1) 
        QmlEngineDebugPrivate::remove(m_client, this);
}

QString QmlDebugExpressionQuery::expression() const
{
    return m_expr;
}

QVariant QmlDebugExpressionQuery::result() const
{
    return m_result;
}

QmlDebugEngineReference::QmlDebugEngineReference()
: m_debugId(-1)
{
}

QmlDebugEngineReference::QmlDebugEngineReference(int debugId)
: m_debugId(debugId)
{
}

QmlDebugEngineReference::QmlDebugEngineReference(const QmlDebugEngineReference &o)
: m_debugId(o.m_debugId), m_name(o.m_name)
{
}

QmlDebugEngineReference &
QmlDebugEngineReference::operator=(const QmlDebugEngineReference &o)
{
    m_debugId = o.m_debugId; m_name = o.m_name;
    return *this;
}

int QmlDebugEngineReference::debugId() const
{
    return m_debugId;
}

QString QmlDebugEngineReference::name() const
{
    return m_name;
}

QmlDebugObjectReference::QmlDebugObjectReference()
: m_debugId(-1), m_contextDebugId(-1)
{
}

QmlDebugObjectReference::QmlDebugObjectReference(int debugId)
: m_debugId(debugId), m_contextDebugId(-1)
{
}

QmlDebugObjectReference::QmlDebugObjectReference(const QmlDebugObjectReference &o)
: m_debugId(o.m_debugId), m_class(o.m_class), m_name(o.m_name),
  m_source(o.m_source), m_contextDebugId(o.m_contextDebugId),
  m_properties(o.m_properties), m_children(o.m_children)
{
}

QmlDebugObjectReference &
QmlDebugObjectReference::operator=(const QmlDebugObjectReference &o)
{
    m_debugId = o.m_debugId; m_class = o.m_class; m_name = o.m_name; 
    m_source = o.m_source; m_contextDebugId = o.m_contextDebugId;
    m_properties = o.m_properties; m_children = o.m_children;
    return *this;
}

int QmlDebugObjectReference::debugId() const
{
    return m_debugId;
}

QString QmlDebugObjectReference::className() const
{
    return m_class;
}

QString QmlDebugObjectReference::name() const
{
    return m_name;
}

QmlDebugFileReference QmlDebugObjectReference::source() const
{
    return m_source;
}

int QmlDebugObjectReference::contextDebugId() const
{
    return m_contextDebugId;
}

QList<QmlDebugPropertyReference> QmlDebugObjectReference::properties() const
{
    return m_properties;
}

QList<QmlDebugObjectReference> QmlDebugObjectReference::children() const
{
    return m_children;
}

QmlDebugContextReference::QmlDebugContextReference()
: m_debugId(-1)
{
}

QmlDebugContextReference::QmlDebugContextReference(const QmlDebugContextReference &o)
: m_debugId(o.m_debugId), m_name(o.m_name), m_objects(o.m_objects), m_contexts(o.m_contexts)
{
}

QmlDebugContextReference &QmlDebugContextReference::operator=(const QmlDebugContextReference &o)
{
    m_debugId = o.m_debugId; m_name = o.m_name; m_objects = o.m_objects; 
    m_contexts = o.m_contexts;
    return *this;
}

int QmlDebugContextReference::debugId() const
{
    return m_debugId;
}

QString QmlDebugContextReference::name() const
{
    return m_name;
}

QList<QmlDebugObjectReference> QmlDebugContextReference::objects() const
{
    return m_objects;
}

QList<QmlDebugContextReference> QmlDebugContextReference::contexts() const
{
    return m_contexts;
}

QmlDebugFileReference::QmlDebugFileReference()
: m_lineNumber(-1), m_columnNumber(-1)
{
}

QmlDebugFileReference::QmlDebugFileReference(const QmlDebugFileReference &o)
: m_url(o.m_url), m_lineNumber(o.m_lineNumber), m_columnNumber(o.m_columnNumber)
{
}

QmlDebugFileReference &QmlDebugFileReference::operator=(const QmlDebugFileReference &o)
{
    m_url = o.m_url; m_lineNumber = o.m_lineNumber; m_columnNumber = o.m_columnNumber;
    return *this;
}

QUrl QmlDebugFileReference::url() const
{
    return m_url;
}

void QmlDebugFileReference::setUrl(const QUrl &u)
{
    m_url = u;
}

int QmlDebugFileReference::lineNumber() const
{
    return m_lineNumber;
}

void QmlDebugFileReference::setLineNumber(int l)
{
    m_lineNumber = l;
}

int QmlDebugFileReference::columnNumber() const
{
    return m_columnNumber;
}

void QmlDebugFileReference::setColumnNumber(int c)
{
    m_columnNumber = c;
}

QmlDebugPropertyReference::QmlDebugPropertyReference()
: m_objectDebugId(-1), m_hasNotifySignal(false)
{
}

QmlDebugPropertyReference::QmlDebugPropertyReference(const QmlDebugPropertyReference &o)
: m_objectDebugId(o.m_objectDebugId), m_name(o.m_name), m_value(o.m_value),
  m_valueTypeName(o.m_valueTypeName), m_binding(o.m_binding),
  m_hasNotifySignal(o.m_hasNotifySignal)
{
}

QmlDebugPropertyReference &QmlDebugPropertyReference::operator=(const QmlDebugPropertyReference &o)
{
    m_objectDebugId = o.m_objectDebugId; m_name = o.m_name; m_value = o.m_value;
    m_valueTypeName = o.m_valueTypeName; m_binding = o.m_binding;
    m_hasNotifySignal = o.m_hasNotifySignal;
    return *this;
}

int QmlDebugPropertyReference::objectDebugId() const
{
    return m_objectDebugId;
}

QString QmlDebugPropertyReference::name() const
{
    return m_name;
}

QString QmlDebugPropertyReference::valueTypeName() const
{
    return m_valueTypeName;
}

QVariant QmlDebugPropertyReference::value() const
{
    return m_value;
}

QString QmlDebugPropertyReference::binding() const
{
    return m_binding;
}

bool QmlDebugPropertyReference::hasNotifySignal() const
{
    return m_hasNotifySignal;
}

QT_END_NAMESPACE

