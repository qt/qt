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

#include "private/qdeclarativedebugclient_p.h"

#include "private/qpacketprotocol_p.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeDebugConnectionPrivate : public QObject
{
    Q_OBJECT
public:
    QDeclarativeDebugConnectionPrivate(QDeclarativeDebugConnection *c);
    QDeclarativeDebugConnection *q;
    QPacketProtocol *protocol;

    QStringList enabled;
    QHash<QString, QDeclarativeDebugClient *> plugins;
public Q_SLOTS:
    void connected();
    void readyRead();
};

QDeclarativeDebugConnectionPrivate::QDeclarativeDebugConnectionPrivate(QDeclarativeDebugConnection *c)
: QObject(c), q(c), protocol(0)
{
    protocol = new QPacketProtocol(q, this);
    QObject::connect(c, SIGNAL(connected()), this, SLOT(connected()));
    QObject::connect(protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void QDeclarativeDebugConnectionPrivate::connected()
{
    QPacket pack;
    pack << QString(QLatin1String("QDeclarativeDebugServer")) << enabled;
    protocol->send(pack);
}

void QDeclarativeDebugConnectionPrivate::readyRead()
{
    QPacket pack = protocol->read();
    QString name; QByteArray message;
    pack >> name >> message;

    QHash<QString, QDeclarativeDebugClient *>::Iterator iter = 
        plugins.find(name);
    if (iter == plugins.end()) {
        qWarning() << "QDeclarativeDebugConnection: Message received for missing plugin" << name;
    } else {
        (*iter)->messageReceived(message);
    }
}

QDeclarativeDebugConnection::QDeclarativeDebugConnection(QObject *parent)
: QTcpSocket(parent), d(new QDeclarativeDebugConnectionPrivate(this))
{
}

bool QDeclarativeDebugConnection::isConnected() const
{
    return state() == ConnectedState;
}

class QDeclarativeDebugClientPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeDebugClient)
public:
    QDeclarativeDebugClientPrivate();

    QString name;
    QDeclarativeDebugConnection *client;
    bool enabled;
};

QDeclarativeDebugClientPrivate::QDeclarativeDebugClientPrivate()
: client(0), enabled(false)
{
}

QDeclarativeDebugClient::QDeclarativeDebugClient(const QString &name, 
                                           QDeclarativeDebugConnection *parent)
: QObject(*(new QDeclarativeDebugClientPrivate), parent)
{
    Q_D(QDeclarativeDebugClient);
    d->name = name;
    d->client = parent;

    if (!d->client)
        return;

    if (d->client->d->plugins.contains(name)) {
        qWarning() << "QDeclarativeDebugClient: Conflicting plugin name" << name;
        d->client = 0;
    } else {
        d->client->d->plugins.insert(name, this);
    }
}

QString QDeclarativeDebugClient::name() const
{
    Q_D(const QDeclarativeDebugClient);
    return d->name;
}

bool QDeclarativeDebugClient::isEnabled() const
{
    Q_D(const QDeclarativeDebugClient);
    return d->enabled;
}

void QDeclarativeDebugClient::setEnabled(bool e)
{
    Q_D(QDeclarativeDebugClient);
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
            pack << QString(QLatin1String("QDeclarativeDebugServer"));
            if (e) pack << (int)1;
            else pack << (int)2;
            pack << d->name;
            d->client->d->protocol->send(pack);
        }
    }
}

bool QDeclarativeDebugClient::isConnected() const
{
    Q_D(const QDeclarativeDebugClient);

    if (!d->client)
        return false;
    return d->client->isConnected();
}

void QDeclarativeDebugClient::sendMessage(const QByteArray &message)
{
    Q_D(QDeclarativeDebugClient);

    if (!d->client || !d->client->isConnected())
        return;

    QPacket pack;
    pack << d->name << message;
    d->client->d->protocol->send(pack);
}

void QDeclarativeDebugClient::messageReceived(const QByteArray &)
{
}

QT_END_NAMESPACE

#include <qdeclarativedebugclient.moc>
