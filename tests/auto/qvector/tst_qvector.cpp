/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <qvector.h>

//TESTED_CLASS=
//TESTED_FILES=corelib/tools/qregexp.h corelib/tools/qregexp.cpp

class tst_QVector : public QObject
{
    Q_OBJECT

public:
    tst_QVector() {}
    virtual ~tst_QVector() {}

private slots:
    void outOfMemory();
    void QTBUG6416_reserve();
};

int fooCtor;
int fooDtor;

struct Foo
{
    int *p;

    Foo() { p = new int; ++fooCtor; }
    Foo(const Foo &other) { Q_UNUSED(other); p = new int; ++fooCtor; }

    void operator=(const Foo & /* other */) { }

    ~Foo() { delete p; ++fooDtor; }
};

void tst_QVector::outOfMemory()
{
    fooCtor = 0;
    fooDtor = 0;

    const int N = 0x7fffffff / sizeof(Foo);

    {
        QVector<Foo> a;

        QSKIP("This test crashes on many of our machines.", SkipSingle);
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
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);

            a.resize(5);
            QCOMPARE(a.size(), 5);
            QVERIFY(a.capacity() >= 5);
            QCOMPARE(fooCtor, 5);
            QCOMPARE(fooDtor, 0);

            const int Prealloc = a.capacity();
            a.resize(Prealloc + 1);
            QCOMPARE(a.size(), Prealloc + 1);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, 5);

            a.resize(0x10000000);
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, Prealloc + 6);
        }
    }

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a(N);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);
        }
    }

    Foo foo;

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a(N, foo);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);
        }
    }

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a;
        a.resize(10);
        QCOMPARE(fooCtor, 10);
        QCOMPARE(fooDtor, 0);

        QVector<Foo> b(a);
        QCOMPARE(fooCtor, 10);
        QCOMPARE(fooDtor, 0);

        a.resize(N);
        if (a.size() == N) {
            QCOMPARE(fooCtor, N + 10);
        } else {
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 10);
            QCOMPARE(fooDtor, 0);

            QCOMPARE(b.size(), 10);
            QVERIFY(b.capacity() >= 10);
        }
    }

    {
        QVector<int> a;
        a.resize(10);

        QVector<int> b(a);

        a.resize(N);
        if (a.size() == N) {
            for (int i = 0; i < N; i += 60000)
                a[i] = i;
        } else {
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);

            QCOMPARE(b.size(), 10);
            QVERIFY(b.capacity() >= 10);
        }

        b.resize(N - 1);
        if (b.size() == N - 1) {
            for (int i = 0; i < N - 1; i += 60000)
                b[i] = i;
        } else {
            QCOMPARE(b.size(), 0);
            QCOMPARE(b.capacity(), 0);
        }
    }
}

void tst_QVector::QTBUG6416_reserve()
{
    fooCtor = 0;
    fooDtor = 0;
    {
        QVector<Foo> a;
        a.resize(2);
        QVector<Foo> b(a);
        b.reserve(1);
    }
    QCOMPARE(fooCtor, fooDtor);
}

QTEST_APPLESS_MAIN(tst_QVector)
#include "tst_qvector.moc"
