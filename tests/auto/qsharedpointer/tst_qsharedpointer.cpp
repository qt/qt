/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#define QT_SHAREDPOINTER_TRACK_POINTERS
#include "qsharedpointer.h"
#include "externaltests.h"
#include <QtTest/QtTest>

class tst_QSharedPointer: public QObject
{
    Q_OBJECT

private slots:
    void basics_data();
    void basics();
    void forwardDeclaration1();
    void forwardDeclaration2();
    void memoryManagement();
    void downCast();
    void upCast();
    void objectCast();
    void differentPointers();
    void virtualBaseDifferentPointers();
#ifndef QTEST_NO_RTTI
    void dynamicCast();
    void dynamicCastDifferentPointers();
    void dynamicCastVirtualBase();
    void dynamicCastFailure();
#endif
    void constCorrectness();
    void customDeleter();
    void creating();
    void validConstructs();
    void invalidConstructs_data();
    void invalidConstructs();
};

class Data
{
public:
    static int destructorCounter;
    static int generationCounter;
    int generation;

    Data() : generation(++generationCounter)
    { }

    virtual ~Data()
    {
        Q_ASSERT_X(generation > 0, "tst_QSharedPointer", "Double deletion!");
        generation = 0;
        ++destructorCounter;
    }

    void doDelete()
    {
        delete this;
    }

    bool alsoDelete()
    {
        doDelete();
        return true;
    }

    virtual void virtualDelete()
    {
        delete this;
    }

    virtual int classLevel() { return 1; }
};
int Data::generationCounter = 0;
int Data::destructorCounter = 0;

void tst_QSharedPointer::basics_data()
{
    QTest::addColumn<bool>("isNull");
    QTest::newRow("null") << true;
    QTest::newRow("non-null") << false;
}

void tst_QSharedPointer::basics()
{
    {
        QSharedPointer<Data> ptr;
        QWeakPointer<Data> weakref;

        QCOMPARE(sizeof(ptr), 2*sizeof(void*));
        QCOMPARE(sizeof(weakref), 2*sizeof(void*));
    }

    QFETCH(bool, isNull);
    Data *aData = 0;
    if (!isNull)
        aData = new Data;
    Data *otherData = new Data;
    QSharedPointer<Data> ptr(aData);

    {
        // basic self tests
        QCOMPARE(ptr.isNull(), isNull);
        QCOMPARE(bool(ptr), !isNull);
        QCOMPARE(!ptr, isNull);

        QCOMPARE(ptr.data(), aData);
        QCOMPARE(ptr.operator->(), aData);
        Data &dataReference = *ptr;
        QCOMPARE(&dataReference, aData);

        QVERIFY(ptr == aData);
        QVERIFY(!(ptr != aData));
        QVERIFY(aData == ptr);
        QVERIFY(!(aData != ptr));

        QVERIFY(ptr != otherData);
        QVERIFY(otherData != ptr);
        QVERIFY(! (ptr == otherData));
        QVERIFY(! (otherData == ptr));
    }
    QVERIFY(!ptr.d || ptr.d->weakref == 1);
    QVERIFY(!ptr.d || ptr.d->strongref == 1);

    {
        // create another object:
        QSharedPointer<Data> otherCopy(otherData);
        QVERIFY(ptr != otherCopy);
        QVERIFY(otherCopy != ptr);
        QVERIFY(! (ptr == otherCopy));
        QVERIFY(! (otherCopy == ptr));

        // otherData is deleted here
    }
    QVERIFY(!ptr.d || ptr.d->weakref == 1);
    QVERIFY(!ptr.d || ptr.d->strongref == 1);

    {
        // create a copy:
        QSharedPointer<Data> copy(ptr);
        QVERIFY(copy == ptr);
        QVERIFY(ptr == copy);
        QVERIFY(! (copy != ptr));
        QVERIFY(! (ptr != copy));
        QCOMPARE(copy, ptr);
        QCOMPARE(ptr, copy);

        QCOMPARE(copy.isNull(), isNull);
        QCOMPARE(copy.data(), aData);
        QVERIFY(copy == aData);
    }
    QVERIFY(!ptr.d || ptr.d->weakref == 1);
    QVERIFY(!ptr.d || ptr.d->strongref == 1);

    {
        // create a weak reference:
        QWeakPointer<Data> weak(ptr);
        QCOMPARE(weak.isNull(), isNull);
        QCOMPARE(!weak, isNull);
        QCOMPARE(bool(weak), !isNull);

        QVERIFY(ptr == weak);
        QVERIFY(weak == ptr);
        QVERIFY(! (ptr != weak));
        QVERIFY(! (weak != ptr));

        // create another reference:
        QWeakPointer<Data> weak2(weak);
        QCOMPARE(weak2.isNull(), isNull);
        QCOMPARE(!weak2, isNull);
        QCOMPARE(bool(weak2), !isNull);

        QVERIFY(weak2 == weak);
        QVERIFY(weak == weak2);
        QVERIFY(! (weak2 != weak));
        QVERIFY(! (weak != weak2));

        // create a strong reference back:
        QSharedPointer<Data> strong(weak);
        QVERIFY(strong == weak);
        QVERIFY(strong == ptr);
        QCOMPARE(strong.data(), aData);
    }
    QVERIFY(!ptr.d || ptr.d->weakref == 1);
    QVERIFY(!ptr.d || ptr.d->strongref == 1);

    // aData is deleted here
}

