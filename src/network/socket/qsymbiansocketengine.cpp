/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

//#define QNATIVESOCKETENGINE_DEBUG
#include "qsymbiansocketengine_p.h"

#include "qiodevice.h"
#include "qhostaddress.h"
#include "qelapsedtimer.h"
#include "qvarlengtharray.h"
#include "qnetworkinterface.h"
#include <es_sock.h>
#include <in_sock.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
#endif

#include <private/qcore_symbian_p.h>

#if !defined(QT_NO_NETWORKPROXY)
# include "qnetworkproxy.h"
# include "qabstractsocket.h"
# include "qtcpserver.h"
#endif

#define QNATIVESOCKETENGINE_DEBUG

#if defined QNATIVESOCKETENGINE_DEBUG
#include <qstring.h>
#include <ctype.h>
#endif

QT_BEGIN_NAMESPACE

#if defined QNATIVESOCKETENGINE_DEBUG

/*
    Returns a human readable representation of the first \a len
    characters in \a data.
*/
static QByteArray qt_prettyDebug(const char *data, int len, int maxSize)
{
    if (!data) return "(null)";
    QByteArray out;
    for (int i = 0; i < len; ++i) {
        char c = data[i];
        if (isprint(c)) {
            out += c;
        } else switch (c) {
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            QString tmp;
            tmp.sprintf("\\%o", c);
            out += tmp.toLatin1();
        }
    }

    if (len < maxSize)
        out += "...";

    return out;
}
#endif

void QSymbianSocketEnginePrivate::getPortAndAddress(const TInetAddr& a, quint16 *port, QHostAddress *addr)
{
#if !defined(QT_NO_IPV6)
    if (a.Family() == KAfInet6) {
        Q_IPV6ADDR tmp;
        memcpy(&tmp, a.Ip6Address().u.iAddr8, sizeof(tmp));
        if (addr) {
            QHostAddress tmpAddress;
            tmpAddress.setAddress(tmp);
            *addr = tmpAddress;
#ifndef QT_NO_IPV6IFNAME
            TPckgBuf<TSoInetIfQuery> query;
            query().iSrcAddr = a;
            TInt err = nativeSocket.GetOpt(KSoInetIfQueryBySrcAddr, KSolInetIfQuery, query);
            if(!err)
                addr->setScopeId(qt_TDesC2QString(query().iName));
            else
#endif
            addr->setScopeId(QString::number(a.Scope()));
        }
        if (port)
            *port = a.Port();
        return;
    }
#endif
    if (port)
        *port = a.Port();
    if (addr) {
        QHostAddress tmpAddress;
        tmpAddress.setAddress(a.Address());
        *addr = tmpAddress;
    }
}
/*! \internal

    Creates and returns a new socket descriptor of type \a socketType
    and \a socketProtocol.  Returns -1 on failure.
*/
bool QSymbianSocketEnginePrivate::createNewSocket(QAbstractSocket::SocketType socketType,
                                         QAbstractSocket::NetworkLayerProtocol socketProtocol)
{
    Q_Q(QSymbianSocketEngine);
#ifndef QT_NO_IPV6
    TUint family = (socketProtocol == QAbstractSocket::IPv6Protocol) ? KAfInet6 : KAfInet;
#else
    Q_UNUSED(socketProtocol);
    TUint family = KAfInet;
#endif
    TUint type = (socketType == QAbstractSocket::UdpSocket) ? KSockDatagram : KSockStream;
    TUint protocol = (socketType == QAbstractSocket::UdpSocket) ? KProtocolInetUdp : KProtocolInetTcp;
    TInt err = nativeSocket.Open(socketServer, family, type, protocol, *connection);

    if (err != KErrNone) {
        switch (err) {
        case KErrNotSupported:
        case KErrNotFound:
            setError(QAbstractSocket::UnsupportedSocketOperationError,
                ProtocolUnsupportedErrorString);
            break;
        case KErrNoMemory:
            setError(QAbstractSocket::SocketResourceError, ResourceErrorString);
            break;
        case KErrPermissionDenied:
            setError(QAbstractSocket::SocketAccessError, AccessErrorString);
            break;
        default:
            break;
        }

        return false;
    }
    // FIXME Set socket to nonblocking. While we are still a QNativeSocketEngine this is done already.
    // Uncomment the following when we switch to QSymbianSocketEngine.
    // setOption(NonBlockingSocketOption, 1)

    socketDescriptor = QSymbianSocketManager::instance().addSocket(nativeSocket);
    return true;
}

