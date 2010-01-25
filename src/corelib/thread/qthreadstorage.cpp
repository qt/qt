/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qthreadstorage.h"

#ifndef QT_NO_THREAD
#include "qthread.h"
#include "qthread_p.h"
#include "qmutex.h"

#include <string.h>

QT_BEGIN_NAMESPACE

// #define THREADSTORAGE_DEBUG
#ifdef THREADSTORAGE_DEBUG
#  define DEBUG_MSG qtsDebug

#  include <stdio.h>
#  include <stdarg.h>
void qtsDebug(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    fprintf(stderr, "QThreadStorage: ");
    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");

    va_end(va);
}
#else
#  define DEBUG_MSG if(false)qDebug
#endif

Q_GLOBAL_STATIC(QMutex, mutex)
typedef QVector<void (*)(void *)> DestructorMap;
Q_GLOBAL_STATIC(DestructorMap, destructors)

QThreadStorageData::QThreadStorageData(void (*func)(void *))
{
    QMutexLocker locker(mutex());
    DestructorMap *destr = destructors();
    for (id = 0; id < destr->count(); id++) {
        if (destr->at(id) == 0)
            break;
    }
    if (id == destr->count()) {
        destr->append(func);
    } else {
        (*destr)[id] = func;
    }
    DEBUG_MSG("QThreadStorageData: Allocated id %d, destructor %p", id, func);
}

QThreadStorageData::~QThreadStorageData()
{
    DEBUG_MSG("QThreadStorageData: Released id %d", id);
    QMutexLocker locker(mutex());
    if (destructors())
        (*destructors())[id] = 0;
}

void **QThreadStorageData::get() const
{
    QThreadData *data = QThreadData::current();
    if (!data) {
        qWarning("QThreadStorage::get: QThreadStorage can only be used with threads started with QThread");
        return 0;
    }
    QVector<void *> &tls = data->tls;
    if (tls.size() <= id)
        tls.resize(id + 1);
    void **v = &tls[id];

    DEBUG_MSG("QThreadStorageData: Returning storage %d, data %p, for thread %p",
          id,
          *v,
          data->thread);

    return *v ? v : 0;
}

void **QThreadStorageData::set(void *p)
{
    QThreadData *data = QThreadData::current();
    if (!data) {
        qWarning("QThreadStorage::set: QThreadStorage can only be used with threads started with QThread");
        return 0;
    }
    QVector<void *> &tls = data->tls;
    if (tls.size() <= id)
        tls.resize(id + 1);

    void *&value = tls[id];
    // delete any previous data
    if (value != 0) {
        DEBUG_MSG("QThreadStorageData: Deleting previous storage %d, data %p, for thread %p",
                id,
                value,
                data->thread);

        QMutexLocker locker(mutex());
        void (*destructor)(void *) = destructors()->value(id);
        locker.unlock();

        void *q = value;
        value = 0;

        destructor(q);
    }

    // store new data
    value = p;
    DEBUG_MSG("QThreadStorageData: Set storage %d for thread %p to %p", id, data->thread, p);
    return &value;
}

void QThreadStorageData::finish(void **p)
{
    QVector<void *> *tls = reinterpret_cast<QVector<void *> *>(p);
    if (!tls || tls->isEmpty() || !mutex())
        return; // nothing to do

    DEBUG_MSG("QThreadStorageData: Destroying storage for thread %p", QThread::currentThread());

    for(int i = tls->size() - 1; i >= 0; i--) {
        void *&value = (*tls)[i];
        void *q = value;
        value = 0;

        if (!q) {
            // data already deleted
            continue;
        }

        QMutexLocker locker(mutex());
        void (*destructor)(void *) = destructors()->value(i);
        locker.unlock();

        if (!destructor) {
            if (QThread::currentThread())
                qWarning("QThreadStorage: Thread %p exited after QThreadStorage %d destroyed",
                         QThread::currentThread(), i);
            continue;
        }
        destructor(q); //crash here might mean the thread exited after qthreadstorage was destroyed
    }
    tls->clear();
}

/*!
    \class QThreadStorage
    \brief The QThreadStorage class provides per-thread data storage.

    \threadsafe

    \ingroup thread

    QThreadStorage is a template class that provides per-thread data
    storage.

    \e{Note that due to compiler limitations, QThreadStorage can only
    store pointers.}

    The setLocalData() function stores a single thread-specific value
    for the calling thread. The data can be accessed later using
    localData(). QThreadStorage takes ownership of the data (which
    must be created on the heap with \c new) and deletes it when the
    thread exits, either normally or via termination.

    The hasLocalData() function allows the programmer to determine if
    data has previously been set using the setLocalData() function.
    This is also useful for lazy initializiation.

    For example, the following code uses QThreadStorage to store a
    single cache for each thread that calls the cacheObject() and
    removeFromCache() functions. The cache is automatically
    deleted when the calling thread exits.

    \snippet doc/src/snippets/threads/threads.cpp 7
    \snippet doc/src/snippets/threads/threads.cpp 8
    \snippet doc/src/snippets/threads/threads.cpp 9

    \section1 Caveats

    \list

    \o As noted above, QThreadStorage can only store pointers due to
    compiler limitations.

    \o The QThreadStorage destructor does not delete per-thread data.
    QThreadStorage only deletes per-thread data when the thread exits
    or when setLocalData() is called multiple times.

    \o QThreadStorage can be used to store data for the \c main()
    thread. QThreadStorage deletes all data set for the \c main()
    thread when QApplication is destroyed, regardless of whether or
    not the \c main() thread has actually finished.

    \endlist

    \sa QThread
*/

/*!
    \fn QThreadStorage::QThreadStorage()

    Constructs a new per-thread data storage object.
*/

/*!
    \fn QThreadStorage::~QThreadStorage()

    Destroys the per-thread data storage object.

    Note: The per-thread data stored is not deleted. Any data left
    in QThreadStorage is leaked. Make sure that all threads using
    QThreadStorage have exited before deleting the QThreadStorage.

    \sa hasLocalData()
*/

/*!
    \fn bool QThreadStorage::hasLocalData() const

    Returns true if the calling thread has non-zero data available;
    otherwise returns false.

    \sa localData()
*/

/*!
    \fn T &QThreadStorage::localData()

    Returns a reference to the data that was set by the calling
    thread.

    Note: QThreadStorage can only store pointers. This function
    returns a reference to the pointer that was set by the calling
    thread. The value of this reference is 0 if no data was set by
    the calling thread,

    \sa hasLocalData()
*/

/*!
    \fn const T QThreadStorage::localData() const
    \overload

    Returns a copy of the data that was set by the calling thread.

    Note: QThreadStorage can only store pointers. This function
    returns a pointer to the data that was set by the calling thread.
    If no data was set by the calling thread, this function returns 0.

    \sa hasLocalData()
*/

/*!
    \fn void QThreadStorage::setLocalData(T data)

    Sets the local data for the calling thread to \a data. It can be
    accessed later using the localData() functions.

    If \a data is 0, this function deletes the previous data (if
    any) and returns immediately.

    If \a data is non-zero, QThreadStorage takes ownership of the \a
    data and deletes it automatically either when the thread exits
    (either normally or via termination) or when setLocalData() is
    called again.

    Note: QThreadStorage can only store pointers. The \a data
    argument must be either a pointer to an object created on the heap
    (i.e. using \c new) or 0. You should not delete \a data
    yourself; QThreadStorage takes ownership and will delete the \a
    data itself.

    \sa localData(), hasLocalData()
*/

#endif // QT_NO_THREAD

QT_END_NAMESPACE
