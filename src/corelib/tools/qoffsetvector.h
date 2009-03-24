/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QCIRCULARBUFFER_H
#define QCIRCULARBUFFER_H

#include <QtCore/qatomic.h>

struct QOffsetVectorData
{
    QBasicAtomicInt ref;
    int alloc;
    int count;
    int start;
    int offset;
    uint sharable : 1;

    void dump() const;
};

template <typename T>
struct QOffsetVectorTypedData
{
    QBasicAtomicInt ref;
    int alloc;
    int count;
    int start;
    int offset;
    uint sharable : 1;

    T array[1];
};

class QOffsetVectorDevice;

template<typename T>
class QOffsetVector {
    typedef QOffsetVectorTypedData<T> Data;
    union { QOffsetVectorData *p; QOffsetVectorTypedData<T> *d; };
public:
    explicit QOffsetVector(int capacity = 0);
    QOffsetVector(const QOffsetVector<T> &v) : d(v.d) { d->ref.ref(); if (!d->sharable) detach_helper(); }

    inline ~QOffsetVector() { if (!d) return; if (!d->ref.deref()) free(d); }

    inline void detach() { if (d->ref != 1) detach_helper(); }
    inline bool isDetached() const { return d->ref == 1; }
    inline void setSharable(bool sharable) { if (!sharable) detach(); d->sharable = sharable; }

    QOffsetVector<T> &operator=(const QOffsetVector<T> &other);
    bool operator==(const QOffsetVector<T> &other) const;
    inline bool operator!=(const QOffsetVector<T> &other) const { return !(*this == other); }

    inline int capacity() const {return d->alloc; }
    inline int count() const { return d->count; }
    inline int size() const { return d->count; }

    inline bool isEmpty() const { return d->count == 0; }
    inline bool isFull() const { return d->count == d->alloc; }
    inline int available() const { return d->alloc - d->count; }

    void clear();
    void setCapacity(int size);

    const T &at(int pos) const;
    T &operator[](int i);
    const T &operator[](int i) const;

    void append(const T &value);
    void prepend(const T &value);
    void insert(int pos, const T &value);

    inline bool containsIndex(int pos) const { return pos >= d->offset && pos - d->offset < d->count; }
    inline int firstIndex() const { return d->offset; }
    inline int lastIndex() const { return d->offset + d->count - 1; }

    inline const T &first() const { Q_ASSERT(!isEmpty()); return d->array[d->start]; }
    inline const T &last() const { Q_ASSERT(!isEmpty()); return d->array[(d->start + d->count -1) % d->alloc]; }
    inline T &first() { Q_ASSERT(!isEmpty()); detach(); return d->array[d->start]; }
    inline T &last() { Q_ASSERT(!isEmpty()); detach(); return d->array[(d->start + d->count -1) % d->alloc]; }

    void removeFirst();
    T takeFirst();
    void removeLast();
    T takeLast();

    // debug
    void dump() const { p->dump(); }
private:
    void detach_helper();

    QOffsetVectorData *malloc(int alloc);
    void free(Data *d);
    int sizeOfTypedData() {
        // this is more or less the same as sizeof(Data), except that it doesn't
        // count the padding at the end
        return reinterpret_cast<const char *>(&(reinterpret_cast<const Data *>(this))->array[1]) - reinterpret_cast<const char *>(this);
    }
};

template <typename T>
void QOffsetVector<T>::detach_helper()
{
    union { QOffsetVectorData *p; QOffsetVectorTypedData<T> *d; } x;

    x.p = malloc(d->alloc);
    x.d->ref = 1;
    x.d->count = d->count;
    x.d->start = d->start;
    x.d->offset = d->offset;
    x.d->alloc = d->alloc;
    x.d->sharable = true;

    T *dest = x.d->array + x.d->start;
    T *src = d->array + d->start;
    int count = x.d->count;
    while (count--) {
        if (QTypeInfo<T>::isComplex) {
            new (dest) T(*src);
        } else {
            *dest = *src;
        }
        dest++;
        if (dest == x.d->array + x.d->alloc)
            dest = x.d->array;
        src++;
        if (src == d->array + d->alloc)
            src = d->array;
    }

    if (!d->ref.deref())
        free(d);
    d = x.d;
}

template <typename T>
void QOffsetVector<T>::setCapacity(int asize)
{
    if (asize == d->alloc)
        return;
    detach();
    union { QOffsetVectorData *p; QOffsetVectorTypedData<T> *d; } x;
    x.p = malloc(asize);
    x.d->alloc = asize;
    x.d->count = qMin(d->count, asize);
    x.d->offset = d->offset + d->count - x.d->count;
    x.d->start = x.d->offset % x.d->alloc;
    /*  deep copy -
        slow way now, get unit test working, then
        improve performance if need be. (e.g. memcpy)
    */
    T *dest = x.d->array + (x.d->start + x.d->count-1) % x.d->alloc;
    T *src = d->array + (d->start + d->count-1) % d->alloc;
    int count = x.d->count;
    while (count--) {
        if (QTypeInfo<T>::isComplex) {
            new (dest) T(*src);
        } else {
            *dest = *src;
        }
        if (dest == x.d->array)
            dest = x.d->array + x.d->alloc;
        dest--;
        if (src == d->array)
            src = d->array + d->alloc;
        src--;
    }
    /* free old */
    free(d);
    d = x.d;
}

