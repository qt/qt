/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#include "qfilesystemwatcher.h"
#include "qfilesystemwatcher_symbian_p.h"
#include "qfileinfo.h"
#include "qdebug.h"
#include "private/qcore_symbian_p.h"
#include <QDir>

#ifndef QT_NO_FILESYSTEMWATCHER


QT_BEGIN_NAMESPACE

CNotifyChangeEvent* CNotifyChangeEvent::New(RFs &fs, const TDesC& file,
        QSymbianFileSystemWatcherEngine* e)
{
    CNotifyChangeEvent* self = new CNotifyChangeEvent(fs, file, e);
    return self;
}

CNotifyChangeEvent::CNotifyChangeEvent(RFs &fs, const TDesC& file,
    QSymbianFileSystemWatcherEngine* e, TInt aPriority) :
        CActive(aPriority),
        fsSession(fs),
        watchedPath(file),
        engine(e)
{
    fsSession.NotifyChange(ENotifyAll, iStatus, file);
    CActiveScheduler::Add(this);
    SetActive();
}

CNotifyChangeEvent::~CNotifyChangeEvent()
{
    Cancel();
}

void CNotifyChangeEvent::RunL()
{
    if (iStatus.Int() == KErrNone){
        fsSession.NotifyChange(ENotifyAll, iStatus, watchedPath);
        SetActive();
        engine->emitPathChanged(this);
    } else {
        qWarning("CNotifyChangeEvent::RunL() - Failed to order change notifications: %d", iStatus.Int());
    }
}

void CNotifyChangeEvent::DoCancel()
{
    fsSession.NotifyChangeCancel();
}

QSymbianFileSystemWatcherEngine::QSymbianFileSystemWatcherEngine() :
    watcherStarted(false)
{
    moveToThread(this);
}

QSymbianFileSystemWatcherEngine::~QSymbianFileSystemWatcherEngine()
{
    stop();
}

QStringList QSymbianFileSystemWatcherEngine::addPaths(const QStringList &paths,
        QStringList *files, QStringList *directories)
{
    QMutexLocker locker(&mutex);
    QStringList p = paths;

    if (!startWatcher()) {
        qWarning("Could not start QSymbianFileSystemWatcherEngine thread");

        return p;
    }

    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QFileInfo fi(path);
        if (!fi.exists())
            continue;

        bool isDir = fi.isDir();
        if (isDir) {
            if (directories->contains(path))
                continue;
        } else {
            if (files->contains(path))
                continue;
        }

        // Use absolute filepath as relative paths seem to have some issues.
        QString filePath = fi.absoluteFilePath();
        if(isDir && filePath.at(filePath.size()-1) != QChar('/')) {
            filePath += QChar('/');
        }

        currentEvent = NULL;
        QMetaObject::invokeMethod(this,
                "addNativeListener",
                Qt::QueuedConnection,
                Q_ARG(QString, filePath));

        syncCondition.wait(&mutex);

        if (currentEvent) {
            currentEvent->isDir = isDir;

            activeObjectToPath.insert(currentEvent, path);
            it.remove();

            if (isDir)
                 directories->append(path);
             else
                 files->append(path);
        }
    }

    return p;
}

QStringList QSymbianFileSystemWatcherEngine::removePaths(const QStringList &paths,
        QStringList *files, QStringList *directories)
{
    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();

        currentEvent = activeObjectToPath.key(path);
        if (!currentEvent)
            continue;
        activeObjectToPath.remove(currentEvent);

        QMetaObject::invokeMethod(this,
                "removeNativeListener",
                Qt::QueuedConnection);

        syncCondition.wait(&mutex);

        it.remove();

        files->removeAll(path);
        directories->removeAll(path);
    }

    if (activeObjectToPath.size() == 0)
        stop();

    return p;
}

void QSymbianFileSystemWatcherEngine::emitPathChanged(CNotifyChangeEvent *e)
{
    QMutexLocker locker(&mutex);

    QString path = activeObjectToPath.value(e);
    QFileInfo fi(path);

    if (e->isDir) {
        emit directoryChanged(path, !fi.exists());
    } else {
        emit fileChanged(path, !fi.exists());
    }
}

void QSymbianFileSystemWatcherEngine::stop()
{
    QMetaObject::invokeMethod(this, "quit");
    wait();
}

// This method must be called inside mutex
bool QSymbianFileSystemWatcherEngine::startWatcher()
{
    bool retval = true;

    if (!watcherStarted) {
#if defined(Q_OS_SYMBIAN)
        setStackSize(0x5000);
#endif
        start();
        syncCondition.wait(&mutex);

        if (errorCode != KErrNone) {
            retval = false;
        } else {
            watcherStarted = true;
        }
    }
    return retval;
}


void QSymbianFileSystemWatcherEngine::run()
{
    // Initialize file session

    errorCode = fsSession.Connect();

    mutex.lock();
    syncCondition.wakeOne();
    mutex.unlock();

    if (errorCode == KErrNone) {
        exec();

        foreach(CNotifyChangeEvent* e, activeObjectToPath.keys()) {
            e->Cancel();
            delete e;
        }

        activeObjectToPath.clear();
        fsSession.Close();
        watcherStarted = false;
    }
}

void QSymbianFileSystemWatcherEngine::addNativeListener(const QString &directoryPath)
{
    QMutexLocker locker(&mutex);
    HBufC* buffer = qt_QString2HBufCNewL(QDir::toNativeSeparators(directoryPath));
    currentEvent = CNotifyChangeEvent::New(fsSession, *buffer, this);
    delete buffer;
    syncCondition.wakeOne();
}

void QSymbianFileSystemWatcherEngine::removeNativeListener()
{
    QMutexLocker locker(&mutex);
    currentEvent->Cancel();
    delete currentEvent;
    currentEvent = NULL;
    syncCondition.wakeOne();
}


QT_END_NAMESPACE
#endif // QT_NO_FILESYSTEMWATCHER
