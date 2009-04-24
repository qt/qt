/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qsharedmemory.h"
#include "qsharedmemory_p.h"
#include "qsystemsemaphore.h"
#include "qcore_symbian_p.h"
#include <qdebug.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SHAREDMEMORY

#define QSHAREDMEMORY_DEBUG

QSharedMemoryPrivate::QSharedMemoryPrivate() : QObjectPrivate(),
        memory(0), size(0), error(QSharedMemory::NoError),
           systemSemaphore(QString()), lockedByMe(false)
{
}

void QSharedMemoryPrivate::setErrorString(const QString &function, TInt errorCode)
{
    if (errorCode == KErrNone)
        return;
    switch (errorCode) {
    case KErrAlreadyExists:
        error = QSharedMemory::AlreadyExists;
        errorString = QSharedMemory::tr("%1: already exists").arg(function);
    break;
    case KErrNotFound:
        error = QSharedMemory::NotFound;
        errorString = QSharedMemory::tr("%1: doesn't exists").arg(function);
        break;
    case KErrArgument:
        error = QSharedMemory::InvalidSize;
        errorString = QSharedMemory::tr("%1: invalid size").arg(function);
        break;
    case KErrNoMemory:
        error = QSharedMemory::OutOfResources;
        errorString = QSharedMemory::tr("%1: out of resources").arg(function);
        break;
    case KErrPermissionDenied:
        error = QSharedMemory::PermissionDenied;
        errorString = QSharedMemory::tr("%1: permission denied").arg(function);
        break;
    default:
        errorString = QSharedMemory::tr("%1: unknown error %2").arg(function).arg(errorCode);
        error = QSharedMemory::UnknownError;
#if defined QSHAREDMEMORY_DEBUG
        qDebug() << errorString << "key" << key;
#endif
    }
}

key_t QSharedMemoryPrivate::handle()
{
    // Not really cost effective to check here if shared memory is attachable, as it requires
    // exactly the same call as attaching, so always assume handle is valid and return failure
    // from attach.
    return 1;
}

bool QSharedMemoryPrivate::cleanHandle()
{
    chunk.Close();
    return true;
}

bool QSharedMemoryPrivate::create(int size)
{
    // Get a windows acceptable key
    QString safeKey = makePlatformSafeKey(key);
    QString function = QLatin1String("QSharedMemory::create");
    if (safeKey.isEmpty()) {
        error = QSharedMemory::KeyError;
        errorString = QSharedMemory::tr("%1: key error").arg(function);
        return false;
    }

    HBufC* buffer = qt_QString2HBufCNewL(safeKey);

    TInt err = chunk.CreateGlobal(*buffer, size, size);

    delete buffer;

    setErrorString(function, err);

    if (err != KErrNone)
        return false;

    // Zero out the created chunk
    Mem::FillZ(chunk.Base(), chunk.Size());

    return true;
}

bool QSharedMemoryPrivate::attach(QSharedMemory::AccessMode mode)
{
    // Grab a pointer to the memory block
    TBool readOnly = (mode == QSharedMemory::ReadOnly ? true : false);

    if (!chunk.Handle()) {
        QString function = QLatin1String("QSharedMemory::handle");
        QString safeKey = makePlatformSafeKey(key);
        if (safeKey.isEmpty()) {
            error = QSharedMemory::KeyError;
            errorString = QSharedMemory::tr("%1: unable to make key").arg(function);
            return false;
        }

        HBufC* buffer = qt_QString2HBufCNewL(safeKey);

        TInt err = KErrNoMemory;

        if (buffer) {
            err = chunk.OpenGlobal(*buffer, false);
        }

        delete buffer;

        if (err != KErrNone) {
            setErrorString(function, err);
            return false;
        }
    }

    size = chunk.Size();
    memory = chunk.Base();

    return true;
}

bool QSharedMemoryPrivate::detach()
{
    chunk.Close();

    memory = 0;
    size = 0;

    return true;
}

#endif //QT_NO_SHAREDMEMORY

QT_END_NAMESPACE
