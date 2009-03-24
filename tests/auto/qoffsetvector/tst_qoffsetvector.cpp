/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <QTest>
#include <QList>
#include <QString>
#include <QCache>
#include <QOffsetVector>

#include <QDebug>
#include <stdio.h>


#if defined(FORCE_UREF)
template <class aT>
inline QDebug &operator<<(QDebug debug, const QOffsetVector<aT> &offsetVector)
#else
template <class aT>
inline QDebug operator<<(QDebug debug, const QOffsetVector<aT> &offsetVector)
#endif
{
    debug.nospace() << "QOffsetVector(";
    for (int i = offsetVector.firstIndex(); i <= offsetVector.lastIndex(); ++i) {
        debug << offsetVector[i];
        if (i != offsetVector.lastIndex())
            debug << ", ";
    }
    debug << ")";
    return debug.space();
}

#if defined(NO_BENCHMARK) and defined(QBENCHMARK)
#undef QBENCHMARK
#define QBENCHMARK
#endif

class tst_QOffsetVector : public QObject
{
    Q_OBJECT
public:
    tst_QOffsetVector() {}
    virtual ~tst_QOffsetVector() {}
private slots:
    void empty();
    void forwardBuffer();
    void scrollingList();

    void complexType();

    void operatorAt();

    void cacheBenchmark();
    void offsetVectorBenchmark();

    void setCapacity();
};

QTEST_MAIN(tst_QOffsetVector)

void tst_QOffsetVector::empty()
{
    QOffsetVector<int> c(10);
    QCOMPARE(c.capacity(), 10);
    QCOMPARE(c.count(), 0);
    QVERIFY(c.isEmpty());
    c.append(1);
    QVERIFY(!c.isEmpty());
    c.clear();
    QCOMPARE(c.capacity(), 10);
    QCOMPARE(c.count(), 0);
    QVERIFY(c.isEmpty());
    c.prepend(1);
    QVERIFY(!c.isEmpty());
    c.clear();
    QCOMPARE(c.count(), 0);
    QVERIFY(c.isEmpty());
    QCOMPARE(c.capacity(), 10);
}

void tst_QOffsetVector::forwardBuffer()
{
    int i;
    QOffsetVector<int> c(10);
    for(i = 1; i < 30; ++i) {
        c.append(i);
        QCOMPARE(c.first(), qMax(1, i-9));
        QCOMPARE(c.last(), i);
        QCOMPARE(c.count(), qMin(i, 10));
    }

    c.clear();

    for(i = 1; i < 30; ++i) {
        c.prepend(i);
        QCOMPARE(c.last(), qMax(1, i-9));
        QCOMPARE(c.first(), i);
        QCOMPARE(c.count(), qMin(i, 10));
    }
}

void tst_QOffsetVector::scrollingList()
{
    int i;
    QOffsetVector<int> c(10);

    // Once allocated QOffsetVector should not
    // allocate any additional memory for non
    // complex data types.
    QBENCHMARK {
        // simulate scrolling in a list of items;
        for(i = 0; i < 10; ++i)
            c.append(i);

        QCOMPARE(c.firstIndex(), 0);
        QCOMPARE(c.lastIndex(), 9);
        QVERIFY(c.containsIndex(0));
        QVERIFY(c.containsIndex(9));
        QVERIFY(!c.containsIndex(10));

        for (i = 0; i < 10; ++i)
            QCOMPARE(c.at(i), i);

        for (i = 10; i < 30; ++i)
            c.append(i);

        QCOMPARE(c.firstIndex(), 20);
        QCOMPARE(c.lastIndex(), 29);
        QVERIFY(c.containsIndex(20));
        QVERIFY(c.containsIndex(29));
        QVERIFY(!c.containsIndex(30));

        for (i = 20; i < 30; ++i)
            QCOMPARE(c.at(i), i);

        for (i = 19; i >= 10; --i)
            c.prepend(i);

        QCOMPARE(c.firstIndex(), 10);
        QCOMPARE(c.lastIndex(), 19);
        QVERIFY(c.containsIndex(10));
        QVERIFY(c.containsIndex(19));
        QVERIFY(!c.containsIndex(20));

        for (i = 10; i < 20; ++i)
            QCOMPARE(c.at(i), i);

        for (i = 200; i < 220; ++i)
            c.insert(i, i);

        QCOMPARE(c.firstIndex(), 210);
        QCOMPARE(c.lastIndex(), 219);
        QVERIFY(c.containsIndex(210));
        QVERIFY(c.containsIndex(219));
        QVERIFY(!c.containsIndex(300));
        QVERIFY(!c.containsIndex(209));

        for (i = 220; i < 220; ++i) {
            QVERIFY(c.containsIndex(i));
            QCOMPARE(c.at(i), i);
        }
        c.clear(); // needed to reset benchmark
    }

    // from a specific bug that was encountered.  100 to 299 cached, attempted to cache 250 - 205 via insert, failed.
    // bug was that item at 150 would instead be item that should have been inserted at 250
    c.setCapacity(200);
    for(i = 100; i < 300; ++i)
        c.insert(i, i);
    for (i = 250; i <= 306; ++i)
        c.insert(i, 1000+i);
    for (i = 107; i <= 306; ++i) {
        QVERIFY(c.containsIndex(i));
        QCOMPARE(c.at(i), i < 250 ? i : 1000+i);
    }
}

struct RefCountingClassData
{
    QBasicAtomicInt ref;
    static RefCountingClassData shared_null;
};

