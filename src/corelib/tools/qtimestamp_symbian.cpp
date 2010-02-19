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

#include "qtimestamp.h"
#include <e32std.h>

QT_BEGIN_NAMESPACE

// return quint64 to avoid sign-extension
static quint64 getMillisecondFromTick()
{
    static TInt nanokernel_tick_period;
    if (!nanokernel_tick_period)
        HAL::Get(HAL::ENanoTickPeriod, nanokernel_tick_period);
    return nanokernel_tick_period * User::NTickCount();
}

static qint64 difference(qint64 a, qint64 b)
{
    qint64 retval = a - b;
    // there can be 32-bit rollover
    // assume there were rollovers if the difference is negative by more than
    // 75% of the 32-bit range

    if (retval < Q_INT64_C(-0xc0000000))
        retval += Q_UINT64_C(0x100000000);
    return retval;
}

bool QTimestamp::isMonotonic()
{
    return true;
}

void QTimestamp::start()
{
    t1 = getMillisecondFromTick();
    t2 = 0;
}

qint64 QTimestamp::restart()
{
    qint64 oldt1 = t1;
    t1 = getMillisecondFromTick();
    return difference(t1, oldt1);
}

qint64 QTimestamp::elapsed() const
{
    return difference(getMillisecondFromTick(), t1);
}

qint64 QTimestamp::msecsTo(const QTimestamp &other) const
{
    return difference(other.t1, t1);
}

qint64 QTimestamp::secsTo(const QTimestamp &other) const
{
    return msecsTo(other) / 1000;
}

bool operator<(const QTimestamp &v1, const QTimestamp &v2)
{
    return difference(v1.t1, v2.t1) < 0;
}

QT_END_NAMESPACE
