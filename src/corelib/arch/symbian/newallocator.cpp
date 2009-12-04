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
** The memory allocator is backported from Symbian OS, and can eventually
** be removed from Qt once it is built in to all supported OS versions.
** The allocator is a composite of three allocators:
**  - A page allocator, for large allocations
**  - A slab allocator, for small allocations
**  - Doug Lea's allocator, for medium size allocations
****************************************************************************/
#include <qglobal.h>
#include <e32std.h>
#include <e32cmn.h>
#include <hal.h>
#include <e32panic.h>
#define RND_SDK
#ifndef RND_SDK
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
#else
#include <u32std.h>
#endif
#include <e32svr.h>

//Named local chunks require support from the kernel, which depends on Symbian^3
#define NO_NAMED_LOCAL_CHUNKS
//Reserving a minimum heap size is not supported, because the implementation does not know what type of
//memory to use. DLA memory grows upwards, slab and page allocators grow downwards.
//This would need kernel support to do properly.
#define NO_RESERVE_MEMORY

//The BTRACE debug framework requires Symbian OS 9.4 or higher.
//Required header files are not included in S60 5.0 SDKs, but
//they are available for open source versions of Symbian OS.
//Note that although Symbian OS 9.3 supports BTRACE, the usage in this file
//depends on 9.4 header files.

//This debug flag uses BTRACE to emit debug traces to identify the heaps.
//Note that it uses the ETest1 trace category which is not reserved
#define TRACING_HEAPS
//This debug flag uses BTRACE to emit debug traces to aid with debugging
//allocs, frees & reallocs. It should be used together with the KUSERHEAPTRACE
//kernel trace flag to enable heap tracing.
#define TRACING_ALLOCS

#if defined(TRACING_ALLOCS) || defined(TRACING_HEAPS)
#include <e32btrace.h>
#endif

#ifndef __WINS__
#pragma push
#pragma arm
#endif

#ifdef QT_USE_NEW_SYMBIAN_ALLOCATOR

#include "dla_p.h"
#include "newallocator_p.h"

// if non zero this causes the slabs to be configured only when the chunk size exceeds this level
#define DELAYED_SLAB_THRESHOLD (64*1024)		// 64KB seems about right based on trace data
#define SLAB_CONFIG (0xabe)

_LIT(KDLHeapPanicCategory, "DL Heap");
#define	GET_PAGE_SIZE(x)			HAL::Get(HALData::EMemoryPageSize, x)
#define	__CHECK_CELL(p)
#define __POWER_OF_2(x)				((TUint32)((x)^((x)-1))>=(TUint32)(x))
#define HEAP_PANIC(r)               Panic(r)

LOCAL_C void Panic(TCdtPanic aPanic)
// Panic the process with USER as the category.
	{
	User::Panic(_L("USER"),aPanic);
	}

size_t getpagesize()
{
    TInt size;
    TInt err = GET_PAGE_SIZE(size);
    if(err != KErrNone)
        return (size_t)0x1000;
    return (size_t)size;
}

#define gm  (&iGlobalMallocState)

RNewAllocator::RNewAllocator(TInt aMaxLength, TInt aAlign, TBool aSingleThread)
// constructor for a fixed heap. Just use DL allocator
	:iMinLength(aMaxLength), iMaxLength(aMaxLength), iOffset(0), iGrowBy(0), iChunkHandle(0),
	iNestingLevel(0), iAllocCount(0), iFailType(ENone), iTestData(NULL), iChunkSize(aMaxLength)
	{

	if ((TUint32)aAlign>=sizeof(TAny*) && __POWER_OF_2(iAlign))
		{
		iAlign = aAlign;
		}
	else
		{
		iAlign = 4;
		}
	iPageSize = 0;
	iFlags = aSingleThread ? (ESingleThreaded|EFixedSize) : EFixedSize;

	Init(0, 0, 0);
	}

RNewAllocator::RNewAllocator(TInt aChunkHandle, TInt aOffset, TInt aMinLength, TInt aMaxLength, TInt aGrowBy,
			TInt aAlign, TBool aSingleThread)
		: iMinLength(aMinLength), iMaxLength(aMaxLength), iOffset(aOffset), iChunkHandle(aChunkHandle), iAlign(aAlign), iNestingLevel(0), iAllocCount(0),
			iFailType(ENone), iTestData(NULL), iChunkSize(aMinLength),iHighWaterMark(aMinLength)
	{
	iPageSize = malloc_getpagesize;
	__ASSERT_ALWAYS(aOffset >=0, User::Panic(KDLHeapPanicCategory, ETHeapNewBadOffset));
	iGrowBy = _ALIGN_UP(aGrowBy, iPageSize);
	iFlags = aSingleThread ? ESingleThreaded : 0;

	// Initialise
	// if the heap is created with aMinLength==aMaxLength then it cannot allocate slab or page memory
	// so these sub-allocators should be disabled. Otherwise initialise with default values
	if (aMinLength == aMaxLength)
		Init(0, 0, 0);
	else
		Init(0xabe, 16, iPageSize*4);	// slabs {48, 40, 32, 24, 20, 16, 12, 8}, page {64KB}, trim {16KB}
#ifdef TRACING_HEAPS
	RChunk chunk;
	chunk.SetHandle(iChunkHandle);
	TKName chunk_name;
	chunk.FullName(chunk_name);
	BTraceContextBig(BTrace::ETest1, 2, 22, chunk_name.Ptr(), chunk_name.Size());

	TUint32 traceData[4];
	traceData[0] = iChunkHandle;
	traceData[1] = iMinLength;
	traceData[2] = iMaxLength;
	traceData[3] = iAlign;
	BTraceContextN(BTrace::ETest1, 1, (TUint32)this, 11, traceData, sizeof(traceData));
#endif

	}

TAny* RNewAllocator::operator new(TUint aSize, TAny* aBase) __NO_THROW
	{
	__ASSERT_ALWAYS(aSize>=sizeof(RNewAllocator), HEAP_PANIC(ETHeapNewBadSize));
	RNewAllocator* h = (RNewAllocator*)aBase;
	h->iAlign = 0x80000000;	// garbage value
	h->iBase = ((TUint8*)aBase) + aSize;
	return aBase;
	}

void RNewAllocator::Init(TInt aBitmapSlab, TInt aPagePower, size_t aTrimThreshold)
	{
	__ASSERT_ALWAYS((TUint32)iAlign>=sizeof(TAny*) && __POWER_OF_2(iAlign), HEAP_PANIC(ETHeapNewBadAlignment));

	/*Moved code which does initialization */
	iTop = (TUint8*)this + iMinLength;
	iAllocCount = 0;
	memset(&mparams,0,sizeof(mparams));

	Init_Dlmalloc(iTop - iBase, 0, aTrimThreshold);

	slab_init();
	slab_config_bits = aBitmapSlab;
#ifdef DELAYED_SLAB_THRESHOLD
	if (iChunkSize < DELAYED_SLAB_THRESHOLD)
		{
		slab_init_threshold = DELAYED_SLAB_THRESHOLD;
		}
	else
#endif // DELAYED_SLAB_THRESHOLD
		{
		slab_init_threshold = KMaxTUint;
		slab_config(aBitmapSlab);
		}

	/*10-1K,11-2K,12-4k,13-8K,14-16K,15-32K,16-64K*/
	paged_init(aPagePower);

#ifdef TRACING_ALLOCS
		TUint32 traceData[3];
		traceData[0] = aBitmapSlab;
		traceData[1] = aPagePower;
		traceData[2] = aTrimThreshold;
		BTraceContextN(BTrace::ETest1, BTrace::EHeapAlloc, (TUint32)this, 0, traceData, sizeof(traceData));
#endif

	}

RNewAllocator::SCell* RNewAllocator::GetAddress(const TAny* aCell) const
//
// As much as possible, check a cell address and backspace it
// to point at the cell header.
//
	{

	TLinAddr m = TLinAddr(iAlign - 1);
	__ASSERT_ALWAYS(!(TLinAddr(aCell)&m), HEAP_PANIC(ETHeapBadCellAddress));

	SCell* pC = (SCell*)(((TUint8*)aCell)-EAllocCellSize);
	__CHECK_CELL(pC);

	return pC;
	}

TInt RNewAllocator::AllocLen(const TAny* aCell) const
{
	if (ptrdiff(aCell, this) >= 0)
	{
		mchunkptr m = mem2chunk(aCell);
		return chunksize(m) - overhead_for(m);
	}
	if (lowbits(aCell, pagesize) > cellalign)
		return header_size(slab::slabfor(aCell)->header);
	if (lowbits(aCell, pagesize) == cellalign)
		return *(unsigned*)(offset(aCell,-int(cellalign)))-cellalign;
	return paged_descriptor(aCell)->size;
}

TAny* RNewAllocator::Alloc(TInt aSize)
{
	__ASSERT_ALWAYS((TUint)aSize<(KMaxTInt/2),HEAP_PANIC(ETHeapBadAllocatedCellSize));

	TAny* addr;

#ifdef TRACING_ALLOCS
	TInt aCnt=0;
#endif
	Lock();
	if (aSize < slab_threshold)
	{
		TInt ix = sizemap[(aSize+3)>>2];
		ASSERT(ix != 0xff);
		addr = slab_allocate(slaballoc[ix]);
	}else if((aSize >> page_threshold)==0)
		{
#ifdef TRACING_ALLOCS
		aCnt=1;
#endif
		addr = dlmalloc(aSize);
		}
	else
		{
#ifdef TRACING_ALLOCS
		aCnt=2;
#endif
		addr = paged_allocate(aSize);
		}

	iCellCount++;
	iTotalAllocSize += aSize;
	Unlock();

#ifdef TRACING_ALLOCS
	if (iFlags & ETraceAllocs)
		{
		TUint32 traceData[3];
		traceData[0] = AllocLen(addr);
		traceData[1] = aSize;
		traceData[2] = aCnt;
		BTraceContextN(BTrace::EHeap, BTrace::EHeapAlloc, (TUint32)this, (TUint32)addr, traceData, sizeof(traceData));
		}
#endif

	return addr;
}

TInt RNewAllocator::Compress()
	{
	if (iFlags & EFixedSize)
		return 0;

	Lock();
	dlmalloc_trim(0);
	if (spare_page)
		{
		unmap(spare_page,pagesize);
		spare_page = 0;
		}
	Unlock();
	return 0;
	}

void RNewAllocator::Free(TAny* aPtr)
{

#ifdef TRACING_ALLOCS
	TInt aCnt=0;
#endif
#ifdef ENABLE_DEBUG_TRACE
	RThread me;
	TBuf<100> thName;
	me.FullName(thName);
#endif
    //if (!aPtr) return; //return in case of NULL pointer

	Lock();

	if (!aPtr)
		;
	else if (ptrdiff(aPtr, this) >= 0)
		{
#ifdef TRACING_ALLOCS
		aCnt = 1;
#endif
		dlfree( aPtr);
		}
	else if (lowbits(aPtr, pagesize) <= cellalign)
		{
#ifdef TRACING_ALLOCS
		aCnt = 2;
#endif
		paged_free(aPtr);
		}
	else
		{
#ifdef TRACING_ALLOCS
		aCnt = 0;
#endif
		slab_free(aPtr);
		}
	iCellCount--;
	Unlock();

#ifdef TRACING_ALLOCS
	if (iFlags & ETraceAllocs)
		{
		TUint32 traceData;
		traceData = aCnt;
		BTraceContextN(BTrace::EHeap, BTrace::EHeapFree, (TUint32)this, (TUint32)aPtr, &traceData, sizeof(traceData));
		}
#endif
}


void RNewAllocator::Reset()
	{
	// TODO free everything
    User::Panic(_L("RNewAllocator"), 1); //this should never be called
	}

inline void RNewAllocator::TraceReAlloc(TAny* aPtr, TInt aSize, TAny* aNewPtr, TInt aZone)
	{
#ifdef TRACING_ALLOCS
    if (aNewPtr && (iFlags & ETraceAllocs))
        {
        TUint32 traceData[3];
        traceData[0] = AllocLen(aNewPtr);
        traceData[1] = aSize;
        traceData[2] = (TUint32)aPtr;
        BTraceContextN(BTrace::EHeap, BTrace::EHeapReAlloc,(TUint32)this, (TUint32)aNewPtr,traceData, sizeof(traceData));
        
        //workaround for SAW not handling reallocs properly
        if(aZone >= 0 && aPtr != aNewPtr) {
            BTraceContextN(BTrace::EHeap, BTrace::EHeapFree, (TUint32)this, (TUint32)aPtr, &aZone, sizeof(aZone));
        }
        }
#else
    Q_UNUSED(aPtr);
    Q_UNUSED(aSize);
    Q_UNUSED(aNewPtr);
    Q_UNUSED(aZone);
#endif
	}

