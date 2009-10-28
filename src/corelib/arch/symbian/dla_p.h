/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Symbian application wrapper of the Qt Toolkit.
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
#ifndef __DLA__
#define __DLA__

#define DEFAULT_TRIM_THRESHOLD ((size_t)4U * (size_t)1024U)

#define __SYMBIAN__
#define MSPACES 0
#define HAVE_MORECORE 1
#define	MORECORE_CONTIGUOUS 1
#define	HAVE_MMAP 0
#define HAVE_MREMAP 0
#define DEFAULT_GRANULARITY (4096U)
#define FOOTERS 0
#define USE_LOCKS 0
#define INSECURE 1
#define NO_MALLINFO 0
#define HAVE_GETPAGESIZE

#define LACKS_SYS_TYPES_H
#ifndef LACKS_SYS_TYPES_H
#include <sys/types.h>  /* For size_t */
#else
#ifndef _SIZE_T_DECLARED
typedef unsigned int size_t;
#define	_SIZE_T_DECLARED
#endif
#endif  /* LACKS_SYS_TYPES_H */

/* The maximum possible size_t value has all bits set */
#define MAX_SIZE_T           (~(size_t)0)

#ifndef ONLY_MSPACES
	#define ONLY_MSPACES 0
#endif  /* ONLY_MSPACES */

#ifndef MSPACES
	#if ONLY_MSPACES
		#define MSPACES 1
	#else   /* ONLY_MSPACES */
		#define MSPACES 0
	#endif  /* ONLY_MSPACES */
#endif  /* MSPACES */

#ifndef MALLOC_ALIGNMENT
	#define MALLOC_ALIGNMENT ((size_t)8U)
#endif  /* MALLOC_ALIGNMENT */

#ifndef FOOTERS
	#define FOOTERS 0
#endif  /* FOOTERS */

#ifndef ABORT
//	#define ABORT  abort()
	#define ABORT  User::Invariant()// redefined so euser isn't dependant on oe
#endif  /* ABORT */

#ifndef ABORT_ON_ASSERT_FAILURE
	#define ABORT_ON_ASSERT_FAILURE 1
#endif  /* ABORT_ON_ASSERT_FAILURE */

#ifndef PROCEED_ON_ERROR
	#define PROCEED_ON_ERROR 0
#endif  /* PROCEED_ON_ERROR */

#ifndef USE_LOCKS
	#define USE_LOCKS 0
#endif  /* USE_LOCKS */

#ifndef INSECURE
	#define INSECURE 0
#endif  /* INSECURE */

#ifndef HAVE_MMAP
	#define HAVE_MMAP 1
#endif  /* HAVE_MMAP */

#ifndef MMAP_CLEARS
	#define MMAP_CLEARS 1
#endif  /* MMAP_CLEARS */

#ifndef HAVE_MREMAP
	#ifdef linux
		#define HAVE_MREMAP 1
	#else   /* linux */
		#define HAVE_MREMAP 0
	#endif  /* linux */
#endif  /* HAVE_MREMAP */

#ifndef MALLOC_FAILURE_ACTION
	//#define MALLOC_FAILURE_ACTION  errno = ENOMEM;
	#define MALLOC_FAILURE_ACTION ;
#endif  /* MALLOC_FAILURE_ACTION */

#ifndef HAVE_MORECORE
	#if ONLY_MSPACES
		#define HAVE_MORECORE 1 /*AMOD: has changed */
	#else   /* ONLY_MSPACES */
		#define HAVE_MORECORE 1
	#endif  /* ONLY_MSPACES */
#endif  /* HAVE_MORECORE */

#if !HAVE_MORECORE
	#define MORECORE_CONTIGUOUS 0
#else   /* !HAVE_MORECORE */
	#ifndef MORECORE
		#define MORECORE DLAdjust
	#endif  /* MORECORE */
	#ifndef MORECORE_CONTIGUOUS
		#define MORECORE_CONTIGUOUS 0
	#endif  /* MORECORE_CONTIGUOUS */
#endif  /* !HAVE_MORECORE */

#ifndef DEFAULT_GRANULARITY
	#if MORECORE_CONTIGUOUS
		#define DEFAULT_GRANULARITY 4096  /* 0 means to compute in init_mparams */
	#else   /* MORECORE_CONTIGUOUS */
		#define DEFAULT_GRANULARITY ((size_t)64U * (size_t)1024U)
	#endif  /* MORECORE_CONTIGUOUS */
#endif  /* DEFAULT_GRANULARITY */

#ifndef DEFAULT_TRIM_THRESHOLD
	#ifndef MORECORE_CANNOT_TRIM
		#define DEFAULT_TRIM_THRESHOLD ((size_t)2U * (size_t)1024U * (size_t)1024U)
	#else   /* MORECORE_CANNOT_TRIM */
		#define DEFAULT_TRIM_THRESHOLD MAX_SIZE_T
	#endif  /* MORECORE_CANNOT_TRIM */
#endif  /* DEFAULT_TRIM_THRESHOLD */

#ifndef DEFAULT_MMAP_THRESHOLD
	#if HAVE_MMAP
		#define DEFAULT_MMAP_THRESHOLD ((size_t)256U * (size_t)1024U)
	#else   /* HAVE_MMAP */
		#define DEFAULT_MMAP_THRESHOLD MAX_SIZE_T
	#endif  /* HAVE_MMAP */
#endif  /* DEFAULT_MMAP_THRESHOLD */

#ifndef USE_BUILTIN_FFS
	#define USE_BUILTIN_FFS 0
#endif  /* USE_BUILTIN_FFS */

#ifndef USE_DEV_RANDOM
	#define USE_DEV_RANDOM 0
#endif  /* USE_DEV_RANDOM */

#ifndef NO_MALLINFO
	#define NO_MALLINFO 0
