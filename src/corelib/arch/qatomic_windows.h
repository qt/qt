/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QATOMIC_WINDOWS_H
#define QATOMIC_WINDOWS_H

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_REFERENCE_COUNTING_IS_WAIT_FREE

inline bool QBasicAtomicInt::isReferenceCountingNative()
{ return true; }
inline bool QBasicAtomicInt::isReferenceCountingWaitFree()
{ return true; }

#define Q_ATOMIC_INT_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_TEST_AND_SET_IS_WAIT_FREE

inline bool QBasicAtomicInt::isTestAndSetNative()
{ return true; }
inline bool QBasicAtomicInt::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_STORE_IS_WAIT_FREE

inline bool QBasicAtomicInt::isFetchAndStoreNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndStoreWaitFree()
{ return true; }

#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_INT_FETCH_AND_ADD_IS_WAIT_FREE

inline bool QBasicAtomicInt::isFetchAndAddNative()
{ return true; }
inline bool QBasicAtomicInt::isFetchAndAddWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_TEST_AND_SET_IS_WAIT_FREE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isTestAndSetWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_STORE_IS_WAIT_FREE

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndStoreWaitFree()
{ return true; }

#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_ALWAYS_NATIVE
#define Q_ATOMIC_POINTER_FETCH_AND_ADD_IS_WAIT_FREE
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddNative()
{ return true; }
template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::isFetchAndAddWaitFree()
{ return true; }

#if defined(Q_CC_MSVC)

// MSVC++ 6.0 doesn't generate correct code when optimizations are turned on!
#if _MSC_VER < 1300 && defined (_M_IX86)

inline bool QBasicAtomicInt::ref()
{
    volatile int *pointer = &_q_value;
    unsigned char retVal;
    __asm {
        mov ECX,pointer
        lock inc DWORD ptr[ECX]
        setne retVal
    }
    return retVal != 0;
}

inline bool QBasicAtomicInt::deref()
{
    volatile int *pointer = &_q_value;
    unsigned char retVal;
    __asm {
        mov ECX,pointer
        lock dec DWORD ptr[ECX]
        setne retVal
    }
    return retVal != 0;
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    volatile int *pointer = &_q_value;
    __asm {
        mov EDX,pointer
        mov EAX,expectedValue
        mov ECX,newValue
        lock cmpxchg dword ptr[EDX],ECX
        mov newValue,EAX
    }
    return newValue == expectedValue;
}


inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    volatile int *pointer = &_q_value;
    __asm {
        mov EDX,pointer
        mov ECX,newValue
        lock xchg dword ptr[EDX],ECX
        mov newValue,ECX
    }
    return newValue;
}


inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    volatile int *pointer = &_q_value;
    __asm {
        mov EDX,pointer
        mov ECX,valueToAdd
        lock xadd dword ptr[EDX],ECX
        mov valueToAdd,ECX
    }
    return valueToAdd;
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    volatile void *pointer = &_q_value;
    __asm {
        mov EDX,pointer
        mov EAX,expectedValue
        mov ECX,newValue
        lock cmpxchg dword ptr[EDX],ECX
        mov newValue,EAX
    }
    return newValue == expectedValue;
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    volatile void *pointer = &_q_value;
    __asm {
        mov EDX,pointer
        mov ECX,newValue
        lock xchg dword ptr[EDX],ECX
        mov newValue,ECX
    }
    return reinterpret_cast<T *>(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    volatile void *pointer = &_q_value;
    valueToAdd *= sizeof(T);
    __asm {
        mov EDX,pointer
        mov ECX,valueToAdd
        lock xadd dword ptr[EDX],ECX
        mov pointer,ECX
    }
    return reinterpret_cast<T *>(const_cast<void *>(pointer));
}

#else

#if !defined(Q_OS_WINCE)
// use compiler intrinsics for all atomic functions
//those functions need to be define in the global namespace
QT_END_NAMESPACE

extern "C" {
    long __cdecl _InterlockedIncrement(volatile long *);
    long __cdecl _InterlockedDecrement(volatile long *);
    long __cdecl _InterlockedExchange(volatile long *, long);
    long __cdecl _InterlockedCompareExchange(volatile long *, long, long);
    long __cdecl _InterlockedExchangeAdd(volatile long *, long);
}
#  pragma intrinsic (_InterlockedIncrement)
#  pragma intrinsic (_InterlockedDecrement)
#  pragma intrinsic (_InterlockedExchange)
#  pragma intrinsic (_InterlockedCompareExchange)
#  pragma intrinsic (_InterlockedExchangeAdd)

