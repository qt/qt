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

#include "qsimplecanvasserver_p.h"
#include "qdebug.h"
#ifndef Q_OS_WIN32
#include <arpa/inet.h>
#endif

QT_BEGIN_NAMESPACE

QSimpleCanvasServer::QSimpleCanvasServer(int port, QObject *parent)
: QObject(parent), _tcpServer(new QTcpServer(this))
{
    QObject::connect(_tcpServer, SIGNAL(newConnection()), 
                     this, SLOT(newConnection()));

    _time.start();

    if (!_tcpServer->listen(QHostAddress::Any, port)) {
        qWarning() << "QSimpleCanvasServer: Cannot listen on port" << port;
        return;
    }
}

void QSimpleCanvasServer::newConnection()
{
    QTcpSocket *socket = _tcpServer->nextPendingConnection();
    QObject::connect(socket, SIGNAL(disconnected()), 
                     this, SLOT(disconnected()));
    _tcpClients << socket;
}

void QSimpleCanvasServer::addTiming(quint32 paint, 
                                quint32 repaint, 
                                quint32 timeBetweenFrames)
{
    /*
    quint32 data[3];
    data[0] = ::htonl(paint);
    data[1] = ::htonl(repaint);
    data[2] = ::htonl(timeBetweenFrames);
    */

    int e = _time.elapsed();
    QString d = QString::number(paint) + QLatin1String(" ") + QString::number(repaint) + QLatin1String(" ") + QString::number(timeBetweenFrames) + QLatin1String(" ") + QString::number(e) + QLatin1String("\n");
    QByteArray ba = d.toLatin1();

    // XXX
    for (int ii = 0; ii < _tcpClients.count(); ++ii) 
//        _tcpClients.at(ii)->write((const char *)data, 12);
        _tcpClients.at(ii)->write(ba.constData(), ba.length());
}

void QSimpleCanvasServer::disconnected()
{
    QTcpSocket *socket = static_cast<QTcpSocket *>(sender());

    for (int ii = 0; ii < _tcpClients.count(); ++ii) {
        if (_tcpClients.at(ii) == socket) {
            socket->disconnect();
            socket->deleteLater();
            _tcpClients.removeAt(ii);
            return;
        }
    }
}

QT_END_NAMESPACE