/*
    Returns the value of the socket option \a opt.
*/
int QSymbianSocketEngine::option(QAbstractSocketEngine::SocketOption opt) const
{
    Q_D(const QSymbianSocketEngine);
    if (!isValid())
        return -1;

    TUint n;
    TUint level = KSOLSocket; // default

    switch (opt) {
    case QAbstractSocketEngine::ReceiveBufferSocketOption:
        n = KSORecvBuf;
        break;
    case QAbstractSocketEngine::SendBufferSocketOption:
        n = KSOSendBuf;
        break;
    case QAbstractSocketEngine::NonBlockingSocketOption:
        n = KSONonBlockingIO;
        break;
    case QAbstractSocketEngine::BroadcastSocketOption:
        return true; //symbian doesn't support or require this option
    case QAbstractSocketEngine::AddressReusable:
        level = KSolInetIp;
        n = KSoReuseAddr;
        break;
    case QAbstractSocketEngine::BindExclusively:
        return true;
    case QAbstractSocketEngine::ReceiveOutOfBandData:
        level = KSolInetTcp;
        n = KSoTcpOobInline;
        break;
    case QAbstractSocketEngine::LowDelayOption:
        level = KSolInetTcp;
        n = KSoTcpNoDelay;
        break;
    case QAbstractSocketEngine::KeepAliveOption:
        level = KSolInetTcp;
        n = KSoTcpKeepAlive;
        break;
    default:
        return -1;
    }

    int v = -1;
    //GetOpt() is non const
    TInt err = d->nativeSocket.GetOpt(n, level, v);
    if (!err)
        return v;

    return -1;
}


void QSymbianSocketEnginePrivate::setPortAndAddress(TInetAddr& nativeAddr, quint16 port, const QHostAddress &addr)
{
    nativeAddr.SetPort(port);
#if !defined(QT_NO_IPV6)
    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
#ifndef QT_NO_IPV6IFNAME
        TPckgBuf<TSoInetIfQuery> query;
        query().iName = qt_QString2TPtrC(addr.scopeId());
        TInt err = nativeSocket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, query);
        if(!err)
            nativeAddr.SetScope(query().iIndex);
        else
            nativeAddr.SetScope(0);
#else
        nativeAddr.SetScope(addr.scopeId().toInt());
#endif
        Q_IPV6ADDR ip6 = addr.toIPv6Address();
        TIp6Addr v6addr;
        memcpy(v6addr.u.iAddr8, ip6.c, 16);
        nativeAddr.SetAddress(v6addr);
    } else
#endif
    if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        nativeAddr.SetAddress(addr.toIPv4Address());
    } else {
        qWarning("unsupported network protocol (%d)", addr.protocol());
    }
}

QSymbianSocketEnginePrivate::QSymbianSocketEnginePrivate() :
    socketDescriptor(-1),
    socketServer(qt_symbianGetSocketServer()),
    connection(QSymbianSocketManager::instance().defaultConnection()),
    readNotifier(0),
    writeNotifier(0),
    exceptNotifier(0)
{
}

QSymbianSocketEnginePrivate::~QSymbianSocketEnginePrivate()
{
}


QSymbianSocketEngine::QSymbianSocketEngine(QObject *parent)
    : QAbstractSocketEngine(*new QSymbianSocketEnginePrivate(), parent)
{
}


QSymbianSocketEngine::~QSymbianSocketEngine()
{
    close();
    // FIXME what else do we need to free?
}

