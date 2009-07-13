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

#include "qmldebugserver.h"
#include <QtCore/qdebug.h>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <private/qobject_p.h>
#include <QtDeclarative/qpacketprotocol.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QmlDebugServerPrivate;
class QmlDebugServer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlDebugServer)
    Q_DISABLE_COPY(QmlDebugServer)
public:
    static QmlDebugServer *instance();

private slots:
    void readyRead();

private:
    friend class QmlDebugServerPlugin;
    friend class QmlDebugServerPluginPrivate;
    QmlDebugServer(int); 
};

class QmlDebugServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDebugServer)
public:
    QmlDebugServerPrivate();
    void init(int port);

    QTcpSocket *connection;
    QPacketProtocol *protocol;
    QHash<QString, QmlDebugServerPlugin *> plugins;
    QStringList enabledPlugins;
};

class QmlDebugServerPluginPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDebugServerPlugin)
public:
    QmlDebugServerPluginPrivate();

    QString name;
    QmlDebugServer *server;
};

QmlDebugServerPrivate::QmlDebugServerPrivate()
: connection(0), protocol(0)
{
}

void QmlDebugServerPrivate::init(int port)
{
    Q_Q(QmlDebugServer);
    QTcpServer server;

    if (!server.listen(QHostAddress::Any, port)) {
        qWarning("QmlDebugServer: Unable to listen on port %d", port);
        return;
    }

    qWarning("QmlDebugServer: Waiting for connection on port %d...", port);

    if (!server.waitForNewConnection(-1)) {
        qWarning("QmlDebugServer: Connection error");
        return;
    }

    connection = server.nextPendingConnection();
    connection->setParent(q);
    protocol = new QPacketProtocol(connection, q);

    // ### Wait for hello
    while (!protocol->packetsAvailable()) {
        connection->waitForReadyRead();
    }
    QPacket hello = protocol->read();
    QString name; 
    hello >> name >> enabledPlugins;
    if (name != QLatin1String("QmlDebugServer")) {
        qWarning("QmlDebugServer: Invalid hello message");
        delete protocol; delete connection; connection = 0; protocol = 0;
        return;
    }

    QObject::connect(protocol, SIGNAL(readyRead()), q, SLOT(readyRead()));
    q->readyRead();

    qWarning("QmlDebugServer: Connection established");
}

QmlDebugServer *QmlDebugServer::instance()
{
    static bool envTested = false;
    static QmlDebugServer *server = 0;

    if (!envTested) {
        envTested = true;
        QByteArray env = qgetenv("QML_DEBUG_SERVER_PORT");

        bool ok = false;
        int port = env.toInt(&ok);

        if (ok && port > 1024) 
            server = new QmlDebugServer(port);
    }

    if (server && server->d_func()->connection)
        return server;
    else
        return 0;
}

QmlDebugServer::QmlDebugServer(int port)
: QObject(*(new QmlDebugServerPrivate))
{
    Q_D(QmlDebugServer);
    d->init(port);
}

void QmlDebugServer::readyRead()
{
    Q_D(QmlDebugServer);

    QString debugServer(QLatin1String("QmlDebugServer"));

    while (d->protocol->packetsAvailable()) {
        QPacket pack = d->protocol->read();

        QString name;
        pack >> name;

        if (name == debugServer) {
            int op = -1; QString plugin;
            pack >> op >> plugin;

            if (op == 1) {
                // Enable
                if (!d->enabledPlugins.contains(plugin)) {
                    d->enabledPlugins.append(plugin);
                    QHash<QString, QmlDebugServerPlugin *>::Iterator iter = 
                        d->plugins.find(plugin);
                    if (iter != d->plugins.end())
                        (*iter)->enabledChanged(true);
                }

            } else if (op == 2) {
                // Disable
                if (d->enabledPlugins.contains(plugin)) {
                    d->enabledPlugins.removeAll(plugin);
                    QHash<QString, QmlDebugServerPlugin *>::Iterator iter = 
                        d->plugins.find(plugin);
                    if (iter != d->plugins.end())
                        (*iter)->enabledChanged(false);
                }
            } else {
                qWarning("QmlDebugServer: Invalid control message %d", op);
            }

        } else {
            QByteArray message;
            pack >> message;

            QHash<QString, QmlDebugServerPlugin *>::Iterator iter = 
                d->plugins.find(name);
            if (iter == d->plugins.end()) {
                qWarning() << "QmlDebugServer: Message received for missing plugin" << name;
            } else {
                (*iter)->messageReceived(message);
            }
        }
    }
}

