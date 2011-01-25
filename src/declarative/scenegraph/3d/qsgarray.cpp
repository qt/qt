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

#include "qsgarray.h"
#include <limits.h>

QT_BEGIN_NAMESPACE

/*!
    \class QSGArray
    \brief The QSGArray class is a template class that provides a dynamic array of simple types.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    QSGArray is similar to QVector except that it has much less overhead
    when constructing large arrays by appending individual elements
    one by one.

    QSGArray instances have a preallocated data area for quickly
    building small arrays on the stack without malloc overhead.
    Once the array grows beyond the preallocated size, it is copied
    to the heap.  The size of the preallocated area, which defaults to 8,
    can be specified with the second template parameter:

    \code
    QSGArray<QVector3D, 32> array;
    \endcode

    QSGArray uses implicit sharing and copy-on-write semantics to support
    passing large arrays around an application with little overhead.

    QSGArray is heavily optimized for copy-on-write and the case of
    constructing an array by calling append().  It has a slight
    performance penalty for random access using the non-const
    version of operator[]().
*/

/*!
    \fn QSGArray::QSGArray()

    Constructs an empty array.

    \sa reserve()
*/

/*!
    \fn QSGArray::QSGArray(int size, const T &value)

    Constructs an array of \a size elements, all initialized
    to \a value.

    \sa fill()
*/

/*!
    \fn QSGArray::QSGArray(int size)

    Constructs an array of \a size elements, all initialized
    to their default-constructed values.
*/

/*!
    \fn QSGArray::QSGArray(const T *values, int size)

    Constructs an array of \a size elements, initialized
    from \a values.
*/

/*!
    \fn QSGArray::QSGArray(const QSGArray<T, PreallocSize> &other)

    Constructs a copy of \a other.

    \sa operator=()
*/

/*!
    \fn QSGArray::~QSGArray()

    Destroys the array.
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::operator=(const QSGArray<T, PreallocSize> &other)

    Assigns \a other to this array and returns a reference
    to this array.
*/

/*!
    \fn int QSGArray::size() const

    Returns the number of elements in this array.

    \sa resize(), capacity(), isEmpty()
*/

/*!
    \fn int QSGArray::count() const
    \overload

    Same as size(), provided for convenience.
*/

/*!
    \fn int QSGArray::capacity() const

    Returns the number of elements that can be stored in this
    array before reallocation.

    \sa reserve(), size()
*/

/*!
    \fn bool QSGArray::isEmpty() const

    Returns true if this array is empty; false otherwise.

    \sa size(), clear()
*/

/*!
    \fn bool QSGArray::isDetached() const
    \internal

    Returns true if this array has definitely been detached from all
    other shared copies of the data; false otherwise.

    It is possible for this function to return false if the
    array was previously shared but no longer is.  It is thus
    an indication that a detach() will probably be required.

    This function can be used to determine if functions that
    write to this array such as append(), replace(),
    and data(), will need to make a copy.

    Raw data arrays that are created with fromRawData() are
    never detached.

    \sa detach()
*/

/*!
    \fn void QSGArray::detach()
    \internal

    Detaches this array from all other shared copies of the data.

    \sa isDetached()
*/

/*!
    \fn void QSGArray::clear()

    Clears all elements from this array and sets the size to zero.

    This function will deallocate any memory that is used on the heap
    to store the array's elements.  To reuse the same memory
    as before, call resize() with an argument of zero.

    \sa resize(), isEmpty()
*/

/*!
    \fn const T &QSGArray::at(int index) const

    Returns the item at position \a index in the array.

    \a index must be a valid index position in the array (i.e., 0 <= \a
    index < size()).

    \sa operator[](), constData(), value()
*/

/*!
    \fn T &QSGArray::operator[](int index)

    Returns the item at position \a index as a modifiable reference.

    \a index must be a valid index position in the vector (i.e., 0 <= \a index
    < size()).

    Note that using non-const operators can cause QSGArray
    to do a deep copy.

    \sa at(), value()
*/

/*!
    \fn const T &QSGArray::operator[](int index) const

    \overload

    Same as at(\a index).
*/

/*!
    \fn T QSGArray::value(int index) const

    Returns the value at position \a index in the vector.

    If the \a index is out of bounds, the function returns
    a default-constructed value.  If you are certain that
    \a index is within bounds, you can use at() instead,
    which is slightly faster.

    \sa at(), operator[]()
*/

/*!
    \fn T QSGArray::value(int index, const T &defaultValue) const
    \overload

    If the \a index is out of bounds, the function returns
    \a defaultValue.
*/

/*!
    \fn T *QSGArray::extend(int size)

    Extends this array by \a size elements and returns a pointer
    to the storage, which is not initialized.  The pointer is only
    valid until the array is reallocated or destroyed.

    The append() or resize() functions are recommended if T is a
    complex type, with extend() only used for simple types.
    Because the storage is not initialized, the caller should use
    the in-place new operator to set elements:

    \code
    QSGArray<QRegExp> array;
    QRegExp *space = array.extend(1);
    new (space) QRegExp(QLatin1String("exp"));
    \endcode

    \sa append(), resize()
*/

/*!
    \fn void QSGArray::append(const T &value)

    Appends \a value to this array.

    \sa prepend(), insert()
*/

/*!
    \fn void QSGArray::append(const T &value1, const T &value2)

    \overload

    Appends \a value1 and \a value2 to this array.
*/

/*!
    \fn void QSGArray::append(const T &value1, const T &value2, const T &value3)

    \overload

    Appends \a value1, \a value2, and \a value3 to this array.
*/

/*!
    \fn void QSGArray::append(const T &value1, const T &value2, const T &value3, const T &value4)

    \overload

    Appends \a value1, \a value2, \a value3, and \a value4 to this array.
*/

/*!
    \fn void QSGArray::append(const T *values, int count)

    Appends the \a count elements of \a values to this array.
*/

/*!
    \fn void QSGArray::append(const QSGArray<T, PreallocSize> &other)

    Appends the elements of \a other to this array.
*/

/*!
    \fn void QSGArray::prepend(const T &value)

    Prepends \a value to this array.

    \sa append(), insert()
*/

/*!
    \fn void QSGArray::insert(int index, const T &value)

    Inserts \a value at position \a index in this array.
    If \a index is 0, then \a value is prepended to the array.
    If \a index is size(), then \a value is appended to the array.

    \sa append(), prepend()
*/

/*!
    \fn void QSGArray::insert(int index, int count, const T &value)
    \overload

    Inserts \a count copies of \a value at position \a index
    in this array.
*/

/*!
    \fn QSGArray::iterator QSGArray::insert(iterator before, int count, const T &value)

    Inserts \a count copies of \a value in front of the item
    pointed to by the iterator \a before.  Returns an iterator
    pointing at the first of the inserted items.
*/

/*!
    \fn QSGArray::iterator QSGArray::insert(iterator before, const T &value)
    \overload

    Inserts \a value in front of the item pointed to by the
    iterator \a before.  Returns an iterator pointing at the
    inserted item.
*/

/*!
    \fn void QSGArray::replace(int index, const T &value)

    Replaces the element at \a index with \a value.

    \sa operator[](), remove()
*/

/*!
    \fn void QSGArray::replace(int index, const T *values, int count)
    \overload

    Replaces the \a count elements of this array with the
    contents of \a values, starting at \a index.

    If (\a index + \a count) is larger than the current size of this
    array, the array will be extended to that size.

    \sa append()
*/

/*!
    \fn void QSGArray::remove(int index)

    \overload

    Removes the element at position \a index in this array.
*/

/*!
    \fn void QSGArray::remove(int index, int count)

    Removes the \a count elements starting at position \a index
    in this array.  If \a index or \a count is out of range,
    the set of removed elements will be truncated to those that
    are in range.
*/

/*!
    \fn QSGArray::iterator QSGArray::erase(iterator begin, iterator end)
    \overload

    Removes all the items from \a begin up to (but not including) \a
    end. Returns an iterator to the same item that \a end referred to
    before the call.
*/

/*!
    \fn QSGArray::iterator QSGArray::erase(iterator pos)

    Removes the item pointed to by the iterator \a pos from the
    vector, and returns an iterator to the next item in the vector
    (which may be end()).

    \sa insert(), remove()
*/

/*!
    \fn void QSGArray::removeFirst()

    Removes the first element from this array.  Does nothing if
    the array is empty.

    \sa remove(), removeLast()
*/

/*!
    \fn void QSGArray::removeLast()

    Removes the last element from this array.  Does nothing if
    the array is empty.

    \sa remove(), removeFirst()
*/

/*!
    \fn int QSGArray::indexOf(const T &value, int from) const

    Returns the index position of the first occurrence of
    \a value in the array, searching forward from index
    position \a from.  Returns -1 if no item matched.

    If \a from is negative, then it indicates an index position
    relative to the end of the array, -1 being the last index
    position.

    This function requires the value type T to have an implementation
    of \c operator==().

    \sa lastIndexOf(), contains()
*/

/*!
    \fn int QSGArray::lastIndexOf(const T &value, int from) const

    Returns the index position of the last occurrence of
    \a value in the array, searching backward from index
    position \a from.  Returns -1 if no item matched.

    If \a from is negative, then it indicates an index position
    relative to the end of the array, -1 being the last index
    position.  The default for \a from is -1.

    This function requires the value type T to have an implementation
    of \c operator==().

    \sa indexOf(), contains()
*/

/*!
    \fn bool QSGArray::contains(const T &value) const

    Returns true if the array contains an occurrence of \a value;
    false otherwise.

    This function requires the value type T to have an implementation
    of \c operator==().

    \sa indexOf(), count()
*/

/*!
    \fn int QSGArray::count(const T &value) const

    Returns the number of occurrences of \a value in the array.

    This function requires the value type T to have an implementation
    of \c operator==().

    \sa contains(), indexOf()
*/

/*!
    \fn void QSGArray::resize(int size)

    Sets the size of the array to \a size.  If \a size is greater
    than the current size, elements are added to the end and are
    initialized to a default-constructed value.  If \a size is less
    than the current size, elements are removed from the end.

    \sa size(), reserve(), squeeze()
*/

/*!
    \fn void QSGArray::reserve(int size)

    Increases the capacity of this array to reserve space for
    at least \a size elements.  If the capacity is already larger
    than \a size, this function does nothing; in particular, it does
    not remove elements from the array like resize() does.

    This function can be useful when you know how roughly many elements
    will be appended ahead of time.  Reserving the space once can avoid
    unnecessary realloc operations later.

    \sa capacity(), resize(), squeeze()
*/

/*!
    \fn void QSGArray::squeeze()

    Releases any memory not required to store the array's elements
    by reducing its capacity() to size().

    This function is intended for reclaiming memory in an
    array that is being used over and over with different contents.
    As elements are added to an array, it will be constantly
    expanded in size.  This function can realloc the array
    to a smaller size to reclaim unused memory.

    \sa reserve(), capacity()
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::fill(const T &value, int size)

    Assigns \a value to all items in the array. If \a size is
    different from -1 (the default), the array is resized to
    \a size beforehand.  Returns a reference to the array.

    \sa resize()
*/

/*!
    \fn void QSGArray::reverse()

    Reverses the order of this array in place.

    \sa reversed()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::reversed() const

    Returns a copy of this array with elements in the reverse order.

    \sa reverse()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::mid(int index, int length) const

    Returns an array containing the \a length elements of
    this array, starting at \a index.  If \a length is less
    than zero, or extends further than the end of the array, then all
    elements extending from \a index to the end of the array will be
    included in the return value.

    \sa left(), right()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::left(int length) const;

    Returns an array containing the first \a length
    elements of this array.  If \a length is less than zero,
    or greater than size(), then all elements in this array will
    be included in the return value.

    \sa mid(), right()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::right(int length) const;

    Returns an array containing the last \a length
    elements of this array.  If \a length is less than zero,
    or greater than size(), then all elements in this array
    will be included in the return value.

    \sa mid(), left()
*/

/*!
    \fn T *QSGArray::data()

    Returns a pointer to the data stored in the array.  The pointer
    can be used to access and modify the items in the array.

    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.  It may make a deep copy of the
    array's elements if the array is implicitly shared.

    \sa constData(), operator[]()
*/

/*!
    \fn const T *QSGArray::data() const

    \overload
*/