/*
    Sets the socket option \a opt to \a v.
*/
bool QSymbianSocketEngine::setOption(QAbstractSocketEngine::SocketOption opt, int v)
{
    Q_D(QSymbianSocketEngine);
    if (!isValid())
        return false;

    int n = 0;
    int level = SOL_SOCKET; // default

    switch (opt) {
    case QAbstractSocketEngine::ReceiveBufferSocketOption:
        n = KSORecvBuf;
        break;
    case QAbstractSocketEngine::SendBufferSocketOption:
        n = KSOSendBuf;
        break;
    case QAbstractSocketEngine::BroadcastSocketOption:
        return true;
    case QAbstractSocketEngine::NonBlockingSocketOption:
        n = KSONonBlockingIO;
        break;
    case QAbstractSocketEngine::AddressReusable:
        level = KSolInetIp;
        n = KSoReuseAddr;
        break;
    case QAbstractSocketEngine::BindExclusively:
        return true;
    case QAbstractSocketEngine::ReceiveOutOfBandData:
        level = KSolInetTcp;
        n = KSoTcpOobInline;
        break;
    case QAbstractSocketEngine::LowDelayOption:
        level = KSolInetTcp;
        n = KSoTcpNoDelay;
        break;
    case QAbstractSocketEngine::KeepAliveOption:
        level = KSolInetTcp;
        n = KSoTcpKeepAlive;
        break;
    }

    return (KErrNone == d->nativeSocket.SetOpt(n, level, v));
}

bool QSymbianSocketEngine::connectToHostByName(const QString &name, quint16 port)
{
    // FIXME for engines that support hostnames.. not for us then i guess.

    return false;
}


bool QSymbianSocketEngine::connectToHost(const QHostAddress &addr, quint16 port)
{
    Q_D(QSymbianSocketEngine);

#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug("QSymbianSocketEngine::connectToHost() : %d ", d->socketDescriptor);
#endif

#if defined (QT_NO_IPV6)
    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        d->setError(QAbstractSocket::UnsupportedSocketOperationError,
                    NoIpV6ErrorString);
        return false;
    }
