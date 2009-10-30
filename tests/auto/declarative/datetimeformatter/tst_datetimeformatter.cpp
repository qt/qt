#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmldatetimeformatter_p.h>
#include <QDebug>

class tst_datetimeformatter : public QObject
{
    Q_OBJECT
public:
    tst_datetimeformatter() {}

private slots:
    void date();
    void time();
    void dateTime();
};

void tst_datetimeformatter::date()
{
    QmlEngine engine;
    QmlComponent formatterComponent(&engine, QByteArray("import Qt 4.6\n DateTimeFormatter { date: \"2008-12-24\" }"),
            QUrl("file://"));
    QmlDateTimeFormatter *formatter = qobject_cast<QmlDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QDate date(2008,12,24);
    QCOMPARE(formatter->dateText(),date.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QCOMPARE(formatter->dateText(),date.toString(Qt::SystemLocaleLongDate));

    formatter->setDateFormat("ddd MMMM d yy");
    QCOMPARE(formatter->dateText(),date.toString("ddd MMMM d yy"));

    QVERIFY(formatter->timeText().isEmpty());
    QVERIFY(formatter->dateTimeText().isEmpty());
}

void tst_datetimeformatter::time()
{
    QmlEngine engine;
    QmlComponent formatterComponent(&engine, "import Qt 4.6\n DateTimeFormatter { time: \"14:15:38.200\" }", QUrl("file://"));
    QmlDateTimeFormatter *formatter = qobject_cast<QmlDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QTime time(14,15,38,200);

    QCOMPARE(formatter->time(),time);

    QCOMPARE(formatter->timeText(),time.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QCOMPARE(formatter->timeText(),time.toString(Qt::SystemLocaleLongDate));

    formatter->setTimeFormat("H:m:s a");
    QCOMPARE(formatter->timeText(),time.toString("H:m:s a"));

    formatter->setTimeFormat("hh:mm:ss.zzz");
    QCOMPARE(formatter->timeText(),time.toString("hh:mm:ss.zzz"));

    QVERIFY(formatter->dateText().isEmpty());
    QVERIFY(formatter->dateTimeText().isEmpty());
}

void tst_datetimeformatter::dateTime()
{
    QmlEngine engine;
    QmlComponent formatterComponent(&engine, "import Qt 4.6\n DateTimeFormatter { dateTime: \"1978-03-04T09:13:54\" }", QUrl("file://"));
    QmlDateTimeFormatter *formatter = qobject_cast<QmlDateTimeFormatter*>(formatterComponent.create());
    if(formatterComponent.isError())
        qDebug() << formatterComponent.errors();
    QVERIFY(formatter != 0);

    QDateTime dateTime(QDate(1978,03,04),QTime(9,13,54));
    QCOMPARE(formatter->dateTimeText(),dateTime.toString(Qt::SystemLocaleShortDate));

    formatter->setLongStyle(true);
    QCOMPARE(formatter->dateTimeText(),dateTime.toString(Qt::SystemLocaleLongDate));

    formatter->setDateTimeFormat("M/d/yy H:m:s a");
    QCOMPARE(formatter->dateTimeText(),dateTime.toString("M/d/yy H:m:s a"));
}

QTEST_MAIN(tst_datetimeformatter)

#include "tst_datetimeformatter.moc"
