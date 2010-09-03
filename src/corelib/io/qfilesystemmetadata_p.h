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

#ifndef QFILESYSTEMMETADATA_P_H_INCLUDED
#define QFILESYSTEMMETADATA_P_H_INCLUDED

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

#include "qplatformdefs.h"
#include <QtCore/qglobal.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qabstractfileengine.h>

// Platform-specific includes
#if defined(Q_OS_WIN)
#else
#endif

QT_BEGIN_NAMESPACE

class QFileSystemEngine;

struct QFileSystemMetaData
{
    QFileSystemMetaData()
        : knownFlagsMask(0)
    {
    }

    enum MetaDataFlag {
        // Permissions, overlaps with QFile::Permissions
        OtherReadPermission = 0x00000004,   OtherWritePermission = 0x00000002,  OtherExecutePermission = 0x00000001,
        GroupReadPermission = 0x00000040,   GroupWritePermission = 0x00000020,  GroupExecutePermission = 0x00000010,
        UserReadPermission  = 0x00000400,   UserWritePermission  = 0x00000200,  UserExecutePermission  = 0x00000100,
        OwnerReadPermission = 0x00004000,   OwnerWritePermission = 0x00002000,  OwnerExecutePermission = 0x00001000,

        OtherPermissions    = OtherReadPermission | OtherWritePermission | OtherExecutePermission,
        GroupPermissions    = GroupReadPermission | GroupWritePermission | GroupExecutePermission,
        UserPermissions     = UserReadPermission  | UserWritePermission  | UserExecutePermission,
        OwnerPermissions    = OwnerReadPermission | OwnerWritePermission | OwnerExecutePermission,

        Permissions         = OtherPermissions | GroupPermissions | UserPermissions | OwnerPermissions,

        // Type
        LinkType            = 0x00010000,
        FileType            = 0x00020000,
        DirectoryType       = 0x00040000,
#if !defined(QWS) && defined(Q_OS_MAC)
        BundleType          = 0x00080000,
        AliasType           = 0x08000000,
#else
        BundleType          =        0x0,
        AliasType           =        0x0,
#endif
        SequentialType      = 0x00800000,   // Note: overlaps with QAbstractFileEngine::RootFlag

        Type                = LinkType | FileType | DirectoryType | BundleType | SequentialType | AliasType,

        // Attributes
        HiddenAttribute     = 0x00100000,
        SizeAttribute       = 0x00200000,   // Note: overlaps with QAbstractFileEngine::LocalDiskFlag
        ExistsAttribute     = 0x00400000,

        Attributes          = HiddenAttribute | SizeAttribute | ExistsAttribute,

        // Times
        CreationTime        = 0x01000000,   // Note: overlaps with QAbstractFileEngine::Refresh
        ModificationTime    = 0x02000000,
        AccessTime          = 0x04000000,

        Times               = CreationTime | ModificationTime | AccessTime,

        // Owner IDs
        UserId              = 0x10000000,
        GroupId             = 0x20000000,

        OwnerIds            = UserId | GroupId,

        PosixStatFlags      = QFileSystemMetaData::OtherPermissions
                            | QFileSystemMetaData::GroupPermissions
                            | QFileSystemMetaData::OwnerPermissions
                            | QFileSystemMetaData::FileType
                            | QFileSystemMetaData::DirectoryType
                            | QFileSystemMetaData::SequentialType
#if !defined(QWS) && defined(Q_OS_MAC) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
                            // Mac OS >= 10.5: st_flags & UF_HIDDEN
                            | QFileSystemMetaData::HiddenAttribute
#endif
                            | QFileSystemMetaData::SizeAttribute
                            | QFileSystemMetaData::Times
                            | QFileSystemMetaData::OwnerIds,

        AllMetaDataFlags    = 0xFFFFFFFF

    };
    Q_DECLARE_FLAGS(MetaDataFlags, MetaDataFlag)

    bool hasFlags(MetaDataFlags flags) const
    {
        return ((knownFlagsMask & flags) == flags);
    }

    MetaDataFlags missingFlags(MetaDataFlags flags)
    {
        return flags & ~knownFlagsMask;
    }

    void clear()
    {
        knownFlagsMask = 0;
    }

    void clearFlags(MetaDataFlags flags = AllMetaDataFlags)
    {
        knownFlagsMask &= ~flags;
    }

    bool exists() const                     { return (entryFlags & ExistsAttribute); }

    bool isLink() const                     { return (entryFlags & LinkType); }
    bool isFile() const                     { return (entryFlags & FileType); }
    bool isDirectory() const                { return (entryFlags & DirectoryType); }
#if !defined(QWS) && defined(Q_OS_MAC)
    bool isBundle() const                   { return (entryFlags & BundleType); }
    bool isAlias() const                    { return (entryFlags & AliasType); }
#else
    bool isBundle() const                   { return false; }
    bool isAlias() const                    { return false; }
#endif
    bool isSequential() const               { return (entryFlags & SequentialType); }
    bool isHidden() const                   { return (entryFlags & HiddenAttribute); }

    qint64 size() const                     { return size_; }

    QFile::Permissions permissions() const  { return QFile::Permissions(Permissions & entryFlags); }

#if defined(Q_OS_UNIX)
    QDateTime creationTime() const          { return QDateTime::fromTime_t(creationTime_); }
    QDateTime modificationTime() const      { return QDateTime::fromTime_t(modificationTime_); }
    QDateTime accessTime() const            { return QDateTime::fromTime_t(accessTime_); }

    QDateTime fileTime(QAbstractFileEngine::FileTime time) const
    {
        switch (time)
        {
        case QAbstractFileEngine::ModificationTime:
            return modificationTime();

        case QAbstractFileEngine::AccessTime:
            return accessTime();

        case QAbstractFileEngine::CreationTime:
            return creationTime();
        }

        return QDateTime();
    }

    uint userId() const                     { return userId_; }
    uint groupId() const                    { return groupId_; }

    uint ownerId(QAbstractFileEngine::FileOwner owner) const
    {
        if (owner == QAbstractFileEngine::OwnerUser)
            return userId();
        else
            return groupId();
    }

    void fillFromStatBuf(const QT_STATBUF &statBuffer);
#endif

private:
    friend class QFileSystemEngine;

    MetaDataFlags knownFlagsMask;
    MetaDataFlags entryFlags;

    qint64 size_;

    // Platform-specific data goes here:
#if defined(Q_OS_WIN)
#else
    time_t creationTime_;
    time_t modificationTime_;
    time_t accessTime_;

    uint userId_;
    uint groupId_;
#endif

};

Q_DECLARE_OPERATORS_FOR_FLAGS(QFileSystemMetaData::MetaDataFlags)

QT_END_NAMESPACE

#endif // include guard
