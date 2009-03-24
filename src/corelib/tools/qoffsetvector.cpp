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

#include "qoffsetvector.h"
#include <QDebug>

void QOffsetVectorData::dump() const
{
    qDebug() << "capacity:" << alloc;
    qDebug() << "count:" << count;
    qDebug() << "start:" << start;
    qDebug() << "offset:" << offset;
}

/*! \class QOffsetVector
    \brief The QOffsetVector class is a template class that provides a offset vector.
    \ingroup tools
    \ingroup shared
    \reentrant

    The QOffsetVector class provides an efficient way of caching items for
    display in a user interface view.  It does this by providing a window
    into a theoretical infinite sized vector.  This has the advantage that
    it matches how user interface views most commonly request the data, in
    a set of rows localized around the current scrolled position.  It also
    allows the cache to use less overhead than QCache both in terms of
    performance and memory.  In turn, unlike a QCache, the key has to be
    an int and has to be contiguous.  That is to say if an item is inserted
    at index 85, then if there were no previous items at 84 or 86 then the
    cache will be cleared before the new item at 85 is inserted.  If this
    restriction is not suitable consider using QCache instead.

    The simplest way of using an offset vector is to use the append()
    and prepend() functions to slide the window to where it is needed.

\code
MyRecord record(int row) const
{
    Q_ASSERT(row >= 0 && row < count());

    while(row > cache.lastIndex())
        cache.append(slowFetchRecord(cache.lastIndex()+1));
    while(row < cache.firstIndex())
        cache.prepend(slowFetchRecord(cache.firstIndex()-1));

    return cache.at(row);
}
\endcode
    
    The append() and prepend() functions cause the vector window to move to
    where the current row is requested from.  This usage can be further
    optimized by using the insert() function to reset the vector window to
    a row in the case where the row is a long way from the current row.  It
    may also be worth while to fetch multiple records into the cache if
    it is faster to retrieve them in a batch operation.

    See the The \l{Offset Vector Example}{Offset Vector} example.
*/

/*! \fn QOffsetVector::QOffsetVector(int capacity)

    Constructs a vector with the given \a capacity.

    \sa setCapacity()
*/

/*! \fn QOffsetVector::QOffsetVector(const QOffsetVector<T> &other)

    Constructs a copy of \a other.

    This operation takes \l{constant time}, because QOffsetVector is
    \l{implicitly shared}.  This makes returning a QOffsetVector from a
    function very fast.  If a shared instance is modified, it will be
    copied (copy-on-write), and that takes \l{linear time}.

    \sa operator=()
*/

/*! \fn QOffsetVector::~QOffsetVector()
    Destorys the vector.
*/

/*! \fn void QOffsetVector::detach()

    \internal
*/

/*! \fn bool QOffsetVector::isDetached() const

    \internal
*/

/*! \fn void QOffsetVector::setSharable(bool sharable)

    \internal
*/


/*! \fn QOffsetVector<T> &QOffsetVector::operator=(const QOffsetVector<T> &other)

    Assigns \a other to this vector and returns a reference to this vector.
*/

/*! \fn bool QOffsetVector::operator==(const QOffsetVector<T> &other) const

    Returns true if \a other is equal to this vector; otherwise returns false.

    Two vectors are considered equal if they contain the same values at the same
    indexes.  This function requires the value type to implement the \c operator==().

    \sa operator!=()
*/

/*! \fn bool QOffsetVector::operator!=(const QOffsetVector<T> &other) const

    Returns true if \a other is not equal to this vector; otherwise
    returns false.

    Two vector are considered equal if they contain the same values at the same
    indexes.  This function requires the value type to implement the \c operator==().

    \sa operator==()
*/

/*! \fn int QOffsetVector::capacity() const
    
    Returns the number of items the vector can store before it is full.
    When a vector contains a number of items equal to its capacity, adding new
    items will cause items furthest from the added item to be removed.

    \sa setCapacity(), size()
*/

/*! \fn int QOffsetVector::count() const

    \overload

    Same as size().
*/

/*! \fn int QOffsetVector::size() const

    Returns the number of items contained within the vector.

    \sa capacity()
*/

/*! \fn bool QOffsetVector::isEmpty() const

    Returns true if no items are stored within the vector.

    \sa size(), capacity()
*/

/*! \fn bool QOffsetVector::isFull() const

    Returns true if the number of items stored within the vector is equal
    to the capacity of the vector.

    \sa size(), capacity()
*/

/*! \fn int QOffsetVector::available() const

    Returns the number of items that can be added to the vector before it becomes full.

    \sa size(), capacity(), isFull()
*/

/*! \fn void QOffsetVector::clear()

    Removes all items from the vector.  The capacity is unchanged.
*/

