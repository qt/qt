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

#ifndef QFILESYSTEMENTRY_P_H_INCLUDED
#define QFILESYSTEMENTRY_P_H_INCLUDED

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

#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>

QT_BEGIN_NAMESPACE

class QFileSystemEntry
{
    QFileSystemEntry(const QString &filePath);
    QFileSystemEntry(const QByteArray &nativeFilePath);
    QFileSystemEntry(const QByteArray &nativeFilePath, const QString &filePath);

    QString filePath() const;
    QString fileName() const;
    QByteArray nativeFileName() const;
    QString suffix() const;
    QString completeSuffix() const;
    bool isAbsolute() const;
    bool isRelative() const {
        return !isAbsolute();
    }

private:
    // creates the QString version out of the bytearray version
    void resolveFilePath() const;
    // creates the bytearray version out of the QString version
    void resolveNativeFilePath() const;
    // resolves the separator
    void findLastSeparator() const;
    /// resolves the dots and the separator
    void findFileNameSeparators() const;

    mutable QString m_filePath; // always has slashes as separator
    mutable QByteArray m_nativeFilePath; // native encoding and separators

    mutable int m_lastSeparator : 16; // index in m_filePath of last separator
    mutable int m_firstDotInFileName : 11; // index after m_filePath for first dot (.)
    mutable int m_lastDotInFileName : 5; // index after m_firstDotInFileName for last dot (.)
};

QT_END_NAMESPACE

#endif // include guard
