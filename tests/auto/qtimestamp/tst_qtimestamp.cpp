#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QTimestamp>
#include <QtTest/QtTest>

class tst_QTimestamp : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void basics();
    void elapsed();
};

void tst_QTimestamp::basics()
{
    QTimestamp t1;
    t1.start();

    QCOMPARE(t1, t1);
    QVERIFY(!(t1 != t1));
    QVERIFY(!(t1 < t1));
    QCOMPARE(t1.msecsTo(t1), qint64(0));
    QCOMPARE(t1.secsTo(t1), qint64(0));
    QCOMPARE(t1 + 0, t1);
    QCOMPARE(t1 - 0, t1);

    QTimestamp t2 = t1;
    t2 += 1000;   // so we can use secsTo

    QVERIFY(t1 != t2);
    QVERIFY(!(t1 == t2));
    QVERIFY(t1 < t2);
    QVERIFY(!(t2 < t1));
    QCOMPARE(t1.msecsTo(t2), qint64(1000));
    QCOMPARE(t1.secsTo(t2), qint64(1));
    QCOMPARE(t2 - t1, qint64(1000));
    QCOMPARE(t1 - t2, qint64(-1000));
}

void tst_QTimestamp::elapsed()
{
    QTimestamp t1;
    t1.start();

    QTest::qWait(100);
    QTimestamp t2;
    t2.start();

    QVERIFY(t1 != t2);
    QVERIFY(!(t1 == t2));
    QVERIFY(t1 < t2);
    QVERIFY(t1.msecsTo(t2) > 0);
    // don't check: t1.secsTo(t2)
    QVERIFY(t1 - t2 < 0);

    QVERIFY(t1.elapsed() > 0);
    QVERIFY(t1.hasExpired(50));
    QVERIFY(!t1.hasExpired(500));
    QVERIFY(!t2.hasExpired(0));

    QVERIFY(!t1.hasExpired(-1));
    QVERIFY(!t2.hasExpired(-1));
}

QTEST_MAIN(tst_QTimestamp);

#include "tst_qtimestamp.moc"