#endif  /* NO_MALLINFO */
#ifndef MALLINFO_FIELD_TYPE
	#define MALLINFO_FIELD_TYPE size_t
#endif  /* MALLINFO_FIELD_TYPE */

/*
  mallopt tuning options.  SVID/XPG defines four standard parameter
  numbers for mallopt, normally defined in malloc.h.  None of these
  are used in this malloc, so setting them has no effect. But this
  malloc does support the following options.
*/

#define M_TRIM_THRESHOLD     (-1)
#define M_GRANULARITY        (-2)
#define M_MMAP_THRESHOLD     (-3)

#if !NO_MALLINFO
/*
  This version of malloc supports the standard SVID/XPG mallinfo
  routine that returns a struct containing usage properties and
  statistics. It should work on any system that has a
  /usr/include/malloc.h defining struct mallinfo.  The main
  declaration needed is the mallinfo struct that is returned (by-copy)
  by mallinfo().  The malloinfo struct contains a bunch of fields that
  are not even meaningful in this version of malloc.  These fields are
  are instead filled by mallinfo() with other numbers that might be of
  interest.

  HAVE_USR_INCLUDE_MALLOC_H should be set if you have a
  /usr/include/malloc.h file that includes a declaration of struct
  mallinfo.  If so, it is included; else a compliant version is
  declared below.  These must be precisely the same for mallinfo() to
  work.  The original SVID version of this struct, defined on most
  systems with mallinfo, declares all fields as ints. But some others
  define as unsigned long. If your system defines the fields using a
  type of different width than listed here, you MUST #include your
  system version and #define HAVE_USR_INCLUDE_MALLOC_H.
*/

/* #define HAVE_USR_INCLUDE_MALLOC_H */

#ifdef HAVE_USR_INCLUDE_MALLOC_H
#include "/usr/include/malloc.h"
#else /* HAVE_USR_INCLUDE_MALLOC_H */

struct mallinfo {
  MALLINFO_FIELD_TYPE arena;    /* non-mmapped space allocated from system */
  MALLINFO_FIELD_TYPE ordblks;  /* number of free chunks */
  MALLINFO_FIELD_TYPE smblks;   /* always 0 */
  MALLINFO_FIELD_TYPE hblks;    /* always 0 */
  MALLINFO_FIELD_TYPE hblkhd;   /* space in mmapped regions */
  MALLINFO_FIELD_TYPE usmblks;  /* maximum total allocated space */
  MALLINFO_FIELD_TYPE fsmblks;  /* always 0 */
  MALLINFO_FIELD_TYPE uordblks; /* total allocated space */
  MALLINFO_FIELD_TYPE fordblks; /* total free space */
  MALLINFO_FIELD_TYPE keepcost; /* releasable (via malloc_trim) space */
  MALLINFO_FIELD_TYPE cellCount;/* Number of chunks allocated*/
};

#endif /* HAVE_USR_INCLUDE_MALLOC_H */
#endif /* NO_MALLINFO */

#if MSPACES
	typedef void* mspace;
#endif /* MSPACES */

#ifndef __SYMBIAN__

#include <stdio.h>/* for printing in malloc_stats */

#ifndef LACKS_ERRNO_H
	#include <errno.h>       /* for MALLOC_FAILURE_ACTION */
#endif /* LACKS_ERRNO_H */

#if FOOTERS
	#include <time.h>        /* for magic initialization */
#endif /* FOOTERS */

#ifndef LACKS_STDLIB_H
	#include <stdlib.h>      /* for abort() */
#endif /* LACKS_STDLIB_H */

#ifdef DEBUG
	#if ABORT_ON_ASSERT_FAILURE
		#define assert(x) if(!(x)) ABORT
	#else /* ABORT_ON_ASSERT_FAILURE */
		#include <assert.h>
	#endif /* ABORT_ON_ASSERT_FAILURE */
#else  /* DEBUG */
		#define assert(x)
#endif /* DEBUG */

#ifndef LACKS_STRING_H
	#include <string.h>      /* for memset etc */
#endif  /* LACKS_STRING_H */

#if USE_BUILTIN_FFS
	#ifndef LACKS_STRINGS_H
		#include <strings.h>     /* for ffs */
	#endif /* LACKS_STRINGS_H */
#endif /* USE_BUILTIN_FFS */

#if HAVE_MMAP
	#ifndef LACKS_SYS_MMAN_H
		#include <sys/mman.h>    /* for mmap */
	#endif /* LACKS_SYS_MMAN_H */
	#ifndef LACKS_FCNTL_H
		#include <fcntl.h>
	#endif /* LACKS_FCNTL_H */
#endif /* HAVE_MMAP */

#if HAVE_MORECORE
	#ifndef LACKS_UNISTD_H
		#include <unistd.h>     /* for sbrk */
	extern void*     sbrk(size_t);
	#else /* LACKS_UNISTD_H */
		#if !defined(__FreeBSD__) && !defined(__OpenBSD__) && !defined(__NetBSD__)
			extern void*     sbrk(ptrdiff_t);
			/*Amod sbrk is not defined in WIN32 need to check in symbian*/
		#endif /* FreeBSD etc */
	#endif /* LACKS_UNISTD_H */
#endif /* HAVE_MORECORE */

#endif

#define assert(x)	ASSERT(x)

#ifndef WIN32
	#ifndef malloc_getpagesize
		#ifdef _SC_PAGESIZE         /* some SVR4 systems omit an underscore */
			#ifndef _SC_PAGE_SIZE
				#define _SC_PAGE_SIZE _SC_PAGESIZE
			#endif
		#endif
		#ifdef _SC_PAGE_SIZE
			#define malloc_getpagesize sysconf(_SC_PAGE_SIZE)
		#else
			#if defined(BSD) || defined(DGUX) || defined(HAVE_GETPAGESIZE)
				extern size_t getpagesize();
				#define malloc_getpagesize getpagesize()
			#else
				#ifdef WIN32 /* use supplied emulation of getpagesize */
					#define malloc_getpagesize getpagesize()
				#else
					#ifndef LACKS_SYS_PARAM_H
						#include <sys/param.h>
					#endif
					#ifdef EXEC_PAGESIZE
						#define malloc_getpagesize EXEC_PAGESIZE
					#else
						#ifdef NBPG
							#ifndef CLSIZE
								#define malloc_getpagesize NBPG
							#else
								#define malloc_getpagesize (NBPG * CLSIZE)
							#endif
						#else
							#ifdef NBPC
								#define malloc_getpagesize NBPC
							#else
								#ifdef PAGESIZE
									#define malloc_getpagesize PAGESIZE
								#else /* just guess */
									#define malloc_getpagesize ((size_t)4096U)
								#endif
							#endif
						#endif
					#endif
				#endif
			#endif
		#endif
	#endif
#endif

/* ------------------- size_t and alignment properties -------------------- */

/* The byte and bit size of a size_t */
#define SIZE_T_SIZE         (sizeof(size_t))
#define SIZE_T_BITSIZE      (sizeof(size_t) << 3)

/* Some constants coerced to size_t */
/* Annoying but necessary to avoid errors on some plaftorms */
#define SIZE_T_ZERO         ((size_t)0)
#define SIZE_T_ONE          ((size_t)1)
#define SIZE_T_TWO          ((size_t)2)
#define TWO_SIZE_T_SIZES    (SIZE_T_SIZE<<1)
#define FOUR_SIZE_T_SIZES   (SIZE_T_SIZE<<2)
#define SIX_SIZE_T_SIZES    (FOUR_SIZE_T_SIZES+TWO_SIZE_T_SIZES)
#define HALF_MAX_SIZE_T     (MAX_SIZE_T / 2U)

/* The bit mask value corresponding to MALLOC_ALIGNMENT */
#define CHUNK_ALIGN_MASK    (MALLOC_ALIGNMENT - SIZE_T_ONE)

/* True if address a has acceptable alignment */
//#define is_aligned(A)       (((size_t)((A)) & (CHUNK_ALIGN_MASK)) == 0)
#define is_aligned(A)       (((unsigned int)((A)) & (CHUNK_ALIGN_MASK)) == 0)

/* the number of bytes to offset an address to align it */
#define align_offset(A)\
	((((size_t)(A) & CHUNK_ALIGN_MASK) == 0)? 0 :\
	((MALLOC_ALIGNMENT - ((size_t)(A) & CHUNK_ALIGN_MASK)) & CHUNK_ALIGN_MASK))

/* -------------------------- MMAP preliminaries ------------------------- */

/*
   If HAVE_MORECORE or HAVE_MMAP are false, we just define calls and
   checks to fail so compiler optimizer can delete code rather than
   using so many "#if"s.
*/


/* MORECORE and MMAP must return MFAIL on failure */
#define MFAIL                ((void*)(MAX_SIZE_T))
#define CMFAIL               ((TUint8*)(MFAIL)) /* defined for convenience */

#if !HAVE_MMAP
	#define IS_MMAPPED_BIT       (SIZE_T_ZERO)
	#define USE_MMAP_BIT         (SIZE_T_ZERO)
	#define CALL_MMAP(s)         MFAIL
	#define CALL_MUNMAP(a, s)    (-1)
	#define DIRECT_MMAP(s)       MFAIL
#else /* !HAVE_MMAP */
	#define IS_MMAPPED_BIT       (SIZE_T_ONE)
	#define USE_MMAP_BIT         (SIZE_T_ONE)
		#ifndef WIN32
			#define CALL_MUNMAP(a, s)    DLUMMAP((a),(s)) /*munmap((a), (s))*/
			#define MMAP_PROT            (PROT_READ|PROT_WRITE)
			#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
				#define MAP_ANONYMOUS        MAP_ANON
			#endif /* MAP_ANON */
			#ifdef MAP_ANONYMOUS
				#define MMAP_FLAGS           (MAP_PRIVATE|MAP_ANONYMOUS)
				#define CALL_MMAP(s)         mmap(0, (s), MMAP_PROT, (int)MMAP_FLAGS, -1, 0)
			#else /* MAP_ANONYMOUS */
				/*
				   Nearly all versions of mmap support MAP_ANONYMOUS, so the following
				   is unlikely to be needed, but is supplied just in case.
				*/
				#define MMAP_FLAGS           (MAP_PRIVATE)
				//static int dev_zero_fd = -1; /* Cached file descriptor for /dev/zero. */
				#define CALL_MMAP(s) DLMMAP(s)
				/*#define CALL_MMAP(s) ((dev_zero_fd < 0) ? \
			           (dev_zero_fd = open("/dev/zero", O_RDWR), \
			            mmap(0, (s), MMAP_PROT, MMAP_FLAGS, dev_zero_fd, 0)) : \
			            mmap(0, (s), MMAP_PROT, MMAP_FLAGS, dev_zero_fd, 0))
			            */
				#define CALL_REMAP(a, s, d)    DLREMAP((a),(s),(d))
			#endif /* MAP_ANONYMOUS */
			#define DIRECT_MMAP(s)       CALL_MMAP(s)
		#else /* WIN32 */
			#define CALL_MMAP(s)         win32mmap(s)
			#define CALL_MUNMAP(a, s)    win32munmap((a), (s))
			#define DIRECT_MMAP(s)       win32direct_mmap(s)
		#endif /* WIN32 */
#endif /* HAVE_MMAP */

#if HAVE_MMAP && HAVE_MREMAP
	#define CALL_MREMAP(addr, osz, nsz, mv) mremap((addr), (osz), (nsz), (mv))
#else  /* HAVE_MMAP && HAVE_MREMAP */
	#define CALL_MREMAP(addr, osz, nsz, mv) MFAIL
#endif /* HAVE_MMAP && HAVE_MREMAP */

