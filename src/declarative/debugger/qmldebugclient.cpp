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

#include "qmldebugclient.h"
#include <private/qobject_p.h>
#include <QtDeclarative/qpacketprotocol.h>
#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QmlDebugClientPrivate : public QObject
{
    Q_OBJECT
public:
    QmlDebugClientPrivate(QmlDebugClient *c);
    QmlDebugClient *q;
    QPacketProtocol *protocol;

    QStringList enabled;
    QHash<QString, QmlDebugClientPlugin *> plugins;
public slots:
    void connected();
    void readyRead();
};

QmlDebugClientPrivate::QmlDebugClientPrivate(QmlDebugClient *c)
: QObject(c), q(c), protocol(0)
{
    protocol = new QPacketProtocol(q, this);
    QObject::connect(c, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void QmlDebugClientPrivate::connected()
{
    QPacket pack;
    pack << QString(QLatin1String("QmlDebugServer")) << enabled;
    protocol->send(pack);
}

void QmlDebugClientPrivate::readyRead()
{
    QPacket pack = protocol->read();
    QString name; QByteArray message;
    pack >> name >> message;

    QHash<QString, QmlDebugClientPlugin *>::Iterator iter = 
        plugins.find(name);
    if (iter == plugins.end()) {
        qWarning() << "QmlDebugClient: Message received for missing plugin" << name;
    } else {
        (*iter)->messageReceived(message);
    }
}

QmlDebugClient::QmlDebugClient(QObject *parent)
: QTcpSocket(parent), d(new QmlDebugClientPrivate(this))
{
}

class QmlDebugClientPluginPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlDebugClientPlugin);
public:
    QmlDebugClientPluginPrivate();

    QString name;
    QmlDebugClient *client;
    bool enabled;
};

QmlDebugClientPluginPrivate::QmlDebugClientPluginPrivate()
: client(0), enabled(false)
{
}

QmlDebugClientPlugin::QmlDebugClientPlugin(const QString &name, 
                                           QmlDebugClient *parent)
: QObject(*(new QmlDebugClientPluginPrivate), parent)
{
    Q_D(QmlDebugClientPlugin);
    d->name = name;
    d->client = parent;

    if (!d->client)
        return;

    if (d->client->d->plugins.contains(name)) {
        qWarning() << "QmlDebugClientPlugin: Conflicting plugin name" << name;
        d->client = 0;
    } else {
        d->client->d->plugins.insert(name, this);
    }
}

QString QmlDebugClientPlugin::name() const
{
    Q_D(const QmlDebugClientPlugin);
    return d->name;
}

bool QmlDebugClientPlugin::isEnabled() const
{
    Q_D(const QmlDebugClientPlugin);
    return d->enabled;
}

void QmlDebugClientPlugin::setEnabled(bool e)
{
    Q_D(QmlDebugClientPlugin);
    if (e == d->enabled)
        return;

    d->enabled = e;

    if (d->client) {
        if (e) 
            d->client->d->enabled.append(d->name);
        else
            d->client->d->enabled.removeAll(d->name);

        if (d->client->state() == QTcpSocket::ConnectedState) {
            QPacket pack;
            pack << QString(QLatin1String("QmlDebugServer"));
            if (e) pack << (int)1;
            else pack << (int)2;
            pack << d->name;
            d->client->d->protocol->send(pack);
        }
    }
}

void QmlDebugClientPlugin::sendMessage(const QByteArray &message)
{
    Q_D(QmlDebugClientPlugin);

    if (!d->client || d->client->state() != QTcpSocket::ConnectedState)
        return;

    QPacket pack;
    pack << d->name << message;
    d->client->d->protocol->send(pack);
}

void QmlDebugClientPlugin::messageReceived(const QByteArray &)
{
}

#include "qmldebugclient.moc"

QT_END_NAMESPACE
