/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>


#include "q3valuelist.h"




#include <q3valuelist.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ValueList : public QObject
{
Q_OBJECT

public:
    tst_Q3ValueList();
    virtual ~tst_Q3ValueList();


public slots:
    void init();
    void cleanup();
private slots:

    void isEmpty();
    void clear();
    void count();
    void size();
    void contains();
    void findIndex();
    void indexing();
    void firstLast();
    void frontBack();
    void beginEnd();
    void pushing();
    void popping();
    void remove();
    void erase();
    void fromLast();
    void append();
    // Doesn't have own test function since all
    // other functions are heavy users of it,
    // thus it must work correctly
    void prepend();
    void insert();
    void find();
    void opEqualNotEqual();
    void opPlus();
    void opPlusEqual();
    void opStreamOut();
    void shared();
    void detach_on_append();
    void detach_on_prepend();
    void detach_on_insert1();
    void detach_on_insert2();
    void detach_on_it_assign();
    void detach_on_ref_assign();
    void detach_on_clear();
    void detach_on_erase1();
    void detach_on_erase2();
    void detach_on_opPE1();
    void detach_on_opPE2();
    void detach_on_opStream();
    void detach_on_pop_front();
    void detach_on_pop_back();
    void detach_on_push_front();
    void detach_on_push_back();
    void detach_on_remove1();
    void detach_on_remove2();
};

tst_Q3ValueList::tst_Q3ValueList()

{
}

tst_Q3ValueList::~tst_Q3ValueList()
{

}

void tst_Q3ValueList::init()
{
}

void tst_Q3ValueList::cleanup()
{
}

void tst_Q3ValueList::isEmpty()
{
    Q3ValueList<int> a;
    QVERIFY( a.isEmpty() );
    QVERIFY( a.empty() );

    a.append( 1 );
    QVERIFY( !a.isEmpty() );
    QVERIFY( !a.empty() );
}

void tst_Q3ValueList::clear()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 2 );
    a.append( 3 );
    a.clear();
    QVERIFY( a.isEmpty() );
}

void tst_Q3ValueList::count()
{
    Q3ValueList<int> a;
    QCOMPARE( (int)a.count(), 0 );

    a.append( 1 );
    QCOMPARE( (int)a.count(), 1 );

    a.append( 2 );
    QCOMPARE( (int)a.count(), 2 );

    a.append( 3 );
    QCOMPARE( (int)a.count(), 3 );

    a.clear();
    QCOMPARE( (int)a.count(), 0 );
}

void tst_Q3ValueList::size()
{
    Q3ValueList<int> a;
    QCOMPARE( (int)a.size(), 0 );

    a.append( 1 );
    QCOMPARE( (int)a.size(), 1 );

    a.append( 2 );
    QCOMPARE( (int)a.size(), 2 );

    a.append( 3 );
    QCOMPARE( (int)a.size(), 3 );

    a.clear();
    QCOMPARE( (int)a.size(), 0 );
}

void tst_Q3ValueList::contains()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 1000 );

    QCOMPARE( (int)a.contains(1), 1 );
    QCOMPARE( (int)a.contains(10), 1 );
    QCOMPARE( (int)a.contains(99), 0 );
    QCOMPARE( (int)a.contains(1000), 2 );
}

void tst_Q3ValueList::findIndex()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 1000 );

    QCOMPARE( a.findIndex( 0 ), -1 );
    QCOMPARE( a.findIndex( 1 ), 0 );
    QCOMPARE( a.findIndex( 1000 ), 3 );
}

void tst_Q3ValueList::indexing()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 99 );

    QCOMPARE( a[0], 1 );
    QCOMPARE( a[1], 10 );
    QCOMPARE( a[2], 99 );
    QCOMPARE( *(a.at(0)), 1 );
    QCOMPARE( *(a.at(1)), 10 );
    QCOMPARE( *(a.at(2)), 99 );

    a[1] = 11;
    QCOMPARE( a[1], 11 );

    *(a.at(0)) = 2;
    QCOMPARE( a[0], 2 );
}

void tst_Q3ValueList::firstLast()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 10000 );

    QCOMPARE( a.first(), 1 );
    QCOMPARE( a.last(), 10000 );

    a.first() = 2;
    a.last() = 20000;
    QCOMPARE( a.first(), 2 );
    QCOMPARE( a.last(), 20000 );
}

void tst_Q3ValueList::frontBack()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 10000 );

    QCOMPARE( a.front(), 1 );
    QCOMPARE( a.back(), 10000 );

    a.first() = 2;
    a.last() = 20000;
    QCOMPARE( a.front(), 2 );
    QCOMPARE( a.back(), 20000 );
}

