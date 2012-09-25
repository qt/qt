/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

// #define QBBSCREENEVENTTHREAD_DEBUG

#include "qbbscreeneventthread.h"
#include "qbbscreeneventhandler.h"

#include <QDebug>

#include <errno.h>

QT_BEGIN_NAMESPACE

QBBScreenEventThread::QBBScreenEventThread(screen_context_t context, QBBScreenEventHandler *eventHandler)
    : mContext(context),
      mQuit(false),
      mEventHandler(eventHandler)
{
}

QBBScreenEventThread::~QBBScreenEventThread()
{
    // block until thread terminates
    shutdown();
}

void QBBScreenEventThread::injectKeyboardEvent(int flags, int sym, int mod, int scan, int cap)
{
    QBBScreenEventHandler::injectKeyboardEvent(flags, sym, mod, scan, cap);
}

void QBBScreenEventThread::injectPointerMoveEvent(int x, int y)
{
    mEventHandler->injectPointerMoveEvent(x, y);
}

void QBBScreenEventThread::run()
{
    screen_event_t event;

    // create screen event
    errno = 0;
    int result = screen_create_event(&event);
    if (result)
        qFatal("QBB: failed to create screen event, errno=%d", errno);

#if defined(QBBSCREENEVENTTHREAD_DEBUG)
    qDebug() << "QBB: screen event thread started";
#endif

    // loop indefinitely
    while (!mQuit) {

        // block until screen event is available
        errno = 0;
        result = screen_get_event(mContext, event, -1);
        if (result)
            qFatal("QBB: failed to get screen event, errno=%d", errno);

        // process received event
        errno = 0;
        int qnxType;
        int result = screen_get_event_property_iv(event, SCREEN_PROPERTY_TYPE, &qnxType);
        if (result)
            qFatal("QBB: failed to query screen event type, errno=%d", errno);

        if (qnxType == SCREEN_EVENT_USER) {
            // treat all screen user events as shutdown requests
            #if defined(QBBSCREENEVENTTHREAD_DEBUG)
            qDebug() << "QBB: QNX screen user event";
            #endif
            mQuit = true;
        } else {
            mEventHandler->handleEvent(event, qnxType);
        }
    }

#if defined(QBBSCREENEVENTTHREAD_DEBUG)
    qDebug() << "QBB: screen event thread stopped";
#endif

    // cleanup
    screen_destroy_event(event);
}

void QBBScreenEventThread::shutdown()
{
    screen_event_t event;

    // create screen event
    errno = 0;
    int result = screen_create_event(&event);
    if (result)
        qFatal("QBB: failed to create screen event, errno=%d", errno);

    // set the event type as user
    errno = 0;
    int type = SCREEN_EVENT_USER;
    result = screen_set_event_property_iv(event, SCREEN_PROPERTY_TYPE, &type);
    if (result)
        qFatal("QBB: failed to set screen event type, errno=%d", errno);

    // NOTE: ignore SCREEN_PROPERTY_USER_DATA; treat all user events as shutdown events

    // post event to event loop so it will wake up and die
    errno = 0;
    result = screen_send_event(mContext, event, getpid());
    if (result)
        qFatal("QBB: failed to set screen event type, errno=%d", errno);

    // cleanup
    screen_destroy_event(event);

#if defined(QBBSCREENEVENTTHREAD_DEBUG)
    qDebug() << "QBB: screen event thread shutdown begin";
#endif

    // block until thread terminates
    wait();

#if defined(QBBSCREENEVENTTHREAD_DEBUG)
    qDebug() << "QBB: screen event thread shutdown end";
#endif
}

QT_END_NAMESPACE
