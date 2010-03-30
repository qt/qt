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

#include "qsimd_p.h"
#include <QByteArray>

#if defined(Q_OS_WINCE)
#include <windows.h>
#endif

QT_BEGIN_NAMESPACE

uint qDetectCPUFeatures()
{
    static uint features = 0xffffffff;
    if (features != 0xffffffff)
        return features;

#if defined (Q_OS_WINCE)
#if defined (ARM)
    if (IsProcessorFeaturePresent(PF_ARM_INTEL_WMMX)) {
        features = IWMMXT;
        return features;
    }
#elif defined(_X86_)
    features = 0;
#if defined QT_HAVE_MMX
    if (IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE))
        features |= MMX;
#endif
#if defined QT_HAVE_3DNOW
    if (IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE))
        features |= MMX3DNOW;
#endif
    return features;
#endif
    features = 0;
    return features;
#elif defined(QT_HAVE_IWMMXT)
    // runtime detection only available when running as a previlegied process
    static const bool doIWMMXT = !qgetenv("QT_NO_IWMMXT").toInt();
    features = doIWMMXT ? IWMMXT : 0;
    return features;
#elif defined(QT_HAVE_NEON)
    static const bool doNEON = !qgetenv("QT_NO_NEON").toInt();
    features = doNEON ? NEON : 0;
    return features;
#else
    features = 0;
#if defined(__x86_64__) || defined(Q_OS_WIN64)
    features = MMX|SSE|SSE2|CMOV;
#elif defined(__ia64__)
    features = MMX|SSE|SSE2;
#elif defined(__i386__) || defined(_M_IX86)
    unsigned int extended_result = 0;
    uint result = 0;
    /* see p. 118 of amd64 instruction set manual Vol3 */
#if defined(Q_CC_GNU)
    asm ("push %%ebx\n"
         "pushf\n"
         "pop %%eax\n"
         "mov %%eax, %%ebx\n"
         "xor $0x00200000, %%eax\n"
         "push %%eax\n"
         "popf\n"
         "pushf\n"
         "pop %%eax\n"
         "xor %%edx, %%edx\n"
         "xor %%ebx, %%eax\n"
         "jz 1f\n"

         "mov $0x00000001, %%eax\n"
         "cpuid\n"
         "1:\n"
         "pop %%ebx\n"
         "mov %%edx, %0\n"
        : "=r" (result)
        :
        : "%eax", "%ecx", "%edx"
        );

    asm ("push %%ebx\n"
         "pushf\n"
         "pop %%eax\n"
         "mov %%eax, %%ebx\n"
         "xor $0x00200000, %%eax\n"
         "push %%eax\n"
         "popf\n"
         "pushf\n"
         "pop %%eax\n"
         "xor %%edx, %%edx\n"
         "xor %%ebx, %%eax\n"
         "jz 2f\n"

         "mov $0x80000000, %%eax\n"
         "cpuid\n"
         "cmp $0x80000000, %%eax\n"
         "jbe 2f\n"
         "mov $0x80000001, %%eax\n"
         "cpuid\n"
         "2:\n"
         "pop %%ebx\n"
         "mov %%edx, %0\n"
        : "=r" (extended_result)
        :
        : "%eax", "%ecx", "%edx"
        );
#elif defined (Q_OS_WIN)
    _asm {
        push eax
        push ebx
        push ecx
        push edx
        pushfd
        pop eax
        mov ebx, eax
        xor eax, 00200000h
        push eax
        popfd
        pushfd
        pop eax
        mov edx, 0
        xor eax, ebx
        jz skip

        mov eax, 1
        cpuid
        mov result, edx
    skip:
        pop edx
        pop ecx
        pop ebx
        pop eax
    }

    _asm {
        push eax
        push ebx
        push ecx
        push edx
        pushfd
        pop eax
        mov ebx, eax
        xor eax, 00200000h
        push eax
        popfd
        pushfd
        pop eax
        mov edx, 0
        xor eax, ebx
        jz skip2

        mov eax, 80000000h
        cpuid
        cmp eax, 80000000h
        jbe skip2
        mov eax, 80000001h
        cpuid
        mov extended_result, edx
    skip2:
        pop edx
        pop ecx
        pop ebx
        pop eax
    }
#endif

    // result now contains the standard feature bits
    if (result & (1u << 15))
        features |= CMOV;
    if (result & (1u << 23))
        features |= MMX;
    if (extended_result & (1u << 22))
        features |= MMXEXT;
    if (extended_result & (1u << 31))
        features |= MMX3DNOW;
    if (extended_result & (1u << 30))
        features |= MMX3DNOWEXT;
    if (result & (1u << 25))
        features |= SSE;
    if (result & (1u << 26))
        features |= SSE2;
#endif // i386

#if defined(QT_HAVE_MMX)
    if (qgetenv("QT_NO_MMX").toInt())
        features ^= MMX;
#endif
    if (qgetenv("QT_NO_MMXEXT").toInt())
        features ^= MMXEXT;

#if defined(QT_HAVE_3DNOW)
    if (qgetenv("QT_NO_3DNOW").toInt())
        features ^= MMX3DNOW;
#endif
    if (qgetenv("QT_NO_3DNOWEXT").toInt())
        features ^= MMX3DNOWEXT;

#if defined(QT_HAVE_SSE)
    if (qgetenv("QT_NO_SSE").toInt())
        features ^= SSE;
#endif
#if defined(QT_HAVE_SSE2)
    if (qgetenv("QT_NO_SSE2").toInt())
        features ^= SSE2;
#endif

    return features;
#endif
}

QT_END_NAMESPACE
