#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qfxview.h>
#include <qfxlistview.h>
#include <qfxtext.h>
#include <qmlcontext.h>
#include <qmlexpression.h>

class tst_QFxListView : public QObject
{
    Q_OBJECT
public:
    tst_QFxListView();

private slots:
    // Test both QListModelInterface and QAbstractItemModel model types
    void qListModelInterface_items();
    void qAbstractItemModel_items();

    void qListModelInterface_changed();
    void qAbstractItemModel_changed();

    void qListModelInterface_inserted();
    void qAbstractItemModel_inserted();

    void qListModelInterface_removed();
    void qAbstractItemModel_removed();

private:
    template <class T> void items();
    template <class T> void changed();
    template <class T> void inserted();
    template <class T> void removed();
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


class TestModel2 : public QAbstractListModel
{
public:
    enum Roles { Name = Qt::UserRole+1, Number = Qt::UserRole+2 };

    TestModel2(QObject *parent=0) : QAbstractListModel(parent) {
        QHash<int, QByteArray> roles;
        roles[Name] = "name";
        roles[Number] = "number";
        setRoleNames(roles);
    }

    int rowCount(const QModelIndex &parent=QModelIndex()) const { return list.count(); }
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const {
        QVariant rv;
        if (role == Name)
            rv = list.at(index.row()).first;
        else if (role == Number)
            rv = list.at(index.row()).second;

        return rv;
    }

    int count() const { return rowCount(); }
    QString name(int index) const { return list.at(index).first; }
    QString number(int index) const { return list.at(index).second; }

    void addItem(const QString &name, const QString &number) {
        emit beginInsertRows(QModelIndex(), list.count(), list.count());
        list.append(QPair<QString,QString>(name, number));
        emit endInsertRows();
    }

    void insertItem(int index, const QString &name, const QString &number) {
        emit beginInsertRows(QModelIndex(), index, index);
        list.insert(index, QPair<QString,QString>(name, number));
        emit endInsertRows();
    }

    void removeItem(int index) {
        emit beginRemoveRows(QModelIndex(), index, index);
        list.removeAt(index);
        emit endRemoveRows();
    }

    void modifyItem(int idx, const QString &name, const QString &number) {
        list[idx] = QPair<QString,QString>(name, number);
        emit dataChanged(index(idx,0), index(idx,0));
    }

private:
    QList<QPair<QString,QString> > list;
};

tst_QFxListView::tst_QFxListView()
{
}

template <class T>
void tst_QFxListView::items()
{
    QFxView *canvas = createView(SRCDIR "/data/listview.qml");

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxFlickable *listview = findItem<QFxFlickable>(canvas->root(), "list");
    QVERIFY(listview != 0);

    QFxItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    QCOMPARE(viewport->children()->count(), model.count()); // assumes all are visible

    for (int i = 0; i < model.count(); ++i) {
        QFxText *name = findItem<QFxText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QFxText *number = findItem<QFxText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

template <class T>
void tst_QFxListView::changed()
{
    QFxView *canvas = createView(SRCDIR "/data/listview.qml");

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxFlickable *listview = findItem<QFxFlickable>(canvas->root(), "list");
    QVERIFY(listview != 0);

    QFxItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.modifyItem(1, "Will", "9876");
    QFxText *name = findItem<QFxText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QFxText *number = findItem<QFxText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    delete canvas;
}

template <class T>
void tst_QFxListView::inserted()
{
    QFxView *canvas = createView(SRCDIR "/data/listview.qml");

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxListView *listview = findItem<QFxListView>(canvas->root(), "list");
    QVERIFY(listview != 0);

    QFxItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.insertItem(1, "Will", "9876");

    // let transitions settle.
    QTest::qWait(1000);

    QCOMPARE(viewport->children()->count(), model.count()); // assumes all are visible

    QFxText *name = findItem<QFxText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QFxText *number = findItem<QFxText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QVERIFY(item->y() == i*20);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    // let transitions settle.
    QTest::qWait(1000);

    QCOMPARE(viewport->children()->count(), model.count()); // assumes all are visible

    name = findItem<QFxText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QFxText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    QCOMPARE(listview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QVERIFY(item->y() == i*20);
    }

    delete canvas;
}

template <class T>
void tst_QFxListView::removed()
{
    QFxView *canvas = createView(SRCDIR "/data/listview.qml");

    T model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QFxListView *listview = findItem<QFxListView>(canvas->root(), "list");
    QVERIFY(listview != 0);

    QFxItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(1000);

    QFxText *name = findItem<QFxText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QFxText *number = findItem<QFxText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < viewport->children()->count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QVERIFY(item->y() == i*20);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);  // post: top item starts at 20

    // let transitions settle.
    QTest::qWait(1000);

    name = findItem<QFxText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QFxText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < viewport->children()->count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(),i*20.0 + 20.0);
    }

    // Remove items not visible
    model.removeItem(18);
    // let transitions settle.
    QTest::qWait(1000);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < viewport->children()->count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(),i*20.0+20.0);
    }

    // Remove items before visible
    listview->setYPosition(80);
    listview->setCurrentIndex(10);

    model.removeItem(1); // post: top item will be at 40
    // let transitions settle.
    QTest::qWait(1000);

    // Confirm items positioned correctly
    for (int i = 2; i < 18; ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(),40+i*20.0);
    }

    listview->setYPosition(40); // That's the top now
    // let transitions settle.
    QTest::qWait(1000);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < viewport->children()->count(); ++i) {
        QFxItem *item = findItem<QFxItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(),40+i*20.0);
    }

    delete canvas;
}

void tst_QFxListView::qListModelInterface_items()
{
    items<TestModel>();
}

void tst_QFxListView::qAbstractItemModel_items()
{
    items<TestModel2>();
}

void tst_QFxListView::qListModelInterface_changed()
{
    changed<TestModel>();
}

void tst_QFxListView::qAbstractItemModel_changed()
{
    changed<TestModel2>();
}

void tst_QFxListView::qListModelInterface_inserted()
{
    inserted<TestModel>();
}

void tst_QFxListView::qAbstractItemModel_inserted()
{
    inserted<TestModel2>();
}

void tst_QFxListView::qListModelInterface_removed()
{
    removed<TestModel>();
}

void tst_QFxListView::qAbstractItemModel_removed()
{
    removed<TestModel2>();
}

QFxView *tst_QFxListView::createView(const QString &filename)
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
   Find an item with the specified id.  If index is supplied then the
   item must also evaluate the {index} expression equal to index
*/
template<typename T>
T *tst_QFxListView::findItem(QFxItem *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    qDebug() << parent->children()->count() << "children";
    for (int i = 0; i < parent->children()->count(); ++i) {
        QFxItem *item = parent->children()->at(i);
        qDebug() << "try" << item;
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

QTEST_MAIN(tst_QFxListView)

#include "tst_listview.moc"
