#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QElapsedTimer>
#include <QtTest/QtTest>

static const int minResolution = 50; // the minimum resolution for the tests

class tst_QElapsedTimer : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void statics();
    void validity();
    void basics();
    void elapsed();
};

void tst_QElapsedTimer::statics()
{
    qDebug() << "Clock type is" << QElapsedTimer::clockType();
    qDebug() << "Said clock is" << (QElapsedTimer::isMonotonic() ? "monotonic" : "not monotonic");
    qDebug() << "Current time is" << QElapsedTimer::started().msecsSinceReference();
}

void tst_QElapsedTimer::validity()
{
    QElapsedTimer t;

    t.invalidate();
    QVERIFY(!t.isValid());

    t.start();
    QVERIFY(t.isValid());

    t.invalidate();
    QVERIFY(!t.isValid());

    t = QElapsedTimer::started();
    QVERIFY(t.isValid());

    t = QElapsedTimer::invalid();
    QVERIFY(!t.isValid());
}

void tst_QElapsedTimer::basics()
{
    QElapsedTimer t1;
    t1.start();

    QVERIFY(t1.msecsSinceReference() != 0);

    QCOMPARE(t1, t1);
    QVERIFY(!(t1 != t1));
    QVERIFY(!(t1 < t1));
    QCOMPARE(t1.msecsTo(t1), qint64(0));
    QCOMPARE(t1.secsTo(t1), qint64(0));
//    QCOMPARE(t1 + 0, t1);
//    QCOMPARE(t1 - 0, t1);

#if 0
    QElapsedTimer t2 = t1;
    t2 += 1000;   // so we can use secsTo

    QVERIFY(t1 != t2);
    QVERIFY(!(t1 == t2));
    QVERIFY(t1 < t2);
    QVERIFY(!(t2 < t1));
    QCOMPARE(t1.msecsTo(t2), qint64(1000));
    QCOMPARE(t1.secsTo(t2), qint64(1));
//    QCOMPARE(t2 - t1, qint64(1000));
//    QCOMPARE(t1 - t2, qint64(-1000));
#endif

    quint64 value1 = t1.msecsSinceReference();
    qint64 elapsed = t1.restart();
    quint64 value2 = t1.msecsSinceReference();
    QCOMPARE(elapsed, qint64(value2 - value1));
    QVERIFY(elapsed < minResolution);
}

void tst_QElapsedTimer::elapsed()
{
    QElapsedTimer t1;
    t1.start();

    QTest::qSleep(4*minResolution);
    QElapsedTimer t2;
    t2.start();

    QVERIFY(t1 != t2);
    QVERIFY(!(t1 == t2));
    QVERIFY(t1 < t2);
    QVERIFY(t1.msecsTo(t2) > 0);
    // don't check: t1.secsTo(t2)
//    QVERIFY(t1 - t2 < 0);

    QVERIFY(t1.elapsed() > 0);
    QVERIFY(t1.hasExpired(minResolution));
    QVERIFY(!t1.hasExpired(8*minResolution));
    QVERIFY(!t2.hasExpired(minResolution));

    QVERIFY(!t1.hasExpired(-1));
    QVERIFY(!t2.hasExpired(-1));

    qint64 elapsed = t1.restart();
    QVERIFY(elapsed > 3*minResolution);
    QVERIFY(elapsed < 5*minResolution);
    qint64 diff = t2.msecsTo(t1);
    QVERIFY(diff < minResolution);
}

QTEST_MAIN(tst_QElapsedTimer);

#include "tst_qelapsedtimer.moc"
