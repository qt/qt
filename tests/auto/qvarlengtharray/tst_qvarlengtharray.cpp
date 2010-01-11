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
#include <qvarlengtharray.h>

const int N = 1;

//TESTED_CLASS=
//TESTED_FILES=

class tst_QVarLengthArray : public QObject
{
    Q_OBJECT

public:
    tst_QVarLengthArray() {}
    virtual ~tst_QVarLengthArray() {}

private slots:
    void append();
    void removeLast();
    void oldTests();
    void task214223();
};

int fooCtor = 0;
int fooDtor = 0;

struct Foo
{
    int *p;

    Foo() { p = new int; ++fooCtor; }
    Foo(const Foo &other) { p = new int; ++fooCtor; }

    void operator=(const Foo & /* other */) { }

    ~Foo() { delete p; ++fooDtor; }
};

void tst_QVarLengthArray::append()
{
    QVarLengthArray<QString> v;
    v.append(QString("hello"));

    QVarLengthArray<int> v2; // rocket!
    v2.append(5);
}

void tst_QVarLengthArray::removeLast()
{
    {
        QVarLengthArray<char, 2> v;
        v.append(0);
        v.append(1);
        QCOMPARE(v.size(), 2);
        v.append(2);
        v.append(3);
        QCOMPARE(v.size(), 4);
        v.removeLast();
        QCOMPARE(v.size(), 3);
        v.removeLast();
        QCOMPARE(v.size(), 2);
    }

    {
        QVarLengthArray<QString, 2> v;
        v.append("0");
        v.append("1");
        QCOMPARE(v.size(), 2);
        v.append("2");
        v.append("3");
        QCOMPARE(v.size(), 4);
        v.removeLast();
        QCOMPARE(v.size(), 3);
        v.removeLast();
        QCOMPARE(v.size(), 2);
    }
}

void tst_QVarLengthArray::oldTests()
{
    {
	QVarLengthArray<int, 256> sa(128);
	QVERIFY(sa.data() == &sa[0]);
	sa[0] = 0xfee;
	sa[10] = 0xff;
	QVERIFY(sa[0] == 0xfee);
	QVERIFY(sa[10] == 0xff);
	sa.resize(512);
	QVERIFY(sa.data() == &sa[0]);
	QVERIFY(sa[0] == 0xfee);
	QVERIFY(sa[10] == 0xff);
	QVERIFY(sa.size() == 512);
	sa.reserve(1024);
	QVERIFY(sa.capacity() == 1024);
	QVERIFY(sa.size() == 512);
    }
    {
	QVarLengthArray<QString> sa(10);
	sa[0] = "Hello";
	sa[9] = "World";
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
	sa.reserve(512);
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
	sa.resize(512);
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
    }
    {
        int arr[2] = {1, 2};
        QVarLengthArray<int> sa(10);
        QCOMPARE(sa.size(), 10);
        sa.append(arr, 2);
        QCOMPARE(sa.size(), 12);
        QCOMPARE(sa[10], 1);
        QCOMPARE(sa[11], 2);
    }
    {
        QString arr[2] = { QString("hello"), QString("world") };
        QVarLengthArray<QString> sa(10);
        QCOMPARE(sa.size(), 10);
        sa.append(arr, 2);
        QCOMPARE(sa.size(), 12);
        QCOMPARE(sa[10], QString("hello"));
        QCOMPARE(sa[11], QString("world"));

        sa.append(arr, 1);
        QCOMPARE(sa.size(), 13);
        QCOMPARE(sa[12], QString("hello"));

        sa.append(arr, 0);
        QCOMPARE(sa.size(), 13);
    }
    {
        // assignment operator and copy constructor

        QVarLengthArray<int> sa(10);
        sa[5] = 5;

        QVarLengthArray<int> sa2(10);
        sa2[5] = 6;
        sa2 = sa;
        QCOMPARE(sa2[5], 5);

        QVarLengthArray<int> sa3(sa);
        QCOMPARE(sa3[5], 5);
    }

    QSKIP("This test causes the machine to crash when allocating too much memory.", SkipSingle);
    {
        QVarLengthArray<Foo> a;
        const int N = 0x7fffffff / sizeof(Foo);
        const int Prealloc = a.capacity();
        const Foo *data0 = a.constData();

        a.resize(N);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), Prealloc);
            QCOMPARE(a.constData(), data0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);

            a.resize(5);
            QCOMPARE(a.size(), 5);
            QCOMPARE(a.capacity(), Prealloc);
            QCOMPARE(a.constData(), data0);
            QCOMPARE(fooCtor, 5);
            QCOMPARE(fooDtor, 0);

            a.resize(Prealloc + 1);
            QCOMPARE(a.size(), Prealloc + 1);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QVERIFY(a.constData() != data0);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, 5);

            const Foo *data1 = a.constData();

            a.resize(0x10000000);
            QCOMPARE(a.size(), 0);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QVERIFY(a.constData() == data1);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, Prealloc + 6);
        }
    }
}

void tst_QVarLengthArray::task214223()
{
    //creating a QVarLengthArray of the same size as the prealloc size
    // will make the next call to append(const T&) corrupt the memory
    // you should get a segfault pretty soon after that :-)
    QVarLengthArray<float, 1> d(1);
    for (int i=0; i<30; i++) 
        d.append(i);
}

QTEST_APPLESS_MAIN(tst_QVarLengthArray)
#include "tst_qvarlengtharray.moc"
