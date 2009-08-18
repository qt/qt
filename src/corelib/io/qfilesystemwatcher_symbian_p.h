/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
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
            bool aIsDir, TInt aPriority = EPriorityStandard);
    ~CNotifyChangeEvent();
    static CNotifyChangeEvent* New(RFs &fsSession, const TDesC& file,
            QSymbianFileSystemWatcherEngine* engine, bool aIsDir);

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
