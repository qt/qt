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
#include "qnativesocketengine_p.h"
#include "private/qnet_unix_p.h"
#include "qiodevice.h"
#include "qhostaddress.h"
#include "qelapsedtimer.h"
#include "qvarlengtharray.h"
#include <es_sock.h>
#include <in_sock.h>
#ifndef QT_NO_IPV6IFNAME
#include <net/if.h>
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

static void qt_ignore_sigpipe()
{
#ifndef Q_NO_POSIX_SIGNALS
    // Set to ignore SIGPIPE once only.
    static QBasicAtomicInt atom = Q_BASIC_ATOMIC_INITIALIZER(0);
    if (atom.testAndSetRelaxed(0, 1)) {
        struct sigaction noaction;
        memset(&noaction, 0, sizeof(noaction));
        noaction.sa_handler = SIG_IGN;
        ::sigaction(SIGPIPE, &noaction, 0);
    }
#else
    // Posix signals are not supported by the underlying platform
    // so we don't need to ignore sigpipe signal explicitly
#endif
}

/*
    Extracts the port and address from a sockaddr, and stores them in
    \a port and \a addr if they are non-null.
*/
static inline void qt_socket_getPortAndAddress(const qt_sockaddr *s, quint16 *port, QHostAddress *addr)
{
#if !defined(QT_NO_IPV6)
    if (s->a.sa_family == AF_INET6) {
        Q_IPV6ADDR tmp;
        memcpy(&tmp, &s->a6.sin6_addr, sizeof(tmp));
        if (addr) {
            QHostAddress tmpAddress;
            tmpAddress.setAddress(tmp);
            *addr = tmpAddress;
#ifndef QT_NO_IPV6IFNAME
            char scopeid[IFNAMSIZ];
            if (::if_indextoname(s->a6.sin6_scope_id, scopeid)) {
                addr->setScopeId(QLatin1String(scopeid));
            } else
#endif
            addr->setScopeId(QString::number(s->a6.sin6_scope_id));
        }
        if (port)
            *port = ntohs(s->a6.sin6_port);
        return;
    }
#endif
    if (port)
        *port = ntohs(s->a4.sin_port);
    if (addr) {
        QHostAddress tmpAddress;
        tmpAddress.setAddress(ntohl(s->a4.sin_addr.s_addr));
        *addr = tmpAddress;
    }
}

static inline void qt_socket_getPortAndAddress(const TInetAddr& a, quint16 *port, QHostAddress *addr)
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
            char scopeid[IFNAMSIZ];
            //TODO: rather than using posix api, the symbian way is
            //to use GetOpt with TSoInetIfQuery and KSoInetIfQueryByIndex
            //which means this should be in a member function to have access to the nativeSocket
            if (::if_indextoname(a.Scope(), scopeid)) {
                addr->setScopeId(QLatin1String(scopeid));
            } else
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
        tmpAddress.setAddress(a.Address()); //TODO: byte order ok?
        *addr = tmpAddress;
    }
}
/*! \internal

    Creates and returns a new socket descriptor of type \a socketType
    and \a socketProtocol.  Returns -1 on failure.
*/
bool QNativeSocketEnginePrivate::createNewSocket(QAbstractSocket::SocketType socketType,
                                         QAbstractSocket::NetworkLayerProtocol socketProtocol)
{
#ifndef QT_NO_IPV6
    TUint family = (socketProtocol == QAbstractSocket::IPv6Protocol) ? KAfInet6 : KAfInet;
#else
    Q_UNUSED(socketProtocol);
    TUint family = KAfInet;
#endif
    TUint type = (socketType == QAbstractSocket::UdpSocket) ? KSockDatagram : KSockStream;
    TUint protocol = (socketType == QAbstractSocket::UdpSocket) ? KProtocolInetUdp : KProtocolInetTcp;
    TInt err = nativeSocket.Open(socketServer, family, type, protocol, connection);

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

    socketDescriptor = nativeSocket.SubSessionHandle(); //TODO
    return true;
}