#  ifndef _M_IX86
extern "C" {
    void *_InterlockedCompareExchangePointer(void * volatile *, void *, void *);
    void *_InterlockedExchangePointer(void * volatile *, void *);
    __int64 _InterlockedExchangeAdd64(__int64 volatile * Addend, __int64 Value);
}
#    pragma intrinsic (_InterlockedCompareExchangePointer)
#    pragma intrinsic (_InterlockedExchangePointer)
#    pragma intrinsic (_InterlockedExchangeAdd64)
#    define _InterlockedExchangeAddPointer(a,b) \
        _InterlockedExchangeAdd64(reinterpret_cast<volatile __int64 *>(a), __int64(b))
#  else
#    define _InterlockedCompareExchangePointer(a,b,c) \
        _InterlockedCompareExchange(reinterpret_cast<volatile long *>(a), long(b), long(c))
#    define _InterlockedExchangePointer(a, b) \
        _InterlockedExchange(reinterpret_cast<volatile long *>(a), long(b))
#    define _InterlockedExchangeAddPointer(a,b) \
        _InterlockedExchangeAdd(reinterpret_cast<volatile long *>(a), long(b))
#  endif

QT_BEGIN_NAMESPACE

inline bool QBasicAtomicInt::ref()
{
    return _InterlockedIncrement(reinterpret_cast<volatile long *>(&_q_value)) != 0;
}

inline bool QBasicAtomicInt::deref()
{
    return _InterlockedDecrement(reinterpret_cast<volatile long *>(&_q_value)) != 0;
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return _InterlockedCompareExchange(reinterpret_cast<volatile long *>(&_q_value), newValue, expectedValue) == expectedValue;
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    return _InterlockedExchange(reinterpret_cast<volatile long *>(&_q_value), newValue);
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    return _InterlockedExchangeAdd(reinterpret_cast<volatile long *>(&_q_value), valueToAdd);
}

#if defined(Q_CC_MSVC)
#pragma warning( push )
#pragma warning( disable : 4311 ) // ignoring the warning from /Wp64
#endif

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    return (_InterlockedCompareExchangePointer(reinterpret_cast<void * volatile *>(&_q_value),
                                              newValue, expectedValue) == 
#ifndef _M_IX86
                                              (void *)
#else
                                              (long)  
#endif
                                              (expectedValue));
}

