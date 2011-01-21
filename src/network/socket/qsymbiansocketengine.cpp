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
#include <net/if.h>

#include <private/qcore_symbian_p.h>

#if !defined(QT_NO_NETWORKPROXY)
# include "qnetworkproxy.h"
# include "qabstractsocket.h"
# include "qtcpserver.h"
#endif

#include <qabstracteventdispatcher.h>
#include <qsocketnotifier.h>
#include <qnetworkinterface.h>

#include <private/qthread_p.h>
#include <private/qobject_p.h>

#define QNATIVESOCKETENGINE_DEBUG

#if defined QNATIVESOCKETENGINE_DEBUG
#include <qstring.h>
#include <ctype.h>
#endif

QT_BEGIN_NAMESPACE

// Common constructs
#define Q_CHECK_VALID_SOCKETLAYER(function, returnValue) do { \
    if (!isValid()) { \
        qWarning(""#function" was called on an uninitialized socket device"); \
        return returnValue; \
    } } while (0)
#define Q_CHECK_INVALID_SOCKETLAYER(function, returnValue) do { \
    if (isValid()) { \
        qWarning(""#function" was called on an already initialized socket device"); \
        return returnValue; \
    } } while (0)
#define Q_CHECK_STATE(function, checkState, returnValue) do { \
    if (d->socketState != (checkState)) { \
        qWarning(""#function" was not called in "#checkState); \
        return (returnValue); \
    } } while (0)
#define Q_CHECK_NOT_STATE(function, checkState, returnValue) do { \
    if (d->socketState == (checkState)) { \
        qWarning(""#function" was called in "#checkState); \
        return (returnValue); \
    } } while (0)
#define Q_CHECK_STATES(function, state1, state2, returnValue) do { \
    if (d->socketState != (state1) && d->socketState != (state2)) { \
        qWarning(""#function" was called" \
                 " not in "#state1" or "#state2); \
        return (returnValue); \
    } } while (0)
#define Q_CHECK_TYPE(function, type, returnValue) do { \
    if (d->socketType != (type)) { \
        qWarning(#function" was called by a" \
                 " socket other than "#type""); \
        return (returnValue); \
    } } while (0)

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
    if (a.Family() == KAfInet6) {
        Q_IPV6ADDR tmp;
        memcpy(&tmp, a.Ip6Address().u.iAddr8, sizeof(tmp));
        if (addr) {
            QHostAddress tmpAddress;
            tmpAddress.setAddress(tmp);
            *addr = tmpAddress;
            TPckgBuf<TSoInetIfQuery> query;
            query().iSrcAddr = a;
            TInt err = nativeSocket.GetOpt(KSoInetIfQueryBySrcAddr, KSolInetIfQuery, query);
            if (!err)
                addr->setScopeId(qt_TDesC2QString(query().iName));
            else
            addr->setScopeId(QString::number(a.Scope()));
        }
        if (port)
            *port = a.Port();
        return;
    }
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
    TUint family = KAfInet; // KAfInet6 is only used as an address family, not as a protocol family
    TUint type = (socketType == QAbstractSocket::UdpSocket) ? KSockDatagram : KSockStream;
    TUint protocol = (socketType == QAbstractSocket::UdpSocket) ? KProtocolInetUdp : KProtocolInetTcp;
    TInt err;
    if (connection)
        err = nativeSocket.Open(socketServer, family, type, protocol, *connection);
    else
        err = nativeSocket.Open(socketServer, family, type, protocol); //TODO: FIXME - deprecated API, make sure we always have a connection instead

    //TODO: combine error handling with setError
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
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << "QSymbianSocketEnginePrivate::createNewSocket - created" << nativeSocket.SubSessionHandle();
#endif
    socketDescriptor = QSymbianSocketManager::instance().addSocket(nativeSocket);
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << " - allocated socket descriptor" << socketDescriptor;
#endif
    return true;
}

void QSymbianSocketEnginePrivate::setPortAndAddress(TInetAddr& nativeAddr, quint16 port, const QHostAddress &addr)
{
    nativeAddr.SetPort(port);
    if (addr.protocol() == QAbstractSocket::IPv6Protocol) {
        TPckgBuf<TSoInetIfQuery> query;
        query().iName = qt_QString2TPtrC(addr.scopeId());
        TInt err = nativeSocket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, query);
        if (!err)
            nativeAddr.SetScope(query().iIndex);
        else
            nativeAddr.SetScope(0);
        Q_IPV6ADDR ip6 = addr.toIPv6Address();
        TIp6Addr v6addr;
        memcpy(v6addr.u.iAddr8, ip6.c, 16);
        nativeAddr.SetAddress(v6addr);
    } else if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
        nativeAddr.SetAddress(addr.toIPv4Address());
    } else {
        qWarning("unsupported network protocol (%d)", addr.protocol());
    }
}

