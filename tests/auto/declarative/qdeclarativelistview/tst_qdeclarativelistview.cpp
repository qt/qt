/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtGui/QStringListModel>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/private/qdeclarativelistview_p.h>
#include <QtDeclarative/private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativevisualitemmodel_p.h>
#include <QtDeclarative/private/qdeclarativelistmodel_p.h>
#include <QtDeclarative/private/qlistmodelinterface_p.h>

class tst_QDeclarativeListView : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeListView();

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

    void qListModelInterface_moved();
    void qAbstractItemModel_moved();

    void qListModelInterface_clear();
    void qAbstractItemModel_clear();

    void itemList();
    void currentIndex();
    void enforceRange();
    void spacing();
    void sections();
    void cacheBuffer();
    void positionViewAtIndex();
    void resetModel();
    void propertyChanges();
    void componentChanges();
    void modelChanges();

private:
    template <class T> void items();
    template <class T> void changed();
    template <class T> void inserted();
    template <class T> void removed(bool animated);
    template <class T> void moved();
    template <class T> void clear();
    QDeclarativeView *createView();
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id, int index=-1);
    template<typename T>
    QList<T*> findItems(QGraphicsObject *parent, const QString &objectName);
    void dumpTree(QDeclarativeItem *parent, int depth = 0);
};

class TestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool error READ error WRITE setError NOTIFY changedError)
    Q_PROPERTY(bool animate READ animate NOTIFY changedAnim)
    Q_PROPERTY(bool invalidHighlight READ invalidHighlight NOTIFY changedHl)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer NOTIFY changedCacheBuffer)

public:
    TestObject(QObject *parent = 0)
        : QObject(parent), mError(true), mAnimate(false), mInvalidHighlight(false)
        , mCacheBuffer(0) {}

    bool error() const { return mError; }
    void setError(bool err) { mError = err; emit changedError(); }

    bool animate() const { return mAnimate; }
    void setAnimate(bool anim) { mAnimate = anim; emit changedAnim(); }

    bool invalidHighlight() const { return mInvalidHighlight; }
    void setInvalidHighlight(bool invalid) { mInvalidHighlight = invalid; emit changedHl(); }

    int cacheBuffer() const { return mCacheBuffer; }
    void setCacheBuffer(int buffer) { mCacheBuffer = buffer; emit changedCacheBuffer(); }

signals:
    void changedError();
    void changedAnim();
    void changedHl();
    void changedCacheBuffer();

public:
    bool mError;
    bool mAnimate;
    bool mInvalidHighlight;
    int mCacheBuffer;
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

    QVariant data(int index, int role) const
    {
        if (role==0)
            return list.at(index).first;
        if (role==1)
            return list.at(index).second;
        return QVariant();
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

    void moveItem(int from, int to) {
        list.move(from, to);
        emit itemsMoved(from, to, 1);
    }

    void modifyItem(int index, const QString &name, const QString &number) {
        list[index] = QPair<QString,QString>(name, number);
        emit itemsChanged(index, 1, roles());
    }

    void clear() {
        int count = list.count();
        list.clear();
        emit itemsRemoved(0, count);
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

    int rowCount(const QModelIndex &parent=QModelIndex()) const { Q_UNUSED(parent); return list.count(); }
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

    void moveItem(int from, int to) {
        emit beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        list.move(from, to);
        emit endMoveRows();
    }

    void modifyItem(int idx, const QString &name, const QString &number) {
        list[idx] = QPair<QString,QString>(name, number);
        emit dataChanged(index(idx,0), index(idx,0));
    }

    void clear() {
        int count = list.count();
        emit beginRemoveRows(QModelIndex(), 0, count-1);
        list.clear();
        emit endRemoveRows();
    }

private:
    QList<QPair<QString,QString> > list;
};

tst_QDeclarativeListView::tst_QDeclarativeListView()
{
}

template <class T>
void tst_QDeclarativeListView::items()
{
    QDeclarativeView *canvas = createView();

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);

    QVERIFY(listview->highlightItem() != 0);
    QCOMPARE(listview->count(), model.count());
    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    // current item should be first item
    QCOMPARE(listview->currentItem(), findItem<QDeclarativeItem>(viewport, "wrapper", 0));

    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // switch to other delegate
    testObject->setAnimate(true);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);
    QVERIFY(listview->currentItem());

    // set invalid highlight
    testObject->setInvalidHighlight(true);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);
    QVERIFY(listview->currentItem());
    QVERIFY(listview->highlightItem() == 0);

    // back to normal highlight
    testObject->setInvalidHighlight(false);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);
    QVERIFY(listview->currentItem());
    QVERIFY(listview->highlightItem() != 0);

    // set an empty model and confirm that items are destroyed
    T model2;
    ctxt->setContextProperty("testModel", &model2);

    // Allow deleteLaters to process
    QTest::qWait(500);

    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    QVERIFY(itemCount == 0);

    QCOMPARE(listview->highlightResizeSpeed(), 1000.0);
    QCOMPARE(listview->highlightMoveSpeed(), 1000.0);

    delete canvas;
}


