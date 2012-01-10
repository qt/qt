/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativedebugtrace_p.h"

#include <QtCore/qdatastream.h>
#include <QtCore/qurl.h>
#include <QtCore/qtimer.h>

Q_GLOBAL_STATIC(QDeclarativeDebugTrace, traceInstance);

// convert to a QByteArray that can be sent to the debug client
// use of QDataStream can skew results if m_deferredSend == false
//     (see tst_qperformancetimer::trace() benchmark)
QByteArray QDeclarativeDebugData::toByteArray() const
{
    QByteArray data;
    //### using QDataStream is relatively expensive
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << time << messageType << detailType;
    if (messageType == (int)QDeclarativeDebugTrace::RangeData)
        ds << detailData;
    if (messageType == (int)QDeclarativeDebugTrace::RangeLocation)
        ds << detailData << line;
    return data;
}

QDeclarativeDebugTrace::QDeclarativeDebugTrace()
: QDeclarativeDebugService(QLatin1String("CanvasFrameRate")),
  m_enabled(false), m_deferredSend(true), m_messageReceived(false)
{
    m_timer.start();
    if (status() == Enabled) {
        // wait for first message indicating whether to trace or not
        while (!m_messageReceived)
            waitForMessage();
    }
}

void QDeclarativeDebugTrace::addEvent(EventType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->addEventImpl(t);
}

void QDeclarativeDebugTrace::startRange(RangeType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->startRangeImpl(t);
}

void QDeclarativeDebugTrace::rangeData(RangeType t, const QString &data)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->rangeDataImpl(t, data);
}

void QDeclarativeDebugTrace::rangeData(RangeType t, const QUrl &data)
{
    if (QDeclarativeDebugService::isDebuggingEnabled())
        traceInstance()->rangeDataImpl(t, data);
}

void QDeclarativeDebugTrace::rangeLocation(RangeType t, const QString &fileName, int line)
{
    if (QDeclarativeDebugService::isDebuggingEnabled())
        traceInstance()->rangeLocationImpl(t, fileName, line);
}

void QDeclarativeDebugTrace::rangeLocation(RangeType t, const QUrl &fileName, int line)
{
    if (QDeclarativeDebugService::isDebuggingEnabled())
        traceInstance()->rangeLocationImpl(t, fileName, line);
}

void QDeclarativeDebugTrace::endRange(RangeType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->endRangeImpl(t);
}

void QDeclarativeDebugTrace::addEventImpl(EventType event)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData ed = {m_timer.elapsed(), (int)Event, (int)event, QString(), -1};
    processMessage(ed);
}

void QDeclarativeDebugTrace::startRangeImpl(RangeType range)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeStart, (int)range, QString(), -1};
    processMessage(rd);
}

void QDeclarativeDebugTrace::rangeDataImpl(RangeType range, const QString &rData)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeData, (int)range, rData, -1};
    processMessage(rd);
}

void QDeclarativeDebugTrace::rangeDataImpl(RangeType range, const QUrl &rData)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeData, (int)range, rData.toString(QUrl::FormattingOption(0x100)), -1};
    processMessage(rd);
}

void QDeclarativeDebugTrace::rangeLocationImpl(RangeType range, const QString &fileName, int line)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeLocation, (int)range, fileName, line};
    processMessage(rd);
}

void QDeclarativeDebugTrace::rangeLocationImpl(RangeType range, const QUrl &fileName, int line)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeLocation, (int)range, fileName.toString(QUrl::FormattingOption(0x100)), line};
    processMessage(rd);
}

void QDeclarativeDebugTrace::endRangeImpl(RangeType range)
{
    if (status() != Enabled || !m_enabled)
        return;

    QDeclarativeDebugData rd = {m_timer.elapsed(), (int)RangeEnd, (int)range, QString(), -1};
    processMessage(rd);
}

/*
    Either send the message directly, or queue up
    a list of messages to send later (via sendMessages)
*/
void QDeclarativeDebugTrace::processMessage(const QDeclarativeDebugData &message)
{
    if (m_deferredSend)
        m_data.append(message);
    else
        sendMessage(message.toByteArray());
}

/*
    Send the messages queued up by processMessage
*/
void QDeclarativeDebugTrace::sendMessages()
{
    if (m_deferredSend) {
        //### this is a suboptimal way to send batched messages
        for (int i = 0; i < m_data.count(); ++i)
            sendMessage(m_data.at(i).toByteArray());
        m_data.clear();

        //indicate completion
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << (qint64)-1 << (int)Complete;
        sendMessage(data);
    }
}

void QDeclarativeDebugTrace::messageReceived(const QByteArray &message)
{
    QByteArray rwData = message;
    QDataStream stream(&rwData, QIODevice::ReadOnly);

    stream >> m_enabled;

    m_messageReceived = true;

    if (!m_enabled)
        sendMessages();
}
