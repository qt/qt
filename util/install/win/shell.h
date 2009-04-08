/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the utils of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef SHELL_H
#define SHELL_H

#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>
#if defined(Q_OS_WIN32)
#include <windows.h>
#include <shlobj.h>
#endif

class WinShell
{
public:
    WinShell();
    ~WinShell();

private:
    bool createDir( QString fullPath );
#if defined(Q_OS_WIN32)
    QString OLESTR2QString( LPOLESTR str );
#endif
public:
    QString localProgramsFolderName;
    QString commonProgramsFolderName;
    QString windowsFolderName;
    QString selectFolder( QString folderName, bool common );

    QString createFolder( QString folderName, bool common );

#if defined(Q_OS_WIN32)
    HRESULT createShortcut( QString folderName, bool common, QString shortcutName, QString target, QString description = QString(), QString arguments = QString(), QString wrkDir = QString() );
    void createInternetShortcut( QString folderName, bool common, QString shortcutName, QString url );
#endif

    static QPixmap* getOpenFolderImage();
    static QPixmap* getClosedFolderImage();
    static QPixmap* getFileImage();
    static QPixmap* getInfoImage();
#if defined(Q_OS_WIN32)
    static ULARGE_INTEGER dirFreeSpace( QString dirPath );
#elif defined(Q_OS_MAC)
    static long dirFreeSpace( QString dirPath );
#endif
};

#endif
