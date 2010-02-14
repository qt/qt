/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QPERFORMANCELOG_H
#define QPERFORMANCELOG_H

#include <QtCore/qdatetime.h>

QT_BEGIN_NAMESPACE

namespace QPerformanceLog
{
    Q_DECLARATIVE_EXPORT void displayData();
    Q_DECLARATIVE_EXPORT void clear();

#ifdef Q_ENABLE_PERFORMANCE_LOG
    struct LogData {
        LogData(const char *);
        QTime timer;
        int sumTime;
        unsigned int *data;
    };

    struct LogMetric {
        LogMetric(LogData *, int, const char *);
    };

    // Internal
    void displayData(LogData *);
    void clear(LogData *);
#endif
}

#ifdef Q_ENABLE_PERFORMANCE_LOG

#define Q_DECLARE_PERFORMANCE_METRIC(name) \
    enum { name = ValueChoice<0, ValueTracker<0, __LINE__>::value, __LINE__>::value };  \
    template<int L> \
    struct ValueTracker<name, L> \
    { \
        enum { value = name }; \
    }; \
    extern QPerformanceLog::LogMetric metric ## name; 

#define Q_DECLARE_PERFORMANCE_LOG(name) \
    namespace name { \
        extern QPerformanceLog::LogData log; \
        inline void displayData() { QPerformanceLog::displayData(&log); } \
        inline void clear() { QPerformanceLog::clear(&log); } \
    } \
    template<int N> \
    class name ## Timer { \
    public: \
            name ## Timer() { \
                lastSum = name::log.sumTime + name::log.timer.restart(); \
                name::log.sumTime = 0; \
            } \
            ~ name ## Timer() { \
                name::log.data[2 * N] += name::log.sumTime + name::log.timer.restart(); \
                ++name::log.data[2 * N + 1]; \
                name::log.sumTime = lastSum; \
            } \
    private: \
             int lastSum; \
    }; \
    namespace name { \
        template<int N, int L> \
        struct ValueTracker \
        { \
            enum { value = -1 }; \
        }; \
        template<int DefNextValue, int NextValue, int L> \
        struct ValueChoice \
        { \
            enum { value = ValueChoice<DefNextValue + 1, ValueTracker<DefNextValue + 1, L>::value, L>::value }; \
        }; \
        template<int DefNextValue, int L> \
        struct ValueChoice<DefNextValue, -1, L> \
        { \
            enum { value = DefNextValue }; \
        }; \
    } \
    namespace name 

#define Q_DEFINE_PERFORMANCE_LOG(name, desc) \
    QPerformanceLog::LogData name::log(desc); \
    namespace name 

#define Q_DEFINE_PERFORMANCE_METRIC(name, desc) \
    QPerformanceLog::LogMetric metrix ## name(&log, name, desc); 

#else // Q_ENABLE_PERFORMANCE_LOG

#define Q_DECLARE_PERFORMANCE_METRIC(name)
#define Q_DECLARE_PERFORMANCE_LOG(name) namespace name
#define Q_DEFINE_PERFORMANCE_LOG(name, desc) namespace name
#define Q_DEFINE_PERFORMANCE_METRIC(name, desc) 

#endif // Q_ENABLE_PERFORMANCE_LOG

QT_END_NAMESPACE

#endif // QPERFORMANCELOG_H
