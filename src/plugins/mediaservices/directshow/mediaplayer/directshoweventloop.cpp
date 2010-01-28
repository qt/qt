/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <directshoweventloop.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qcoreevent.h>


QT_BEGIN_NAMESPACE


class DirectShowPostedEvent
{
public:
    DirectShowPostedEvent(QObject *receiver, QEvent *event)
        : receiver(receiver)
        , event(event)
        , next(0)
    {
    }

    ~DirectShowPostedEvent()
    {
        delete event;
    }

    QObject *receiver;
    QEvent *event;
    DirectShowPostedEvent *next;
};

DirectShowEventLoop::DirectShowEventLoop(QObject *parent)
    : QWinEventNotifier(parent)
    , m_postsHead(0)
    , m_postsTail(0)
    , m_eventHandle(::CreateEvent(0, 0, 0, 0))
    , m_waitHandle(::CreateEvent(0, 0, 0, 0))
{
    setHandle(m_eventHandle);
    setEnabled(true);
}

DirectShowEventLoop::~DirectShowEventLoop()
{
    setEnabled(false);

    ::CloseHandle(m_eventHandle);
    ::CloseHandle(m_waitHandle);

    for (DirectShowPostedEvent *post = m_postsHead; post; post = m_postsHead) {
        m_postsHead = m_postsHead->next;

        delete post;
    }
}

void DirectShowEventLoop::wait(QMutex *mutex)
{
    ::ResetEvent(m_waitHandle);

    mutex->unlock();

    HANDLE handles[] = { m_eventHandle, m_waitHandle };
    while (::WaitForMultipleObjects(2, handles, false, INFINITE) == WAIT_OBJECT_0)
        processEvents();
  
    mutex->lock();
}

void DirectShowEventLoop::wake()
{
    ::SetEvent(m_waitHandle);
}

void DirectShowEventLoop::postEvent(QObject *receiver, QEvent *event)
{
    QMutexLocker locker(&m_mutex);

    DirectShowPostedEvent *post = new DirectShowPostedEvent(receiver, event);

    if (m_postsTail)
        m_postsTail->next = post;
    else
        m_postsHead = post;

    m_postsTail = post;

    ::SetEvent(m_eventHandle);
}

bool DirectShowEventLoop::event(QEvent *event)
{
    if (event->type() == QEvent::WinEventAct) {
        processEvents();

        return true;
    } else {
        return QWinEventNotifier::event(event);
    }
}

void DirectShowEventLoop::processEvents()
{
    QMutexLocker locker(&m_mutex);

    while(m_postsHead) {
        ::ResetEvent(m_eventHandle);

        DirectShowPostedEvent *post = m_postsHead;
        m_postsHead = m_postsHead->next;

        locker.unlock();
        QCoreApplication::sendEvent(post->receiver, post->event);
        delete post;
        locker.relock();
    }

    m_postsTail = 0;
}

QT_END_NAMESPACE
