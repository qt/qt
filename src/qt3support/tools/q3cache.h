/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q3CACHE_H
#define Q3CACHE_H

#include <Qt3Support/q3gcache.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template<class type>
class Q3Cache
#ifdef qdoc
        : public Q3PtrCollection
#else
        : public Q3GCache
#endif
{
public:
    Q3Cache(const Q3Cache<type> &c) : Q3GCache(c) {}
    Q3Cache(int maxCost=100, int size=17, bool caseSensitive=true)
        : Q3GCache(maxCost, size, StringKey, caseSensitive, false) {}
   ~Q3Cache()                           { clear(); }
    Q3Cache<type> &operator=(const Q3Cache<type> &c)
                        { return (Q3Cache<type>&)Q3GCache::operator=(c); }
    int   maxCost()   const             { return Q3GCache::maxCost(); }
    int   totalCost() const             { return Q3GCache::totalCost(); }
    void  setMaxCost(int m)           { Q3GCache::setMaxCost(m); }
    uint  count()     const             { return Q3GCache::count(); }
    uint  size()      const             { return Q3GCache::size(); }
    bool  isEmpty()   const             { return Q3GCache::count() == 0; }
    void  clear()                       { Q3GCache::clear(); }
    bool  insert(const QString &k, const type *d, int c=1, int p=0)
                        { return Q3GCache::insert_string(k,(Item)d,c,p);}
    bool  remove(const QString &k)
                        { return Q3GCache::remove_string(k); }
    type *take(const QString &k)
                        { return (type *)Q3GCache::take_string(k); }
    type *find(const QString &k, bool ref=true) const
                        { return (type *)Q3GCache::find_string(k,ref);}
    type *operator[](const QString &k) const
                        { return (type *)Q3GCache::find_string(k);}
    void  statistics() const          { Q3GCache::statistics(); }
private:
    void  deleteItem(Item d);
};

#if !defined(Q_BROKEN_TEMPLATE_SPECIALIZATION)
template<> inline void Q3Cache<void>::deleteItem(Q3PtrCollection::Item)
{
}
#endif

template<class type> inline void Q3Cache<type>::deleteItem(Q3PtrCollection::Item d)
{
    if (del_item) delete (type *)d;
}

template<class type>
class Q3CacheIterator : public Q3GCacheIterator
{
public:
    Q3CacheIterator(const Q3Cache<type> &c):Q3GCacheIterator((Q3GCache &)c) {}
    Q3CacheIterator(const Q3CacheIterator<type> &ci)
                                : Q3GCacheIterator((Q3GCacheIterator &)ci) {}
    Q3CacheIterator<type> &operator=(const Q3CacheIterator<type>&ci)
        { return (Q3CacheIterator<type>&)Q3GCacheIterator::operator=(ci); }
    uint  count()   const     { return Q3GCacheIterator::count(); }
    bool  isEmpty() const     { return Q3GCacheIterator::count() == 0; }
    bool  atFirst() const     { return Q3GCacheIterator::atFirst(); }
    bool  atLast()  const     { return Q3GCacheIterator::atLast(); }
    type *toFirst()           { return (type *)Q3GCacheIterator::toFirst(); }
    type *toLast()            { return (type *)Q3GCacheIterator::toLast(); }
    operator type *() const   { return (type *)Q3GCacheIterator::get(); }
    type *current()   const   { return (type *)Q3GCacheIterator::get(); }
    QString currentKey() const{ return Q3GCacheIterator::getKeyString(); }
    type *operator()()        { return (type *)Q3GCacheIterator::operator()();}
    type *operator++()        { return (type *)Q3GCacheIterator::operator++(); }
    type *operator+=(uint j)  { return (type *)Q3GCacheIterator::operator+=(j);}
    type *operator--()        { return (type *)Q3GCacheIterator::operator--(); }
    type *operator-=(uint j)  { return (type *)Q3GCacheIterator::operator-=(j);}
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3CACHE_H