template <class T>
void tst_QDeclarativeListView::changed()
{
    QDeclarativeView *canvas = createView();

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeFlickable *listview = findItem<QDeclarativeFlickable>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.modifyItem(1, "Will", "9876");
    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    delete canvas;
}

template <class T>
void tst_QDeclarativeListView::inserted()
{
    QDeclarativeView *canvas = createView();

    T model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.insertItem(1, "Will", "9876");

    // let transitions settle.
    QTest::qWait(300);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(), i*20.0);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    // let transitions settle.
    QTest::qWait(300);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    name = findItem<QDeclarativeText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    QCOMPARE(listview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        QCOMPARE(item->y(), i*20.0);
    }

    for (int i = model.count(); i < 30; ++i)
        model.insertItem(i, "Hello", QString::number(i));
    QTest::qWait(300);

    listview->setContentY(80);
    QTest::qWait(300);

    // Insert item outside visible area
    model.insertItem(1, "Hello", "1324");
    QTest::qWait(300);

    QVERIFY(listview->contentY() == 80);

    // Confirm items positioned correctly
    for (int i = 5; i < 5+15; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.0 - 20.0);
    }

//    QCOMPARE(listview->viewportHeight(), model.count() * 20.0);

    delete canvas;
}

template <class T>
void tst_QDeclarativeListView::removed(bool animated)
{
    QDeclarativeView *canvas = createView();

    T model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    testObject->setAnimate(animated);
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(300);

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*20);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);  // post: top item starts at 20

    // let transitions settle.
    QTest::qWait(300);

    name = findItem<QDeclarativeText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(),i*20.0 + 20.0);
    }

    // Remove items not visible
    model.removeItem(18);
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(),i*20.0+20.0);
    }

    // Remove items before visible
    listview->setContentY(80);
    listview->setCurrentIndex(10);

    model.removeItem(1); // post: top item will be at 40
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    for (int i = 2; i < 18; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(),40+i*20.0);
    }

    // Remove current index
    QVERIFY(listview->currentIndex() == 9);
    QDeclarativeItem *oldCurrent = listview->currentItem();
    model.removeItem(9);
    QTest::qWait(300);

    QCOMPARE(listview->currentIndex(), 9);
    QVERIFY(listview->currentItem() != oldCurrent);

    listview->setContentY(40); // That's the top now
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(),40+i*20.0);
    }

    // remove current item beyond visible items.
    listview->setCurrentIndex(20);
    QTest::qWait(300);
    listview->setContentY(40);
    model.removeItem(20);
    QTest::qWait(300);

    QCOMPARE(listview->currentIndex(), 20);
    QVERIFY(listview->currentItem() != 0);

    // remove item before current, but visible
    listview->setCurrentIndex(8);
    QTest::qWait(300);
    oldCurrent = listview->currentItem();
    model.removeItem(6);
    QTest::qWait(300);

    QCOMPARE(listview->currentIndex(), 7);
    QVERIFY(listview->currentItem() == oldCurrent);

    delete canvas;
}

template <class T>
void tst_QDeclarativeListView::clear()
{
    QDeclarativeView *canvas = createView();

    T model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.clear();

    // let transitions settle.
    QTest::qWait(500);

    QVERIFY(listview->count() == 0);
    QVERIFY(listview->currentItem() == 0);
    QVERIFY(listview->contentY() == 0);

    delete canvas;
}


