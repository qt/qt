/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include <malloc.h>
#include <limits.h>
#include "3rdparty/memcheck.h"

/* Use glibc's memory allocation hooks */

/* our hooks */
static void *my_malloc_hook(size_t, const void *);
static void *my_realloc_hook(void *, size_t, const void *);
static void *my_memalign_hook(size_t, size_t, const void *);
static void my_free_hook(void *, const void *);

/* original hooks. */
static void *(*old_malloc_hook)(size_t, const void *);
static void *(*old_realloc_hook)(void *, size_t, const void *);
static void *(*old_memalign_hook)(size_t, size_t, const void *);
static void (*old_free_hook)(void *, const void *);

/* initializer function */
static void my_init_hook();

/* Override initialising hook from the C library. */
void (*__malloc_initialize_hook) (void) = my_init_hook;

static void disableHooks()
{
    __malloc_hook = old_malloc_hook;
    __realloc_hook = old_realloc_hook;
    __memalign_hook = old_memalign_hook;
    __free_hook = old_free_hook;
}

static void enableHooks()
{
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;
    __memalign_hook = my_memalign_hook;
    __free_hook = my_free_hook;
}

void my_init_hook()
{
    old_malloc_hook = __malloc_hook;
    old_realloc_hook = __realloc_hook;
    old_memalign_hook = __memalign_hook;
    old_free_hook = __free_hook;
    enableHooks();
}

static bool mallocFailActive = false;
static int mallocFailIndex = 0;
static int mallocCount = 0;

struct AllocFailer
{
    inline AllocFailer() { mallocFailActive = true; setAllocFailIndex(0); }
    inline ~AllocFailer() { deactivate(); }

    inline void setAllocFailIndex(int index)
    {
        if (RUNNING_ON_VALGRIND) {
            VALGRIND_ENABLE_OOM_AT_ALLOC_INDEX(VALGRIND_GET_ALLOC_INDEX + index + 1);
        } else {
            mallocFailIndex = index;
        }
    }

    inline void deactivate()
    {
        mallocFailActive = false;
        VALGRIND_ENABLE_OOM_AT_ALLOC_INDEX(INT_MAX);
    }

    inline int currentAllocIndex() const
    {
        if (RUNNING_ON_VALGRIND) {
            return VALGRIND_GET_ALLOC_INDEX;
        } else {
            return mallocCount;
        }
    }
};

void *my_malloc_hook(size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __malloc_hook = old_malloc_hook;
    void *result = ::malloc (size);
    __malloc_hook = my_malloc_hook;

    return result;
}

void *my_memalign_hook(size_t alignment, size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __memalign_hook = old_memalign_hook;
    void *result = ::memalign(alignment, size);
    __memalign_hook = my_memalign_hook;

    return result;
}

void *my_realloc_hook(void *ptr, size_t size, const void *)
{
    ++mallocCount;

    if (mallocFailActive && --mallocFailIndex < 0)
        return 0; // simulate OOM

    __realloc_hook = old_realloc_hook;
    __malloc_hook = old_malloc_hook;
    void *result = ::realloc(ptr, size);
    __malloc_hook = my_malloc_hook;
    __realloc_hook = my_realloc_hook;

    return result;
}

void my_free_hook(void *ptr, const void *)
{
    __free_hook = old_free_hook;
    ::free(ptr);
    __free_hook = my_free_hook;
}

static void *new_helper(std::size_t size)
{
    void *ptr = malloc(size);
    if (!ptr)
        throw std::bad_alloc();
    return ptr;
}

// overload operator new
void* operator new(size_t size) throw (std::bad_alloc) { return new_helper(size); }
void* operator new[](size_t size) throw (std::bad_alloc) { return new_helper(size); }
void* operator new(size_t size, const std::nothrow_t&) throw() { return malloc(size); }
void* operator new[](std::size_t size, const std::nothrow_t&) throw() { return malloc(size); }

// overload operator delete
void operator delete(void *ptr) throw() { if (ptr) free(ptr); }
void operator delete[](void *ptr) throw() { if (ptr) free(ptr); }
void operator delete(void *ptr, const std::nothrow_t&) throw() { if (ptr) free(ptr); }
void operator delete[](void *ptr, const std::nothrow_t&) throw() { if (ptr) free (ptr); }

// ignore placement new and placement delete - those don't allocate.