TAny* RNewAllocator::ReAlloc(TAny* aPtr, TInt aSize, TInt /*aMode = 0*/)
	{
    if(ptrdiff(aPtr,this)>=0)
    {
        // original cell is in DL zone
        if(aSize >= slab_threshold && (aSize>>page_threshold)==0)
            {
            // and so is the new one
            Lock();
            TAny* addr = dlrealloc(aPtr,aSize);
            Unlock();
            TraceReAlloc(aPtr, aSize, addr, 0);
            return addr;
            }
    }
    else if(lowbits(aPtr,pagesize)<=cellalign)
    {
        // original cell is either NULL or in paged zone
        if (!aPtr)
            return Alloc(aSize);
        if(aSize >> page_threshold)
            {
            // and so is the new one
            Lock();
            TAny* addr = paged_reallocate(aPtr,aSize);
            Unlock();
            TraceReAlloc(aPtr, aSize, addr, 2);
            return addr;
            }
    }
    else
    {
        // original cell is in slab znoe
        if(aSize <= header_size(slab::slabfor(aPtr)->header)) {
            TraceReAlloc(aPtr, aSize, aPtr, 1);
            return aPtr;
        }
    }
    TAny* newp = Alloc(aSize);
    if(newp)
    {
        TInt oldsize = AllocLen(aPtr);
        memcpy(newp,aPtr,oldsize<aSize?oldsize:aSize);
        Free(aPtr);
    }
    return newp;
	}

TInt RNewAllocator::Available(TInt& aBiggestBlock) const
{
	//TODO: consider page and slab allocators

	//this gets free space in DL region - the C ported code doesn't respect const yet.
	RNewAllocator* self = const_cast<RNewAllocator*> (this);
	mallinfo info = self->dlmallinfo();
	aBiggestBlock = info.largestBlock;
	return info.fordblks;
}
TInt RNewAllocator::AllocSize(TInt& aTotalAllocSize) const
{
	aTotalAllocSize = iTotalAllocSize;
	return iCellCount;
}

TInt RNewAllocator::DebugFunction(TInt aFunc, TAny* a1, TAny* /*a2*/)
	{
    TInt r = KErrNotSupported;
    TInt* a1int = reinterpret_cast<TInt*>(a1);
    switch(aFunc) {
    case RAllocator::ECount:
    {
        struct mallinfo mi = dlmallinfo();
        *a1int = mi.fordblks;
        r = mi.uordblks;
    }
        break;
    case RAllocator::EMarkStart:
    case RAllocator::EMarkEnd:
    case RAllocator::ESetFail:
    case RAllocator::ECheck:
        r = KErrNone;
        break;
    }
	return r;
	}

TInt RNewAllocator::Extension_(TUint /* aExtensionId */, TAny*& /* a0 */, TAny* /* a1 */)
	{
	return KErrNotSupported;
	}

///////////////////////////////////////////////////////////////////////////////
// imported from dla.cpp
///////////////////////////////////////////////////////////////////////////////

//#include <unistd.h>
//#define DEBUG_REALLOC
#ifdef DEBUG_REALLOC
#include <e32debug.h>
#endif
int RNewAllocator::init_mparams(size_t aTrimThreshold /*= DEFAULT_TRIM_THRESHOLD*/)
{
	if (mparams.page_size == 0)
	{
		size_t s;
		mparams.mmap_threshold = DEFAULT_MMAP_THRESHOLD;
		mparams.trim_threshold = aTrimThreshold;
		#if MORECORE_CONTIGUOUS
			mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT;
		#else  /* MORECORE_CONTIGUOUS */
			mparams.default_mflags = USE_LOCK_BIT|USE_MMAP_BIT|USE_NONCONTIGUOUS_BIT;
		#endif /* MORECORE_CONTIGUOUS */

			s = (size_t)0x58585858U;
		ACQUIRE_MAGIC_INIT_LOCK(&mparams);
		if (mparams.magic == 0) {
		  mparams.magic = s;
		  /* Set up lock for main malloc area */
		  INITIAL_LOCK(&gm->mutex);
		  gm->mflags = mparams.default_mflags;
		}
		RELEASE_MAGIC_INIT_LOCK(&mparams);

		mparams.page_size = malloc_getpagesize;

		mparams.granularity = ((DEFAULT_GRANULARITY != 0)?
							   DEFAULT_GRANULARITY : mparams.page_size);

		/* Sanity-check configuration:
		   size_t must be unsigned and as wide as pointer type.
		   ints must be at least 4 bytes.
		   alignment must be at least 8.
		   Alignment, min chunk size, and page size must all be powers of 2.
		*/

		if ((sizeof(size_t) != sizeof(TUint8*)) ||
			(MAX_SIZE_T < MIN_CHUNK_SIZE)  ||
			(sizeof(int) < 4)  ||
			(MALLOC_ALIGNMENT < (size_t)8U) ||
			((MALLOC_ALIGNMENT    & (MALLOC_ALIGNMENT-SIZE_T_ONE))    != 0) ||
			((MCHUNK_SIZE         & (MCHUNK_SIZE-SIZE_T_ONE))         != 0) ||
			((mparams.granularity & (mparams.granularity-SIZE_T_ONE)) != 0) ||
			((mparams.page_size   & (mparams.page_size-SIZE_T_ONE))   != 0))
		  ABORT;
	}
	return 0;
}

void RNewAllocator::init_bins(mstate m) {
  /* Establish circular links for smallbins */
  bindex_t i;
  for (i = 0; i < NSMALLBINS; ++i) {
    sbinptr bin = smallbin_at(m,i);
    bin->fd = bin->bk = bin;
  }
}
/* ---------------------------- malloc support --------------------------- */