/*!
    \fn const T *QSGArray::constData() const

    Returns a const pointer to the data stored in the array.
    The pointer can be used to access the items in the array.
    The pointer remains valid as long as the array isn't
    reallocated.

    This function is mostly useful to pass an array to a function
    that accepts a plain C++ array.

    \sa data(), operator[]()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::fromRawData(const T *data, int size)

    Returns an array consisting of the \a size elements from \a data.

    This function takes a reference to \a data, but does not copy
    the elements until the array is modified.  The memory at \a data
    must remain valid until the returned array is destroyed
    or modified.

    Use append() instead of fromRawData() to force a copy to be made
    of the elements at \a data when the array is created:

    \code
    // Makes a copy of the data immediately.
    QSGArray<float> array;
    array.append(data, size);

    // Does not make a copy of the data until the array is modified.
    QSGArray<float> array;
    array = QSGArray<float>::fromRawData(data, size);
    \endcode

    \sa fromWritableRawData(), append()
*/

/*!
    \fn QSGArray<T, PreallocSize> QSGArray::fromWritableRawData(T *data, int size)

    Returns an array consisting of the \a size elements from \a data.

    This function takes a reference to \a data, but does not copy
    the elements until the array is reallocated to a larger size.
    The memory at \a data must remain valid until the returned
    array is destroyed or reallocated.

    The elements of \a data will be modified in-place.  This differs
    from fromRawData() which will make a copy of the elements
    of \a data when the array is modified.

    If the returned array is resized to less than \a size,
    then a copy will not be made, and append() can be used to
    append new items up to \a size.  Further calls to append()
    after \a size will force the array to be reallocated.

    If the returned array is resized to more than \a size,
    then a copy of the data will be made and further modifications
    will not affect the elements at \a data.

    \sa fromRawData()
*/

/*!
    \fn bool QSGArray::operator==(const QSGArray<T, PreallocSize> &other) const

    Returns true if \a other is equal to this array; otherwise
    returns false.

    Two arrays are considered equal if they contain the same values
    in the same order.

    This function requires the value type to have an implementation
    of \c operator==().

    \sa operator!=()
*/

/*!
    \fn bool QSGArray::operator!=(const QSGArray<T, PreallocSize> &other) const

    Returns true if \a other is not equal to this array; otherwise
    returns false.

    Two arrays are considered equal if they contain the same values
    in the same order.

    This function requires the value type to have an implementation
    of \c operator==().

    \sa operator==()
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::operator+=(const T &value)

    \overload

    Appends \a value to this array and returns a reference to
    this array.

    \sa operator<<(), append()
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::operator+=(const QSGArray<T, PreallocSize> &other)

    Appends the elements of the \a other array to this array
    and returns a reference to this array.

    \sa operator<<(), append()
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::operator<<(const T &value)

    \overload

    Appends \a value to this array and returns a reference to
    this array.

    \sa operator+=(), append()
*/

/*!
    \fn QSGArray<T, PreallocSize> &QSGArray::operator<<(const QSGArray<T, PreallocSize> &other)

    Appends the elements of the \a other array to this array
    and returns a reference to this array.

    \sa operator+=(), append()
*/

/*!
    \typedef QSGArray::iterator

    The QSGArray::iterator typedef provides an STL-style non-const
    iterator for QSGArray.  The iterator is simply a typedef
    for "T *" (pointer to T).

    \sa QSGArray::begin(), QSGArray::const_iterator
*/

/*!
    \typedef QSGArray::const_iterator

    The QSGArray::iterator typedef provides an STL-style const
    iterator for QSGArray.  The iterator is simply a typedef
    for "const T *" (pointer to const T).

    \sa QSGArray::constBegin(), QSGArray::iterator
*/

/*!
    \typedef QSGArray::Iterator

    Qt-style synonym for QSGArray::iterator.
*/

/*!
    \typedef QSGArray::ConstIterator

    Qt-style synonym for QSGArray::const_iterator.
*/

/*!
    \typedef QSGArray::const_pointer

    Typedef for const T *. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::const_reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::difference_type

    Typedef for ptrdiff_t. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::pointer

    Typedef for T *. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::reference

    Typedef for T &. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::size_type

    Typedef for int. Provided for STL compatibility.
*/

/*!
    \typedef QSGArray::value_type

    Typedef for T. Provided for STL compatibility.
*/

