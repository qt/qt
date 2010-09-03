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

#include "qfilesystemengine_p.h"
#include <QtCore/qdir.h>
#include <QtCore/qset.h>
#include <QtCore/private/qabstractfileengine_p.h>
#ifdef QT_BUILD_CORE_LIB
#include <QtCore/private/qresource_p.h>
#endif

QT_BEGIN_NAMESPACE

/*!
    \internal

    Returns the canonicalized form of \a path (i.e., with all symlinks
    resolved, and all redundant path elements removed.
*/
QString QFileSystemEngine::slowCanonicalized(const QString &path)
{
    if (path.isEmpty())
        return path;

    QFileInfo fi;
    const QChar slash(QLatin1Char('/'));
    QString tmpPath = path;
    int separatorPos = 0;
    QSet<QString> nonSymlinks;
    QSet<QString> known;

    known.insert(path);
    do {
#ifdef Q_OS_WIN
        if (separatorPos == 0) {
            if (tmpPath.size() >= 2 && tmpPath.at(0) == slash && tmpPath.at(1) == slash) {
                // UNC, skip past the first two elements
                separatorPos = tmpPath.indexOf(slash, 2);
            } else if (tmpPath.size() >= 3 && tmpPath.at(1) == QLatin1Char(':') && tmpPath.at(2) == slash) {
                // volume root, skip since it can not be a symlink
                separatorPos = 2;
            }
        }
        if (separatorPos != -1)
#endif
        separatorPos = tmpPath.indexOf(slash, separatorPos + 1);
        QString prefix = separatorPos == -1 ? tmpPath : tmpPath.left(separatorPos);
        if (
#ifdef Q_OS_SYMBIAN
            // Symbian doesn't support directory symlinks, so do not check for link unless we
            // are handling the last path element. This not only slightly improves performance,
            // but also saves us from lot of unnecessary platform security check failures
            // when dealing with files under *:/private directories.
            separatorPos == -1 &&
#endif
            !nonSymlinks.contains(prefix)) {
            fi.setFile(prefix);
            if (fi.isSymLink()) {
                QString target = fi.symLinkTarget();
                if(QFileInfo(target).isRelative())
                    target = fi.absolutePath() + slash + target;
                if (separatorPos != -1) {
                    if (fi.isDir() && !target.endsWith(slash))
                        target.append(slash);
                    target.append(tmpPath.mid(separatorPos));
                }
                tmpPath = QDir::cleanPath(target);
                separatorPos = 0;

                if (known.contains(tmpPath))
                    return QString();
                known.insert(tmpPath);
            } else {
                nonSymlinks.insert(prefix);
            }
        }
    } while (separatorPos != -1);

    return QDir::cleanPath(tmpPath);
}

static bool _q_resolveEntryAndCreateLegacyEngine_recursive(QFileSystemEntry &entry, QFileSystemMetaData &data,
        QAbstractFileEngine *&engine)
{
    QString const &filePath = entry.filePath();
    if ((engine = qt_custom_file_engine_handler_create(filePath)))
        return true;

#if defined(QT_BUILD_CORE_LIB)
    for (int prefixSeparator = 0; prefixSeparator < filePath.size(); ++prefixSeparator) {
        QChar const ch = filePath[prefixSeparator];
        if (ch == QLatin1Char('/'))
            break;

        if (ch == QLatin1Char(':')) {
            if (prefixSeparator == 0) {
                engine = new QResourceFileEngine(filePath);
                return true;
            }

            if (prefixSeparator == 1)
                break;

            const QStringList &paths = QDir::searchPaths(filePath.left(prefixSeparator));
            for (int i = 0; i < paths.count(); i++) {
                entry = QFileSystemEntry(paths.at(i) % QLatin1Char('/') % filePath.mid(prefixSeparator + 1));
                // Recurse!
                if (_q_resolveEntryAndCreateLegacyEngine_recursive(entry, data, engine)) {
                    // FIXME: This will over-stat if we recurse
                    if (engine) {
                        if (engine->fileFlags(QAbstractFileEngine::FlagsMask) & QAbstractFileEngine::ExistsFlag)
                            return true;
                        delete engine;
                        engine = 0;
                    } else if (QFileSystemEngine::fillMetaData(entry, data, QFileSystemMetaData::ExistsAttribute)
                            && data.exists()) {
                        return true;
                    }
                }
            }

            // entry may have been clobbered at this point.
            return false;
        }

        //  There's no need to fully validate the prefix here. Consulting the
        //  unicode tables could be expensive and validation is already
        //  performed in QDir::setSearchPaths.
        //
        //  if (!ch.isLetterOrNumber())
        //      break;
    }
#endif // defined(QT_BUILD_CORE_LIB)

    return true;
}

/*!
    \internal

    Resolves the \a entry (see QDir::searchPaths) and returns an engine for
    it, but never a QFSFileEngine.

    \returns a file engine that can be used to access the entry. Returns 0 if
    QFileSystemEngine API should be used to query and interact with the file
    system object.
*/
QAbstractFileEngine *QFileSystemEngine::resolveEntryAndCreateLegacyEngine(
        QFileSystemEntry &entry, QFileSystemMetaData &data) {
    QFileSystemEntry copy = entry;
    QAbstractFileEngine *engine = 0;

    if (_q_resolveEntryAndCreateLegacyEngine_recursive(copy, data, engine))
        // Reset entry to resolved copy.
        entry = copy;
    else
        data.clear();

    return engine;
}

QT_END_NAMESPACE
