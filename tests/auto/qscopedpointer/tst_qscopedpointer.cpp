/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtCore/QScopedPointer>

/*!
 \class tst_QScopedPointer
 \internal
 \since 4.6
 \brief Tests class QScopedPointer.

 */
class tst_QScopedPointer : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defaultConstructor();
    void dataOnDefaultConstructed();
    void useSubClassInConstructor();
    void dataOnValue();
    void dataSignature();
    void reset();
    void dereferenceOperator();
    void dereferenceOperatorSignature();
    void pointerOperator();
    void pointerOperatorSignature();
    void negationOperator();
    void negationOperatorSignature();
    void operatorBool();
    void operatorBoolSignature();
    void isNull();
    void isNullSignature();
    void objectSize();
    void comparison();
    // TODO instanciate on const object
};

void tst_QScopedPointer::defaultConstructor()
{
    /* Check that the members, one, is correctly initialized. */
    QScopedPointer<int> p;
    QCOMPARE(p.data(), static_cast<int *>(0));
}

void tst_QScopedPointer::dataOnDefaultConstructed()
{
    QScopedPointer<int> p;

    QCOMPARE(p.data(), static_cast<int *>(0));
}

class MyClass
{
};

class MySubClass : public MyClass
{
};

void tst_QScopedPointer::useSubClassInConstructor()
{
    /* Use a syntax which users typically would do. */
    QScopedPointer<MyClass> p(new MyClass());
}

void tst_QScopedPointer::dataOnValue()
{
    int *const rawPointer = new int(5);
    QScopedPointer<int> p(rawPointer);

    QCOMPARE(p.data(), rawPointer);
}

void tst_QScopedPointer::dataSignature()
{
    const QScopedPointer<int> p;
    /* data() should be const. */
    p.data();
}

void tst_QScopedPointer::reset()
{
    /* Call reset() on a default constructed value. */
    {
        QScopedPointer<int> p;
        p.reset();
        QCOMPARE(p.data(), static_cast<int *>(0));
    }

    /* Call reset() on an active value. */
    {
        QScopedPointer<int> p(new int(3));
        p.reset();
        QCOMPARE(p.data(), static_cast<int *>(0));
    }

    /* Call reset() with a value, on an active value. */
    {
        QScopedPointer<int> p(new int(3));

        int *const value = new int(9);
        p.reset(value);
        QCOMPARE(*p.data(), 9);
    }

    /* Call reset() with a value, on default constructed value. */
    {
        QScopedPointer<int> p;

        int *const value = new int(9);
        p.reset(value);
        QCOMPARE(*p.data(), 9);
    }
}

class AbstractClass
{
public:
    virtual ~AbstractClass()
    {
    }

    virtual int member() const = 0;
};

class SubClass : public AbstractClass
{
public:
    virtual int member() const
    {
        return 5;
    }
};

void tst_QScopedPointer::dereferenceOperator()
{
    /* Dereference a basic value. */
    {
        QScopedPointer<int> p(new int(5));

        const int value2 = *p;
        QCOMPARE(value2, 5);
    }

    /* Dereference a pointer to an abstract class. This verifies
     * that the operator returns a reference, when compiling
     * with MSVC 2005. */
    {
        QScopedPointer<AbstractClass> p(new SubClass());

        QCOMPARE((*p).member(), 5);
    }
}

void tst_QScopedPointer::dereferenceOperatorSignature()
{
    /* The operator should be const. */
    {
        const QScopedPointer<int> p(new int(5));
        *p;
    }

    /* A reference should be returned, not a value. */
    {
        const QScopedPointer<int> p(new int(5));
        Q_UNUSED(static_cast<int &>(*p));
    }

    /* Instantiated on a const object, the returned object is a const reference. */
    {
        const QScopedPointer<const int> p(new int(5));
        Q_UNUSED(static_cast<const int &>(*p));
    }
}

class AnyForm
{
public:
    int value;
};

void tst_QScopedPointer::pointerOperator()
{
    QScopedPointer<AnyForm> p(new AnyForm());
    p->value = 5;

    QCOMPARE(p->value, 5);
}

void tst_QScopedPointer::pointerOperatorSignature()
{
    /* The operator should be const. */
    const QScopedPointer<AnyForm> p(new AnyForm);
    p->value = 5;

    QVERIFY(p->value);
}

void tst_QScopedPointer::negationOperator()
{
    /* Invoke on default constructed value. */
    {
        QScopedPointer<int> p;
        QVERIFY(!p);
    }

    /* Invoke on a value. */
    {
        QScopedPointer<int> p(new int(2));
        QCOMPARE(!p, false);
    }
}

void tst_QScopedPointer::negationOperatorSignature()
{
    /* The signature should be const. */
    const QScopedPointer<int> p;
    !p;

    /* The return value should be bool. */
    static_cast<bool>(!p);
}

void tst_QScopedPointer::operatorBool()
{
    /* Invoke on default constructed value. */
    {
        QScopedPointer<int> p;
        QCOMPARE(bool(p), false);
    }

    /* Invoke on active value. */
    {
        QScopedPointer<int> p(new int(3));
        QVERIFY(p);
    }
}

void tst_QScopedPointer::operatorBoolSignature()
{
    /* The signature should be const and return bool. */
    const QScopedPointer<int> p;

    (void)static_cast<bool>(p);
}

void tst_QScopedPointer::isNull()
{
    /* Invoke on default constructed value. */
    {
        QScopedPointer<int> p;
        QVERIFY(p.isNull());
    }

    /* Invoke on a set value. */
    {
        QScopedPointer<int> p(new int(69));
        QVERIFY(!p.isNull());
    }
}

void tst_QScopedPointer::isNullSignature()
{
    const QScopedPointer<int> p(new int(69));

    /* The signature should be const and return bool. */
    static_cast<bool>(p.isNull());
}

void tst_QScopedPointer::objectSize()
{
    /* The size of QScopedPointer should be the same as one pointer. */
    QCOMPARE(sizeof(QScopedPointer<int>), sizeof(void *));
}

void tst_QScopedPointer::comparison()
{
    int *a = new int(42);
    int *b = new int(43);

    QScopedPointer<int> pa(a);
    QScopedPointer<int> pa2(a);
    QScopedPointer<int> pb(b);

    // test equality on equal pointers
    QVERIFY(pa == pa2);
    QVERIFY(pa2 == pa);

    // test unequality on equal pointers
    QVERIFY(!(pa != pa2));
    QVERIFY(!(pa2 != pa));

    // test on unequal pointers
    QVERIFY(!(pa == pb));
    QVERIFY(!(pb == pa));
    QVERIFY(pb != pa);
    QVERIFY(pa != pb);

    pa2.take();
}

QTEST_MAIN(tst_QScopedPointer)
#include "tst_qscopedpointer.moc"
