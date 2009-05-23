/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#include "qlocalserver.h"
#include "qlocalserver_p.h"
#include "qlocalsocket.h"

#include <qdebug.h>
#include <qdatetime.h>
#include <qcoreapplication.h>
#include <QMetaType>

// The buffer size need to be 0 otherwise data could be
// lost if the socket that has written data closes the connection
// before it is read.  Pipewriter is used for write buffering.
#define BUFSIZE 0

QT_BEGIN_NAMESPACE

QLocalServerThread::QLocalServerThread(QObject *parent) : QThread(parent),
    maxPendingConnections(1)
{
    stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    gotConnectionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

QLocalServerThread::~QLocalServerThread()
{
    stop();
    closeServer();
    CloseHandle(stopEvent);
    CloseHandle(gotConnectionEvent);
}

void QLocalServerThread::stop()
{
    if (isRunning()) {
        SetEvent(stopEvent);
        wait();
        ResetEvent(stopEvent);
    }
}

void QLocalServerThread::closeServer()
{
    while (!pendingHandles.isEmpty())
        CloseHandle(pendingHandles.dequeue());
}

QString QLocalServerThread::setName(const QString &name)
{
    QString pipePath = QLatin1String("\\\\.\\pipe\\");
    if (name.startsWith(pipePath))
        fullServerName = name;
    else
        fullServerName = pipePath + name;
    for (int i = pendingHandles.count(); i < maxPendingConnections; ++i)
        if (!makeHandle())
            break;
    return fullServerName;
}

bool QLocalServerThread::makeHandle()
{
    if (pendingHandles.count() >= maxPendingConnections)
        return false;

    HANDLE handle = INVALID_HANDLE_VALUE;
    QT_WA({
    handle = CreateNamedPipeW(
                 (TCHAR*)fullServerName.utf16(), // pipe name
                 PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access
                 PIPE_TYPE_MESSAGE |       // message type pipe
                 PIPE_READMODE_MESSAGE |   // message-read mode
                 PIPE_WAIT,                // blocking mode
                 PIPE_UNLIMITED_INSTANCES, // max. instances
                 BUFSIZE,                  // output buffer size
                 BUFSIZE,                  // input buffer size
                 3000,                     // client time-out
                 NULL);
    }, {
    handle = CreateNamedPipeA(
                 fullServerName.toLocal8Bit().constData(), // pipe name
                 PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,       // read/write access
                 PIPE_TYPE_MESSAGE |       // message type pipe
                 PIPE_READMODE_MESSAGE |   // message-read mode
                 PIPE_WAIT,                // blocking mode
                 PIPE_UNLIMITED_INSTANCES, // max. instances
                 BUFSIZE,                  // output buffer size
                 BUFSIZE,                  // input buffer size
                 3000,                     // client time-out
                 NULL);
    });

    if (INVALID_HANDLE_VALUE == handle) {
        return false;
    }
    pendingHandles.enqueue(handle);
    return true;
}

void QLocalServerThread::run()
{
    OVERLAPPED  op;
    HANDLE      handleArray[2];
    memset(&op, 0, sizeof(op));
    handleArray[0] = op.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    handleArray[1] = stopEvent;
    HANDLE handle = INVALID_HANDLE_VALUE;

    forever {
        if (INVALID_HANDLE_VALUE == handle) {
            makeHandle();
            if (!pendingHandles.isEmpty())
                handle = pendingHandles.dequeue();
        }
        if (INVALID_HANDLE_VALUE == handle) {
            int windowsError = GetLastError();
            QString function = QLatin1String("QLocalServer::run");
            QString errorString = QLocalServer::tr("%1: Unknown error %2").arg(function).arg(windowsError);
            emit error(QAbstractSocket::UnknownSocketError, errorString);
            CloseHandle(handleArray[0]);
            SetEvent(gotConnectionEvent);
            return;
        }

        BOOL isConnected = ConnectNamedPipe(handle, &op) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!isConnected) {
            switch (WaitForMultipleObjects(2, handleArray, FALSE, INFINITE))
            {
            case WAIT_OBJECT_0 + 1:
                CloseHandle(handle);
                CloseHandle(handleArray[0]);
                return;
            }
        }
        emit connected(handle);
        handle = INVALID_HANDLE_VALUE;
        ResetEvent(handleArray[0]);
        SetEvent(gotConnectionEvent);
    }
}

void QLocalServerPrivate::init()
{
    Q_Q(QLocalServer);
    qRegisterMetaType<HANDLE>("HANDLE");
    q->connect(&waitForConnection, SIGNAL(connected(HANDLE)),
               q, SLOT(_q_openSocket(HANDLE)), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(finished()),
               q, SLOT(_q_stoppedListening()), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(terminated()),
               q, SLOT(_q_stoppedListening()), Qt::QueuedConnection);
    q->connect(&waitForConnection, SIGNAL(error(QAbstractSocket::SocketError, const QString &)),
               q, SLOT(_q_setError(QAbstractSocket::SocketError, const QString &)));
}

bool QLocalServerPrivate::removeServer(const QString &name)
{
    Q_UNUSED(name);
    return true;
}

bool QLocalServerPrivate::listen(const QString &name)
{
    fullServerName = waitForConnection.setName(name);
    serverName = name;
    waitForConnection.start();
    return true;
}

void QLocalServerPrivate::_q_setError(QAbstractSocket::SocketError e, const QString &eString)
{
    error = e;
    errorString = eString;
}

void QLocalServerPrivate::_q_stoppedListening()
{
    Q_Q(QLocalServer);
    if (!inWaitingFunction)
        q->close();
}

void QLocalServerPrivate::_q_openSocket(HANDLE handle)
{
    Q_Q(QLocalServer);
    q->incomingConnection((int)handle);
}

void QLocalServerPrivate::closeServer()
{
    waitForConnection.stop();
    waitForConnection.closeServer();
}

void QLocalServerPrivate::waitForNewConnection(int msecs, bool *timedOut)
{
    Q_Q(QLocalServer);
    if (!pendingConnections.isEmpty() || !q->isListening())
        return;

    DWORD result = WaitForSingleObject(waitForConnection.gotConnectionEvent,
                                       (msecs == -1) ? INFINITE : msecs);
    if (result == WAIT_TIMEOUT) {
        if (timedOut)
            *timedOut = true;
    } else {
        ResetEvent(waitForConnection.gotConnectionEvent);
        QCoreApplication::instance()->processEvents();
    }
}

QT_END_NAMESPACE