RefCountingClassData RefCountingClassData::shared_null = {
    Q_BASIC_ATOMIC_INITIALIZER(1)
};

class RefCountingClass
{
public:
    RefCountingClass() : d(&RefCountingClassData::shared_null) { d->ref.ref(); }

    RefCountingClass(const RefCountingClass &other)
    {
        d = other.d;
        d->ref.ref();
    }

    ~RefCountingClass()
    {
        if (!d->ref.deref())
            delete d;
    }

    RefCountingClass &operator=(const RefCountingClass &other)
    {
        if (!d->ref.deref())
            delete d;
        d = other.d;
        d->ref.ref();
        return *this;
    }

    int refCount() const { return d->ref; }
private:
    RefCountingClassData *d;
};

void tst_QOffsetVector::complexType()
{
    RefCountingClass original;

    QOffsetVector<RefCountingClass> offsetVector(10);
    offsetVector.append(original);
    QCOMPARE(original.refCount(), 3);
    offsetVector.removeFirst();
    QCOMPARE(original.refCount(), 2); // shared null, 'original'.
    offsetVector.append(original);
    QCOMPARE(original.refCount(), 3);
    offsetVector.clear();
    QCOMPARE(original.refCount(), 2);

    for(int i = 0; i < 100; ++i)
        offsetVector.insert(i, original);

    QCOMPARE(original.refCount(), 12); // shared null, 'original', + 10 in offsetVector.

    offsetVector.clear();
    QCOMPARE(original.refCount(), 2);
    for (int i = 0; i < 100; i++)
        offsetVector.append(original);

    QCOMPARE(original.refCount(), 12); // shared null, 'original', + 10 in offsetVector.
    offsetVector.clear();
    QCOMPARE(original.refCount(), 2);

    for (int i = 0; i < 100; i++)
        offsetVector.prepend(original);

    QCOMPARE(original.refCount(), 12); // shared null, 'original', + 10 in offsetVector.
    offsetVector.clear();
    QCOMPARE(original.refCount(), 2);

    for (int i = 0; i < 100; i++)
        offsetVector.append(original);

    offsetVector.takeLast();
    QCOMPARE(original.refCount(), 11);

    offsetVector.takeFirst();
    QCOMPARE(original.refCount(), 10);
}

void tst_QOffsetVector::operatorAt()
{
    RefCountingClass original;
    QOffsetVector<RefCountingClass> offsetVector(10);

    for (int i = 25; i < 35; ++i)
        offsetVector[i] = original;

    QCOMPARE(original.refCount(), 12); // shared null, orig, items in vector

    // verify const access does not copy items.
    const QOffsetVector<RefCountingClass> copy(offsetVector);
    for (int i = 25; i < 35; ++i)
        QCOMPARE(copy[i].refCount(), 12);

    // verify modifying the original increments ref count (e.g. does a detach)
    offsetVector[25] = original;
    QCOMPARE(original.refCount(), 22);
}

/*
    Benchmarks must be near identical in tasks to be fair.
    QCache uses pointers to ints as its a requirement of QCache,
    whereas QOffsetVector doesn't support pointers (won't free them).
    Given the ability to use simple data types is a benefit, its
    fair.  Although this obviously must take into account we are
    testing QOffsetVector use cases here, QCache has its own
    areas where it is the more sensible class to use.
*/
void tst_QOffsetVector::cacheBenchmark()
{
    QBENCHMARK {
        QCache<int, int> cache;
        cache.setMaxCost(100);

        for (int i = 0; i < 1000; i++)
            cache.insert(i, new int(i));
    }
}

void tst_QOffsetVector::offsetVectorBenchmark()
{
    QBENCHMARK {
        QOffsetVector<int> offsetVector(100);
        for (int i = 0; i < 1000; i++)
            offsetVector.insert(i, i);
    }
}

void tst_QOffsetVector::setCapacity()
{
    int i;
    QOffsetVector<int> offsetVector(100);
    for (i = 280; i < 310; ++i)
        offsetVector.insert(i, i);
    QCOMPARE(offsetVector.capacity(), 100);
    QCOMPARE(offsetVector.count(), 30);
    QCOMPARE(offsetVector.firstIndex(), 280);
    QCOMPARE(offsetVector.lastIndex(), 309);

    for (i = offsetVector.firstIndex(); i <= offsetVector.lastIndex(); ++i) {
        QVERIFY(offsetVector.containsIndex(i));
        QCOMPARE(offsetVector.at(i), i);
    }

    offsetVector.setCapacity(150);

    QCOMPARE(offsetVector.capacity(), 150);
    QCOMPARE(offsetVector.count(), 30);
    QCOMPARE(offsetVector.firstIndex(), 280);
    QCOMPARE(offsetVector.lastIndex(), 309);

    for (i = offsetVector.firstIndex(); i <= offsetVector.lastIndex(); ++i) {
        QVERIFY(offsetVector.containsIndex(i));
        QCOMPARE(offsetVector.at(i), i);
    }

    offsetVector.setCapacity(20);

    QCOMPARE(offsetVector.capacity(), 20);
    QCOMPARE(offsetVector.count(), 20);
    QCOMPARE(offsetVector.firstIndex(), 290);
    QCOMPARE(offsetVector.lastIndex(), 309);

    for (i = offsetVector.firstIndex(); i <= offsetVector.lastIndex(); ++i) {
        QVERIFY(offsetVector.containsIndex(i));
        QCOMPARE(offsetVector.at(i), i);
    }
}

#include "tst_qoffsetvector.moc"
