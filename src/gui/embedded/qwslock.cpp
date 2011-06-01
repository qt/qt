/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qwslock_p.h"

#ifndef QT_NO_QWS_MULTIPROCESS

#include "qwssignalhandler_p.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <time.h>
#ifdef Q_OS_LINUX
#include <linux/version.h>
#endif
#include <unistd.h>

#include <private/qcore_unix_p.h>

QT_BEGIN_NAMESPACE

#ifdef QT_NO_SEMAPHORE
#error QWSLock currently requires semaphores
#endif

QWSLock::QWSLock(int id) : semId(id)
{
    static unsigned short initialValues[3] = { 1, 1, 0 };

    if (semId == -1) {
        semId = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
        if (semId == -1) {
            perror("QWSLock::QWSLock");
            qFatal("Unable to create semaphore");
        }

        qt_semun semval;
        semval.array = initialValues;
        if (semctl(semId, 0, SETALL, semval) == -1) {
            perror("QWSLock::QWSLock");
            qFatal("Unable to initialize semaphores");
        }
    }

    lockCount[0] = lockCount[1] = 0;

#ifndef QT_NO_QWS_SIGNALHANDLER
    QWSSignalHandler::instance()->addSemaphore(semId);
#endif
}

QWSLock::~QWSLock()
{
    if (semId != -1) {
#ifndef QT_NO_QWS_SIGNALHANDLER
        QWSSignalHandler::instance()->removeSemaphore(semId);
#else
        qt_semun semval;
        semval.val = 0;
        semctl(semId, 0, IPC_RMID, semval);
#endif
    }
}

bool QWSLock::up(unsigned short semNum)
{
    int ret;

    sembuf sops = { semNum, 1, 0 };
    // As the BackingStore lock is a mutex, and only one process may own
    // the lock, it's safe to use SEM_UNDO. On the other hand, the
    // Communication lock is locked by the client but unlocked by the
    // server and therefore can't use SEM_UNDO.
    if (semNum == BackingStore)
        sops.sem_flg |= SEM_UNDO;

    EINTR_LOOP(ret, semop(semId, &sops, 1));
    if (ret == -1) {
        qDebug("QWSLock::up(): %s", strerror(errno));
        return false;
    }

    return true;
}

bool QWSLock::down(unsigned short semNum, int)
{
    int ret;

    sembuf sops = { semNum, -1, 0 };
    // As the BackingStore lock is a mutex, and only one process may own
    // the lock, it's safe to use SEM_UNDO. On the other hand, the
    // Communication lock is locked by the client but unlocked by the
    // server and therefore can't use SEM_UNDO.
    if (semNum == BackingStore)
        sops.sem_flg |= SEM_UNDO;

    EINTR_LOOP(ret, semop(semId, &sops, 1));
    if (ret == -1) {
        qDebug("QWSLock::down(): %s", strerror(errno));
        return false;
    }

    return true;
}

int QWSLock::getValue(unsigned short semNum) const
{
    int ret = semctl(semId, semNum, GETVAL, 0);
    if (ret == -1)
        qDebug("QWSLock::getValue(): %s", strerror(errno));
    return ret;
}

bool QWSLock::lock(LockType type, int timeout)
{
    if (type == RegionEvent)
        return up(type);

    if (lockCount[type] > 0) {
        ++lockCount[type];
        return true;
    }

    if (down(type, timeout)) {
        ++lockCount[type];
        return true;
    }

    return false;
}

bool QWSLock::hasLock(LockType type)
{
    if (type == RegionEvent)
        return getValue(type) == 0;

    return lockCount[type] > 0;
}

void QWSLock::unlock(LockType type)
{
    if (type == RegionEvent) {
        down(type, -1);
        return;
    }

    if (lockCount[type] > 0) {
        --lockCount[type];
        if (lockCount[type] > 0)
            return;
    }

    up(type);
}

bool QWSLock::wait(LockType type, int timeout)
{
    bool ok = down(type, timeout);
    if (ok)
        unlock(type);
    return ok;
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS
