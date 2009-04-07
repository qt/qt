/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontiguouscache.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

void QContiguousCacheData::dump() const
{
    qDebug() << "capacity:" << alloc;
    qDebug() << "count:" << count;
    qDebug() << "start:" << start;
    qDebug() << "offset:" << offset;
}

/*! \class QContiguousCache
    \brief The QContiguousCache class is a template class that provides a contiguous cache.
    \ingroup tools
    \ingroup shared
    \reentrant

    The QContiguousCache class provides an efficient way of caching items for
    display in a user interface view.  Unlike QCache though it adds a restriction
    that elements within the cache are contiguous.  This has the advantage that
    of matching how user interface views most commonly request data, as
    a set of rows localized around the current scrolled position.  It also
    allows the cache to use less overhead than QCache both in terms of
    performance and memory.

    The simplest way of using an contiguous cache is to use the append()
    and prepend().

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
    
    If the cache is full then the item with the furthest index from where
    the new item is appended or prepended is removed.

    This usage can be further optimized by using the insert() function
    in the case where the requested row is a long way from  the currently cached
    items. If there is a is a gap between where the new item is inserted and the currently
    cached items then the existing cached items are first removed to retain
    the contiguous nature of the cache.  Hence it is important to take some care then
    when using insert() in order to avoid  to unwanted clearing of the cache.

    See the The \l{Contiguous Cache Example}{Contiguous Cache} example.
*/

/*! \fn QContiguousCache::QContiguousCache(int capacity)

    Constructs a cache with the given \a capacity.

    \sa setCapacity()
*/

/*! \fn QContiguousCache::QContiguousCache(const QContiguousCache<T> &other)

    Constructs a copy of \a other.

    This operation takes \l{constant time}, because QContiguousCache is
    \l{implicitly shared}.  This makes returning a QContiguousCache from a
    function very fast.  If a shared instance is modified, it will be
    copied (copy-on-write), and that takes \l{linear time}.

    \sa operator=()
*/

/*! \fn QContiguousCache::~QContiguousCache()
    Destroys the cache.
*/

/*! \fn void QContiguousCache::detach()

    \internal
*/

/*! \fn bool QContiguousCache::isDetached() const

    \internal
*/

/*! \fn void QContiguousCache::setSharable(bool sharable)

    \internal
*/


/*! \fn QContiguousCache<T> &QContiguousCache::operator=(const QContiguousCache<T> &other)

    Assigns \a other to this cache and returns a reference to this cache.
*/

/*! \fn bool QContiguousCache::operator==(const QContiguousCache<T> &other) const

    Returns true if \a other is equal to this cache; otherwise returns false.

    Two cache are considered equal if they contain the same values at the same
    indexes.  This function requires the value type to implement the \c operator==().

    \sa operator!=()
*/

/*! \fn bool QContiguousCache::operator!=(const QContiguousCache<T> &other) const

    Returns true if \a other is not equal to this cache; otherwise
    returns false.

    Two cache are considered equal if they contain the same values at the same
    indexes.  This function requires the value type to implement the \c operator==().

    \sa operator==()
*/

/*! \fn int QContiguousCache::capacity() const
    
    Returns the number of items the cache can store before it is full.
    When a cache contains a number of items equal to its capacity, adding new
    items will cause items furthest from the added item to be removed.

    \sa setCapacity(), size()
*/

/*! \fn int QContiguousCache::count() const

    \overload

    Same as size().
*/

/*! \fn int QContiguousCache::size() const

    Returns the number of items contained within the cache.

    \sa capacity()
*/

/*! \fn bool QContiguousCache::isEmpty() const

    Returns true if no items are stored within the cache.

    \sa size(), capacity()
*/

/*! \fn bool QContiguousCache::isFull() const

    Returns true if the number of items stored within the cache is equal
    to the capacity of the cache.

    \sa size(), capacity()
*/

/*! \fn int QContiguousCache::available() const

    Returns the number of items that can be added to the cache before it becomes full.

    \sa size(), capacity(), isFull()
*/

/*! \fn void QContiguousCache::clear()

    Removes all items from the cache.  The capacity is unchanged.
*/

/*! \fn void QContiguousCache::setCapacity(int size)

    Sets the capacity of the cache to the given \a size.  A cache can hold a
    number of items equal to its capacity.  When inserting, appending or prepending
    items to the cache, if the cache is already full then the item furthest from
    the added item will be removed.

    If the given \a size is smaller than the current count of items in the cache
    then only the last \a size items from the cache will remain.

    \sa capacity(), isFull()
*/

