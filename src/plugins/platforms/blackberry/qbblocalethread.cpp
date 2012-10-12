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

//#define QBBLOCALETHREAD_DEBUG

#ifdef QBBLOCALETHREAD_ENABLED

#include "qbblocalethread.h"

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QWindowSystemInterface>
#include <QByteArray>
#include <QList>
#include <QDebug>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NAV_CONTROL_PATH    "/pps/services/confstr/_CS_LOCALE"
#define PPS_BUFFER_SIZE     4096

QBBLocaleThread::QBBLocaleThread()
    : mFd(-1),
      mQuit(false)
{
}

QBBLocaleThread::~QBBLocaleThread()
{
    // block until thread terminates
    shutdown();
}

void QBBLocaleThread::run()
{
#if defined(QBBLOCALETHREAD_DEBUG)
    qDebug() << "QBB: Locale thread started";
#endif

    // open connection to Locale
    errno = 0;
    mFd = open(NAV_CONTROL_PATH, O_RDONLY);
    if (mFd == -1) {
        qWarning("QBB: failed to open Locale pps, errno=%d", errno);
        return;
    }

    // allocate buffer for pps data
    char buffer[PPS_BUFFER_SIZE];

    // loop indefinitely
    while (!mQuit) {

        // attempt to read pps data
        errno = 0;
        int bytes = read(mFd, buffer, PPS_BUFFER_SIZE - 1);
        if (bytes == -1) {
            qWarning("QBB: failed to read Locale pps, errno=%d", errno);
            return;
        }

        // check if pps data was received
        if (bytes > 0) {
            // Notify the world that the locale has changed.
            QWindowSystemInterface::handleLocaleChange();
        }

        // yield
        msleep(5);
    }

    // close connection to Locale
    close(mFd);

#if defined(QBBLOCALETHREAD_DEBUG)
    qDebug() << "QBB: Locale thread stopped";
#endif
}

void QBBLocaleThread::shutdown()
{
    // signal thread to terminate
    mQuit = true;

#if defined(QBBLOCALETHREAD_DEBUG)
    qDebug() << "QBB: Locale thread shutdown begin";
#endif

    // block until thread terminates
    wait();

#if defined(QBBLOCALETHREAD_DEBUG)
    qDebug() << "QBB: Locale thread shutdown end";
#endif
}

#endif // QBBLOCALETHREAD_ENABLED
