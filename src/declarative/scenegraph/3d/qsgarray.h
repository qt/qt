/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QSGARRAY_H
#define QSGARRAY_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qdebug.h>
#include <string.h>
#include <new>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

#if defined(Q_DECL_ALIGN) && defined(Q_ALIGNOF)

#if defined(Q_CC_GNU) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
    typedef char __attribute__((__may_alias__)) QSGArrayAlignedChar;
#else
    typedef char QSGArrayAlignedChar;
#endif

template <typename T, int PreallocSize, size_t AlignT>
struct QSGArrayAlignedPrealloc;
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 1>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(1) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 2>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(2) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 4>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(4) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 8>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(8) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 16>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(16) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 32>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(32) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 64>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(64) data[sizeof(T) * PreallocSize];
};
template <typename T, int PreallocSize>
struct QSGArrayAlignedPrealloc<T, PreallocSize, 128>
{
    QSGArrayAlignedChar Q_DECL_ALIGN(128) data[sizeof(T) * PreallocSize];
};

#else

template <typename T, int PreallocSize, size_t AlignT>
union QSGArrayAlignedPrealloc
{
    char data[sizeof(T) * PreallocSize];
    qint64 q_for_alignment_1;
    double q_for_alignment_2;
};

#endif

template <typename T, int PreallocSize>
class QSGArrayData
{
public:
#if defined(Q_ALIGNOF)
    QSGArrayAlignedPrealloc<T, PreallocSize, Q_ALIGNOF(T)> m_prealloc;
#else
    QSGArrayAlignedPrealloc<T, PreallocSize, sizeof(T)> m_prealloc;
#endif

    inline T *prealloc()
    {
        return reinterpret_cast<T *>(m_prealloc.data);
    }

    inline bool isPrealloc(const T *start) const
    {
        return start == reinterpret_cast<const T *>(m_prealloc.data);
    }
};

template <typename T>
class QSGArrayData<T, 0>
{
public:

    inline T *prealloc() { return 0; }

    inline bool isPrealloc(const T *start) const
    {
        Q_UNUSED(start);
        return false;
    }
};

template <typename T, int PreallocSize = 8>
class QSGArray : private QSGArrayData<T, PreallocSize>
{
public:
    QSGArray();
    explicit QSGArray(int size);
    QSGArray(int size, const T &value);
    QSGArray(const T *values, int size);
    QSGArray(const QSGArray<T, PreallocSize> &other);
    ~QSGArray();

#ifdef Q_CC_RVCT // RVCT 2.2. doesn't see inherited templated functions without scope.
    inline bool isPrealloc(const T *start) const { return QSGArrayData<T, PreallocSize>::isPrealloc(start); }
#endif

    typedef T *iterator;
    typedef const T *const_iterator;

    QSGArray<T, PreallocSize> &operator=
        (const QSGArray<T, PreallocSize> &other);

    int size() const;
    int count() const;
    int capacity() const;

    bool isEmpty() const;

    bool isDetached() const;
    void detach();

    void clear();

    const T &at(int index) const;
    const T &operator[](int index) const;
    T &operator[](int index);

    T value(int index) const;
    T value(int index, const T &defaultValue) const;

    T *extend(int size);

    void append(const T &value);
    void append(const T &value1, const T &value2);
    void append(const T &value1, const T &value2, const T &value3);
    void append(const T &value1, const T &value2, const T &value3, const T &value4);
    void append(const T *values, int count);
    void append(const QSGArray<T, PreallocSize> &other);

    void prepend(const T &value);

    void insert(int index, const T &value);
    void insert(int index, int count, const T &value);
    iterator insert(iterator before, int count, const T &value);
    iterator insert(iterator before, const T &value);

    void replace(int index, const T &value);
    void replace(int index, const T *values, int count);

    void remove(int index);
    void remove(int index, int count);
    void removeFirst() { remove(0); }
    void removeLast() { remove(size() - 1); }

    iterator erase(iterator begin, iterator end);
    iterator erase(iterator pos);

    int indexOf(const T &value, int from = 0) const;
    int lastIndexOf(const T &value, int from = -1) const;
    bool contains(const T &value) const;
    int count(const T &value) const;

    void resize(int size);
    void reserve(int size);
    void squeeze();

    QSGArray<T, PreallocSize> &fill(const T &value, int size = -1);

    void reverse();
    QSGArray<T, PreallocSize> reversed() const;

