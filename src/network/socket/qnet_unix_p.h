/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNET_UNIX_P_H
#define QNET_UNIX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt code on Unix. This header file may change from version to
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "private/qcore_unix_p.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

QT_BEGIN_NAMESPACE

// Almost always the same. If not, specify in qplatformdefs.h.
#if !defined(QT_SOCKOPTLEN_T)
# define QT_SOCKOPTLEN_T QT_SOCKLEN_T
#endif

// UnixWare 7 redefines socket -> _socket
static inline int qt_safe_socket(int domain, int type, int protocol, int flags = 0)
{
    Q_ASSERT((flags & ~O_NONBLOCK) == 0);

    register int fd;
#ifdef SOCK_CLOEXEC
    int newtype = type | SOCK_CLOEXEC;
    if (flags & O_NONBLOCK)
        newtype |= SOCK_NONBLOCK;
    fd = ::socket(domain, newtype, protocol);
    if (fd != -1 || errno != EINVAL)
        return fd;
#endif

    fd = ::socket(domain, type, protocol);
    if (fd == -1)
        return -1;

    ::fcntl(fd, F_SETFD, FD_CLOEXEC);

    // set non-block too?
    if (flags & O_NONBLOCK)
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

    return fd;
}

// Tru64 redefines accept -> _accept with _XOPEN_SOURCE_EXTENDED
static inline int qt_safe_accept(int s, struct sockaddr *addr, QT_SOCKLEN_T *addrlen, int flags = 0)
{
    Q_ASSERT((flags & ~O_NONBLOCK) == 0);

    register int fd;
#if QT_UNIX_SUPPORTS_THREADSAFE_CLOEXEC && defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
    // use accept4
    int sockflags = SOCK_CLOEXEC;
    if (flags & O_NONBLOCK)
        sockflags |= SOCK_NONBLOCK;
    fd = ::accept4(s, addr, static_cast<QT_SOCKLEN_T *>(addrlen), sockflags);
    if (fd != -1 || !(errno == ENOSYS || errno == EINVAL))
        return fd;
#endif

    fd = ::accept(s, addr, static_cast<QT_SOCKLEN_T *>(addrlen));
    if (fd == -1)
        return -1;

    ::fcntl(fd, F_SETFD, FD_CLOEXEC);

    // set non-block too?
    if (flags & O_NONBLOCK)
        ::fcntl(fd, F_SETFL, ::fcntl(fd, F_GETFL) | O_NONBLOCK);

    return fd;
}

// UnixWare 7 redefines listen -> _listen
static inline int qt_safe_listen(int s, int backlog)
{
    return ::listen(s, backlog);
}

static inline int qt_safe_connect(int sockfd, const struct sockaddr *addr, QT_SOCKLEN_T addrlen)
{
    register int ret;
    EINTR_LOOP(ret, QT_SOCKET_CONNECT(sockfd, addr, addrlen));
    return ret;
}
#undef QT_SOCKET_CONNECT
#define QT_SOCKET_CONNECT qt_safe_connect

#if defined(socket)
# undef socket
#endif
#if defined(accept)
# undef accept
#endif
#if defined(listen)
# undef listen
#endif

QT_END_NAMESPACE

#endif // QNET_UNIX_P_H
