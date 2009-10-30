#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlgraphicsloader.h>

class tst_qfxloader : public QObject

{
    Q_OBJECT
public:
    tst_qfxloader();

private slots:
    void url();
    void component();
    void sizeLoaderToItem();
    void sizeItemToLoader();
    void noResize();

private:
    QmlEngine engine;
};

/*
inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath(filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}
*/

tst_qfxloader::tst_qfxloader()
{
}

void tst_qfxloader::url()
{
    QmlComponent component(&engine, QByteArray("import Qt 4.6\nLoader { source: \"Rect120x60.qml\" }"), QUrl("file://" SRCDIR "/"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
}

void tst_qfxloader::component()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SetSourceComponent.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(component.create());
    QVERIFY(item);

    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(item->QGraphicsObject::children().at(1)); 
    QVERIFY(loader);
    QVERIFY(loader->item());
    QCOMPARE(loader->progress(), 1.0);
    QCOMPARE(static_cast<QGraphicsItem*>(loader)->children().count(), 1);
}

void tst_qfxloader::sizeLoaderToItem()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToItem.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 120.0);
    QCOMPARE(loader->height(), 60.0);
}

void tst_qfxloader::sizeItemToLoader()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/SizeToLoader.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 200.0);
    QCOMPARE(rect->height(), 80.0);
}

void tst_qfxloader::noResize()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/NoResize.qml"));
    QmlGraphicsLoader *loader = qobject_cast<QmlGraphicsLoader*>(component.create());
    QVERIFY(loader != 0);
    QCOMPARE(loader->width(), 200.0);
    QCOMPARE(loader->height(), 80.0);

    QmlGraphicsItem *rect = loader->item();
    QVERIFY(rect);
    QCOMPARE(rect->width(), 120.0);
    QCOMPARE(rect->height(), 60.0);
}

QTEST_MAIN(tst_qfxloader)

#include "tst_qfxloader.moc"
