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
#include "globalinformation.h"

GlobalInformation::GlobalInformation() :
    _qtVersionStr( QT_VERSION_STR ),
    _reconfig( false )
{
#if defined(Q_OS_WIN32)
    _sysId = Other;
#elif defined(Q_OS_MACX)
    _sysId = MACX;
#else
    _sysId = MingW32;
#endif
}

GlobalInformation::~GlobalInformation()
{
}

void GlobalInformation::setReconfig( bool r )
{
    _reconfig = r;
}

bool GlobalInformation::reconfig() const
{
    return _reconfig;
}

void GlobalInformation::setQtVersionStr( const QString& qvs )
{
    _qtVersionStr = qvs;
}

QString GlobalInformation::qtVersionStr() const
{
    return _qtVersionStr;
}

#if defined(QSA)
void GlobalInformation::setQsaVersionStr( const QString& qvs )
{
    _qsaVersionStr = qvs;
}

QString GlobalInformation::qsaVersionStr() const
{
    return _qsaVersionStr;
}
#endif

void GlobalInformation::setSysId( SysId s )
{
    _sysId = s;
}

GlobalInformation::SysId GlobalInformation::sysId() const
{
    return _sysId;
}

QString GlobalInformation::text(Text t) const
{
    QString str;

    switch (_sysId) {
    case MSVC:
	if (t ==  IDE)
	    str = "Microsoft Visual Studio 6.0";
	else if (t == Mkspec)
	    str = "win32-msvc";
	else if (t == MakeTool)
	    str = "nmake.exe";
	break;
    case MSVCNET:
	if (t ==  IDE)
	    str = "Microsoft Visual Studio .NET";
	else if (t == Mkspec)
	    str = "win32-msvc.net";
	else if (t == MakeTool)
	    str = "nmake.exe";
	break;
    case Watcom:
	if (t == Mkspec)
	    str = "win32-watcom";
	else if (t == MakeTool)
	    str = "wmake.exe";
	break;
    case Intel:
	if (t == Mkspec)
	    str = "win32-icc";
	else if (t == MakeTool)
	    str = "nmake.exe";
	break;
    case GCC:
	if (t == Mkspec)
	    str = "win32-g++";
	else if (t == MakeTool)
	    str = "gmake.exe";
	break;
    case MACX:
	if (t == Mkspec)
	    str = "mac-g++";
	else if (t == MakeTool)
	    str = "make";
	break;
    case MinGW:
	if (t == Mkspec)
	    str = "win32-g++";
	else if (t == MakeTool)
	    str = "mingw32-make.exe";
	break;
    case Borland:
	if (t == Mkspec)
	    str = "win32-borland";
	else if (t == MakeTool)
	    str = "make.exe";
	break;
    default:
	if (t == Mkspec)
	    str = "Custom";
	else if (t == MakeTool)
	    str = "make.exe";
	break;
    }

    return str;
}