#if HAVE_MORECORE
	#define CALL_MORECORE(S)     SetBrk(S)
#else  /* HAVE_MORECORE */
	#define CALL_MORECORE(S)     MFAIL
#endif /* HAVE_MORECORE */

/* mstate bit set if continguous morecore disabled or failed */
#define USE_NONCONTIGUOUS_BIT (4U)

/* segment bit set in create_mspace_with_base */
#define EXTERN_BIT            (8U)


#if USE_LOCKS
/*
  When locks are defined, there are up to two global locks:
  * If HAVE_MORECORE, morecore_mutex protects sequences of calls to
    MORECORE.  In many cases sys_alloc requires two calls, that should
    not be interleaved with calls by other threads.  This does not
    protect against direct calls to MORECORE by other threads not
    using this lock, so there is still code to cope the best we can on
    interference.
  * magic_init_mutex ensures that mparams.magic and other
    unique mparams values are initialized only once.
*/
	#ifndef WIN32
		/* By default use posix locks */
		#include <pthread.h>
		#define MLOCK_T pthread_mutex_t
		#define INITIAL_LOCK(l)      pthread_mutex_init(l, NULL)
		#define ACQUIRE_LOCK(l)      pthread_mutex_lock(l)
		#define RELEASE_LOCK(l)      pthread_mutex_unlock(l)

		#if HAVE_MORECORE
			//static MLOCK_T morecore_mutex = PTHREAD_MUTEX_INITIALIZER;
		#endif /* HAVE_MORECORE */
			//static MLOCK_T magic_init_mutex = PTHREAD_MUTEX_INITIALIZER;
	#else /* WIN32 */
		#define MLOCK_T long
		#define INITIAL_LOCK(l)      *(l)=0
		#define ACQUIRE_LOCK(l)      win32_acquire_lock(l)
		#define RELEASE_LOCK(l)      win32_release_lock(l)
		#if HAVE_MORECORE
			static MLOCK_T morecore_mutex;
		#endif /* HAVE_MORECORE */
		static MLOCK_T magic_init_mutex;
	#endif /* WIN32 */
	#define USE_LOCK_BIT               (2U)
#else  /* USE_LOCKS */
	#define USE_LOCK_BIT               (0U)
	#define INITIAL_LOCK(l)
#endif /* USE_LOCKS */

#if USE_LOCKS && HAVE_MORECORE
	#define ACQUIRE_MORECORE_LOCK(M)    ACQUIRE_LOCK((M->morecore_mutex)/*&morecore_mutex*/);
	#define RELEASE_MORECORE_LOCK(M)    RELEASE_LOCK((M->morecore_mutex)/*&morecore_mutex*/);
#else /* USE_LOCKS && HAVE_MORECORE */
	#define ACQUIRE_MORECORE_LOCK(M)
	#define RELEASE_MORECORE_LOCK(M)
#endif /* USE_LOCKS && HAVE_MORECORE */

#if USE_LOCKS
		/*Currently not suporting this*/
	#define ACQUIRE_MAGIC_INIT_LOCK(M)  ACQUIRE_LOCK(((M)->magic_init_mutex));
	//AMOD: changed #define ACQUIRE_MAGIC_INIT_LOCK()
	//#define RELEASE_MAGIC_INIT_LOCK()
	#define RELEASE_MAGIC_INIT_LOCK(M)  RELEASE_LOCK(((M)->magic_init_mutex));
#else  /* USE_LOCKS */
	#define ACQUIRE_MAGIC_INIT_LOCK(M)
	#define RELEASE_MAGIC_INIT_LOCK(M)
#endif /* USE_LOCKS */

/*CHUNK representation*/
struct malloc_chunk {
  size_t               prev_foot;  /* Size of previous chunk (if free).  */
  size_t               head;       /* Size and inuse bits. */
  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;
};

typedef struct malloc_chunk  mchunk;
typedef struct malloc_chunk* mchunkptr;
typedef struct malloc_chunk* sbinptr;  /* The type of bins of chunks */
typedef unsigned int bindex_t;         /* Described below */
typedef unsigned int binmap_t;         /* Described below */
typedef unsigned int flag_t;           /* The type of various bit flag sets */


/* ------------------- Chunks sizes and alignments ----------------------- */
#define MCHUNK_SIZE         (sizeof(mchunk))

#if FOOTERS
	#define CHUNK_OVERHEAD      (TWO_SIZE_T_SIZES)
#else /* FOOTERS */
	#define CHUNK_OVERHEAD      (SIZE_T_SIZE)
#endif /* FOOTERS */

/* MMapped chunks need a second word of overhead ... */
#define MMAP_CHUNK_OVERHEAD (TWO_SIZE_T_SIZES)
/* ... and additional padding for fake next-chunk at foot */
#define MMAP_FOOT_PAD       (FOUR_SIZE_T_SIZES)

/* The smallest size we can malloc is an aligned minimal chunk */
#define MIN_CHUNK_SIZE ((MCHUNK_SIZE + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* conversion from malloc headers to user pointers, and back */
#define chunk2mem(p)        ((void*)((TUint8*)(p)       + TWO_SIZE_T_SIZES))
#define mem2chunk(mem)      ((mchunkptr)((TUint8*)(mem) - TWO_SIZE_T_SIZES))
/* chunk associated with aligned address A */
#define align_as_chunk(A)   (mchunkptr)((A) + align_offset(chunk2mem(A)))

/* Bounds on request (not chunk) sizes. */
#define MAX_REQUEST         ((-MIN_CHUNK_SIZE) << 2)
#define MIN_REQUEST         (MIN_CHUNK_SIZE - CHUNK_OVERHEAD - SIZE_T_ONE)

/* pad request bytes into a usable size */
#define pad_request(req) (((req) + CHUNK_OVERHEAD + CHUNK_ALIGN_MASK) & ~CHUNK_ALIGN_MASK)