/* allocate a large request from the best fitting chunk in a treebin */
void* RNewAllocator::tmalloc_large(mstate m, size_t nb) {
  tchunkptr v = 0;
  size_t rsize = -nb; /* Unsigned negation */
  tchunkptr t;
  bindex_t idx;
  compute_tree_index(nb, idx);

  if ((t = *treebin_at(m, idx)) != 0) {
    /* Traverse tree for this bin looking for node with size == nb */
    size_t sizebits =
    nb <<
    leftshift_for_tree_index(idx);
    tchunkptr rst = 0;  /* The deepest untaken right subtree */
    for (;;) {
      tchunkptr rt;
      size_t trem = chunksize(t) - nb;
      if (trem < rsize) {
        v = t;
        if ((rsize = trem) == 0)
          break;
      }
      rt = t->child[1];
      t = t->child[(sizebits >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1];
      if (rt != 0 && rt != t)
        rst = rt;
      if (t == 0) {
        t = rst; /* set t to least subtree holding sizes > nb */
        break;
      }
      sizebits <<= 1;
    }
  }
  if (t == 0 && v == 0) { /* set t to root of next non-empty treebin */
    binmap_t leftbits = left_bits(idx2bit(idx)) & m->treemap;
    if (leftbits != 0) {
      bindex_t i;
      binmap_t leastbit = least_bit(leftbits);
      compute_bit2idx(leastbit, i);
      t = *treebin_at(m, i);
    }
  }
  while (t != 0) { /* find smallest of tree or subtree */
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
    t = leftmost_child(t);
  }
  /*  If dv is a better fit, return 0 so malloc will use it */
  if (v != 0 && rsize < (size_t)(m->dvsize - nb)) {
    if (RTCHECK(ok_address(m, v))) { /* split */
      mchunkptr r = chunk_plus_offset(v, nb);
      assert(chunksize(v) == rsize + nb);
      if (RTCHECK(ok_next(v, r))) {
        unlink_large_chunk(m, v);
        if (rsize < MIN_CHUNK_SIZE)
          set_inuse_and_pinuse(m, v, (rsize + nb));
        else {
          set_size_and_pinuse_of_inuse_chunk(m, v, nb);
          set_size_and_pinuse_of_free_chunk(r, rsize);
          insert_chunk(m, r, rsize);
        }
        return chunk2mem(v);
      }
    }
    CORRUPTION_ERROR_ACTION(m);
  }
  return 0;
}

/* allocate a small request from the best fitting chunk in a treebin */
void* RNewAllocator::tmalloc_small(mstate m, size_t nb) {
  tchunkptr t, v;
  size_t rsize;
  bindex_t i;
  binmap_t leastbit = least_bit(m->treemap);
  compute_bit2idx(leastbit, i);

  v = t = *treebin_at(m, i);
  rsize = chunksize(t) - nb;

  while ((t = leftmost_child(t)) != 0) {
    size_t trem = chunksize(t) - nb;
    if (trem < rsize) {
      rsize = trem;
      v = t;
    }
  }

  if (RTCHECK(ok_address(m, v))) {
    mchunkptr r = chunk_plus_offset(v, nb);
    assert(chunksize(v) == rsize + nb);
    if (RTCHECK(ok_next(v, r))) {
      unlink_large_chunk(m, v);
      if (rsize < MIN_CHUNK_SIZE)
        set_inuse_and_pinuse(m, v, (rsize + nb));
      else {
        set_size_and_pinuse_of_inuse_chunk(m, v, nb);
        set_size_and_pinuse_of_free_chunk(r, rsize);
        replace_dv(m, r, rsize);
      }
      return chunk2mem(v);
    }
  }
  CORRUPTION_ERROR_ACTION(m);
  return 0;
}

void RNewAllocator::init_top(mstate m, mchunkptr p, size_t psize)
{
	/* Ensure alignment */
	size_t offset = align_offset(chunk2mem(p));
	p = (mchunkptr)((TUint8*)p + offset);
	psize -= offset;
	m->top = p;
	m->topsize = psize;
	p->head = psize | PINUSE_BIT;
	/* set size of fake trailing chunk holding overhead space only once */
	mchunkptr chunkPlusOff = chunk_plus_offset(p, psize);
	chunkPlusOff->head = TOP_FOOT_SIZE;
	m->trim_check = mparams.trim_threshold; /* reset on each update */
}

void* RNewAllocator::internal_realloc(mstate m, void* oldmem, size_t bytes)
{
  if (bytes >= MAX_REQUEST) {
    MALLOC_FAILURE_ACTION;
    return 0;
  }
  if (!PREACTION(m)) {
    mchunkptr oldp = mem2chunk(oldmem);
    size_t oldsize = chunksize(oldp);
    mchunkptr next = chunk_plus_offset(oldp, oldsize);
    mchunkptr newp = 0;
    void* extra = 0;

    /* Try to either shrink or extend into top. Else malloc-copy-free */

    if (RTCHECK(ok_address(m, oldp) && ok_cinuse(oldp) &&
                ok_next(oldp, next) && ok_pinuse(next))) {
      size_t nb = request2size(bytes);
      if (is_mmapped(oldp))
        newp = mmap_resize(m, oldp, nb);
      else
	  if (oldsize >= nb) { /* already big enough */
        size_t rsize = oldsize - nb;
        newp = oldp;
        if (rsize >= MIN_CHUNK_SIZE) {
          mchunkptr remainder = chunk_plus_offset(newp, nb);
          set_inuse(m, newp, nb);
          set_inuse(m, remainder, rsize);
          extra = chunk2mem(remainder);
        }
      }
		/*AMOD: Modified to optimized*/
		else if (next == m->top && oldsize + m->topsize > nb)
		{
			/* Expand into top */
			if(oldsize + m->topsize > nb)
			{
		        size_t newsize = oldsize + m->topsize;
		        size_t newtopsize = newsize - nb;
		        mchunkptr newtop = chunk_plus_offset(oldp, nb);
		        set_inuse(m, oldp, nb);
		        newtop->head = newtopsize |PINUSE_BIT;
		        m->top = newtop;
		        m->topsize = newtopsize;
		        newp = oldp;
			}
      }
    }
    else {
      USAGE_ERROR_ACTION(m, oldmem);
      POSTACTION(m);
      return 0;
    }

    POSTACTION(m);

    if (newp != 0) {
      if (extra != 0) {
        internal_free(m, extra);
      }
      check_inuse_chunk(m, newp);
      return chunk2mem(newp);
    }
    else {
      void* newmem = internal_malloc(m, bytes);
      if (newmem != 0) {
        size_t oc = oldsize - overhead_for(oldp);
        memcpy(newmem, oldmem, (oc < bytes)? oc : bytes);
        internal_free(m, oldmem);
      }
      return newmem;
    }
  }
  return 0;
}
/* ----------------------------- statistics ------------------------------ */
mallinfo RNewAllocator::internal_mallinfo(mstate m) {
  struct mallinfo nm = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  TInt chunkCnt = 0;
  if (!PREACTION(m)) {
    check_malloc_state(m);
    if (is_initialized(m)) {
      size_t nfree = SIZE_T_ONE; /* top always free */
      size_t mfree = m->topsize + TOP_FOOT_SIZE;
      size_t sum = mfree;
      msegmentptr s = &m->seg;
      while (s != 0) {
        mchunkptr q = align_as_chunk(s->base);
        chunkCnt++;
        while (segment_holds(s, q) &&
               q != m->top && q->head != FENCEPOST_HEAD) {
          size_t sz = chunksize(q);
          sum += sz;
          if (!cinuse(q)) {
            if (sz > nm.largestBlock)
              nm.largestBlock = sz;
            mfree += sz;
            ++nfree;
          }
          q = next_chunk(q);
        }
        s = s->next;
      }
      nm.arena    = sum;
      nm.ordblks  = nfree;
      nm.hblkhd   = m->footprint - sum;
      nm.usmblks  = m->max_footprint;
      nm.uordblks = m->footprint - mfree;
      nm.fordblks = mfree;
      nm.keepcost = m->topsize;
      nm.cellCount= chunkCnt;/*number of chunks allocated*/
    }
    POSTACTION(m);
  }
  return nm;
}

void  RNewAllocator::internal_malloc_stats(mstate m) {
if (!PREACTION(m)) {
  size_t fp = 0;
  size_t used = 0;
  check_malloc_state(m);
  if (is_initialized(m)) {
    msegmentptr s = &m->seg;
    size_t maxfp = m->max_footprint;
    fp = m->footprint;
    used = fp - (m->topsize + TOP_FOOT_SIZE);

    while (s != 0) {
      mchunkptr q = align_as_chunk(s->base);
      while (segment_holds(s, q) &&
             q != m->top && q->head != FENCEPOST_HEAD) {
        if (!cinuse(q))
          used -= chunksize(q);
        q = next_chunk(q);
      }
      s = s->next;
    }
  }
  POSTACTION(m);
}
}
/* support for mallopt */
int RNewAllocator::change_mparam(int param_number, int value) {
  size_t val = (size_t)value;
  init_mparams(DEFAULT_TRIM_THRESHOLD);
  switch(param_number) {
  case M_TRIM_THRESHOLD:
    mparams.trim_threshold = val;
    return 1;
  case M_GRANULARITY:
    if (val >= mparams.page_size && ((val & (val-1)) == 0)) {
      mparams.granularity = val;
      return 1;
    }
    else
      return 0;
  case M_MMAP_THRESHOLD:
    mparams.mmap_threshold = val;
    return 1;
  default:
    return 0;
  }
}
/* Get memory from system using MORECORE or MMAP */
void* RNewAllocator::sys_alloc(mstate m, size_t nb)
{
	TUint8* tbase = CMFAIL;
	size_t tsize = 0;
	flag_t mmap_flag = 0;
	//init_mparams();/*No need to do init_params here*/
	/* Directly map large chunks */
	if (use_mmap(m) && nb >= mparams.mmap_threshold)
	{
		void* mem = mmap_alloc(m, nb);
		if (mem != 0)
			return mem;
	}
  /*
    Try getting memory in any of three ways (in most-preferred to
    least-preferred order):
    1. A call to MORECORE that can normally contiguously extend memory.
       (disabled if not MORECORE_CONTIGUOUS or not HAVE_MORECORE or
       or main space is mmapped or a previous contiguous call failed)
    2. A call to MMAP new space (disabled if not HAVE_MMAP).
       Note that under the default settings, if MORECORE is unable to
       fulfill a request, and HAVE_MMAP is true, then mmap is
       used as a noncontiguous system allocator. This is a useful backup
       strategy for systems with holes in address spaces -- in this case
       sbrk cannot contiguously expand the heap, but mmap may be able to
       find space.
    3. A call to MORECORE that cannot usually contiguously extend memory.
       (disabled if not HAVE_MORECORE)
  */
  /*Trying to allocate the memory*/
	if(MORECORE_CONTIGUOUS && !use_noncontiguous(m))
	{
	TUint8* br = CMFAIL;
    msegmentptr ss = (m->top == 0)? 0 : segment_holding(m, (TUint8*)m->top);
    size_t asize = 0;
    ACQUIRE_MORECORE_LOCK(m);
    if (ss == 0)
	{  /* First time through or recovery */
		TUint8* base = (TUint8*)CALL_MORECORE(0);
		if (base != CMFAIL)
		{
			asize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE);
			/* Adjust to end on a page boundary */
			if (!is_page_aligned(base))
				asize += (page_align((size_t)base) - (size_t)base);
			/* Can't call MORECORE if size is negative when treated as signed */
			if (asize < HALF_MAX_SIZE_T &&(br = (TUint8*)(CALL_MORECORE(asize))) == base)
			{
				tbase = base;
				tsize = asize;
			}
		}
    }
    else
	{
      /* Subtract out existing available top space from MORECORE request. */
		asize = granularity_align(nb - m->topsize + TOP_FOOT_SIZE + SIZE_T_ONE);
    /* Use mem here only if it did continuously extend old space */
      if (asize < HALF_MAX_SIZE_T &&
          (br = (TUint8*)(CALL_MORECORE(asize))) == ss->base+ss->size) {
        tbase = br;
        tsize = asize;
      }
    }
    if (tbase == CMFAIL) {    /* Cope with partial failure */
      if (br != CMFAIL) {    /* Try to use/extend the space we did get */
        if (asize < HALF_MAX_SIZE_T &&
            asize < nb + TOP_FOOT_SIZE + SIZE_T_ONE) {
          size_t esize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE - asize);
          if (esize < HALF_MAX_SIZE_T) {
            TUint8* end = (TUint8*)CALL_MORECORE(esize);
            if (end != CMFAIL)
              asize += esize;
            else {            /* Can't use; try to release */
              CALL_MORECORE(-asize);
              br = CMFAIL;
            }
          }
        }
      }
      if (br != CMFAIL) {    /* Use the space we did get */
        tbase = br;
        tsize = asize;
      }
      else
        disable_contiguous(m); /* Don't try contiguous path in the future */
    }
    RELEASE_MORECORE_LOCK(m);
  }
  if (HAVE_MMAP && tbase == CMFAIL) {  /* Try MMAP */
    size_t req = nb + TOP_FOOT_SIZE + SIZE_T_ONE;
    size_t rsize = granularity_align(req);
    if (rsize > nb) { /* Fail if wraps around zero */
      TUint8* mp = (TUint8*)(CALL_MMAP(rsize));
      if (mp != CMFAIL) {
        tbase = mp;
        tsize = rsize;
        mmap_flag = IS_MMAPPED_BIT;
      }
    }
  }
  if (HAVE_MORECORE && tbase == CMFAIL) { /* Try noncontiguous MORECORE */
    size_t asize = granularity_align(nb + TOP_FOOT_SIZE + SIZE_T_ONE);
    if (asize < HALF_MAX_SIZE_T) {
      TUint8* br = CMFAIL;
      TUint8* end = CMFAIL;
      ACQUIRE_MORECORE_LOCK(m);
      br = (TUint8*)(CALL_MORECORE(asize));
      end = (TUint8*)(CALL_MORECORE(0));
      RELEASE_MORECORE_LOCK(m);
      if (br != CMFAIL && end != CMFAIL && br < end) {
        size_t ssize = end - br;
        if (ssize > nb + TOP_FOOT_SIZE) {
          tbase = br;
          tsize = ssize;
        }
      }
    }
  }
  if (tbase != CMFAIL) {
    if ((m->footprint += tsize) > m->max_footprint)
      m->max_footprint = m->footprint;
    if (!is_initialized(m)) { /* first-time initialization */
      m->seg.base = m->least_addr = tbase;
      m->seg.size = tsize;
      m->seg.sflags = mmap_flag;
      m->magic = mparams.magic;
      init_bins(m);
      if (is_global(m))
        init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);
      else {
        /* Offset top by embedded malloc_state */
        mchunkptr mn = next_chunk(mem2chunk(m));
        init_top(m, mn, (size_t)((tbase + tsize) - (TUint8*)mn) -TOP_FOOT_SIZE);
      }
    }else {
      /* Try to merge with an existing segment */
      msegmentptr sp = &m->seg;
      while (sp != 0 && tbase != sp->base + sp->size)
        sp = sp->next;
      if (sp != 0 && !is_extern_segment(sp) &&
          (sp->sflags & IS_MMAPPED_BIT) == mmap_flag &&
          segment_holds(sp, m->top))
    	  { /* append */
        sp->size += tsize;
        init_top(m, m->top, m->topsize + tsize);
      }
      else {
        if (tbase < m->least_addr)
          m->least_addr = tbase;
        sp = &m->seg;
        while (sp != 0 && sp->base != tbase + tsize)
          sp = sp->next;
        if (sp != 0 &&
            !is_extern_segment(sp) &&
            (sp->sflags & IS_MMAPPED_BIT) == mmap_flag) {
          TUint8* oldbase = sp->base;
          sp->base = tbase;
          sp->size += tsize;
          return prepend_alloc(m, tbase, oldbase, nb);
        }
        else
          add_segment(m, tbase, tsize, mmap_flag);
      }
    }
    if (nb < m->topsize) { /* Allocate from new or extended top space */
      size_t rsize = m->topsize -= nb;
      mchunkptr p = m->top;
      mchunkptr r = m->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(m, p, nb);
      check_top_chunk(m, m->top);
      check_malloced_chunk(m, chunk2mem(p), nb);
      return chunk2mem(p);
    }
  }
  /*need to check this*/
  //errno = -1;
  return 0;
}
msegmentptr RNewAllocator::segment_holding(mstate m, TUint8* addr) {
  msegmentptr sp = &m->seg;
  for (;;) {
    if (addr >= sp->base && addr < sp->base + sp->size)
      return sp;
    if ((sp = sp->next) == 0)
      return 0;
  }
}
/* Unlink the first chunk from a smallbin */
inline void RNewAllocator::unlink_first_small_chunk(mstate M,mchunkptr B,mchunkptr P,bindex_t& I)
{
  mchunkptr F = P->fd;
  assert(P != B);
  assert(P != F);
  assert(chunksize(P) == small_index2size(I));
  if (B == F)
    clear_smallmap(M, I);
  else if (RTCHECK(ok_address(M, F))) {
    B->fd = F;
    F->bk = B;
  }
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
}
/* Link a free chunk into a smallbin  */
inline void RNewAllocator::insert_small_chunk(mstate M,mchunkptr P, size_t S)
{
  bindex_t I  = small_index(S);
  mchunkptr B = smallbin_at(M, I);
  mchunkptr F = B;
  assert(S >= MIN_CHUNK_SIZE);
  if (!smallmap_is_marked(M, I))
    mark_smallmap(M, I);
  else if (RTCHECK(ok_address(M, B->fd)))
    F = B->fd;
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
  B->fd = P;
  F->bk = P;
  P->fd = F;
  P->bk = B;
}


inline void RNewAllocator::insert_chunk(mstate M,mchunkptr P,size_t S)
{
	if (is_small(S))
		insert_small_chunk(M, P, S);
	else{
		tchunkptr TP = (tchunkptr)(P); insert_large_chunk(M, TP, S);
	 }
}

