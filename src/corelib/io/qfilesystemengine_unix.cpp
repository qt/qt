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
#include "qfsfileengine.h"

#include <stdlib.h> // for realpath()
#include <errno.h>

#if defined(Q_OS_MAC)
# include <private/qcore_mac_p.h>
#endif

QT_BEGIN_NAMESPACE

bool QFileSystemEngine::isCaseSensitive()
{
#if defined(Q_OS_SYMBIAN)
    return false;
#else
    return true;
#endif
}

//static
QFileSystemEntry QFileSystemEngine::getLinkTarget(const QFileSystemEntry &link)
{
    return link; // TODO implement
}

//static
QFileSystemEntry QFileSystemEngine::canonicalName(const QFileSystemEntry &entry)
{
    if (entry.isEmpty() || entry.isRoot())
        return entry;

#ifdef __UCLIBC__
    return QFileSystemEntry::slowCanonicalName(entry);
#else
    char *ret = 0;
# if defined(Q_OS_MAC)
    // Mac OS X 10.5.x doesn't support the realpath(X,0) extension we use here.
    if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_6) {
        ret = realpath(entry.nativeFilePath().constData(), (char*)0);
    } else {
        // on 10.5 we can use FSRef to resolve the file path.
        QString path = QDir::cleanPath(entry.filePath());
        FSRef fsref;
        if (FSPathMakeRef((const UInt8 *)path.toUtf8().data(), &fsref, 0) == noErr) {
            CFURLRef urlref = CFURLCreateFromFSRef(NULL, &fsref);
            CFStringRef canonicalPath = CFURLCopyFileSystemPath(urlref, kCFURLPOSIXPathStyle);
            QString ret = QCFString::toQString(canonicalPath);
            CFRelease(canonicalPath);
            CFRelease(urlref);
            return QFileSystemEntry(ret);
        }
    }
# else
    ret = realpath(entry.nativeFilePath().constData(), (char*)0);
# endif
    if (ret) {
        QString canonicalPath = QDir::cleanPath(QString::fromLocal8Bit(ret));
        free(ret);
        return QFileSystemEntry(canonicalPath);
    } else if (errno == ENOENT) { // file doesn't exist
        return QFileSystemEntry();
    }
    return entry;
#endif
}

//static
QFileSystemEntry QFileSystemEngine::absoluteName(const QFileSystemEntry &entry)
{
    if (entry.isAbsolute())
        return entry;

    QByteArray orig = entry.nativeFilePath();
    QByteArray result;
    if (orig.isEmpty() || !orig.startsWith('/')) {
        QFileSystemEntry cur(QFSFileEngine::currentPath());
        result = cur.nativeFilePath();
    }
    if (!orig.isEmpty() && !(orig.length() == 1 && orig[0] == '.')) {
        if (!result.isEmpty() && !result.endsWith('/'))
            result.append('/');
        result.append(orig);
    }

    if (result.length() == 1 && result[0] == '/')
        return QFileSystemEntry(result);
    const bool isDir = result.endsWith('/');

    /* as long as QDir::cleanPath() operates on a QString we have to convert to a string here.
     * ideally we never convert to a string since that loses information. Please fix after
     * we get a QByteArray version of QDir::cleanPath()
     */
    QFileSystemEntry resultingEntry(result);
    QString stringVersion = QDir::cleanPath(resultingEntry.filePath());
    if (isDir)
        stringVersion.append(QLatin1Char('/'));
    return QFileSystemEntry(stringVersion);
}

//static
QString QFileSystemEngine::bundleName(const QFileSystemEntry &entry)
{
    return QString(); // TODO implement;
}

//static
bool QFileSystemEngine::fillMetaData(const QFileSystemEntry &entry, QFileSystemMetaData &data, QFileSystemMetaData::FileFlags what)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::createDirectory(const QFileSystemEntry &entry, bool createParents)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::removeDirectory(const QFileSystemEntry &entry, bool removeEmptyParents)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::createLink(const QFileSystemEntry &source, const QFileSystemEntry &target)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::copyFile(const QFileSystemEntry &source, const QFileSystemEntry &target)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::renameFile(const QFileSystemEntry &source, const QFileSystemEntry &target)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::removeFile(const QFileSystemEntry &entry)
{
    return false; // TODO implement;
}

//static
bool QFileSystemEngine::setPermissions(const QFileSystemEntry &entry, QFile::Permissions permissions, QFileSystemMetaData *data)
{
    return false; // TODO implement;
}

QT_END_NAMESPACE
