/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qopenkodeeventloopintegration.h"

#include <QDebug>

#include <KD/kd.h>
#include <KD/ATX_keyboard.h>

QT_BEGIN_NAMESPACE

static const int QT_EVENT_WAKEUP_EVENTLOOP = KD_EVENT_USER + 1;

void kdprocessevent( const KDEvent *event)
{
    switch (event->type) {
    case KD_EVENT_INPUT:
        qDebug() << "KD_EVENT_INPUT";
        break;
    case KD_EVENT_INPUT_POINTER:
        qDebug() << "KD_EVENT_INPUT_POINTER";
        break;
    case KD_EVENT_WINDOW_CLOSE:
        qDebug() << "KD_EVENT_WINDOW_CLOSE";
        break;
    case KD_EVENT_WINDOWPROPERTY_CHANGE:
        qDebug() << "KD_EVENT_WINDOWPROPERTY_CHANGE";
        qDebug() << event->data.windowproperty.pname;
        break;
    case KD_EVENT_WINDOW_FOCUS:
        qDebug() << "KD_EVENT_WINDOW_FOCUS";
        break;
    case KD_EVENT_WINDOW_REDRAW:
        qDebug() << "KD_EVENT_WINDOW_REDRAW";
        break;
    case KD_EVENT_USER:
        qDebug() << "KD_EVENT_USER";
        break;
    case KD_EVENT_INPUT_KEY_ATX:
        qDebug() << "KD_EVENT_INPUT_KEY_ATX";
        break;
    case QT_EVENT_WAKEUP_EVENTLOOP:
//        qDebug() << "QT_EVENT_WAKEUP_EVENTLOOP";
        break;
    default:
        break;
    }

    kdDefaultEvent(event);

}

QOpenKODEEventLoopIntegration::QOpenKODEEventLoopIntegration()
{
    m_kdThread = kdThreadSelf();
    kdInstallCallback(&kdprocessevent,QT_EVENT_WAKEUP_EVENTLOOP,this);
}

void QOpenKODEEventLoopIntegration::processEvents(qint64 msec)
{
    if (msec == 0)
        msec = -1;
    const KDEvent *event = kdWaitEvent(msec*1000);
    if (event) {
        kdDefaultEvent(event);
        while ((event = kdWaitEvent(0)) != 0) {
            kdDefaultEvent(event);
        }
    }
}

void QOpenKODEEventLoopIntegration::wakeup()
{
    KDEvent *event = kdCreateEvent();
    event->type = QT_EVENT_WAKEUP_EVENTLOOP;
    event->userptr = this;
    kdPostThreadEvent(event,m_kdThread);
}

QT_END_NAMESPACE
