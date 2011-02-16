/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativedebugserver_p.h"
#include "private/qdeclarativedebugservice_p.h"
#include "private/qdeclarativedebugservice_p_p.h"
#include "private/qdeclarativeengine_p.h"

#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QStringList>

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

    QDeclarativeDebugServerConnection *connection;
    QHash<QString, QDeclarativeDebugService *> plugins;
    QStringList clientPlugins;
    bool gotHello;

    static QDeclarativeDebugServerConnection *loadConnectionPlugin();
};

QDeclarativeDebugServerPrivate::QDeclarativeDebugServerPrivate() :
    connection(0),
    gotHello(false)
{
}

void QDeclarativeDebugServerPrivate::advertisePlugins()
{
    if (!gotHello)
        return;

    QByteArray message;
    {
        QDataStream out(&message, QIODevice::WriteOnly);
        out << QString(QLatin1String("QDeclarativeDebugClient")) << 1 << plugins.keys();
    }
    connection->send(message);
}

QDeclarativeDebugServerConnection *QDeclarativeDebugServerPrivate::loadConnectionPlugin()
{
    QStringList pluginCandidates;
    const QStringList paths = QCoreApplication::libraryPaths();
    foreach (const QString &libPath, paths) {
        const QDir dir(libPath + QLatin1String("/qmltooling"));
        if (dir.exists()) {
            QStringList plugins(dir.entryList(QDir::Files));
            foreach (const QString &pluginPath, plugins) {
                pluginCandidates << dir.absoluteFilePath(pluginPath);
            }
        }
    }

    foreach (const QString &pluginPath, pluginCandidates) {
        QPluginLoader loader(pluginPath);
        if (!loader.load()) {
            continue;
        }
        QDeclarativeDebugServerConnection *connection = 0;
        if (QObject *instance = loader.instance())
            connection = qobject_cast<QDeclarativeDebugServerConnection*>(instance);

        if (connection)
            return connection;
        loader.unload();
    }
    return 0;
}

bool QDeclarativeDebugServer::hasDebuggingClient() const
{
    Q_D(const QDeclarativeDebugServer);
    return d->connection
            && d->connection->isConnected()
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
                server = new QDeclarativeDebugServer();

                QDeclarativeDebugServerConnection *connection
                        = QDeclarativeDebugServerPrivate::loadConnectionPlugin();
                if (connection) {
                    server->d_func()->connection = connection;

                    connection->setServer(server);
                    connection->setPort(port, block);
                } else {
                    qWarning() << QString::fromAscii("QDeclarativeDebugServer: Ignoring\"-qmljsdebugger=%1\". "
                                                     "Remote debugger plugin has not been found.").arg(appD->qmljsDebugArgumentsString());
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

QDeclarativeDebugServer::QDeclarativeDebugServer()
: QObject(*(new QDeclarativeDebugServerPrivate))
{
}

void QDeclarativeDebugServer::receiveMessage(const QByteArray &message)
{
    Q_D(QDeclarativeDebugServer);

    QDataStream in(message);
    if (!d->gotHello) {

        QString name;
        int op;
        in >> name >> op;

        if (name != QLatin1String("QDeclarativeDebugServer")
                || op != 0) {
            qWarning("QDeclarativeDebugServer: Invalid hello message");
            d->connection->disconnect();
            return;
        }

        int version;
        in >> version >> d->clientPlugins;

        QHash<QString, QDeclarativeDebugService*>::Iterator iter = d->plugins.begin();
        for (; iter != d->plugins.end(); ++iter) {
            QDeclarativeDebugService::Status newStatus = QDeclarativeDebugService::Unavailable;
            if (d->clientPlugins.contains(iter.key()))
                newStatus = QDeclarativeDebugService::Enabled;
            iter.value()->d_func()->status = newStatus;
            iter.value()->statusChanged(newStatus);
        }

        QByteArray helloAnswer;
        {
            QDataStream out(&helloAnswer, QIODevice::WriteOnly);
            out << QString(QLatin1String("QDeclarativeDebugClient")) << 0 << protocolVersion << d->plugins.keys();
        }
        d->connection->send(helloAnswer);

        d->gotHello = true;
        qWarning("QDeclarativeDebugServer: Connection established");
    } else {

        QString debugServer(QLatin1String("QDeclarativeDebugServer"));

        QString name;
        in >> name;

        if (name == debugServer) {
            int op = -1;
            in >> op;

            if (op == 1) {
                // Service Discovery
                QStringList oldClientPlugins = d->clientPlugins;
                in >> d->clientPlugins;

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
            in >> message;

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
    QByteArray msg;
    {
        QDataStream out(&msg, QIODevice::WriteOnly);
        out << service->name() << message;
    }
    d->connection->send(msg);
}

QT_END_NAMESPACE
