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

#include "private/qdeclarativedebugserver_p.h"
#include "private/qdeclarativedebugservice_p.h"
#include "private/qdeclarativedebugservice_p_p.h"
#include "private/qdeclarativeengine_p.h"

#include "private/qpacketprotocol_p.h"

#include <QtCore/QStringList>

#include <QtNetwork/qtcpserver.h>
#include <QtNetwork/qtcpsocket.h>

#include <private/qobject_p.h>
#include <private/qapplication_p.h>

QT_BEGIN_NAMESPACE

/*
  QDeclarativeDebug Protocol (Version 1):

  handshake:
    1. Client sends
         "QDeclarativeDebugServer" 0 version pluginNames
       version: an int representing the highest protocol version the client knows
       pluginNames: plugins available on client side
    2. Server sends
         "QDeclarativeDebugClient" 0 version pluginNames
       version: an int representing the highest protocol version the client & server know
       pluginNames: plugins available on server side. plugins both in the client and server message are enabled.
  client plugin advertisement
    1. Client sends
         "QDeclarativeDebugServer" 1 pluginNames
  server plugin advertisement
    1. Server sends
         "QDeclarativeDebugClient" 1 pluginNames
  plugin communication:
       Everything send with a header different to "QDeclarativeDebugServer" is sent to the appropriate plugin.
  */

const int protocolVersion = 1;


class QDeclarativeDebugServerPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeDebugServer)
public:
    QDeclarativeDebugServerPrivate();

    void advertisePlugins();

    int port;
    QTcpSocket *connection;
    QPacketProtocol *protocol;
    QHash<QString, QDeclarativeDebugService *> plugins;
    QStringList clientPlugins;
    QTcpServer *tcpServer;
    bool gotHello;
};

QDeclarativeDebugServerPrivate::QDeclarativeDebugServerPrivate()
: connection(0), protocol(0), gotHello(false)
{
}

void QDeclarativeDebugServerPrivate::advertisePlugins()
{
    if (!connection
            || connection->state() != QTcpSocket::ConnectedState
            || !gotHello)
        return;

    QPacket pack;
    pack << QString(QLatin1String("QDeclarativeDebugClient")) << 1 << plugins.keys();
    protocol->send(pack);
    connection->flush();
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
        QTcpSocket *faultyConnection = d->tcpServer->nextPendingConnection();
        delete faultyConnection;
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
    return d->connection
            && (d->connection->state() == QTcpSocket::ConnectedState)
            && d->gotHello;
}

