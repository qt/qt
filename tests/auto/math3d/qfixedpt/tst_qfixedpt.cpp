/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtCore/qmath.h>
#include <QtGui/qfixedpt.h>

class tst_QFixedPt : public QObject
{
    Q_OBJECT
public:
    tst_QFixedPt() {}
    ~tst_QFixedPt() {}

private slots:
    void create_data();
    void create();
    void add_data();
    void add();
    void sub_data();
    void sub();
    void mul_data();
    void mul();
    void div_data();
    void div();
    void neg_data();
    void neg();
    void shift_data();
    void shift();
    void sqrt_data();
    void sqrt();
    void round_data();
    void round();
    void compare_data();
    void compare();
};

// qFuzzyCompare isn't quite "fuzzy" enough to handle conversion
// to fixed-point and back again.  So create a "fuzzier" compare.
static bool fuzzyCompare(double x, double y)
{
    double diff = x - y;
    if (diff < 0.0f)
        diff = -diff;
    return (diff < 0.0001);
}

// Test the creation of QFixedPt values in various ways.
void tst_QFixedPt::create_data()
{
    QTest::addColumn<int>("value");
    QTest::addColumn<int>("intValue");
    QTest::addColumn<float>("realValue");

    QTest::newRow("zero") << 0x00000000 << 0 << 0.0f;
    QTest::newRow("one") << 0x00010000 << 1 << 1.0f;
    QTest::newRow("1.5") << 0x00018000 << 2 << 1.5f;         // int rounds up
    QTest::newRow("-1.5") << (int)0xFFFE8000 << -1 << -1.5f; // int rounds up
    QTest::newRow("-7") << (int)0xFFF90000 << -7 << -7.0f;
}
void tst_QFixedPt::create()
{
    QFETCH(int, value);
    QFETCH(int, intValue);
    QFETCH(float, realValue);

    if (qFloor(realValue) == realValue) {
        QFixedPt<16> ivalue(intValue);
        QCOMPARE(ivalue.bits(), value);
        QCOMPARE(ivalue.toInt(), intValue);
        QCOMPARE(ivalue.toReal(), (qreal)realValue);
    }

    QFixedPt<16> fvalue(realValue);
    QCOMPARE(fvalue.bits(), value);
    QCOMPARE(fvalue.toInt(), intValue);
    QCOMPARE(fvalue.toReal(), (qreal)realValue);

    QFixedPt<16> fpvalue;
    fpvalue.setBits(value);
    QCOMPARE(fpvalue.bits(), value);
    QCOMPARE(fpvalue.toInt(), intValue);
    QCOMPARE(fpvalue.toReal(), (qreal)realValue);

    QFixedPt<16> fpvalue2(fpvalue);
    QCOMPARE(fpvalue2.bits(), value);
    QCOMPARE(fpvalue2.toInt(), intValue);
    QCOMPARE(fpvalue2.toReal(), (qreal)realValue);

    if (qFloor(realValue) == realValue) {
        QFixedPt<16> ivalue2(63);   // Initialize the something else.
        ivalue2 = intValue;         // Then change the value.
        QCOMPARE(ivalue2.bits(), value);
        QCOMPARE(ivalue2.toInt(), intValue);
        QCOMPARE(ivalue2.toReal(), (qreal)realValue);
    }

    QFixedPt<16> fvalue2(36);
    fvalue2 = realValue;
    QCOMPARE(fvalue2.bits(), value);
    QCOMPARE(fvalue2.toInt(), intValue);
    QCOMPARE(fvalue2.toReal(), (qreal)realValue);

    QFixedPt<16> fpvalue3;
    fpvalue3 = fpvalue;
    QCOMPARE(fpvalue3.bits(), value);
    QCOMPARE(fpvalue3.toInt(), intValue);
    QCOMPARE(fpvalue3.toReal(), (qreal)realValue);

    // Now do some of the tests again with a different precision value.

    if (qFloor(realValue) == realValue) {
        QFixedPt<4> ivalue3(intValue);
        QCOMPARE(ivalue3.bits(), value >> 12);
        QCOMPARE(ivalue3.toInt(), intValue);
        QCOMPARE(ivalue3.toReal(), (qreal)realValue);
    }

    QFixedPt<4> fvalue3(realValue);
    QCOMPARE(fvalue3.bits(), value >> 12);
    QCOMPARE(fvalue3.toInt(), intValue);
    QCOMPARE(fvalue3.toReal(), (qreal)realValue);

    QFixedPt<4> fpvalue4;
    fpvalue4.setBits(value >> 12);
    QCOMPARE(fpvalue4.bits(), value >> 12);
    QCOMPARE(fpvalue4.toInt(), intValue);
    QCOMPARE(fpvalue4.toReal(), (qreal)realValue);

    // Test conversion between the precision values.

#if !defined(QT_NO_MEMBER_TEMPLATES)
    if (qFloor(realValue) == realValue) {
        QFixedPt<16> ivalue(intValue);
        QFixedPt<4> ivalue3(intValue);
        QVERIFY(ivalue.toPrecision<4>() == ivalue3);
        QVERIFY(ivalue3.toPrecision<16>() == ivalue);
    }
    QVERIFY(fvalue.toPrecision<4>() == fvalue3);
    QVERIFY(fvalue3.toPrecision<16>() == fvalue);
#endif

    if (qFloor(realValue) == realValue) {
        QFixedPt<16> ivalue(intValue);
        QFixedPt<4> ivalue3(intValue);
        QVERIFY(qFixedPtToPrecision<4>(ivalue) == ivalue3);
        QVERIFY(qFixedPtToPrecision<16>(ivalue3) == ivalue);
    }
    QVERIFY(qFixedPtToPrecision<4>(fvalue) == fvalue3);
    QVERIFY(qFixedPtToPrecision<16>(fvalue3) == fvalue);
}

// Test fixed point addition.
void tst_QFixedPt::add_data()
{
    QTest::addColumn<float>("a");
    QTest::addColumn<float>("b");

    QTest::newRow("zero") << 0.0f << 0.0f;
    QTest::newRow("test1") << 1.0f << 0.0f;
    QTest::newRow("test2") << 0.0f << 1.0f;
    QTest::newRow("test3") << 10.0f << -3.0f;
    QTest::newRow("test4") << 10.5f << 3.25f;
}
void tst_QFixedPt::add()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    QFixedPt<16> cvalue = avalue + bvalue;
    QFixedPt<16> dvalue = a + bvalue;
    QFixedPt<16> evalue = avalue + b;

    QCOMPARE(cvalue.toReal(), (qreal)(a + b));
    QCOMPARE(dvalue.toReal(), (qreal)(a + b));
    QCOMPARE(evalue.toReal(), (qreal)(a + b));

    QFixedPt<16> fvalue(avalue);
    fvalue += bvalue;
    QCOMPARE(fvalue.toReal(), (qreal)(a + b));

    QFixedPt<16> gvalue(avalue);
    gvalue += b;
    QCOMPARE(gvalue.toReal(), (qreal)(a + b));

    if (qFloor(a) == a && qFloor(b) == b) {
        QFixedPt<16> hvalue = int(a) + bvalue;
        QFixedPt<16> ivalue = avalue + int(b);

        QCOMPARE(hvalue.toInt(), int(a) + int(b));
        QCOMPARE(ivalue.toInt(), int(a) + int(b));
        QCOMPARE(hvalue.toReal(), (qreal)(a + b));
        QCOMPARE(ivalue.toReal(), (qreal)(a + b));

        QFixedPt<16> jvalue(avalue);
        jvalue += int(b);
        QCOMPARE(jvalue.toReal(), (qreal)(a + b));
    }
}

// Test fixed point subtraction.
void tst_QFixedPt::sub_data()
{
    // Use the same test data as the add() test.
    add_data();
}
void tst_QFixedPt::sub()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    QFixedPt<16> cvalue = avalue - bvalue;
    QFixedPt<16> dvalue = a - bvalue;
    QFixedPt<16> evalue = avalue - b;

    QCOMPARE(cvalue.toReal(), (qreal)(a - b));
    QCOMPARE(dvalue.toReal(), (qreal)(a - b));
    QCOMPARE(evalue.toReal(), (qreal)(a - b));

    QFixedPt<16> fvalue(avalue);
    fvalue -= bvalue;
    QCOMPARE(fvalue.toReal(), (qreal)(a - b));

    QFixedPt<16> gvalue(avalue);
    gvalue -= b;
    QCOMPARE(gvalue.toReal(), (qreal)(a - b));

    if (qFloor(a) == a && qFloor(b) == b) {
        QFixedPt<16> hvalue = int(a) - bvalue;
        QFixedPt<16> ivalue = avalue - int(b);

        QCOMPARE(hvalue.toInt(), int(a) - int(b));
        QCOMPARE(ivalue.toInt(), int(a) - int(b));
        QCOMPARE(hvalue.toReal(), (qreal)(a - b));
        QCOMPARE(ivalue.toReal(), (qreal)(a - b));

        QFixedPt<16> jvalue(avalue);
        jvalue -= int(b);
        QCOMPARE(jvalue.toReal(), (qreal)(a - b));
    }
}

// Test fixed point multiplication.
void tst_QFixedPt::mul_data()
{
    // Use the same test data as the add() test.
    add_data();
}
void tst_QFixedPt::mul()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    QFixedPt<16> cvalue = avalue * bvalue;
    QFixedPt<16> dvalue = a * bvalue;
    QFixedPt<16> evalue = avalue * b;

    QCOMPARE(cvalue.toReal(), (qreal)(a * b));
    QCOMPARE(dvalue.toReal(), (qreal)(a * b));
    QCOMPARE(evalue.toReal(), (qreal)(a * b));

    QFixedPt<16> fvalue(avalue);
    fvalue *= bvalue;
    QCOMPARE(fvalue.toReal(), (qreal)(a * b));

    QFixedPt<16> gvalue(avalue);
    gvalue *= b;
    QCOMPARE(gvalue.toReal(), (qreal)(a * b));

    if (qFloor(a) == a && qFloor(b) == b) {
        QFixedPt<16> hvalue = int(a) * bvalue;
        QFixedPt<16> ivalue = avalue * int(b);

        QCOMPARE(hvalue.toInt(), int(a) * int(b));
        QCOMPARE(ivalue.toInt(), int(a) * int(b));
        QCOMPARE(hvalue.toReal(), (qreal)(a * b));
        QCOMPARE(ivalue.toReal(), (qreal)(a * b));

        QFixedPt<16> jvalue(avalue);
        jvalue *= int(b);
        QCOMPARE(jvalue.toReal(), (qreal)(a * b));
    }
}

// Test fixed point division.
void tst_QFixedPt::div_data()
{
    // Use the same test data as the add() test.
    add_data();
}
void tst_QFixedPt::div()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    qreal result;
    if (b == 0.0f)
        result = 0.0f;       // Divide by zero results in zero.
    else
        result = a / b;

    QFixedPt<16> cvalue = avalue / bvalue;
    QFixedPt<16> dvalue = a / bvalue;
    QFixedPt<16> evalue = avalue / b;

    QVERIFY(fuzzyCompare(cvalue.toReal(), result));
    QVERIFY(fuzzyCompare(dvalue.toReal(), result));
    QVERIFY(fuzzyCompare(evalue.toReal(), result));

    QFixedPt<16> fvalue(avalue);
    fvalue /= bvalue;
    QVERIFY(fuzzyCompare(fvalue.toReal(), result));

    QFixedPt<16> gvalue(avalue);
    gvalue /= b;
    QVERIFY(fuzzyCompare(gvalue.toReal(), result));

    if (qFloor(a) == a && qFloor(b) == b) {
        QFixedPt<16> hvalue = int(a) / bvalue;
        QFixedPt<16> ivalue = avalue / int(b);

        QCOMPARE(hvalue.toInt(), int(result));
        QCOMPARE(ivalue.toInt(), int(result));
        QVERIFY(fuzzyCompare(hvalue.toReal(), result));
        QVERIFY(fuzzyCompare(ivalue.toReal(), result));

        QFixedPt<16> jvalue(avalue);
        jvalue /= int(b);
        QVERIFY(fuzzyCompare(jvalue.toReal(), result));
    }
}

// Test fixed point negation.
void tst_QFixedPt::neg_data()
{
    // Use the same test data as the add() test.
    add_data();
}
void tst_QFixedPt::neg()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    QFixedPt<16> cvalue = -avalue;
    QCOMPARE(cvalue.bits(), -avalue.bits());
    QCOMPARE(cvalue.toInt(), int(-a));
    QCOMPARE(cvalue.toReal(), (qreal)-a);

    QFixedPt<16> dvalue = -bvalue;
    QCOMPARE(dvalue.bits(), -bvalue.bits());
    QCOMPARE(dvalue.toInt(), int(-b));
    QCOMPARE(dvalue.toReal(), (qreal)-b);
}