class ForwardDeclared;
ForwardDeclared *forwardPointer();
void externalForwardDeclaration();
extern int forwardDeclaredDestructorRunCount;

void tst_QSharedPointer::forwardDeclaration1()
{
#if defined(Q_CC_SUN)
    QSKIP("This type of forward declaration is not valid with this compiler", SkipAll);
#else
    externalForwardDeclaration();

    struct Wrapper { QSharedPointer<ForwardDeclared> pointer; };

    forwardDeclaredDestructorRunCount = 0;
    {
        Wrapper w;
        w.pointer = QSharedPointer<ForwardDeclared>(forwardPointer());
        QVERIFY(!w.pointer.isNull());
    }
    QCOMPARE(forwardDeclaredDestructorRunCount, 1);
#endif
}

#include "forwarddeclared.h"

void tst_QSharedPointer::forwardDeclaration2()
{
    forwardDeclaredDestructorRunCount = 0;
    {
        struct Wrapper { QSharedPointer<ForwardDeclared> pointer; };
        Wrapper w1, w2;
        w1.pointer = QSharedPointer<ForwardDeclared>(forwardPointer());
        QVERIFY(!w1.pointer.isNull());
    }
    QCOMPARE(forwardDeclaredDestructorRunCount, 1);
}

void tst_QSharedPointer::memoryManagement()
{
    int generation = Data::generationCounter + 1;
    int destructorCounter = Data::destructorCounter;

    QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data);
    QCOMPARE(ptr->generation, generation);
    QCOMPARE(Data::destructorCounter, destructorCounter);
    QCOMPARE(Data::generationCounter, generation);

    ptr = ptr;
    QCOMPARE(ptr->generation, generation);
    QCOMPARE(Data::destructorCounter, destructorCounter);
    QCOMPARE(Data::generationCounter, generation);

    {
        QSharedPointer<Data> copy = ptr;
        QCOMPARE(ptr->generation, generation);
        QCOMPARE(copy->generation, generation);

        // copy goes out of scope, ptr continues
    }
    QCOMPARE(ptr->generation, generation);
    QCOMPARE(Data::destructorCounter, destructorCounter);
    QCOMPARE(Data::generationCounter, generation);

    {
        QWeakPointer<Data> weak = ptr;
        weak = ptr;
        QCOMPARE(ptr->generation, generation);
        QCOMPARE(Data::destructorCounter, destructorCounter);
        QCOMPARE(Data::generationCounter, generation);

        weak = weak;
        QCOMPARE(ptr->generation, generation);
        QCOMPARE(Data::destructorCounter, destructorCounter);
        QCOMPARE(Data::generationCounter, generation);

        QSharedPointer<Data> strong = weak;
        QCOMPARE(ptr->generation, generation);
        QCOMPARE(strong->generation, generation);
        QCOMPARE(Data::destructorCounter, destructorCounter);
        QCOMPARE(Data::generationCounter, generation);

        // both weak and strong go out of scope
    }
    QCOMPARE(ptr->generation, generation);
    QCOMPARE(Data::destructorCounter, destructorCounter);
    QCOMPARE(Data::generationCounter, generation);

    QWeakPointer<Data> weak = ptr;
    ptr = QSharedPointer<Data>();

    // destructor must have been called
    QCOMPARE(Data::destructorCounter, destructorCounter + 1);
    QVERIFY(ptr.isNull());
    QVERIFY(weak.isNull());

    // if we create a strong pointer from the weak, it must still be null
    ptr = weak;
    QVERIFY(ptr.isNull());
    QVERIFY(ptr == 0);
    QCOMPARE(ptr.data(), (Data*)0);
}

class DerivedData: public Data
{
public:
    static int derivedDestructorCounter;
    int moreData;
    DerivedData() : moreData(0) { }
    ~DerivedData() { ++derivedDestructorCounter; }

    virtual void virtualDelete()
    {
        delete this;
    }

    virtual int classLevel() { return 2; }
};
int DerivedData::derivedDestructorCounter = 0;

class Stuffing
{
public:
    char buffer[16];
    Stuffing() { for (uint i = 0; i < sizeof buffer; ++i) buffer[i] = 16 - i; }
    virtual ~Stuffing() { }
};

class DiffPtrDerivedData: public Stuffing, public Data
{
public:
    virtual int classLevel() { return 3; }
};

class VirtualDerived: virtual public Data
{
public:
    int moreData;

    VirtualDerived() : moreData(0xc0ffee) { }
    virtual int classLevel() { return 4; }
};