QSymbianSocketEnginePrivate::QSymbianSocketEnginePrivate() :
    socketDescriptor(-1),
    socketServer(QSymbianSocketManager::instance().getSocketServer()),
    connection(QSymbianSocketManager::instance().defaultConnection()),
    readNotifier(0),
    writeNotifier(0),
    exceptNotifier(0),
    asyncSelect(0)
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

/*!
    Initializes a QSymbianSocketEngine by creating a new socket of type \a
    socketType and network layer protocol \a protocol. Returns true on
    success; otherwise returns false.

    If the socket was already initialized, this function closes the
    socket before reeinitializing it.

    The new socket is non-blocking, and for UDP sockets it's also
    broadcast enabled.
*/
bool QSymbianSocketEngine::initialize(QAbstractSocket::SocketType socketType, QAbstractSocket::NetworkLayerProtocol protocol)
{
    Q_D(QSymbianSocketEngine);
    if (isValid())
        close();

    // Create the socket
    if (!d->createNewSocket(socketType, protocol)) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
        QString typeStr = QLatin1String("UnknownSocketType");
        if (socketType == QAbstractSocket::TcpSocket) typeStr = QLatin1String("TcpSocket");
        else if (socketType == QAbstractSocket::UdpSocket) typeStr = QLatin1String("UdpSocket");
        QString protocolStr = QLatin1String("UnknownProtocol");
        if (protocol == QAbstractSocket::IPv4Protocol) protocolStr = QLatin1String("IPv4Protocol");
        else if (protocol == QAbstractSocket::IPv6Protocol) protocolStr = QLatin1String("IPv6Protocol");
        qDebug("QNativeSocketEngine::initialize(type == %s, protocol == %s) failed: %s",
               typeStr.toLatin1().constData(), protocolStr.toLatin1().constData(), d->socketErrorString.toLatin1().constData());
#endif
        return false;
    }

    // Make the socket nonblocking.
    if (!setOption(NonBlockingSocketOption, 1)) {
        d->setError(QAbstractSocket::UnsupportedSocketOperationError,
                    d->NonBlockingInitFailedErrorString);
        close();
        return false;
    }

    // Set the broadcasting flag if it's a UDP socket.
    if (socketType == QAbstractSocket::UdpSocket
        && !setOption(BroadcastSocketOption, 1)) {
        d->setError(QAbstractSocket::UnsupportedSocketOperationError,
                    d->BroadcastingInitFailedErrorString);
        close();
        return false;
    }


    // Make sure we receive out-of-band data
    if (socketType == QAbstractSocket::TcpSocket
        && !setOption(ReceiveOutOfBandData, 1)) {
        qWarning("QNativeSocketEngine::initialize unable to inline out-of-band data");
    }


    d->socketType = socketType;
    d->socketProtocol = protocol;
    return true;
}

/*! \overload

    Initializes the socket using \a socketDescriptor instead of
    creating a new one. The socket type and network layer protocol are
    determined automatically. The socket's state is set to \a
    socketState.

    If the socket type is either TCP or UDP, it is made non-blocking.
    UDP sockets are also broadcast enabled.
 */
bool QSymbianSocketEngine::initialize(int socketDescriptor, QAbstractSocket::SocketState socketState)
{
    Q_D(QSymbianSocketEngine);

    if (isValid())
        close();

    if (!QSymbianSocketManager::instance().lookupSocket(socketDescriptor, d->nativeSocket)) {
        qWarning("QSymbianSocketEngine::initialize - socket descriptor not found");
        d->setError(QAbstractSocket::SocketResourceError,
            QSymbianSocketEnginePrivate::InvalidSocketErrorString);
        return false;
    }
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << "QSymbianSocketEngine::initialize - attached to" << d->nativeSocket.SubSessionHandle() << socketDescriptor;
#endif
    Q_ASSERT(d->socketDescriptor == socketDescriptor || d->socketDescriptor == -1);
    d->socketDescriptor = socketDescriptor;

    // determine socket type and protocol
    if (!d->fetchConnectionParameters()) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
        qDebug("QNativeSocketEngine::initialize(socketDescriptor == %i) failed: %s",
               socketDescriptor, d->socketErrorString.toLatin1().constData());
#endif
        d->socketDescriptor = -1;
        return false;
    }

    if (d->socketType != QAbstractSocket::UnknownSocketType) {
        // Make the socket nonblocking.
        if (!setOption(NonBlockingSocketOption, 1)) {
            d->setError(QAbstractSocket::UnsupportedSocketOperationError,
                d->NonBlockingInitFailedErrorString);
            close();
            return false;
        }

        // Set the broadcasting flag if it's a UDP socket.
        if (d->socketType == QAbstractSocket::UdpSocket
            && !setOption(BroadcastSocketOption, 1)) {
            d->setError(QAbstractSocket::UnsupportedSocketOperationError,
                d->BroadcastingInitFailedErrorString);
            close();
            return false;
        }

        // Make sure we receive out-of-band data
        if (d->socketType == QAbstractSocket::TcpSocket
            && !setOption(ReceiveOutOfBandData, 1)) {
            qWarning("QNativeSocketEngine::initialize unable to inline out-of-band data");
        }
    }

    d->socketState = socketState;
    return true;
}