// Test left and right shift operators on fixed point values.
void tst_QFixedPt::shift_data()
{
    QTest::addColumn<float>("a");
    QTest::addColumn<int>("amount");

    QTest::newRow("zero") << 0.0f << 5;
    QTest::newRow("one") << 1.0f << 4;
    QTest::newRow("-1.75") << -1.75f << 4;
}
void tst_QFixedPt::shift()
{
    QFETCH(float, a);
    QFETCH(int, amount);

    int lresult = int((a * 65536.0) * (1 << amount));
    int rresult = int((a * 65536.0) / (1 << amount));

    QFixedPt<16> avalue(a);
    avalue <<= amount;
    QCOMPARE(avalue.bits(), lresult);

    QFixedPt<16> bvalue(a);
    bvalue >>= amount;
    QCOMPARE(bvalue.bits(), rresult);

    QFixedPt<16> cvalue(a);

    QFixedPt<16> dvalue;
    dvalue = cvalue << amount;
    QCOMPARE(dvalue.bits(), lresult);

    QFixedPt<16> evalue;
    evalue = cvalue >> amount;
    QCOMPARE(evalue.bits(), rresult);
}

// Test fixed point square root.
void tst_QFixedPt::sqrt_data()
{
    QTest::addColumn<float>("a");

    QTest::newRow("zero") << 0.0f;
    QTest::newRow("one") << 1.0f;
    QTest::newRow("two") << 2.0f;
    QTest::newRow("sixteen") << 16.0f;
    QTest::newRow("1.5") << 1.5f;
}
void tst_QFixedPt::sqrt()
{
    QFETCH(float, a);

    QFixedPt<16> avalue(a);
    QVERIFY(fuzzyCompare(avalue.sqrt().toReal(), qSqrt(a)));
    QVERIFY(fuzzyCompare(avalue.sqrtF(), qSqrt(a)));
}

// Test fixed point rounding.
void tst_QFixedPt::round_data()
{
    QTest::addColumn<float>("a");
    QTest::addColumn<int>("rounded");
    QTest::addColumn<int>("ceiling");
    QTest::addColumn<int>("flooring");
    QTest::addColumn<int>("truncated");

    QTest::newRow("zero") << 0.0f << 0 << 0 << 0 << 0;
    QTest::newRow("test1") << 1.0f << 1 << 1 << 1 << 1;
    QTest::newRow("test2") << 2.5f << 3 << 3 << 2 << 2;
    QTest::newRow("test3") << 2.3f << 2 << 3 << 2 << 2;
    QTest::newRow("test4") << -2.5f << -2 << -2 << -3 << -3;
    QTest::newRow("test5") << -2.3f << -2 << -2 << -3 << -3;
    QTest::newRow("test6") << -2.7f << -3 << -2 << -3 << -3;
}
void tst_QFixedPt::round()
{
    QFETCH(float, a);
    QFETCH(int, rounded);
    QFETCH(int, ceiling);
    QFETCH(int, flooring);
    QFETCH(int, truncated);

    QFixedPt<16> avalue(a);
    QVERIFY(avalue.round() == rounded);
    QVERIFY(avalue.ceil() == ceiling);
    QVERIFY(avalue.floor() == flooring);
    QVERIFY(avalue.truncate() == truncated);

    QCOMPARE(qRound(avalue), rounded);
    QCOMPARE(qCeil(avalue), ceiling);
    QCOMPARE(qFloor(avalue), flooring);
}

// Test comparison operators.
void tst_QFixedPt::compare_data()
{
    QTest::addColumn<float>("a");
    QTest::addColumn<float>("b");

    QTest::newRow("test1") << 0.0f << 0.0f;
    QTest::newRow("test2") << 1.0f << 0.0f;
    QTest::newRow("test3") << 2.5f << 2.5f;
    QTest::newRow("test4") << 2.5f << -2.5f;
    QTest::newRow("test5") << -2.5f << 2.5f;
}
void tst_QFixedPt::compare()
{
    QFETCH(float, a);
    QFETCH(float, b);

    QFixedPt<16> avalue(a);
    QFixedPt<16> bvalue(b);

    if (a == b) {
        QVERIFY(avalue == bvalue);
        QVERIFY(avalue == b);
        QVERIFY(a == bvalue);
        QVERIFY(!(avalue != bvalue));
        QVERIFY(!(avalue != b));
        QVERIFY(!(a != bvalue));
        QVERIFY(!(avalue < bvalue));
        QVERIFY(!(avalue < b));
        QVERIFY(!(a < bvalue));
        QVERIFY(!(avalue > bvalue));
        QVERIFY(!(avalue > b));
        QVERIFY(!(a > bvalue));
        QVERIFY(avalue <= bvalue);
        QVERIFY(avalue <= b);
        QVERIFY(a <= bvalue);
        QVERIFY(avalue >= bvalue);
        QVERIFY(avalue >= b);
        QVERIFY(a >= bvalue);
        if (qFloor(a) == a) {
            QVERIFY(int(a) == bvalue);
            QVERIFY(!(int(a) != bvalue));
            QVERIFY(!(int(a) < bvalue));
            QVERIFY(!(int(a) > bvalue));
            QVERIFY(int(a) <= bvalue);
            QVERIFY(int(a) >= bvalue);
        }
        if (qFloor(b) == b) {
            QVERIFY(avalue == int(b));
            QVERIFY(!(avalue != int(b)));
            QVERIFY(!(avalue < int(b)));
            QVERIFY(!(avalue > int(b)));
            QVERIFY(avalue <= int(b));
            QVERIFY(avalue >= int(b));
        }
    }

    if (a != b) {
        QVERIFY(avalue != bvalue);
        QVERIFY(avalue != b);
        QVERIFY(a != bvalue);
        QVERIFY(!(avalue == bvalue));
        QVERIFY(!(avalue == b));
        QVERIFY(!(a == bvalue));
        if (qFloor(a) == a) {
            QVERIFY(int(a) != bvalue);
            QVERIFY(!(int(a) == bvalue));
        }
        if (qFloor(b) == b) {
            QVERIFY(avalue != int(b));
            QVERIFY(!(avalue == int(b)));
        }
    }

    if (a < b) {
        QVERIFY(avalue < bvalue);
        QVERIFY(avalue < b);
        QVERIFY(a < bvalue);
        QVERIFY(!(avalue >= bvalue));
        QVERIFY(!(avalue >= b));
        QVERIFY(!(a >= bvalue));
        QVERIFY(!(avalue > bvalue));
        QVERIFY(!(avalue > b));
        QVERIFY(!(a > bvalue));
        QVERIFY(avalue <= bvalue);
        QVERIFY(avalue <= b);
        QVERIFY(a <= bvalue);
        QVERIFY(!(avalue >= bvalue));
        QVERIFY(!(avalue >= b));
        QVERIFY(!(a >= bvalue));
        if (qFloor(a) == a) {
            QVERIFY(int(a) < bvalue);
            QVERIFY(!(int(a) >= bvalue));
            QVERIFY(!(int(a) > bvalue));
            QVERIFY(int(a) <= bvalue);
        }
        if (qFloor(b) == b) {
            QVERIFY(avalue < int(b));
            QVERIFY(!(avalue >= int(b)));
            QVERIFY(!(avalue > int(b)));
            QVERIFY(avalue <= int(b));
        }
    }

    if (a > b) {
        QVERIFY(avalue > bvalue);
        QVERIFY(avalue > b);
        QVERIFY(a > bvalue);
        QVERIFY(!(avalue <= bvalue));
        QVERIFY(!(avalue <= b));
        QVERIFY(!(a <= bvalue));
        QVERIFY(!(avalue < bvalue));
        QVERIFY(!(avalue < b));
        QVERIFY(!(a < bvalue));
        QVERIFY(avalue >= bvalue);
        QVERIFY(avalue >= b);
        QVERIFY(a >= bvalue);
        QVERIFY(!(avalue <= bvalue));
        QVERIFY(!(avalue <= b));
        QVERIFY(!(a <= bvalue));
        if (qFloor(a) == a) {
            QVERIFY(int(a) > bvalue);
            QVERIFY(!(int(a) <= bvalue));
            QVERIFY(!(int(a) < bvalue));
            QVERIFY(int(a) >= bvalue);
        }
        if (qFloor(b) == b) {
            QVERIFY(avalue > int(b));
            QVERIFY(!(avalue <= int(b)));
            QVERIFY(!(avalue < int(b)));
            QVERIFY(avalue >= int(b));
        }
    }

    if (qFuzzyCompare(a, b))
        QVERIFY(qFuzzyCompare(avalue, bvalue));
    else
        QVERIFY(!qFuzzyCompare(avalue, bvalue));
}

QTEST_APPLESS_MAIN(tst_QFixedPt)

#include "tst_qfixedpt.moc"
