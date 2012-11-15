/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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


#ifndef QSYSTEMLOCK_H
#define QSYSTEMLOCK_H

#include <QtCore/qstring.h>

QT_BEGIN_HEADER

#ifndef QT_NO_SYSTEMLOCK

QT_FORWARD_DECLARE_CLASS(QSystemLockPrivate)

class QSystemLock
{

public:
    enum SystemLockError
    {
        NoError,
        UnknownError
    };

    QSystemLock(const QString &key);
    ~QSystemLock();

    void setKey(const QString &key);
    QString key() const;

    enum LockMode
    {
        ReadOnly,
        ReadWrite
    };

    bool lock(LockMode mode = ReadWrite);
    bool unlock();

    SystemLockError error() const;
    QString errorString() const;

private:
    Q_DISABLE_COPY(QSystemLock)

    QSystemLockPrivate *d;
};

class QSystemLocker
{

public:
    inline QSystemLocker(QSystemLock *systemLock,
                         QSystemLock::LockMode mode = QSystemLock::ReadWrite) : q_lock(systemLock)
    {
        autoUnLocked = relock(mode);
    }

    inline ~QSystemLocker()
    {
        if (autoUnLocked)
            unlock();
    }

    inline QSystemLock *systemLock() const
    {
        return q_lock;
    }

    inline bool relock(QSystemLock::LockMode mode = QSystemLock::ReadWrite)
    {
        return (q_lock && q_lock->lock(mode));
    }

    inline bool unlock()
    {
        if (q_lock && q_lock->unlock()) {
            autoUnLocked = false;
            return true;
        }
        return false;
    }

private:
    Q_DISABLE_COPY(QSystemLocker)

    bool autoUnLocked;
    QSystemLock *q_lock;
};

#endif // QT_NO_SYSTEMLOCK

QT_END_HEADER

#endif // QSYSTEMLOCK_H

