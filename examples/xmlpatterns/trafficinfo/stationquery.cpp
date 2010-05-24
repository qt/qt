/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stationquery.h"

#include <QtCore/QStringList>
#include <QtXmlPatterns/QXmlQuery>

StationInformation::StationInformation()
{
}

StationInformation::StationInformation(const QString &id, const QString &name)
    : m_id(id), m_name(name)
{
}

QString StationInformation::id() const
{
    return m_id;
}

QString StationInformation::name() const
{
    return m_name;
}

//! [0]
StationInformation::List StationQuery::query(const QString &name)
{
    const QString stationIdQueryUrl = QString("doc(concat('http://wap.trafikanten.no/FromLink1.asp?fra=', $station))/wml/card/p/small/a[@title='Velg']/substring(@href,18)");
    const QString stationNameQueryUrl = QString("doc(concat('http://wap.trafikanten.no/FromLink1.asp?fra=', $station))/wml/card/p/small/a[@title='Velg']/string()");

    QStringList stationIds;
    QStringList stationNames;

    QXmlQuery query;

    query.bindVariable("station", QVariant(QString::fromLatin1(QUrl::toPercentEncoding(name))));
    query.setQuery(stationIdQueryUrl);
    query.evaluateTo(&stationIds);

    query.bindVariable("station", QVariant(QString::fromLatin1(QUrl::toPercentEncoding(name))));
    query.setQuery(stationNameQueryUrl);
    query.evaluateTo(&stationNames);

    if (stationIds.count() != stationNames.count()) // something went wrong...
        return StationInformation::List();

    StationInformation::List information;
    for (int i = 0; i < stationIds.count(); ++i)
        information.append(StationInformation(stationIds.at(i), stationNames.at(i)));

    return information;
}
//! [0]
