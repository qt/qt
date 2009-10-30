#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qmlview.h>
#include <qfxrepeater.h>
#include <qfxtext.h>
#include <qmlcontext.h>

class tst_QmlGraphicsRepeater : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsRepeater();

private slots:
    void stringList();

private:
    QmlView *createView(const QString &filename);
    template<typename T>
    T *findItem(QObject *parent, const QString &id);
};

tst_QmlGraphicsRepeater::tst_QmlGraphicsRepeater()
{
}

/*
The Repeater element creates children at its own position in its parent's
stacking order.  In this test we insert a repeater between two other Text
elements to test this.
*/
void tst_QmlGraphicsRepeater::stringList()
{
    QmlView *canvas = createView(SRCDIR "/data/repeater.qml");

    QStringList data;
    data << "One";
    data << "Two";
    data << "Three";
    data << "Four";

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testData", data);

    canvas->execute();
    qApp->processEvents();

    QmlGraphicsRepeater *repeater = findItem<QmlGraphicsRepeater>(canvas->root(), "repeater");
    QVERIFY(repeater != 0);

    QmlGraphicsItem *container = findItem<QmlGraphicsItem>(canvas->root(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->childItems().count(), data.count() + 3);

    bool saw_repeater = false;
    for (int i = 0; i < container->childItems().count(); ++i) {

        if (i == 0) {
            QmlGraphicsText *name = qobject_cast<QmlGraphicsText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Zero"));
        } else if (i == container->childItems().count() - 2) {
            // The repeater itself
            QmlGraphicsRepeater *rep = qobject_cast<QmlGraphicsRepeater*>(container->childItems().at(i));
            QCOMPARE(rep, repeater);
            saw_repeater = true;
            continue;
        } else if (i == container->childItems().count() - 1) {
            QmlGraphicsText *name = qobject_cast<QmlGraphicsText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Last"));
        } else {
            QmlGraphicsText *name = qobject_cast<QmlGraphicsText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), data.at(i-1));
        }
    }
    QVERIFY(saw_repeater);

    delete canvas;
}


QmlView *tst_QmlGraphicsRepeater::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString qml = file.readAll();
    canvas->setQml(qml, filename);

    return canvas;
}

template<typename T>
T *tst_QmlGraphicsRepeater::findItem(QObject *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    if (mo.cast(parent) && (objectName.isEmpty() || parent->objectName() == objectName))
        return static_cast<T*>(parent);
    for (int i = 0; i < parent->children().count(); ++i) {
        QmlGraphicsItem *item = findItem<T>(parent->children().at(i), objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_QmlGraphicsRepeater)

#include "tst_repeater.moc"
