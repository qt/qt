/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwssharedmemory_p.h"

#if !defined(QT_NO_QWS_MULTIPROCESS)

#include <sys/types.h>
#include <sys/ipc.h>
#ifndef QT_POSIX_IPC
#include <sys/shm.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#endif
#include <fcntl.h>
#include <unistd.h>

#include <private/qcore_unix_p.h>

//#define QT_SHM_DEBUG

QT_BEGIN_NAMESPACE

#ifdef QT_POSIX_IPC
#include <QtCore/QAtomicInt>
#include <QByteArray>

static QBasicAtomicInt localUniqueId = Q_BASIC_ATOMIC_INITIALIZER(1);

static inline QByteArray makeKey(int id)
{
    return "/qwsshm_" + QByteArray::number(id, 16);
}
#endif

QWSSharedMemory::QWSSharedMemory()
    : shmId(-1), shmBase(0), shmSize(0)
#ifdef QT_POSIX_IPC
    , hand(-1)
#endif
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

#ifndef QT_POSIX_IPC
    shmId = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
#else
    // ### generate really unique IDs
    shmId = (getpid() << 16) + (localUniqueId.fetchAndAddRelaxed(1) % ushort(-1));
    QByteArray shmName = makeKey(shmId);
    EINTR_LOOP(hand, shm_open(shmName.constData(), O_RDWR | O_CREAT, 0660));
    if (hand != -1) {
        // the size may only be set once; ignore errors
        int ret;
        EINTR_LOOP(ret, ftruncate(hand, size));
        if (ret == -1)
            shmId = -1;
    } else {
        shmId = -1;
    }
#endif
    if (shmId == -1) {
#ifdef QT_SHM_DEBUG
        perror("QWSSharedMemory::create():");
        qWarning("Error allocating shared memory of size %d", size);
#endif
        detach();
        return false;
    }

#ifndef QT_POSIX_IPC
    shmBase = shmat(shmId, 0, 0);
    // On Linux, it is possible to attach a shared memory segment even if it
    // is already marked to be deleted. However, POSIX.1-2001 does not specify
    // this behaviour and many other implementations do not support it.
    shmctl(shmId, IPC_RMID, 0);
#else
    // grab the size
    QT_STATBUF st;
    if (QT_FSTAT(hand, &st) != -1) {
        shmSize = st.st_size;
        // grab the memory
        shmBase = mmap(0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, hand, 0);
    }
#endif
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
#ifndef QT_POSIX_IPC
    shmBase = shmat(shmId, 0, 0);
#else
    QByteArray shmName = makeKey(shmId);
    EINTR_LOOP(hand, shm_open(shmName.constData(), O_RDWR, 0660));
    if (hand != -1) {
        // grab the size
        QT_STATBUF st;
        if (QT_FSTAT(hand, &st) != -1) {
            shmSize = st.st_size;
            // grab the memory
            shmBase = mmap(0, shmSize, PROT_READ | PROT_WRITE, MAP_SHARED, hand, 0);
        }
    }
#endif
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
#ifndef QT_POSIX_IPC
    if (shmBase && shmBase != (void*)-1)
        shmdt(shmBase);
#else
    if (shmBase && shmBase != (void*)-1)
        munmap(shmBase, shmSize);
    if (hand > 0) {
        // get the number of current attachments
        int shm_nattch = 0;
        QT_STATBUF st;
        if (QT_FSTAT(hand, &st) == 0) {
            // subtract 2 from linkcount: one for our own open and one for the dir entry
            shm_nattch = st.st_nlink - 2;
        }
        qt_safe_close(hand);
        // if there are no attachments then unlink the shared memory
        if (shm_nattch == 0) {
            QByteArray shmName = makeKey(shmId);
            shm_unlink(shmName.constData());
        }
    }
#endif
    shmBase = 0;
    shmSize = 0;
    shmId = -1;
}

int QWSSharedMemory::size() const
{
    if (shmId == -1)
        return 0;

#ifndef QT_POSIX_IPC
    if (!shmSize) {
        struct shmid_ds shm;
        shmctl(shmId, IPC_STAT, &shm);
        shmSize = shm.shm_segsz;
    }
#endif

    return shmSize;
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_MULTIPROCESS
