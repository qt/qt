/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNATIVESOCKETENGINE_P_H
#define QNATIVESOCKETENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
#include "QtNetwork/qhostaddress.h"
#include "private/qabstractsocketengine_p.h"
#ifndef Q_OS_WIN
#  include "qplatformdefs.h"
#else
#  include <winsock2.h>
#endif

#ifdef Q_OS_SYMBIAN
#include <private/qeventdispatcher_symbian_p.h>
#include<unistd.h>
#endif

QT_BEGIN_NAMESPACE

static inline int qt_socket_connect(int s, const struct sockaddr * addrptr, socklen_t namelen)
{
	return ::connect(s, addrptr, namelen);
}
#if defined(connect)
# undef connect
#endif

static inline int qt_socket_bind(int s, const struct sockaddr * addrptr, socklen_t namelen)
{
	return ::bind(s, addrptr, namelen);
}
#if defined(bind)
# undef bind
#endif

static inline int qt_socket_write(int socket, const char *data, qint64 len)
{
	return ::write(socket, data, len);
}
#if defined(write)
# undef write
#endif

static inline int qt_socket_read(int socket, char *data, qint64 len)
{
	return ::read(socket, data, len);
}
#if defined(read)
# undef read
#endif

static inline int qt_socket_recv(int socket, void *data, size_t length, int flags)
{
	return ::recv(socket, data, length, flags);
}
#if defined(recv)
# undef recv
#endif

static inline int qt_socket_recvfrom(int socket, void *data, size_t length,
	                                 int flags, struct sockaddr *address,
	                                 socklen_t *address_length)
{
	return ::recvfrom(socket, data, length, flags, address, address_length);
}
#if defined(recvfrom)
# undef recvfrom
#endif

static inline int qt_socket_sendto(int socket, const void *data, size_t length,
	                               int flags, const struct sockaddr *dest_addr,
	                               socklen_t dest_length)
{
	return ::sendto(socket, data, length, flags, dest_addr, dest_length);
}
#if defined(sendto)
# undef sendto
#endif
static inline int qt_socket_close(int socket)
{
	return ::close(socket);
}
#if defined(close)
# undef close
#endif

static inline int qt_socket_fcntl(int socket, int command, int option)
{
	return ::fcntl(socket, command, option);
}
#if defined(fcntl)
# undef fcntl
#endif

static inline int qt_socket_ioctl(int socket, int command, char *option)
{
	return ::ioctl(socket, command, option);
}
#if defined(ioctl)
# undef ioctl
#endif

static inline int qt_socket_getsockname(int socket, struct sockaddr *address, socklen_t *address_len)
{
	return ::getsockname(socket, address, address_len);
}
#if defined(getsockname)
# undef getsockname
#endif

static inline int qt_socket_getpeername(int socket, struct sockaddr *address, socklen_t *address_len)
{
	return ::getpeername(socket, address, address_len);
}
#if defined(getpeername)
# undef getpeername
#endif

static inline int qt_socket_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return ::select(nfds, readfds, writefds, exceptfds, timeout);
}

#if defined(select)
# undef select
#endif

static inline int qt_socket_getsockopt(int socket, int level, int optname, void *optval, socklen_t *optlen)
{
	return ::getsockopt(socket, level, optname, optval, optlen);
}

#if defined(getsockopt)
# undef getsockopt
#endif

static inline int qt_socket_setsockopt(int socket, int level, int optname, void *optval, socklen_t optlen)
{
	return ::setsockopt(socket, level, optname, optval, optlen);
}

#if defined(setsockopt)
# undef setsockopt
#endif

// Use our own defines and structs which we know are correct
#  define QT_SS_MAXSIZE 128
#  define QT_SS_ALIGNSIZE (sizeof(qint64))
#  define QT_SS_PAD1SIZE (QT_SS_ALIGNSIZE - sizeof (short))
#  define QT_SS_PAD2SIZE (QT_SS_MAXSIZE - (sizeof (short) + QT_SS_PAD1SIZE + QT_SS_ALIGNSIZE))
struct qt_sockaddr_storage {
      short ss_family;
      char __ss_pad1[QT_SS_PAD1SIZE];
      qint64 __ss_align;
      char __ss_pad2[QT_SS_PAD2SIZE];
};

// sockaddr_in6 size changed between old and new SDK
// Only the new version is the correct one, so always
// use this structure.
struct qt_in6_addr {
    quint8 qt_s6_addr[16];
};
struct qt_sockaddr_in6 {
    short   sin6_family;            /* AF_INET6 */
    quint16 sin6_port;              /* Transport level port number */
    quint32 sin6_flowinfo;          /* IPv6 flow information */
    struct  qt_in6_addr sin6_addr;  /* IPv6 address */
    quint32 sin6_scope_id;          /* set of interfaces for a scope */
};

union qt_sockaddr {
    sockaddr a;
    sockaddr_in a4;
    qt_sockaddr_in6 a6;
    qt_sockaddr_storage storage;
};

class QNativeSocketEnginePrivate;

class Q_AUTOTEST_EXPORT QNativeSocketEngine : public QAbstractSocketEngine
{
    Q_OBJECT
public:
    QNativeSocketEngine(QObject *parent = 0);
    ~QNativeSocketEngine();

    bool initialize(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol = QAbstractSocket::IPv4Protocol);
    bool initialize(int socketDescriptor, QAbstractSocket::SocketState socketState = QAbstractSocket::ConnectedState);

    int socketDescriptor() const;

    bool isValid() const;

    bool connectToHost(const QHostAddress &address, quint16 port);
    bool connectToHostByName(const QString &name, quint16 port);
    bool bind(const QHostAddress &address, quint16 port);
    bool listen();
    int accept();
    void close();

    qint64 bytesAvailable() const;

    qint64 read(char *data, qint64 maxlen);
    qint64 write(const char *data, qint64 len);

    qint64 readDatagram(char *data, qint64 maxlen, QHostAddress *addr = 0,
                            quint16 *port = 0);
    qint64 writeDatagram(const char *data, qint64 len, const QHostAddress &addr,
                             quint16 port);
    bool hasPendingDatagrams() const;
    qint64 pendingDatagramSize() const;

    qint64 receiveBufferSize() const;
    void setReceiveBufferSize(qint64 bufferSize);

    qint64 sendBufferSize() const;
    void setSendBufferSize(qint64 bufferSize);

    int option(SocketOption option) const;
    bool setOption(SocketOption option, int value);

    bool waitForRead(int msecs = 30000, bool *timedOut = 0);
    bool waitForWrite(int msecs = 30000, bool *timedOut = 0);
    bool waitForReadOrWrite(bool *readyToRead, bool *readyToWrite,
			    bool checkRead, bool checkWrite,
			    int msecs = 30000, bool *timedOut = 0);

    bool isReadNotificationEnabled() const;
    void setReadNotificationEnabled(bool enable);
    bool isWriteNotificationEnabled() const;
    void setWriteNotificationEnabled(bool enable);
    bool isExceptionNotificationEnabled() const;
    void setExceptionNotificationEnabled(bool enable);

public Q_SLOTS:
    // non-virtual override;
    void connectionNotification();

private:
    Q_DECLARE_PRIVATE(QNativeSocketEngine)
    Q_DISABLE_COPY(QNativeSocketEngine)
};

#ifdef Q_OS_WIN
class QWindowsSockInit
{
public:
    QWindowsSockInit();
    ~QWindowsSockInit();
    int version;
};
#endif

class QSocketNotifier;

class QNativeSocketEnginePrivate : public QAbstractSocketEnginePrivate
{
    Q_DECLARE_PUBLIC(QNativeSocketEngine)
public:
    QNativeSocketEnginePrivate();
    ~QNativeSocketEnginePrivate();

    int socketDescriptor;

#if !defined(QT_NO_IPV6)
    struct sockaddr_storage aa;
#else
    struct sockaddr_in aa;
#endif

    QSocketNotifier *readNotifier, *writeNotifier, *exceptNotifier;

#ifdef Q_OS_WIN
    QWindowsSockInit winSock;
#endif

    enum ErrorString {
        NonBlockingInitFailedErrorString,
        BroadcastingInitFailedErrorString,
        NoIpV6ErrorString,
        RemoteHostClosedErrorString,
        TimeOutErrorString,
        ResourceErrorString,
        OperationUnsupportedErrorString,
        ProtocolUnsupportedErrorString,
        InvalidSocketErrorString,
        HostUnreachableErrorString,
        NetworkUnreachableErrorString,
        AccessErrorString,
        ConnectionTimeOutErrorString,
        ConnectionRefusedErrorString,
        AddressInuseErrorString,
        AddressNotAvailableErrorString,
        AddressProtectedErrorString,
        DatagramTooLargeErrorString,
        SendDatagramErrorString,
        ReceiveDatagramErrorString,
        WriteErrorString,
        ReadErrorString,
        PortInuseErrorString,
        NotSocketErrorString,
        InvalidProxyTypeString,

        UnknownSocketErrorString = -1
    };

    void setError(QAbstractSocket::SocketError error, ErrorString errorString) const;

    // native functions
    int option(QNativeSocketEngine::SocketOption option) const;
    bool setOption(QNativeSocketEngine::SocketOption option, int value);

    bool createNewSocket(QAbstractSocket::SocketType type, QAbstractSocket::NetworkLayerProtocol protocol);

    bool nativeConnect(const QHostAddress &address, quint16 port);
    bool nativeBind(const QHostAddress &address, quint16 port);
    bool nativeListen(int backlog);
    int nativeAccept();
    qint64 nativeBytesAvailable() const;

    bool nativeHasPendingDatagrams() const;
    qint64 nativePendingDatagramSize() const;
    qint64 nativeReceiveDatagram(char *data, qint64 maxLength,
                                     QHostAddress *address, quint16 *port);
    qint64 nativeSendDatagram(const char *data, qint64 length,
                                  const QHostAddress &host, quint16 port);
    qint64 nativeRead(char *data, qint64 maxLength);
    qint64 nativeWrite(const char *data, qint64 length);
    int nativeSelect(int timeout, bool selectForRead) const;
    int nativeSelect(int timeout, bool checkRead, bool checkWrite,
		     bool *selectForRead, bool *selectForWrite) const;

    void nativeClose();

    bool checkProxy(const QHostAddress &address);
    bool fetchConnectionParameters();
};

QT_END_NAMESPACE

#endif // QNATIVESOCKETENGINE_P_H
