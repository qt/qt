/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_QDOC

#ifndef QSHAREDPOINTER_H
#error Do not include qsharedpointer_impl.h directly
#endif
#if 0
#pragma qt_sync_stop_processing
#endif

#include <new>
#include <QtCore/qatomic.h>
#include <QtCore/qobject.h>    // for qobject_cast

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

// Macro QSHAREDPOINTER_VERIFY_AUTO_CAST
//  generates a compiler error if the following construct isn't valid:
//    T *ptr1;
//    X *ptr2 = ptr1;
//
#ifdef QT_NO_DEBUG
# define QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X)          qt_noop()
#else

template<typename T> inline void qt_sharedpointer_cast_check(T *) { }
# define QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X)          \
    qt_sharedpointer_cast_check<T>(static_cast<X *>(0))
#endif

//
// forward declarations
//
template <class T> class QWeakPointer;
template <class T> class QSharedPointer;

template <class X, class T>
QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &ptr);
template <class X, class T>
QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &ptr);

#ifndef QT_NO_QOBJECT
template <class X, class T>
QSharedPointer<X> qSharedPointerObjectCast(const QSharedPointer<T> &ptr);
#endif

namespace QtSharedPointer {
    template <class T> class InternalRefCount;
    template <class T> class ExternalRefCount;

    template <class X, class T> QSharedPointer<X> strongRefFromWeakHelper(const QWeakPointer<T> &, X*);
    template <class X, class Y> QSharedPointer<X> copyAndSetPointer(X * ptr, const QSharedPointer<Y> &src);

    // used in debug mode to verify the reuse of pointers
    Q_CORE_EXPORT void internalSafetyCheckAdd2(const void *, const volatile void *);
    Q_CORE_EXPORT void internalSafetyCheckRemove2(const void *);

    template <class T, typename Klass, typename RetVal>
    inline void executeDeleter(T *t, RetVal (Klass:: *memberDeleter)())
    { (t->*memberDeleter)(); }
    template <class T, typename Deleter>
    inline void executeDeleter(T *t, Deleter d)
    { d(t); }
    template <class T> inline void normalDeleter(T *t) { delete t; }

    // this uses partial template specialization
    // the only compilers that didn't support this were MSVC 6.0 and 2002
    template <class T> struct RemovePointer;
    template <class T> struct RemovePointer<T *> { typedef T Type; };
    template <class T> struct RemovePointer<QSharedPointer<T> > { typedef T Type; };
    template <class T> struct RemovePointer<QWeakPointer<T> > { typedef T Type; };

    template <class T>
    class Basic
    {
        typedef T *Basic:: *RestrictedBool;
    public:
        typedef T Type;

        inline T *data() const { return value; }
        inline bool isNull() const { return !data(); }
        inline operator RestrictedBool() const { return isNull() ? 0 : &Basic::value; }
        inline bool operator !() const { return isNull(); }
        inline T &operator*() const { return *data(); }
        inline T *operator->() const { return data(); }

    protected:
        inline Basic() : value(0) { }
        // ~Basic();

        inline void verifyReconstruction(const T *ptr)
        {
            Q_ASSERT_X(!ptr || value != ptr, "QSharedPointer",
                       "QSharedPointer violation: you cannot create two QSharedPointer objects "
                       "from the same pointer");

            // make use of the "ptr" variable in the no-op statement below
            // since this function is in a public header, we don't
            // want warnings on "unused variables" to show up everywhere
            ptr = 0;
        }

        inline void internalConstruct(T *ptr)
        {
            value = ptr;
        }

#if defined(Q_NO_TEMPLATE_FRIENDS)
    public:
#else
        template <class X> friend class QWeakPointer;
#endif

        Type *value;
    };

    struct ExternalRefCountData
    {
        QAtomicInt weakref;
        QAtomicInt strongref;

        inline ExternalRefCountData() : weakref(1), strongref(1) { }
        virtual inline ~ExternalRefCountData() { Q_ASSERT(!weakref); Q_ASSERT(!strongref); }

        virtual inline bool destroy() { return false; }
    };
    // sizeof(ExternalRefCount) = 12 (32-bit) / 16 (64-bit)

    template <class T, typename Deleter>
    struct CustomDeleter
    {
        Deleter deleter;
        T *ptr;

        inline CustomDeleter(T *p, Deleter d) : deleter(d), ptr(p) {}
    };
    // sizeof(CustomDeleter) = sizeof(Deleter) + sizeof(void*)
    // for Deleter = function pointer:  8 (32-bit) / 16 (64-bit)
    // for Deleter = PMF: 12 (32-bit) / 24 (64-bit)  (GCC)

    struct ExternalRefCountWithDestroyFn: public ExternalRefCountData
    {
        typedef void (*DestroyerFn)(ExternalRefCountData *);
        DestroyerFn destroyer;

        inline ExternalRefCountWithDestroyFn(DestroyerFn d)
            : destroyer(d)
        { }

        inline bool destroy() { destroyer(this); return true; }
        inline void operator delete(void *ptr) { ::operator delete(ptr); }
    };
    // sizeof(ExternalRefCountWithDestroyFn) = 16 (32-bit) / 24 (64-bit)

    template <class T, typename Deleter>
    struct ExternalRefCountWithCustomDeleter: public ExternalRefCountWithDestroyFn
    {
        typedef ExternalRefCountWithCustomDeleter Self;
        typedef ExternalRefCountWithDestroyFn Parent;
        typedef CustomDeleter<T, Deleter> Next;
        Next extra;

        static inline void deleter(ExternalRefCountData *self)
        {
            Self *realself = static_cast<Self *>(self);
            executeDeleter(realself->extra.ptr, realself->extra.deleter);

            // delete the deleter too
            realself->extra.~Next();
        }
        static void safetyCheckDeleter(ExternalRefCountData *self)
        {
            internalSafetyCheckRemove2(self);
            deleter(self);
        }

        static inline Self *create(T *ptr, Deleter userDeleter)
        {
# ifdef QT_SHAREDPOINTER_TRACK_POINTERS
            DestroyerFn destroy = &safetyCheckDeleter;
# else
            DestroyerFn destroy = &deleter;
# endif
            Self *d = static_cast<Self *>(::operator new(sizeof(Self)));

            // initialize the two sub-objects
            new (&d->extra) Next(ptr, userDeleter);
            new (d) Parent(destroy); // can't throw

            return d;
        }
    private:
        // prevent construction and the emission of virtual symbols
        ExternalRefCountWithCustomDeleter();
        ~ExternalRefCountWithCustomDeleter();
    };

    template <class T>
    struct ExternalRefCountWithContiguousData: public ExternalRefCountWithDestroyFn
    {
        typedef ExternalRefCountWithDestroyFn Parent;
        T data;

        static void deleter(ExternalRefCountData *self)
        {
            ExternalRefCountWithContiguousData *that =
                    static_cast<ExternalRefCountWithContiguousData *>(self);
            that->data.~T();
        }
        static void safetyCheckDeleter(ExternalRefCountData *self)
        {
            internalSafetyCheckRemove2(self);
            deleter(self);
        }

        static inline ExternalRefCountData *create(T **ptr)
        {
# ifdef QT_SHAREDPOINTER_TRACK_POINTERS
            DestroyerFn destroy = &safetyCheckDeleter;
# else
            DestroyerFn destroy = &deleter;
# endif
            ExternalRefCountWithContiguousData *d =
                static_cast<ExternalRefCountWithContiguousData *>(::operator new(sizeof(ExternalRefCountWithContiguousData)));

            // initialize the d-pointer sub-object
            // leave d->data uninitialized
            new (d) Parent(destroy); // can't throw

            *ptr = &d->data;
            return d;
        }

    private:
        // prevent construction and the emission of virtual symbols
        ExternalRefCountWithContiguousData();
        ~ExternalRefCountWithContiguousData();
    };

    template <class T>
    class ExternalRefCount: public Basic<T>
    {
    protected:
        typedef ExternalRefCountData Data;

        inline void ref() const { d->weakref.ref(); d->strongref.ref(); }
        inline bool deref()
        {
            if (!d->strongref.deref()) {
                internalDestroy();
            }
            return d->weakref.deref();
        }

        inline void internalConstruct(T *ptr)
        {
#ifdef QT_SHAREDPOINTER_TRACK_POINTERS
            internalConstruct<void (*)(T *)>(ptr, normalDeleter);
#else
            Q_ASSERT(!d);
            if (ptr)
                d = new Data;
            internalFinishConstruction(ptr);
#endif
        }

        template <typename Deleter>
        inline void internalConstruct(T *ptr, Deleter deleter)
        {
            Q_ASSERT(!d);
            if (ptr)
                d = ExternalRefCountWithCustomDeleter<T, Deleter>::create(ptr, deleter);
            internalFinishConstruction(ptr);
        }

        inline void internalCreate()
        {
            T *ptr;
            d = ExternalRefCountWithContiguousData<T>::create(&ptr);
            Basic<T>::internalConstruct(ptr);
        }

        inline void internalFinishConstruction(T *ptr)
        {
            Basic<T>::internalConstruct(ptr);
#ifdef QT_SHAREDPOINTER_TRACK_POINTERS
            if (ptr) internalSafetyCheckAdd2(d, ptr);
#endif
        }

        inline ExternalRefCount() : d(0) { }
        inline ~ExternalRefCount() { if (d && !deref()) delete d; }
        inline ExternalRefCount(const ExternalRefCount<T> &other) : Basic<T>(other), d(other.d)
        { if (d) ref(); }

        template <class X>
        inline void internalCopy(const ExternalRefCount<X> &other)
        {
            internalSet(other.d, other.data());
        }

        inline void internalDestroy()
        {
            if (!d->destroy())
                delete this->value;
        }

    private:
#if defined(Q_NO_TEMPLATE_FRIENDS)
    public:
#else
        template <class X> friend class ExternalRefCount;
        template <class X> friend class QWeakPointer;
        template <class X, class Y> friend QSharedPointer<X> copyAndSetPointer(X * ptr, const QSharedPointer<Y> &src);
        template <class X, class Y> friend QSharedPointer<X> QtSharedPointer::strongRefFromWeakHelper(const QWeakPointer<Y> &src, X *);
#endif

        inline void internalSet(Data *o, T *actual)
        {
            if (d == o) return;
            if (o) {
                verifyReconstruction(actual);

                // increase the strongref, but never up from zero
                register int tmp = o->strongref;
                while (tmp > 0) {
                    // try to increment from "tmp" to "tmp + 1"
                    if (o->strongref.testAndSetRelaxed(tmp, tmp + 1))
                        break;   // succeeded
                    tmp = o->strongref;  // failed, try again
                }

                if (tmp)
                    o->weakref.ref();
                else
                    o = 0;
            }
            if (d && !deref())
                delete d;
            d = o;
            this->value = d && d->strongref ? actual : 0;
        }

    protected:
        Data *d;

    private:
        template<class X> ExternalRefCount(const InternalRefCount<X> &);
    };
} // namespace QtSharedPointer

template <class T>
class QSharedPointer: public QtSharedPointer::ExternalRefCount<T>
{
    typedef typename QtSharedPointer::ExternalRefCount<T> BaseClass;
public:
    inline QSharedPointer() { }
    // inline ~QSharedPointer() { }

    inline explicit QSharedPointer(T *ptr) { internalConstruct(ptr); }

    template <typename Deleter>
    inline QSharedPointer(T *ptr, Deleter d) { internalConstruct(ptr, d); }

    inline QSharedPointer(const QSharedPointer<T> &other) : BaseClass(other) { }
    inline QSharedPointer<T> &operator=(const QSharedPointer<T> &other)
    {
        internalCopy(other);
        return *this;
    }

    inline QSharedPointer(const QWeakPointer<T> &other)
    { *this = QtSharedPointer::strongRefFromWeakHelper(other, static_cast<T*>(0)); }
    inline QSharedPointer<T> &operator=(const QWeakPointer<T> &other)
    { *this = QtSharedPointer::strongRefFromWeakHelper(other, static_cast<T*>(0)); return *this; }

    template <class X>
    inline QSharedPointer(const QSharedPointer<X> &other) { *this = other; }

    template <class X>
    inline QSharedPointer<T> &operator=(const QSharedPointer<X> &other)
    {
        QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X); // if you get an error in this line, the cast is invalid
        internalCopy(other);
        return *this;
    }

    template <class X>
    inline QSharedPointer(const QWeakPointer<X> &other)
    { *this = QtSharedPointer::strongRefFromWeakHelper(other, static_cast<T *>(0)); }

    template <class X>
    inline QSharedPointer<T> &operator=(const QWeakPointer<X> &other)
    { *this = strongRefFromWeakHelper(other, static_cast<T *>(0)); return *this; }

    template <class X>
    QSharedPointer<X> staticCast() const
    {
        return qSharedPointerCast<X, T>(*this);
    }

    template <class X>
    QSharedPointer<X> dynamicCast() const
    {
        return qSharedPointerDynamicCast<X, T>(*this);
    }

    template <class X>
    QSharedPointer<X> constCast() const
    {
        return qSharedPointerConstCast<X, T>(*this);
    }

#ifndef QT_NO_QOBJECT
    template <class X>
    QSharedPointer<X> objectCast() const
    {
        return qSharedPointerObjectCast<X, T>(*this);
    }
#endif

    inline void clear() { *this = QSharedPointer<T>(); }

    QWeakPointer<T> toWeakRef() const;

public:
    static inline QSharedPointer<T> create()
    {
        QSharedPointer<T> result;
        result.internalCreate();

        // now initialize the data
        new (result.data()) T();
        result.internalFinishConstruction(result.data());
        return result;
    }
};

template <class T>
class QWeakPointer
{
    typedef T *QWeakPointer:: *RestrictedBool;
    typedef QtSharedPointer::ExternalRefCountData Data;

public:
    inline bool isNull() const { return d == 0 || d->strongref == 0 || value == 0; }
    inline operator RestrictedBool() const { return isNull() ? 0 : &QWeakPointer::value; }
    inline bool operator !() const { return isNull(); }

    inline QWeakPointer() : d(0), value(0) { }
    inline ~QWeakPointer() { if (d && !d->weakref.deref()) delete d; }
    inline QWeakPointer(const QWeakPointer<T> &o) : d(o.d), value(o.value)
    { if (d) d->weakref.ref(); }
    inline QWeakPointer<T> &operator=(const QWeakPointer<T> &o)
    {
        internalSet(o.d, o.value);
        return *this;
    }

    inline QWeakPointer(const QSharedPointer<T> &o) : d(o.d), value(o.data())
    { if (d) d->weakref.ref();}
    inline QWeakPointer<T> &operator=(const QSharedPointer<T> &o)
    {
        internalSet(o.d, o.value);
        return *this;
    }

    template <class X>
    inline QWeakPointer(const QWeakPointer<X> &o) : d(0), value(0)
    { *this = o; }

    template <class X>
    inline QWeakPointer<T> &operator=(const QWeakPointer<X> &o)
    {
        // conversion between X and T could require access to the virtual table
        // so force the operation to go through QSharedPointer
        *this = o.toStrongRef();
        return *this;
    }

    template <class X>
    inline bool operator==(const QWeakPointer<X> &o) const
    { return d == o.d && value == static_cast<const T *>(o.value); }

    template <class X>
    inline bool operator!=(const QWeakPointer<X> &o) const
    { return !(*this == o); }

    template <class X>
    inline QWeakPointer(const QSharedPointer<X> &o) : d(0), value(0)
    { *this = o; }

    template <class X>
    inline QWeakPointer<T> &operator=(const QSharedPointer<X> &o)
    {
        QSHAREDPOINTER_VERIFY_AUTO_CAST(T, X); // if you get an error in this line, the cast is invalid
        internalSet(o.d, o.data());
        return *this;
    }

    template <class X>
    inline bool operator==(const QSharedPointer<X> &o) const
    { return d == o.d && value == static_cast<const T *>(o.data()); }

    template <class X>
    inline bool operator!=(const QSharedPointer<X> &o) const
    { return !(*this == o); }

    inline void clear() { *this = QWeakPointer<T>(); }

    inline QSharedPointer<T> toStrongRef() const { return QSharedPointer<T>(*this); }

private:

#if defined(Q_NO_TEMPLATE_FRIENDS)
public:
#else
    template <class X, class Y> friend QSharedPointer<X> QtSharedPointer::strongRefFromWeakHelper(const QWeakPointer<Y> &src, X *);
#endif