template <typename T>
void QOffsetVector<T>::clear()
{
    if (d->ref == 1) {
        if (QTypeInfo<T>::isComplex) {
            int count = d->count;
            T * i = d->array + d->start;
            T * e = d->array + d->alloc;
            while (count--) {
                i->~T();
                i++;
                if (i == e)
                    i = d->array;
            }
        }
        d->count = d->start = d->offset = 0;
    } else {
        union { QOffsetVectorData *p; QOffsetVectorTypedData<T> *d; } x;
        x.p = malloc(d->alloc);
        x.d->ref = 1;
        x.d->alloc = d->alloc;
        x.d->count = x.d->start = x.d->offset = 0;
        x.d->sharable = true;
        if (!d->ref.deref()) free(d);
        d = x.d;
    }
}

template <typename T>
inline QOffsetVectorData *QOffsetVector<T>::malloc(int aalloc)
{
    return static_cast<QOffsetVectorData *>(qMalloc(sizeOfTypedData() + (aalloc - 1) * sizeof(T)));
}

template <typename T>
QOffsetVector<T>::QOffsetVector(int asize)
{
    p = malloc(asize);
    d->ref = 1;
    d->alloc = asize;
    d->count = d->start = d->offset = 0;
    d->sharable = true;
}

template <typename T>
QOffsetVector<T> &QOffsetVector<T>::operator=(const QOffsetVector<T> &other)
{
    other.d->ref.ref();
    if (!d->ref.deref())
        free(d);
    d = other.d;
    if (!d->sharable)
        detach_helper();
    return *this;
}

template <typename T>
bool QOffsetVector<T>::operator==(const QOffsetVector<T> &other) const
{
    if (other.d == d)
        return true;
    if (other.d->start != d->start
            || other.d->count != d->count
            || other.d->offset != d->offset
            || other.d->alloc != d->alloc)
        return false;
    for (int i = firstIndex(); i <= lastIndex(); ++i)
        if (!(at(i) == other.at(i)))
            return false;
    return true;
}

template <typename T>
void QOffsetVector<T>::free(Data *x)
{
    if (QTypeInfo<T>::isComplex) {
        int count = d->count;
        T * i = d->array + d->start;
        T * e = d->array + d->alloc;
        while (count--) {
            i->~T();
            i++;
            if (i == e)
                i = d->array;
        }
    }
    qFree(x);
}

template <typename T>
void QOffsetVector<T>::append(const T &value)
{
    detach();
    if (QTypeInfo<T>::isComplex) {
        if (d->count == d->alloc)
            (d->array + (d->start+d->count) % d->alloc)->~T();
        new (d->array + (d->start+d->count) % d->alloc) T(value);
    } else {
        d->array[(d->start+d->count) % d->alloc] = value;
    }

    if (d->count == d->alloc) {
        d->start++;
        d->start %= d->alloc;
        d->offset++;
    } else {
        d->count++;
    }
}

template<typename T>
void QOffsetVector<T>::prepend(const T &value)
{
    detach();
    if (d->start)
        d->start--;
    else
        d->start = d->alloc-1;
    d->offset--;

    if (d->count != d->alloc)
        d->count++;
    else
        if (d->count == d->alloc)
            (d->array + d->start)->~T();

    if (QTypeInfo<T>::isComplex)
        new (d->array + d->start) T(value);
    else
        d->array[d->start] = value;
}

template<typename T>
void QOffsetVector<T>::insert(int pos, const T &value)
{
    detach();
    if (containsIndex(pos)) {
        if(QTypeInfo<T>::isComplex)
            new (d->array + pos % d->alloc) T(value);
        else
            d->array[pos % d->alloc] = value;
    } else if (pos == d->offset-1)
        prepend(value);
    else if (pos == d->offset+d->count)
        append(value);
    else {
        // we don't leave gaps.
        clear();
        d->offset = d->start = pos;
        d->start %= d->alloc;
        d->count = 1;
        if (QTypeInfo<T>::isComplex)
            new (d->array + d->start) T(value);
        else
            d->array[d->start] = value;
    }
}

template <typename T>
inline const T &QOffsetVector<T>::at(int pos) const
{ Q_ASSERT_X(pos >= d->offset && pos - d->offset < d->count, "QOffsetVector<T>::at", "index out of range"); return d->array[pos % d->alloc]; }
template <typename T>
inline const T &QOffsetVector<T>::operator[](int pos) const
{ Q_ASSERT_X(pos >= d->offset && pos - d->offset < d->count, "QOffsetVector<T>::at", "index out of range"); return d->array[pos % d->alloc]; }
template <typename T>

// can use the non-inline one to modify the index range.
inline T &QOffsetVector<T>::operator[](int pos)
{
    detach();
    if (!containsIndex(pos))
        insert(pos, T());
    return d->array[pos % d->alloc];
}

template <typename T>
inline void QOffsetVector<T>::removeFirst()
{
    Q_ASSERT(d->count > 0);
    detach();
    d->count--;
    if (QTypeInfo<T>::isComplex)
        (d->array + d->start)->~T();
    d->start = (d->start + 1) % d->alloc;
    d->offset++;
}

template <typename T>
inline void QOffsetVector<T>::removeLast()
{
    Q_ASSERT(d->count > 0);
    detach();
    d->count--;
    if (QTypeInfo<T>::isComplex)
        (d->array + (d->start + d->count) % d->alloc)->~T();
}

template <typename T>
inline T QOffsetVector<T>::takeFirst()
{ T t = first(); removeFirst(); return t; }

template <typename T>
inline T QOffsetVector<T>::takeLast()
{ T t = last(); removeLast(); return t; }

#endif