/*! \fn const T &QContiguousCache::at(int i) const

    Returns the item at index position \a i in the cache.  \a i must
    be a valid index position in the cache (i.e, firstIndex() <= \a i <= lastIndex()).

    The indexes in the cache refer to number of positions the item is from the
    first item appended into the cache.  That is to say a cache with a capacity of
    100, that has had 150 items appended will have a valid index range of
    50 to 149.  This allows inserting an retrieving items into the cache based
    on a theoretical infinite list

    \sa firstIndex(), lastIndex(), insert(), operator[]()
*/

/*! \fn T &QContiguousCache::operator[](int i)

    Returns the item at index position \a i as a modifiable reference.  If
    the cache does not contain an item at the given index position \a i
    then it will first insert an empty item at that position.

    In most cases it is better to use either at() or insert().

    Note that using non-const operators can cause QContiguousCache to do a deep
    copy.

    \sa insert(), at()
*/

/*! \fn const T &QContiguousCache::operator[](int i) const

    \overload

    Same as at(\a i).
*/

/*! \fn void QContiguousCache::append(const T &value)

    Inserts \a value at the end of the cache.  If the cache is already full
    the item at the start of the cache will be removed.

    \sa prepend(), insert(), isFull()
*/

/*! \fn void QContiguousCache::prepend(const T &value)

    Inserts \a value at the start of the cache.  If the cache is already full
    the item at the end of the cache will be removed.

    \sa append(), insert(), isFull()
*/

/*! \fn void QContiguousCache::insert(int i, const T &value)

    Inserts the \a value at the index position \a i.  If the cache already contains
    an item at \a i then that value is replaced.  If \a i is either one more than
    lastIndex() or one less than firstIndex() it is the equivalent to an append()
    or a prepend().

    If the given index \a i is not within the current range of the cache nor adjacent
    to the bounds of the cache's index range the cache is first cleared before
    inserting the item.  At this point the cache will have a size of 1.  It is worth
    while then taking effort to insert items in an order that starts adjacent to the
    current index range for the cache.

    \sa prepend(), append(), isFull(), firstIndex(), lastIndex()
*/

/*! \fn bool QContiguousCache::containsIndex(int i) const

    Returns true if the cache's index range includes the given index \a i.

    \sa firstIndex(), lastIndex()
*/

/*! \fn int QContiguousCache::firstIndex() const
    Returns the first valid index in the cache.  The index will be invalid if the
    cache is empty.  However the following code is valid even when the cache is empty:

    \code
    for (int i = cache.firstIndex(); i <= cache.lastIndex(); ++i)
        qDebug() << "Item" << i << "of the cache is" << cache.at(i);
    \endcode

    \sa capacity(), size(), lastIndex()
*/

/*! \fn int QContiguousCache::lastIndex() const

    Returns the last valid index in the cache.  If the cache is empty will return -1.

    \code
    for (int i = cache.firstIndex(); i <= cache.lastIndex(); ++i)
        qDebug() << "Item" << i << "of the cache is" << cache.at(i);
    \endcode

    \sa capacity(), size(), firstIndex()
*/


/*! \fn T &QContiguousCache::first()

    Returns a reference to the first item in the cache.  This function
    assumes that the cache isn't empty.

    \sa last(), isEmpty()
*/

/*! \fn T &QContiguousCache::last()

    Returns a reference to the last item in the cache.  This function
    assumes that the cache isn't empty.

    \sa first(), isEmpty()
*/

/*! \fn const T& QContiguousCache::first() const

    \overload
*/

/*! \fn const T& QContiguousCache::last() const

    \overload
*/

/*! \fn void QContiguousCache::removeFirst()
    
    Removes the first item from the cache.  This function assumes that
    the cache isn't empty.

    \sa removeLast()
*/

/*! \fn void QContiguousCache::removeLast()
    
    Removes the last item from the cache.  This function assumes that
    the cache isn't empty.

    \sa removeFirst()
*/

/*! \fn T QContiguousCache::takeFirst()
    
    Removes the first item in the cache and returns it.

    If you don't sue the return value, removeFirst() is more efficient.

    \sa takeLast(), removeFirst()
*/

/*! \fn T QContiguousCache::takeLast()
    
    Removes the last item in the cache and returns it.

    If you don't sue the return value, removeLast() is more efficient.

    \sa takeFirst(), removeLast()
*/

/*! \fn void QContiguousCache::dump() const

    \internal

    Sends information about the cache's internal structure to qDebug()
*/

QT_END_NAMESPACE
