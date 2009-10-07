/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>

#include <e32debug.h>

QT_BEGIN_NAMESPACE

// Heap and handle info printer.
// This way we can report on heap cells and handles that are really not owned by anything which still exists.
// This information can be used to detect whether memory leaks are happening, particularly if these numbers grow as the app is used more.
// This code is placed here as it happens to make it the very last static to be destroyed in a Qt app. The
// reason assumed is that this file appears before any other file declaring static data in the generated 
// Symbian MMP file. This particular file was chosen as it is the earliest symbian specific file.
struct QSymbianPrintExitInfo
{
    QSymbianPrintExitInfo()
    {
        RThread().HandleCount(initProcessHandleCount, initThreadHandleCount);
        initCells = User::CountAllocCells();
    }
    ~QSymbianPrintExitInfo()
    {
        RProcess myProc;
        TFullName fullName = myProc.FileName();
        TInt cells = User::CountAllocCells();
        TInt processHandleCount=0;
        TInt threadHandleCount=0;
        RThread().HandleCount(processHandleCount, threadHandleCount);
        RDebug::Print(_L("%S exiting with %d allocated cells, %d handles"),
            &fullName,
            cells - initCells,
            (processHandleCount + threadHandleCount) - (initProcessHandleCount + initThreadHandleCount));
    }
    TInt initCells;
    TInt initProcessHandleCount;
    TInt initThreadHandleCount;
} symbian_printExitInfo;

QT_END_NAMESPACE


#if defined(Q_CC_RVCT)

#include "../arm/qatomic_arm.cpp"

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT __asm char q_atomic_swp(volatile char *ptr, char newval)
{
    add r2, pc, #0
    bx r2
    arm
    swpb r2,r1,[r0]
    mov r0, r2
    bx lr
    thumb
}

Q_CORE_EXPORT __asm int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    add r2, pc, #0
    bx r2
    arm
    swp r2,r1,[r0]
    mov r0, r2
    bx lr
    thumb
}

QT_END_NAMESPACE

#endif // Q_CC_RVCT