/*!
    Returns true if the socket is valid; otherwise returns false. A
    socket is valid if it has not been successfully initialized, or if
    it has been closed.
*/
bool QSymbianSocketEngine::isValid() const
{
    Q_D(const QSymbianSocketEngine);
    return d->socketDescriptor != -1;
}


/*!
    Returns the native socket descriptor. Any use of this descriptor
    stands the risk of being non-portable.
*/
int QSymbianSocketEngine::socketDescriptor() const
{
    Q_D(const QSymbianSocketEngine);
    return d->socketDescriptor;
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

qint64 QSymbianSocketEngine::receiveBufferSize() const
{
    return option(ReceiveBufferSocketOption);
}

void QSymbianSocketEngine::setReceiveBufferSize(qint64 size)
{
    setOption(ReceiveBufferSocketOption, size);
}

qint64 QSymbianSocketEngine::sendBufferSize() const
{
    return option(SendBufferSocketOption);
}

void QSymbianSocketEngine::setSendBufferSize(qint64 size)
{
    setOption(SendBufferSocketOption, size);
}

bool QSymbianSocketEngine::connectToHostByName(const QString &name, quint16 port)
{
    // FIXME for engines that support hostnames.. not for us then i guess.

    return false;
}

/*!
    If there's a connection activity on the socket, process it. Then
    notify our parent if there really was activity.
*/
void QSymbianSocketEngine::connectionNotification()
{
    // FIXME check if we really need to do it like that in Symbian
    Q_D(QSymbianSocketEngine);
    Q_ASSERT(state() == QAbstractSocket::ConnectingState);

    connectToHost(d->peerAddress, d->peerPort);
    if (state() != QAbstractSocket::ConnectingState) {
        // we changed states
        QAbstractSocketEngine::connectionNotification();
    }
}


bool QSymbianSocketEngine::connectToHost(const QHostAddress &addr, quint16 port)
{
    Q_D(QSymbianSocketEngine);

#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug("QSymbianSocketEngine::connectToHost() : %d ", d->socketDescriptor);
#endif

    if (!d->checkProxy(addr))
        return false;

    d->peerAddress = addr;
    d->peerPort = port;

    TInetAddr nativeAddr;
    d->setPortAndAddress(nativeAddr, port, addr);
    TRequestStatus status;
    d->nativeSocket.Connect(nativeAddr, status);
    User::WaitForRequest(status);
    TInt err = status.Int();
    //TODO: combine with setError(int)
    //For non blocking connect, KErrAlreadyExists is returned from the second Connect() to indicate
    //the connection is up. So treat this the same as KErrNone which would be returned from the first
    //call if it wouldn't block. (e.g. winsock wrapper in the emulator ignores the nonblocking flag)
    if (err && err != KErrAlreadyExists) {
        switch (err) {
        case KErrWouldBlock:
            d->socketState = QAbstractSocket::ConnectingState;
            break;
        case KErrCouldNotConnect:
            d->setError(QAbstractSocket::ConnectionRefusedError, d->ConnectionRefusedErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrTimedOut:
            d->setError(QAbstractSocket::NetworkError, d->ConnectionTimeOutErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrHostUnreach:
            d->setError(QAbstractSocket::NetworkError, d->HostUnreachableErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNetUnreach:
            d->setError(QAbstractSocket::NetworkError, d->NetworkUnreachableErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrInUse:
            d->setError(QAbstractSocket::NetworkError, d->AddressInuseErrorString);
            break;
        case KErrPermissionDenied:
            d->setError(QAbstractSocket::SocketAccessError, d->AccessErrorString);
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        case KErrNotSupported:
        case KErrBadDescriptor:
        default:
            d->socketState = QAbstractSocket::UnconnectedState;
            break;
        }

        if (d->socketState != QAbstractSocket::ConnectedState) {
#if defined (QNATIVESOCKETENGINE_DEBUG)
            qDebug("QSymbianSocketEngine::connectToHost(%s, %i) == false (%s)",
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
    Q_CHECK_VALID_SOCKETLAYER(QNativeSocketEngine::bind(), false);

    if (!d->checkProxy(address))
        return false;

    Q_CHECK_STATE(QNativeSocketEngine::bind(), QAbstractSocket::UnconnectedState, false);

    TInetAddr nativeAddr;
    d->setPortAndAddress(nativeAddr, port, address);

    TInt err = d->nativeSocket.Bind(nativeAddr);
#ifdef __WINS__
    if (err == KErrArgument) // winsock prt returns wrong error code
        err = KErrInUse;
#endif

    if (err) {
        d->setError(err);

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

    d->fetchConnectionParameters();
    return true;
}

bool QSymbianSocketEngine::listen()
{
    Q_D(QSymbianSocketEngine);
    // TODO the value 50 is from the QNativeSocketEngine. Maybe it's a bit too much
    // for a mobile platform
    TInt err = d->nativeSocket.Listen(50);
    if (err) {
        //TODO: combine with setError(int)
        switch (err) {
        case KErrInUse:
            d->setError(QAbstractSocket::AddressInUseError,
                     d->PortInuseErrorString);
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
    Q_CHECK_VALID_SOCKETLAYER(QSymbianSocketEngine::accept(), -1);
    Q_CHECK_STATE(QSymbianSocketEngine::accept(), QAbstractSocket::ListeningState, false);
    Q_CHECK_TYPE(QSymbianSocketEngine::accept(), QAbstractSocket::TcpSocket, false);
    RSocket blankSocket;
    blankSocket.Open(d->socketServer);
    TRequestStatus status;
    d->nativeSocket.Accept(blankSocket, status);
    User::WaitForRequest(status);
    if (status.Int()) {
        blankSocket.Close();
        qWarning("QSymbianSocketEnginePrivate::nativeAccept() - error %d", status.Int());
        return -1;
    }

#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << "QSymbianSocketEnginePrivate::accept - created" << blankSocket.SubSessionHandle();
#endif
    int fd = QSymbianSocketManager::instance().addSocket(blankSocket);
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << " - allocated socket descriptor" << fd;
#endif
    return fd;
}

qint64 QSymbianSocketEngine::bytesAvailable() const
{
    Q_D(const QSymbianSocketEngine);
    Q_CHECK_VALID_SOCKETLAYER(QNativeSocketEngine::bytesAvailable(), -1);
    Q_CHECK_NOT_STATE(QNativeSocketEngine::bytesAvailable(), QAbstractSocket::UnconnectedState, false);
    int nbytes = 0;
    qint64 available = 0;
    TInt err = d->nativeSocket.GetOpt(KSOReadBytesPending, KSOLSocket, nbytes);
    if (err)
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
    Q_CHECK_VALID_SOCKETLAYER(QNativeSocketEngine::hasPendingDatagrams(), false);
    Q_CHECK_NOT_STATE(QNativeSocketEngine::hasPendingDatagrams(), QAbstractSocket::UnconnectedState, false);
    Q_CHECK_TYPE(QNativeSocketEngine::hasPendingDatagrams(), QAbstractSocket::UdpSocket, false);
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
    return qint64(nbytes);
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
        d->setError(QAbstractSocket::NetworkError, d->ReceiveDatagramErrorString);
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
    Q_CHECK_VALID_SOCKETLAYER(QNativeSocketEngine::writeDatagram(), -1);
    Q_CHECK_TYPE(QNativeSocketEngine::writeDatagram(), QAbstractSocket::UdpSocket, -1);
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
            d->setError(QAbstractSocket::DatagramTooLargeError, d->DatagramTooLargeErrorString);
            break;
        default:
            d->setError(QAbstractSocket::NetworkError, d->SendDatagramErrorString);
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
    case KAfInet6:
        socketProtocol = QAbstractSocket::IPv6Protocol;
        break;
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
    if (!isValid())
        return;
    Q_D(QSymbianSocketEngine);
#if defined (QNATIVESOCKETENGINE_DEBUG)
    qDebug("QSymbianSocketEnginePrivate::nativeClose()");
#endif

    if (d->readNotifier)
        d->readNotifier->setEnabled(false);
    if (d->writeNotifier)
        d->writeNotifier->setEnabled(false);
    if (d->exceptNotifier)
        d->exceptNotifier->setEnabled(false);
    if (d->asyncSelect) {
        d->asyncSelect->deleteLater();
        d->asyncSelect = 0;
    }

    //TODO: call nativeSocket.Shutdown(EImmediate) in some cases?
    if (d->socketType == QAbstractSocket::UdpSocket) {
        //TODO: Close hangs without this, but only for UDP - why?
        TRequestStatus stat;
        d->nativeSocket.Shutdown(RSocket::EImmediate, stat);
        User::WaitForRequest(stat);
    }
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << "QSymbianSocketEngine::close - closing socket" << d->nativeSocket.SubSessionHandle() << d->socketDescriptor;
#endif
    //remove must come before close to avoid a race where another thread gets the old subsession handle
    //reused & asserts when calling QSymbianSocketManager::instance->addSocket
    QSymbianSocketManager::instance().removeSocket(d->nativeSocket);
    d->nativeSocket.Close();
    d->socketDescriptor = -1;

    d->socketState = QAbstractSocket::UnconnectedState;
    d->hasSetSocketError = false;
    d->localPort = 0;
    d->localAddress.clear();
    d->peerPort = 0;
    d->peerAddress.clear();
    if (d->readNotifier) {
        qDeleteInEventHandler(d->readNotifier);
        d->readNotifier = 0;
    }
    if (d->writeNotifier) {
        qDeleteInEventHandler(d->writeNotifier);
        d->writeNotifier = 0;
    }
    if (d->exceptNotifier) {
        qDeleteInEventHandler(d->exceptNotifier);
        d->exceptNotifier = 0;
    }
}

qint64 QSymbianSocketEngine::write(const char *data, qint64 len)
{
    Q_D(QSymbianSocketEngine);
    TPtrC8 buffer((TUint8*)data, (int)len);
    TSockXfrLength sentBytes = 0;
    TRequestStatus status;
    d->nativeSocket.Send(buffer, 0, status, sentBytes);
    User::WaitForRequest(status); //TODO: on emulator this blocks for write >16kB (non blocking IO not implemented properly?)
    TInt err = status.Int(); 

    if (err) {
        switch (err) {
        case KErrDisconnected:
        case KErrEof:
            sentBytes = -1;
            d->setError(QAbstractSocket::RemoteHostClosedError, d->RemoteHostClosedErrorString);
            close();
            break;
        case KErrTooBig:
            d->setError(QAbstractSocket::DatagramTooLargeError, d->DatagramTooLargeErrorString);
            break;
        case KErrWouldBlock:
            break;
        default:
            sentBytes = -1;
            d->setError(err);
            close();
            break;
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

    if (err == KErrWouldBlock) {
        // No data was available for reading
        r = -2;
    } else if (err != KErrNone) {
        d->setError(err);
        close();
        r = -1;
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
    //cancel asynchronous notifier (only one IOCTL allowed at a time)
    if (asyncSelect)
        asyncSelect->Cancel();
    //TODO: implement
    //as above, but checking both read and write status at the same time

    TPckgBuf<TUint> selectFlags;
    selectFlags() = KSockSelectExcept;
    if (checkRead)
        selectFlags() |= KSockSelectRead;
    if (checkWrite)
        selectFlags() |= KSockSelectWrite;
    TRequestStatus selectStat;
    nativeSocket.Ioctl(KIOctlSelect, selectStat, &selectFlags, KSOLSocket);

    if (timeout < 0)
        User::WaitForRequest(selectStat); //negative means no timeout
    else {
        if (!selectTimer.Handle())
            qt_symbian_throwIfError(selectTimer.CreateLocal());
        TRequestStatus timerStat;
        selectTimer.HighRes(timerStat, timeout * 1000);
        User::WaitForRequest(timerStat, selectStat);
        if (selectStat == KRequestPending) {
            nativeSocket.CancelIoctl();
            //CancelIoctl completes the request (most likely with KErrCancel)
            //We need to wait for this to keep the thread semaphore balanced (or active scheduler will panic)
            User::WaitForRequest(selectStat);
            //restart asynchronous notifier (only one IOCTL allowed at a time)
            if (asyncSelect)
                asyncSelect->IssueRequest();
            return 0; //timeout
        } else {
            selectTimer.Cancel();
            User::WaitForRequest(timerStat);
        }
    }

    if (selectStat != KErrNone)
        return selectStat.Int();
    if (selectFlags() & KSockSelectExcept) {
        TInt err;
        nativeSocket.GetOpt(KSOSelectLastError, KSOLSocket, err);
        //restart asynchronous notifier (only one IOCTL allowed at a time)
        if (asyncSelect)
            asyncSelect->IssueRequest(); //TODO: in error case should we restart or not?
        return err;
    }
    if (checkRead && (selectFlags() & KSockSelectRead)) {
        Q_ASSERT(selectForRead);
        *selectForRead = true;
    }
    if (checkWrite && (selectFlags() & KSockSelectWrite)) {
        Q_ASSERT(selectForWrite);
        *selectForWrite = true;
    }
    //restart asynchronous notifier (only one IOCTL allowed at a time)
    if (asyncSelect)
        asyncSelect->IssueRequest();
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

// FIXME this is also in QNativeSocketEngine, unify it
/*! \internal

    Sets the error and error string if not set already. The only
    interesting error is the first one that occurred, and not the last
    one.
*/
void QSymbianSocketEnginePrivate::setError(QAbstractSocket::SocketError error, ErrorString errorString) const
{
    if (hasSetSocketError) {
        // Only set socket errors once for one engine; expect the
        // socket to recreate its engine after an error. Note: There's
        // one exception: SocketError(11) bypasses this as it's purely
        // a temporary internal error condition.
        // Another exception is the way the waitFor*() functions set
        // an error when a timeout occurs. After the call to setError()
        // they reset the hasSetSocketError to false
        return;
    }
    if (error != QAbstractSocket::SocketError(11))
        hasSetSocketError = true;

    socketError = error;

    switch (errorString) {
    case NonBlockingInitFailedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unable to initialize non-blocking socket");
        break;
    case BroadcastingInitFailedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unable to initialize broadcast socket");
        break;
    case NoIpV6ErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Attempt to use IPv6 socket on a platform with no IPv6 support");
        break;
    case RemoteHostClosedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("The remote host closed the connection");
        break;
    case TimeOutErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Network operation timed out");
        break;
    case ResourceErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Out of resources");
        break;
    case OperationUnsupportedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unsupported socket operation");
        break;
    case ProtocolUnsupportedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Protocol type not supported");
        break;
    case InvalidSocketErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Invalid socket descriptor");
        break;
    case HostUnreachableErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Host unreachable");
        break;
    case NetworkUnreachableErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Network unreachable");
        break;
    case AccessErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Permission denied");
        break;
    case ConnectionTimeOutErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Connection timed out");
        break;
    case ConnectionRefusedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Connection refused");
        break;
    case AddressInuseErrorString:
        socketErrorString = QSymbianSocketEngine::tr("The bound address is already in use");
        break;
    case AddressNotAvailableErrorString:
        socketErrorString = QSymbianSocketEngine::tr("The address is not available");
        break;
    case AddressProtectedErrorString:
        socketErrorString = QSymbianSocketEngine::tr("The address is protected");
        break;
    case DatagramTooLargeErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Datagram was too large to send");
        break;
    case SendDatagramErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unable to send a message");
        break;
    case ReceiveDatagramErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unable to receive a message");
        break;
    case WriteErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unable to write");
        break;
    case ReadErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Network error");
        break;
    case PortInuseErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Another socket is already listening on the same port");
        break;
    case NotSocketErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Operation on non-socket");
        break;
    case InvalidProxyTypeString:
        socketErrorString = QSymbianSocketEngine::tr("The proxy type is invalid for this operation");
        break;
    case UnknownSocketErrorString:
        socketErrorString = QSymbianSocketEngine::tr("Unknown error");
        break;
    }
}

//TODO: use QSystemError class when file engine is merged to master
void QSymbianSocketEnginePrivate::setError(TInt symbianError)
{
    switch (symbianError) {
    case KErrDisconnected:
    case KErrEof:
        setError(QAbstractSocket::RemoteHostClosedError,
                 QSymbianSocketEnginePrivate::RemoteHostClosedErrorString);
        break;
    case KErrNetUnreach:
        setError(QAbstractSocket::NetworkError,
                 QSymbianSocketEnginePrivate::NetworkUnreachableErrorString);
        break;
    case KErrHostUnreach:
        setError(QAbstractSocket::NetworkError,
                 QSymbianSocketEnginePrivate::HostUnreachableErrorString);
        break;
    case KErrNoProtocolOpt:
        setError(QAbstractSocket::NetworkError,
                 QSymbianSocketEnginePrivate::ProtocolUnsupportedErrorString);
        break;
    case KErrInUse:
        setError(QAbstractSocket::AddressInUseError, AddressInuseErrorString);
        break;
    case KErrPermissionDenied:
        setError(QAbstractSocket::SocketAccessError, AddressProtectedErrorString);
        break;
    case KErrNotSupported:
        setError(QAbstractSocket::UnsupportedSocketOperationError, OperationUnsupportedErrorString);
        break;
    case KErrNoMemory:
        setError(QAbstractSocket::SocketResourceError, ResourceErrorString);
        break;
    default:
        socketError = QAbstractSocket::NetworkError;
        socketErrorString = QString::number(symbianError);
        break;
    }
    hasSetSocketError = true;
}

class QReadNotifier : public QSocketNotifier
{
    friend class QAsyncSelect;
public:
    QReadNotifier(int fd, QSymbianSocketEngine *parent)
        : QSocketNotifier(fd, QSocketNotifier::Read, parent)
    { engine = parent; }
protected:
    bool event(QEvent *);

    QSymbianSocketEngine *engine;
};

bool QReadNotifier::event(QEvent *e)
{
    if (e->type() == QEvent::SockAct) {
        engine->readNotification();
        return true;
    }
    return QSocketNotifier::event(e);
}

bool QSymbianSocketEngine::isReadNotificationEnabled() const
{
    Q_D(const QSymbianSocketEngine);
    // TODO
    return d->readNotifier && d->readNotifier->isEnabled();
}

void QSymbianSocketEngine::setReadNotificationEnabled(bool enable)
{
    Q_D(QSymbianSocketEngine);
    // TODO
    if (d->readNotifier) {
        d->readNotifier->setEnabled(enable);
    } else if (enable && d->threadData->eventDispatcher) {
        QReadNotifier *rn = new QReadNotifier(d->socketDescriptor, this);
        d->readNotifier = rn;
        if (!d->asyncSelect)
            d->asyncSelect = q_check_ptr(new QAsyncSelect(0, d->nativeSocket, this));
        d->asyncSelect->setReadNotifier(rn);
        d->readNotifier->setEnabled(true);
    }
    // TODO: what do we do if event dispatcher doesn't exist yet?
    if (d->asyncSelect)
        d->asyncSelect->IssueRequest();
}


class QWriteNotifier : public QSocketNotifier
{
    friend class QAsyncSelect;
public:
    QWriteNotifier(int fd, QSymbianSocketEngine *parent)
        : QSocketNotifier(fd, QSocketNotifier::Write, parent)
    { engine = parent; }
protected:
    bool event(QEvent *);

    QSymbianSocketEngine *engine;
};

bool QWriteNotifier::event(QEvent *e)
{
    if (e->type() == QEvent::SockAct) {
        if (engine->state() == QAbstractSocket::ConnectingState)
            engine->connectionNotification();
        else
            engine->writeNotification();
        return true;
    }
    return QSocketNotifier::event(e);
}

bool QSymbianSocketEngine::isWriteNotificationEnabled() const
{
    Q_D(const QSymbianSocketEngine);
    // TODO
    return d->writeNotifier && d->writeNotifier->isEnabled();
}

void QSymbianSocketEngine::setWriteNotificationEnabled(bool enable)
{
    Q_D(QSymbianSocketEngine);
    // TODO
    if (d->writeNotifier) {
        d->writeNotifier->setEnabled(enable);
    } else if (enable && d->threadData->eventDispatcher) {
        QWriteNotifier *wn = new QWriteNotifier(d->socketDescriptor, this);
        d->writeNotifier = wn;
        if (!(d->asyncSelect))
            d->asyncSelect = q_check_ptr(new QAsyncSelect(d->threadData->eventDispatcher, d->nativeSocket, this));
        d->asyncSelect->setWriteNotifier(wn);
        d->writeNotifier->setEnabled(true);
    }
    // TODO: what do we do if event dispatcher doesn't exist yet?
    if (d->asyncSelect)
        d->asyncSelect->IssueRequest();
}

class QExceptionNotifier : public QSocketNotifier
{
    friend class QAsyncSelect;
public:
    QExceptionNotifier(int fd, QSymbianSocketEngine *parent)
        : QSocketNotifier(fd, QSocketNotifier::Exception, parent) { engine = parent; }

protected:
    bool event(QEvent *);

    QSymbianSocketEngine *engine;
};

bool QExceptionNotifier::event(QEvent *e)
{
    if (e->type() == QEvent::SockAct) {
        if (engine->state() == QAbstractSocket::ConnectingState)
            engine->connectionNotification();
        else
            engine->exceptionNotification();
        return true;
    }
    return QSocketNotifier::event(e);
}

bool QSymbianSocketEngine::isExceptionNotificationEnabled() const
{
    Q_D(const QSymbianSocketEngine);
    // TODO
    return d->exceptNotifier && d->exceptNotifier->isEnabled();
    return false;
}

// FIXME do we really need this for symbian?
void QSymbianSocketEngine::setExceptionNotificationEnabled(bool enable)
{
    Q_D(QSymbianSocketEngine);
    // TODO
    if (d->exceptNotifier) {
        d->exceptNotifier->setEnabled(enable);
    } else if (enable && d->threadData->eventDispatcher) {
        QExceptionNotifier *en = new QExceptionNotifier(d->socketDescriptor, this);
        d->exceptNotifier = en;
        if (!(d->asyncSelect))
            d->asyncSelect = q_check_ptr(new QAsyncSelect(d->threadData->eventDispatcher, d->nativeSocket, this));
        d->asyncSelect->setExceptionNotifier(en);
        d->writeNotifier->setEnabled(true);
    }
    if (d->asyncSelect)
        d->asyncSelect->IssueRequest();
}

bool QSymbianSocketEngine::waitForRead(int msecs, bool *timedOut)
{
    Q_D(const QSymbianSocketEngine);

    if (timedOut)
        *timedOut = false;

    int ret = d->nativeSelect(msecs, true);
    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        d->setError(QAbstractSocket::SocketTimeoutError,
            d->TimeOutErrorString);
        d->hasSetSocketError = false; // A timeout error is temporary in waitFor functions
        return false;
    } else if (state() == QAbstractSocket::ConnectingState) {
        connectToHost(d->peerAddress, d->peerPort);
    }

    return ret > 0;
}

bool QSymbianSocketEngine::waitForWrite(int msecs, bool *timedOut)
{
    Q_D(QSymbianSocketEngine);

    if (timedOut)
        *timedOut = false;

    int ret = d->nativeSelect(msecs, false);

    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        d->setError(QAbstractSocket::SocketTimeoutError,
                    d->TimeOutErrorString);
        d->hasSetSocketError = false; // A timeout error is temporary in waitFor functions
        return false;
    } else if (state() == QAbstractSocket::ConnectingState) {
        connectToHost(d->peerAddress, d->peerPort);
    }

    return ret > 0;
}

bool QSymbianSocketEngine::waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
                                      bool checkRead, bool checkWrite,
                                      int msecs, bool *timedOut)
{
    Q_D(QSymbianSocketEngine);

    int ret = d->nativeSelect(msecs, checkRead, checkWrite, readyToRead, readyToWrite);

    if (ret == 0) {
        if (timedOut)
            *timedOut = true;
        d->setError(QAbstractSocket::SocketTimeoutError,
                    d->TimeOutErrorString);
        d->hasSetSocketError = false; // A timeout error is temporary in waitFor functions
        return false;
    } else if (state() == QAbstractSocket::ConnectingState) {
        connectToHost(d->peerAddress, d->peerPort);
    }

    return ret > 0;
}

qint64 QSymbianSocketEngine::bytesToWrite() const
{
    // This is what the QNativeSocketEngine does
    return 0;
}

QAsyncSelect::QAsyncSelect(QAbstractEventDispatcher *dispatcher, RSocket& sock, QSymbianSocketEngine *parent)
    : CActive(CActive::EPriorityStandard),
      m_inSocketEvent(false),
      m_deleteLater(false),
      m_socket(sock),
      m_selectFlags(0),
      engine(parent)
{
    CActiveScheduler::Add(this);
}

QAsyncSelect::~QAsyncSelect()
{
    Cancel();
}

void QAsyncSelect::DoCancel()
{
    m_socket.CancelIoctl();
}

void QAsyncSelect::RunL()
{
    QT_TRYCATCH_LEAVING(run());
}

//RunError is called by the active scheduler if RunL leaves.
//Typically this will happen if a std::bad_alloc propagates down from the application
TInt QAsyncSelect::RunError(TInt aError)
{
    if (engine) {
        QT_TRY {
            engine->d_func()->setError(aError);
            QEvent e(QEvent::SockAct);
            if (iExcN)
                iExcN->event(&e);
            if (iReadN)
                iReadN->event(&e);
        }
        QT_CATCH(...) {}
    }
#ifdef QNATIVESOCKETENGINE_DEBUG
    qDebug() << "QAsyncSelect::RunError" << aError;
#endif
    return KErrNone;
}

void QAsyncSelect::run()
{
    //TODO: block when event loop demands it
    //if (maybeQueueForLater())
    //    return;
    m_inSocketEvent = true;
    m_selectBuf() &= m_selectFlags; //the select ioctl reports everything, so mask to only what we requested
    //TODO: KSockSelectReadContinuation does what?
    if (iReadN && (m_selectBuf() & KSockSelectRead)) {
        QEvent e(QEvent::SockAct);
        iReadN->event(&e);
    }
    if (iWriteN && (m_selectBuf() & KSockSelectWrite)) {
        QEvent e(QEvent::SockAct);
        iWriteN->event(&e);
    }
    if (iExcN && ((m_selectBuf() & KSockSelectExcept) || iStatus != KErrNone)) {
        QEvent e(QEvent::SockAct);
        iExcN->event(&e);
    }
    m_inSocketEvent = false;
    if (m_deleteLater) {
        delete this;
        return;
    }
    // select again (unless disabled by one of the callbacks)
    IssueRequest();
}

void QAsyncSelect::deleteLater()
{
    if (m_inSocketEvent) {
        iExcN = 0;
        iReadN = 0;
        iWriteN = 0;
        engine = 0;
        m_deleteLater = true;
    } else {
        delete this;
    }
}

void QAsyncSelect::IssueRequest()
{
    if (m_inSocketEvent)
        return; //prevent thrashing during a callback - socket engine enables/disables multiple notifiers
    TUint selectFlags = 0;
    if (iReadN && iReadN->isEnabled())
        selectFlags |= KSockSelectRead;
    if (iWriteN && iWriteN->isEnabled())
        selectFlags |= KSockSelectWrite;
    if (iExcN && iExcN->isEnabled())
        selectFlags |= KSockSelectExcept;
    if (selectFlags != m_selectFlags) {
        Cancel();
        m_selectFlags = selectFlags;
    }
    if (m_selectFlags && !IsActive()) {
        m_selectBuf() = m_selectFlags;
        m_socket.Ioctl(KIOctlSelect, iStatus, &m_selectBuf, KSOLSocket);
        SetActive();
    }
}

QT_END_NAMESPACE
