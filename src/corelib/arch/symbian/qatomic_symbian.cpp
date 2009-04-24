/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>

#if defined(Q_CC_RVCT)

#include "../arm/qatomic_arm.cpp"

QT_BEGIN_NAMESPACE

// This declspec needs to be explicit. RVCT has a bug which prevents embedded
// assembler functions from being exported (normally all functions are
// exported, and Q_CORE_EXPORT resolves to nothing).
__declspec(dllexport) __asm char q_atomic_swp(volatile char *ptr, char newval)
{
    add r2, pc, #0
    bx r2
    arm
    swpb r2,r1,[r0]
    mov r0, r2
    bx lr
    thumb
}

__declspec(dllexport) __asm int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
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

