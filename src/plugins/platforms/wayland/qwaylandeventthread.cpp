/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qwaylandeventthread.h"

#include <QtCore/QMetaType>

QWaylandEventThread::QWaylandEventThread(QWaylandDisplay *display)
    : QObject()
    , mDisplay(display)
    , mReadNotifier(0)
    , mWriteNotifier(0)
    , mFd(0)
    , mScreensInitialized(false)
{
    qRegisterMetaType<uint32_t>("uint32_t");
    QThread *thread = new QThread(this);
    moveToThread(thread);
    connect(thread,SIGNAL(started()),SLOT(runningInThread()));
    thread->start();

}

QWaylandEventThread::~QWaylandEventThread()
{
    close(mFd);
}

void QWaylandEventThread::runningInThread()
{

    wl_display_add_global_listener(mDisplay->wl_display(),
                                   QWaylandEventThread::displayHandleGlobal, this);
    int fd = wl_display_get_fd(mDisplay->wl_display(), sourceUpdate, this);
    mReadNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(mReadNotifier,
            SIGNAL(activated(int)), this, SLOT(readEvents()));

    mWriteNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    connect(mWriteNotifier,
            SIGNAL(activated(int)), this, SLOT(flushRequests()));
    mWriteNotifier->setEnabled(false);

    readEvents();
}


int QWaylandEventThread::sourceUpdate(uint32_t mask, void *data)
{
    QWaylandEventThread *eventThread = (QWaylandEventThread *) data;

    /* FIXME: We get a callback here when we ask wl_display for the
     * fd, but at that point we don't have the socket notifier as we
     * need the fd to create that.  We'll probably need to split that
     * API into get_fd and set_update_func functions. */
    if (eventThread->mWriteNotifier == NULL)
        return 0;

    QMetaObject::invokeMethod(eventThread->mWriteNotifier,"setEnabled",Qt::QueuedConnection,
                              Q_ARG(bool,mask & WL_DISPLAY_WRITABLE));
//    eventThread->mWriteNotifier->setEnabled(mask & WL_DISPLAY_WRITABLE);

    return 0;
}

void QWaylandEventThread::readEvents()
{
    wl_display_iterate(mDisplay->wl_display(), WL_DISPLAY_READABLE);
}


void QWaylandEventThread::flushRequests()
{
    wl_display_iterate(mDisplay->wl_display(), WL_DISPLAY_WRITABLE);
}

void QWaylandEventThread::outputHandleGeometry(void *data,
                                           struct wl_output *output,
                                           int32_t x, int32_t y,
                                           int32_t width, int32_t height)
{
    //call back function called from another thread;
    //but its safe to call createScreen from another thread since
    //QWaylandScreen does a moveToThread
    QWaylandEventThread *waylandEventThread = static_cast<QWaylandEventThread *>(data);
    QRect outputRect = QRect(x, y, width, height);
    waylandEventThread->mDisplay->createNewScreen(output,outputRect);
    waylandEventThread->mScreensInitialized = true;
    waylandEventThread->mWaitForScreens.wakeAll();
}

const struct wl_output_listener QWaylandEventThread::outputListener = {
    QWaylandEventThread::outputHandleGeometry
};

void QWaylandEventThread::displayHandleGlobal(wl_display *display,
                                              uint32_t id,
                                              const char *interface,
                                              uint32_t version,
                                              void *data)
{
    if (strcmp(interface, "output") == 0) {
        struct wl_output *output = wl_output_create(display, id);
        wl_output_add_listener(output, &outputListener, data);
    }
    Q_UNUSED(display);
    QWaylandEventThread *that = static_cast<QWaylandEventThread *>(data);
    QByteArray interfaceByteArray(interface);
    QMetaObject::invokeMethod(that->mDisplay,"displayHandleGlobal",Qt::QueuedConnection,
                              Q_ARG(uint32_t,id),Q_ARG(QByteArray,interfaceByteArray),Q_ARG(uint32_t,version));
}

void QWaylandEventThread::waitForScreens()
{
    QMutex lock;
    lock.lock();
    if (!mScreensInitialized) {
        mWaitForScreens.wait(&lock);
    }
}
