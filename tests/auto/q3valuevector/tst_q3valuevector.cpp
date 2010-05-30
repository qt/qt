/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>


#include "q3valuevector.h"

#include <qiodevice.h>
#include <q3valuevector.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_Q3ValueVector : public QObject
{
Q_OBJECT

public:
    tst_Q3ValueVector();
    virtual ~tst_Q3ValueVector();


public slots:
    void init();
    void cleanup();
private slots:
    void acc_01();

    void operator_equalEqual();
    void operator_assign();
    // a test for assignment from a std::list has been omitted
    void size();
    void empty();
    void capacity_data();
    void capacity();
    // ommiting test for const iterators (for begin() and end())
    void begin();
    void end();
    // omitting test for const reference
    void at();
    // omitting test for const reference
    void operator_index();
    void front();
    void back();
    void push_back();
    void pop_back();
    void insert();
    void reserve_data();
    void reserve();
    void resize();
    void clear();
};


static QByteArray toBA( const Q3ValueVector<int> &vv )
{
    QByteArray ba;
    QDataStream ds( &ba, IO_ReadWrite );
    (void)vv;
    ds << vv;
    return ba;
}

tst_Q3ValueVector::tst_Q3ValueVector()
{
}

tst_Q3ValueVector::~tst_Q3ValueVector()
{

}

void tst_Q3ValueVector::init()
{
// TODO: Add initialization code here.
// This will be executed immediately before each test is run.
}

void tst_Q3ValueVector::cleanup()
{
// TODO: Add cleanup code here.
// This will be executed immediately after each test is run.
}

void tst_Q3ValueVector::reserve_data()
{
    capacity_data();
}

void tst_Q3ValueVector::capacity_data()
{
    QTest::addColumn<QByteArray>("ba");

    Q3ValueVector<int> a;
    QTest::newRow( "data0" ) << toBA( a );

    Q3ValueVector<int> b;
    b.push_back( 1 );
    b.push_back( 2 );
    QTest::newRow( "data1" ) << toBA( b );

    Q3ValueVector<int> c;

    // let's try one that's rather large

    for(int i = 0; i < 10000; i++)
	c.push_back( i );

    QTest::newRow( "data2" ) << toBA( c );
}

