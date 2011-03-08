/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "timequery.h"

#include <QtCore/QStringList>
#include <QtXmlPatterns/QXmlQuery>

TimeInformation::TimeInformation(const QString &time, const QString &direction)
    : m_time(time), m_direction(direction)
{
}

QString TimeInformation::time() const
{
    return m_time;
}

QString TimeInformation::direction() const
{
    return m_direction;
}

TimeInformation::List TimeQuery::query(const QString &stationId, const QStringList &lineNumbers, const QDateTime &dateTime)
{
    const TimeInformation::List information = queryInternal(stationId, dateTime);

    TimeInformation::List filteredInformation;

    if (!lineNumbers.isEmpty()) {
        for (int i = 0; i < information.count(); ++i) {
            const TimeInformation info = information.at(i);
            for (int j = 0; j < lineNumbers.count(); ++j) {
                if (info.direction().startsWith(QString("%1 ").arg(lineNumbers.at(j))))
                    filteredInformation.append(info);
            }
        }
    } else {
        filteredInformation = information;
    }

    return filteredInformation;
}

//! [1]
TimeInformation::List TimeQuery::queryInternal(const QString &stationId, const QDateTime &dateTime)
{
    const QString timesQueryUrl = QString("doc('http://wap.trafikanten.no/F.asp?f=%1&amp;t=%2&amp;m=%3&amp;d=%4&amp;start=1')/wml/card/p/small/a[fn:starts-with(@href, 'Rute')]/string()")
                                         .arg(stationId)
                                         .arg(dateTime.time().hour())
                                         .arg(dateTime.time().minute())
                                         .arg(dateTime.toString("dd.MM.yyyy"));
    const QString directionsQueryUrl = QString("doc('http://wap.trafikanten.no/F.asp?f=%1&amp;t=%2&amp;m=%3&amp;d=%4&amp;start=1')/wml/card/p/small/text()[matches(., '[0-9].*')]/string()")
                                              .arg(stationId)
                                              .arg(dateTime.time().hour())
                                              .arg(dateTime.time().minute())
                                              .arg(dateTime.toString("dd.MM.yyyy"));

    QStringList times;
    QStringList directions;

    QXmlQuery query;
    query.setQuery(timesQueryUrl);
    query.evaluateTo(&times);

    query.setQuery(directionsQueryUrl);
    query.evaluateTo(&directions);

    if (times.count() != directions.count()) // something went wrong...
        return TimeInformation::List();

    TimeInformation::List information;
    for (int i = 0; i < times.count(); ++i)
        information.append(TimeInformation(times.at(i).simplified(), directions.at(i).simplified()));

    return information;
}
//! [1]
