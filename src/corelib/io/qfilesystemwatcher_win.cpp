/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qfilesystemwatcher.h"
#include "qfilesystemwatcher_win_p.h"

#ifndef QT_NO_FILESYSTEMWATCHER

#include <qdebug.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <qset.h>
#include <qdatetime.h>
#include <qdir.h>

QT_BEGIN_NAMESPACE

QWindowsFileSystemWatcherEngine::QWindowsFileSystemWatcherEngine()
    : msg(0)
{
    HANDLE h = QT_WA_INLINE(CreateEventW(0, false, false, 0),
                                CreateEventA(0, false, false, 0));
    if (h != INVALID_HANDLE_VALUE) {
        handles.reserve(MAXIMUM_WAIT_OBJECTS);
        handles.append(h);
    }
}

QWindowsFileSystemWatcherEngine::~QWindowsFileSystemWatcherEngine()
{
    if (handles.isEmpty())
        return;

    stop();
    wait();

    CloseHandle(handles.at(0));
    handles[0] = INVALID_HANDLE_VALUE;

    foreach (HANDLE h, handles) {
        if (h == INVALID_HANDLE_VALUE)
            continue;
        FindCloseChangeNotification(h);
    }
}

void QWindowsFileSystemWatcherEngine::run()
{
    QMutexLocker locker(&mutex);
    forever {
        QVector<HANDLE> handlesCopy = handles;
        locker.unlock();
        // qDebug() << "QWindowsFileSystemWatcherEngine: waiting on" << handlesCopy.count() << "handles";
        DWORD r = WaitForMultipleObjects(handlesCopy.count(), handlesCopy.constData(), false, INFINITE);
        locker.relock();
        do {
            if (r == WAIT_OBJECT_0) {
                int m = msg;
                msg = 0;
                if (m == 'q') {
                    // qDebug() << "thread told to quit";
                    return;
                }
                if (m != '@')  {
                    qDebug("QWindowsFileSystemWatcherEngine: unknown message '%c' send to thread", char(m));
                }
                break;
            } else if (r > WAIT_OBJECT_0 && r < WAIT_OBJECT_0 + uint(handlesCopy.count())) {
                int at = r - WAIT_OBJECT_0;
                Q_ASSERT(at < handlesCopy.count());
                HANDLE handle = handlesCopy.at(at);

                // When removing a path, FindCloseChangeNotification might actually fire a notification
                // for some reason, so we must check if the handle exist in the handles vector
                if (handles.contains(handle)) {
                    // qDebug("Acknowledged handle: %d, %p", at, handle);
                    if (!FindNextChangeNotification(handle)) {
                        qErrnoWarning("QFileSystemWatcher: FindNextChangeNotification failed");
                    }

                    QHash<QString, PathInfo> &h = pathInfoForHandle[handle];
                    QMutableHashIterator<QString, PathInfo> it(h);
                    while (it.hasNext()) {
                        QHash<QString, PathInfo>::iterator x = it.next();
                        QString absolutePath = x.value().absolutePath;
                        QFileInfo fileInfo(x.value().path);
                        // qDebug() << "checking" << x.key();
                        if (!fileInfo.exists()) {
                            // qDebug() << x.key() << "removed!";
                            if (x.value().isDir)
                                emit directoryChanged(x.value().path, true);
                            else
                                emit fileChanged(x.value().path, true);
                            h.erase(x);

                            // close the notification handle if the directory has been removed
                            if (h.isEmpty()) {
                                // qDebug() << "Thread closing handle" << handle;
                                FindCloseChangeNotification(handle);    // This one might generate a notification

                                int indexOfHandle = handles.indexOf(handle);
                                Q_ASSERT(indexOfHandle != -1);
                                handles.remove(indexOfHandle);

                                handleForDir.remove(absolutePath);
                                // h is now invalid
                            }
                        } else if (x.value().isDir) {
                            // qDebug() << x.key() << "directory changed!";
                            emit directoryChanged(x.value().path, false);
                            x.value() = fileInfo;
                        } else if (x.value() != fileInfo) {
                            // qDebug() << x.key() << "file changed!";
                            emit fileChanged(x.value().path, false);
                            x.value() = fileInfo;
                        }
                    }
                }
            } else {
                // qErrnoWarning("QFileSystemWatcher: error while waiting for change notification");
                break;  // avoid endless loop
            }
            handlesCopy = handles;
            r = WaitForMultipleObjects(handlesCopy.count(), handlesCopy.constData(), false, 0);
        } while (r != WAIT_TIMEOUT);
    }
}

QStringList QWindowsFileSystemWatcherEngine::addPaths(const QStringList &paths,
                                                      QStringList *files,
                                                      QStringList *directories)
{
    if (handles.isEmpty() || handles.count() == MAXIMUM_WAIT_OBJECTS)
        return paths;

    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QString normalPath = path;
        if ((normalPath.endsWith(QLatin1Char('/')) || normalPath.endsWith(QLatin1Char('\\')))
#ifdef Q_OS_WINCE
            && normalPath.size() > 1)
#else
            )
