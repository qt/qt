/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qcore_unix_p.h"

#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>

#include "qeventdispatcher_unix_p.h" // for the timeval operators

#if !defined(QT_NO_CLOCK_MONOTONIC)
# if defined(QT_BOOTSTRAPPED)
#  define QT_NO_CLOCK_MONOTONIC
# endif
#endif

QT_BEGIN_NAMESPACE

bool qt_gettime_is_monotonic()
{
#if (_POSIX_MONOTONIC_CLOCK-0 > 0) || defined(Q_OS_MAC)
    return true;
#else
    static int returnValue = 0;

    if (returnValue == 0) {
#  if (_POSIX_MONOTONIC_CLOCK-0 < 0)
        returnValue = -1;
#  elif (_POSIX_MONOTONIC_CLOCK == 0)
        // detect if the system support monotonic timers
        long x = sysconf(_SC_MONOTONIC_CLOCK);
        returnValue = (x >= 200112L) ? 1 : -1;
#  endif
    }

    return returnValue != -1;
#endif
}

timeval qt_gettime()
{
    timeval tv;
#if defined(Q_OS_MAC)
    static mach_timebase_info_data_t info = {0,0};
    if (info.denom == 0)
        mach_timebase_info(&info);

    uint64_t cpu_time = mach_absolute_time();
    uint64_t nsecs = cpu_time * (info.numer / info.denom);
    tv.tv_sec = nsecs / 1000000000ull;
    tv.tv_usec = (nsecs / 1000) - (t.tv_sec * 1000000);
    return tv;
#elif (_POSIX_MONOTONIC_CLOCK-0 > 0)
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
    return tv;
#else
#  if !defined(QT_NO_CLOCK_MONOTONIC) && !defined(QT_BOOTSTRAPPED)
    if (qt_gettime_is_monotonic()) {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        tv.tv_sec = ts.tv_sec;
        tv.tv_usec = ts.tv_nsec / 1000;
        return tv;
    }
#  endif
    // use gettimeofday
    ::gettimeofday(&tv, 0);
    return tv;
#endif
}

static inline bool time_update(struct timeval *tv, const struct timeval &start,
                               const struct timeval &timeout)
{
    if (!qt_gettime_is_monotonic()) {
        // we cannot recalculate the timeout without a monotonic clock as the time may have changed
        return false;
    }

    // clock source is monotonic, so we can recalculate how much timeout is left
    struct timeval now = qt_gettime();
    *tv = timeout + start - now;
    return true;
}

int qt_safe_select(int nfds, fd_set *fdread, fd_set *fdwrite, fd_set *fdexcept,
                   const struct timeval *orig_timeout)
{
    if (!orig_timeout) {
        // no timeout -> block forever
        register int ret;
        EINTR_LOOP(ret, select(nfds, fdread, fdwrite, fdexcept, 0));
        return ret;
    }

    timeval start = qt_gettime();
    timeval timeout = *orig_timeout;

    // loop and recalculate the timeout as needed
    int ret;
    forever {
        ret = ::select(nfds, fdread, fdwrite, fdexcept, &timeout);
        if (ret != -1 || errno != EINTR)
            return ret;

        // recalculate the timeout
        if (!time_update(&timeout, start, *orig_timeout)) {
            // clock reset, fake timeout error
            return 0;
        }
    }
}

QT_END_NAMESPACE

#ifdef Q_OS_LINUX
// Don't wait for libc to supply the calls we need
// Make syscalls directly

# if defined(__GLIBC__) && (__GLIBC__ * 0x100 + __GLIBC_MINOR__) >= 0x0204
// glibc 2.4 has syscall(...)
#  include <sys/syscall.h>
#  include <asm/unistd.h>
# else
// no syscall(...)
static inline int syscall(...) { errno = ENOSYS; return -1;}
# endif

# ifndef __NR_dup3
#  if defined(__i386__)
#   define __NR_dup3        330
#   define __NR_pipe2       331
#  elif defined(__x86_64__)
#   define __NR_dup3        292
#   define __NR_pipe2       293
#  elif defined(__ia64__)
#   define __NR_dup3        1316
#   define __NR_pipe2       1317
#  else
//  set the syscalls to absurd numbers so that they'll cause ENOSYS errors
#   warning "Please port the pipe2/dup3 code to this platform"
#   define __NR_dup3        -1
#   define __NR_pipe2       -1
#  endif
# endif

# if !defined(__NR_socketcall) && !defined(__NR_accept4)
#  if defined(__x86_64__)
#   define __NR_accept4     288
#  elif defined(__ia64__)
//  not assigned yet to IA-64
#   define __NR_accept4     -1
#  else
//  set the syscalls to absurd numbers so that they'll cause ENOSYS errors
#   warning "Please port the accept4 code to this platform"
#   define __NR_accept4     -1
#  endif
# endif

QT_BEGIN_NAMESPACE
namespace QtLibcSupplement {
    int pipe2(int pipes[], int flags)
    {
        return syscall(__NR_pipe2, pipes, flags);
    }

    int dup3(int oldfd, int newfd, int flags)
    {
        return syscall(__NR_dup3, oldfd, newfd, flags);
    }

    int accept4(int s, sockaddr *addr, QT_SOCKLEN_T *addrlen, int flags)
    {
# if defined(__NR_socketcall)
        // This platform uses socketcall() instead of raw syscalls
        // the SYS_ACCEPT4 number is cross-platform: 18
        return syscall(__NR_socketcall, 18, &s);
# else
        return syscall(__NR_accept4, s, addr, addrlen, flags);
# endif

        Q_UNUSED(addr); Q_UNUSED(addrlen); Q_UNUSED(flags); // they're actually used
    }
}
QT_END_NAMESPACE
#endif  // Q_OS_LINUX