void tst_Q3ValueVector::acc_01()
{
    // vectors
    Q3ValueVector<int> v1;
    QVERIFY( v1.empty() );
    QVERIFY( v1.size() == 0 );
    QVERIFY( v1.capacity() >= v1.size() );

    Q3ValueVector<int> v2( v1 );
    QVERIFY( v2.empty() );
    QVERIFY( v2.size() == 0 );
    QVERIFY( v2.capacity() >= v1.size() );

    Q3ValueVector<int> v5( 5 );
    QVERIFY( !v5.empty() );
    QVERIFY( v5.size() == 5 );
    QVERIFY( v5.capacity() >= 5 );

    //operator=
    Q3ValueVector<int> v4 = v2;
    QVERIFY( v4.empty() );
    QVERIFY( v4.size() == 0 );
    QVERIFY( (int)v4.capacity() >= 0 );

    // adding elements
    v4.push_back( 1 );
    v4.push_back( 2 );
    v4.push_back( 3 );
    QVERIFY( !v4.empty() );
    QVERIFY( v2.empty() ); // should have detached
    QVERIFY( v4.size() == 3 );
    QVERIFY( v4.capacity() >= v4.size() );
    v4.insert( v4.end(), 4 );
    v4.insert( v4.begin(), 0 );
    QVERIFY( !v4.empty() );
    QVERIFY( v4.size() == 5 );
    QVERIFY( v4.capacity() >= v4.size() );

    QVERIFY( v4[0] == 0);
    QVERIFY( v4[1] == 1);
    QVERIFY( v4[2] == 2);
    QVERIFY( v4[3] == 3);
    QVERIFY( v4[4] == 4);

    // swap
    Q3ValueVector<int> tmp = v2;
    v2 = v4;
    v4 = tmp;
//    v4.swap( v2 );
    QVERIFY( v4.empty() );
    QVERIFY( !v2.empty() );
    QVERIFY( v2.size() == 5 );
    QVERIFY( v2.capacity() >= v2.size() );
    QVERIFY( v2[0] == 0);
    QVERIFY( v2[1] == 1);
    QVERIFY( v2[2] == 2);
    QVERIFY( v2[3] == 3);
    QVERIFY( v2[4] == 4);
    // v2 Should contain 5 elements: 0,1,2,3,4

    QVERIFY(v4.size() == 0); //Should contain no elements

    // element access
    Q3ValueVector<int> v3( 5 );
    v3[0] = 0;
    v3[1] = 1;
    v3[2] = 2;
    v3[3] = 3;
    v3[4] = 4;
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );
    bool ok = FALSE;
    (void) v3.at( 1000, &ok );
    QVERIFY( !ok );
    int& j = v3.at( 2, &ok );
    QVERIFY( ok );
    QVERIFY( j == 2 );

    // iterators
    Q3ValueVector<int>::iterator it = v3.begin();
    int k = 0;
    for ( ; k < 5; ++k, ++it )
	QVERIFY( *it == k );
    QVERIFY( it == v3.end() );
    --it;
    for ( k = 4; k >= 0; --k, --it )
	QVERIFY( *it == k );

    QVERIFY( v3.front() == 0 );
    QVERIFY( v3.back() == 4 );

    // capacity stuff
    v3.resize( 5 );
	// Should contain 5 elements: 0,1,2,3,4
    QVERIFY( v3.size() == 5 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );

    v3.resize( 6 );
    // Should now contain 6 elements: 0,1,2,3,4,0
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );
    QVERIFY( v3[5] == 0 );

    v3.reserve( 1000 );
    QVERIFY( v3.size() == 6 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );
    QVERIFY( v3.capacity() >= 1000 );

    v3.back() = 5;
    // Should contain 5 elements: 0,1,2,3,4
    QVERIFY( v3.back() == 5 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );

    v3.resize( 5 );
    // Should contain 5 elements: 0,1,2,3,4
    QVERIFY( v3.size() == 5 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3[4] == 4 );
    QVERIFY( v3.capacity() >= 1000 );

    it = v3.end();
    v3.erase( --it );
    // Should contain 4 elements: 0,1,2,3
    QVERIFY( v3.size() == 4 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 1 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 3 );
    QVERIFY( v3.capacity() >= 1000 );

    it = v3.begin();
    Q3ValueVector<int>::iterator it2 = v3.end();
    v3.erase( ++it, --it2 );
	// Should contain 2 elements: 0,3
    QVERIFY( v3.size() == 2 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 3 );
    QVERIFY( v3.capacity() >= 1000 );

    it = v3.begin();
    v3.insert( ++it, 9 );
    // Should contain 3 elements: 0,9,3
    QVERIFY( v3.size() == 3 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 9 );
    QVERIFY( v3[2] == 3 );
    QVERIFY( v3.capacity() >= 1000 );

    it = v3.begin();
    v3.insert( ++it, 4, 4 );
    // Should contain 7 elements: 0,4,4,4,4,9,3
    QVERIFY( v3.size() == 7 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 4 );
    QVERIFY( v3[2] == 4 );
    QVERIFY( v3[3] == 4 );
    QVERIFY( v3[4] == 4 );
    QVERIFY( v3[5] == 9 );
    QVERIFY( v3[6] == 3 );
    QVERIFY( v3.capacity() >= 1000 );

    it = v3.begin();
    v3.insert( ++it, 2000, 2 );
    // Should contain 2007 elements: 0,2,2,...2,4,4,4,4,9,3
    QVERIFY( v3.size() == 2007 );

    it = qFind( v3.begin(), v3.end(), 3 );
    it2 = v3.end();
    QVERIFY( it == --it2 );

    v3.resize( 4 );
    // Should contain 4 elements: 0,2,2,2
    QVERIFY( v3.size() == 4 );
    QVERIFY( v3[0] == 0 );
    QVERIFY( v3[1] == 2 );
    QVERIFY( v3[2] == 2 );
    QVERIFY( v3[3] == 2 );
    QVERIFY( v3.capacity() >= 2007 );

    it = v3.begin();
    v3.insert( ++it, 2000, 2 );
    v3.push_back( 9 );
    v3.push_back( 3 );
    it = v3.begin();
    it2 = v3.end();
    v3.erase( ++it, ----it2 );
    // Should contain 3 elements: 0,9,3
    QVERIFY( v3.size() == 3 );

    v3.pop_back();
    // Should contain 2 elements: 0,9
    QVERIFY( v3.size() == 2 );

    /*
    // instantiate other member functions
    Q3ValueVector<int>::const_iterator cit = v3.begin();
    cit = v3.end();
    Q3ValueVector<int>::size_type max_size = v3.max_size();
    std::cout << "max size of vector:" << max_size << std::endl;
    const int& ci = v3.at( 1 );
    const int& ci2 = v3[1];
    const int& ci3 = v3.front();
    const int& ci4 = v3.back();
    v3.clear();

    QStringList l1, l2;
    l1 << "Weis" << "Ettrich" << "Arnt" << "Sue";
    l2 << "Torben" << "Matthias";
    qCopy( l2.begin(), l2.end(), l1.begin() );

    Q3ValueVector<QString> v( l1.size(), "Dave" );
    qCopy( l2.begin(), l2.end(), v.begin() );
    std::for_each( v.begin(), v.end(), qDebug );

    std::vector<int> stdvec( 5, 100 );
    Q3ValueVector<int> cvec( stdvec );
    std::cout << "Should contain 5 elements: 100,100,100,100,100" << std::endl;
    print( cvec );
    Q3ValueVector<int> cvec2 = stdvec;
    std::cout << "Should contain 5 elements: 100,100,100,100,100" << std::endl;
    print( cvec2 );

    QFile f( "file.dta" );
    f.open( QIODevice::WriteOnly );
    QDataStream s( &f );
    s << cvec2;
    f.close();

    f.open( QIODevice::ReadOnly );
    Q3ValueVector<int> in;
    s >> in;
    std::cout << "Should contain 5 elements: 100,100,100,100,100" << std::endl;
    print( in );
*/
}

