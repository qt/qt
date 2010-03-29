/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qelapsedtimer.h"
#include "qpair.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#if !defined(QT_NO_CLOCK_MONOTONIC)
# if defined(QT_BOOTSTRAPPED)
#  define QT_NO_CLOCK_MONOTONIC
# endif
#endif

QT_BEGIN_NAMESPACE

static qint64 fractionAdjustment()
{
    if (QElapsedTimer::isMonotonic()) {
        // the monotonic timer is measured in nanoseconds
        // 1 ms = 1000000 ns
        return 1000*1000ull;
    } else {
        // gettimeofday is measured in microseconds
        // 1 ms = 1000 us
        return 1000;
    }
}

bool QElapsedTimer::isMonotonic()
{
#if (_POSIX_MONOTONIC_CLOCK-0 > 0)
    return true;
#else
    static int returnValue = 0;

    if (returnValue == 0) {
#  if (_POSIX_MONOTONIC_CLOCK-0 < 0) || !defined(_SC_MONOTONIC_CLOCK)
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

QElapsedTimer::ClockType QElapsedTimer::clockType()
{
    return isMonotonic() ? MonotonicClock : SystemTime;
}

static inline QPair<long, long> do_gettime()
{
#if (_POSIX_MONOTONIC_CLOCK-0 > 0)
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return qMakePair<long,long>(ts.tv_sec, ts.tv_nsec);
#else
#  if !defined(QT_NO_CLOCK_MONOTONIC) && !defined(QT_BOOTSTRAPPED)
    if (QElapsedTimer::isMonotonic()) {
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return qMakePair<long,long>(ts.tv_sec, ts.tv_nsec);
    }
#  endif
    // use gettimeofday
    timeval tv;
    ::gettimeofday(&tv, 0);
    return qMakePair<long,long>(tv.tv_sec, tv.tv_usec);
#endif
}

// used in qcore_unix.cpp and qeventdispatcher_unix.cpp
timeval qt_gettime()
{
    QPair<long, long> r = do_gettime();

    timeval tv;
    tv.tv_sec = r.first;
    tv.tv_usec = r.second;
    if (QElapsedTimer::isMonotonic())
        tv.tv_usec /= 1000;

    return tv;
}

void QElapsedTimer::start()
{
    QPair<long, long> r = do_gettime();
    t1 = r.first;
    t2 = r.second;
}

qint64 QElapsedTimer::restart()
{
    QPair<long, long> r = do_gettime();
    qint64 oldt1 = t1;
    qint64 oldt2 = t2;
    t1 = r.first;
    t2 = r.second;

    r.first -= oldt1;
    r.second -= oldt2;
    return r.first * Q_INT64_C(1000) + r.second / fractionAdjustment();
}

qint64 QElapsedTimer::elapsed() const
{
    QElapsedTimer now;
    now.start();
    return msecsTo(now);
}

qint64 QElapsedTimer::msecsSinceReference() const
{
    return t1 * Q_INT64_C(1000) + t2 / fractionAdjustment();
}

qint64 QElapsedTimer::msecsTo(const QElapsedTimer &other) const
{
    qint64 secs = other.t1 - t1;
    qint64 fraction = other.t2 - t2;
    return secs * Q_INT64_C(1000) + fraction / fractionAdjustment();
}

qint64 QElapsedTimer::secsTo(const QElapsedTimer &other) const
{
    return other.t1 - t1;
}

bool operator<(const QElapsedTimer &v1, const QElapsedTimer &v2)
{
    return v1.t1 < v2.t1 || (v1.t1 == v2.t1 && v1.t2 < v2.t2);
}

QT_END_NAMESPACE
