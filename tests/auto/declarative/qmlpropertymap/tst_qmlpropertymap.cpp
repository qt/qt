#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlpropertymap.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlgraphicstext.h>
#include <QSignalSpy>

class tst_QmlPropertyMap : public QObject
{
    Q_OBJECT
public:
    tst_QmlPropertyMap() {}

private slots:
    void insert();
    void operatorInsert();
    void clear();
    void changed();
    void count();
};

void tst_QmlPropertyMap::insert()
{
    QmlPropertyMap map;
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QVERIFY(map.keys().count() == 2);

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));
    QCOMPARE(map.value(QLatin1String("key2")), QVariant(200));

    map.insert(QLatin1String("key1"),"Hello World");
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));
}

void tst_QmlPropertyMap::operatorInsert()
{
    QmlPropertyMap map;
    map[QLatin1String("key1")] = 100;
    map[QLatin1String("key2")] = 200;
    QVERIFY(map.keys().count() == 2);

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));
    QCOMPARE(map.value(QLatin1String("key2")), QVariant(200));

    map[QLatin1String("key1")] = "Hello World";
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));
}

void tst_QmlPropertyMap::clear()
{
    QmlPropertyMap map;
    map.insert(QLatin1String("key1"),100);
    QVERIFY(map.keys().count() == 1);

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));

    map.clear(QLatin1String("key1"));
    QVERIFY(map.keys().count() == 1);
    QCOMPARE(map.value(QLatin1String("key1")), QVariant());
}

void tst_QmlPropertyMap::changed()
{
    QmlPropertyMap map;
    QSignalSpy spy(&map, SIGNAL(valueChanged(const QString&)));
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QCOMPARE(spy.count(), 0);

    map.clear(QLatin1String("key1"));
    QCOMPARE(spy.count(), 0);

    //make changes in QML
    QmlEngine engine;
    QmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty(QLatin1String("testdata"), &map);
    QmlComponent component(&engine, "import Qt 4.6\nText { text: { testdata.key1 = 'Hello World'; 'X' } }",
            QUrl("file://"));
    QVERIFY(component.isReady());
    QmlGraphicsText *txt = qobject_cast<QmlGraphicsText*>(component.create());
    QVERIFY(txt);
    QCOMPARE(txt->text(), QString('X'));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(),QLatin1String("key1"));
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));
}

void tst_QmlPropertyMap::count()
{
    QmlPropertyMap map;
    QCOMPARE(map.isEmpty(), true);
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QCOMPARE(map.count(), 2);
    QCOMPARE(map.isEmpty(), false);

    map.insert(QLatin1String("key3"),"Hello World");
    QCOMPARE(map.count(), 3);

    //clearing doesn't remove the key
    map.clear(QLatin1String("key3"));
    QCOMPARE(map.count(), 3);
}

QTEST_MAIN(tst_QmlPropertyMap)

#include "tst_qmlpropertymap.moc"