void tst_Q3ValueVector::operator_equalEqual()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 10 );
    a.push_back( 100 );

    Q3ValueVector<int> b;
    b.push_back( 1 );
    b.push_back( 10 );
    b.push_back( 100 );

    QVERIFY( a == b );
    QVERIFY( b == a );

    a.push_back( 1000 );
    QVERIFY( !(a == b) );
    QVERIFY( !(b == a) );
}

void tst_Q3ValueVector::operator_assign()
{
    // test assignment using an int

    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );
    Q3ValueVector<int> b = a;
    QVERIFY( (a == b) );
}

void tst_Q3ValueVector::size()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );
    QCOMPARE( (int)a.size(), 2 );
}

void tst_Q3ValueVector::empty()
{
    Q3ValueVector<int> a;
    QVERIFY( a.empty() );
}

void tst_Q3ValueVector::capacity()
{
    QFETCH( QByteArray, ba );
    Q3ValueVector<int> vector;

    QDataStream ds( &ba, IO_ReadWrite );
    ds >> vector;

    QVERIFY( vector.capacity() >= vector.size() );
}

void tst_Q3ValueVector::begin()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    Q3ValueVector<int>::iterator it_a = a.begin();
    QCOMPARE( (*it_a), 1 );

    // now try it for an empty vector; per the documentation begin() should equal end()

    Q3ValueVector<int> b;
    Q3ValueVector<int>::iterator it_b = b.begin();
    QVERIFY( it_b == b.end() );
}

void tst_Q3ValueVector::end()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    Q3ValueVector<int>::iterator it_a = a.end();
    QCOMPARE( (*(--it_a)), 2 );

    Q3ValueVector<int> b;
    Q3ValueVector<int>::iterator it_b = b.end();
    QVERIFY( it_b == b.begin() );
}

