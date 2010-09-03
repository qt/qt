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

#ifndef QFILESYSTEMENGINE_P_H_INCLUDED
#define QFILESYSTEMENGINE_P_H_INCLUDED

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

#include "qfile.h"
#include "qfilesystementry_p.h"
#include "qfilesystemmetadata_p.h"

QT_BEGIN_NAMESPACE

class QFileSystemEngine
{
public:
    static bool isCaseSensitive();

    static QFileSystemEntry getLinkTarget(const QFileSystemEntry &link);
    static QFileSystemEntry canonicalName(const QFileSystemEntry &entry);
    static QFileSystemEntry absoluteName(const QFileSystemEntry &entry);

    static QString bundleName(const QFileSystemEntry &entry);

    static bool fillMetaData(const QFileSystemEntry &entry, QFileSystemMetaData &data,
            QFileSystemMetaData::MetaDataFlags what);
#if defined(Q_OS_UNIX)
    static bool fillMetaData(int fd, QFileSystemMetaData &data); // what = PosixStatFlags
#endif

    static bool createDirectory(const QFileSystemEntry &entry, bool createParents);
    static bool removeDirectory(const QFileSystemEntry &entry, bool removeEmptyParents);

    static bool createLink(const QFileSystemEntry &source, const QFileSystemEntry &target);

    static bool copyFile(const QFileSystemEntry &source, const QFileSystemEntry &target);
    static bool renameFile(const QFileSystemEntry &source, const QFileSystemEntry &target);
    static bool removeFile(const QFileSystemEntry &entry);

    static bool setPermissions(const QFileSystemEntry &entry, QFile::Permissions permissions,
            QFileSystemMetaData *data = 0);

    static QAbstractFileEngine *resolveEntryAndCreateLegacyEngine(QFileSystemEntry &entry,
            QFileSystemMetaData &data);
private:
    static QString slowCanonicalized(const QString &path);
};

QT_END_NAMESPACE

#endif // include guard
