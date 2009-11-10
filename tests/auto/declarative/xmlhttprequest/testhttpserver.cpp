/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include "testhttpserver.h"
#include <QTcpSocket>
#include <QDebug>
#include <QFile>

TestHTTPServer::TestHTTPServer(quint16 port)
: m_hasFailed(false)
{
    QObject::connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    server.listen(QHostAddress::LocalHost, port);
}

bool TestHTTPServer::isValid() const
{
    return server.isListening();
}

bool TestHTTPServer::wait(const QUrl &expect, const QUrl &reply, const QUrl &body)
{
    m_hasFailed = false;

    QFile expectFile(expect.toLocalFile());
    if (!expectFile.open(QIODevice::ReadOnly)) return false;
    
    QFile replyFile(reply.toLocalFile());
    if (!replyFile.open(QIODevice::ReadOnly)) return false;

    bodyData = QByteArray();
    if (body.isValid()) {
        QFile bodyFile(body.toLocalFile());
        if (!bodyFile.open(QIODevice::ReadOnly)) return false;
        bodyData = bodyFile.readAll();
    }

    waitData = expectFile.readAll();
    /*
    while (waitData.endsWith('\n'))
        waitData = waitData.left(waitData.count() - 1);
        */

    replyData = replyFile.readAll();

    if (!replyData.endsWith('\n'))
        replyData.append("\n");
    replyData.append("Content-length: " + QByteArray::number(bodyData.length()));
    replyData .append("\n\n");

    for (int ii = 0; ii < replyData.count(); ++ii) {
        if (replyData.at(ii) == '\n' && (!ii || replyData.at(ii - 1) != '\r')) {
            replyData.insert(ii, '\r');
            ++ii;
        }
    }
    replyData.append(bodyData);

    return true;
}

bool TestHTTPServer::hasFailed() const
{
    return m_hasFailed;
}

void TestHTTPServer::newConnection()
{
    QTcpSocket *socket = server.nextPendingConnection();
    if (!socket) return;

    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void TestHTTPServer::disconnected()
{
    sender()->deleteLater();
}

void TestHTTPServer::readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray ba = socket->readAll();

    if (m_hasFailed || waitData.isEmpty()) {
        qWarning() << "TestHTTPServer: Unexpected data" << ba;
        return;
    }

    for (int ii = 0; ii < ba.count(); ++ii) {
        const char c = ba.at(ii);
        if (c == '\r' && waitData.isEmpty())
           continue;
        else if (!waitData.isEmpty() && c == waitData.at(0))
            waitData = waitData.mid(1);
        else if (c == '\r')
            continue;
        else {
            QByteArray data = ba.mid(ii);
            qWarning() << "TestHTTPServer: Unexpected data" << data;
            m_hasFailed = true;
            socket->disconnect();
            return;
        }
    }

    if (waitData.isEmpty()) {
        socket->write(replyData);
        socket->disconnect();
    }
}