#if defined(Q_CC_MSVC)
#pragma warning( pop )
#endif

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    return reinterpret_cast<T *>(_InterlockedExchangePointer(reinterpret_cast<void * volatile *>(&_q_value), newValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    return reinterpret_cast<T *>(_InterlockedExchangeAddPointer(reinterpret_cast<void * volatile *>(&_q_value), valueToAdd * sizeof(T)));
}

#else // Q_OS_WINCE

#if _WIN32_WCE < 0x600 && defined(_X86_)
// For X86 Windows CE build we need to include winbase.h to be able
// to catch the inline functions which overwrite the regular 
// definitions inside of coredll.dll. Though one could use the 
// original version of Increment/Decrement, the others are not
// exported at all.
#include <winbase.h>
#else

#if _WIN32_WCE >= 0x600
#define Q_ARGUMENT_TYPE volatile
#  if defined(_X86_)
#    define InterlockedIncrement _InterlockedIncrement
#    define InterlockedDecrement _InterlockedDecrement
#    define InterlockedExchange _InterlockedExchange
#    define InterlockedCompareExchange _InterlockedCompareExchange
#    define InterlockedExchangeAdd _InterlockedExchangeAdd
#  endif
#else
#define Q_ARGUMENT_TYPE
#endif

QT_END_NAMESPACE

extern "C" {
long __cdecl InterlockedIncrement(long Q_ARGUMENT_TYPE * lpAddend);
long __cdecl InterlockedDecrement(long Q_ARGUMENT_TYPE * lpAddend);
long __cdecl InterlockedExchange(long Q_ARGUMENT_TYPE * Target, long Value);
long __cdecl InterlockedCompareExchange(long Q_ARGUMENT_TYPE * Destination, long Exchange, long Comperand);
long __cdecl InterlockedExchangeAdd(long Q_ARGUMENT_TYPE * Addend, long Value);
}

#if _WIN32_WCE >= 0x600 && defined(_X86_)
#  pragma intrinsic (_InterlockedIncrement)
#  pragma intrinsic (_InterlockedDecrement)
#  pragma intrinsic (_InterlockedExchange)
#  pragma intrinsic (_InterlockedCompareExchange)
#  pragma intrinsic (_InterlockedExchangeAdd)
#endif

QT_BEGIN_NAMESPACE

#endif


inline bool QBasicAtomicInt::ref()
{
    return InterlockedIncrement((long*)(&_q_value)) != 0;
}

inline bool QBasicAtomicInt::deref()
{
    return InterlockedDecrement((long*)(&_q_value)) != 0;
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return InterlockedCompareExchange((long*)(&_q_value), newValue, expectedValue) == expectedValue;
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{
    return InterlockedExchange((long*)(&_q_value), newValue);
}

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    return InterlockedExchangeAdd((long*)(&_q_value), valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T *newValue)
{
    return (InterlockedCompareExchange((long*)(&_q_value),
                                              (long)newValue, (long)expectedValue) == 
                                              (long)(expectedValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T *newValue)
{
    return reinterpret_cast<T *>(InterlockedExchange((long*)(&_q_value), (long)newValue));
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{
    return reinterpret_cast<T *>(InterlockedExchangeAdd((long*)(&_q_value), valueToAdd * sizeof(T)));
}

#endif //Q_OS_WINCE

#endif // _MSC_VER ...

#else

// __INTERLOCKED_DECLARED is defined in MinGW's winbase.h. Simply, preferrably we use
// MinGW's definition, such that we pick up variations in the headers.
#ifndef __INTERLOCKED_DECLARED
#define __INTERLOCKED_DECLARED
QT_END_NAMESPACE

extern "C" {
    __declspec(dllimport) long __stdcall InterlockedCompareExchange(long *, long, long);
    __declspec(dllimport) long __stdcall InterlockedIncrement(long *);
    __declspec(dllimport) long __stdcall InterlockedDecrement(long *);
    __declspec(dllimport) long __stdcall InterlockedExchange(long *, long);
    __declspec(dllimport) long __stdcall InterlockedExchangeAdd(long *, long);
}

QT_BEGIN_NAMESPACE
#endif

inline bool QBasicAtomicInt::ref()
{
    return InterlockedIncrement(reinterpret_cast<long *>(const_cast<int *>(&_q_value))) != 0;
}

inline bool QBasicAtomicInt::deref()
{
    return InterlockedDecrement(reinterpret_cast<long *>(const_cast<int *>(&_q_value))) != 0;
}

inline bool QBasicAtomicInt::testAndSetOrdered(int expectedValue, int newValue)
{
    return InterlockedCompareExchange(reinterpret_cast<long *>(const_cast<int *>(&_q_value)), newValue, expectedValue) == expectedValue;
}

inline int QBasicAtomicInt::fetchAndStoreOrdered(int newValue)
{ return InterlockedExchange(reinterpret_cast<long *>(const_cast<int *>(&_q_value)), newValue); }

inline int QBasicAtomicInt::fetchAndAddOrdered(int valueToAdd)
{
    return InterlockedExchangeAdd(reinterpret_cast<long *>(const_cast<int *>(&_q_value)), valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetOrdered(T *expectedValue, T* newValue)
{ return InterlockedCompareExchange(reinterpret_cast<long *>(const_cast<T **>(&_q_value)),
                                    reinterpret_cast<long>(newValue),
                                    reinterpret_cast<long>(expectedValue)) == reinterpret_cast<long>(expectedValue); }

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreOrdered(T* newValue)
{ return reinterpret_cast<T *>(InterlockedExchange(reinterpret_cast<long *>(const_cast<T **>(&_q_value)),
			                           reinterpret_cast<long>(newValue))); }

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddOrdered(qptrdiff valueToAdd)
{ return reinterpret_cast<T *>(InterlockedExchangeAdd(reinterpret_cast<long *>(const_cast<T **>(&_q_value)), valueToAdd * sizeof(T))); }

#endif // Q_CC_GNU

inline bool QBasicAtomicInt::testAndSetRelaxed(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetAcquire(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

inline bool QBasicAtomicInt::testAndSetRelease(int expectedValue, int newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

inline int QBasicAtomicInt::fetchAndStoreRelaxed(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreAcquire(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

inline int QBasicAtomicInt::fetchAndStoreRelease(int newValue)
{
    return fetchAndStoreOrdered(newValue);
}

inline int QBasicAtomicInt::fetchAndAddRelaxed(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddAcquire(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

inline int QBasicAtomicInt::fetchAndAddRelease(int valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelaxed(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetAcquire(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE bool QBasicAtomicPointer<T>::testAndSetRelease(T *expectedValue, T *newValue)
{
    return testAndSetOrdered(expectedValue, newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelaxed(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreAcquire(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndStoreRelease(T *newValue)
{
    return fetchAndStoreOrdered(newValue);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelaxed(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddAcquire(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

template <typename T>
Q_INLINE_TEMPLATE T *QBasicAtomicPointer<T>::fetchAndAddRelease(qptrdiff valueToAdd)
{
    return fetchAndAddOrdered(valueToAdd);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QATOMIC_WINDOWS_H