/*
    Returns the value of the socket option \a opt.
*/
int QNativeSocketEnginePrivate::option(QNativeSocketEngine::SocketOption opt) const
{
    Q_Q(const QNativeSocketEngine);
    if (!q->isValid())
        return -1;

    TUint n;
    TUint level = KSOLSocket; // default

    switch (opt) {
    case QNativeSocketEngine::ReceiveBufferSocketOption:
        n = KSORecvBuf;
        break;
    case QNativeSocketEngine::SendBufferSocketOption:
        n = KSOSendBuf;
        break;
    case QNativeSocketEngine::NonBlockingSocketOption:
        n = KSONonBlockingIO;
        break;
    case QNativeSocketEngine::BroadcastSocketOption:
        n = SO_BROADCAST; //TODO
        break;
    case QNativeSocketEngine::AddressReusable:
        n = SO_REUSEADDR; //TODO
        break;
    case QNativeSocketEngine::BindExclusively:
        return true;
    case QNativeSocketEngine::ReceiveOutOfBandData:
        n = SO_OOBINLINE; //TODO
        break;
    case QNativeSocketEngine::LowDelayOption:
        level = KSolInetTcp;
        n = KSoTcpNoDelay;
        break;
    case QNativeSocketEngine::KeepAliveOption:
        level = KSolInetTcp;
        n = KSoTcpKeepAlive;
        break;
    default:
        return -1;
    }

    int v = -1;
    //GetOpt() is non const
    TInt err = nativeSocket.GetOpt(n, level, v);
    if (!err)
        return v;

    return -1;
}


/*
    Sets the socket option \a opt to \a v.
*/
bool QNativeSocketEnginePrivate::setOption(QNativeSocketEngine::SocketOption opt, int v)
{
    Q_Q(QNativeSocketEngine);
    if (!q->isValid())
        return false;

    int n = 0;
    int level = SOL_SOCKET; // default

    switch (opt) {
    case QNativeSocketEngine::ReceiveBufferSocketOption:
        n = KSORecvBuf;
        break;
    case QNativeSocketEngine::SendBufferSocketOption:
        n = KSOSendBuf;
        break;
    case QNativeSocketEngine::BroadcastSocketOption:
        n = SO_BROADCAST; //TODO
        break;
    case QNativeSocketEngine::NonBlockingSocketOption:
        n = KSONonBlockingIO;
        break;
    case QNativeSocketEngine::AddressReusable:
        n = SO_REUSEADDR; //TODO
        break;
    case QNativeSocketEngine::BindExclusively:
        return true;
    case QNativeSocketEngine::ReceiveOutOfBandData:
        n = SO_OOBINLINE; //TODO
        break;
    case QNativeSocketEngine::LowDelayOption:
        level = KSolInetTcp;
        n = KSoTcpNoDelay;
        break;
    case QNativeSocketEngine::KeepAliveOption:
        level = KSolInetTcp;
        n = KSoTcpKeepAlive;
        break;
    }

    return (KErrNone == nativeSocket.SetOpt(n, level, v));
}

static TInetAddr qt_QHostAddressToTInetAddr(const QHostAddress &addr)
{
    TInetAddr nativeAddr;
#if !defined(QT_NO_IPV6)
    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
#ifndef QT_NO_IPV6IFNAME
        nativeAddr.SetScope(::if_nametoindex(addr.scopeId().toLatin1().data())); //TODO - if_nametoindex
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
    return nativeAddr;
}