void tst_QSharedPointer::downCast()
{
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData);
        QSharedPointer<Data> baseptr = qSharedPointerCast<Data>(ptr);
        QSharedPointer<Data> other;

        QVERIFY(ptr == baseptr);
        QVERIFY(baseptr == ptr);
        QVERIFY(! (ptr != baseptr));
        QVERIFY(! (baseptr != ptr));

        QVERIFY(ptr != other);
        QVERIFY(other != ptr);
        QVERIFY(! (ptr == other));
        QVERIFY(! (other == ptr));
    }

    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData);
        QSharedPointer<Data> baseptr = ptr;
    }

    int destructorCount;
    destructorCount = DerivedData::derivedDestructorCounter;
    {
        QSharedPointer<Data> baseptr;
        {
            QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData);
            baseptr = ptr;
            QVERIFY(baseptr == ptr);
        }
    }
    QCOMPARE(DerivedData::derivedDestructorCounter, destructorCount + 1);

    destructorCount = DerivedData::derivedDestructorCounter;
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData);
        QWeakPointer<Data> baseptr = ptr;
        QVERIFY(baseptr == ptr);

        ptr = QSharedPointer<DerivedData>();
        QVERIFY(baseptr.isNull());
    }
    QCOMPARE(DerivedData::derivedDestructorCounter, destructorCount + 1);

    destructorCount = DerivedData::derivedDestructorCounter;
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData);
        QWeakPointer<DerivedData> weakptr(ptr);

        QSharedPointer<Data> baseptr = weakptr;
        QVERIFY(baseptr == ptr);
        QWeakPointer<Data> baseweakptr = weakptr;
        QVERIFY(baseweakptr == ptr);
    }
    QCOMPARE(DerivedData::derivedDestructorCounter, destructorCount + 1);
}