template <class T>
void tst_QDeclarativeListView::moved()
{
    QDeclarativeView *canvas = createView();

    T model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    model.moveItem(1, 4);

    // let transitions settle.
    QTest::qWait(500);

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    name = findItem<QDeclarativeText>(viewport, "textName", 4);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(4));
    number = findItem<QDeclarativeText>(viewport, "textNumber", 4);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(4));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*20);
    }

    listview->setContentY(80);

    // move outside visible area
    model.moveItem(1, 18);

    // let transitions settle.
    QTest::qWait(500);

    // Confirm items positioned correctly and indexes correct
    for (int i = 3; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.0 + 20);
        name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // move from outside visible into visible
    model.moveItem(20, 4);

    // let transitions settle.
    QTest::qWait(500);

    // Confirm items positioned correctly and indexes correct
    for (int i = 3; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.0 + 20);
        name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QDeclarativeListView::enforceRange()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview-enforcerange.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QCOMPARE(listview->preferredHighlightBegin(), 100.0);
    QCOMPARE(listview->preferredHighlightEnd(), 100.0);
    QCOMPARE(listview->highlightRangeMode(), QDeclarativeListView::StrictlyEnforceRange);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    // view should be positioned at the top of the range.
    QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", 0);
    QVERIFY(item);
    QCOMPARE(listview->contentY(), -100.0);

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    // Check currentIndex is updated when viewport moves
    listview->setContentY(20);
    QTest::qWait(500);

    QCOMPARE(listview->currentIndex(), 6);

    delete canvas;
}

void tst_QDeclarativeListView::spacing()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*20);
    }

    listview->setSpacing(10);
    QVERIFY(listview->spacing() == 10);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*30);
    }

    listview->setSpacing(0);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.0);
    }

    delete canvas;
}

void tst_QDeclarativeListView::sections()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i/5));

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview-sections.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        QVERIFY(item);
        QCOMPARE(item->y(), qreal(i*20 + ((i+4)/5) * 20));
    }

    // Remove section boundary
    model.removeItem(5);
    QTest::qWait(100);

    // New section header created
    QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", 5);
    QVERIFY(item);
    QCOMPARE(item->height(), 40.0);

    model.insertItem(3, "New Item", "0");
    QTest::qWait(100);

    // Section header moved
    item = findItem<QDeclarativeItem>(viewport, "wrapper", 5);
    QVERIFY(item);
    QCOMPARE(item->height(), 20.0);

    item = findItem<QDeclarativeItem>(viewport, "wrapper", 6);
    QVERIFY(item);
    QCOMPARE(item->height(), 40.0);

    // insert item which will become a section header
    model.insertItem(6, "Replace header", "1");
    QTest::qWait(100);

    item = findItem<QDeclarativeItem>(viewport, "wrapper", 6);
    QVERIFY(item);
    QCOMPARE(item->height(), 40.0);

    item = findItem<QDeclarativeItem>(viewport, "wrapper", 7);
    QVERIFY(item);
    QCOMPARE(item->height(), 20.0);

    QCOMPARE(listview->currentSection(), QString("0"));

    listview->setContentY(140);
    QCOMPARE(listview->currentSection(), QString("1"));

    listview->setContentY(20);
    QCOMPARE(listview->currentSection(), QString("0"));

    item = findItem<QDeclarativeItem>(viewport, "wrapper", 1);
    QVERIFY(item);
    QCOMPARE(item->height(), 20.0);

    delete canvas;
}

void tst_QDeclarativeListView::currentIndex()
{
    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testWrap", QVariant(false));

    QString filename(SRCDIR "/data/listview-initCurrent.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    QTest::qWait(500);

    // current item should be third item
    QCOMPARE(listview->currentIndex(), 3);
    QCOMPARE(listview->currentItem(), findItem<QDeclarativeItem>(viewport, "wrapper", 3));
    QCOMPARE(listview->highlightItem()->y(), listview->currentItem()->y());

    // no wrap
    listview->setCurrentIndex(0);
    QCOMPARE(listview->currentIndex(), 0);

    listview->incrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), 1);
    listview->decrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), 0);

    listview->decrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), 0);

    // with wrap
    ctxt->setContextProperty("testWrap", QVariant(true));
    QVERIFY(listview->isWrapEnabled());

    listview->decrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), model.count()-1);

    QTest::qWait(1000);
    QCOMPARE(listview->contentY(), 279.0);

    listview->incrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), 0);

    QTest::qWait(1000);
    QCOMPARE(listview->contentY(), 0.0);

    // Test keys
    canvas->show();
    qApp->setActiveWindow(canvas);
