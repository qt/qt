/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef FIXED_STDLIB_H
#define FIXED_STDLIB_H

// This hack fixes defect in Symbian stdlib.h. The original file
// does not work correctly when intermixing C and C++ (STL). Remove the hack
// when Open C / C++ team has fixed the defect.

// If _WCHAR_T_DECLARED is defined, undef it and store information that we
// need to revert the _WCHAR_T_DECLARED define after include
#   ifdef _WCHAR_T_DECLARED
#       define REVERT_WCHAR_T_DECLARED
#       undef _WCHAR_T_DECLARED
#   endif //_WCHAR_T_DECLARED

#include <stdlib.h>

// Revert _WCHAR_T_DECLARED if necessary
#   ifdef REVERT_WCHAR_T_DECLARED
#       define _WCHAR_T_DECLARED
#       undef REVERT_WCHAR_T_DECLARED
#   endif //REVERT_WCHAR_T_DECLARED

#endif