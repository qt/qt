/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QSYSTEMLOCK_P_H
#define QSYSTEMLOCK_P_H

#ifndef QT_NO_SYSTEMLOCK

#include "qsystemlock.h"
#include "private/qsharedmemory_p.h"
#ifndef Q_OS_WINCE
#include <sys/types.h>
#endif

#define MAX_LOCKS 64

class QSystemLockPrivate
{

public:
    QSystemLockPrivate();

    QString makeKeyFileName()
    {
        return QSharedMemoryPrivate::makePlatformSafeKey(key, QLatin1String("qipc_systemlock_"));
    }

    void setErrorString(const QString &function);

#ifdef Q_OS_WIN
    HANDLE handle();
    bool lock(HANDLE, int count);
    bool unlock(HANDLE, int count);
#else
    key_t handle();
#endif
    void cleanHandle();

    enum Operation {
        Lock,
        Unlock
    };
    bool modifySemaphore(Operation op, QSystemLock::LockMode mode = QSystemLock::ReadOnly);

    QString key;
    QString fileName;
#ifdef Q_OS_WIN
    HANDLE semaphore;
    HANDLE semaphoreLock;
#else
    int semaphore;
#endif
    int lockCount;
    QSystemLock::LockMode lockedMode;

    QSystemLock::SystemLockError error;
    QString errorString;

private:
#ifndef Q_OS_WIN
    key_t unix_key;
    bool createdFile;
    bool createdSemaphore;
#endif
};

#endif // QT_NO_SYSTEMLOCK

#endif // QSYSTEMLOCK_P_H

