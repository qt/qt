/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qperformancelog_p_p.h"

#include <QHash>
#include <QDebug>

QT_BEGIN_NAMESPACE

#ifdef Q_ENABLE_PERFORMANCE_LOG

struct QPerformanceLogData 
{
    struct Log
    {
        Log()
        : logDescription(0), maxId(-1) {}

        QHash<int, const char *> descriptions;
        const char *logDescription;
        int maxId;
    };

    typedef QHash<QPerformanceLog::LogData *, Log> Logs;
    Logs logs;
};
Q_GLOBAL_STATIC(QPerformanceLogData, performanceLogData);

QPerformanceLog::LogData::LogData(const char *desc)
: sumTime(0), data(0)
{
    QPerformanceLogData *logData = performanceLogData();

    QPerformanceLogData::Log log;
    log.logDescription = desc;
    logData->logs.insert(this, log);

    timer.start();
}

QPerformanceLog::LogMetric::LogMetric(LogData *l, int id, const char *desc)
{
    if (id < 0)
        qFatal("QPerformanceLog: Invalid log id %d ('%s')", id, desc);

    QPerformanceLogData *logData = performanceLogData();

    QPerformanceLogData::Logs::Iterator logIter = logData->logs.find(l);
    if (logIter == logData->logs.end())
        qFatal("QPerformanceLog: Unable to locate log for metric '%s'", desc);
    QPerformanceLogData::Log &log = *logIter;
    if (log.descriptions.contains(id))
        qFatal("QPerformanceLog: Duplicate log metric %d ('%s')", id, desc);
    log.descriptions.insert(id, desc);

    if (log.maxId < id) {
        log.maxId = id;
        if (l->data) delete [] l->data;
        l->data = new unsigned int[2 * (log.maxId + 1)];
        ::memset(l->data, 0, 2 * (log.maxId + 1) * sizeof(unsigned int));
    }
}

static void QPerformanceLog_clear(QPerformanceLog::LogData *l, const QPerformanceLogData::Log *pl)
{
    ::memset(l->data, 0, 2 * (pl->maxId + 1) * sizeof(unsigned int));
}

static void QPerformanceLog_displayData(const QPerformanceLog::LogData *l, const QPerformanceLogData::Log *pl)
{
    qWarning() << pl->logDescription << "performance data";
    unsigned int total = 0;
    for (QHash<int, const char *>::ConstIterator iter = pl->descriptions.begin();
            iter != pl->descriptions.end();
            ++iter) {

        int id = iter.key();
        unsigned int ms = l->data[id * 2];
        total += ms;
        unsigned int inst = l->data[id * 2 + 1];
        float pi = float(ms) / float(inst);
        qWarning().nospace() << "    " << *iter << ": " << ms << " ms over " 
                             << inst << " instances (" << pi << " ms/instance)";
    }
    qWarning().nospace() << "    TOTAL: " << total;
}

void QPerformanceLog::displayData()
{
    QPerformanceLogData *logData = performanceLogData();

    for (QPerformanceLogData::Logs::ConstIterator iter = logData->logs.begin();
            iter != logData->logs.end();
            ++iter) {
        QPerformanceLog_displayData(iter.key(), &(*iter));
    }
}

void QPerformanceLog::clear()
{
    QPerformanceLogData *logData = performanceLogData();

    for (QPerformanceLogData::Logs::ConstIterator iter = logData->logs.begin();
            iter != logData->logs.end();
            ++iter) {
        QPerformanceLog_clear(iter.key(), &(*iter));
    }
}

void QPerformanceLog::displayData(LogData *l)
{
    QPerformanceLogData *logData = performanceLogData();
    QPerformanceLogData::Logs::ConstIterator iter = logData->logs.find(l);
    if (iter == logData->logs.end())
        qFatal("QPerformanceLog: Internal corruption - unable to locate log");

    QPerformanceLog_displayData(iter.key(), &(*iter));
}

void QPerformanceLog::clear(LogData *l)
{
    QPerformanceLogData *logData = performanceLogData();
    QPerformanceLogData::Logs::ConstIterator iter = logData->logs.find(l);
    if (iter == logData->logs.end())
        qFatal("QPerformanceLog: Internal corruption - unable to locate log");

    QPerformanceLog_clear(iter.key(), &(*iter));
}

#else // Q_ENABLE_PERFORMANCE_LOG

void QPerformanceLog::displayData()
{
}

void QPerformanceLog::clear()
{
}

#endif // Q_ENABLE_PERFORMANCE_LOG

QT_END_NAMESPACE