/*!
    \fn QSGArray::iterator QSGArray::begin()

    Returns an STL-style iterator pointing to the first item
    in the array.

    \sa end(), constBegin(), QSGArray::iterator
*/

/*!
    \fn QSGArray::const_iterator QSGArray::begin() const
    \overload
*/

/*!
    \fn QSGArray::const_iterator QSGArray::constBegin() const

    Returns a const STL-style iterator pointing to the first item
    in the array.

    \sa constEnd(), begin(), QSGArray::const_iterator
*/

/*!
    \fn QSGArray::iterator QSGArray::end()

    Returns an STL-style iterator pointing to the imaginary item
    after the last item in the array.

    \sa begin(), constEnd(), QSGArray::iterator
*/

/*!
    \fn QSGArray::const_iterator QSGArray::end() const
    \overload
*/

/*!
    \fn QSGArray::const_iterator QSGArray::constEnd() const

    Returns a const STL-style iterator pointing to the imaginary item
    after the last item in the array.

    \sa constBegin(), end(), QSGArray::const_iterator
*/

/*!
    \fn T &QSGArray::first()

    Returns a reference to the first item in the array. This
    function assumes that the array isn't empty.

    \sa last(), isEmpty()
*/

/*!
    \fn const T &QSGArray::first() const
    \overload
*/

/*!
    \fn T &QSGArray::last()

    Returns a reference to the last item in the array. This function
    assumes that the array isn't empty.

    \sa first(), isEmpty()
*/

/*!
    \fn const T &QSGArray::last() const
    \overload
*/

/*!
    \fn bool QSGArray::startsWith(const T &value) const

    Returns true if this array is not empty and its first
    item is equal to \a value; otherwise returns false.

    \sa isEmpty(), first()
*/

/*!
    \fn bool QSGArray::endsWith(const T &value) const

    Returns true if this array is not empty and its last
    item is equal to \a value; otherwise returns false.

    \sa isEmpty(), last()
*/

/*!
    \fn void QSGArray::push_back(const T &value)

    This function is provided for STL compatibility. It is equivalent
    to append(\a value).
*/

/*!
    \fn void QSGArray::push_front(const T &value)

    This function is provided for STL compatibility. It is equivalent
    to prepend(\a value).
*/

/*!
    \fn void QSGArray::pop_front()

    This function is provided for STL compatibility. It is equivalent
    to removeFirst().
*/

/*!
    \fn void QSGArray::pop_back()

    This function is provided for STL compatibility. It is equivalent
    to removeLast().
*/

/*!
    \fn QSGArray::reference QSGArray::front()

    This function is provided for STL compatibility. It is equivalent
    to first().
*/

/*!
    \fn QSGArray::const_reference QSGArray::front() const
    \overload
*/

/*!
    \fn QSGArray::reference QSGArray::back()

    This function is provided for STL compatibility. It is equivalent
    to last().
*/

/*!
    \fn QSGArray::const_reference QSGArray::back() const
    \overload
*/

/*!
    \fn bool QSGArray::empty() const

    This function is provided for STL compatibility. It is equivalent
    to isEmpty(), returning true if the array is empty; otherwise
    returns false.
*/

#ifndef QT_NO_DATASTREAM

/*!
    \fn QDataStream& operator<<(QDataStream& stream, const QSGArray<T, PreallocSize>& array)
    \relates QSGArray

    Writes \a array to the given \a stream and returns a reference
    to the \a stream.
*/

/*!
    \fn QDataStream& operator>>(QDataStream& stream, QSGArray<T, PreallocSize>& array)
    \relates QSGArray

    Reads \a array from the given \a stream and returns a reference
    to the \a stream.
*/

#endif

int qArrayAllocMore(int alloc, int extra, int sizeOfT)
{
    if (alloc == 0 && extra == 0)
        return 0;
    const int page = 1 << 12;
    int nalloc;
    alloc += extra;
    alloc *= sizeOfT;
    // don't do anything if the loop will overflow signed int.
    if (alloc >= INT_MAX/2)
        return INT_MAX / sizeOfT;
    nalloc = (alloc < page) ? 64 : page;
    while (nalloc < alloc) {
        if (nalloc <= 0)
            return INT_MAX / sizeOfT;
        nalloc *= 2;
    }
    return nalloc / sizeOfT;
}

QT_END_NAMESPACE