#endif
            normalPath.chop(1);
        QFileInfo fileInfo(normalPath.toLower());
        if (!fileInfo.exists())
            continue;

        bool isDir = fileInfo.isDir();
        if (isDir) {
            if (directories->contains(path))
                continue;
        } else {
            if (files->contains(path))
                continue;
        }

        const QString absolutePath = isDir ? fileInfo.absoluteFilePath() : fileInfo.absolutePath();
        const uint flags = isDir
            ? (FILE_NOTIFY_CHANGE_DIR_NAME
               | FILE_NOTIFY_CHANGE_FILE_NAME)
            : (FILE_NOTIFY_CHANGE_DIR_NAME
               | FILE_NOTIFY_CHANGE_FILE_NAME
               | FILE_NOTIFY_CHANGE_ATTRIBUTES
               | FILE_NOTIFY_CHANGE_SIZE
               | FILE_NOTIFY_CHANGE_LAST_WRITE
               | FILE_NOTIFY_CHANGE_SECURITY);

        Handle handle = handleForDir.value(absolutePath);
        if (handle.handle == INVALID_HANDLE_VALUE || handle.flags != flags) {
            // Volume and folder paths need a trailing slash for proper notification
            // (e.g. "c:" -> "c:/").
            const QString effectiveAbsolutePath =
                isDir ? (absolutePath + QLatin1Char('/')) : absolutePath;

            QT_WA({
                    handle.handle = FindFirstChangeNotificationW((TCHAR *) QDir::toNativeSeparators(effectiveAbsolutePath).utf16(),
                                                      false, flags);
            },{
                    handle.handle = FindFirstChangeNotificationA(QDir::toNativeSeparators(effectiveAbsolutePath).toLocal8Bit(),
                                                      false, flags);
            })
            handle.flags = flags;
            if (handle.handle == INVALID_HANDLE_VALUE)
                continue;
            // qDebug() << "Added handle" << handle << "for" << absolutePath << "to watch" << fileInfo.absoluteFilePath();
            handles.append(handle.handle);
            handleForDir.insert(absolutePath, handle);
        }

        PathInfo pathInfo;
        pathInfo.absolutePath = absolutePath;
        pathInfo.isDir = isDir;
        pathInfo.path = path;
        pathInfo = fileInfo;
        QHash<QString, PathInfo> &h = pathInfoForHandle[handle.handle];
        if (!h.contains(fileInfo.absoluteFilePath())) {
            pathInfoForHandle[handle.handle].insert(fileInfo.absoluteFilePath(), pathInfo);
            if (isDir)
                directories->append(path);
            else
                files->append(path);
        }

        it.remove();
    }

    if (!isRunning()) {
        msg = '@';
        start();
    } else {
        wakeup();
    }

    return p;
}

QStringList QWindowsFileSystemWatcherEngine::removePaths(const QStringList &paths,
                                                         QStringList *files,
                                                         QStringList *directories)
{
    QMutexLocker locker(&mutex);

    QStringList p = paths;
    QMutableListIterator<QString> it(p);
    while (it.hasNext()) {
        QString path = it.next();
        QString normalPath = path;
        if (normalPath.endsWith(QLatin1Char('/')) || normalPath.endsWith(QLatin1Char('\\')))
            normalPath.chop(1);
        QFileInfo fileInfo(normalPath.toLower());

        QString absolutePath = fileInfo.absoluteFilePath();
        Handle handle = handleForDir.value(absolutePath);
        if (handle.handle == INVALID_HANDLE_VALUE) {
            // perhaps path is a file?
            absolutePath = fileInfo.absolutePath();
            handle = handleForDir.value(absolutePath);
            if (handle.handle == INVALID_HANDLE_VALUE)
                continue;
        }

        QHash<QString, PathInfo> &h = pathInfoForHandle[handle.handle];
        if (h.remove(fileInfo.absoluteFilePath())) {
            // ###
            files->removeAll(path);
            directories->removeAll(path);

            if (h.isEmpty()) {
                // qDebug() << "Closing handle" << handle;
                FindCloseChangeNotification(handle.handle);    // This one might generate a notification

                int indexOfHandle = handles.indexOf(handle.handle);
                Q_ASSERT(indexOfHandle != -1);
                handles.remove(indexOfHandle);

                handleForDir.remove(absolutePath);
                // h is now invalid

                it.remove();
            }
        }
    }

    if (handleForDir.isEmpty()) {
        stop();
        locker.unlock();
        wait();
        locker.relock();
    } else {
        wakeup();
    }

    return p;
}

void QWindowsFileSystemWatcherEngine::stop()
{
    msg = 'q';
    SetEvent(handles.at(0));
}

void QWindowsFileSystemWatcherEngine::wakeup()
{
    msg = '@';
    SetEvent(handles.at(0));
}

QT_END_NAMESPACE
#endif // QT_NO_FILESYSTEMWATCHER
