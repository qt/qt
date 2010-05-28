/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativedebugservice_p.h"

#include "private/qpacketprotocol_p.h"

#include <QtCore/qdebug.h>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeDebugServerPrivate;
class QDeclarativeDebugServer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeDebugServer)
    Q_DISABLE_COPY(QDeclarativeDebugServer)
public:
    static QDeclarativeDebugServer *instance();
    void listen();
    void waitForConnection();
    bool hasDebuggingClient() const;

private Q_SLOTS:
    void readyRead();
    void newConnection();

private:
    friend class QDeclarativeDebugService;
    friend class QDeclarativeDebugServicePrivate;
    QDeclarativeDebugServer(int); 
};

class QDeclarativeDebugServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeDebugServer)
public:
    QDeclarativeDebugServerPrivate();

    int port;
    QTcpSocket *connection;
    QPacketProtocol *protocol;
    QHash<QString, QDeclarativeDebugService *> plugins;
    QStringList enabledPlugins;
    QTcpServer *tcpServer;
    bool gotHello;
};

class QDeclarativeDebugServicePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeDebugService)
public:
    QDeclarativeDebugServicePrivate();

    QString name;
    QDeclarativeDebugServer *server;
};

QDeclarativeDebugServerPrivate::QDeclarativeDebugServerPrivate()
: connection(0), protocol(0), gotHello(false)
{
}

void QDeclarativeDebugServer::listen()
{
    Q_D(QDeclarativeDebugServer);

    d->tcpServer = new QTcpServer(this);
    QObject::connect(d->tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if (d->tcpServer->listen(QHostAddress::Any, d->port)) 
        qWarning("QDeclarativeDebugServer: Waiting for connection on port %d...", d->port);
    else
        qWarning("QDeclarativeDebugServer: Unable to listen on port %d", d->port);
}

void QDeclarativeDebugServer::waitForConnection()
{
    Q_D(QDeclarativeDebugServer);
    d->tcpServer->waitForNewConnection(-1);
}

void QDeclarativeDebugServer::newConnection()
{
    Q_D(QDeclarativeDebugServer);

    if (d->connection) {
        qWarning("QDeclarativeDebugServer error: another client is already connected");
        return;
    }

    d->connection = d->tcpServer->nextPendingConnection();
    d->connection->setParent(this);
    d->protocol = new QPacketProtocol(d->connection, this);
    QObject::connect(d->protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

bool QDeclarativeDebugServer::hasDebuggingClient() const
{
    Q_D(const QDeclarativeDebugServer);
    return d->gotHello;
}

QDeclarativeDebugServer *QDeclarativeDebugServer::instance()
{
    static bool envTested = false;
    static QDeclarativeDebugServer *server = 0;

    if (!envTested) {
        envTested = true;
        QByteArray env = qgetenv("QML_DEBUG_SERVER_PORT");
        QByteArray block = qgetenv("QML_DEBUG_SERVER_BLOCK");

        bool ok = false;
        int port = env.toInt(&ok);

        if (ok && port > 1024) {
            server = new QDeclarativeDebugServer(port);
            server->listen();
            if (!block.isEmpty()) {
                server->waitForConnection();
            }
        }
    }

    return server;
}

QDeclarativeDebugServer::QDeclarativeDebugServer(int port)
: QObject(*(new QDeclarativeDebugServerPrivate))
{
    Q_D(QDeclarativeDebugServer);
    d->port = port;
}

void QDeclarativeDebugServer::readyRead()
{
    Q_D(QDeclarativeDebugServer);

    if (!d->gotHello) {
        QPacket hello = d->protocol->read();
        QString name; 
        hello >> name >> d->enabledPlugins;
        if (name != QLatin1String("QDeclarativeDebugServer")) {
            qWarning("QDeclarativeDebugServer: Invalid hello message");
            QObject::disconnect(d->protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
            d->protocol->deleteLater();
            d->protocol = 0;
            d->connection->deleteLater();
            d->connection = 0;
            return;
        }
        d->gotHello = true;
        qWarning("QDeclarativeDebugServer: Connection established");
    }

    QString debugServer(QLatin1String("QDeclarativeDebugServer"));

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
                    QHash<QString, QDeclarativeDebugService *>::Iterator iter = 
                        d->plugins.find(plugin);
                    if (iter != d->plugins.end())
                        (*iter)->enabledChanged(true);
                }

            } else if (op == 2) {
                // Disable
                if (d->enabledPlugins.contains(plugin)) {
                    d->enabledPlugins.removeAll(plugin);
                    QHash<QString, QDeclarativeDebugService *>::Iterator iter = 
                        d->plugins.find(plugin);
                    if (iter != d->plugins.end())
                        (*iter)->enabledChanged(false);
                }
            } else {
                qWarning("QDeclarativeDebugServer: Invalid control message %d", op);
            }

        } else {
            QByteArray message;
            pack >> message;

            QHash<QString, QDeclarativeDebugService *>::Iterator iter = 
                d->plugins.find(name);
            if (iter == d->plugins.end()) {
                qWarning() << "QDeclarativeDebugServer: Message received for missing plugin" << name;
            } else {
                (*iter)->messageReceived(message);
            }
        }
    }
}

QDeclarativeDebugServicePrivate::QDeclarativeDebugServicePrivate()
: server(0)
{
}

QDeclarativeDebugService::QDeclarativeDebugService(const QString &name, QObject *parent)
: QObject(*(new QDeclarativeDebugServicePrivate), parent)
{
    Q_D(QDeclarativeDebugService);
    d->name = name;
    d->server = QDeclarativeDebugServer::instance();

    if (!d->server)
        return;

    if (d->server->d_func()->plugins.contains(name)) {
        qWarning() << "QDeclarativeDebugService: Conflicting plugin name" << name;
        d->server = 0;
    } else {
        d->server->d_func()->plugins.insert(name, this);
    }
}

QString QDeclarativeDebugService::name() const
{
    Q_D(const QDeclarativeDebugService);
    return d->name;
}

bool QDeclarativeDebugService::isEnabled() const
{
    Q_D(const QDeclarativeDebugService);
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
int QDeclarativeDebugService::idForObject(QObject *object)
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
QObject *QDeclarativeDebugService::objectForId(int id)
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

bool QDeclarativeDebugService::isDebuggingEnabled()
{
    return QDeclarativeDebugServer::instance() != 0;
}

bool QDeclarativeDebugService::hasDebuggingClient()
{
    return QDeclarativeDebugServer::instance() != 0
            && QDeclarativeDebugServer::instance()->hasDebuggingClient();
}

QString QDeclarativeDebugService::objectToString(QObject *obj)
{
    if(!obj)
        return QLatin1String("NULL");

    QString objectName = obj->objectName();
    if(objectName.isEmpty())
        objectName = QLatin1String("<unnamed>");

    QString rv = QString::fromUtf8(obj->metaObject()->className()) + 
                 QLatin1String(": ") + objectName;

    return rv;
}

void QDeclarativeDebugService::sendMessage(const QByteArray &message)
{
    Q_D(QDeclarativeDebugService);

    if (!d->server || !d->server->d_func()->connection)
        return;

    QPacket pack;
    pack << d->name << message;
    d->server->d_func()->protocol->send(pack);
    d->server->d_func()->connection->flush();
}

void QDeclarativeDebugService::enabledChanged(bool)
{
}

void QDeclarativeDebugService::messageReceived(const QByteArray &)
{
}

QT_END_NAMESPACE

#include <qdeclarativedebugservice.moc>
