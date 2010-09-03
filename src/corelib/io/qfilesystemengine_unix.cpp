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

#include "qplatformdefs.h"
#include "qfilesystemengine_p.h"
#include "qplatformdefs.h"
#include "qfsfileengine.h"

#include <stdlib.h> // for realpath()
#include <errno.h>

#if defined(Q_OS_SYMBIAN)
# include <sys/syslimits.h>
# include <f32file.h>
# include <pathinfo.h>
# include <QtCore/private/qcore_symbian_p.h>
#endif

#if defined(Q_OS_MAC)
# include <QtCore/private/qcore_mac_p.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_OS_SYMBIAN)
static bool _q_isSymbianHidden(const QFileSystemEntry &entry, bool isDir)
{
    RFs rfs = qt_s60GetRFs();

    QFileSystemEntry absoluteEntry = QFileSystemEngine::absoluteName(entry);
    QString absolutePath = absoluteEntry.filePath();

    if (isDir && !absolutePath.endsWith(QLatin1Char('/')))
        absolutePath.append(QLatin1Char('/'));

    TPtrC ptr(qt_QString2TPtrC(absolutePath));
    TUint attributes;
    TInt err = rfs.Att(ptr, attributes);
    return (err == KErrNone && (attributes & KEntryAttHidden));
}
#endif

#if !defined(QWS) && defined(Q_OS_MAC)
static inline bool _q_isMacHidden(const char *nativePath)
{
    OSErr err;

    FSRef fsRef;
    err = FSPathMakeRefWithOptions(reinterpret_cast<const UInt8 *>(nativePath),
            kFSPathMakeRefDoNotFollowLeafSymlink, &fsRef, 0);
    if (err != noErr)
        return false;

    FSCatalogInfo catInfo;
    err = FSGetCatalogInfo(&fsRef, kFSCatInfoFinderInfo, &catInfo, NULL, NULL, NULL);
    if (err != noErr)
        return false;

    FileInfo * const fileInfo = reinterpret_cast<FileInfo*>(&catInfo.finderInfo);
    return (fileInfo->finderFlags & kIsInvisible);
}
#else
static inline bool _q_isMacHidden(const char *nativePath)
{
    // no-op
    return false;
}
#endif

void QFileSystemMetaData::fillFromStatBuf(const QT_STATBUF &statBuffer)
{
    // Permissions
    if (statBuffer.st_mode & S_IRUSR)
        entryFlags |= QFileSystemMetaData::OwnerReadPermission;
    if (statBuffer.st_mode & S_IWUSR)
        entryFlags |= QFileSystemMetaData::OwnerWritePermission;
    if (statBuffer.st_mode & S_IXUSR)
        entryFlags |= QFileSystemMetaData::OwnerExecutePermission;

    if (statBuffer.st_mode & S_IRGRP)
        entryFlags |= QFileSystemMetaData::GroupReadPermission;
    if (statBuffer.st_mode & S_IWGRP)
        entryFlags |= QFileSystemMetaData::GroupWritePermission;
    if (statBuffer.st_mode & S_IXGRP)
        entryFlags |= QFileSystemMetaData::GroupExecutePermission;

    if (statBuffer.st_mode & S_IROTH)
        entryFlags |= QFileSystemMetaData::OtherReadPermission;
    if (statBuffer.st_mode & S_IWOTH)
        entryFlags |= QFileSystemMetaData::OtherWritePermission;
    if (statBuffer.st_mode & S_IXOTH)
        entryFlags |= QFileSystemMetaData::OtherExecutePermission;

    // Type
    if ((statBuffer.st_mode & S_IFMT) == S_IFREG)
        entryFlags |= QFileSystemMetaData::FileType;
    else if ((statBuffer.st_mode & S_IFMT) == S_IFDIR)
        entryFlags |= QFileSystemMetaData::DirectoryType;
    else
        entryFlags |= QFileSystemMetaData::SequentialType;

    // Attributes
    entryFlags |= QFileSystemMetaData::ExistsAttribute;
    size_ = statBuffer.st_size;
#if !defined(QWS) && defined(Q_OS_MAC) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    if (statBuffer.st_flags & UF_HIDDEN) {
        entryFlags |= QFileSystemMetaData::HiddenAttribute;
        knownFlagsMask |= QFileSystemMetaData::HiddenAttribute;
    }
#endif

    // Times
    creationTime_ = statBuffer.st_ctime ? statBuffer.st_ctime : statBuffer.st_mtime;
    modificationTime_ = statBuffer.st_mtime;
    accessTime_ = statBuffer.st_atime;
}

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
        return QFileSystemEntry(result, QFileSystemEntry::FromNativePath());
    const bool isDir = result.endsWith('/');

    /* as long as QDir::cleanPath() operates on a QString we have to convert to a string here.
     * ideally we never convert to a string since that loses information. Please fix after
     * we get a QByteArray version of QDir::cleanPath()
     */
    QFileSystemEntry resultingEntry(result, QFileSystemEntry::FromNativePath());
    QString stringVersion = QDir::cleanPath(resultingEntry.filePath());
    if (isDir)
        stringVersion.append(QLatin1Char('/'));
    return QFileSystemEntry(stringVersion);
}