#endif
    if (!d->checkProxy(addr))
        return false;

    d->peerAddress = addr;
    d->peerPort = port;

    TInetAddr nativeAddr;
    d->setPortAndAddress(nativeAddr, port, addr);
    //TODO: async connect with active object - from here to end of function is a mess
    TRequestStatus status;
    d->nativeSocket.Connect(nativeAddr, status);
    User::WaitForRequest(status);
    TInt err = status.Int();
    if (err) {
        switch (err) {
        case KErrCouldNotConnect:
            d->setError(QAbstractSocket::ConnectionRefusedError, ConnectionRefusedErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrTimedOut:
            d->setError(QAbstractSocket::NetworkError, ConnectionTimeOutErrorString);
            break;
        case KErrHostUnreach:
            d->setError(QAbstractSocket::NetworkError, HostUnreachableErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNetUnreach:
            d->setError(QAbstractSocket::NetworkError, NetworkUnreachableErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrInUse:
            d->setError(QAbstractSocket::NetworkError, AddressInuseErrorString);
            break;
        case KErrPermissionDenied:
            d->setError(QAbstractSocket::SocketAccessError, AccessErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNotSupported:
        case KErrBadDescriptor:
            d->socketState = QAbstractSocket::UnconnectedState;
        default:
            break;
        }

        if (d->socketState != QAbstractSocket::ConnectedState) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
            qDebug("QSymbianSocketEnginePrivate::nativeConnect(%s, %i) == false (%s)",
                   addr.toString().toLatin1().constData(), port,
                   d->socketState == QAbstractSocket::ConnectingState
                   ? "Connection in progress" : d->socketErrorString.toLatin1().constData());
#endif
            return false;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == true",
           addr.toString().toLatin1().constData(), port);
#endif

    d->socketState = QAbstractSocket::ConnectedState;
    d->fetchConnectionParameters();
    return true;
}

bool QSymbianSocketEngine::bind(const QHostAddress &address, quint16 port)
{
    Q_D(QSymbianSocketEngine);
    TInetAddr nativeAddr;
    d->setPortAndAddress(nativeAddr, port, address);

    TInt err = d->nativeSocket.Bind(nativeAddr);

    if (err) {
        switch(errno) {
        case KErrInUse:
            d->setError(QAbstractSocket::AddressInUseError, AddressInuseErrorString);
            break;
        case KErrPermissionDenied:
            d->setError(QAbstractSocket::SocketAccessError, AddressProtectedErrorString);
            break;
        case KErrNotSupported:
            d->setError(QAbstractSocket::UnsupportedSocketOperationError, OperationUnsupportedErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QSymbianSocketEnginePrivate::nativeBind(%s, %i) == false (%s)",
               address.toString().toLatin1().constData(), port, d->socketErrorString.toLatin1().constData());
#endif

        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeBind(%s, %i) == true",
           address.toString().toLatin1().constData(), port);
#endif
    d->socketState = QAbstractSocket::BoundState;
    return true;
}

bool QSymbianSocketEngine::listen()
{
    Q_D(QSymbianSocketEngine);
    // TODO the value 50 is from the QNativeSocketEngine. Maybe it's a bit too much
    // for a mobile platform
    TInt err = d->nativeSocket.Listen(50);
    if (err) {
        switch (errno) {
        case KErrInUse:
            d->setError(QAbstractSocket::AddressInUseError,
                     PortInuseErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QSymbianSocketEnginePrivate::nativeListen() == false (%s)",
               d->socketErrorString.toLatin1().constData());
#endif
        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeListen() == true");
#endif

    d->socketState = QAbstractSocket::ListeningState;
    return true;
}

int QSymbianSocketEngine::accept()
{
    Q_D(QSymbianSocketEngine);
    RSocket blankSocket;
    //TODO: this is unbelievably broken, needs to be properly async
    blankSocket.Open(d->socketServer);
    TRequestStatus status;
    d->nativeSocket.Accept(blankSocket, status);
    User::WaitForRequest(status);
    if(status.Int()) {
        qWarning("QSymbianSocketEnginePrivate::nativeAccept() - error %d", status.Int());
        return 0;
    }
    // FIXME Qt Handle of new socket must be retrieved from QSymbianSocketManager
    // and then returned. Not the SubSessionHandle! Also set the socket to nonblocking.
    return blankSocket.SubSessionHandle();
}

qint64 QSymbianSocketEngine::bytesAvailable() const
{
    Q_D(const QSymbianSocketEngine);
    int nbytes = 0;
    qint64 available = 0;
    // FIXME is this the right thing also for UDP?
    // What is expected for UDP, the length for the next packet I guess?
    TInt err = d->nativeSocket.GetOpt(KSOReadBytesPending, KSOLSocket, nbytes);
    if(err)
        return 0;
    available = (qint64) nbytes;

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeBytesAvailable() == %lli", available);
#endif
    return available;
}

bool QSymbianSocketEngine::hasPendingDatagrams() const
{
    Q_D(const QSymbianSocketEngine);
    int nbytes;
    TInt err = d->nativeSocket.GetOpt(KSOReadBytesPending,KSOLSocket, nbytes);
    return err == KErrNone && nbytes > 0;
    //TODO: this is pretty horrible too...
    // FIXME why?
}

qint64 QSymbianSocketEngine::pendingDatagramSize() const
{
    Q_D(const QSymbianSocketEngine);
    int nbytes;
    TInt err = d->nativeSocket.GetOpt(KSOReadBytesPending,KSOLSocket, nbytes);
    return qint64(nbytes-28); //TODO: why -28 (open C version had this)?
    // Why = Could it be that this is about header lengths etc? if yes
    // this could be pretty broken, especially for IPv6
}


qint64 QSymbianSocketEngine::readDatagram(char *data, qint64 maxSize,
                                                    QHostAddress *address, quint16 *port)
{
    Q_D(QSymbianSocketEngine);
    TPtr8 buffer((TUint8*)data, (int)maxSize);
    TInetAddr addr;
    TRequestStatus status; //TODO: OMG sync receive!
    d->nativeSocket.RecvFrom(buffer, addr, 0, status);
    User::WaitForRequest(status);

    if (status.Int()) {
        d->setError(QAbstractSocket::NetworkError, ReceiveDatagramErrorString);
    } else if (port || address) {
        d->getPortAndAddress(addr, port, address);
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    int len = buffer.Length();
    qDebug("QSymbianSocketEnginePrivate::nativeReceiveDatagram(%p \"%s\", %lli, %s, %i) == %lli",
           data, qt_prettyDebug(data, qMin(len, ssize_t(16)), len).data(), maxSize,
           address ? address->toString().toLatin1().constData() : "(nil)",
           port ? *port : 0, (qint64) len);
#endif

    if (status.Int())
        return -1;
    return qint64(buffer.Length());
}


qint64 QSymbianSocketEngine::writeDatagram(const char *data, qint64 len,
                                                   const QHostAddress &host, quint16 port)
{
    Q_D(QSymbianSocketEngine);
    TPtrC8 buffer((TUint8*)data, (int)len);
    TInetAddr addr;
    d->setPortAndAddress(addr, port, host);
    TSockXfrLength sentBytes;
    TRequestStatus status; //TODO: OMG sync send!
    d->nativeSocket.SendTo(buffer, addr, 0, status, sentBytes);
    User::WaitForRequest(status);
    TInt err = status.Int(); 

    if (err) {
        switch (err) {
        case KErrTooBig:
            setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
            break;
        default:
            setError(QAbstractSocket::NetworkError, SendDatagramErrorString);
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::sendDatagram(%p \"%s\", %lli, \"%s\", %i) == %lli", data,
           qt_prettyDebug(data, qMin<int>(len, 16), len).data(), len, host.toString().toLatin1().constData(),
           port, (qint64) sentBytes());
#endif

    return qint64(sentBytes());
}

// FIXME check where the native socket engine called that..
bool QSymbianSocketEnginePrivate::fetchConnectionParameters()
{
    localPort = 0;
    localAddress.clear();
    peerPort = 0;
    peerAddress.clear();

    if (socketDescriptor == -1)
        return false;

    if (!nativeSocket.SubSessionHandle()) {
        if (!QSymbianSocketManager::instance().lookupSocket(socketDescriptor, nativeSocket))
            return false;
    }

    // Determine local address
    TSockAddr addr;
    nativeSocket.LocalName(addr);
    getPortAndAddress(addr, &localPort, &localAddress);

    // Determine protocol family
    switch (addr.Family()) {
    case KAfInet:
        socketProtocol = QAbstractSocket::IPv4Protocol;
        break;
#if !defined (QT_NO_IPV6)
    case KAfInet6:
        socketProtocol = QAbstractSocket::IPv6Protocol;
        break;
#endif
    default:
        socketProtocol = QAbstractSocket::UnknownNetworkLayerProtocol;
        break;
    }

    // Determine the remote address
    nativeSocket.RemoteName(addr);
    getPortAndAddress(addr, &peerPort, &peerAddress);

    // Determine the socket type (UDP/TCP)
    TProtocolDesc protocol;
    TInt err = nativeSocket.Info(protocol);
    if (err) {
        // ?
        // QAbstractSocket::UnknownSocketType;
    } else {
        switch (protocol.iProtocol) {
        case KProtocolInetTcp:
            socketType = QAbstractSocket::TcpSocket;
            break;
        case KProtocolInetUdp:
            socketType = QAbstractSocket::UdpSocket;
            break;
        default:
            socketType = QAbstractSocket::UnknownSocketType;
            break;
        }
    }
#if defined (QNATIVESOCKETENGINE_DEBUG)
    QString socketProtocolStr = QLatin1String("UnknownProtocol");
    if (socketProtocol == QAbstractSocket::IPv4Protocol) socketProtocolStr = QLatin1String("IPv4Protocol");
    else if (socketProtocol == QAbstractSocket::IPv6Protocol) socketProtocolStr = QLatin1String("IPv6Protocol");

    QString socketTypeStr = QLatin1String("UnknownSocketType");
    if (socketType == QAbstractSocket::TcpSocket) socketTypeStr = QLatin1String("TcpSocket");
    else if (socketType == QAbstractSocket::UdpSocket) socketTypeStr = QLatin1String("UdpSocket");

    qDebug("QSymbianSocketEnginePrivate::fetchConnectionParameters() local == %s:%i,"
           " peer == %s:%i, socket == %s - %s",
           localAddress.toString().toLatin1().constData(), localPort,
           peerAddress.toString().toLatin1().constData(), peerPort,socketTypeStr.toLatin1().constData(),
           socketProtocolStr.toLatin1().constData());
#endif
    return true;
}

void QSymbianSocketEngine::close()
{
    Q_D(QSymbianSocketEngine);
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeClose()");
#endif

    //TODO: call nativeSocket.Shutdown(EImmediate) in some cases?
    d->nativeSocket.Close();
    QSymbianSocketManager::instance().removeSocket(d->nativeSocket);

    // FIXME set nativeSocket to 0 ?
}

qint64 QSymbianSocketEngine::write(const char *data, qint64 len)
{
    Q_D(QSymbianSocketEngine);
    TPtrC8 buffer((TUint8*)data, (int)len);
    TSockXfrLength sentBytes;
    TRequestStatus status; //TODO: OMG sync send!
    d->nativeSocket.Send(buffer, 0, status, sentBytes);
    User::WaitForRequest(status);
    TInt err = status.Int(); 

    if (err) {
        switch (err) {
        case KErrDisconnected:
            sentBytes = -1;
            d->setError(QAbstractSocket::RemoteHostClosedError, RemoteHostClosedErrorString);
            close();
            break;
        case KErrTooBig:
            d->setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
            break;
        case KErrWouldBlock:
            sentBytes = 0;
        default:
            d->setError(QAbstractSocket::NetworkError, SendDatagramErrorString);
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeWrite(%p \"%s\", %llu) == %i",
           data, qt_prettyDebug(data, qMin((int) len, 16),
                                (int) len).data(), len, (int) sentBytes());
#endif

    return qint64(sentBytes());
}
/*
*/
qint64 QSymbianSocketEngine::read(char *data, qint64 maxSize)
{
    Q_D(QSymbianSocketEngine);
    if (!isValid()) {
        qWarning("QSymbianSocketEnginePrivate::nativeRead: Invalid socket");
        return -1;
    }

    TPtr8 buffer((TUint8*)data, (int)maxSize);
    TSockXfrLength received = 0;
    TRequestStatus status; //TODO: OMG sync receive!
    d->nativeSocket.RecvOneOrMore(buffer, 0, status, received);
    User::WaitForRequest(status);
    TInt err = status.Int();
    int r = received();

    if (err) {
        switch(err) {
        case KErrWouldBlock:
            // No data was available for reading
            r = -2;
            break;
        case KErrDisconnected:
            r = 0;
            break;
        default:
            r = -1;
            //error string is now set in read(), not here in nativeRead()
            break;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeRead(%p \"%s\", %llu) == %i",
           data, qt_prettyDebug(data, qMin(r, ssize_t(16)), r).data(),
           maxSize, r);
#endif

    return qint64(r);
}

// FIXME wait vs select
int QSymbianSocketEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
    bool readyRead = false;
    bool readyWrite = false;
    if (selectForRead)
        return nativeSelect(timeout, true, false, &readyRead, &readyWrite);
    else
        return nativeSelect(timeout, false, true, &readyRead, &readyWrite);
}

/*!
 \internal
 \param timeout timeout in milliseconds
 \param checkRead caller is interested if the socket is ready to read
 \param checkWrite caller is interested if the socket is ready for write
 \param selectForRead (out) should set to true if ready to read
 \param selectForWrite (out) should set to true if ready to write
 \return 0 on timeout, >0 on success, <0 on error
 */
int QSymbianSocketEnginePrivate::nativeSelect(int timeout, bool checkRead, bool checkWrite,
                       bool *selectForRead, bool *selectForWrite) const
{
    //TODO: implement
    //as above, but checking both read and write status at the same time
    if (!selectTimer.Handle())
        qt_symbian_throwIfError(selectTimer.CreateLocal());
    TRequestStatus timerStat;
    selectTimer.HighRes(timerStat, timeout * 1000);
    TRequestStatus* readStat = 0;
    TRequestStatus* writeStat = 0;
    TRequestStatus* array[3];
    array[0] = &timerStat;
    int count = 1;
    if (checkRead) {
        //TODO: get from read AO
        //readStat = ?
        array[count++] = readStat;
    }
    if (checkWrite) {
        //TODO: get from write AO
        //writeStat = ?
        array[count++] = writeStat;
    }
    // TODO: for selecting, we can use getOpt(KSOSelectPoll) to get the select result
    // and KIOCtlSelect for the selecting.

    User::WaitForNRequest(array, count);
    //IMPORTANT - WaitForNRequest only decrements the thread semaphore once, although more than one status may have completed.
    if (timerStat.Int() != KRequestPending) {
        //timed out
        return 0;
    }
    selectTimer.Cancel();
    User::WaitForRequest(timerStat);

    if(readStat && readStat->Int() != KRequestPending) {
        Q_ASSERT(checkRead && selectForRead);
        //TODO: cancel the AO, but call its RunL anyway? looking for an UnsetActive()
        *selectForRead = true;
    }
    if(writeStat && writeStat->Int() != KRequestPending) {
        Q_ASSERT(checkWrite && selectForWrite);
        //TODO: cancel the AO, but call its RunL anyway? looking for an UnsetActive()
        *selectForWrite = true;
    }
    return 1;
}

bool QSymbianSocketEngine::joinMulticastGroup(const QHostAddress &groupAddress,
                              const QNetworkInterface &iface)
{
    //TODO
    return false;
}

bool QSymbianSocketEngine::leaveMulticastGroup(const QHostAddress &groupAddress,
                               const QNetworkInterface &iface)
{
    //TODO
    return false;
}

QNetworkInterface QSymbianSocketEngine::multicastInterface() const
{
    //TODO
    return QNetworkInterface();
}

bool QSymbianSocketEngine::setMulticastInterface(const QNetworkInterface &iface)
{
    //TODO
    return false;
}

bool QSymbianSocketEnginePrivate::checkProxy(const QHostAddress &address)
{
    if (address == QHostAddress::LocalHost || address == QHostAddress::LocalHostIPv6)
        return true;

#if !defined(QT_NO_NETWORKPROXY)
    QObject *parent = q_func()->parent();
    QNetworkProxy proxy;
    if (QAbstractSocket *socket = qobject_cast<QAbstractSocket *>(parent)) {
        proxy = socket->proxy();
    } else if (QTcpServer *server = qobject_cast<QTcpServer *>(parent)) {
        proxy = server->proxy();
    } else {
        // no parent -> no proxy
        return true;
    }

    if (proxy.type() == QNetworkProxy::DefaultProxy)
        proxy = QNetworkProxy::applicationProxy();

    if (proxy.type() != QNetworkProxy::DefaultProxy &&
        proxy.type() != QNetworkProxy::NoProxy) {
        // QNativeSocketEngine doesn't do proxies
        setError(QAbstractSocket::UnsupportedSocketOperationError,
                 InvalidProxyTypeString);
        return false;
    }
#endif

    return true;
}

QT_END_NAMESPACE