/* pad request, checking for minimum (but not maximum) */
#define request2size(req) (((req) < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(req))

/* ------------------ Operations on head and foot fields ----------------- */

/*
  The head field of a chunk is or'ed with PINUSE_BIT when previous
  adjacent chunk in use, and or'ed with CINUSE_BIT if this chunk is in
  use. If the chunk was obtained with mmap, the prev_foot field has
  IS_MMAPPED_BIT set, otherwise holding the offset of the base of the
  mmapped region to the base of the chunk.
*/
#define PINUSE_BIT          (SIZE_T_ONE)
#define CINUSE_BIT          (SIZE_T_TWO)
#define INUSE_BITS          (PINUSE_BIT|CINUSE_BIT)

/* Head value for fenceposts */
#define FENCEPOST_HEAD      (INUSE_BITS|SIZE_T_SIZE)

/* extraction of fields from head words */
#define cinuse(p)           ((p)->head & CINUSE_BIT)
#define pinuse(p)           ((p)->head & PINUSE_BIT)
#define chunksize(p)        ((p)->head & ~(INUSE_BITS))

#define clear_pinuse(p)     ((p)->head &= ~PINUSE_BIT)
#define clear_cinuse(p)     ((p)->head &= ~CINUSE_BIT)

/* Treat space at ptr +/- offset as a chunk */
#define chunk_plus_offset(p, s)  ((mchunkptr)(((TUint8*)(p)) + (s)))
#define chunk_minus_offset(p, s) ((mchunkptr)(((TUint8*)(p)) - (s)))

/* Ptr to next or previous physical malloc_chunk. */
#define next_chunk(p) ((mchunkptr)( ((TUint8*)(p)) + ((p)->head & ~INUSE_BITS)))
#define prev_chunk(p) ((mchunkptr)( ((TUint8*)(p)) - ((p)->prev_foot) ))

/* extract next chunk's pinuse bit */
#define next_pinuse(p)  ((next_chunk(p)->head) & PINUSE_BIT)

/* Get/set size at footer */
#define get_foot(p, s)  (((mchunkptr)((TUint8*)(p) + (s)))->prev_foot)
#define set_foot(p, s)  (((mchunkptr)((TUint8*)(p) + (s)))->prev_foot = (s))

/* Set size, pinuse bit, and foot */
#define set_size_and_pinuse_of_free_chunk(p, s) ((p)->head = (s|PINUSE_BIT), set_foot(p, s))

/* Set size, pinuse bit, foot, and clear next pinuse */
#define set_free_with_pinuse(p, s, n) (clear_pinuse(n), set_size_and_pinuse_of_free_chunk(p, s))

#define is_mmapped(p) (!((p)->head & PINUSE_BIT) && ((p)->prev_foot & IS_MMAPPED_BIT))

/* Get the internal overhead associated with chunk p */
#define overhead_for(p) (is_mmapped(p)? MMAP_CHUNK_OVERHEAD : CHUNK_OVERHEAD)

/* Return true if malloced space is not necessarily cleared */
#if MMAP_CLEARS
	#define calloc_must_clear(p) (!is_mmapped(p))
#else /* MMAP_CLEARS */
	#define calloc_must_clear(p) (1)
#endif /* MMAP_CLEARS */

/* ---------------------- Overlaid data structures ----------------------- */
struct malloc_tree_chunk {
  /* The first four fields must be compatible with malloc_chunk */
  size_t                    				prev_foot;
  size_t                    				head;
  struct malloc_tree_chunk*	fd;
  struct malloc_tree_chunk*	bk;

  struct malloc_tree_chunk* child[2];
  struct malloc_tree_chunk* parent;
  bindex_t                  index;
};

typedef struct malloc_tree_chunk  tchunk;
typedef struct malloc_tree_chunk* tchunkptr;
typedef struct malloc_tree_chunk* tbinptr; /* The type of bins of trees */

/* A little helper macro for trees */
#define leftmost_child(t) ((t)->child[0] != 0? (t)->child[0] : (t)->child[1])
/*Segment structur*/
struct malloc_segment {
  TUint8*        base;             /* base address */
  size_t       size;             /* allocated size */
  struct malloc_segment* next;   /* ptr to next segment */
  flag_t       sflags;           /* mmap and extern flag */
};

#define is_mmapped_segment(S)  ((S)->sflags & IS_MMAPPED_BIT)
#define is_extern_segment(S)   ((S)->sflags & EXTERN_BIT)

typedef struct malloc_segment  msegment;
typedef struct malloc_segment* msegmentptr;

/*Malloc State data structur*/

#define NSMALLBINS        (32U)
#define NTREEBINS         (32U)
#define SMALLBIN_SHIFT    (3U)
#define SMALLBIN_WIDTH    (SIZE_T_ONE << SMALLBIN_SHIFT)
#define TREEBIN_SHIFT     (8U)
#define MIN_LARGE_SIZE    (SIZE_T_ONE << TREEBIN_SHIFT)
#define MAX_SMALL_SIZE    (MIN_LARGE_SIZE - SIZE_T_ONE)
#define MAX_SMALL_REQUEST (MAX_SMALL_SIZE - CHUNK_ALIGN_MASK - CHUNK_OVERHEAD)

struct malloc_state {
  binmap_t   smallmap;
  binmap_t   treemap;
  size_t     dvsize;
  size_t     topsize;
  TUint8*      least_addr;
  mchunkptr  dv;
  mchunkptr  top;
  size_t     trim_check;
  size_t     magic;
  mchunkptr  smallbins[(NSMALLBINS+1)*2];
  tbinptr    treebins[NTREEBINS];
  size_t     footprint;
  size_t     max_footprint;
  flag_t     mflags;
#if USE_LOCKS
  MLOCK_T    mutex;     /* locate lock among fields that rarely change */
  MLOCK_T	magic_init_mutex;
  MLOCK_T	morecore_mutex;
#endif /* USE_LOCKS */
  msegment   seg;
};

