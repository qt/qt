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

#include "qdeclarativedebugtiming_p.h"

#include <QtCore/qdatastream.h>

Q_GLOBAL_STATIC(QDeclarativeDebugTiming, timerInstance);

QDeclarativeDebugTiming::QDeclarativeDebugTiming()
: QDeclarativeDebugService(QLatin1String("CanvasFrameRate"))
{
    m_timer.start();
}

void QDeclarativeDebugTiming::addEvent(EventType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        timerInstance()->addEventImpl(t);
}

void QDeclarativeDebugTiming::startRange(RangeType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        timerInstance()->startRangeImpl(t);
}

void QDeclarativeDebugTiming::endRange(RangeType t)
{
    if (QDeclarativeDebugService::isDebuggingEnabled()) 
        timerInstance()->endRangeImpl(t);
}

void QDeclarativeDebugTiming::addEventImpl(EventType event)
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)Event << (int)event;
    sendMessage(data);
}

void QDeclarativeDebugTiming::startRangeImpl(RangeType range)
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)RangeStart << (int)range;
    sendMessage(data);
}

void QDeclarativeDebugTiming::endRangeImpl(RangeType range)
{
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds << m_timer.elapsed() << (int)RangeEnd << (int)range;
    sendMessage(data);
}

