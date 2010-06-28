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

#ifndef QSIMD_P_H
#define QSIMD_P_H

#include <qglobal.h>


QT_BEGIN_HEADER


#if defined(QT_NO_MAC_XARCH) || (defined(Q_OS_DARWIN) && (defined(__ppc__) || defined(__ppc64__)))
// Disable MMX and SSE on Mac/PPC builds, or if the compiler
// does not support -Xarch argument passing
#undef QT_HAVE_SSE2
#undef QT_HAVE_SSE
#undef QT_HAVE_3DNOW
#undef QT_HAVE_MMX
#endif

// SSE intrinsics
#if defined(QT_HAVE_SSE2) && !defined(QT_BOOTSTRAPPED) && (defined(__SSE2__) \
    || (defined(Q_CC_MSVC) && (defined(_M_X64) || _M_IX86_FP == 2)))
#if defined(QT_LINUXBASE)
/// this is an evil hack - the posix_memalign declaration in LSB
/// is wrong - see http://bugs.linuxbase.org/show_bug.cgi?id=2431
#  define posix_memalign _lsb_hack_posix_memalign
#  include <emmintrin.h>
#  undef posix_memalign
#else
#  ifdef Q_CC_MINGW
#    include <windows.h>
#  endif
#  include <emmintrin.h>
#endif

#define QT_ALWAYS_HAVE_SSE2
#endif

// NEON intrinsics
#if defined(QT_HAVE_NEON)
#include <arm_neon.h>
#endif


// IWMMXT intrinsics
#if defined(QT_HAVE_IWMMXT)
#include <mmintrin.h>
#if defined(Q_OS_WINCE)
#  include "qplatformdefs.h"
#endif
#endif

#if defined(QT_HAVE_IWMMXT)
#if !defined(__IWMMXT__) && !defined(Q_OS_WINCE)
#  include <xmmintrin.h>
#elif defined(Q_OS_WINCE_STD) && defined(_X86_)
#  pragma warning(disable: 4391)
#  include <xmmintrin.h>
#endif
#endif

// 3D now intrinsics
#if defined(QT_HAVE_3DNOW)
#include <mm3dnow.h>
#endif

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

enum CPUFeatures {
    None        = 0,
    MMX         = 0x1,
    MMXEXT      = 0x2,
    MMX3DNOW    = 0x4,
    MMX3DNOWEXT = 0x8,
    SSE         = 0x10,
    SSE2        = 0x20,
    CMOV        = 0x40,
    IWMMXT      = 0x80,
    NEON        = 0x100
};

Q_CORE_EXPORT uint qDetectCPUFeatures();

Q_CORE_EXPORT uint qDetectCPUFeatures();

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIMD_P_H
