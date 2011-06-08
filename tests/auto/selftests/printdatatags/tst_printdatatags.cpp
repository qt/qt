#include <QtTest/QtTest>

class tst_Foo: public QObject
{
    Q_OBJECT
private slots:
    void a_data() const;
    void a() const;

    void b() const;

    void c_data() const;
    void c() const;
};

void tst_Foo::a_data() const
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");

    QTest::newRow("data tag a1 ") << 1 << 2;
    QTest::newRow("data tag a2") << 1 << 2;
}

void tst_Foo::a() const
{
}

void tst_Foo::b() const
{
}

void tst_Foo::c_data() const
{
    QTest::addColumn<int>("x");

    QTest::newRow("data tag c1") << 1;
    QTest::newRow("data tag c2") << 1;
    QTest::newRow("data tag c3") << 1;
}

void tst_Foo::c() const
{
}

QTEST_MAIN(tst_Foo)

#include "tst_printdatatags.moc"