/*! \fn void QOffsetVector::setCapacity(int size)

    Sets the capacity of the vector to the given \a size.  A vector can hold a
    number of items equal to its capacity.  When inserting, appending or prepending
    items to the vector, if the vector is already full then the item furthest from
    the added item will be removed.

    If the given \a size is smaller than the current count of items in the vector
    then only the last \a size items from the vector will remain.

    \sa capacity(), isFull()
*/

/*! \fn const T &QOffsetVector::at(int i) const

    Returns the item at index position \a i in the vector.  \a i must
    be a valid index position in the vector (i.e, firstIndex() <= \a i <= lastIndex()).

    The indexes in the vector refer to number of positions the item is from the
    first item appended into the vector.  That is to say a vector with a capacity of
    100, that has had 150 items appended will have a valid index range of
    50 to 149.  This allows inserting an retrieving items into the vector based
    on a theoretical infinite list

    \sa firstIndex(), lastIndex(), insert(), operator[]()
*/

/*! \fn T &QOffsetVector::operator[](int i)

    Returns the item at index position \a i as a modifiable reference.  If
    the vector does not contain an item at the given index position \a i
    then it will first insert an empty item at that position.

    In most cases it is better to use either at() or insert().

    Note that using non-const operators can cause QOffsetVector to do a deep
    copy.

    \sa insert(), at()
*/

/*! \fn const T &QOffsetVector::operator[](int i) const

    \overload

    Same as at(\a i).
*/

/*! \fn void QOffsetVector::append(const T &value)

    Inserts \a value at the end of the vector.  If the vector is already full
    the item at the start of the vector will be removed.

    \sa prepend(), insert(), isFull()
*/

/*! \fn void QOffsetVector::prepend(const T &value)

    Inserts \a value at the start of the vector.  If the vector is already full
    the item at the end of the vector will be removed.

    \sa append(), insert(), isFull()
*/

/*! \fn void QOffsetVector::insert(int i, const T &value)

    Inserts the \a value at the index position \a i.  If the vector already contains
    an item at \a i then that value is replaced.  If \a i is either one more than
    lastIndex() or one less than firstIndex() it is the equivalent to an append()
    or a prepend().

    If the given index \a i is not within the current range of the vector nor adjacent
    to the bounds of the vector's index range the vector is first cleared before
    inserting the item.  At this point the vector will have a size of 1.  It is worth
    while then taking effort to insert items in an order than starts adjacent to the
    current index range for the vector.

    \sa prepend(), append(), isFull(), firstIndex(), lastIndex()
*/

/*! \fn bool QOffsetVector::containsIndex(int i) const

    Returns true if the vector's index range includes the given index \a i.

    \sa firstIndex(), lastIndex()
*/

/*! \fn int QOffsetVector::firstIndex() const
    Returns the first valid index in the vector.  The index will be invalid if the
    vector is empty.  However the following code is valid even when the vector is empty:

    \code
    for (int i = vector.firstIndex(); i <= vector.lastIndex(); ++i)
        qDebug() << "Item" << i << "of the vector is" << vector.at(i);
    \endcode

    \sa capacity(), size(), lastIndex()
*/

/*! \fn int QOffsetVector::lastIndex() const

    Returns the last valid index in the vector.  If the vector is empty will return -1.

    \code
    for (int i = vector.firstIndex(); i <= vector.lastIndex(); ++i)
        qDebug() << "Item" << i << "of the vector is" << vector.at(i);
    \endcode

    \sa capacity(), size(), firstIndex()
*/


/*! \fn T &QOffsetVector::first()

    Returns a reference to the first item in the vector.  This function
    assumes that the vector isn't empty.

    \sa last(), isEmpty()
*/

/*! \fn T &QOffsetVector::last()

    Returns a reference to the last item in the vector.  This function
    assumes that the vector isn't empty.

    \sa first(), isEmpty()
*/

/*! \fn const T& QOffsetVector::first() const

    \overload
*/

/*! \fn const T& QOffsetVector::last() const

    \overload
*/

/*! \fn void QOffsetVector::removeFirst()
    
    Removes the first item from the vector.  This function assumes that
    the vector isn't empty.

    \sa removeLast()
*/

/*! \fn void QOffsetVector::removeLast()
    
    Removes the last item from the vector.  This function assumes that
    the vector isn't empty.

    \sa removeFirst()
*/

/*! \fn T QOffsetVector::takeFirst()
    
    Removes the first item in the vector and returns it.

    If you don't sue the return value, removeFirst() is more efficient.

    \sa takeLast(), removeFirst()
*/

/*! \fn T QOffsetVector::takeLast()
    
    Removes the last item in the vector and returns it.

    If you don't sue the return value, removeLast() is more efficient.

    \sa takeFirst(), removeLast()
*/

/*! \fn void QOffsetVector::dump() const

    \internal

    Sends information about the vector's internal structure to qDebug()
*/
