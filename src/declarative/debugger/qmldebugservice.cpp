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

#include "qmldebugservice_p.h"

#include "qpacketprotocol_p.h"

#include <QtCore/qdebug.h>
#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlDebugServerPrivate;
class QmlDebugServer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlDebugServer)
    Q_DISABLE_COPY(QmlDebugServer)
public:
    static QmlDebugServer *instance();
    void wait();

private Q_SLOTS:
    void readyRead();

private:
    friend class QmlDebugService;
    friend class QmlDebugServicePrivate;
    QmlDebugServer(int); 
};

class QmlDebugServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDebugServer)
public:
    QmlDebugServerPrivate();
    void wait();

    int port;
    QTcpSocket *connection;
    QPacketProtocol *protocol;
    QHash<QString, QmlDebugService *> plugins;
    QStringList enabledPlugins;
};

class QmlDebugServicePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDebugService)
public:
    QmlDebugServicePrivate();

    QString name;
    QmlDebugServer *server;
};

QmlDebugServerPrivate::QmlDebugServerPrivate()
: connection(0), protocol(0)
{
}

void QmlDebugServerPrivate::wait()
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

    return server;
}

void QmlDebugServer::wait()
{
    Q_D(QmlDebugServer);
    d->wait();
}

QmlDebugServer::QmlDebugServer(int port)
: QObject(*(new QmlDebugServerPrivate))
{
    Q_D(QmlDebugServer);
    d->port = port;
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
                    QHash<QString, QmlDebugService *>::Iterator iter = 
                        d->plugins.find(plugin);
                    if (iter != d->plugins.end())
                        (*iter)->enabledChanged(true);
                }

            } else if (op == 2) {
                // Disable
                if (d->enabledPlugins.contains(plugin)) {
                    d->enabledPlugins.removeAll(plugin);
                    QHash<QString, QmlDebugService *>::Iterator iter = 
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

            QHash<QString, QmlDebugService *>::Iterator iter = 
                d->plugins.find(name);
            if (iter == d->plugins.end()) {
                qWarning() << "QmlDebugServer: Message received for missing plugin" << name;
            } else {
                (*iter)->messageReceived(message);
            }
        }
    }
}

QmlDebugServicePrivate::QmlDebugServicePrivate()
: server(0)
{
}

QmlDebugService::QmlDebugService(const QString &name, QObject *parent)
: QObject(*(new QmlDebugServicePrivate), parent)
{
    Q_D(QmlDebugService);
    d->name = name;
    d->server = QmlDebugServer::instance();

    if (!d->server)
        return;

    if (d->server->d_func()->plugins.contains(name)) {
        qWarning() << "QmlDebugService: Conflicting plugin name" << name;
        d->server = 0;
    } else {
        d->server->d_func()->plugins.insert(name, this);
    }
}

QString QmlDebugService::name() const
{
    Q_D(const QmlDebugService);
    return d->name;
}

bool QmlDebugService::isEnabled() const
{
    Q_D(const QmlDebugService);
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
int QmlDebugService::idForObject(QObject *object)
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
QObject *QmlDebugService::objectForId(int id)
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

bool QmlDebugService::isDebuggingEnabled()
{
    return QmlDebugServer::instance() != 0;
}

QString QmlDebugService::objectToString(QObject *obj)
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

void QmlDebugService::waitForClients()
{
    QmlDebugServer::instance()->wait();
}

void QmlDebugService::sendMessage(const QByteArray &message)
{
    Q_D(QmlDebugService);

    if (!d->server || !d->server->d_func()->connection)
        return;

    QPacket pack;
    pack << d->name << message;
    d->server->d_func()->protocol->send(pack);
    d->server->d_func()->connection->flush();
}

void QmlDebugService::enabledChanged(bool)
{
}

void QmlDebugService::messageReceived(const QByteArray &)
{
}

QT_END_NAMESPACE

#include <qmldebugservice.moc>