    QSGArray<T, PreallocSize> mid(int index, int length = -1) const;
    QSGArray<T, PreallocSize> left(int length) const;
    QSGArray<T, PreallocSize> right(int length) const;

    T *data();
    const T *data() const;
    const T *constData() const;

    static QSGArray<T, PreallocSize> fromRawData(const T *data, int size);
    static QSGArray<T, PreallocSize> fromWritableRawData(T *data, int size);

    bool operator==(const QSGArray<T, PreallocSize> &other) const;
    bool operator!=(const QSGArray<T, PreallocSize> &other) const;

    QSGArray<T, PreallocSize> &operator+=(const T &value);
    QSGArray<T, PreallocSize> &operator+=(const QSGArray<T, PreallocSize> &other);
    QSGArray<T, PreallocSize> &operator<<(const T &value);
    QSGArray<T, PreallocSize> &operator<<(const QSGArray<T, PreallocSize> &other);

    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef ptrdiff_t difference_type;
    typedef int size_type;

    inline iterator begin() { return data(); }
    inline const_iterator begin() const { return constData(); }
    inline const_iterator constBegin() const { return constData(); }
    inline iterator end() { return data() + size(); }
    inline const_iterator end() const { return constData() + size(); }
    inline const_iterator constEnd() const { return constData() + size(); }

    inline T &first() { Q_ASSERT(!isEmpty()); return *begin(); }
    inline const T &first() const { Q_ASSERT(!isEmpty()); return *begin(); }
    inline T &last() { Q_ASSERT(!isEmpty()); return *(end()-1); }
    inline const T &last() const { Q_ASSERT(!isEmpty()); return *(end()-1); }
    inline bool startsWith(const T &t) const { return !isEmpty() && first() == t; }
    inline bool endsWith(const T &t) const { return !isEmpty() && last() == t; }

    inline void push_back(const T &value) { append(value); }
    inline void push_front(const T &value) { prepend(value); }
    inline void pop_back() { Q_ASSERT(!isEmpty()); removeLast(); }
    inline void pop_front() { Q_ASSERT(!isEmpty()); removeFirst(); }
    inline bool empty() const { return isEmpty(); }
    inline reference front() { return first(); }
    inline const_reference front() const { return first(); }
    inline reference back() { return last(); }
    inline const_reference back() const { return last(); }

private:
    struct Data
    {
        QBasicAtomicInt ref;
        int capacity;
        T array[1];
    };

    // Invariants:
    // 1. If the data is not shared, then the usual condition is
    //    for m_limit >= m_end.
    // 2. If the data is shared, then m_limit == m_start.
    //    This triggers the range check in append() to call grow(),
    //    which will copy-on-write.  It also triggers the detach
    //    check in data() and operator[] to cause a copy-on-write.
    // 3. If the data is not shared, but previously was, then
    //    m_limit == m_start.  This will trigger grow() or
    //    detach(), which may then notice that it doesn't have to
    //    copy-on-write.  In that case, m_limit is set back
    //    to m_start + m_data->capacity.
    // 4. If m_data is null, then m_start is either the same as
    //    m_prealloc, or it points at raw data (const or non-const).
    // 5. If the array contains const raw data, then m_limit will
    //    be set to m_start to force copy-on-write.
    T *m_start;
    T *m_end;
    mutable T *m_limit;
    Data *m_data;

    inline void initPrealloc()
    {
        m_end = m_start = QSGArrayData<T, PreallocSize>::prealloc();
        m_limit = m_start + PreallocSize;
    }

    QSGArray(const T *data, int size, bool isWritable);

    void free(T *data, int count);
    void release();
    void copyReplace(T *dst, const T *src, int count);
    Data *copyData(const T *src, int size, int capacity);
    void reallocate(int capacity);
    void detach_helper();
    void assign(const QSGArray<T, PreallocSize> &other);
    void grow(int needed);
    void setSize(int size);
};

