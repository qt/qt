/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <string.h>
#include <stdio.h>

#if defined(__WIN32__) || defined(__WIN64__)
#  include <fcntl.h>
#  include <io.h>
#  if defined(__BORLANDC__) || defined(__TURBOC__)
#    define _setmode(a,b) setmode(a,b)
#  endif
#endif

int main( int argc, char ** argv )
{
#if defined(__WIN32__) || defined(__WIN64__)
    _setmode( _fileno(stdin), _O_BINARY );
    _setmode( _fileno(stdout), _O_BINARY );
    _setmode( _fileno(stderr), _O_BINARY );
#endif

    int useStdout = 1;
    int useStderr = 0;
    for ( int i=1; i<argc; i++ ) {
	if ( strcmp( argv[i], "-stdout_and_stderr" ) == 0 ) {
	    useStdout = 1;
	    useStderr = 1;
	} else if ( strcmp( argv[i], "-stderr" ) == 0 ) {
	    useStdout = 0;
	    useStderr = 1;
	}
    }

    int ch;
    while( (ch = fgetc(stdin)) != -1 ) {
	if ( useStdout ) {
	    printf("%c", (char) ch);
	    fflush(stdout);
	    // if both, useStdout and useStderr are used, we want to write
	    // different characters
	    ch++;
	}
	if ( useStderr ) {
	    fprintf(stderr, "%c", (char) ch);
            fflush(stderr);
	}
    }
    return ch;
}
