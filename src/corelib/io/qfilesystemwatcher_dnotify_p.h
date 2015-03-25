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

#ifndef QFILESYSTEMWATCHER_DNOTIFY_P_H
#define QFILESYSTEMWATCHER_DNOTIFY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qfilesystemwatcher_p.h"

#ifndef QT_NO_FILESYSTEMWATCHER

#include <qmutex.h>
#include <qhash.h>
#include <qdatetime.h>
#include <qfile.h>

QT_BEGIN_NAMESPACE

class QDnotifyFileSystemWatcherEngine : public QFileSystemWatcherEngine
{
    Q_OBJECT

public:
    virtual ~QDnotifyFileSystemWatcherEngine();

    static QDnotifyFileSystemWatcherEngine *create();

    void run();

    QStringList addPaths(const QStringList &paths, QStringList *files, QStringList *directories);
    QStringList removePaths(const QStringList &paths, QStringList *files, QStringList *directories);

    void stop();

private Q_SLOTS:
    void refresh(int);

private:
    struct Directory {
        Directory() : fd(0), parentFd(0), isMonitored(false) {}
        Directory(const Directory &o) : path(o.path),
                                        fd(o.fd),
                                        parentFd(o.parentFd),
                                        isMonitored(o.isMonitored),
                                        files(o.files) {}
        QString path;
        int fd;
        int parentFd;
        bool isMonitored;

        struct File {
            File() : ownerId(0u), groupId(0u), permissions(0u) { }
            File(const File &o) : path(o.path),
                                  ownerId(o.ownerId),
                                  groupId(o.groupId),
                                  permissions(o.permissions),
                                  lastWrite(o.lastWrite) {}
            QString path;

            bool updateInfo();

            uint ownerId;
            uint groupId;
            QFile::Permissions permissions;
            QDateTime lastWrite;
        };

        QList<File> files;
    };

    QDnotifyFileSystemWatcherEngine();

    QMutex mutex;
    QHash<QString, int> pathToFD;
    QHash<int, Directory> fdToDirectory;
    QHash<int, int> parentToFD;
};



QT_END_NAMESPACE
#endif // QT_NO_FILESYSTEMWATCHER
#endif // QFILESYSTEMWATCHER_DNOTIFY_P_H
