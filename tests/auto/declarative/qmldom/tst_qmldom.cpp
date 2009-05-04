#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmldom.h>

#include <QtCore/QDebug>

class tst_qmldom : public QObject
{
    Q_OBJECT
public:
    tst_qmldom() {}

private slots:
    void loadSimple();
    void loadProperties();
    void loadChildObject();

private:
    QmlEngine engine;
};


void tst_qmldom::loadSimple()
{
    QByteArray qml = "Item {}";
    //QByteArray qml = "<Item/>";

    QmlDomDocument document;
    QVERIFY(document.load(&engine, qml));
    QVERIFY(document.loadError().isEmpty());

    QmlDomObject rootObject = document.rootObject();
    QVERIFY(rootObject.isValid());
    QVERIFY(!rootObject.isComponent());
    QVERIFY(!rootObject.isCustomType());
    QVERIFY(rootObject.objectType() == "Item");
}

void tst_qmldom::loadProperties()
{
    QByteArray qml = "Item { id : item; x : 300; visible : true }";
    //QByteArray qml = "<Item id='item' x='300' visible='true'/>";

    QmlDomDocument document;
    QVERIFY(document.load(&engine, qml));

    QmlDomObject rootObject = document.rootObject();
    QVERIFY(rootObject.isValid());
    QVERIFY(rootObject.objectId() == "item");
    QVERIFY(rootObject.properties().size() == 2);

    QmlDomProperty xProperty = rootObject.property("x");
    QVERIFY(xProperty.propertyName() == "x");
    QVERIFY(xProperty.value().isLiteral());
    QVERIFY(xProperty.value().toLiteral().literal() == "300");

    QmlDomProperty visibleProperty = rootObject.property("visible");
    QVERIFY(visibleProperty.propertyName() == "visible");
    QVERIFY(visibleProperty.value().isLiteral());
    QVERIFY(visibleProperty.value().toLiteral().literal() == "true");
}

void tst_qmldom::loadChildObject()
{
    QByteArray qml = "Item { Item {} }";
    //QByteArray qml = "<Item> <Item/> </Item>";

    QmlDomDocument document;
    QVERIFY(document.load(&engine, qml));

    QmlDomObject rootItem = document.rootObject();
    QVERIFY(rootItem.isValid());
    QVERIFY(rootItem.properties().size() == 1);

    QmlDomProperty listProperty = rootItem.properties().at(0);
    QVERIFY(listProperty.isDefaultProperty());
    QVERIFY(listProperty.value().isList());

    QmlDomList list = listProperty.value().toList();
    QVERIFY(list.values().size() == 1);

    QmlDomObject childItem = list.values().first().toObject();
    QVERIFY(childItem.isValid());
    QVERIFY(childItem.objectType() == "Item");
}

QTEST_MAIN(tst_qmldom)

#include "tst_qmldom.moc"