inline void RNewAllocator::unlink_large_chunk(mstate M,tchunkptr X)
{
  tchunkptr XP = X->parent;
  tchunkptr R;
  if (X->bk != X) {
    tchunkptr F = X->fd;
    R = X->bk;
    if (RTCHECK(ok_address(M, F))) {
      F->bk = R;
      R->fd = F;
    }
    else {
      CORRUPTION_ERROR_ACTION(M);
    }
  }
  else {
    tchunkptr* RP;
    if (((R = *(RP = &(X->child[1]))) != 0) ||
        ((R = *(RP = &(X->child[0]))) != 0)) {
      tchunkptr* CP;
      while ((*(CP = &(R->child[1])) != 0) ||
             (*(CP = &(R->child[0])) != 0)) {
        R = *(RP = CP);
      }
      if (RTCHECK(ok_address(M, RP)))
        *RP = 0;
      else {
        CORRUPTION_ERROR_ACTION(M);
      }
    }
  }
  if (XP != 0) {
    tbinptr* H = treebin_at(M, X->index);
    if (X == *H) {
      if ((*H = R) == 0)
        clear_treemap(M, X->index);
    }
    else if (RTCHECK(ok_address(M, XP))) {
      if (XP->child[0] == X)
        XP->child[0] = R;
      else
        XP->child[1] = R;
    }
    else
      CORRUPTION_ERROR_ACTION(M);
    if (R != 0) {
      if (RTCHECK(ok_address(M, R))) {
        tchunkptr C0, C1;
        R->parent = XP;
        if ((C0 = X->child[0]) != 0) {
          if (RTCHECK(ok_address(M, C0))) {
            R->child[0] = C0;
            C0->parent = R;
          }
          else
            CORRUPTION_ERROR_ACTION(M);
        }
        if ((C1 = X->child[1]) != 0) {
          if (RTCHECK(ok_address(M, C1))) {
            R->child[1] = C1;
            C1->parent = R;
          }
          else
            CORRUPTION_ERROR_ACTION(M);
        }
      }
      else
        CORRUPTION_ERROR_ACTION(M);
    }
  }
}

/* Unlink a chunk from a smallbin  */
inline void RNewAllocator::unlink_small_chunk(mstate M, mchunkptr P,size_t S)
{
  mchunkptr F = P->fd;
  mchunkptr B = P->bk;
  bindex_t I = small_index(S);
  assert(P != B);
  assert(P != F);
  assert(chunksize(P) == small_index2size(I));
  if (F == B)
    clear_smallmap(M, I);
  else if (RTCHECK((F == smallbin_at(M,I) || ok_address(M, F)) &&
                   (B == smallbin_at(M,I) || ok_address(M, B)))) {
    F->bk = B;
    B->fd = F;
  }
  else {
    CORRUPTION_ERROR_ACTION(M);
  }
}

inline void RNewAllocator::unlink_chunk(mstate M, mchunkptr P, size_t S)
{
  if (is_small(S))
	unlink_small_chunk(M, P, S);
  else
  {
	  tchunkptr TP = (tchunkptr)(P); unlink_large_chunk(M, TP);
  }
}

inline void RNewAllocator::compute_tree_index(size_t S, bindex_t& I)
{
  size_t X = S >> TREEBIN_SHIFT;
  if (X == 0)
    I = 0;
  else if (X > 0xFFFF)
    I = NTREEBINS-1;
  else {
    unsigned int Y = (unsigned int)X;
    unsigned int N = ((Y - 0x100) >> 16) & 8;
    unsigned int K = (((Y <<= N) - 0x1000) >> 16) & 4;
    N += K;
    N += K = (((Y <<= K) - 0x4000) >> 16) & 2;
    K = 14 - N + ((Y <<= K) >> 15);
    I = (K << 1) + ((S >> (K + (TREEBIN_SHIFT-1)) & 1));
  }
}

/* ------------------------- Operations on trees ------------------------- */

/* Insert chunk into tree */
inline void RNewAllocator::insert_large_chunk(mstate M,tchunkptr X,size_t S)
{
  tbinptr* H;
  bindex_t I;
  compute_tree_index(S, I);
  H = treebin_at(M, I);
  X->index = I;
  X->child[0] = X->child[1] = 0;
  if (!treemap_is_marked(M, I)) {
    mark_treemap(M, I);
    *H = X;
    X->parent = (tchunkptr)H;
    X->fd = X->bk = X;
  }
  else {
    tchunkptr T = *H;
    size_t K = S << leftshift_for_tree_index(I);
    for (;;) {
      if (chunksize(T) != S) {
        tchunkptr* C = &(T->child[(K >> (SIZE_T_BITSIZE-SIZE_T_ONE)) & 1]);
        K <<= 1;
        if (*C != 0)
          T = *C;
        else if (RTCHECK(ok_address(M, C))) {
          *C = X;
          X->parent = T;
          X->fd = X->bk = X;
          break;
        }
        else {
          CORRUPTION_ERROR_ACTION(M);
          break;
        }
      }
      else {
        tchunkptr F = T->fd;
        if (RTCHECK(ok_address(M, T) && ok_address(M, F))) {
          T->fd = F->bk = X;
          X->fd = F;
          X->bk = T;
          X->parent = 0;
          break;
        }
        else {
          CORRUPTION_ERROR_ACTION(M);
          break;
        }
      }
    }
  }
}

/*
  Unlink steps:

  1. If x is a chained node, unlink it from its same-sized fd/bk links
     and choose its bk node as its replacement.
  2. If x was the last node of its size, but not a leaf node, it must
     be replaced with a leaf node (not merely one with an open left or
     right), to make sure that lefts and rights of descendents
     correspond properly to bit masks.  We use the rightmost descendent
     of x.  We could use any other leaf, but this is easy to locate and
     tends to counteract removal of leftmosts elsewhere, and so keeps
     paths shorter than minimally guaranteed.  This doesn't loop much
     because on average a node in a tree is near the bottom.
  3. If x is the base of a chain (i.e., has parent links) relink
     x's parent and children to x's replacement (or null if none).
*/

/* Replace dv node, binning the old one */
/* Used only when dvsize known to be small */
inline void RNewAllocator::replace_dv(mstate M, mchunkptr P, size_t S)
{
  size_t DVS = M->dvsize;
  if (DVS != 0) {
    mchunkptr DV = M->dv;
    assert(is_small(DVS));
    insert_small_chunk(M, DV, DVS);
  }
  M->dvsize = S;
  M->dv = P;
}

inline void RNewAllocator::compute_bit2idx(binmap_t X,bindex_t& I)
{
	unsigned int Y = X - 1;
	unsigned int K = Y >> (16-4) & 16;
	unsigned int N = K;        Y >>= K;
	N += K = Y >> (8-3) &  8;  Y >>= K;
	N += K = Y >> (4-2) &  4;  Y >>= K;
	N += K = Y >> (2-1) &  2;  Y >>= K;
	N += K = Y >> (1-0) &  1;  Y >>= K;
	I = (bindex_t)(N + Y);
}