#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(canvas);
#endif
    QVERIFY(canvas->hasFocus());
    QVERIFY(canvas->scene()->hasFocus());
    qApp->processEvents();

    QTest::keyClick(canvas, Qt::Key_Down);
    QCOMPARE(listview->currentIndex(), 1);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(listview->currentIndex(), 0);

    // turn off auto highlight
    listview->setHighlightFollowsCurrentItem(false);
    QVERIFY(listview->highlightFollowsCurrentItem() == false);

    QTest::qWait(500);
    QVERIFY(listview->highlightItem());
    qreal hlPos = listview->highlightItem()->y();

    listview->setCurrentIndex(4);
    QTest::qWait(500);
    QCOMPARE(listview->highlightItem()->y(), hlPos);

    // insert item before currentIndex
    listview->setCurrentIndex(28);
    model.insertItem(0, "Foo", "1111");
    QCOMPARE(canvas->rootObject()->property("current").toInt(), 29);

    delete canvas;
}

void tst_QDeclarativeListView::itemList()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/itemlist.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "view");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    QDeclarativeVisualItemModel *model = canvas->rootObject()->findChild<QDeclarativeVisualItemModel*>("itemModel");
    QVERIFY(model != 0);

    QVERIFY(model->count() == 3);
    QCOMPARE(listview->currentIndex(), 0);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "item1");
    QVERIFY(item);
    QCOMPARE(item->x(), 0.0);

    QDeclarativeText *text = findItem<QDeclarativeText>(viewport, "text1");
    QVERIFY(text);
    QCOMPARE(text->text(), QLatin1String("index: 0"));

    listview->setCurrentIndex(2);
    QTest::qWait(1000);

    item = findItem<QDeclarativeItem>(viewport, "item3");
    QVERIFY(item);
    QCOMPARE(item->x(), 480.0);

    text = findItem<QDeclarativeText>(viewport, "text3");
    QVERIFY(text);
    QCOMPARE(text->text(), QLatin1String("index: 2"));

    delete canvas;
}

void tst_QDeclarativeListView::cacheBuffer()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);
    QVERIFY(listview->delegate() != 0);
    QVERIFY(listview->model() != 0);
    QVERIFY(listview->highlight() != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*20);
    }

    testObject->setCacheBuffer(400);
    QVERIFY(listview->cacheBuffer() == 400);

    int newItemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    QVERIFY(newItemCount > itemCount);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < newItemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->y() == i*20);
    }

    delete canvas;
}

void tst_QDeclarativeListView::positionViewAtIndex()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 40; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listview.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.);
    }

    // Position on a currently visible item
    listview->positionViewAtIndex(3);
    QCOMPARE(listview->contentY(), 60.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 3; i < model.count() && i < itemCount-3-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.);
    }

    // Position on an item beyond the visible items
    listview->positionViewAtIndex(22);
    QCOMPARE(listview->contentY(), 440.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 22; i < model.count() && i < itemCount-22-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.);
    }

    // Position on an item that would leave empty space if positioned at the top
    listview->positionViewAtIndex(28);
    QCOMPARE(listview->contentY(), 480.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 24; i < model.count() && i < itemCount-24-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.);
    }

    // Position at the beginning again
    listview->positionViewAtIndex(0);
    QCOMPARE(listview->contentY(), 0.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->y(), i*20.);
    }

    delete canvas;
}

