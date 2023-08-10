/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QMUTEX_H
#define QMUTEX_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>
#include <new>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#if !defined(QT_NO_THREAD) && !defined(qdoc)

class QMutexPrivate;

class Q_CORE_EXPORT QBasicMutex
{
public:
    inline void lock() {
        if (!fastTryLock())
            lockInternal();
    }

    inline void unlock() {
        Q_ASSERT(d.load()); //mutex must be locked
        if (!d.testAndSetRelease(dummyLocked(), 0))
            unlockInternal();
    }

    bool tryLock(int timeout = 0) {
        return fastTryLock() || lockInternal(timeout);
    }

    bool isRecursive();
/*
#if defined(QT3_SUPPORT)
    inline QT3_SUPPORT bool locked()
    {
        if (!tryLock())
            return true;
        unlock();
        return false;
    }
    inline QT3_SUPPORT_CONSTRUCTOR QMutex(bool recursive)
    {
        new (this) QMutex(recursive ? Recursive : NonRecursive);
    }
#endif
*/
private:
    inline bool fastTryLock() {
        return d.testAndSetAcquire(0, dummyLocked());
    }
    bool lockInternal(int timeout = -1);
    void unlockInternal();
    QBasicAtomicPointer<QMutexPrivate> d;
    static inline QMutexPrivate *dummyLocked() {
        return reinterpret_cast<QMutexPrivate *>(quintptr(1));
    }

    friend class QMutex;
    friend class QMutexPrivate;
};

class Q_CORE_EXPORT QMutex : public QBasicMutex {
public:
    enum RecursionMode { NonRecursive, Recursive };
    explicit QMutex(RecursionMode mode = NonRecursive);
    ~QMutex();
private:
    Q_DISABLE_COPY(QMutex)
};

class Q_CORE_EXPORT QMutexLocker
{
public:
    inline explicit QMutexLocker(QBasicMutex *m)
    {
        Q_ASSERT_X((reinterpret_cast<quintptr>(m) & quintptr(1u)) == quintptr(0),
                   "QMutexLocker", "QMutex pointer is misaligned");
        if (m) {
            m->lock();
            val = reinterpret_cast<quintptr>(m) | quintptr(1u);
        } else {
            val = 0;
        }
    }
    inline ~QMutexLocker() { unlock(); }

    inline void unlock()
    {
        if ((val & quintptr(1u)) == quintptr(1u)) {
            val &= ~quintptr(1u);
            mutex()->unlock();
        }
    }

    inline void relock()
    {
        if (val) {
            if ((val & quintptr(1u)) == quintptr(0u)) {
                mutex()->lock();
                val |= quintptr(1u);
            }
        }
    }

#if defined(Q_CC_MSVC)
#pragma warning( push )
#pragma warning( disable : 4312 ) // ignoring the warning from /Wp64
#endif

    inline QMutex *mutex() const
    {
        return reinterpret_cast<QMutex *>(val & ~quintptr(1u));
    }

#if defined(Q_CC_MSVC)
#pragma warning( pop )
#endif

private:
    Q_DISABLE_COPY(QMutexLocker)

    quintptr val;
};

#else // QT_NO_THREAD or qdoc

class Q_CORE_EXPORT QMutex
{
public:
    enum RecursionMode { NonRecursive, Recursive };

    inline explicit QMutex(RecursionMode mode = NonRecursive) { Q_UNUSED(mode); }

    static inline void lock() {}
    static inline bool tryLock(int timeout = 0) { Q_UNUSED(timeout); return true; }
    static inline void unlock() {}
    static inline bool isRecursive() { return true; }

#if defined(QT3_SUPPORT)
    static inline QT3_SUPPORT bool locked() { return false; }
#endif

private:
    Q_DISABLE_COPY(QMutex)
};

class Q_CORE_EXPORT QMutexLocker
{
public:
    inline explicit QMutexLocker(QMutex *) {}
    inline ~QMutexLocker() {}

    static inline void unlock() {}
    static void relock() {}
    static inline QMutex *mutex() { return nullptr; }

private:
    Q_DISABLE_COPY(QMutexLocker)
};

typedef QMutex QBasicMutex;

#endif // QT_NO_THREAD or qdoc

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMUTEX_H