int Q_DECLARATIVE_EXPORT qArrayAllocMore(int alloc, int extra, int sizeOfT);

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::free(T *data, int count)
{
    while (count-- > 0) {
        data->~T();
        ++data;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::release()
{
    if (m_data) {
        if (!m_data->ref.deref()) {
            if (QTypeInfo<T>::isComplex)
                free(m_start, m_end - m_start);
            qFree(m_data);
        }
    } else if (isPrealloc(m_start)) {
        if (QTypeInfo<T>::isComplex)
            free(m_start, m_end - m_start);
    }
}

// Copy values to initialized memory, replacing previous values.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::copyReplace(T *dst, const T *src, int count)
{
    if (!QTypeInfo<T>::isStatic) {
        ::memmove(dst, src, count * sizeof(T));
    } else {
        while (count-- > 0)
            *dst++ = *src++;
    }
}

// Make a copy of m_data, while remaining exception-safe.
template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE Q_TYPENAME QSGArray<T, PreallocSize>::Data *QSGArray<T, PreallocSize>::copyData(const T *src, int size, int capacity)
{
    Data *data = reinterpret_cast<Data *>
        (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
    Q_CHECK_PTR(data);
    data->ref = 1;
    data->capacity = capacity;
    T *dst = data->array;
    int copied = 0;
    QT_TRY {
        while (copied < size) {
            new (dst) T(*src++);
            ++dst;
            ++copied;
        }
    } QT_CATCH(...) {
        while (copied-- > 0)
            (--dst)->~T();
        qFree(data);
        QT_RETHROW;
    }
    return data;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::reallocate(int capacity)
{
    int size = m_end - m_start;
    if (!QTypeInfo<T>::isStatic) {
        Data *data = reinterpret_cast<Data *>
            (qRealloc(m_data, sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        data->capacity = capacity;
        m_data = data;
    } else {
        Data *data = copyData(m_data->array, size, capacity);
        free(m_data->array, size);
        qFree(m_data);
        m_data = data;
    }
    m_start = m_data->array;
    m_end = m_start + size;
    m_limit = m_start + capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::detach_helper()
{
    // If the reference count is 1, then the array may have been
    // copied and then the copy released.  So just reset the limit.
    if (m_data && m_data->ref == 1) {
        m_limit = m_start + m_data->capacity;
        return;
    }

    // Allocate a new block on the heap and copy the data across.
    int size = m_end - m_start;
    int capacity = qArrayAllocMore(size, 0, sizeof(T));
    m_data = copyData(m_start, size, capacity);

    // Update the start/end/append pointers for faster updates.
    m_start = m_data->array;
    m_end = m_start + size;
    m_limit = m_start + capacity;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::assign(const QSGArray<T, PreallocSize> &other)
{
    if (other.m_data) {
        m_start = other.m_start;
        m_end = other.m_end;
        m_data = other.m_data;
        m_data->ref.ref();

        // We set the append limit of both objects to m_start, which forces
        // the next append() or data() in either object to copy-on-write.
        other.m_limit = m_limit = m_start;
    } else if (other.isPrealloc(other.m_start)) {
        // Make a deep copy of preallocated data.
        initPrealloc();
        m_data = 0;
        append(other.constData(), other.size());
    } else {
        // Shallow copy of raw data.
        m_start = other.m_start;
        m_end = other.m_end;
        m_limit = other.m_limit;
        m_data = 0;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::grow(int needed)
{
    int size = m_end - m_start;
    int capacity = qArrayAllocMore(size, needed, sizeof(T));
    if (!m_data || m_data->ref != 1) {
        // Copy preallocated, raw, or shared data and expand the capacity.
        Data *data = copyData(m_start, size, capacity);
        if (isPrealloc(m_start))
            free(m_start, size);
        if (m_data)
            m_data->ref.deref();
        m_data = data;
        m_start = data->array;
        m_end = m_start + size;
        m_limit = m_start + capacity;
    } else if ((size + needed) > m_data->capacity) {
        // Reallocate to create more capacity.
        reallocate(capacity);
    } else {
        // We have enough capacity - just fix the append limit.
        // This can happen when an array is copied and then the
        // copy is removed.
        m_limit = m_start + m_data->capacity;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::setSize(int size)
{
    if (size <= PreallocSize) {
        initPrealloc();
        m_data = 0;
    } else {
        int capacity = qArrayAllocMore(size, 0, sizeof(T));
        Data *data = reinterpret_cast<Data *>
            (qMalloc(sizeof(Data) + sizeof(T) * (capacity - 1)));
        Q_CHECK_PTR(data);
        m_data = data;
        m_data->ref = 1;
        m_data->capacity = capacity;
        m_start = m_data->array;
        m_end = m_start;
        m_limit = m_start + capacity;
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray()
{
    initPrealloc();
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray(int size)
{
    setSize(size);
    while (size-- > 0)
        new (m_end++) T();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray(int size, const T &value)
{
    setSize(size);
    while (size-- > 0)
        new (m_end++) T(value);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray(const T *values, int size)
{
    setSize(size);
    while (size-- > 0)
        new (m_end++) T(*values++);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray(const QSGArray<T, PreallocSize> &other)
{
    assign(other);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::QSGArray(const T *data, int size, bool isWritable)
{
    // Constructing a raw data array.
    m_start = const_cast<T *>(data);
    m_end = m_start + size;
    if (isWritable)
        m_limit = m_end;
    else
        m_limit = m_start;
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize>::~QSGArray()
{
    release();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::operator=(const QSGArray<T, PreallocSize> &other)
{
    if (this == &other)
        return *this;
    if (other.m_data && m_data == other.m_data)
        return *this;
    release();
    assign(other);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QSGArray<T, PreallocSize>::size() const
{
    return m_end - m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QSGArray<T, PreallocSize>::count() const
{
    return m_end - m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE int QSGArray<T, PreallocSize>::capacity() const
{
    if (m_data)
        return m_data->capacity;
    else if (isPrealloc(m_start))
        return PreallocSize;
    else
        return m_end - m_start; // raw data, m_limit == m_start
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QSGArray<T, PreallocSize>::isEmpty() const
{
    return m_start == m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QSGArray<T, PreallocSize>::isDetached() const
{
    // If m_limit is the same as m_start, then the array
    // is either shared or contains raw data.
    return m_limit != m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::detach()
{
    if (m_limit == m_start)
        detach_helper();
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::clear()
{
    release();
    initPrealloc();
    m_data = 0;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T &QSGArray<T, PreallocSize>::operator[](int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QSGArray<T>::at", "index out of range");
    return m_start[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T &QSGArray<T, PreallocSize>::at(int index) const
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QSGArray<T>::operator[]", "index out of range");
    return m_start[index];
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T &QSGArray<T, PreallocSize>::operator[](int index)
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QSGArray<T>::operator[]", "index out of range");
    return data()[index];
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE T QSGArray<T, PreallocSize>::value(int index) const
{
    if (index >= 0 && index < size())
        return m_start[index];
    else
        return T();
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE T QSGArray<T, PreallocSize>::value(int index, const T &defaultValue) const
{
    if (index >= 0 && index < size())
        return m_start[index];
    else
        return defaultValue;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QSGArray<T, PreallocSize>::extend(int size)
{
    Q_ASSERT(size > 0);
    if ((m_end + size) >= m_limit)
        grow(size);
    T *end = m_end;
    m_end += size;  // Note: new elements are not initialized.
    return end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const T &value)
{
    if (m_end >= m_limit)
        grow(1);
    new (m_end) T(value);
    ++m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const T &value1, const T &value2)
{
    if ((m_end + 1) >= m_limit)
        grow(2);
    new (m_end) T(value1);
    ++m_end; // Increment one at a time in case an exception is thrown.
    new (m_end) T(value2);
    ++m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const T &value1, const T &value2, const T &value3)
{
    if ((m_end + 2) >= m_limit)
        grow(3);
    new (m_end) T(value1);
    ++m_end;
    new (m_end) T(value2);
    ++m_end;
    new (m_end) T(value3);
    ++m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const T &value1, const T &value2, const T &value3, const T &value4)
{
    if ((m_end + 3) >= m_limit)
        grow(4);
    new (m_end) T(value1);
    ++m_end;
    new (m_end) T(value2);
    ++m_end;
    new (m_end) T(value3);
    ++m_end;
    new (m_end) T(value4);
    ++m_end;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const T *values, int count)
{
    if (count <= 0)
        return;
    if (!m_start || (m_end + count) > m_limit)
        grow(count);
    while (count-- > 0) {
        new (m_end) T(*values++);
        ++m_end;
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::append(const QSGArray<T, PreallocSize> &other)
{
    if (isEmpty()) {
        *this = other;
    } else {
        if (&other == this || (m_data && other.m_data == m_data))
            grow(size());   // Appending to ourselves: make some room.
        append(other.constData(), other.size());
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::prepend(const T &value)
{
    insert(begin(), 1, value);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::insert(int index, const T &value)
{
    Q_ASSERT_X(index >= 0 && index <= size(),
               "QSGArray<T>::insert", "index out of range");
    insert(begin() + index, 1, value);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::insert(int index, int count, const T &value)
{
    Q_ASSERT_X(index >= 0 && index <= size(),
               "QSGArray<T>::insert", "index out of range");
    insert(begin() + index, count, value);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE Q_TYPENAME QSGArray<T, PreallocSize>::iterator QSGArray<T, PreallocSize>::insert(iterator before, int count, const T &value)
{
    // Check the parameters.
    int size = this->size();
    int offset = int(before - m_start);
    Q_ASSERT_X(offset >= 0 && offset <= size,
               "QSGArray<T>::insert", "iterator offset is out of range");
    Q_ASSERT(count >= 0);
    if (count <= 0)
        return m_start + offset;

    // Reserve extra space and then copy-on-write.
    reserve(size + count);
    detach();

    // Move items up to make room, and replace at the insert point.
    if (QTypeInfo<T>::isStatic) {
        int newcount = count;
        while (newcount > 0) {
            new (m_end++) T();
            --newcount;
        }
        int posn = size;
        while (posn > offset) {
            --posn;
            m_start[posn + count] = m_start[posn];
        }
        while (count > 0) {
            --count;
            m_start[offset + count] = value;
        }
    } else {
        ::memmove(m_start + offset + count, m_start + offset,
                  (size - offset) * sizeof(T));
        m_end += count;
        while (count > 0) {
            --count;
            new (m_start + offset + count) T(value);
        }
    }

    // Return the new iterator at the insert position.
    return m_start + offset;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE Q_TYPENAME QSGArray<T, PreallocSize>::iterator QSGArray<T, PreallocSize>::insert(iterator before, const T &value)
{
    return insert(before, 1, value);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::replace(int index, const T &value)
{
    Q_ASSERT_X(index >= 0 && index < size(),
               "QSGArray<T>::replace", "index out of range");
    data()[index] = value;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::replace(int index, const T *values, int count)
{
    if (index < 0 || count <= 0)
        return;
    int replaceSize = index + count;
    if (replaceSize > size())
        resize(replaceSize);
    copyReplace(data() + index, values, count);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::remove(int index)
{
    remove(index, 1);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::remove(int index, int count)
{
    // Truncate the range to be removed.
    int currentSize = size();
    if (index < 0) {
        count += index;
        index = 0;
    }
    if (count > 0 && (index + count) > currentSize)
        count = currentSize - index;
    if (count <= 0)
        return;

    // Perform the removal.
    if (index == 0 && count >= currentSize) {
        clear();
        return;
    }
    T *start = data();
    copyReplace(start + index, start + index + count,
                (currentSize - (index + count)));
    resize(currentSize - count);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE Q_TYPENAME QSGArray<T, PreallocSize>::iterator QSGArray<T, PreallocSize>::erase(iterator begin, iterator end)
{
    int index = begin - m_start;
    remove(index, end - begin);
    return m_start + index;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE Q_TYPENAME QSGArray<T, PreallocSize>::iterator QSGArray<T, PreallocSize>::erase(iterator pos)
{
    int index = pos - m_start;
    remove(index, 1);
    return m_start + index;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE int QSGArray<T, PreallocSize>::indexOf(const T &value, int from) const
{
    if (from < 0)
        from = qMax(from + size(), 0);
    const T *ptr = m_start + from;
    while (ptr < m_end) {
        if (*ptr == value)
            return ptr - m_start;
        ++ptr;
    }
    return -1;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE int QSGArray<T, PreallocSize>::lastIndexOf(const T &value, int from) const
{
    int size = count();
    if (from < 0)
        from += size;
    else if (from >= size)
        from = size - 1;
    if (from >= 0) {
        const T *ptr = m_start + from;
        while (ptr >= m_start) {
            if (*ptr == value)
                return ptr - m_start;
            --ptr;
        }
    }
    return -1;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QSGArray<T, PreallocSize>::contains(const T &value) const
{
    const T *ptr = m_start;
    while (ptr < m_end) {
        if (*ptr == value)
            return true;
        ++ptr;
    }
    return false;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE int QSGArray<T, PreallocSize>::count(const T &value) const
{
    const T *ptr = m_start;
    int count = 0;
    while (ptr < m_end) {
        if (*ptr == value)
            ++count;
        ++ptr;
    }
    return count;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::resize(int size)
{
    if (size < 0)
        return;
    int currentSize = count();
    if (size < currentSize) {
        T *start = data();  // Force copy on write if necessary.
        if (QTypeInfo<T>::isComplex)
            free(start + size, currentSize - size);
        m_end = start + size;
    } else if (size > currentSize) {
        grow(size - currentSize);
        while (currentSize++ < size) {
            new (m_end) T();
            ++m_end;
        }
    }
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE void QSGArray<T, PreallocSize>::reserve(int size)
{
    int cap = capacity();
    if (size > cap)
        grow(size - this->size());
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::fill(const T &value, int size)
{
    if (size >= 0)
        resize(size);
    T *ptr = m_start;
    while (ptr < m_end)
        *ptr++ = value;
    return *this;
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::squeeze()
{
    int size = count();
    if (size <= 0) {
        clear();
    } else if (size < capacity() && m_data) {
        reallocate(size);
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE void QSGArray<T, PreallocSize>::reverse()
{
    if (count() > 0) {
        T *src = m_start;
        T *dst = m_end - 1;
        while (src < dst)
            qSwap(*(dst--), *(src++));
    }
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::reversed() const
{
    QSGArray<T, PreallocSize> result;
    int count = size();
    if (count > 0) {
        result.extend(count);
        const T *src = m_start;
        T *dst = result.m_end - 1;
        if (!QTypeInfo<T>::isComplex) {
            while (src != m_end)
                *(dst--) = *(src++);
        } else {
            while (src != m_end)
                new (dst--) T(*src++);
        }
    }
    return result;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::mid(int index, int length) const
{
    int count = size();
    Q_ASSERT(index >= 0 && index <= count);
    if (length < 0 || (index + length) > count)
        length = count - index;
    if (index == 0 && length == count)
        return *this;
    QSGArray<T, PreallocSize> result;
    result.append(constData() + index, length);
    return result;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::left(int length) const
{
    return mid(0, length);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::right(int length) const
{
    int size = count();
    if (length < 0 || length >= size)
        length = size;
    return mid(size - length, length);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE T *QSGArray<T, PreallocSize>::data()
{
    detach();
    return m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QSGArray<T, PreallocSize>::data() const
{
    return m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE const T *QSGArray<T, PreallocSize>::constData() const
{
    return m_start;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::fromRawData(const T *data, int size)
{
    Q_ASSERT(size >= 0);
    return QSGArray<T, PreallocSize>(data, size, false);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> QSGArray<T, PreallocSize>::fromWritableRawData(T *data, int size)
{
    Q_ASSERT(size >= 0);
    return QSGArray<T, PreallocSize>(data, size, true);
}

template <typename T, int PreallocSize>
Q_OUTOFLINE_TEMPLATE bool QSGArray<T, PreallocSize>::operator==
    (const QSGArray<T, PreallocSize> &other) const
{
    if (this == &other)
        return true;
    const T *thisData = constData();
    const T *otherData = other.constData();
    if (thisData == otherData)
        return true;
    int thisCount = count();
    int otherCount = other.count();
    if (thisCount == 0 && otherCount == 0)
        return true;
    if (thisCount != otherCount)
        return false;
    for (int index = 0; index < thisCount; ++index, ++thisData, ++otherData)
        if (*thisData != *otherData)
            return false;
    return true;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE bool QSGArray<T, PreallocSize>::operator!=
    (const QSGArray<T, PreallocSize> &other) const
{
    return !(*this == other);
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::operator+=(const T &value)
{
    append(value);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::operator+=(const QSGArray<T, PreallocSize> &other)
{
    append(other);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::operator<<(const T &value)
{
    append(value);
    return *this;
}

template <typename T, int PreallocSize>
Q_INLINE_TEMPLATE QSGArray<T, PreallocSize> &QSGArray<T, PreallocSize>::operator<<(const QSGArray<T, PreallocSize> &other)
{
    append(other);
    return *this;
}

#ifndef QT_NO_DATASTREAM

template <typename T, int PreallocSize>
QDataStream& operator<<(QDataStream& stream, const QSGArray<T, PreallocSize>& array)
{
    int size = array.size();
    stream << quint32(size);
    for (int index = 0; index < size; ++index)
        stream << array.at(index);
    return stream;
}

template <typename T, int PreallocSize>
QDataStream& operator>>(QDataStream& stream, QSGArray<T, PreallocSize>& array)
{
    array.clear();
    quint32 size;
    stream >> size;
    array.reserve(size);
    for (int index = 0; index < int(size); ++index) {
        T t;
        stream >> t;
        array.append(t);
        if (stream.atEnd())
            break;
    }
    return stream;
}

#endif

#ifndef QT_NO_DEBUG_STREAM

template <typename T, int PreallocSize>
QDebug operator<<(QDebug dbg, const QSGArray<T, PreallocSize>& array)
{
    dbg.nospace() << "QSGArray(\n";
    int size = array.size();
    for (int index = 0; index < size; ++index) {
        dbg << "   " << index << ":  " << array.at(index) << "\n";
    }
    dbg << ")\n";
    return dbg.space();
}

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