void tst_QSharedPointer::upCast()
{
    QSharedPointer<Data> baseptr = QSharedPointer<Data>(new DerivedData);

    {
        QSharedPointer<DerivedData> derivedptr = qSharedPointerCast<DerivedData>(baseptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QWeakPointer<DerivedData> derivedptr = qWeakPointerCast<DerivedData>(baseptr);
        QVERIFY(baseptr == derivedptr);
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QWeakPointer<Data> weakptr = baseptr;
        QSharedPointer<DerivedData> derivedptr = qSharedPointerCast<DerivedData>(weakptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QSharedPointer<DerivedData> derivedptr = baseptr.staticCast<DerivedData>();
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);
}

class OtherObject: public QObject
{
    Q_OBJECT
};

void tst_QSharedPointer::objectCast()
{
    {
        OtherObject *data = new OtherObject;
        QSharedPointer<QObject> baseptr = QSharedPointer<QObject>(data);
        QVERIFY(baseptr == data);
        QVERIFY(data == baseptr);

        // perform object cast
        QSharedPointer<OtherObject> ptr = qSharedPointerObjectCast<OtherObject>(baseptr);
        QVERIFY(!ptr.isNull());
        QCOMPARE(ptr.data(), data);
        QVERIFY(ptr == data);

        // again:
        ptr = baseptr.objectCast<OtherObject>();
        QVERIFY(ptr == data);

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
        // again:
        ptr = qobject_cast<OtherObject *>(baseptr);
        QVERIFY(ptr == data);

        // again:
        ptr = qobject_cast<QSharedPointer<OtherObject> >(baseptr);
        QVERIFY(ptr == data);
#endif
    }

    {
        const OtherObject *data = new OtherObject;
        QSharedPointer<const QObject> baseptr = QSharedPointer<const QObject>(data);
        QVERIFY(baseptr == data);
        QVERIFY(data == baseptr);

        // perform object cast
        QSharedPointer<const OtherObject> ptr = qSharedPointerObjectCast<const OtherObject>(baseptr);
        QVERIFY(!ptr.isNull());
        QCOMPARE(ptr.data(), data);
        QVERIFY(ptr == data);

        // again:
        ptr = baseptr.objectCast<const OtherObject>();
        QVERIFY(ptr == data);

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
        // again:
        ptr = qobject_cast<const OtherObject *>(baseptr);
        QVERIFY(ptr == data);

        // again:
        ptr = qobject_cast<QSharedPointer<const OtherObject> >(baseptr);
        QVERIFY(ptr == data);
#endif
    }

    {
        OtherObject *data = new OtherObject;
        QPointer<OtherObject> qptr = data;
        QSharedPointer<OtherObject> ptr = QSharedPointer<OtherObject>(data);
        QWeakPointer<QObject> weakptr = ptr;

        {
            // perform object cast
            QSharedPointer<OtherObject> otherptr = qSharedPointerObjectCast<OtherObject>(weakptr);
            QVERIFY(otherptr == ptr);

            // again:
            otherptr = qobject_cast<OtherObject *>(weakptr);
            QVERIFY(otherptr == ptr);

            // again:
            otherptr = qobject_cast<QSharedPointer<OtherObject> >(weakptr);
            QVERIFY(otherptr == ptr);
        }

        // drop the reference:
        ptr.clear();
        QVERIFY(ptr.isNull());
        QVERIFY(qptr.isNull());
        QVERIFY(weakptr.toStrongRef().isNull());

        // verify that the object casts fail without crash
        QSharedPointer<OtherObject> otherptr = qSharedPointerObjectCast<OtherObject>(weakptr);
        QVERIFY(otherptr.isNull());

#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
        // again:
        otherptr = qobject_cast<OtherObject *>(weakptr);
        QVERIFY(otherptr.isNull());

        // again:
        otherptr = qobject_cast<QSharedPointer<OtherObject> >(weakptr);
        QVERIFY(otherptr.isNull());
#endif
    }
}

void tst_QSharedPointer::differentPointers()
{
    {
        DiffPtrDerivedData *aData = new DiffPtrDerivedData;
        Data *aBase = aData;
        Q_ASSERT(aData == aBase);
        Q_ASSERT(*reinterpret_cast<quintptr *>(&aData) != *reinterpret_cast<quintptr *>(&aBase));

        QSharedPointer<DiffPtrDerivedData> ptr = QSharedPointer<DiffPtrDerivedData>(aData);
        QSharedPointer<Data> baseptr = qSharedPointerCast<Data>(ptr);
        QVERIFY(ptr == baseptr);
        QVERIFY(ptr.data() == baseptr.data());
        QVERIFY(ptr == aBase);
        QVERIFY(ptr == aData);
        QVERIFY(baseptr == aData);
        QVERIFY(baseptr == aBase);
    }

    {
        DiffPtrDerivedData *aData = new DiffPtrDerivedData;
        Data *aBase = aData;
        Q_ASSERT(aData == aBase);
        Q_ASSERT(*reinterpret_cast<quintptr *>(&aData) != *reinterpret_cast<quintptr *>(&aBase));

        QSharedPointer<Data> baseptr = QSharedPointer<Data>(aData);
        QSharedPointer<DiffPtrDerivedData> ptr = qSharedPointerCast<DiffPtrDerivedData>(baseptr);
        QVERIFY(ptr == baseptr);
        QVERIFY(ptr.data() == baseptr.data());
        QVERIFY(ptr == aBase);
        QVERIFY(baseptr == aData);
    }

    {
        DiffPtrDerivedData *aData = new DiffPtrDerivedData;
        Data *aBase = aData;
        Q_ASSERT(aData == aBase);
        Q_ASSERT(*reinterpret_cast<quintptr *>(&aData) != *reinterpret_cast<quintptr *>(&aBase));

        QSharedPointer<DiffPtrDerivedData> ptr = QSharedPointer<DiffPtrDerivedData>(aData);
        QSharedPointer<Data> baseptr = ptr;
        QVERIFY(ptr == baseptr);
        QVERIFY(ptr.data() == baseptr.data());
        QVERIFY(ptr == aBase);
        QVERIFY(ptr == aData);
        QVERIFY(baseptr == aData);
        QVERIFY(baseptr == aBase);
    }
}

void tst_QSharedPointer::virtualBaseDifferentPointers()
{
    {
        VirtualDerived *aData = new VirtualDerived;
        Data *aBase = aData;
        Q_ASSERT(aData == aBase);
        Q_ASSERT(*reinterpret_cast<quintptr *>(&aData) != *reinterpret_cast<quintptr *>(&aBase));

        QSharedPointer<VirtualDerived> ptr = QSharedPointer<VirtualDerived>(aData);
        QSharedPointer<Data> baseptr = qSharedPointerCast<Data>(ptr);
        QVERIFY(ptr == baseptr);
        QVERIFY(ptr.data() == baseptr.data());
        QVERIFY(ptr == aBase);
        QVERIFY(ptr == aData);
        QVERIFY(baseptr == aData);
        QVERIFY(baseptr == aBase);
    }

    {
        VirtualDerived *aData = new VirtualDerived;
        Data *aBase = aData;
        Q_ASSERT(aData == aBase);
        Q_ASSERT(*reinterpret_cast<quintptr *>(&aData) != *reinterpret_cast<quintptr *>(&aBase));

        QSharedPointer<VirtualDerived> ptr = QSharedPointer<VirtualDerived>(aData);
        QSharedPointer<Data> baseptr = ptr;
        QVERIFY(ptr == baseptr);
        QVERIFY(ptr.data() == baseptr.data());
        QVERIFY(ptr == aBase);
        QVERIFY(ptr == aData);
        QVERIFY(baseptr == aData);
        QVERIFY(baseptr == aBase);
    }
}

#ifndef QTEST_NO_RTTI
void tst_QSharedPointer::dynamicCast()
{
    DerivedData *aData = new DerivedData;
    QSharedPointer<Data> baseptr = QSharedPointer<Data>(aData);

    {
        QSharedPointer<DerivedData> derivedptr = qSharedPointerDynamicCast<DerivedData>(baseptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QWeakPointer<Data> weakptr = baseptr;
        QSharedPointer<DerivedData> derivedptr = qSharedPointerDynamicCast<DerivedData>(weakptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QSharedPointer<DerivedData> derivedptr = baseptr.dynamicCast<DerivedData>();
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);
}

void tst_QSharedPointer::dynamicCastDifferentPointers()
{
    // DiffPtrDerivedData derives from both Data and Stuffing
    DiffPtrDerivedData *aData = new DiffPtrDerivedData;
    QSharedPointer<Data> baseptr = QSharedPointer<Data>(aData);

    {
        QSharedPointer<DiffPtrDerivedData> derivedptr = qSharedPointerDynamicCast<DiffPtrDerivedData>(baseptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QWeakPointer<Data> weakptr = baseptr;
        QSharedPointer<DiffPtrDerivedData> derivedptr = qSharedPointerDynamicCast<DiffPtrDerivedData>(weakptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QSharedPointer<DiffPtrDerivedData> derivedptr = baseptr.dynamicCast<DiffPtrDerivedData>();
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        Stuffing *nakedptr = dynamic_cast<Stuffing *>(baseptr.data());
        QVERIFY(nakedptr);

        QSharedPointer<Stuffing> otherbaseptr = qSharedPointerDynamicCast<Stuffing>(baseptr);
        QVERIFY(!otherbaseptr.isNull());
        QVERIFY(otherbaseptr == nakedptr);
        QCOMPARE(otherbaseptr.data(), nakedptr);
        QCOMPARE(static_cast<DiffPtrDerivedData*>(otherbaseptr.data()), aData);
    }
}

void tst_QSharedPointer::dynamicCastVirtualBase()
{
    VirtualDerived *aData = new VirtualDerived;
    QSharedPointer<Data> baseptr = QSharedPointer<Data>(aData);

    {
        QSharedPointer<VirtualDerived> derivedptr = qSharedPointerDynamicCast<VirtualDerived>(baseptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QWeakPointer<Data> weakptr = baseptr;
        QSharedPointer<VirtualDerived> derivedptr = qSharedPointerDynamicCast<VirtualDerived>(weakptr);
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QSharedPointer<VirtualDerived> derivedptr = baseptr.dynamicCast<VirtualDerived>();
        QVERIFY(baseptr == derivedptr);
        QCOMPARE(derivedptr.data(), aData);
        QCOMPARE(static_cast<Data *>(derivedptr.data()), baseptr.data());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);
}

void tst_QSharedPointer::dynamicCastFailure()
{
    QSharedPointer<Data> baseptr = QSharedPointer<Data>(new Data);
    QVERIFY(dynamic_cast<DerivedData *>(baseptr.data()) == 0);

    {
        QSharedPointer<DerivedData> derivedptr = qSharedPointerDynamicCast<DerivedData>(baseptr);
        QVERIFY(derivedptr.isNull());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);

    {
        QSharedPointer<DerivedData> derivedptr = baseptr.dynamicCast<DerivedData>();
        QVERIFY(derivedptr.isNull());
    }
    QCOMPARE(int(baseptr.d->weakref), 1);
    QCOMPARE(int(baseptr.d->strongref), 1);
}
#endif

void tst_QSharedPointer::constCorrectness()
{
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data);
        QSharedPointer<const Data> cptr(ptr);
        QSharedPointer<volatile Data> vptr(ptr);
        cptr = ptr;
        vptr = ptr;

        ptr = qSharedPointerConstCast<Data>(cptr);
        ptr = qSharedPointerConstCast<Data>(vptr);
        ptr = cptr.constCast<Data>();
        ptr = vptr.constCast<Data>();

#if !defined(Q_CC_HPACC) && !defined(QT_ARCH_PARISC)
        // the aCC series 3 compiler we have on the PA-RISC
        // machine crashes compiling this code

        QSharedPointer<const volatile Data> cvptr(ptr);
        QSharedPointer<const volatile Data> cvptr2(cptr);
        QSharedPointer<const volatile Data> cvptr3(vptr);
        cvptr = ptr;
        cvptr2 = cptr;
        cvptr3 = vptr;
        ptr = qSharedPointerConstCast<Data>(cvptr);
        ptr = cvptr.constCast<Data>();
#endif
    }

    {
        Data *aData = new Data;
        QSharedPointer<Data> ptr = QSharedPointer<Data>(aData);
        const QSharedPointer<Data> cptr = ptr;

        ptr = cptr;
        QSharedPointer<Data> other = qSharedPointerCast<Data>(cptr);
        other = qSharedPointerDynamicCast<Data>(cptr);

        QCOMPARE(cptr.data(), aData);
        QCOMPARE(cptr.operator->(), aData);
    }
}

static int customDeleterFnCallCount;
void customDeleterFn(Data *ptr)
{
    ++customDeleterFnCallCount;
    delete ptr;
}

template <typename T>
struct CustomDeleter
{
    inline void operator()(T *ptr)
    {
        delete ptr;
        ++callCount;
    }
    static int callCount;
};
template<typename T> int CustomDeleter<T>::callCount = 0;

void tst_QSharedPointer::customDeleter()
{
    {
        QSharedPointer<Data> ptr(new Data, &Data::doDelete);
        QSharedPointer<Data> ptr2(new Data, &Data::alsoDelete);
        QSharedPointer<Data> ptr3(new Data, &Data::virtualDelete);
    }
    {
        QSharedPointer<DerivedData> ptr(new DerivedData, &Data::doDelete);
        QSharedPointer<DerivedData> ptr2(new DerivedData, &Data::alsoDelete);
        QSharedPointer<DerivedData> ptr3(new DerivedData, &Data::virtualDelete);
    }

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, customDeleterFn);
        ptr.data();
        QCOMPARE(customDeleterFnCallCount, 0);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, customDeleterFn);
        QCOMPARE(customDeleterFnCallCount, 0);
        ptr.clear();
        QCOMPARE(customDeleterFnCallCount, 1);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, customDeleterFn);
        QCOMPARE(customDeleterFnCallCount, 0);
        ptr = QSharedPointer<Data>(new Data);
        QCOMPARE(customDeleterFnCallCount, 1);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new DerivedData, customDeleterFn);
        ptr.data();
        QCOMPARE(customDeleterFnCallCount, 0);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, customDeleterFn);
        ptr.data();
        QCOMPARE(customDeleterFnCallCount, 0);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> other;
        {
            QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, customDeleterFn);
            other = ptr;
            QCOMPARE(customDeleterFnCallCount, 0);
        }
        QCOMPARE(customDeleterFnCallCount, 0);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    customDeleterFnCallCount = 0;
    {
        QSharedPointer<Data> other;
        {
            QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, customDeleterFn);
            other = ptr;
            QCOMPARE(customDeleterFnCallCount, 0);
        }
        QCOMPARE(customDeleterFnCallCount, 0);
    }
    QCOMPARE(customDeleterFnCallCount, 1);

    CustomDeleter<Data> dataDeleter;
    dataDeleter.callCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, dataDeleter);
        ptr.data();
        QCOMPARE(dataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 1);

    dataDeleter.callCount = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, dataDeleter);
        QSharedPointer<Data> other = ptr;
        other.clear();
        QCOMPARE(dataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 1);

    dataDeleter.callCount = 0;
    {
        QSharedPointer<Data> other;
        {
            QSharedPointer<Data> ptr = QSharedPointer<Data>(new Data, dataDeleter);
            other = ptr;
            QCOMPARE(dataDeleter.callCount, 0);
        }
        QCOMPARE(dataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 1);

    dataDeleter.callCount = 0;
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, dataDeleter);
        ptr.data();
        QCOMPARE(dataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 1);

    CustomDeleter<DerivedData> derivedDataDeleter;
    derivedDataDeleter.callCount = 0;
    dataDeleter.callCount = 0;
    {
        QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, derivedDataDeleter);
        ptr.data();
        QCOMPARE(dataDeleter.callCount, 0);
        QCOMPARE(derivedDataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 0);
    QCOMPARE(derivedDataDeleter.callCount, 1);

    derivedDataDeleter.callCount = 0;
    dataDeleter.callCount = 0;
    {
        QSharedPointer<Data> other;
        {
            QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, dataDeleter);
            other = ptr;
            QCOMPARE(dataDeleter.callCount, 0);
            QCOMPARE(derivedDataDeleter.callCount, 0);
        }
        QCOMPARE(dataDeleter.callCount, 0);
        QCOMPARE(derivedDataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 1);
    QCOMPARE(derivedDataDeleter.callCount, 0);

    derivedDataDeleter.callCount = 0;
    dataDeleter.callCount = 0;
    {
        QSharedPointer<Data> other;
        {
            QSharedPointer<DerivedData> ptr = QSharedPointer<DerivedData>(new DerivedData, derivedDataDeleter);
            other = ptr;
            QCOMPARE(dataDeleter.callCount, 0);
            QCOMPARE(derivedDataDeleter.callCount, 0);
        }
        QCOMPARE(dataDeleter.callCount, 0);
        QCOMPARE(derivedDataDeleter.callCount, 0);
    }
    QCOMPARE(dataDeleter.callCount, 0);
    QCOMPARE(derivedDataDeleter.callCount, 1);
}