//static
QString QFileSystemEngine::bundleName(const QFileSystemEntry &entry)
{
#if !defined(QWS) && defined(Q_OS_MAC)
    QCFType<CFURLRef> url = CFURLCreateWithFileSystemPath(0, QCFString(entry.filePath()),
            kCFURLPOSIXPathStyle, true);
    if (QCFType<CFDictionaryRef> dict = CFBundleCopyInfoDictionaryForURL(url)) {
        if (CFTypeRef name = (CFTypeRef)CFDictionaryGetValue(dict, kCFBundleNameKey)) {
            if (CFGetTypeID(name) == CFStringGetTypeID())
                return QCFString::toQString((CFStringRef)name);
        }
    }
#endif
    return QString();
}

//static
bool QFileSystemEngine::fillMetaData(int fd, QFileSystemMetaData &data)
{
    data.entryFlags &= ~QFileSystemMetaData::PosixStatFlags;
    data.knownFlagsMask |= QFileSystemMetaData::PosixStatFlags;

    QT_STATBUF statBuffer;
    if (QT_FSTAT(fd, &statBuffer) == 0) {
        data.fillFromStatBuf(statBuffer);
        return true;
    }

    return false;
}

//static
bool QFileSystemEngine::fillMetaData(const QFileSystemEntry &entry, QFileSystemMetaData &data,
        QFileSystemMetaData::MetaDataFlags what)
{
#if !defined(QWS) && defined(Q_OS_MAC)
    if (what & QFileSystemMetaData::BundleType) {
        if (!data.hasFlags(QFileSystemMetaData::DirectoryType))
            what |= QFileSystemMetaData::DirectoryType;
    }
#endif

#if defined(Q_OS_SYMBIAN)
    if (what & QFileSystemMetaData::HiddenAttribute) {
        if (!data.hasFlags(QFileSystemMetaData::LinkType | QFileSystemMetaData::DirectoryType))
            what |= QFileSystemMetaData::LinkType | QFileSystemMetaData::DirectoryType;
    }
#endif

    if (what & QFileSystemMetaData::PosixStatFlags)
        what |= QFileSystemMetaData::PosixStatFlags;

    if (what & QFileSystemMetaData::ExistsAttribute) {
        //  FIXME:  Would other queries being performed provide this bit?
        what |= QFileSystemMetaData::PosixStatFlags;
    }

    data.entryFlags &= ~what;

    const char * nativeFilePath;
    int nativeFilePathLength;
    {
        const QByteArray &path = entry.nativeFilePath();
        nativeFilePath = path.constData();
        nativeFilePathLength = path.size();
    }

    bool entryExists = true; // innocent until proven otherwise

    QT_STATBUF statBuffer;
    bool statBufferValid = false;
    if (what & QFileSystemMetaData::LinkType) {
        if (QT_LSTAT(nativeFilePath, &statBuffer) == 0) {
            if (S_ISLNK(statBuffer.st_mode)) {
                data.entryFlags |= QFileSystemMetaData::LinkType;
            } else {
                statBufferValid = true;
                data.entryFlags &= ~QFileSystemMetaData::PosixStatFlags;
            }
        } else {
            entryExists = false;
        }

        data.knownFlagsMask |= QFileSystemMetaData::LinkType;
    }

    if (statBufferValid || (what & QFileSystemMetaData::PosixStatFlags)) {
        if (entryExists && !statBufferValid)
            statBufferValid = (QT_STAT(nativeFilePath, &statBuffer) == 0);

        if (statBufferValid)
            data.fillFromStatBuf(statBuffer);
        else
            entryExists = false;

        // reset the mask
        data.knownFlagsMask |= QFileSystemMetaData::PosixStatFlags
            | QFileSystemMetaData::ExistsAttribute;
    }

#if !defined(QWS) && defined(Q_OS_MAC)
    if (what & QFileSystemMetaData::AliasType)
    {
        if (entryExists) {
            FSRef fref;
            if (FSPathMakeRef((const UInt8 *)nativeFilePath, &fref, NULL) == noErr) {
                Boolean isAlias, isFolder;
                if (FSIsAliasFile(&fref, &isAlias, &isFolder) == noErr) {
                    if (isAlias)
                        data.entryFlags |= QFileSystemMetaData::AliasType;
                }
            }
        }
        data.knownFlagsMask |= QFileSystemMetaData::AliasType;
    }
#endif

    if (what & QFileSystemMetaData::UserPermissions) {
        // calculate user permissions

        if (entryExists) {
            if (what & QFileSystemMetaData::UserReadPermission) {
                if (QT_ACCESS(nativeFilePath, R_OK) == 0)
                    data.entryFlags |= QFileSystemMetaData::UserReadPermission;
            }
            if (what & QFileSystemMetaData::UserWritePermission) {
                if (QT_ACCESS(nativeFilePath, W_OK) == 0)
                    data.entryFlags |= QFileSystemMetaData::UserWritePermission;
            }
            if (what & QFileSystemMetaData::UserExecutePermission) {
                if (QT_ACCESS(nativeFilePath, X_OK) == 0)
                    data.entryFlags |= QFileSystemMetaData::UserExecutePermission;
            }
        }
        data.knownFlagsMask |= (what & QFileSystemMetaData::UserPermissions);
    }

    if (what & QFileSystemMetaData::HiddenAttribute
            && !data.isHidden()) {
#if defined(Q_OS_SYMBIAN)
        // In Symbian, all symlinks have hidden attribute for some reason;
        // lets make them visible for better compatibility with other platforms.
        // If somebody actually wants a hidden link, then they are out of luck.
        if (entryExists && !data.isLink() && _q_isSymbianHidden(entry, data.isDirectory()))
            data.entryFlags |= QFileSystemMetaData::HiddenAttribute;
#else
        QString fileName = entry.fileName();
        if ((fileName.size() > 0 && fileName.at(0) == QLatin1Char('.'))
                || (entryExists && _q_isMacHidden(nativeFilePath)))
            data.entryFlags |= QFileSystemMetaData::HiddenAttribute;
#endif
        data.knownFlagsMask |= QFileSystemMetaData::HiddenAttribute;
    }

#if !defined(QWS) && defined(Q_OS_MAC)
    if (what & QFileSystemMetaData::BundleType) {
        if (entryExists && data.isDirectory()) {
            QCFType<CFStringRef> path = CFStringCreateWithBytes(0,
                    (const UInt8*)nativeFilePath, nativeFilePathLength,
                    kCFStringEncodingUTF8, false);
            QCFType<CFURLRef> url = CFURLCreateWithFileSystemPath(0, path,
                    kCFURLPOSIXPathStyle, true);

            UInt32 type, creator;
            if (CFBundleGetPackageInfoInDirectory(url, &type, &creator))
                data.entryFlags |= QFileSystemMetaData::BundleType;
        }

        data.knownFlagsMask |= QFileSystemMetaData::BundleType;
    }
#endif

    return data.hasFlags(what);
}

