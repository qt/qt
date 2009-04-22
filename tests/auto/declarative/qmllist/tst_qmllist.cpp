#include <qtest.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlprivate.h>

class tst_QmlList : public QObject
{
    Q_OBJECT
public:
    tst_QmlList() {}

private slots:
    void interface();
};

void tst_QmlList::interface()
{
    QmlConcreteList<QObject*> list;
    QObject *obj = new QObject;
    obj->setObjectName("foo");
    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlPrivate::ListInterface *li = (QmlPrivate::ListInterface*)&list;

    void *ptr[1];
    li->at(0, ptr);
    QVERIFY(li->count() == 1);
    QCOMPARE(ptr[0], obj);

    li->removeAt(0);
    QVERIFY(li->count() == 0);
    QVERIFY(list.count() == 0);
}

QTEST_MAIN(tst_QmlList)

#include "tst_qmllist.moc"