QDeclarativeDebugServer *QDeclarativeDebugServer::instance()
{
    static bool commandLineTested = false;
    static QDeclarativeDebugServer *server = 0;

    if (!commandLineTested) {
        commandLineTested = true;

#ifndef QDECLARATIVE_NO_DEBUG_PROTOCOL
        QApplicationPrivate *appD = static_cast<QApplicationPrivate*>(QObjectPrivate::get(qApp));
        // ### remove port definition when protocol is changed
        int port = 0;
        bool block = false;
        bool ok = false;

        // format: qmljsdebugger=port:3768[,block]
        if (!appD->qmljsDebugArgumentsString().isEmpty()) {
            if (!QDeclarativeEnginePrivate::qml_debugging_enabled) {
                const QString message =
                    QString::fromAscii("QDeclarativeDebugServer: Ignoring \"-qmljsdebugger=%1\". "
                              "Debugging has not been enabled.").arg(
                                  appD->qmljsDebugArgumentsString());
                qWarning("%s", qPrintable(message));
                return 0;
            }

            if (appD->qmljsDebugArgumentsString().indexOf(QLatin1String("port:")) == 0) {
                int separatorIndex = appD->qmljsDebugArgumentsString().indexOf(QLatin1Char(','));
                port = appD->qmljsDebugArgumentsString().mid(5, separatorIndex - 5).toInt(&ok);
            }
            block = appD->qmljsDebugArgumentsString().contains(QLatin1String("block"));

            if (ok) {
                server = new QDeclarativeDebugServer(port);
                server->listen();
                if (block) {
                    server->waitForConnection();
                }
            } else {
                qWarning(QString::fromAscii("QDeclarativeDebugServer: Ignoring \"-qmljsdebugger=%1\". "
                                            "Format is -qmljsdebugger=port:<port>[,block]").arg(
                             appD->qmljsDebugArgumentsString()).toAscii().constData());
            }
        }
#endif
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
        int op;
        hello >> name >> op;

        if (name != QLatin1String("QDeclarativeDebugServer")
                || op != 0) {
            qWarning("QDeclarativeDebugServer: Invalid hello message");
            QObject::disconnect(d->protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
            d->protocol->deleteLater();
            d->protocol = 0;
            d->connection->deleteLater();
            d->connection = 0;
            return;
        }

        int version;
        hello >> version >> d->clientPlugins;

        QHash<QString, QDeclarativeDebugService*>::Iterator iter = d->plugins.begin();
        for (; iter != d->plugins.end(); ++iter) {
            QDeclarativeDebugService::Status newStatus = QDeclarativeDebugService::Unavailable;
            if (d->clientPlugins.contains(iter.key()))
                newStatus = QDeclarativeDebugService::Enabled;
            iter.value()->d_func()->status = newStatus;
            iter.value()->statusChanged(newStatus);
        }

        QPacket helloAnswer;
        helloAnswer << QString(QLatin1String("QDeclarativeDebugClient")) << 0 << protocolVersion << d->plugins.keys();
        d->protocol->send(helloAnswer);
        d->connection->flush();

        d->gotHello = true;
        qWarning("QDeclarativeDebugServer: Connection established");
    }

    QString debugServer(QLatin1String("QDeclarativeDebugServer"));

    while (d->protocol->packetsAvailable()) {
        QPacket pack = d->protocol->read();

        QString name;
        pack >> name;

        if (name == debugServer) {
            int op = -1;
            pack >> op;

            if (op == 1) {
                // Service Discovery
                QStringList oldClientPlugins = d->clientPlugins;
                pack >> d->clientPlugins;

                QHash<QString, QDeclarativeDebugService*>::Iterator iter = d->plugins.begin();
                for (; iter != d->plugins.end(); ++iter) {
                    const QString pluginName = iter.key();
                    QDeclarativeDebugService::Status newStatus = QDeclarativeDebugService::Unavailable;
                    if (d->clientPlugins.contains(pluginName))
                        newStatus = QDeclarativeDebugService::Enabled;

                    if (oldClientPlugins.contains(pluginName)
                            != d->clientPlugins.contains(pluginName)) {
                        iter.value()->d_func()->status = newStatus;
                        iter.value()->statusChanged(newStatus);
                    }
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


QList<QDeclarativeDebugService*> QDeclarativeDebugServer::services() const
{
    const Q_D(QDeclarativeDebugServer);
    return d->plugins.values();
}

QStringList QDeclarativeDebugServer::serviceNames() const
{
    const Q_D(QDeclarativeDebugServer);
    return d->plugins.keys();
}

bool QDeclarativeDebugServer::addService(QDeclarativeDebugService *service)
{
    Q_D(QDeclarativeDebugServer);
    if (!service || d->plugins.contains(service->name()))
        return false;

    d->plugins.insert(service->name(), service);
    d->advertisePlugins();

    QDeclarativeDebugService::Status newStatus = QDeclarativeDebugService::Unavailable;
    if (d->clientPlugins.contains(service->name()))
        newStatus = QDeclarativeDebugService::Enabled;
    service->d_func()->status = newStatus;
    service->statusChanged(newStatus);
    return true;
}

bool QDeclarativeDebugServer::removeService(QDeclarativeDebugService *service)
{
    Q_D(QDeclarativeDebugServer);
    if (!service || !d->plugins.contains(service->name()))
        return false;

    d->plugins.remove(service->name());
    d->advertisePlugins();

    QDeclarativeDebugService::Status newStatus = QDeclarativeDebugService::NotConnected;
    service->d_func()->server = 0;
    service->d_func()->status = newStatus;
    service->statusChanged(newStatus);
    return true;
}

void QDeclarativeDebugServer::sendMessage(QDeclarativeDebugService *service,
                                          const QByteArray &message)
{
    Q_D(QDeclarativeDebugServer);
    QPacket pack;
    pack << service->name() << message;
    d->protocol->send(pack);
    d->connection->flush();
}

QT_END_NAMESPACE