//static
bool QFileSystemEngine::createDirectory(const QFileSystemEntry &entry, bool createParents)
{
    QString dirName = entry.filePath();
    if (createParents) {
        dirName = QDir::cleanPath(dirName);
#if defined(Q_OS_SYMBIAN)
        dirName = QDir::toNativeSeparators(dirName);
#endif
        for (int oldslash = -1, slash=0; slash != -1; oldslash = slash) {
            slash = dirName.indexOf(QDir::separator(), oldslash+1);
            if (slash == -1) {
                if (oldslash == dirName.length())
                    break;
                slash = dirName.length();
            }
            if (slash) {
                QByteArray chunk = QFile::encodeName(dirName.left(slash));
                QT_STATBUF st;
                if (QT_STAT(chunk, &st) != -1) {
                    if ((st.st_mode & S_IFMT) != S_IFDIR)
                        return false;
                } else if (QT_MKDIR(chunk, 0777) != 0) {
                    return false;
                }
            }
        }
        return true;
    }
#if defined(Q_OS_DARWIN)  // Mac X doesn't support trailing /'s
    if (dirName.endsWith(QLatin1Char('/')))
        dirName.chop(1);
#endif
    return (QT_MKDIR(QFile::encodeName(dirName), 0777) == 0);
}

//static
bool QFileSystemEngine::removeDirectory(const QFileSystemEntry &entry, bool removeEmptyParents)
{
    if (removeEmptyParents) {
        QString dirName = QDir::cleanPath(entry.filePath());
#if defined(Q_OS_SYMBIAN)
        dirName = QDir::toNativeSeparators(dirName);
#endif
        for (int oldslash = 0, slash=dirName.length(); slash > 0; oldslash = slash) {
            QByteArray chunk = QFile::encodeName(dirName.left(slash));
            QT_STATBUF st;
            if (QT_STAT(chunk, &st) != -1) {
                if ((st.st_mode & S_IFMT) != S_IFDIR)
                    return false;
                if (::rmdir(chunk) != 0)
                    return oldslash != 0;
            } else {
                return false;
            }
            slash = dirName.lastIndexOf(QDir::separator(), oldslash-1);
        }
        return true;
    }
    return rmdir(QFile::encodeName(entry.filePath())) == 0;
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