    inline void internalSet(Data *o, T *actual)
    {
        if (d == o) return;
        if (o)
            o->weakref.ref();
        if (d && !d->weakref.deref())
            delete d;
        d = o;
        value = actual;
    }

    Data *d;
    T *value;
};

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1.data() == ptr2.data();
}
template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1.data() != ptr2.data();
}

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const X *ptr2)
{
    return ptr1.data() == ptr2;
}
template <class T, class X>
bool operator==(const T *ptr1, const QSharedPointer<X> &ptr2)
{
    return ptr1 == ptr2.data();
}

template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const X *ptr2)
{
    return !(ptr1 == ptr2);
}
template <class T, class X>
bool operator!=(const T *ptr1, const QSharedPointer<X> &ptr2)
{
    return !(ptr2 == ptr1);
}

template <class T, class X>
bool operator==(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
{
    return ptr2 == ptr1;
}
template <class T, class X>
bool operator!=(const QSharedPointer<T> &ptr1, const QWeakPointer<X> &ptr2)
{
    return ptr2 != ptr1;
}

template <class T>
Q_INLINE_TEMPLATE QWeakPointer<T> QSharedPointer<T>::toWeakRef() const
{
    return QWeakPointer<T>(*this);
}

namespace QtSharedPointer {
// helper functions:
    template <class X, class T>
    Q_INLINE_TEMPLATE QSharedPointer<X> copyAndSetPointer(X *ptr, const QSharedPointer<T> &src)
    {
        QSharedPointer<X> result;
        result.internalSet(src.d, ptr);
        return result;
    }
    template <class X, class T>
    Q_INLINE_TEMPLATE QSharedPointer<X> strongRefFromWeakHelper
        (const QT_PREPEND_NAMESPACE(QWeakPointer<T>) &src, X *)
    {
        QSharedPointer<X> result;
        result.internalSet(src.d, src.value);
        return result;
    }
}

// cast operators
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerCast(const QSharedPointer<T> &src)
{
    register X *ptr = static_cast<X *>(src.data()); // if you get an error in this line, the cast is invalid
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerCast(const QWeakPointer<T> &src)
{
    return qSharedPointerCast<X, T>(src.toStrongRef());
}

template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerDynamicCast(const QSharedPointer<T> &src)
{
    register X *ptr = dynamic_cast<X *>(src.data()); // if you get an error in this line, the cast is invalid
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerDynamicCast(const QWeakPointer<T> &src)
{
    return qSharedPointerDynamicCast<X, T>(src.toStrongRef());
}

template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerConstCast(const QSharedPointer<T> &src)
{
    register X *ptr = const_cast<X *>(src.data()); // if you get an error in this line, the cast is invalid
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerConstCast(const QWeakPointer<T> &src)
{
    return qSharedPointerConstCast<X, T>(src.toStrongRef());
}

template <class X, class T>
Q_INLINE_TEMPLATE
QWeakPointer<X> qWeakPointerCast(const QSharedPointer<T> &src)
{
    return qSharedPointerCast<X, T>(src).toWeakRef();
}

#ifndef QT_NO_QOBJECT
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerObjectCast(const QSharedPointer<T> &src)
{
    register X *ptr = qobject_cast<X *>(src.data());
    return QtSharedPointer::copyAndSetPointer(ptr, src);
}
template <class X, class T>
Q_INLINE_TEMPLATE QSharedPointer<X> qSharedPointerObjectCast(const QWeakPointer<T> &src)
{
    return qSharedPointerObjectCast<X>(src.toStrongRef());
}

template <class X, class T>
inline QSharedPointer<typename QtSharedPointer::RemovePointer<X>::Type>
qobject_cast(const QSharedPointer<T> &src)
{
    return qSharedPointerObjectCast<typename QtSharedPointer::RemovePointer<X>::Type, T>(src);
}
template <class X, class T>
inline QSharedPointer<typename QtSharedPointer::RemovePointer<X>::Type>
qobject_cast(const QWeakPointer<T> &src)
{
    return qSharedPointerObjectCast<typename QtSharedPointer::RemovePointer<X>::Type, T>(src);
}

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
