/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

/* Sample program for configure to test Large File support on target
platforms.
*/

#define _LARGEFILE_SOURCE
#define _LARGE_FILES
#define _FILE_OFFSET_BITS 64
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>

int main( int, char **argv )
{
// check that off_t can hold 2^63 - 1 and perform basic operations...
#define OFF_T_64 (((off_t) 1 << 62) - 1 + ((off_t) 1 << 62))
    if (OFF_T_64 % 2147483647 != 1)
	return 1;

    // stat breaks on SCO OpenServer
    struct stat buf;
    stat( argv[0], &buf );
    if (!S_ISREG(buf.st_mode))
	return 2;

    FILE *file = fopen( argv[0], "r" );
    off_t offset = ftello( file );
    fseek( file, offset, SEEK_CUR );
    fclose( file );
    return 0;
}
