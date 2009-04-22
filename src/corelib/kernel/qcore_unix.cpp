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
** contact the sales department at qt-sales@nokia.com.
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

static inline timeval gettime()
{
    timeval tv;
#ifndef QT_NO_CLOCK_MONOTONIC
    // use the monotonic clock
    static volatile bool monotonicClockDisabled = false;
    struct timespec ts;
    if (!monotonicClockDisabled) {
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
            monotonicClockDisabled = true;
        } else {
            tv.tv_sec = ts.tv_sec;
            tv.tv_usec = ts.tv_nsec / 1000;
            return tv;
        }
    }
#endif
    // use gettimeofday
    ::gettimeofday(&tv, 0);
    return tv;
}

static inline bool time_update(struct timeval *tv, const struct timeval &start,
                               const struct timeval &timeout)
{
    struct timeval now = gettime();
    if (now < start) {
        // clock reset, give up
        return false;
    }

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

    timeval start = gettime();
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
