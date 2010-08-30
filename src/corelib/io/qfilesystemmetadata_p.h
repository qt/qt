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

#include <QtCore/qglobal.h>

// Platform-specific includes
#if defined(Q_OS_WIN)
#else
#endif

QT_BEGIN_NAMESPACE

class QFileSystemEngine;

struct QFileSystemMetaData
{
    QFileSystemMetaData()
        : cachedFlags(0)
    {
    }

    enum MetaDataFlag {
        // Permissions, overlaps with QFile::Permissions
        OtherReadPermission = 0x00000004,   OtherWritePermission = 0x00000002,  OtherExecutePermission = 0x00000001,
        GroupReadPermission = 0x00000040,   GroupWritePermission = 0x00000020,  GroupExecutePermission = 0x00000010,
        UserReadPermission  = 0x00000400,   UserWritePermission  = 0x00000200,  UserExecutePermission  = 0x00000100,
        OwnerReadPermission = 0x00004000,   OwnerWritePermission = 0x00002000,  OwnerExecutePermission = 0x00001000,

        OtherPermissions    = 0x0000000F,
        GroupPermissions    = 0x000000F0,
        UserPermissions     = 0x00000F00,
        OwnerPermissions    = 0x0000F000,

        Permissions         = 0x0000FFFF,

        // Type
        LinkType            = 0x00010000,
        FileType            = 0x00020000,
        DirectoryType       = 0x00040000,
        BundleType          = 0x00080000,

        Type                = 0x000F0000,

        // Attributes
        HiddenAttribute     = 0x00100000,
        LocalDiskAttribute  = 0x00200000,
        ExistsAttribute     = 0x00400000,
        SizeAttribute       = 0x00800000,   // Note: overlaps with QAbstractFileEngine::RootFlag

        Attributes          = 0x00F00000,

        // Times
        CreationTime        = 0x01000000,   // Note: overlaps with QAbstractFileEngine::Refresh
        ModificationTime    = 0x02000000,
        AccessTime          = 0x04000000,

        Times               = 0x07000000
    };
    Q_DECLARE_FLAGS(MetaDataFlags, MetaDataFlag)

    bool hasFlags(MetaDataFlags flags) const
    {
        return ((cachedFlags & flags) == flags);
    }

    void clear()
    {
        cachedFlags = 0;
    }

private:
    friend class QFileSystemEngine;

    MetaDataFlags cachedFlags;

    // Platform-specific data goes here:
#if defined(Q_OS_WIN)
#else
#endif

};

QT_END_NAMESPACE

#endif // include guard