typedef struct malloc_state*    mstate;

/* ------------- Global malloc_state and malloc_params ------------------- */

/*
  malloc_params holds global properties, including those that can be
  dynamically set using mallopt. There is a single instance, mparams,
  initialized in init_mparams.
*/

struct malloc_params {
  size_t magic;
  size_t page_size;
  size_t granularity;
  size_t mmap_threshold;
  size_t trim_threshold;
  flag_t default_mflags;
#if USE_LOCKS
  MLOCK_T	magic_init_mutex;
#endif /* USE_LOCKS */
};

/* The global malloc_state used for all non-"mspace" calls */
/*AMOD: Need to check this as this will be the member of the class*/

//static struct malloc_state _gm_;
//#define gm                 (&_gm_)

//#define is_global(M)       ((M) == &_gm_)
/*AMOD: has changed*/
#define is_global(M)       ((M) == gm)
#define is_initialized(M)  ((M)->top != 0)

/* -------------------------- system alloc setup ------------------------- */

/* Operations on mflags */

#define use_lock(M)           ((M)->mflags &   USE_LOCK_BIT)
#define enable_lock(M)        ((M)->mflags |=  USE_LOCK_BIT)
#define disable_lock(M)       ((M)->mflags &= ~USE_LOCK_BIT)

#define use_mmap(M)           ((M)->mflags &   USE_MMAP_BIT)
#define enable_mmap(M)        ((M)->mflags |=  USE_MMAP_BIT)
#define disable_mmap(M)       ((M)->mflags &= ~USE_MMAP_BIT)

#define use_noncontiguous(M)  ((M)->mflags &   USE_NONCONTIGUOUS_BIT)
#define disable_contiguous(M) ((M)->mflags |=  USE_NONCONTIGUOUS_BIT)

#define set_lock(M,L) ((M)->mflags = (L)? ((M)->mflags | USE_LOCK_BIT) :  ((M)->mflags & ~USE_LOCK_BIT))

/* page-align a size */
#define page_align(S) (((S) + (mparams.page_size)) & ~(mparams.page_size - SIZE_T_ONE))

/* granularity-align a size */
#define granularity_align(S)  (((S) + (mparams.granularity)) & ~(mparams.granularity - SIZE_T_ONE))

#define is_page_aligned(S)   (((size_t)(S) & (mparams.page_size - SIZE_T_ONE)) == 0)
#define is_granularity_aligned(S)   (((size_t)(S) & (mparams.granularity - SIZE_T_ONE)) == 0)

/*  True if segment S holds address A */
#define segment_holds(S, A)  ((TUint8*)(A) >= S->base && (TUint8*)(A) < S->base + S->size)

#ifndef MORECORE_CANNOT_TRIM
	#define should_trim(M,s)  ((s) > (M)->trim_check)
#else  /* MORECORE_CANNOT_TRIM */
	#define should_trim(M,s)  (0)
#endif /* MORECORE_CANNOT_TRIM */

/*
  TOP_FOOT_SIZE is padding at the end of a segment, including space
  that may be needed to place segment records and fenceposts when new
  noncontiguous segments are added.
*/
#define TOP_FOOT_SIZE  (align_offset(chunk2mem(0))+pad_request(sizeof(struct malloc_segment))+MIN_CHUNK_SIZE)

/* -------------------------------  Hooks -------------------------------- */

/*
  PREACTION should be defined to return 0 on success, and nonzero on
  failure. If you are not using locking, you can redefine these to do
  anything you like.
*/

#if USE_LOCKS
	/* Ensure locks are initialized */
	#define GLOBALLY_INITIALIZE() (mparams.page_size == 0 && init_mparams())
	#define PREACTION(M) (use_lock((M))?(ACQUIRE_LOCK((M)->mutex),0):0) /*Action to take like lock before alloc*/
	#define POSTACTION(M) { if (use_lock(M)) RELEASE_LOCK((M)->mutex); }

#else /* USE_LOCKS */
	#ifndef PREACTION
		#define PREACTION(M) (0)
	#endif  /* PREACTION */
	#ifndef POSTACTION
		#define POSTACTION(M)
	#endif  /* POSTACTION */
#endif /* USE_LOCKS */

/*
  CORRUPTION_ERROR_ACTION is triggered upon detected bad addresses.
  USAGE_ERROR_ACTION is triggered on detected bad frees and
  reallocs. The argument p is an address that might have triggered the
  fault. It is ignored by the two predefined actions, but might be
  useful in custom actions that try to help diagnose errors.
*/

#if PROCEED_ON_ERROR
	/* A count of the number of corruption errors causing resets */
	int malloc_corruption_error_count;
	/* default corruption action */
	static void reset_on_error(mstate m);
	#define CORRUPTION_ERROR_ACTION(m)  reset_on_error(m)
	#define USAGE_ERROR_ACTION(m, p)
#else /* PROCEED_ON_ERROR */
	#ifndef CORRUPTION_ERROR_ACTION
		#define CORRUPTION_ERROR_ACTION(m) ABORT
	#endif /* CORRUPTION_ERROR_ACTION */
	#ifndef USAGE_ERROR_ACTION
		#define USAGE_ERROR_ACTION(m,p) ABORT
	#endif /* USAGE_ERROR_ACTION */
#endif /* PROCEED_ON_ERROR */

	/* -------------------------- Debugging setup ---------------------------- */

#if ! DEBUG
	#define check_free_chunk(M,P)
	#define check_inuse_chunk(M,P)
	#define check_malloced_chunk(M,P,N)
	#define check_mmapped_chunk(M,P)
	#define check_malloc_state(M)
	#define check_top_chunk(M,P)