void tst_QSharedPointer::creating()
{
    Data::generationCounter = Data::destructorCounter = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>::create();
        QVERIFY(ptr.data());
        QCOMPARE(Data::generationCounter, 1);
        QCOMPARE(ptr->generation, 1);
        QCOMPARE(Data::destructorCounter, 0);

        QCOMPARE(ptr->classLevel(), 1);

        ptr.clear();
        QCOMPARE(Data::destructorCounter, 1);
    }

    Data::generationCounter = Data::destructorCounter = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<Data>::create();
        QWeakPointer<Data> weakptr = ptr;
        QtSharedPointer::ExternalRefCountData *d = ptr.d;

        ptr.clear();
        QVERIFY(ptr.isNull());
        QCOMPARE(Data::destructorCounter, 1);

        // valgrind will complain here if something happened to the pointer
        QVERIFY(d->weakref == 1);
        QVERIFY(d->strongref == 0);
    }

    Data::generationCounter = Data::destructorCounter = 0;
    DerivedData::derivedDestructorCounter = 0;
    {
        QSharedPointer<Data> ptr = QSharedPointer<DerivedData>::create();
        QCOMPARE(ptr->classLevel(), 2);
        QCOMPARE(ptr.staticCast<DerivedData>()->moreData, 0);
        ptr.clear();

        QCOMPARE(Data::destructorCounter, 1);
        QCOMPARE(DerivedData::derivedDestructorCounter, 1);
    }

    {
        QSharedPointer<Data> ptr = QSharedPointer<DiffPtrDerivedData>::create();
        QCOMPARE(ptr->classLevel(), 3);
        QCOMPARE(ptr.staticCast<DiffPtrDerivedData>()->buffer[7]+0, 16-7);
        QCOMPARE(ptr.staticCast<DiffPtrDerivedData>()->buffer[3]+0, 16-3);
        QCOMPARE(ptr.staticCast<DiffPtrDerivedData>()->buffer[0]+0, 16);
    }

    {
        QSharedPointer<VirtualDerived> ptr = QSharedPointer<VirtualDerived>::create();
        QCOMPARE(ptr->classLevel(), 4);
        QCOMPARE(ptr->moreData, 0xc0ffee);

        QSharedPointer<Data> baseptr = ptr;
        QCOMPARE(baseptr->classLevel(), 4);
    }

    {
        QSharedPointer<QObject> ptr = QSharedPointer<QObject>::create();
        QCOMPARE(ptr->metaObject(), &QObject::staticMetaObject);

        QPointer<QObject> qptr = ptr.data();
        ptr.clear();

        QVERIFY(qptr.isNull());
    }

    {
        QSharedPointer<QObject> ptr = QSharedPointer<OtherObject>::create();
        QCOMPARE(ptr->metaObject(), &OtherObject::staticMetaObject);
    }
}