void tst_Q3ValueList::beginEnd()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    Q3ValueListConstIterator<int> cit1 = a.begin();
    Q3ValueListConstIterator<int> cit2 = a.end();
    QCOMPARE( *(cit1), 1 );
    QCOMPARE( *(--cit2), 100 );

    Q3ValueListIterator<int> it1 = a.begin();
    Q3ValueListIterator<int> it2 = a.end();
    *(it1) = 2;
    *(--it2) = 200;

    // Using const iterators to verify
    QCOMPARE( *(cit1), 2 );
    QCOMPARE( *(cit2), 200 );

    Q3ValueList<int> b;
    b.append( 1 );
    Q3ValueList<int> b2 = b;
    QVERIFY( b.constBegin() == b2.constBegin() );
    QVERIFY( b.constEnd() == b2.constEnd() );
    b2.append( 2 );
    QVERIFY( b.constBegin() != b2.constBegin() );
    QVERIFY( b2.constBegin() == b2.constBegin() );
}

void tst_Q3ValueList::pushing()
{
    Q3ValueList<int> a;
    a.append( 100 );

    a.push_front( 10 );
    QCOMPARE( a.first(), 10 );
    QCOMPARE( a.last(), 100 );

    a.push_back( 1000 );
    QCOMPARE( a.first(), 10 );
    QCOMPARE( a.last(), 1000 );
}

void tst_Q3ValueList::popping()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 10000 );

    a.pop_front();
    QCOMPARE( a.first(), 10 );
    QCOMPARE( a.last(), 10000 );

    a.pop_back();
    QCOMPARE( a.first(), 10 );
    QCOMPARE( a.last(), 1000 );

    QCOMPARE( (int)a.count(), 3 );

    a.pop_back();
    a.pop_back();
    a.pop_back();
    QVERIFY( a.isEmpty() );
}

void tst_Q3ValueList::remove()
{
    {
        Q3ValueList<int> a;
        a.append( 1 );
        a.append( 10 );
        a.append( 100 );
        a.append( 1000 );
        a.append( 1000 );
        a.append( 10000 );

        QCOMPARE( (uint)a.remove(100), (uint)1 );
        QCOMPARE( (uint)a.remove(1000), (uint)2 );
        QCOMPARE( (int)a.first(), 1 );
        QCOMPARE( (int)a.last(), 10000 );

        a.remove( a.at(0) );
        QCOMPARE( (int)a.first(), 10 );
        QCOMPARE( (int)a.last(), 10000 );

        a.remove( a.at(1) );
        QCOMPARE( (int)a.first(), 10 );
        QCOMPARE( (int)a.last(), 10 );
    }
    {
        Q3ValueList<int> a;
        a.append( 1 );
        a.append( 10 );
        a.append( 100 );
        a.append( 1000 );
        a.append( 10000 );

        Q3ValueList<int>::Iterator it = a.begin();
        ++it;
        QVERIFY(*it == 10);
        it = a.remove(it);
        QVERIFY(*it == 100);
        it = a.remove(it);
        QVERIFY(*it == 1000);
        it = a.remove(it);
        QVERIFY(*it == 10000);
        it = a.remove(it);
        QVERIFY(it == a.end());
    }
}

void tst_Q3ValueList::erase()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 5 );
    a.append( 10 );
    a.append( 50 );
    a.append( 100 );
    a.append( 500 );
    a.append( 1000 );
    a.append( 5000 );
    a.append( 10000 );
    a.append( 50000 );

    a.erase( a.at(0), a.at(5) ); // Remove 1 to 100 (inclusive)
    QCOMPARE( (int)a.first(), 500 );
    QCOMPARE( (int)a.last(), 50000 );

    Q3ValueListIterator<int> it = a.erase( a.at(2) ); // remove 5000
    QCOMPARE( *(it), 10000 );

    it = a.erase( a.at(3) ); // remove 50000
    QVERIFY( (it == a.end()) );
}

void tst_Q3ValueList::fromLast()
{
    Q3ValueList<int> a;
    Q3ValueListIterator<int> it = a.fromLast();
    QVERIFY( (it == a.end()) );

    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    it = a.fromLast();
    QVERIFY( (it != a.end()) );

    QCOMPARE( a.last(), 100 );
    *(a.fromLast()) = 200;
    QCOMPARE( a.last(), 200 );
}

void tst_Q3ValueList::prepend()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    QCOMPARE( (int)a[0], 1 );
    Q3ValueList<int>::Iterator it = a.prepend( 1000 );
    QCOMPARE( (int)a[0], 1000 );
    QVERIFY( *it == 1000 );
}

void tst_Q3ValueList::insert()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    Q3ValueListIterator<int> it = a.fromLast();
    it = a.insert( it, 1000 );

    QCOMPARE( *(it), 1000 );
    QCOMPARE( *(++it), 100 );
    QCOMPARE( (int)a.count(), 4 );

    it = a.fromLast();
    a.insert( it, 10, 1234 );
    QCOMPARE( (int)a.count(), 14 );
}