bool QNativeSocketEnginePrivate::nativeConnect(const QHostAddress &addr, quint16 port)
{
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug("QNativeSocketEnginePrivate::nativeConnect() : %d ", socketDescriptor);
#endif

    TInetAddr nativeAddr = qt_QHostAddressToTInetAddr(addr);
    nativeAddr.SetPort(port);
    //TODO: async connect with active object - from here to end of function is a mess
    TRequestStatus status;
    nativeSocket.Connect(nativeAddr, status);
    User::WaitForRequest(status);
    TInt err = status.Int();
    if (err) {
        switch (err) {
        case KErrCouldNotConnect:
            setError(QAbstractSocket::ConnectionRefusedError, ConnectionRefusedErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrTimedOut:
            setError(QAbstractSocket::NetworkError, ConnectionTimeOutErrorString);
            break;
        case KErrHostUnreach:
            setError(QAbstractSocket::NetworkError, HostUnreachableErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNetUnreach:
            setError(QAbstractSocket::NetworkError, NetworkUnreachableErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrInUse:
            setError(QAbstractSocket::NetworkError, AddressInuseErrorString);
            break;
        case KErrPermissionDenied:
            setError(QAbstractSocket::SocketAccessError, AccessErrorString);
            socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNotSupported:
        case KErrBadDescriptor:
            socketState = QAbstractSocket::UnconnectedState;
        default:
            break;
        }

        if (socketState != QAbstractSocket::ConnectedState) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
            qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == false (%s)",
                   addr.toString().toLatin1().constData(), port,
                   socketState == QAbstractSocket::ConnectingState
                   ? "Connection in progress" : socketErrorString.toLatin1().constData());
#endif
            return false;
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeConnect(%s, %i) == true",
           addr.toString().toLatin1().constData(), port);
#endif

    socketState = QAbstractSocket::ConnectedState;
    return true;
}

bool QNativeSocketEnginePrivate::nativeBind(const QHostAddress &address, quint16 port)
{
    TInetAddr nativeAddr = qt_QHostAddressToTInetAddr(address);
    nativeAddr.SetPort(port);

    TInt err = nativeSocket.Bind(nativeAddr);

    if (err) {
        switch(errno) {
        case KErrInUse:
            setError(QAbstractSocket::AddressInUseError, AddressInuseErrorString);
            break;
        case KErrPermissionDenied:
            setError(QAbstractSocket::SocketAccessError, AddressProtectedErrorString);
            break;
        case KErrNotSupported:
            setError(QAbstractSocket::UnsupportedSocketOperationError, OperationUnsupportedErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QNativeSocketEnginePrivate::nativeBind(%s, %i) == false (%s)",
               address.toString().toLatin1().constData(), port, socketErrorString.toLatin1().constData());
#endif

        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeBind(%s, %i) == true",
           address.toString().toLatin1().constData(), port);
#endif
    socketState = QAbstractSocket::BoundState;
    return true;
}

bool QNativeSocketEnginePrivate::nativeListen(int backlog)
{
    TInt err = nativeSocket.Listen(backlog);
    if (err) {
        switch (errno) {
        case KErrInUse:
            setError(QAbstractSocket::AddressInUseError,
                     PortInuseErrorString);
            break;
        default:
            break;
        }

#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QNativeSocketEnginePrivate::nativeListen(%i) == false (%s)",
               backlog, socketErrorString.toLatin1().constData());
#endif
        return false;
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeListen(%i) == true", backlog);
#endif

    socketState = QAbstractSocket::ListeningState;
    return true;
}

int QNativeSocketEnginePrivate::nativeAccept()
{
    RSocket blankSocket;
    //TODO: this is unbelievably broken, needs to be properly async
    blankSocket.Open(socketServer);
    TRequestStatus status;
    nativeSocket.Accept(blankSocket, status);
    User::WaitForRequest(status);
    if(status.Int()) {
        qWarning("QNativeSocketEnginePrivate::nativeAccept() - error %d", status.Int());
        return 0;
    }

    return blankSocket.SubSessionHandle();
}

qint64 QNativeSocketEnginePrivate::nativeBytesAvailable() const
{
    int nbytes = 0;
    qint64 available = 0;
    TInt err = nativeSocket.GetOpt(KSOReadBytesPending, KSOLSocket, nbytes);
    if(err)
        return 0;
    available = (qint64) nbytes;

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeBytesAvailable() == %lli", available);
#endif
    return available;
}

bool QNativeSocketEnginePrivate::nativeHasPendingDatagrams() const
{
    int nbytes;
    TInt err = nativeSocket.GetOpt(KSOReadBytesPending,KSOLSocket, nbytes);
    return err == KErrNone && nbytes > 0;
    //TODO: this is pretty horrible too...
}

qint64 QNativeSocketEnginePrivate::nativePendingDatagramSize() const
{
    int nbytes;
    TInt err = nativeSocket.GetOpt(KSOReadBytesPending,KSOLSocket, nbytes);
    return qint64(nbytes-28); //TODO: why -28 (open C version had this)?
}

qint64 QNativeSocketEnginePrivate::nativeReceiveDatagram(char *data, qint64 maxSize,
                                                    QHostAddress *address, quint16 *port)
{
    TPtr8 buffer((TUint8*)data, (int)maxSize);
    TInetAddr addr;
    TRequestStatus status; //TODO: OMG sync receive!
    nativeSocket.RecvFrom(buffer, addr, 0, status);
    User::WaitForRequest(status);

    if (status.Int()) {
        setError(QAbstractSocket::NetworkError, ReceiveDatagramErrorString);
    } else if (port || address) {
        qt_socket_getPortAndAddress(addr, port, address);
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    int len = buffer.Length();
    qDebug("QNativeSocketEnginePrivate::nativeReceiveDatagram(%p \"%s\", %lli, %s, %i) == %lli",
           data, qt_prettyDebug(data, qMin(len, ssize_t(16)), len).data(), maxSize,
           address ? address->toString().toLatin1().constData() : "(nil)",
           port ? *port : 0, (qint64) len);
#endif

    if (status.Int())
        return -1;
    return qint64(buffer.Length());
}

qint64 QNativeSocketEnginePrivate::nativeSendDatagram(const char *data, qint64 len,
                                                   const QHostAddress &host, quint16 port)
{
    TPtrC8 buffer((TUint8*)data, (int)len);
    TInetAddr addr = qt_QHostAddressToTInetAddr(host);
    TSockXfrLength sentBytes;
    addr.SetPort(port);
    TRequestStatus status; //TODO: OMG sync send!
    nativeSocket.SendTo(buffer, addr, 0, status, sentBytes);
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
    qDebug("QNativeSocketEngine::sendDatagram(%p \"%s\", %lli, \"%s\", %i) == %lli", data,
           qt_prettyDebug(data, qMin<int>(len, 16), len).data(), len, host.toString().toLatin1().constData(),
           port, (qint64) sentBytes());
#endif

    return qint64(sentBytes());
}

bool QNativeSocketEnginePrivate::fetchConnectionParameters()
{
    localPort = 0;
    localAddress.clear();
    peerPort = 0;
    peerAddress.clear();

    if (socketDescriptor == -1)
        return false;

    //TODO: work out how to initialise nativeSocket from socketDescriptor

    // Determine local address
    TSockAddr addr;
    nativeSocket.LocalName(addr);
    qt_socket_getPortAndAddress(addr, &localPort, &localAddress);

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
    qt_socket_getPortAndAddress(addr, &peerPort, &peerAddress);

    // Determine the socket type (UDP/TCP)
    TProtocolDesc protocol;
    TInt err = nativeSocket.Info(protocol);
    if (err) {
        QAbstractSocket::UnknownSocketType;
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
    QString socketProtocolStr = "UnknownProtocol";
    if (socketProtocol == QAbstractSocket::IPv4Protocol) socketProtocolStr = "IPv4Protocol";
    else if (socketProtocol == QAbstractSocket::IPv6Protocol) socketProtocolStr = "IPv6Protocol";

    QString socketTypeStr = "UnknownSocketType";
    if (socketType == QAbstractSocket::TcpSocket) socketTypeStr = "TcpSocket";
    else if (socketType == QAbstractSocket::UdpSocket) socketTypeStr = "UdpSocket";

    qDebug("QNativeSocketEnginePrivate::fetchConnectionParameters() local == %s:%i,"
           " peer == %s:%i, socket == %s - %s",
           localAddress.toString().toLatin1().constData(), localPort,
           peerAddress.toString().toLatin1().constData(), peerPort,socketTypeStr.toLatin1().constData(),
           socketProtocolStr.toLatin1().constData());
#endif
    return true;
}

void QNativeSocketEnginePrivate::nativeClose()
{
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEngine::nativeClose()");
#endif

    //TODO: call nativeSocket.Shutdown(EImmediate) in some cases?
    nativeSocket.Close();
}

qint64 QNativeSocketEnginePrivate::nativeWrite(const char *data, qint64 len)
{
    Q_Q(QNativeSocketEngine);
    TPtrC8 buffer((TUint8*)data, (int)len);
    TSockXfrLength sentBytes;
    TRequestStatus status; //TODO: OMG sync send!
    nativeSocket.Send(buffer, 0, status, sentBytes);
    User::WaitForRequest(status);
    TInt err = status.Int(); 

    if (err) {
        switch (err) {
        case KErrDisconnected:
            sentBytes = -1;
            setError(QAbstractSocket::RemoteHostClosedError, RemoteHostClosedErrorString);
            q->close();
            break;
        case KErrTooBig:
            setError(QAbstractSocket::DatagramTooLargeError, DatagramTooLargeErrorString);
            break;
        case KErrWouldBlock:
            sentBytes = 0;
        default:
            setError(QAbstractSocket::NetworkError, SendDatagramErrorString);
        }
    }

#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QNativeSocketEnginePrivate::nativeWrite(%p \"%s\", %llu) == %i",
           data, qt_prettyDebug(data, qMin((int) len, 16),
                                (int) len).data(), len, (int) sentBytes());
#endif

    return qint64(sentBytes());
}
/*
*/
qint64 QNativeSocketEnginePrivate::nativeRead(char *data, qint64 maxSize)
{
    Q_Q(QNativeSocketEngine);
    if (!q->isValid()) {
        qWarning("QNativeSocketEngine::nativeRead: Invalid socket");
        return -1;
    }

    TPtr8 buffer((TUint8*)data, (int)maxSize);
    TSockXfrLength received = 0;
    TRequestStatus status; //TODO: OMG sync receive!
    nativeSocket.RecvOneOrMore(buffer, 0, status, received);
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

int QNativeSocketEnginePrivate::nativeSelect(int timeout, bool selectForRead) const
{
//TODO: implement
}

int QNativeSocketEnginePrivate::nativeSelect(int timeout, bool checkRead, bool checkWrite,
                       bool *selectForRead, bool *selectForWrite) const
{
    //TODO: implement
}

QT_END_NAMESPACE