void tst_QSharedPointer::validConstructs()
{
    {
        Data *aData = new Data;
        QSharedPointer<Data> ptr1 = QSharedPointer<Data>(aData);

        ptr1 = ptr1;            // valid

        QSharedPointer<Data> ptr2(ptr1);

        ptr1 = ptr2;
        ptr2 = ptr1;

        ptr1 = QSharedPointer<Data>();
        ptr1 = ptr2;
    }
}

typedef bool (QTest::QExternalTest:: * TestFunction)(const QByteArray &body);
Q_DECLARE_METATYPE(TestFunction)
void tst_QSharedPointer::invalidConstructs_data()
{
    QTest::addColumn<TestFunction>("testFunction");
    QTest::addColumn<QString>("code");
    QTest::newRow("sanity-checking") << &QTest::QExternalTest::tryCompile << "";

    // QSharedPointer<void> is not allowed
    QTest::newRow("void") << &QTest::QExternalTest::tryCompileFail << "QSharedPointer<void> ptr;";

    // implicit initialization
    QTest::newRow("implicit-initialization1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr = new Data;";
    QTest::newRow("implicit-initialization2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr;"
           "ptr = new Data;";
    QTest::newRow("implicit-initialization3")
        << &QTest::QExternalTest::tryCompileFail
        << "QWeakPointer<Data> ptr = new Data;";
    QTest::newRow("implicit-initialization4")
        << &QTest::QExternalTest::tryCompileFail
        << "QWeakPointer<Data> ptr;"
           "ptr = new Data;";

    // use of forward-declared class
    QTest::newRow("forward-declaration")
        << &QTest::QExternalTest::tryRun
        << "forwardDeclaredDestructorRunCount = 0;\n"
           "{ QSharedPointer<ForwardDeclared> ptr = QSharedPointer<ForwardDeclared>(forwardPointer()); }\n"
           "exit(forwardDeclaredDestructorRunCount);";
    QTest::newRow("creating-forward-declaration")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<ForwardDeclared>::create();";

    // upcast without cast operator:
    QTest::newRow("upcast1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> baseptr = QSharedPointer<Data>(new DerivedData);\n"
           "QSharedPointer<DerivedData> ptr(baseptr);";
    QTest::newRow("upcast2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> baseptr = QSharedPointer<Data>(new DerivedData);\n"
           "QSharedPointer<DerivedData> ptr;\n"
           "ptr = baseptr;";

    // dropping of const
    QTest::newRow("const-dropping1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const Data> baseptr = QSharedPointer<const Data>(new Data);\n"
           "QSharedPointer<Data> ptr(baseptr);";
    QTest::newRow("const-dropping2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const Data> baseptr = QSharedPointer<const Data>(new Data);\n"
           "QSharedPointer<Data> ptr;"
           "ptr = baseptr;";
    QTest::newRow("const-dropping-static-cast")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const Data> baseptr = QSharedPointer<const Data>(new Data);\n"
        "qSharedPointerCast<DerivedData>(baseptr);";
#ifndef QTEST_NO_RTTI
    QTest::newRow("const-dropping-dynamic-cast")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const Data> baseptr = QSharedPointer<const Data>(new Data);\n"
        "qSharedPointerDynamicCast<DerivedData>(baseptr);";
#endif
    QTest::newRow("const-dropping-object-cast1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const QObject> baseptr = QSharedPointer<const QObject>(new QObject);\n"
        "qSharedPointerObjectCast<QCoreApplication>(baseptr);";
#ifndef QT_NO_PARTIAL_TEMPLATE_SPECIALIZATION
    QTest::newRow("const-dropping-object-cast2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<const QObject> baseptr = QSharedPointer<const QObject>(new QObject);\n"
        "qobject_cast<QCoreApplication *>(baseptr);";
#endif

    // arithmethics through automatic cast operators
    QTest::newRow("arithmethic1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<int> a;"
           "QSharedPointer<Data> b;\n"
           "if (a == b) return;";
    QTest::newRow("arithmethic2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<int> a;"
           "QSharedPointer<Data> b;\n"
           "if (a + b) return;";

#if QT_VERSION >= 0x040600
    // two objects with the same pointer
    QTest::newRow("same-pointer")
        << &QTest::QExternalTest::tryRunFail
        << "Data *aData = new Data;\n"
           "QSharedPointer<Data> ptr1 = QSharedPointer<Data>(aData);\n"
           "QSharedPointer<Data> ptr2 = QSharedPointer<Data>(aData);\n";

    // re-creation:
    QTest::newRow("re-creation")
        << &QTest::QExternalTest::tryRunFail
        << "Data *aData = new Data;\n"
           "QSharedPointer<Data> ptr1 = QSharedPointer<Data>(aData);"
           "ptr1 = QSharedPointer<Data>(aData);";
#endif

    // any type of cast for unrelated types:
    // (we have no reinterpret_cast)
    QTest::newRow("invalid-cast1")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr1;\n"
           "QSharedPointer<int> ptr2 = qSharedPointerCast<int>(ptr1);";
#ifndef QTEST_NO_RTTI
    QTest::newRow("invalid-cast2")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr1;\n"
           "QSharedPointer<int> ptr2 = qSharedPointerDynamicCast<int>(ptr1);";
#endif
    QTest::newRow("invalid-cast3")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr1;\n"
           "QSharedPointer<int> ptr2 = qSharedPointerConstCast<int>(ptr1);";
    QTest::newRow("invalid-cast4")
        << &QTest::QExternalTest::tryCompileFail
        << "QSharedPointer<Data> ptr1;\n"
           "QSharedPointer<int> ptr2 = qSharedPointerObjectCast<int>(ptr1);";
}

void tst_QSharedPointer::invalidConstructs()
{
#ifdef Q_CC_MINGW
    QSKIP("The maintainer of QSharedPointer: 'We don't know what the problem is so skip the tests.'", SkipAll);
#endif
#ifdef QTEST_CROSS_COMPILED
    QSKIP("This test does not work on cross compiled systems", SkipAll);
#endif

    QTest::QExternalTest test;
    test.setDebugMode(true);
    test.setQtModules(QTest::QExternalTest::QtCore);
    test.setExtraProgramSources(QStringList() << SRCDIR "forwarddeclared.cpp");
    test.setProgramHeader(
        "#define QT_SHAREDPOINTER_TRACK_POINTERS\n"
        "#include <QtCore/qsharedpointer.h>\n"
        "#include <QtCore/qcoreapplication.h>\n"
        "\n"
        "struct Data { int i; };\n"
        "struct DerivedData: public Data { int j; };\n"
        "\n"
        "extern int forwardDeclaredDestructorRunCount;\n"
        "struct ForwardDeclared;\n"
        "ForwardDeclared *forwardPointer();\n"
        );

    QFETCH(QString, code);
    static bool sane = true;
    if (code.isEmpty()) {
        static const char snippet[] = "QSharedPointer<Data> baseptr; QSharedPointer<DerivedData> ptr;";
        if (!test.tryCompile("")
            || !test.tryRun("")
            || !test.tryRunFail("exit(1);")
            || !test.tryCompile(snippet)
            || !test.tryLink(snippet)
            || !test.tryRun(snippet)) {
            sane = false;
            qWarning("Sanity checking failed\nCode:\n%s\n",
                     qPrintable(test.errorReport()));
        }
    }
    if (!sane)
        QFAIL("External testing failed sanity checking, cannot proceed");

    QFETCH(TestFunction, testFunction);

    QByteArray body = code.toLatin1();

    bool result = (test.*testFunction)(body);
    if (qgetenv("QTEST_EXTERNAL_DEBUG").toInt() > 0) {
        qDebug("External test output:");
        printf("%s\n", test.standardError().constData());
    }
    if (!result) {
        qWarning("External code testing failed\nCode:\n%s\n", body.constData());
        QFAIL("Fail");
    }
}

QTEST_MAIN(tst_QSharedPointer)

#include "tst_qsharedpointer.moc"