#else /* DEBUG */
	#define check_free_chunk(M,P)       do_check_free_chunk(M,P)
	#define check_inuse_chunk(M,P)      do_check_inuse_chunk(M,P)
	#define check_top_chunk(M,P)        do_check_top_chunk(M,P)
	#define check_malloced_chunk(M,P,N) do_check_malloced_chunk(M,P,N)
	#define check_mmapped_chunk(M,P)    do_check_mmapped_chunk(M,P)
	#define check_malloc_state(M)       do_check_malloc_state(M)
	static void   do_check_any_chunk(mstate m, mchunkptr p);
	static void   do_check_top_chunk(mstate m, mchunkptr p);
	static void   do_check_mmapped_chunk(mstate m, mchunkptr p);
	static void   do_check_inuse_chunk(mstate m, mchunkptr p);
	static void   do_check_free_chunk(mstate m, mchunkptr p);
	static void   do_check_malloced_chunk(mstate m, void* mem, size_t s);
	static void   do_check_tree(mstate m, tchunkptr t);
	static void   do_check_treebin(mstate m, bindex_t i);
	static void   do_check_smallbin(mstate m, bindex_t i);
	static void   do_check_malloc_state(mstate m);
	static int    bin_find(mstate m, mchunkptr x);
	static size_t traverse_and_check(mstate m);
#endif /* DEBUG */

/* ---------------------------- Indexing Bins ---------------------------- */

#define is_small(s)         (((s) >> SMALLBIN_SHIFT) < NSMALLBINS)
#define small_index(s)      ((s)  >> SMALLBIN_SHIFT)
#define small_index2size(i) ((i)  << SMALLBIN_SHIFT)
#define MIN_SMALL_INDEX     (small_index(MIN_CHUNK_SIZE))

/* addressing by index. See above about smallbin repositioning */
#define smallbin_at(M, i)   ((sbinptr)((TUint8*)&((M)->smallbins[(i)<<1])))
#define treebin_at(M,i)     (&((M)->treebins[i]))


/* Bit representing maximum resolved size in a treebin at i */
#define bit_for_tree_index(i) (i == NTREEBINS-1)? (SIZE_T_BITSIZE-1) : (((i) >> 1) + TREEBIN_SHIFT - 2)

/* Shift placing maximum resolved bit in a treebin at i as sign bit */
#define leftshift_for_tree_index(i) ((i == NTREEBINS-1)? 0 : ((SIZE_T_BITSIZE-SIZE_T_ONE) - (((i) >> 1) + TREEBIN_SHIFT - 2)))

/* The size of the smallest chunk held in bin with index i */
#define minsize_for_tree_index(i) ((SIZE_T_ONE << (((i) >> 1) + TREEBIN_SHIFT)) |  (((size_t)((i) & SIZE_T_ONE)) << (((i) >> 1) + TREEBIN_SHIFT - 1)))


/* ------------------------ Operations on bin maps ----------------------- */
/* bit corresponding to given index */
#define idx2bit(i)              ((binmap_t)(1) << (i))
/* Mark/Clear bits with given index */
#define mark_smallmap(M,i)      ((M)->smallmap |=  idx2bit(i))
#define clear_smallmap(M,i)     ((M)->smallmap &= ~idx2bit(i))
#define smallmap_is_marked(M,i) ((M)->smallmap &   idx2bit(i))
#define mark_treemap(M,i)       ((M)->treemap  |=  idx2bit(i))
#define clear_treemap(M,i)      ((M)->treemap  &= ~idx2bit(i))
#define treemap_is_marked(M,i)  ((M)->treemap  &   idx2bit(i))

/* isolate the least set bit of a bitmap */
#define least_bit(x)         ((x) & -(x))

/* mask with all bits to left of least bit of x on */
#define left_bits(x)         ((x<<1) | -(x<<1))

/* mask with all bits to left of or equal to least bit of x on */
#define same_or_left_bits(x) ((x) | -(x))

	/* isolate the least set bit of a bitmap */
#define least_bit(x)         ((x) & -(x))

/* mask with all bits to left of least bit of x on */
#define left_bits(x)         ((x<<1) | -(x<<1))

/* mask with all bits to left of or equal to least bit of x on */
#define same_or_left_bits(x) ((x) | -(x))

#if !INSECURE
	/* Check if address a is at least as high as any from MORECORE or MMAP */
	#define ok_address(M, a) ((TUint8*)(a) >= (M)->least_addr)
	/* Check if address of next chunk n is higher than base chunk p */
	#define ok_next(p, n)    ((TUint8*)(p) < (TUint8*)(n))
	/* Check if p has its cinuse bit on */
	#define ok_cinuse(p)     cinuse(p)
	/* Check if p has its pinuse bit on */
	#define ok_pinuse(p)     pinuse(p)
#else /* !INSECURE */
	#define ok_address(M, a) (1)
	#define ok_next(b, n)    (1)
	#define ok_cinuse(p)     (1)
	#define ok_pinuse(p)     (1)
#endif /* !INSECURE */

#if (FOOTERS && !INSECURE)
	/* Check if (alleged) mstate m has expected magic field */
	#define ok_magic(M)      ((M)->magic == mparams.magic)
#else  /* (FOOTERS && !INSECURE) */
	#define ok_magic(M)      (1)
#endif /* (FOOTERS && !INSECURE) */

/* In gcc, use __builtin_expect to minimize impact of checks */
#if !INSECURE
	#if defined(__GNUC__) && __GNUC__ >= 3
		#define RTCHECK(e)  __builtin_expect(e, 1)
	#else /* GNUC */
		#define RTCHECK(e)  (e)
	#endif /* GNUC */

#else /* !INSECURE */
	#define RTCHECK(e)  (1)