void tst_Q3ValueVector::at()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    bool ok;
    QVERIFY( a.at( 0, &ok ) == 1 && ok == true );
    QVERIFY( a.at( 1, &ok ) == 2 && ok == true );
    (void)a.at( 3, &ok );
    QCOMPARE( ok, false );
}

void tst_Q3ValueVector::operator_index()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    QCOMPARE( a[0], 1 );
    QCOMPARE( a[1], 2 );
}

void tst_Q3ValueVector::front()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    QCOMPARE(a.front(), 1 );
}

void tst_Q3ValueVector::back()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    QCOMPARE(a.back(), 2 );
}

void tst_Q3ValueVector::push_back()
{
    Q3ValueVector<int> a;

    a.push_back( 1 );
    QVERIFY( a.back() == 1 && a.size() == 1 );

    a.push_back( 2 );
    QVERIFY( a.back() == 2 && a.size() == 2 );
}

void tst_Q3ValueVector::pop_back()
{
    Q3ValueVector<int> a;
    a.push_back( 1 );
    a.push_back( 2 );

    a.pop_back();
    QVERIFY( a.back() == 1 && a.size() == 1 );

    a.pop_back();
    QVERIFY( a.empty() );
}

void tst_Q3ValueVector::insert()
{
    // insert at the beginning
    Q3ValueVector<int> a;
    a.insert( a.begin(), 1 );
    QCOMPARE( a[0], 1 );

    // insert at the end
    a.insert( a.end(), 2 );
    QCOMPARE( a[1], 2 );
    // insert in the middle
    Q3ValueVector<int>::iterator it_a = a.begin();
    a.insert( ++it_a, 3 );
    QCOMPARE( a[1], 3 );

    // now testing the overloaded insert() which takes an
    // argument for the number of items to insert

    // we'll insert two of each value

    Q3ValueVector<int> b;
    b.insert( b.begin(), 2, 1 );
    QCOMPARE( b[0], 1 );
    QCOMPARE( b[1], 1 );

    // insert at the end
    b.insert( b.end(), 2, 2 );
    QCOMPARE( b[2], 2 );
    QCOMPARE( b[3], 2 );

    // insert in the middle
    Q3ValueVector<int>::iterator it_b = b.begin();
    b.insert( ++++it_b, 2, 3 );
    QCOMPARE( b[2], 3 );
    QCOMPARE( b[3], 3 );
}

void tst_Q3ValueVector::reserve()
{
    QFETCH( QByteArray, ba );
    Q3ValueVector<int> vector;

    QDataStream ds( &ba, IO_ReadWrite );
    ds >> vector;

    Q3ValueVector<int>::size_type cap = vector.capacity();

    // should do nothing
    if( cap > 5 )
	vector.reserve( vector.capacity() - 5 );
    else
	vector.reserve( 0 );

    QVERIFY( vector.capacity() == cap );

    // should make capacity() grow
    vector.reserve( vector.capacity() + 5 );
    QVERIFY( cap < vector.capacity() );
}

void tst_Q3ValueVector::resize()
{
    Q3ValueVector<int> a;
    a.resize( 2 );

    QVERIFY( a.size() == 2 );

    Q3ValueVector<int> b;
    b.resize( 2, 42 );

    QVERIFY( b.size() == 2 );
    QCOMPARE( b[0], 42 );
    QCOMPARE( b[1], 42 );

    b.resize( 1 );
    QVERIFY( b.size() == 1 );

    b.resize( 4, 21 );
    QCOMPARE( b[0], 42 );
    QCOMPARE( b[1], 21 );
    QCOMPARE( b[2], 21 );
    QCOMPARE( b[3], 21 );

    b.resize( 0 );
    QVERIFY( b.empty() );

}

void tst_Q3ValueVector::clear()
{
    Q3ValueVector<int> a;
    a.clear();
    QVERIFY( a.empty() );

    a.push_back( 1 );
    a.push_back( 2 );

    a.clear();
    QVERIFY( a.empty() );
}

QTEST_APPLESS_MAIN(tst_Q3ValueVector)
#include "tst_q3valuevector.moc"
