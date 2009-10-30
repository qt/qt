#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmltimer_p.h>

class tst_qmltimer : public QObject
{
    Q_OBJECT
public:
    tst_qmltimer();

private slots:
    void notRepeating();
    void notRepeatingStart();
    void repeat();
    void triggeredOnStart();
    void triggeredOnStartRepeat();
};

class TimerHelper : public QObject
{
    Q_OBJECT
public:
    TimerHelper() : QObject(), count(0)
    {
    }

    int count;

public slots:
    void timeout() {
        ++count;
    }
};

#if defined(Q_OS_SYMBIAN) && defined(Q_CC_NOKIAX86)
// Increase wait as emulator startup can cause unexpected delays
#define TIMEOUT_TIMEOUT 2000
#else
#define TIMEOUT_TIMEOUT 200
#endif

tst_qmltimer::tst_qmltimer()
{
}

void tst_qmltimer::notRepeating()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
}

void tst_qmltimer::notRepeatingStart()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100 }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 0);

    timer->start();
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 1);
}

void tst_qmltimer::repeat()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; repeat: true; running: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QCOMPARE(helper.count, 0);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > 0);
    int oldCount = helper.count;

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > oldCount);
}

void tst_qmltimer::triggeredOnStart()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QTest::qWait(1);
    QCOMPARE(helper.count, 1);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
    QTest::qWait(TIMEOUT_TIMEOUT);
    QCOMPARE(helper.count, 2);
}

void tst_qmltimer::triggeredOnStartRepeat()
{
    QmlEngine engine;
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nTimer { interval: 100; running: true; triggeredOnStart: true; repeat: true }"), QUrl("file://"));
    QmlTimer *timer = qobject_cast<QmlTimer*>(component.create());
    QVERIFY(timer != 0);

    TimerHelper helper;
    connect(timer, SIGNAL(triggered()), &helper, SLOT(timeout()));
    QTest::qWait(1);
    QCOMPARE(helper.count, 1);

    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > 1);
    int oldCount = helper.count;
    QTest::qWait(TIMEOUT_TIMEOUT);
    QVERIFY(helper.count > oldCount);
}

QTEST_MAIN(tst_qmltimer)

#include "tst_qmltimer.moc"
