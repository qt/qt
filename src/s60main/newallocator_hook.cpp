/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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
#include <e32std.h>
#include <qglobal.h>

#ifdef QT_EXPORTS_NOT_FROZEN
// If exports in Qt DLLs are not frozen in this build, then we have to pick up the
// allocator creation function by import link. We know the function will be present
// in the DLLs we test with, as we have to use the DLLs we have built.

struct SStdEpocThreadCreateInfo;

Q_CORE_EXPORT TInt qt_symbian_SetupThreadHeap(TBool aNotFirst, SStdEpocThreadCreateInfo& aInfo);


/* \internal
 *
 * Uses link-time symbol preemption to capture a call from the application
 * startup. On return, there is some kind of heap allocator installed on the
 * thread.
*/
TInt UserHeap::SetupThreadHeap(TBool aNotFirst, SStdEpocThreadCreateInfo& aInfo)
{
    return qt_symbian_SetupThreadHeap(aNotFirst, aInfo);
}

#else // QT_EXPORTS_NOT_FROZEN
// If we are using an export frozen build, it should be compatible with all 4.7.x Qt releases.
// We want to use the allocator creation function introduced in qtcore.dll after 4.7.1. But we
// can't import link to it, as it may not be present in whatever 4.7.x DLLs we are running with.
// So the function is found and called dynamically, by library lookup. If it is not found, we
// use the OS allocator creation functions instead.

#if defined(QT_LIBINFIX)
#  define QT_LSTRING2(x) L##x
#  define QT_LSTRING(x) QT_LSTRING2(x)
#  define QT_LIBINFIX_UNICODE QT_LSTRING(QT_LIBINFIX)
#else
#  define QT_LIBINFIX_UNICODE L""
#endif

_LIT(QtCoreLibName, "qtcore" QT_LIBINFIX_UNICODE L".dll");

struct SThreadCreateInfo
    {
    TAny* iHandle;
    TInt iType;
    TThreadFunction iFunction;
    TAny* iPtr;
    TAny* iSupervisorStack;
    TInt iSupervisorStackSize;
    TAny* iUserStack;
    TInt iUserStackSize;
    TInt iInitialThreadPriority;
    TPtrC iName;
    TInt iTotalSize;    // Size including any extras (must be a multiple of 8 bytes)
    };

struct SStdEpocThreadCreateInfo : public SThreadCreateInfo
    {
    RAllocator* iAllocator;
    TInt iHeapInitialSize;
    TInt iHeapMaxSize;
    TInt iPadding;      // Make structure size a multiple of 8 bytes
    };


/* \internal
 *
 * Uses link-time symbol preemption to capture a call from the application
 * startup. On return, there is some kind of heap allocator installed on the
 * thread.
*/
TInt UserHeap::SetupThreadHeap(TBool aNotFirst, SStdEpocThreadCreateInfo& aInfo)
{
    TInt r = KErrNone;

#ifndef __WINS__
    // attempt to create the fast allocator through a known export ordinal in qtcore.dll
    RLibrary qtcore;
    if (qtcore.Load(QtCoreLibName) == KErrNone)
    {
        const int qt_symbian_SetupThreadHeap_eabi_ordinal = 3713;
        TLibraryFunction libFunc = qtcore.Lookup(qt_symbian_SetupThreadHeap_eabi_ordinal);
        if (libFunc)
        {
            typedef int (*TSetupThreadHeapFunc)(TBool aNotFirst, SStdEpocThreadCreateInfo& aInfo);
            TSetupThreadHeapFunc p_qt_symbian_SetupThreadHeap = TSetupThreadHeapFunc(libFunc);
            r = (*p_qt_symbian_SetupThreadHeap)(aNotFirst, aInfo);
        }
        qtcore.Close();
        if (libFunc)
            return r;
    }
#endif

    // no fast allocator support - use default allocator creation
    if (!aInfo.iAllocator && aInfo.iHeapInitialSize>0)
        {
        // new heap required
        RHeap* pH = NULL;
        r = UserHeap::CreateThreadHeap(aInfo, pH);
        }
    else if (aInfo.iAllocator)
        {
        // sharing a heap
        RAllocator* pA = aInfo.iAllocator;
        pA->Open();
        User::SwitchAllocator(pA);
        r = KErrNone;
        }
    return r;
}

#endif // QT_EXPORTS_NOT_FROZEN
