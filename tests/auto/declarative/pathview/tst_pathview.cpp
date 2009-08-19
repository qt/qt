#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qfxview.h>
#include <qfxpathview.h>
#include <qfxtext.h>
#include <qfxrect.h>
#include <qmlcontext.h>
#include <qmlexpression.h>

class tst_QFxPathView : public QObject
{
    Q_OBJECT
public:
    tst_QFxPathView();

private slots:
    void items();
    void pathMoved();
    void limitedItems();

private:
    QFxView *createView(const QString &filename);
    template<typename T>
    T *findItem(QFxItem *parent, const QString &id, int index=0);
};

class TestModel : public QListModelInterface
{
    Q_OBJECT
public:
    TestModel(QObject *parent = 0) : QListModelInterface(parent) {}
    ~TestModel() {}

    enum Roles { Name, Number };

    QString name(int index) const { return list.at(index).first; }
    QString number(int index) const { return list.at(index).second; }

    int count() const { return list.count(); }

    QList<int> roles() const { return QList<int>() << Name << Number; }
    QString toString(int role) const {
        switch(role) {
        case Name:
            return "name";
        case Number:
            return "number";
        default:
            return "";
        }
    }

    QHash<int, QVariant> data(int index, const QList<int> &roles) const {
        QHash<int,QVariant> returnHash;

        for (int i = 0; i < roles.size(); ++i) {
            int role = roles.at(i);
            QVariant info;
            switch(role) {
            case Name:
                info = list.at(index).first;
                break;
            case Number:
                info = list.at(index).second;
                break;
            default:
                break;
            }
            returnHash.insert(role, info);
        }
        return returnHash;
    }

    void addItem(const QString &name, const QString &number) {
        list.append(QPair<QString,QString>(name, number));
        emit itemsInserted(list.count()-1, 1);
    }

    void insertItem(int index, const QString &name, const QString &number) {
        list.insert(index, QPair<QString,QString>(name, number));
        emit itemsInserted(index, 1);
    }

    void removeItem(int index) {
        list.removeAt(index);
        emit itemsRemoved(index, 1);
    }

    void modifyItem(int index, const QString &name, const QString &number) {
        list[index] = QPair<QString,QString>(name, number);
        emit itemsChanged(index, 1, roles());
    }

private:
    QList<QPair<QString,QString> > list;
};

tst_QFxPathView::tst_QFxPathView()
{
}

void tst_QFxPathView::items()
{
    QFxView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxPathView *pathview = findItem<QFxPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    QCOMPARE(pathview->children()->count(), model.count()); // assumes all are visible

    for (int i = 0; i < model.count(); ++i) {
        QFxText *name = findItem<QFxText>(pathview, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QFxText *number = findItem<QFxText>(pathview, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QFxPathView::pathMoved()
{
    QFxView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    model.addItem("Ben", "12345");
    model.addItem("Bohn", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Bill", "4321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxPathView *pathview = findItem<QFxPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    QFxRect *firstItem = findItem<QFxRect>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QFxPath *path = qobject_cast<QFxPath*>(pathview->path());
    QVERIFY(path);
    QPointF start = path->pointAt(0.0);
    QPointF offset;//Center of item is at point, but pos is from corner
    offset.setX(firstItem->width()/2);
    offset.setY(firstItem->height()/2);
    QCOMPARE(firstItem->pos() + offset, start);
    pathview->setOffset(10);
    QTest::qWait(1000);//Moving is animated?

    for(int i=0; i<model.count(); i++){
        QFxRect *curItem = findItem<QFxRect>(pathview, "wrapper", i);
        QCOMPARE(curItem->pos() + offset, path->pointAt(0.1 + i*0.25));
    }

    pathview->setOffset(100);
    QTest::qWait(1000);//Moving is animated?
    QCOMPARE(firstItem->pos() + offset, start);

    delete canvas;
}

void tst_QFxPathView::limitedItems()
{
    QFxView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    for(int i=0; i<100; i++)
        model.addItem("Bob", QString::number(i));

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxPathView *pathview = findItem<QFxPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    pathview->setPathItemCount(10);
    QCOMPARE(pathview->pathItemCount(), 10);

    QFxRect *testItem = findItem<QFxRect>(pathview, "wrapper", 0);
    QVERIFY(testItem != 0);
    testItem = findItem<QFxRect>(pathview, "wrapper", 9);
    QVERIFY(testItem != 0);
    testItem = findItem<QFxRect>(pathview, "wrapper", 10);
    QVERIFY(testItem == 0);

    pathview->setCurrentIndex(50);
    QTest::qWait(5100);//Moving is animated and it's travelling far - should be reconsidered.
    testItem = findItem<QFxRect>(pathview, "wrapper", 0);
    QVERIFY(testItem == 0);
    testItem = findItem<QFxRect>(pathview, "wrapper", 1);
    QVERIFY(testItem == 0);
    testItem = findItem<QFxRect>(pathview, "wrapper", 9);
    QVERIFY(testItem == 0);
    testItem = findItem<QFxRect>(pathview, "wrapper", 50);
    QVERIFY(testItem != 0);
}

QFxView *tst_QFxPathView::createView(const QString &filename)
{
    QFxView *canvas = new QFxView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString qml = file.readAll();
    canvas->setQml(qml, filename);

    return canvas;
}

/*
   Find an item with the specified objectName.  If index is supplied then the
   item must also evaluate the {index} expression equal to index
*/
template<typename T>
T *tst_QFxPathView::findItem(QFxItem *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    for (int i = 0; i < parent->children()->count(); ++i) {
        QFxItem *item = parent->children()->at(i);
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            if (index != -1) {
                QmlExpression e(qmlContext(item), "index", item);
                e.setTrackChange(false);
                if (e.value().toInt() == index)
                    return static_cast<T*>(item);
            } else {
                return static_cast<T*>(item);
            }
        }
        item = findItem<T>(item, objectName, index);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_QFxPathView)

#include "tst_pathview.moc"
