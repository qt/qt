/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qplatformdefs.h"
#include "qapplication.h"
#include "qeventdispatcher_lite_p.h"
#include "private/qeventdispatcher_unix_p.h"
#include "qapplication_p.h"
#ifndef QT_NO_THREAD
#  include "qmutex.h"
#endif
#include <QWindowSystemInterface>

#include <errno.h>
QT_BEGIN_NAMESPACE

QT_USE_NAMESPACE

class QEventDispatcherLitePrivate : public QEventDispatcherUNIXPrivate
{
    Q_DECLARE_PUBLIC(QEventDispatcherLite)
public:
    inline QEventDispatcherLitePrivate()
    { }
};


QEventDispatcherLite::QEventDispatcherLite(QObject *parent)
    : QEventDispatcherUNIX(*new QEventDispatcherLitePrivate, parent)
{ }

QEventDispatcherLite::~QEventDispatcherLite()
{ }



//#define ZERO_FOR_THE_MOMENT

bool QEventDispatcherLite::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    Q_D(QEventDispatcherLite);
    int nevents = 0;

    // handle gui and posted events
    d->interrupt = false;
    QApplication::sendPostedEvents();

    while (!d->interrupt) {        // also flushes output buffer ###can be optimized
        QWindowSystemInterface::UserEvent *event;
        if (!(flags & QEventLoop::ExcludeUserInputEvents)
            && QWindowSystemInterfacePrivate::userEventsQueued() > 0) {
            // process a pending user input event
            event = QWindowSystemInterfacePrivate::getUserEvent();
            if (!event)
                break;
        } else {
            break;
        }

        if (filterEvent(event)) {
            delete event;
            continue;
        }
        nevents++;

        QApplicationPrivate::processUserEvent(event);
        delete event;
    }

    if (!d->interrupt) {
        if (QEventDispatcherUNIX::processEvents(flags))
            return true;
    }
    return (nevents > 0);
}

bool QEventDispatcherLite::hasPendingEvents()
{
    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return qGlobalPostedEventsCount() || QWindowSystemInterfacePrivate::userEventsQueued();;
}

void QEventDispatcherLite::startingUp()
{

}

void QEventDispatcherLite::closingDown()
{

}

void QEventDispatcherLite::flush()
{
    if(qApp)
        qApp->sendPostedEvents();
}


int QEventDispatcherLite::select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                                timeval *timeout)
{
    return QEventDispatcherUNIX::select(nfds, readfds, writefds, exceptfds, timeout);
}

QT_END_NAMESPACE