void RNewAllocator::add_segment(mstate m, TUint8* tbase, size_t tsize, flag_t mmapped) {
  /* Determine locations and sizes of segment, fenceposts, old top */
  TUint8* old_top = (TUint8*)m->top;
  msegmentptr oldsp = segment_holding(m, old_top);
  TUint8* old_end = oldsp->base + oldsp->size;
  size_t ssize = pad_request(sizeof(struct malloc_segment));
  TUint8* rawsp = old_end - (ssize + FOUR_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  size_t offset = align_offset(chunk2mem(rawsp));
  TUint8* asp = rawsp + offset;
  TUint8* csp = (asp < (old_top + MIN_CHUNK_SIZE))? old_top : asp;
  mchunkptr sp = (mchunkptr)csp;
  msegmentptr ss = (msegmentptr)(chunk2mem(sp));
  mchunkptr tnext = chunk_plus_offset(sp, ssize);
  mchunkptr p = tnext;
  int nfences = 0;

  /* reset top to new space */
  init_top(m, (mchunkptr)tbase, tsize - TOP_FOOT_SIZE);

  /* Set up segment record */
  assert(is_aligned(ss));
  set_size_and_pinuse_of_inuse_chunk(m, sp, ssize);
  *ss = m->seg; /* Push current record */
  m->seg.base = tbase;
  m->seg.size = tsize;
  m->seg.sflags = mmapped;
  m->seg.next = ss;

  /* Insert trailing fenceposts */
  for (;;) {
    mchunkptr nextp = chunk_plus_offset(p, SIZE_T_SIZE);
    p->head = FENCEPOST_HEAD;
    ++nfences;
    if ((TUint8*)(&(nextp->head)) < old_end)
      p = nextp;
    else
      break;
  }
  assert(nfences >= 2);

  /* Insert the rest of old top into a bin as an ordinary free chunk */
  if (csp != old_top) {
    mchunkptr q = (mchunkptr)old_top;
    size_t psize = csp - old_top;
    mchunkptr tn = chunk_plus_offset(q, psize);
    set_free_with_pinuse(q, psize, tn);
    insert_chunk(m, q, psize);
  }

  check_top_chunk(m, m->top);
}


void* RNewAllocator::prepend_alloc(mstate m, TUint8* newbase, TUint8* oldbase,
                           size_t nb) {
  mchunkptr p = align_as_chunk(newbase);
  mchunkptr oldfirst = align_as_chunk(oldbase);
  size_t psize = (TUint8*)oldfirst - (TUint8*)p;
  mchunkptr q = chunk_plus_offset(p, nb);
  size_t qsize = psize - nb;
  set_size_and_pinuse_of_inuse_chunk(m, p, nb);

  assert((TUint8*)oldfirst > (TUint8*)q);
  assert(pinuse(oldfirst));
  assert(qsize >= MIN_CHUNK_SIZE);

  /* consolidate remainder with first chunk of old base */
  if (oldfirst == m->top) {
    size_t tsize = m->topsize += qsize;
    m->top = q;
    q->head = tsize | PINUSE_BIT;
    check_top_chunk(m, q);
  }
  else if (oldfirst == m->dv) {
    size_t dsize = m->dvsize += qsize;
    m->dv = q;
    set_size_and_pinuse_of_free_chunk(q, dsize);
  }
  else {
    if (!cinuse(oldfirst)) {
      size_t nsize = chunksize(oldfirst);
      unlink_chunk(m, oldfirst, nsize);
      oldfirst = chunk_plus_offset(oldfirst, nsize);
      qsize += nsize;
    }
    set_free_with_pinuse(q, qsize, oldfirst);
    insert_chunk(m, q, qsize);
    check_free_chunk(m, q);
  }

  check_malloced_chunk(m, chunk2mem(p), nb);
  return chunk2mem(p);
}

void* RNewAllocator::mmap_alloc(mstate m, size_t nb) {
  size_t mmsize = granularity_align(nb + SIX_SIZE_T_SIZES + CHUNK_ALIGN_MASK);
  if (mmsize > nb) {     /* Check for wrap around 0 */
    TUint8* mm = (TUint8*)(DIRECT_MMAP(mmsize));
    if (mm != CMFAIL) {
      size_t offset = align_offset(chunk2mem(mm));
      size_t psize = mmsize - offset - MMAP_FOOT_PAD;
      mchunkptr p = (mchunkptr)(mm + offset);
      p->prev_foot = offset | IS_MMAPPED_BIT;
      (p)->head = (psize|CINUSE_BIT);
      mark_inuse_foot(m, p, psize);
      chunk_plus_offset(p, psize)->head = FENCEPOST_HEAD;
      chunk_plus_offset(p, psize+SIZE_T_SIZE)->head = 0;

      if (mm < m->least_addr)
        m->least_addr = mm;
      if ((m->footprint += mmsize) > m->max_footprint)
        m->max_footprint = m->footprint;
      assert(is_aligned(chunk2mem(p)));
      check_mmapped_chunk(m, p);
      return chunk2mem(p);
    }
  }
  return 0;
}

	int RNewAllocator::sys_trim(mstate m, size_t pad)
	{
	  size_t released = 0;
	  if (pad < MAX_REQUEST && is_initialized(m)) {
	    pad += TOP_FOOT_SIZE; /* ensure enough room for segment overhead */

	    if (m->topsize > pad) {
	      /* Shrink top space in granularity-size units, keeping at least one */
	      size_t unit = mparams.granularity;
				size_t extra = ((m->topsize - pad + (unit - SIZE_T_ONE)) / unit - SIZE_T_ONE) * unit;
	      msegmentptr sp = segment_holding(m, (TUint8*)m->top);

	      if (!is_extern_segment(sp)) {
	        if (is_mmapped_segment(sp)) {
	          if (HAVE_MMAP &&
	              sp->size >= extra &&
	              !has_segment_link(m, sp)) { /* can't shrink if pinned */
	            size_t newsize = sp->size - extra;
	            /* Prefer mremap, fall back to munmap */
	            if ((CALL_MREMAP(sp->base, sp->size, newsize, 0) != MFAIL) ||
	                (CALL_MUNMAP(sp->base + newsize, extra) == 0)) {
	              released = extra;
	            }
	          }
	        }
	        else if (HAVE_MORECORE) {
	          if (extra >= HALF_MAX_SIZE_T) /* Avoid wrapping negative */
	            extra = (HALF_MAX_SIZE_T) + SIZE_T_ONE - unit;
	          ACQUIRE_MORECORE_LOCK(m);
	          {
	            /* Make sure end of memory is where we last set it. */
	            TUint8* old_br = (TUint8*)(CALL_MORECORE(0));
	            if (old_br == sp->base + sp->size) {
	              TUint8* rel_br = (TUint8*)(CALL_MORECORE(-extra));
	              TUint8* new_br = (TUint8*)(CALL_MORECORE(0));
	              if (rel_br != CMFAIL && new_br < old_br)
	                released = old_br - new_br;
	            }
	          }
	          RELEASE_MORECORE_LOCK(m);
	        }
	      }

	      if (released != 0) {
	        sp->size -= released;
	        m->footprint -= released;
	        init_top(m, m->top, m->topsize - released);
	        check_top_chunk(m, m->top);
	      }
	    }

	    /* Unmap any unused mmapped segments */
	    if (HAVE_MMAP)
	      released += release_unused_segments(m);

	    /* On failure, disable autotrim to avoid repeated failed future calls */
	    if (released == 0)
	      m->trim_check = MAX_SIZE_T;
	  }

	  return (released != 0)? 1 : 0;
	}

	inline int RNewAllocator::has_segment_link(mstate m, msegmentptr ss)
	{
	  msegmentptr sp = &m->seg;
	  for (;;) {
	    if ((TUint8*)sp >= ss->base && (TUint8*)sp < ss->base + ss->size)
	      return 1;
	    if ((sp = sp->next) == 0)
	      return 0;
	  }
	}

	/* Unmap and unlink any mmapped segments that don't contain used chunks */
	size_t RNewAllocator::release_unused_segments(mstate m)
	{
	  size_t released = 0;
	  msegmentptr pred = &m->seg;
	  msegmentptr sp = pred->next;
	  while (sp != 0) {
	    TUint8* base = sp->base;
	    size_t size = sp->size;
	    msegmentptr next = sp->next;
	    if (is_mmapped_segment(sp) && !is_extern_segment(sp)) {
	      mchunkptr p = align_as_chunk(base);
	      size_t psize = chunksize(p);
	      /* Can unmap if first chunk holds entire segment and not pinned */
	      if (!cinuse(p) && (TUint8*)p + psize >= base + size - TOP_FOOT_SIZE) {
	        tchunkptr tp = (tchunkptr)p;
	        assert(segment_holds(sp, (TUint8*)sp));
	        if (p == m->dv) {
	          m->dv = 0;
	          m->dvsize = 0;
	        }
	        else {
	          unlink_large_chunk(m, tp);
	        }
	        if (CALL_MUNMAP(base, size) == 0) {
	          released += size;
	          m->footprint -= size;
	          /* unlink obsoleted record */
	          sp = pred;
	          sp->next = next;
	        }
	        else { /* back out if cannot unmap */
	          insert_large_chunk(m, tp, psize);
	        }
	      }
	    }
	    pred = sp;
	    sp = next;
	  }/*End of while*/
	  return released;
	}
	/* Realloc using mmap */
	inline	mchunkptr RNewAllocator::mmap_resize(mstate m, mchunkptr oldp, size_t nb)
	{
	  size_t oldsize = chunksize(oldp);
	  if (is_small(nb)) /* Can't shrink mmap regions below small size */
	    return 0;
	  /* Keep old chunk if big enough but not too big */
	  if (oldsize >= nb + SIZE_T_SIZE &&
	      (oldsize - nb) <= (mparams.granularity << 1))
	    return oldp;
	  else {
	    size_t offset = oldp->prev_foot & ~IS_MMAPPED_BIT;
	    size_t oldmmsize = oldsize + offset + MMAP_FOOT_PAD;
	    size_t newmmsize = granularity_align(nb + SIX_SIZE_T_SIZES +
	                                         CHUNK_ALIGN_MASK);
	    TUint8* cp = (TUint8*)CALL_MREMAP((char*)oldp - offset,
	                                  oldmmsize, newmmsize, 1);
	    if (cp != CMFAIL) {
	      mchunkptr newp = (mchunkptr)(cp + offset);
	      size_t psize = newmmsize - offset - MMAP_FOOT_PAD;
	      newp->head = (psize|CINUSE_BIT);
	      mark_inuse_foot(m, newp, psize);
	      chunk_plus_offset(newp, psize)->head = FENCEPOST_HEAD;
	      chunk_plus_offset(newp, psize+SIZE_T_SIZE)->head = 0;

	      if (cp < m->least_addr)
	        m->least_addr = cp;
	      if ((m->footprint += newmmsize - oldmmsize) > m->max_footprint)
	        m->max_footprint = m->footprint;
	      check_mmapped_chunk(m, newp);
	      return newp;
	    }
	  }
	  return 0;
	}


void RNewAllocator::Init_Dlmalloc(size_t capacity, int locked, size_t aTrimThreshold)
	{
		memset(gm,0,sizeof(malloc_state));
		init_mparams(aTrimThreshold); /* Ensure pagesize etc initialized */
		// The maximum amount that can be allocated can be calculated as:-
		// 2^sizeof(size_t) - sizeof(malloc_state) - TOP_FOOT_SIZE - page size (all accordingly padded)
		// If the capacity exceeds this, no allocation will be done.
		gm->seg.base = gm->least_addr = iBase;
		gm->seg.size = capacity;
		gm->seg.sflags = !IS_MMAPPED_BIT;
		set_lock(gm, locked);
		gm->magic = mparams.magic;
		init_bins(gm);
		init_top(gm, (mchunkptr)iBase, capacity - TOP_FOOT_SIZE);
	}

void* RNewAllocator::dlmalloc(size_t bytes) {
  /*
     Basic algorithm:
     If a small request (< 256 bytes minus per-chunk overhead):
       1. If one exists, use a remainderless chunk in associated smallbin.
          (Remainderless means that there are too few excess bytes to
          represent as a chunk.)
       2. If it is big enough, use the dv chunk, which is normally the
          chunk adjacent to the one used for the most recent small request.
       3. If one exists, split the smallest available chunk in a bin,
          saving remainder in dv.
       4. If it is big enough, use the top chunk.
       5. If available, get memory from system and use it
     Otherwise, for a large request:
       1. Find the smallest available binned chunk that fits, and use it
          if it is better fitting than dv chunk, splitting if necessary.
       2. If better fitting than any binned chunk, use the dv chunk.
       3. If it is big enough, use the top chunk.
       4. If request size >= mmap threshold, try to directly mmap this chunk.
       5. If available, get memory from system and use it

     The ugly goto's here ensure that postaction occurs along all paths.
  */
  if (!PREACTION(gm)) {
    void* mem;
    size_t nb;
    if (bytes <= MAX_SMALL_REQUEST) {
      bindex_t idx;
      binmap_t smallbits;
      nb = (bytes < MIN_REQUEST)? MIN_CHUNK_SIZE : pad_request(bytes);
      idx = small_index(nb);
      smallbits = gm->smallmap >> idx;

      if ((smallbits & 0x3U) != 0) { /* Remainderless fit to a smallbin. */
        mchunkptr b, p;
        idx += ~smallbits & 1;       /* Uses next bin if idx empty */
        b = smallbin_at(gm, idx);
        p = b->fd;
        assert(chunksize(p) == small_index2size(idx));
        unlink_first_small_chunk(gm, b, p, idx);
        set_inuse_and_pinuse(gm, p, small_index2size(idx));
        mem = chunk2mem(p);
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }

      else if (nb > gm->dvsize) {
        if (smallbits != 0) { /* Use chunk in next nonempty smallbin */
          mchunkptr b, p, r;
          size_t rsize;
          bindex_t i;
          binmap_t leftbits = (smallbits << idx) & left_bits(idx2bit(idx));
          binmap_t leastbit = least_bit(leftbits);
          compute_bit2idx(leastbit, i);
          b = smallbin_at(gm, i);
          p = b->fd;
          assert(chunksize(p) == small_index2size(i));
          unlink_first_small_chunk(gm, b, p, i);
          rsize = small_index2size(i) - nb;
          /* Fit here cannot be remainderless if 4byte sizes */
          if (SIZE_T_SIZE != 4 && rsize < MIN_CHUNK_SIZE)
            set_inuse_and_pinuse(gm, p, small_index2size(i));
          else {
            set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
            r = chunk_plus_offset(p, nb);
            set_size_and_pinuse_of_free_chunk(r, rsize);
            replace_dv(gm, r, rsize);
          }
          mem = chunk2mem(p);
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }

        else if (gm->treemap != 0 && (mem = tmalloc_small(gm, nb)) != 0) {
          check_malloced_chunk(gm, mem, nb);
          goto postaction;
        }
      }
    }
    else if (bytes >= MAX_REQUEST)
      nb = MAX_SIZE_T; /* Too big to allocate. Force failure (in sys alloc) */
    else {
      nb = pad_request(bytes);
      if (gm->treemap != 0 && (mem = tmalloc_large(gm, nb)) != 0) {
        check_malloced_chunk(gm, mem, nb);
        goto postaction;
      }
    }

    if (nb <= gm->dvsize) {
      size_t rsize = gm->dvsize - nb;
      mchunkptr p = gm->dv;
      if (rsize >= MIN_CHUNK_SIZE) { /* split dv */
        mchunkptr r = gm->dv = chunk_plus_offset(p, nb);
        gm->dvsize = rsize;
        set_size_and_pinuse_of_free_chunk(r, rsize);
        set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      }
      else { /* exhaust dv */
        size_t dvs = gm->dvsize;
        gm->dvsize = 0;
        gm->dv = 0;
        set_inuse_and_pinuse(gm, p, dvs);
      }
      mem = chunk2mem(p);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    else if (nb < gm->topsize) { /* Split top */
      size_t rsize = gm->topsize -= nb;
      mchunkptr p = gm->top;
      mchunkptr r = gm->top = chunk_plus_offset(p, nb);
      r->head = rsize | PINUSE_BIT;
      set_size_and_pinuse_of_inuse_chunk(gm, p, nb);
      mem = chunk2mem(p);
      check_top_chunk(gm, gm->top);
      check_malloced_chunk(gm, mem, nb);
      goto postaction;
    }

    mem = sys_alloc(gm, nb);

  postaction:
    POSTACTION(gm);
    return mem;
  }

  return 0;
}

void RNewAllocator::dlfree(void* mem) {
  /*
     Consolidate freed chunks with preceeding or succeeding bordering
     free chunks, if they exist, and then place in a bin.  Intermixed
     with special cases for top, dv, mmapped chunks, and usage errors.
  */

	if (mem != 0)
	{
		mchunkptr p  = mem2chunk(mem);
#if FOOTERS
		mstate fm = get_mstate_for(p);
		if (!ok_magic(fm))
		{
			USAGE_ERROR_ACTION(fm, p);
			return;
		}
#else /* FOOTERS */
#define fm gm
#endif /* FOOTERS */

		if (!PREACTION(fm))
		{
			check_inuse_chunk(fm, p);
			if (RTCHECK(ok_address(fm, p) && ok_cinuse(p)))
			{
				size_t psize = chunksize(p);
				iTotalAllocSize -= psize;
				mchunkptr next = chunk_plus_offset(p, psize);
				if (!pinuse(p))
				{
					size_t prevsize = p->prev_foot;
					if ((prevsize & IS_MMAPPED_BIT) != 0)
					{
						prevsize &= ~IS_MMAPPED_BIT;
						psize += prevsize + MMAP_FOOT_PAD;
						/*TInt tmp = TOP_FOOT_SIZE;
						TUint8* top = (TUint8*)fm->top + fm->topsize + 40;
						if((top == (TUint8*)p)&& fm->topsize > 4096)
						{
							fm->topsize += psize;
							msegmentptr sp = segment_holding(fm, (TUint8*)fm->top);
							sp->size+=psize;
							if (should_trim(fm, fm->topsize))
								sys_trim(fm, 0);
 							goto postaction;
						}
						else*/
						{
							if (CALL_MUNMAP((char*)p - prevsize, psize) == 0)
								fm->footprint -= psize;
							goto postaction;
						}
					}
					else
					{
						mchunkptr prev = chunk_minus_offset(p, prevsize);
						psize += prevsize;
						p = prev;
						if (RTCHECK(ok_address(fm, prev)))
						{ /* consolidate backward */
							if (p != fm->dv)
							{
								unlink_chunk(fm, p, prevsize);
							}
							else if ((next->head & INUSE_BITS) == INUSE_BITS)
							{
								fm->dvsize = psize;
								set_free_with_pinuse(p, psize, next);
								goto postaction;
							}
						}
						else
							goto erroraction;
					}
				}

				if (RTCHECK(ok_next(p, next) && ok_pinuse(next)))
				{
					if (!cinuse(next))
					{  /* consolidate forward */
						if (next == fm->top)
						{
							size_t tsize = fm->topsize += psize;
							fm->top = p;
							p->head = tsize | PINUSE_BIT;
							if (p == fm->dv)
							{
								fm->dv = 0;
								fm->dvsize = 0;
							}
							if (should_trim(fm, tsize))
								sys_trim(fm, 0);
							goto postaction;
						}
						else if (next == fm->dv)
						{
							size_t dsize = fm->dvsize += psize;
							fm->dv = p;
							set_size_and_pinuse_of_free_chunk(p, dsize);
							goto postaction;
						}
						else
						{
							size_t nsize = chunksize(next);
							psize += nsize;
							unlink_chunk(fm, next, nsize);
							set_size_and_pinuse_of_free_chunk(p, psize);
							if (p == fm->dv)
							{
								fm->dvsize = psize;
								goto postaction;
							}
						}
					}
					else
						set_free_with_pinuse(p, psize, next);
					insert_chunk(fm, p, psize);
					check_free_chunk(fm, p);
					goto postaction;
				}
			}
erroraction:
    	USAGE_ERROR_ACTION(fm, p);
postaction:
    	POSTACTION(fm);
		}
	}
#if !FOOTERS
#undef fm
#endif /* FOOTERS */
}

void* RNewAllocator::dlrealloc(void* oldmem, size_t bytes) {
  if (oldmem == 0)
    return dlmalloc(bytes);
#ifdef REALLOC_ZERO_BYTES_FREES
  if (bytes == 0) {
    dlfree(oldmem);
    return 0;
  }
#endif /* REALLOC_ZERO_BYTES_FREES */
  else {
#if ! FOOTERS
    mstate m = gm;
#else /* FOOTERS */
    mstate m = get_mstate_for(mem2chunk(oldmem));
    if (!ok_magic(m)) {
      USAGE_ERROR_ACTION(m, oldmem);
      return 0;
    }
#endif /* FOOTERS */
    return internal_realloc(m, oldmem, bytes);
  }
}


int RNewAllocator::dlmalloc_trim(size_t pad) {
  int result = 0;
  if (!PREACTION(gm)) {
    result = sys_trim(gm, pad);
    POSTACTION(gm);
  }
  return result;
}

size_t RNewAllocator::dlmalloc_footprint(void) {
  return gm->footprint;
}

size_t RNewAllocator::dlmalloc_max_footprint(void) {
  return gm->max_footprint;
}

#if !NO_MALLINFO
struct mallinfo RNewAllocator::dlmallinfo(void) {
  return internal_mallinfo(gm);
}
#endif /* NO_MALLINFO */

void RNewAllocator::dlmalloc_stats() {
  internal_malloc_stats(gm);
}

int RNewAllocator::dlmallopt(int param_number, int value) {
  return change_mparam(param_number, value);
}

//inline slab* slab::slabfor(void* p)
inline slab* slab::slabfor( const void* p)
	{return (slab*)(floor(p, slabsize));}


void RNewAllocator::tree_remove(slab* s)
{
	slab** r = s->parent;
	slab* c1 = s->child1;
	slab* c2 = s->child2;
	for (;;)
	{
		if (!c2)
		{
			*r = c1;
			if (c1)
				c1->parent = r;
			return;
		}
		if (!c1)
		{
			*r = c2;
			c2->parent = r;
			return;
		}
		if (c1 > c2)
		{
			slab* c3 = c1;
			c1 = c2;
			c2 = c3;
		}
		slab* newc2 = c1->child2;
		*r = c1;
		c1->parent = r;
		c1->child2 = c2;
		c2->parent = &c1->child2;
		s = c1;
		c1 = s->child1;
		c2 = newc2;
		r = &s->child1;
	}
}
void RNewAllocator::tree_insert(slab* s,slab** r)
	{
		slab* n = *r;
		for (;;)
		{
			if (!n)
			{	// tree empty
				*r = s;
				s->parent = r;
				s->child1 = s->child2 = 0;
				break;
			}
			if (s < n)
			{	// insert between parent and n
				*r = s;
				s->parent = r;
				s->child1 = n;
				s->child2 = 0;
				n->parent = &s->child1;
				break;
			}
			slab* c1 = n->child1;
			slab* c2 = n->child2;
			if (c1 < c2)
			{
				r = &n->child1;
				n = c1;
			}
			else
			{
				r = &n->child2;
				n = c2;
			}
		}
	}
void* RNewAllocator::allocnewslab(slabset& allocator)
//
// Acquire and initialise a new slab, returning a cell from the slab
// The strategy is:
// 1. Use the lowest address free slab, if available. This is done by using the lowest slab
//    in the page at the root of the partial_page heap (which is address ordered). If the
//    is now fully used, remove it from the partial_page heap.
// 2. Allocate a new page for slabs if no empty slabs are available
//
{
	page* p = page::pagefor(partial_page);
	if (!p)
		return allocnewpage(allocator);

	unsigned h = p->slabs[0].header;
	unsigned pagemap = header_pagemap(h);
	ASSERT(&p->slabs[hibit(pagemap)] == partial_page);

	unsigned slabix = lowbit(pagemap);
	p->slabs[0].header = h &~ (0x100<<slabix);
	if (!(pagemap &~ (1<<slabix)))
	{
		tree_remove(partial_page);	// last free slab in page
	}
	return initnewslab(allocator,&p->slabs[slabix]);
}

/**Defination of this functionis not there in proto code***/
#if 0
void RNewAllocator::partial_insert(slab* s)
	{
		// slab has had first cell freed and needs to be linked back into partial tree
		slabset& ss = slaballoc[sizemap[s->clz]];

		ASSERT(s->used == slabfull);
		s->used = ss.fulluse - s->clz;		// full-1 loading
		tree_insert(s,&ss.partial);
		checktree(ss.partial);
	}
/**Defination of this functionis not there in proto code***/
#endif

void* RNewAllocator::allocnewpage(slabset& allocator)
//
// Acquire and initialise a new page, returning a cell from a new slab
// The partial_page tree is empty (otherwise we'd have used a slab from there)
// The partial_page link is put in the highest addressed slab in the page, and the
// lowest addressed slab is used to fulfill the allocation request
//
{
	page* p	 = spare_page;
	if (p)
		spare_page = 0;
	else
	{
		p = static_cast<page*>(map(0,pagesize));
		if (!p)
			return 0;
	}
	ASSERT(p == floor(p,pagesize));
	p->slabs[0].header = ((1<<3) + (1<<2) + (1<<1))<<8;		// set pagemap
	p->slabs[3].parent = &partial_page;
	p->slabs[3].child1 = p->slabs[3].child2 = 0;
	partial_page = &p->slabs[3];
	return initnewslab(allocator,&p->slabs[0]);
}

void RNewAllocator::freepage(page* p)
//
// Release an unused page to the OS
// A single page is cached for reuse to reduce thrashing
// the OS allocator.
//
{
	ASSERT(ceiling(p,pagesize) == p);
	if (!spare_page)
	{
		spare_page = p;
		return;
	}
	unmap(p,pagesize);
}

void RNewAllocator::freeslab(slab* s)
//
// Release an empty slab to the slab manager
// The strategy is:
// 1. The page containing the slab is checked to see the state of the other slabs in the page by
//    inspecting the pagemap field in the header of the first slab in the page.
// 2. The pagemap is updated to indicate the new unused slab
// 3. If this is the only unused slab in the page then the slab header is used to add the page to
//    the partial_page tree/heap
// 4. If all the slabs in the page are now unused the page is release back to the OS
// 5. If this slab has a higher address than the one currently used to track this page in
//    the partial_page heap, the linkage is moved to the new unused slab
//
{
	tree_remove(s);
	checktree(*s->parent);
	ASSERT(header_usedm4(s->header) == header_size(s->header)-4);
	CHECK(s->header |= 0xFF00000);			// illegal value for debug purposes
	page* p = page::pagefor(s);
	unsigned h = p->slabs[0].header;
	int slabix = s - &p->slabs[0];
	unsigned pagemap = header_pagemap(h);
	p->slabs[0].header = h | (0x100<<slabix);
	if (pagemap == 0)
	{	// page was full before, use this slab as link in empty heap
		tree_insert(s, &partial_page);
	}
	else
	{	// find the current empty-link slab
		slab* sl = &p->slabs[hibit(pagemap)];
		pagemap ^= (1<<slabix);
		if (pagemap == 0xf)
		{	// page is now empty so recycle page to os
			tree_remove(sl);
			freepage(p);
			return;
		}
		// ensure the free list link is in highest address slab in page
		if (s > sl)
		{	// replace current link with new one. Address-order tree so position stays the same
			slab** r = sl->parent;
			slab* c1 = sl->child1;
			slab* c2 = sl->child2;
			s->parent = r;
			s->child1 = c1;
			s->child2 = c2;
			*r = s;
			if (c1)
				c1->parent = &s->child1;
			if (c2)
				c2->parent = &s->child2;
		}
		CHECK(if (s < sl) s=sl);
	}
	ASSERT(header_pagemap(p->slabs[0].header) != 0);
	ASSERT(hibit(header_pagemap(p->slabs[0].header)) == unsigned(s - &p->slabs[0]));
}

void RNewAllocator::slab_init()
{
	slab_threshold=0;
	partial_page = 0;
	spare_page = 0;
	memset(&sizemap[0],0xff,sizeof(sizemap));
	memset(&slaballoc[0],0,sizeof(slaballoc));
}

void RNewAllocator::slab_config(unsigned slabbitmap)
{
	ASSERT((slabbitmap & ~okbits) == 0);
	ASSERT(maxslabsize <= 60);

	unsigned char ix = 0xff;
	unsigned bit = 1<<((maxslabsize>>2)-1);
	for (int sz = maxslabsize; sz >= 0; sz -= 4, bit >>= 1)
	{
		if (slabbitmap & bit)
		{
			if (ix == 0xff)
				slab_threshold=sz+1;
			ix = (sz>>2)-1;
		}
		sizemap[sz>>2] = ix;
	}
}

void* RNewAllocator::slab_allocate(slabset& ss)
//
// Allocate a cell from the given slabset
// Strategy:
// 1. Take the partially full slab at the top of the heap (lowest address).
// 2. If there is no such slab, allocate from a new slab
// 3. If the slab has a non-empty freelist, pop the cell from the front of the list and update the slab
// 4. Otherwise, if the slab is not full, return the cell at the end of the currently used region of
//    the slab, updating the slab
// 5. Otherwise, release the slab from the partial tree/heap, marking it as 'floating' and go back to
//    step 1
//
{
	for (;;)
	{
		slab *s = ss.partial;
		if (!s)
			break;
		unsigned h = s->header;
		unsigned free = h & 0xff;		// extract free cell positiong
		if (free)
		{
			ASSERT(((free<<2)-sizeof(slabhdr))%header_size(h) == 0);
			void* p = offset(s,free<<2);
			free = *(unsigned char*)p;	// get next pos in free list
			h += (h&0x3C000)<<6;		// update usedm4
			h &= ~0xff;
			h |= free;					// update freelist
			s->header = h;
			ASSERT(header_free(h) == 0 || ((header_free(h)<<2)-sizeof(slabhdr))%header_size(h) == 0);
			ASSERT(header_usedm4(h) <= 0x3F8u);
			ASSERT((header_usedm4(h)+4)%header_size(h) == 0);
			return p;
		}
		unsigned h2 = h + ((h&0x3C000)<<6);
		if (h2 < 0xfc00000)
		{
			ASSERT((header_usedm4(h2)+4)%header_size(h2) == 0);
			s->header = h2;
			return offset(s,(h>>18) + sizeof(unsigned) + sizeof(slabhdr));
		}
		h |= 0x80000000;				// mark the slab as full-floating
		s->header = h;
		tree_remove(s);
		checktree(ss.partial);
		// go back and try the next slab...
	}
	// no partial slabs found, so allocate from a new slab
	return allocnewslab(ss);
}

void RNewAllocator::slab_free(void* p)
//
// Free a cell from the slab allocator
// Strategy:
// 1. Find the containing slab (round down to nearest 1KB boundary)
// 2. Push the cell into the slab's freelist, and update the slab usage count
// 3. If this is the last allocated cell, free the slab to the main slab manager
// 4. If the slab was full-floating then insert the slab in it's respective partial tree
//
{
	ASSERT(lowbits(p,3)==0);
	slab* s = slab::slabfor(p);

	unsigned pos = lowbits(p, slabsize);
	unsigned h = s->header;
	ASSERT(header_usedm4(h) != 0x3fC);		// slab is empty already
	ASSERT((pos-sizeof(slabhdr))%header_size(h) == 0);
	*(unsigned char*)p = (unsigned char)h;
	h &= ~0xFF;
	h |= (pos>>2);
	unsigned size = h & 0x3C000;
	iTotalAllocSize -= size;
	if (int(h) >= 0)
	{
		h -= size<<6;
		if (int(h)>=0)
		{
			s->header = h;
			return;
		}
		freeslab(s);
		return;
	}
	h -= size<<6;
	h &= ~0x80000000;
	s->header = h;
	slabset& ss = slaballoc[(size>>14)-1];
	tree_insert(s,&ss.partial);
	checktree(ss.partial);
}

void* RNewAllocator::initnewslab(slabset& allocator, slab* s)
//
// initialise an empty slab for this allocator and return the fist cell
// pre-condition: the slabset has no partial slabs for allocation
//
{
	ASSERT(allocator.partial==0);
	TInt size = 4 + ((&allocator-&slaballoc[0])<<2);	// infer size from slab allocator address
	unsigned h = s->header & 0xF00;	// preserve pagemap only
	h |= (size<<12);					// set size
	h |= (size-4)<<18;					// set usedminus4 to one object minus 4
	s->header = h;
	allocator.partial = s;
	s->parent = &allocator.partial;
	s->child1 = s->child2 = 0;
	return offset(s,sizeof(slabhdr));
}

TAny* RNewAllocator::SetBrk(TInt32 aDelta)
{
	if (iFlags & EFixedSize)
		return MFAIL;

	if (aDelta < 0)
		{
		unmap(offset(iTop, aDelta), -aDelta);
		}
	else if (aDelta > 0)
		{
		if (!map(iTop, aDelta))
			return MFAIL;
		}
	void * p =iTop;
	iTop = offset(iTop, aDelta);
	return p;
}

void* RNewAllocator::map(void* p,unsigned sz)
//
// allocate pages in the chunk
// if p is NULL, find an allocate the required number of pages (which must lie in the lower half)
// otherwise commit the pages specified
//
{
ASSERT(p == floor(p, pagesize));
ASSERT(sz == ceiling(sz, pagesize));
ASSERT(sz > 0);

	if (iChunkSize + sz > iMaxLength)
		return 0;

	RChunk chunk;
	chunk.SetHandle(iChunkHandle);
	if (p)
	{
		TInt r = chunk.Commit(iOffset + ptrdiff(p, this),sz);
		if (r < 0)
			return 0;
		//ASSERT(p = offset(this, r - iOffset));
	}
	else
	{
		TInt r = chunk.Allocate(sz);
		if (r < 0)
			return 0;
		if (r > iOffset)
		{
			// can't allow page allocations in DL zone
			chunk.Decommit(r, sz);
			return 0;
		}
		p = offset(this, r - iOffset);
	}
	iChunkSize += sz;
#ifdef TRACING_HEAPS
	if(iChunkSize > iHighWaterMark)
		{
			iHighWaterMark = ceiling(iChunkSize,16*pagesize);


			RChunk chunk;
			chunk.SetHandle(iChunkHandle);
			TKName chunk_name;
			chunk.FullName(chunk_name);
			BTraceContextBig(BTrace::ETest1, 4, 44, chunk_name.Ptr(), chunk_name.Size());

			TUint32 traceData[6];
			traceData[0] = iChunkHandle;
			traceData[1] = iMinLength;
			traceData[2] = iMaxLength;
			traceData[3] = sz;
			traceData[4] = iChunkSize;
			traceData[5] = iHighWaterMark;
			BTraceContextN(BTrace::ETest1, 3, (TUint32)this, 33, traceData, sizeof(traceData));
		}
#endif
	if (iChunkSize >= slab_init_threshold)
	{	// set up slab system now that heap is large enough
		slab_config(slab_config_bits);
		slab_init_threshold = KMaxTUint;
	}
	return p;
}

void* RNewAllocator::remap(void* p,unsigned oldsz,unsigned sz)
{
	if (oldsz > sz)
		{	// shrink
		unmap(offset(p,sz), oldsz-sz);
		}
	else if (oldsz < sz)
		{	// grow, try and do this in place first
		if (!map(offset(p, oldsz), sz-oldsz))
			{
			// need to allocate-copy-free
			void* newp = map(0, sz);
			memcpy(newp, p, oldsz);
			unmap(p,oldsz);
			return newp;
			}
		}
	return p;
}

void RNewAllocator::unmap(void* p,unsigned sz)
{
	ASSERT(p == floor(p, pagesize));
	ASSERT(sz == ceiling(sz, pagesize));
	ASSERT(sz > 0);

	RChunk chunk;
	chunk.SetHandle(iChunkHandle);
	TInt r = chunk.Decommit(ptrdiff(p, offset(this,-iOffset)), sz);
	//TInt offset = (TUint8*)p-(TUint8*)chunk.Base();
	//TInt r = chunk.Decommit(offset,sz);

	ASSERT(r >= 0);
	iChunkSize -= sz;
}

void RNewAllocator::paged_init(unsigned pagepower)
	{
		if (pagepower == 0)
			pagepower = 31;
		else if (pagepower < minpagepower)
			pagepower = minpagepower;
		page_threshold = pagepower;
		for (int i=0;i<npagecells;++i)
		{
			pagelist[i].page = 0;
			pagelist[i].size = 0;
		}
	}

void* RNewAllocator::paged_allocate(unsigned size)
{
	unsigned nbytes = ceiling(size, pagesize);
	if (nbytes < size + cellalign)
	{	// not enough extra space for header and alignment, try and use cell list
		for (pagecell *c = pagelist,*e = c + npagecells;c < e;++c)
			if (c->page == 0)
			{
				void* p = map(0, nbytes);
				if (!p)
					return 0;
				c->page = p;
				c->size = nbytes;
				return p;
			}
	}
	// use a cell header
	nbytes = ceiling(size + cellalign, pagesize);
	void* p = map(0, nbytes);
	if (!p)
		return 0;
	*static_cast<unsigned*>(p) = nbytes;
	return offset(p, cellalign);
}

void* RNewAllocator::paged_reallocate(void* p, unsigned size)
{
	if (lowbits(p, pagesize) == 0)
	{	// continue using descriptor
		pagecell* c = paged_descriptor(p);
		unsigned nbytes = ceiling(size, pagesize);
		void* newp = remap(p, c->size, nbytes);
		if (!newp)
			return 0;
		c->page = newp;
		c->size = nbytes;
		return newp;
	}
	else
	{	// use a cell header
		ASSERT(lowbits(p,pagesize) == cellalign);
		p = offset(p,-int(cellalign));
		unsigned nbytes = ceiling(size + cellalign, pagesize);
		unsigned obytes = *static_cast<unsigned*>(p);
		void* newp = remap(p, obytes, nbytes);
		if (!newp)
			return 0;
		*static_cast<unsigned*>(newp) = nbytes;
		return offset(newp, cellalign);
	}
}

void RNewAllocator::paged_free(void* p)
{
	if (lowbits(p,pagesize) == 0)
	{	// check pagelist
		pagecell* c = paged_descriptor(p);

		iTotalAllocSize -= c->size;

		unmap(p, c->size);
		c->page = 0;
		c->size = 0;
	}
	else
	{	// check page header
		unsigned* page = static_cast<unsigned*>(offset(p,-int(cellalign)));
		unsigned size = *page;
		unmap(page,size);
	}
}

pagecell* RNewAllocator::paged_descriptor(const void* p) const
{
	ASSERT(lowbits(p,pagesize) == 0);
	// Double casting to keep the compiler happy. Seems to think we can trying to
	// change a non-const member (pagelist) in a const function
	pagecell* c = (pagecell*)((void*)pagelist);
	pagecell* e = c + npagecells;
	for (;;)
	{
		ASSERT(c!=e);
		if (c->page == p)
			return c;
		++c;
	}
}

RNewAllocator* RNewAllocator::FixedHeap(TAny* aBase, TInt aMaxLength, TInt aAlign, TBool aSingleThread)
/**
Creates a fixed length heap at a specified location.

On successful return from this function, aMaxLength bytes are committed by the chunk.
The heap cannot be extended.

@param aBase         A pointer to the location where the heap is to be constructed.
@param aMaxLength    The length of the heap. If the supplied value is less
                     than KMinHeapSize, it is discarded and the value KMinHeapSize
                     is used instead.
@param aAlign        The alignment of heap cells.
@param aSingleThread Indicates whether single threaded or not.

@return A pointer to the new heap, or NULL if the heap could not be created.

@panic USER 56 if aMaxLength is negative.
*/
//
// Force construction of the fixed memory.
//
	{

	__ASSERT_ALWAYS(aMaxLength>=0, ::Panic(ETHeapMaxLengthNegative));
	if (aMaxLength<KMinHeapSize)
		aMaxLength=KMinHeapSize;

	RNewAllocator* h = new(aBase) RNewAllocator(aMaxLength, aAlign, aSingleThread);

	if (!aSingleThread)
		{
		TInt r = h->iLock.CreateLocal();
		if (r!=KErrNone)
			return NULL;
		h->iHandles = (TInt*)&h->iLock;
		h->iHandleCount = 1;
		}
	return h;
	}

RNewAllocator* RNewAllocator::ChunkHeap(const TDesC* aName, TInt aMinLength, TInt aMaxLength, TInt aGrowBy, TInt aAlign, TBool aSingleThread)
/**
Creates a heap in a local or global chunk.

The chunk hosting the heap can be local or global.

A local chunk is one which is private to the process creating it and is not
intended for access by other user processes.
A global chunk is one which is visible to all processes.

The hosting chunk is local, if the pointer aName is NULL, otherwise
the hosting chunk is global and the descriptor *aName is assumed to contain
the name to be assigned to it.

Ownership of the host chunk is vested in the current process.

A minimum and a maximum size for the heap can be specified. On successful
return from this function, the size of the heap is at least aMinLength.
If subsequent requests for allocation of memory from the heap cannot be
satisfied by compressing the heap, the size of the heap is extended in
increments of aGrowBy until the request can be satisfied. Attempts to extend
the heap causes the size of the host chunk to be adjusted.

Note that the size of the heap cannot be adjusted by more than aMaxLength.

@param aName         If NULL, the function constructs a local chunk to host
                     the heap.
                     If not NULL, a pointer to a descriptor containing the name
                     to be assigned to the global chunk hosting the heap.
@param aMinLength    The minimum length of the heap.
@param aMaxLength    The maximum length to which the heap can grow.
                     If the supplied value is less than KMinHeapSize, then it
                     is discarded and the value KMinHeapSize used instead.
@param aGrowBy       The increments to the size of the host chunk. If a value is
                     not explicitly specified, the value KMinHeapGrowBy is taken
                     by default
@param aAlign        The alignment of heap cells.
@param aSingleThread Indicates whether single threaded or not.

@return A pointer to the new heap or NULL if the heap could not be created.

@panic USER 41 if aMinLength is greater than the supplied value of aMaxLength.
@panic USER 55 if aMinLength is negative.
@panic USER 56 if aMaxLength is negative.
*/
//
// Allocate a Chunk of the requested size and force construction.
//
	{

	__ASSERT_ALWAYS(aMinLength>=0, ::Panic(ETHeapMinLengthNegative));
	__ASSERT_ALWAYS(aMaxLength>=aMinLength, ::Panic(ETHeapCreateMaxLessThanMin));
	if (aMaxLength<KMinHeapSize)
		aMaxLength=KMinHeapSize;
	RChunk c;
	TInt r;
	if (aName)
		r = c.CreateDisconnectedGlobal(*aName, 0, 0, aMaxLength*2, aSingleThread ? EOwnerThread : EOwnerProcess);
	else
		r = c.CreateDisconnectedLocal(0, 0, aMaxLength*2, aSingleThread ? EOwnerThread : EOwnerProcess);
	if (r!=KErrNone)
		return NULL;

	RNewAllocator* h = ChunkHeap(c, aMinLength, aGrowBy, aMaxLength, aAlign, aSingleThread, UserHeap::EChunkHeapDuplicate);
	c.Close();
	return h;
	}

RNewAllocator* RNewAllocator::ChunkHeap(RChunk aChunk, TInt aMinLength, TInt aGrowBy, TInt aMaxLength, TInt aAlign, TBool aSingleThread, TUint32 aMode)
/**
Creates a heap in an existing chunk.

This function is intended to be used to create a heap in a user writable code
chunk as created by a call to RChunk::CreateLocalCode().
This type of heap can be used to hold code fragments from a JIT compiler.

The maximum length to which the heap can grow is the same as
the maximum size of the chunk.

@param aChunk        The chunk that will host the heap.
@param aMinLength    The minimum length of the heap.
@param aGrowBy       The increments to the size of the host chunk.
@param aMaxLength    The maximum length to which the heap can grow.
@param aAlign        The alignment of heap cells.
@param aSingleThread Indicates whether single threaded or not.
@param aMode         Flags controlling the reallocation. The only bit which has any
                     effect on reallocation is that defined by the enumeration
                     ENeverMove of the enum RAllocator::TReAllocMode.
                     If this is set, then any successful reallocation guarantees not
                     to have changed the start address of the cell.
                     By default, this parameter is zero.

@return A pointer to the new heap or NULL if the heap could not be created.
*/
//
// Construct a heap in an already existing chunk
//
	{

	return OffsetChunkHeap(aChunk, aMinLength, 0, aGrowBy, aMaxLength, aAlign, aSingleThread, aMode);
	}

RNewAllocator* RNewAllocator::OffsetChunkHeap(RChunk aChunk, TInt aMinLength, TInt aOffset, TInt aGrowBy, TInt aMaxLength, TInt aAlign, TBool aSingleThread, TUint32 aMode)
/**
Creates a heap in an existing chunk, offset from the beginning of the chunk.

This function is intended to be used to create a heap where a fixed amount of
additional data must be stored at a known location. The additional data can be
placed at the base address of the chunk, allowing it to be located without
depending on the internals of the heap structure.

The maximum length to which the heap can grow is the maximum size of the chunk,
minus the offset.

@param aChunk        The chunk that will host the heap.
@param aMinLength    The minimum length of the heap.
@param aOffset       The offset from the start of the chunk, to the start of the heap.
@param aGrowBy       The increments to the size of the host chunk.
@param aMaxLength    The maximum length to which the heap can grow.
@param aAlign        The alignment of heap cells.
@param aSingleThread Indicates whether single threaded or not.
@param aMode         Flags controlling the reallocation. The only bit which has any
                     effect on reallocation is that defined by the enumeration
                     ENeverMove of the enum RAllocator::TReAllocMode.
                     If this is set, then any successful reallocation guarantees not
                     to have changed the start address of the cell.
                     By default, this parameter is zero.

@return A pointer to the new heap or NULL if the heap could not be created.
*/
//
// Construct a heap in an already existing chunk
//
	{

	TInt page_size = malloc_getpagesize;
	if (!aAlign)
		aAlign = RNewAllocator::ECellAlignment;
	TInt maxLength = aChunk.MaxSize();
	TInt round_up = Max(aAlign, page_size);
	TInt min_cell = _ALIGN_UP(Max((TInt)RNewAllocator::EAllocCellSize, (TInt)RNewAllocator::EFreeCellSize), aAlign);
	aOffset = _ALIGN_UP(aOffset, 8);

#ifdef NO_RESERVE_MEMORY
#ifdef TRACING_HEAPS
	TKName chunk_name;
	aChunk.FullName(chunk_name);
	BTraceContextBig(BTrace::ETest1, 0xF, 0xFF, chunk_name.Ptr(), chunk_name.Size());

	TUint32 traceData[4];
	traceData[0] = aChunk.Handle();
	traceData[1] = aMinLength;
	traceData[2] = aMaxLength;
	traceData[3] = aAlign;
	BTraceContextN(BTrace::ETest1, 0xE, 0xEE, 0xEE, traceData, sizeof(traceData));
#endif
	//modifying the aMinLength because not all memory is the same in the new allocator. So it cannot reserve it properly
	if( aMinLength<aMaxLength)
		aMinLength = 0;
#endif

	if (aMaxLength && aMaxLength+aOffset<maxLength)
		maxLength = _ALIGN_UP(aMaxLength+aOffset, round_up);
	__ASSERT_ALWAYS(aMinLength>=0, ::Panic(ETHeapMinLengthNegative));
	__ASSERT_ALWAYS(maxLength>=aMinLength, ::Panic(ETHeapCreateMaxLessThanMin));
	aMinLength = _ALIGN_UP(Max(aMinLength, (TInt)sizeof(RNewAllocator) + min_cell) + aOffset, round_up);

	// the new allocator uses a disconnected chunk so must commit the initial allocation
	// with Commit() instead of Adjust()
	//	TInt r=aChunk.Adjust(aMinLength);
	//TInt r = aChunk.Commit(aOffset, aMinLength);

	aOffset = maxLength;
	//TInt MORE_CORE_OFFSET = maxLength/2;
	//TInt r = aChunk.Commit(MORE_CORE_OFFSET, aMinLength);
	TInt r = aChunk.Commit(aOffset, aMinLength);

	if (r!=KErrNone)
		return NULL;

	RNewAllocator* h = new (aChunk.Base() + aOffset) RNewAllocator(aChunk.Handle(), aOffset, aMinLength, maxLength, aGrowBy, aAlign, aSingleThread);
	//RNewAllocator* h = new (aChunk.Base() + MORE_CORE_OFFSET) RNewAllocator(aChunk.Handle(), aOffset, aMinLength, maxLength, aGrowBy, aAlign, aSingleThread);

	TBool duplicateLock = EFalse;
	if (!aSingleThread)
		{
		duplicateLock = aMode & UserHeap::EChunkHeapSwitchTo;
		if(h->iLock.CreateLocal(duplicateLock ? EOwnerThread : EOwnerProcess)!=KErrNone)
			{
			h->iChunkHandle = 0;
			return NULL;
			}
		}

	if (aMode & UserHeap::EChunkHeapSwitchTo)
		User::SwitchHeap(h);

	h->iHandles = &h->iChunkHandle;
	if (!aSingleThread)
		{
		// now change the thread-relative chunk/semaphore handles into process-relative handles
		h->iHandleCount = 2;
		if(duplicateLock)
			{
			RHandleBase s = h->iLock;
			r = h->iLock.Duplicate(RThread());
			s.Close();
			}
		if (r==KErrNone && (aMode & UserHeap::EChunkHeapDuplicate))
			{
			r = ((RChunk*)&h->iChunkHandle)->Duplicate(RThread());
			if (r!=KErrNone)
				h->iLock.Close(), h->iChunkHandle=0;
			}
		}
	else
		{
		h->iHandleCount = 1;
		if (aMode & UserHeap::EChunkHeapDuplicate)
			r = ((RChunk*)&h->iChunkHandle)->Duplicate(RThread(), EOwnerThread);
		}

	// return the heap address
	return (r==KErrNone) ? h : NULL;
	}


#define UserTestDebugMaskBit(bit) (TBool)(UserSvr::DebugMask(bit>>5) & (1<<(bit&31)))

#ifndef NO_NAMED_LOCAL_CHUNKS
//this class requires Symbian^3 for ElocalNamed

// Hack to get access to TChunkCreateInfo internals outside of the kernel
class TFakeChunkCreateInfo: public TChunkCreateInfo
	{
public:
	 void SetThreadNewAllocator(TInt aInitialSize, TInt aMaxSize, const TDesC& aName)
	 	{
		iType = TChunkCreate::ENormal | TChunkCreate::EDisconnected | TChunkCreate::EData;
		iMaxSize = aMaxSize * 2;

	 	iInitialBottom = 0;
	 	iInitialTop = aInitialSize;
	 	iAttributes = TChunkCreate::ELocalNamed;
	 	iName = &aName;
	 	iOwnerType = EOwnerThread;
	 	}
	};
#endif

#ifndef NO_NAMED_LOCAL_CHUNKS
_LIT(KLitDollarHeap,"$HEAP");
#endif
TInt RNewAllocator::CreateThreadHeap(SStdEpocThreadCreateInfo& aInfo, RNewAllocator*& aHeap, TInt aAlign, TBool aSingleThread)
/**
@internalComponent
*/
//
// Create a user-side heap
//
	{
	TInt page_size = malloc_getpagesize;
	TInt minLength = _ALIGN_UP(aInfo.iHeapInitialSize, page_size);
	TInt maxLength = Max(aInfo.iHeapMaxSize, minLength);
#ifdef TRACING_ALLOCS
	if (UserTestDebugMaskBit(96)) // 96 == KUSERHEAPTRACE in nk_trace.h
		aInfo.iFlags |= ETraceHeapAllocs;
#endif
	// Create the thread's heap chunk.
	RChunk c;
#ifndef NO_NAMED_LOCAL_CHUNKS
	TFakeChunkCreateInfo createInfo;
	createInfo.SetThreadNewAllocator(0, maxLength, KLitDollarHeap());	// Initialise with no memory committed.
	TInt r = c.Create(createInfo);
#else
	TInt r = c.CreateDisconnectedLocal(0, 0, maxLength * 2);
#endif
	if (r!=KErrNone)
		return r;
	aHeap = ChunkHeap(c, minLength, page_size, maxLength, aAlign, aSingleThread, UserHeap::EChunkHeapSwitchTo|UserHeap::EChunkHeapDuplicate);
	c.Close();
	if (!aHeap)
		return KErrNoMemory;
#ifdef TRACING_ALLOCS
	if (aInfo.iFlags & ETraceHeapAllocs)
		{
		aHeap->iFlags |= RAllocator::ETraceAllocs;
		BTraceContext8(BTrace::EHeap, BTrace::EHeapCreate,(TUint32)aHeap, RNewAllocator::EAllocCellSize);
		TInt handle = aHeap->ChunkHandle();
		TInt chunkId = ((RHandleBase&)handle).BTraceId();
		BTraceContext8(BTrace::EHeap, BTrace::EHeapChunkCreate, (TUint32)aHeap, chunkId);
		}
#endif
	return KErrNone;
	}

/*
 * \internal
 * Called from the qtmain.lib application wrapper.
 * Create a new heap as requested, but use the new allocator
 */
Q_CORE_EXPORT TInt qt_symbian_SetupThreadHeap(TBool /*aNotFirst*/, SStdEpocThreadCreateInfo& aInfo)
    {
    TInt r = KErrNone;
    if (!aInfo.iAllocator && aInfo.iHeapInitialSize>0)
        {
        // new heap required
        RNewAllocator* pH = NULL;
        r = RNewAllocator::CreateThreadHeap(aInfo, pH);
        }
    else if (aInfo.iAllocator)
        {
        // sharing a heap
        RAllocator* pA = aInfo.iAllocator;
        pA->Open();
        User::SwitchAllocator(pA);
        }
    return r;
    }

#else
/*
 * \internal
 * Called from the qtmain.lib application wrapper.
 * Create a new heap as requested, using the default system allocator
 */
Q_CORE_EXPORT TInt qt_symbian_SetupThreadHeap(TBool aNotFirst, SStdEpocThreadCreateInfo& aInfo)
{
    return UserHeap::SetupThreadHeap(aNotFirst, aInfo);
}
#endif

#ifndef __WINS__
#pragma pop
#endif