void tst_Q3ValueList::find()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );
    a.append( 1000 );
    a.append( 10000 );
    a.append( 10000 );
    a.append( 20000 );
    a.append( 30000 );

    // Constant iterators
    Q3ValueListConstIterator<int> cit1 = a.find( 200 );
    QVERIFY( (cit1 == a.end()) );

    cit1 = a.find( 1000 );
    QCOMPARE( *(cit1), 1000 );
    QCOMPARE( *(++cit1), 10000 );
    QCOMPARE( *(++cit1), 10000 );
    QCOMPARE( *(++cit1), 20000 );

    cit1 = a.at( 3 );
    Q3ValueListConstIterator<int> cit2 = a.find( cit1, 20000 );
    QCOMPARE( *(cit2), 20000 );
    QCOMPARE( *(++cit2), 30000 );

    // Non constant iterators
    Q3ValueListIterator<int> it1 = a.find( 200 );
    QVERIFY( (it1 == a.end()) );

    it1 = a.find( 1000 );
    QCOMPARE( *(it1), 1000 );
    QCOMPARE( *(++it1), 10000 );
    QCOMPARE( *(++it1), 10000 );
    QCOMPARE( *(++it1), 20000 );
    *(it1) = 25000;
    it1--;
    it1++;
    QCOMPARE( *(it1), 25000 );

    Q3ValueListIterator<int> it2 = a.find( it1, 30000 );
    *(it2) = 35000;
    QCOMPARE( *(it2), 35000 );
}

void tst_Q3ValueList::opEqualNotEqual()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    Q3ValueList<int> b;
    b.append( 1 );
    b.append( 10 );
    b.append( 100 );

    QVERIFY( a == b );
    QVERIFY( !(a != b) );

    a.append( 1000 );
    QVERIFY( a != b );
    QVERIFY( !(a == b) );
}

void tst_Q3ValueList::opPlus()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    Q3ValueList<int> b;
    b.append( 2 );
    b.append( 20 );
    b.append( 200 );

    Q3ValueList<int> c = a + b;

    QCOMPARE( c[0], 1 );
    QCOMPARE( c[1], 10 );
    QCOMPARE( c[2], 100 );
    QCOMPARE( c[3], 2 );
    QCOMPARE( c[4], 20 );
    QCOMPARE( c[5], 200 );
}

void tst_Q3ValueList::opPlusEqual()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    Q3ValueList<int> b;
    b.append( 2 );
    b.append( 20 );
    b.append( 200 );

    a += b;
    QCOMPARE( a[0], 1 );
    QCOMPARE( a[1], 10 );
    QCOMPARE( a[2], 100 );
    QCOMPARE( a[3], 2 );
    QCOMPARE( a[4], 20 );
    QCOMPARE( a[5], 200 );

    a += 1000;
    QCOMPARE( a[6], 1000 );
}

void tst_Q3ValueList::opStreamOut()
{
    Q3ValueList<int> a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    a << 1000 << 10000;
    QCOMPARE( a.last(), 10000 );
}

class ListVerifier : public Q3ValueList<int>
{
public:
    const int* pointer() const { return &*begin(); }
};
void tst_Q3ValueList::shared()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    //Checking for identical d-pointers
    QVERIFY( (a == b) );
    QVERIFY( (a.pointer() == b.pointer()) );
}
void tst_Q3ValueList::detach_on_append()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // append detach?
    b.append( 10 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_prepend()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // prepend detach?
    b.prepend( 10000 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_insert1()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // insert detach?
    Q3ValueListIterator<int> it = b.at(1);
    b.insert( it, 20 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_insert2()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;
    // insert detach?
    Q3ValueListIterator<int> it = b.at(1);
    b.insert( it, 2, 20 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_it_assign()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // iterator assignment detach?
    Q3ValueListIterator<int> it = b.at(0);
    *(it) = 2;
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_ref_assign()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // reference assignment detach?
    int &i1 = b.back();
    i1 = 2;
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_clear()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // clear detach?
    b.clear();
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_erase1()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // erase detach?
    Q3ValueListIterator<int> it = b.at(1);
    b.erase( it );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_erase2()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // erase detach?
    Q3ValueListIterator<int> it1 = b.at(0);
    Q3ValueListIterator<int> it2 = b.at(1);
    b.erase( it1, it2 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_opPE1()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // operator+= detach?
    b += 1000;
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_opPE2()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // operator+= detach?
    b += a;
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_opStream()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // operator<< detach?
    b << 1000;
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_pop_front()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // pop_front detach?
    b.pop_front();
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_pop_back()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // pop_back detach?
    b.pop_back();
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_push_front()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // push_front detach?
    b.push_front( 2 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_push_back()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // pop_back detach?
    b.push_back( 2 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_remove1()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // remove detach?
    b.remove( 10 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::detach_on_remove2()
{
    ListVerifier a;
    a.append( 1 );
    a.append( 10 );
    a.append( 100 );

    ListVerifier b = a;

    // remove detach?
    Q3ValueListIterator<int> it6 = b.at(1);
    b.remove( it6 );
    QVERIFY( !(a == b) );
    QVERIFY( !(a.pointer() == b.pointer()) );
}

void tst_Q3ValueList::append()
{
    Q3ValueList<int> list;
    Q3ValueList<int>::Iterator it = list.append(1);
    QVERIFY(*it == 1);
    it = list.append(2);
    QVERIFY(*it == 2);
}


QTEST_APPLESS_MAIN(tst_Q3ValueList)
#include "tst_q3valuelist.moc"
