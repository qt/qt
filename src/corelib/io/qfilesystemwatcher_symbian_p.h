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

#ifndef QFILESYSTEMWATCHER_SYMBIAN_P_H
#define QFILESYSTEMWATCHER_SYMBIAN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qfilesystemwatcher_p.h"
#include "qhash.h"
#include "qmutex.h"
#include "qwaitcondition.h"

#ifndef QT_NO_FILESYSTEMWATCHER

#include <e32base.h>
#include <f32file.h>

QT_BEGIN_NAMESPACE

class QSymbianFileSystemWatcherEngine;

class CNotifyChangeEvent : public CActive
{
public:
    CNotifyChangeEvent(RFs &fsSession, const TDesC& file, QSymbianFileSystemWatcherEngine* engine,
            TInt aPriority = EPriorityStandard);
    ~CNotifyChangeEvent();
    static CNotifyChangeEvent* New(RFs &fsSession, const TDesC& file,
            QSymbianFileSystemWatcherEngine* engine);

    bool isDir;

private:
    void RunL();
    void DoCancel();

    RFs &fsSession;
    TPath watchedPath;
    QSymbianFileSystemWatcherEngine *engine;
};

class QSymbianFileSystemWatcherEngine : public QFileSystemWatcherEngine
{
    Q_OBJECT

public:
    QSymbianFileSystemWatcherEngine();
    ~QSymbianFileSystemWatcherEngine();

    QStringList addPaths(const QStringList &paths, QStringList *files,
            QStringList *directories);
    QStringList removePaths(const QStringList &paths, QStringList *files,
            QStringList *directories);

    void stop();

protected:
    void run();

public Q_SLOTS:
    void addNativeListener(const QString &directoryPath);
    void removeNativeListener();

private:
    friend class CNotifyChangeEvent;
    void emitPathChanged(CNotifyChangeEvent *e);

    bool startWatcher();

    RFs fsSession;
    QHash<CNotifyChangeEvent*, QString> activeObjectToPath;
    QMutex mutex;
    QWaitCondition syncCondition;
    int errorCode;
    bool watcherStarted;
    CNotifyChangeEvent *currentEvent;
};

#endif // QT_NO_FILESYSTEMWATCHER

QT_END_NAMESPACE

#endif // QFILESYSTEMWATCHER_WIN_P_H
