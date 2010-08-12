#include <QtCore>
#include <QtTest/QtTest>

class tst_Random: public QObject
{
    Q_OBJECT

private slots:
    void testOne() const {}
    void testTwo() const {}
    void testThree() const {}
};

/*
int main(int argc, char * argv[])
{
    int failures = 0;
    tst_Random tst;
    for (int i = 0; i != 10; i++) {
        failures += QTest::qExec(&tst, argc, argv);
    }
    return failures;
}
*/
QTEST_MAIN(tst_Random)

#include "tst_random.moc"

