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
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <qstring.h>

class QEnvironment
{
public:
    static QString getEnv( const QString &varName, int envBlock = LocalEnv );
    static void putEnv( const QString &varName, const QString &varValue, int envBlock = LocalEnv );
    static void removeEnv( const QString &varName, int envBlock = LocalEnv );
#if defined(Q_OS_WIN32)
    static QString getRegistryString( const QString &keyName, const QString &valueName, int scope = CurrentUser );
    static void recordUninstall( const QString &displayName, const QString &cmdString );
    static void removeUninstall( const QString &displayName );
#endif
    static QString getTempPath();
    static QString getLastError();
    static QString getFSFileName( const QString& fileName );

    enum {
	LocalEnv = 1,
	PersistentEnv = 2,
	GlobalEnv = 4
    };

    enum {
	CurrentUser = 0,
	LocalMachine = 1
    };
};

#endif
