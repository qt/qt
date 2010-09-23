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

#ifndef QT_HYBRIDHEAP_SYMBIAN_H
#define QT_HYBRIDHEAP_SYMBIAN_H

#include <qglobal.h>

#ifdef QT_USE_NEW_SYMBIAN_ALLOCATOR

#include "common_p.h"
#ifdef __KERNEL_MODE__
#include <kernel/kern_priv.h>
#endif
#include "dla_p.h"
#ifndef __KERNEL_MODE__
#include "slab_p.h"
#include "page_alloc_p.h"
#endif
#include "heap_hybrid_p.h"

// disabling Symbian import/export macros to prevent code copied from Symbian^4 from exporting symbols in arm builds
#undef UIMPORT_C
#define UIMPORT_C
#undef IMPORT_C
#define IMPORT_C
#undef UEXPORT_C
#define UEXPORT_C
#undef EXPORT_C
#define EXPORT_C
#undef IMPORT_D
#define IMPORT_D

// disabling code ported from Symbian^4 that we don't want/can't have in earlier platforms
#define SYMBIAN4_ALLOCATOR_UNWANTED_CODE

#endif /* QT_USE_NEW_SYMBIAN_ALLOCATOR */

#endif /* QT_HYBRIDHEAP_SYMBIAN_H */
