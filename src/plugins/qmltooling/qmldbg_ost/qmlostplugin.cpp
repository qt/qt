/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlostplugin.h"
#include "qostdevice.h"

#include <private/qdeclarativedebugserver_p.h>
#include <private/qpacketprotocol_p.h>

QT_BEGIN_NAMESPACE

static const TInt KQmlOstProtocolId = 0x94;

class QmlOstPluginPrivate {
public:
    QmlOstPluginPrivate();

    QOstDevice *ost;
    QPacketProtocol *protocol;
    QDeclarativeDebugServer *debugServer;
};

QmlOstPluginPrivate::QmlOstPluginPrivate() :
    ost(0),
    protocol(0),
    debugServer(0)
{
}

QmlOstPlugin::QmlOstPlugin() :
    d_ptr(new QmlOstPluginPrivate)
{
}

QmlOstPlugin::~QmlOstPlugin()
{
    delete d_ptr;
}

void QmlOstPlugin::setServer(QDeclarativeDebugServer *server)
{
    Q_D(QmlOstPlugin);
    d->debugServer = server;
}

bool QmlOstPlugin::isConnected() const
{
    Q_D(const QmlOstPlugin);
    return d->ost && d->ost->isOpen();
}

void QmlOstPlugin::send(const QByteArray &message)
{
    Q_D(QmlOstPlugin);

    if (!isConnected())
        return;

    QPacket pack;
    pack.writeRawData(message.data(), message.length());

    d->protocol->send(pack);
    //d->socket->flush();
}

void QmlOstPlugin::disconnect()
{
    Q_D(QmlOstPlugin);

    delete d->protocol;
    d->protocol = 0;
}

bool QmlOstPlugin::waitForMessage()
{
    Q_D(QmlOstPlugin);
    return d->protocol->waitForReadyRead(-1);
}

void QmlOstPlugin::setPort(int port, bool block)
{
    Q_UNUSED(port);
    Q_UNUSED(block);

    Q_D(QmlOstPlugin);

    d->ost = new QOstDevice(this);
    bool ok = d->ost->open(KQmlOstProtocolId);
    if (!ok) {
        if (d->ost->errorString().length())
            qDebug("Error from QOstDevice: %s", qPrintable(d->ost->errorString()));
        qWarning("QDeclarativeDebugServer: Unable to listen on OST"); // This message is part of the signalling - do not change the format!
        return;
    }
    d->protocol = new QPacketProtocol(d->ost, this);
    QObject::connect(d->protocol, SIGNAL(readyRead()), this, SLOT(readyRead()));
    qWarning("QDeclarativeDebugServer: Waiting for connection via OST"); // This message is part of the signalling - do not change the format!
}

void QmlOstPlugin::readyRead()
{
    Q_D(QmlOstPlugin);
    QPacket packet = d->protocol->read();

    QByteArray content = packet.data();
    d->debugServer->receiveMessage(content);
}

Q_EXPORT_PLUGIN2(qmlostplugin, QmlOstPlugin)

QT_END_NAMESPACE
