/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativedebugtrace_p.h"

#include <QtCore/qdatastream.h>
#include <QtCore/qurl.h>

Q_GLOBAL_STATIC(QDeclarativeDebugTrace, traceInstance);

QDeclarativeDebugTrace::QDeclarativeDebugTrace()
: QDeclarativeDebugService(QLatin1String("CanvasFrameRate"))
{
    m_timer.start();
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

void QDeclarativeDebugTrace::rangeData(RangeType t, const QUrl &url)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->rangeDataImpl(t, url);
}

void QDeclarativeDebugTrace::endRange(RangeType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        traceInstance()->endRangeImpl(t);
}

void QDeclarativeDebugTrace::addEventImpl(EventType event)
{
    if (status() != Enabled)
        return;

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)Event << (int)event;
    sendMessage(data);
}

void QDeclarativeDebugTrace::startRangeImpl(RangeType range)
{
    if (status() != Enabled)
        return;

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)RangeStart << (int)range;
    sendMessage(data);
}

void QDeclarativeDebugTrace::rangeDataImpl(RangeType range, const QUrl &u)
{
    if (status() != Enabled)
        return;

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)RangeData << (int)range << (QString)u.toString();
    sendMessage(data);
}

void QDeclarativeDebugTrace::endRangeImpl(RangeType range)
{
    if (status() != Enabled)
        return;

    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)RangeEnd << (int)range;
    sendMessage(data);
}