QmlDebugServerPluginPrivate::QmlDebugServerPluginPrivate()
: server(0)
{
}

QmlDebugServerPlugin::QmlDebugServerPlugin(const QString &name, QObject *parent)
: QObject(*(new QmlDebugServerPluginPrivate), parent)
{
    Q_D(QmlDebugServerPlugin);
    d->name = name;
    d->server = QmlDebugServer::instance();

    if (!d->server)
        return;

    if (d->server->d_func()->plugins.contains(name)) {
        qWarning() << "QmlDebugServerPlugin: Conflicting plugin name" << name;
        d->server = 0;
    } else {
        d->server->d_func()->plugins.insert(name, this);
    }
}

QString QmlDebugServerPlugin::name() const
{
    Q_D(const QmlDebugServerPlugin);
    return d->name;
}

bool QmlDebugServerPlugin::isEnabled() const
{
    Q_D(const QmlDebugServerPlugin);
    return (d->server && d->server->d_func()->enabledPlugins.contains(d->name));
}

namespace {

    struct ObjectReference 
    {
        QPointer<QObject> object;
        int id;
    };

    struct ObjectReferenceHash 
    {
        ObjectReferenceHash() : nextId(0) {}

        QHash<QObject *, ObjectReference> objects;
        QHash<int, QObject *> ids;

        int nextId;
    };

}
Q_GLOBAL_STATIC(ObjectReferenceHash, objectReferenceHash);


/*!
    Returns a unique id for \a object.  Calling this method multiple times
    for the same object will return the same id.
*/
int QmlDebugServerPlugin::idForObject(QObject *object)
{
    if (!object)
        return -1;

    ObjectReferenceHash *hash = objectReferenceHash();
    QHash<QObject *, ObjectReference>::Iterator iter = 
        hash->objects.find(object);

    if (iter == hash->objects.end()) {
        int id = hash->nextId++;

        hash->ids.insert(id, object);
        iter = hash->objects.insert(object, ObjectReference());
        iter->object = object;
        iter->id = id;
    } else if (iter->object != object) {
        int id = hash->nextId++;

        hash->ids.remove(iter->id);

        hash->ids.insert(id, object);
        iter->object = object;
        iter->id = id;
    } 
    return iter->id;
}

/*!
    Returns the object for unique \a id.  If the object has not previously been
    assigned an id, through idForObject(), then 0 is returned.  If the object
    has been destroyed, 0 is returned.
*/
QObject *QmlDebugServerPlugin::objectForId(int id)
{
    ObjectReferenceHash *hash = objectReferenceHash();

    QHash<int, QObject *>::Iterator iter = hash->ids.find(id);
    if (iter == hash->ids.end())
        return 0;


    QHash<QObject *, ObjectReference>::Iterator objIter = 
        hash->objects.find(*iter);
    Q_ASSERT(objIter != hash->objects.end());

    if (objIter->object == 0) {
        hash->ids.erase(iter);
        hash->objects.erase(objIter);
        return 0;
    } else {
        return *iter;
    }
}

bool QmlDebugServerPlugin::isDebuggingEnabled()
{
    return QmlDebugServer::instance() != 0;
}

QString QmlDebugServerPlugin::objectToString(QObject *obj)
{
    if(!obj)
        return QLatin1String("NULL");

    QString objectName = obj->objectName();
    if(objectName.isEmpty())
        objectName = QLatin1String("<unnamed>");

    QString rv = QLatin1String(obj->metaObject()->className()) + 
                 QLatin1String(": ") + objectName;

    return rv;
}

void QmlDebugServerPlugin::sendMessage(const QByteArray &message)
{
    Q_D(QmlDebugServerPlugin);

    if (!d->server || !d->server->d_func()->connection)
        return;

    QPacket pack;
    pack << d->name << message;
    d->server->d_func()->protocol->send(pack);
    d->server->d_func()->connection->flush();
}

void QmlDebugServerPlugin::enabledChanged(bool)
{
}

void QmlDebugServerPlugin::messageReceived(const QByteArray &)
{
}

QT_END_NAMESPACE

#include "qmldebugserver.moc"
