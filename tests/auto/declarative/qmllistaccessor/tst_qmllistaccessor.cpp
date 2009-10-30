#include <qtest.h>
#include <QtDeclarative/qml.h>
#include <private/qmllistaccessor_p.h>

class tst_QmlListAccessor : public QObject
{
    Q_OBJECT
public:
    tst_QmlListAccessor() {}

private slots:
    void qmllist();
    //void qlist();
    //void qstringlist();
};

void tst_QmlListAccessor::qmllist()
{
    QmlConcreteList<QObject*> list;
    QObject *obj = new QObject;
    list.append(obj);
    QVERIFY(list.count() == 1);
    QCOMPARE(list.at(0), obj);

    QmlListAccessor accessor;
    accessor.setList(qVariantFromValue((QmlList<QObject*>*)&list));

    QVERIFY(accessor.isValid());
    QVERIFY(accessor.count() == 1);

    QVariant v = accessor.at(0);
    QCOMPARE(qvariant_cast<QObject*>(v), obj);

    accessor.removeAt(3);
    QVERIFY(accessor.count() == 1);

    accessor.removeAt(0);
    QVERIFY(accessor.count() == 0);

    accessor.insert(4, qVariantFromValue(obj));
    QVERIFY(accessor.count() == 1);

    v = accessor.at(0);
    QCOMPARE(qvariant_cast<QObject*>(v), obj);

    QObject *obj2 = new QObject;
    accessor.append(qVariantFromValue(obj2));
    QVERIFY(accessor.count() == 2);

    v = accessor.at(1);
    QCOMPARE(qvariant_cast<QObject*>(v), obj2);

    accessor.clear();
    QVERIFY(accessor.count() == 0);

    QVERIFY(accessor.isValid());
}

QTEST_MAIN(tst_QmlListAccessor)

#include "tst_qmllistaccessor.moc"