void tst_QDeclarativeListView::resetModel()
{
    QDeclarativeView *canvas = createView();

    QStringList strings;
    strings << "one" << "two" << "three";
    QStringListModel model(strings);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/displaylist.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QVERIFY(listview != 0);

    QDeclarativeItem *viewport = listview->viewport();
    QVERIFY(viewport != 0);

    QCOMPARE(listview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(viewport, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }

    strings.clear();
    strings << "four" << "five" << "six" << "seven";
    model.setStringList(strings);

    QCOMPARE(listview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(viewport, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }
}

void tst_QDeclarativeListView::propertyChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QVERIFY(listView);

    QSignalSpy highlightFollowsCurrentItemSpy(listView, SIGNAL(highlightFollowsCurrentItemChanged()));
    QSignalSpy preferredHighlightBeginSpy(listView, SIGNAL(preferredHighlightBeginChanged()));
    QSignalSpy preferredHighlightEndSpy(listView, SIGNAL(preferredHighlightEndChanged()));
    QSignalSpy highlightRangeModeSpy(listView, SIGNAL(highlightRangeModeChanged()));
    QSignalSpy keyNavigationWrapsSpy(listView, SIGNAL(keyNavigationWrapsChanged()));
    QSignalSpy cacheBufferSpy(listView, SIGNAL(cacheBufferChanged()));
    QSignalSpy snapModeSpy(listView, SIGNAL(snapModeChanged()));

    QCOMPARE(listView->highlightFollowsCurrentItem(), true);
    QCOMPARE(listView->preferredHighlightBegin(), 0.0);
    QCOMPARE(listView->preferredHighlightEnd(), 0.0);
    QCOMPARE(listView->highlightRangeMode(), QDeclarativeListView::ApplyRange);
    QCOMPARE(listView->isWrapEnabled(), true);
    QCOMPARE(listView->cacheBuffer(), 10);
    QCOMPARE(listView->snapMode(), QDeclarativeListView::SnapToItem);

    listView->setHighlightFollowsCurrentItem(false);
    listView->setPreferredHighlightBegin(1.0);
    listView->setPreferredHighlightEnd(1.0);
    listView->setHighlightRangeMode(QDeclarativeListView::StrictlyEnforceRange);
    listView->setWrapEnabled(false);
    listView->setCacheBuffer(3);
    listView->setSnapMode(QDeclarativeListView::SnapOneItem);

    QCOMPARE(listView->highlightFollowsCurrentItem(), false);
    QCOMPARE(listView->preferredHighlightBegin(), 1.0);
    QCOMPARE(listView->preferredHighlightEnd(), 1.0);
    QCOMPARE(listView->highlightRangeMode(), QDeclarativeListView::StrictlyEnforceRange);
    QCOMPARE(listView->isWrapEnabled(), false);
    QCOMPARE(listView->cacheBuffer(), 3);
    QCOMPARE(listView->snapMode(), QDeclarativeListView::SnapOneItem);

    QCOMPARE(highlightFollowsCurrentItemSpy.count(),1);
    QCOMPARE(preferredHighlightBeginSpy.count(),1);
    QCOMPARE(preferredHighlightEndSpy.count(),1);
    QCOMPARE(highlightRangeModeSpy.count(),1);
    QCOMPARE(keyNavigationWrapsSpy.count(),1);
    QCOMPARE(cacheBufferSpy.count(),1);
    QCOMPARE(snapModeSpy.count(),1);

    listView->setHighlightFollowsCurrentItem(false);
    listView->setPreferredHighlightBegin(1.0);
    listView->setPreferredHighlightEnd(1.0);
    listView->setHighlightRangeMode(QDeclarativeListView::StrictlyEnforceRange);
    listView->setWrapEnabled(false);
    listView->setCacheBuffer(3);
    listView->setSnapMode(QDeclarativeListView::SnapOneItem);

    QCOMPARE(highlightFollowsCurrentItemSpy.count(),1);
    QCOMPARE(preferredHighlightBeginSpy.count(),1);
    QCOMPARE(preferredHighlightEndSpy.count(),1);
    QCOMPARE(highlightRangeModeSpy.count(),1);
    QCOMPARE(keyNavigationWrapsSpy.count(),1);
    QCOMPARE(cacheBufferSpy.count(),1);
    QCOMPARE(snapModeSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeListView::componentChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QVERIFY(listView);

    QDeclarativeComponent component(canvas->engine());
    component.setData("import Qt 4.6; Rectangle { color: \"blue\"; }", QUrl::fromLocalFile(""));

    QDeclarativeComponent delegateComponent(canvas->engine());
    delegateComponent.setData("import Qt 4.6; Text { text: '<b>Name:</b> ' + name }", QUrl::fromLocalFile(""));

    QSignalSpy highlightSpy(listView, SIGNAL(highlightChanged()));
    QSignalSpy delegateSpy(listView, SIGNAL(delegateChanged()));
    QSignalSpy headerSpy(listView, SIGNAL(headerChanged()));
    QSignalSpy footerSpy(listView, SIGNAL(footerChanged()));

    listView->setHighlight(&component);
    listView->setHeader(&component);
    listView->setFooter(&component);
    listView->setDelegate(&delegateComponent);

    QCOMPARE(listView->highlight(), &component);
    QCOMPARE(listView->header(), &component);
    QCOMPARE(listView->footer(), &component);
    QCOMPARE(listView->delegate(), &delegateComponent);

    QCOMPARE(highlightSpy.count(),1);
    QCOMPARE(delegateSpy.count(),1);
    QCOMPARE(headerSpy.count(),1);
    QCOMPARE(footerSpy.count(),1);

    listView->setHighlight(&component);
    listView->setHeader(&component);
    listView->setFooter(&component);
    listView->setDelegate(&delegateComponent);

    QCOMPARE(highlightSpy.count(),1);
    QCOMPARE(delegateSpy.count(),1);
    QCOMPARE(headerSpy.count(),1);
    QCOMPARE(footerSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeListView::modelChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QVERIFY(listView);

    QDeclarativeListModel *alternateModel = canvas->rootObject()->findChild<QDeclarativeListModel*>("alternateModel");
    QVERIFY(alternateModel);
    QVariant modelVariant = QVariant::fromValue(alternateModel);
    QSignalSpy modelSpy(listView, SIGNAL(modelChanged()));

    listView->setModel(modelVariant);
    QCOMPARE(listView->model(), modelVariant);
    QCOMPARE(modelSpy.count(),1);

    listView->setModel(modelVariant);
    QCOMPARE(modelSpy.count(),1);

    listView->setModel(QVariant());
    QCOMPARE(modelSpy.count(),2);

    delete canvas;    
}

void tst_QDeclarativeListView::qListModelInterface_items()
{
    items<TestModel>();
}

void tst_QDeclarativeListView::qAbstractItemModel_items()
{
    items<TestModel2>();
}

void tst_QDeclarativeListView::qListModelInterface_changed()
{
    changed<TestModel>();
}

void tst_QDeclarativeListView::qAbstractItemModel_changed()
{
    changed<TestModel2>();
}

void tst_QDeclarativeListView::qListModelInterface_inserted()
{
    inserted<TestModel>();
}

void tst_QDeclarativeListView::qAbstractItemModel_inserted()
{
    inserted<TestModel2>();
}

void tst_QDeclarativeListView::qListModelInterface_removed()
{
    removed<TestModel>(false);
    removed<TestModel>(true);
}

void tst_QDeclarativeListView::qAbstractItemModel_removed()
{
    removed<TestModel2>(false);
    removed<TestModel2>(true);
}

void tst_QDeclarativeListView::qListModelInterface_moved()
{
    moved<TestModel>();
}

void tst_QDeclarativeListView::qAbstractItemModel_moved()
{
    moved<TestModel2>();
}

void tst_QDeclarativeListView::qListModelInterface_clear()
{
    clear<TestModel>();
}

void tst_QDeclarativeListView::qAbstractItemModel_clear()
{
    clear<TestModel2>();
}

QDeclarativeView *tst_QDeclarativeListView::createView()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    return canvas;
}

/*
   Find an item with the specified objectName.  If index is supplied then the
   item must also evaluate the {index} expression equal to index
*/
template<typename T>
T *tst_QDeclarativeListView::findItem(QGraphicsObject *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            if (index != -1) {
                QDeclarativeExpression e(qmlContext(item), "index", item);
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

template<typename T>
QList<T*> tst_QDeclarativeListView::findItems(QGraphicsObject *parent, const QString &objectName)
{
    QList<T*> items;
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName))
            items.append(static_cast<T*>(item));
        items += findItems<T>(item, objectName);
    }

    return items;
}

void tst_QDeclarativeListView::dumpTree(QDeclarativeItem *parent, int depth)
{
    static QString padding("                       ");
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        qDebug() << padding.left(depth*2) << item;
        dumpTree(item, depth+1);
    }
}


QTEST_MAIN(tst_QDeclarativeListView)

#include "tst_qdeclarativelistview.moc"
