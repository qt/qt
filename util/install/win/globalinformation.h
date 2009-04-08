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
#ifndef GLOBALINFORMATION_H
#define GLOBALINFORMATION_H
#include <qstring.h>

class GlobalInformation
{
public:
    GlobalInformation();
    ~GlobalInformation();

    void setReconfig( bool );
    bool reconfig() const;
    void setQtVersionStr( const QString& );
    QString qtVersionStr() const;
#if defined(QSA)
    void setQsaVersionStr( const QString& );
    QString qsaVersionStr() const;
#endif

    enum SysId {
	MSVCNET = 0,
	MSVC	= 1,
	Borland	= 2,
	MinGW   = 3,
	Other   = 4,
	Watcom  = 5,
	Intel	= 6,
	GCC	= 7,
	MACX	= 8
    };
    void setSysId( SysId );
    SysId sysId() const;

    enum Text {
	MakeTool,
	IDE,
	Mkspec
    };

    QString text(Text t) const;

private:
    bool _reconfig;
    QString _qtVersionStr;
#if defined(QSA)
    QString _qsaVersionStr;
#endif
    SysId _sysId;
};

extern GlobalInformation globalInformation;

#endif // GLOBALINFORMATION_H
