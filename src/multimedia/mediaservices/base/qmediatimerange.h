/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#ifndef QMEDIATIMERANGE_H
#define QMEDIATIMERANGE_H

#include <QtCore/qshareddata.h>
#include <QtMediaServices/qtmedianamespace.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Multimedia)

class QMediaTimeRangePrivate;

class Q_MEDIASERVICES_EXPORT QMediaTimeInterval
{
public:
    QMediaTimeInterval();
    QMediaTimeInterval(qint64 start, qint64 end);
    QMediaTimeInterval(const QMediaTimeInterval&);

    qint64 start() const;
    qint64 end() const;

    bool contains(qint64 time) const;

    bool isNormal() const;
    QMediaTimeInterval normalized() const;
    QMediaTimeInterval translated(qint64 offset) const;

private:
    friend class QMediaTimeRangePrivate;
    friend class QMediaTimeRange;

    qint64 s;
    qint64 e;
};

Q_MEDIASERVICES_EXPORT bool operator==(const QMediaTimeInterval&, const QMediaTimeInterval&);
Q_MEDIASERVICES_EXPORT bool operator!=(const QMediaTimeInterval&, const QMediaTimeInterval&);

class Q_MEDIASERVICES_EXPORT QMediaTimeRange
{
public:

    QMediaTimeRange();
    QMediaTimeRange(qint64 start, qint64 end);
    QMediaTimeRange(const QMediaTimeInterval&);
    QMediaTimeRange(const QMediaTimeRange &range);
    ~QMediaTimeRange();

    QMediaTimeRange &operator=(const QMediaTimeRange&);
    QMediaTimeRange &operator=(const QMediaTimeInterval&);

    qint64 earliestTime() const;
    qint64 latestTime() const;

    QList<QMediaTimeInterval> intervals() const;
    bool isEmpty() const;
    bool isContinuous() const;

    bool contains(qint64 time) const;

    void addInterval(qint64 start, qint64 end);
    void addInterval(const QMediaTimeInterval &interval);
    void addTimeRange(const QMediaTimeRange&);

    void removeInterval(qint64 start, qint64 end);
    void removeInterval(const QMediaTimeInterval &interval);
    void removeTimeRange(const QMediaTimeRange&);

    QMediaTimeRange& operator+=(const QMediaTimeRange&);
    QMediaTimeRange& operator+=(const QMediaTimeInterval&);
    QMediaTimeRange& operator-=(const QMediaTimeRange&);
    QMediaTimeRange& operator-=(const QMediaTimeInterval&);

    void clear();

private:
    QSharedDataPointer<QMediaTimeRangePrivate> d;
};

Q_MEDIASERVICES_EXPORT bool operator==(const QMediaTimeRange&, const QMediaTimeRange&);
Q_MEDIASERVICES_EXPORT bool operator!=(const QMediaTimeRange&, const QMediaTimeRange&);
Q_MEDIASERVICES_EXPORT QMediaTimeRange operator+(const QMediaTimeRange&, const QMediaTimeRange&);
Q_MEDIASERVICES_EXPORT QMediaTimeRange operator-(const QMediaTimeRange&, const QMediaTimeRange&);


QT_END_NAMESPACE

QT_END_HEADER

#endif  // QMEDIATIMERANGE_H
