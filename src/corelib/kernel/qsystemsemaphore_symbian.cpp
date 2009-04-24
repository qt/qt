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

#include "qsystemsemaphore.h"
#include "qsystemsemaphore_p.h"
#include "qcoreapplication.h"
#include <qdebug.h>

#include <qcore_symbian_p.h>
#include <e32cmn.h>
QT_BEGIN_NAMESPACE
        
#ifndef QT_NO_SYSTEMSEMAPHORE

QSystemSemaphorePrivate::QSystemSemaphorePrivate() :
        error(QSystemSemaphore::NoError)
{
}

void QSystemSemaphorePrivate::setErrorString(const QString &function, int err)
{
    if (err == KErrNone){
        return;
    }
    switch(err){
    case KErrAlreadyExists:
        errorString = QCoreApplication::tr("%1: already exists", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::AlreadyExists;
    break;
    case KErrNotFound:    
        errorString = QCoreApplication::tr("%1: doesn't exists", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::NotFound;
    break;
    case KErrNoMemory:
    case KErrInUse:
        errorString = QCoreApplication::tr("%1: out of resources", "QSystemSemaphore").arg(function);
        error = QSystemSemaphore::OutOfResources;
    break;
default:
    errorString = QCoreApplication::tr("%1: unknown error %2", "QSystemSemaphore").arg(function).arg(err);
    error = QSystemSemaphore::UnknownError;
    }

#if defined QSYSTEMSEMAPHORE_DEBUG
        qDebug() << errorString << "key" << key;
#endif
}

int QSystemSemaphorePrivate::handle(QSystemSemaphore::AccessMode)
{
    // don't allow making handles on empty keys
    if (key.isEmpty())
        return 0;
    QString safeName = makeKeyFileName();
    HBufC* name = qt_QString2HBufCNewL(safeName);
    int err;
    err = semaphore.OpenGlobal(*name,EOwnerProcess);
    if (err == KErrNotFound){
        err = semaphore.CreateGlobal(*name,initialValue, EOwnerProcess);
    }
    delete name;
    if (err){
        setErrorString(QLatin1String("QSystemSemaphore::handle"),err);
        return 0;
    }
    return semaphore.Handle();
}

void QSystemSemaphorePrivate::cleanHandle()
{
    semaphore.Close();
}

bool QSystemSemaphorePrivate::modifySemaphore(int count)
{
    if (0 == handle())
        return false;

    if (count > 0) {
        semaphore.Signal(count);
    } else {
        semaphore.Wait();
        }
    return true;
}

#endif //QT_NO_SYSTEMSEMAPHORE

QT_END_NAMESPACE