#endif /* !INSECURE */
/* macros to set up inuse chunks with or without footers */
#if !FOOTERS
	#define mark_inuse_foot(M,p,s)
	/* Set cinuse bit and pinuse bit of next chunk */
	#define set_inuse(M,p,s)  ((p)->head = (((p)->head & PINUSE_BIT)|s|CINUSE_BIT),((mchunkptr)(((TUint8*)(p)) + (s)))->head |= PINUSE_BIT)
	/* Set cinuse and pinuse of this chunk and pinuse of next chunk */
	#define set_inuse_and_pinuse(M,p,s) ((p)->head = (s|PINUSE_BIT|CINUSE_BIT),((mchunkptr)(((TUint8*)(p)) + (s)))->head |= PINUSE_BIT)
	/* Set size, cinuse and pinuse bit of this chunk */
	#define set_size_and_pinuse_of_inuse_chunk(M, p, s) ((p)->head = (s|PINUSE_BIT|CINUSE_BIT))
#else /* FOOTERS */
	/* Set foot of inuse chunk to be xor of mstate and seed */
	#define mark_inuse_foot(M,p,s) (((mchunkptr)((TUint8*)(p) + (s)))->prev_foot = ((size_t)(M) ^ mparams.magic))
	#define get_mstate_for(p) ((mstate)(((mchunkptr)((TUint8*)(p)+(chunksize(p))))->prev_foot ^ mparams.magic))
	#define set_inuse(M,p,s)\
		((p)->head = (((p)->head & PINUSE_BIT)|s|CINUSE_BIT),\
		(((mchunkptr)(((TUint8*)(p)) + (s)))->head |= PINUSE_BIT), \
		mark_inuse_foot(M,p,s))
	#define set_inuse_and_pinuse(M,p,s)\
	((p)->head = (s|PINUSE_BIT|CINUSE_BIT),\
	(((mchunkptr)(((TUint8*)(p)) + (s)))->head |= PINUSE_BIT),\
	mark_inuse_foot(M,p,s))
	#define set_size_and_pinuse_of_inuse_chunk(M, p, s)\
	((p)->head = (s|PINUSE_BIT|CINUSE_BIT),\
	mark_inuse_foot(M, p, s))
#endif /* !FOOTERS */


#if ONLY_MSPACES
#define internal_malloc(m, b) mspace_malloc(m, b)
#define internal_free(m, mem) mspace_free(m,mem);
#else /* ONLY_MSPACES */
	#if MSPACES
		#define internal_malloc(m, b) (m == gm)? dlmalloc(b) : mspace_malloc(m, b)
		#define internal_free(m, mem) if (m == gm) dlfree(mem); else mspace_free(m,mem);
	#else /* MSPACES */
		#define internal_malloc(m, b) dlmalloc(b)
		#define internal_free(m, mem) dlfree(mem)
	#endif /* MSPACES */
#endif /* ONLY_MSPACES */
/******CODE TO SUPORT SLAB ALLOCATOR******/

	#ifndef NDEBUG
	#define CHECKING 1
	#endif

	#if CHECKING
	//#define ASSERT(x) {if (!(x)) abort();}
	#define CHECK(x) x
	#else
	#define ASSERT(x) (void)0
	#define CHECK(x) (void)0
	#endif

	class slab;
	class slabhdr;
	#define maxslabsize		56
	#define	pageshift		12
	#define pagesize		(1<<pageshift)
	#define	slabshift		10
	#define	slabsize		(1 << slabshift)
	#define cellalign		8
	const unsigned slabfull = 0;
	const TInt	slabsperpage	=	(int)(pagesize/slabsize);
	#define hibit(bits) (((unsigned)bits & 0xc) ? 2 + ((unsigned)bits>>3) : ((unsigned) bits>>1))

	#define lowbit(bits)	(((unsigned) bits&3) ? 1 - ((unsigned)bits&1) : 3 - (((unsigned)bits>>2)&1))
	#define minpagepower	pageshift+2
	#define cellalign	8
	class slabhdr
	{
	public:
		unsigned header;
		// made up of
		// bits   |    31    | 30..28 | 27..18 | 17..12 |  11..8  |   7..0   |
		//        +----------+--------+--------+--------+---------+----------+
		// field  | floating |  zero  | used-4 |  size  | pagemap | free pos |
		//
		slab** parent;		// reference to parent's pointer to this slab in tree
		slab* child1;		// 1st child in tree
		slab* child2;		// 2nd child in tree
	};

	inline unsigned header_floating(unsigned h)
	{return (h&0x80000000);}
	const unsigned maxuse = (slabsize - sizeof(slabhdr))>>2;
	const unsigned firstpos = sizeof(slabhdr)>>2;
	#define checktree(x) (void)0
	template <class T> inline T floor(const T addr, unsigned aln)
		{return T((unsigned(addr))&~(aln-1));}
	template <class T> inline T ceiling(T addr, unsigned aln)
		{return T((unsigned(addr)+(aln-1))&~(aln-1));}
	template <class T> inline unsigned lowbits(T addr, unsigned aln)
		{return unsigned(addr)&(aln-1);}
	template <class T1, class T2> inline int ptrdiff(const T1* a1, const T2* a2)
		{return reinterpret_cast<const unsigned char*>(a1) - reinterpret_cast<const unsigned char*>(a2);}
	template <class T> inline T offset(T addr, unsigned ofs)
		{return T(unsigned(addr)+ofs);}
	class slabset
	{
	public:
		slab* partial;
	};

	class slab : public slabhdr
	{
	public:
		void init(unsigned clz);
		//static slab* slabfor( void* p);
		static slab* slabfor(const void* p) ;
	private:
		unsigned char payload[slabsize-sizeof(slabhdr)];
	};
	class page
	{
	public:
		inline static page* pagefor(slab* s);
		//slab slabs;
		slab slabs[slabsperpage];
	};


	inline page* page::pagefor(slab* s)
		{return reinterpret_cast<page*>(floor(s, pagesize));}
	struct pagecell
	{
		void* page;
		unsigned size;
	};
	/******CODE TO SUPORT SLAB ALLOCATOR******/
#endif/*__DLA__*/
