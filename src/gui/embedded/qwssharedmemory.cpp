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

#include "qwssharedmemory_p.h"

#if !defined(QT_NO_QWS_MULTIPROCESS)

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

//#define QT_SHM_DEBUG

QT_BEGIN_NAMESPACE

QWSSharedMemory::QWSSharedMemory()
    : shmId(-1), shmBase(0), shmSize(0)
{
}

QWSSharedMemory::~QWSSharedMemory()
{
    detach();
}

bool QWSSharedMemory::create(int size)
{
    if (shmId != -1)
        detach();

    shmId = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
    if (shmId == -1) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create():");
        qWarning("Error allocating shared memory of size %d", size);
#endif
        return false;
    }
    shmBase = shmat(shmId, 0, 0);
    // On Linux, it is possible to attach a shared memory segment even if it
    // is already marked to be deleted. However, POSIX.1-2001 does not specify
    // this behaviour and many other implementations do not support it.
    shmctl(shmId, IPC_RMID, 0);
    if (shmBase == (void*)-1 || !shmBase) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create():");
        qWarning("Error attaching to shared memory id %d", shmId);
#endif
        detach();
        return false;
    }

    return true;
}

bool QWSSharedMemory::attach(int id)
{
    if (shmId == id)
        return id != -1;

    detach();

    if (id == -1)
        return false;

    shmId = id;
    shmBase = shmat(shmId, 0, 0);
    if (shmBase == (void*)-1 || !shmBase) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::attach():");
        qWarning("Error attaching to shared memory id %d", shmId);
#endif
        detach();
        return false;
    }

    return true;
}

void QWSSharedMemory::detach()
{
    if (shmBase && shmBase != (void*)-1)
        shmdt(shmBase);
    shmBase = 0;
    shmSize = 0;
    shmId = -1;
}

int QWSSharedMemory::size() const
{
    if (shmId == -1)
        return 0;

    if (!shmSize) {
        struct shmid_ds shm;
        shmctl(shmId, IPC_STAT, &shm);
        const_cast<QWSSharedMemory *>(this)->shmSize = shm.shm_segsz;
    }

    return shmSize;
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS
