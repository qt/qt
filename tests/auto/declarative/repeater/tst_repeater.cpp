#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qfxview.h>
#include <qfxrepeater.h>
#include <qfxtext.h>
#include <qmlcontext.h>

class tst_QFxRepeater : public QObject
{
    Q_OBJECT
public:
    tst_QFxRepeater();

private slots:
    void stringList();

private:
    QFxView *createView(const QString &filename);
    template<typename T>
    T *findItem(QFxItem *parent, const QString &id);
};

tst_QFxRepeater::tst_QFxRepeater()
{
}

void tst_QFxRepeater::stringList()
{
    QFxView *canvas = createView(SRCDIR "/data/repeater.xml");

    QStringList data;
    data << "One";
    data << "Two";
    data << "Three";
    data << "Four";

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setProperty("testData", data);

    canvas->execute();
    qApp->processEvents();

    QFxRepeater *repeater = findItem<QFxRepeater>(canvas->root(), "repeater");
    QVERIFY(repeater != 0);

    QFxItem *container = findItem<QFxItem>(canvas->root(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->children()->count(), data.count() + 1);

    for (int i = 1; i < container->children()->count(); ++i) {
        QFxText *name = qobject_cast<QFxText*>(container->children()->at(i));
        QVERIFY(name != 0);
        QCOMPARE(name->text(), data.at(i-1));
    }

    delete canvas;
}


QFxView *tst_QFxRepeater::createView(const QString &filename)
{
    QFxView *canvas = new QFxView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}

template<typename T>
T *tst_QFxRepeater::findItem(QFxItem *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    if (mo.cast(parent) && (objectName.isEmpty() || parent->objectName() == objectName))
        return static_cast<T*>(parent);
    for (int i = 0; i < parent->children()->count(); ++i) {
        QFxItem *item = findItem<T>(parent->children()->at(i), objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_QFxRepeater)

#include "tst_repeater.moc"
