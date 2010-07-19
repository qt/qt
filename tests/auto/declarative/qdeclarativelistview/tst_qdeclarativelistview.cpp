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
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

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
    void QTBUG_9791();
    void manualHighlight();
    void QTBUG_11105();
    void footer();

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QTRY_VERIFY(testObject->error() == false);

    QTRY_VERIFY(listview->highlightItem() != 0);
    QTRY_COMPARE(listview->count(), model.count());
    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    // current item should be first item
    QTRY_COMPARE(listview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 0));

    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    // switch to other delegate
    testObject->setAnimate(true);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QTRY_VERIFY(testObject->error() == false);
    QTRY_VERIFY(listview->currentItem());

    // set invalid highlight
    testObject->setInvalidHighlight(true);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QTRY_VERIFY(testObject->error() == false);
    QTRY_VERIFY(listview->currentItem());
    QTRY_VERIFY(listview->highlightItem() == 0);

    // back to normal highlight
    testObject->setInvalidHighlight(false);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QTRY_VERIFY(testObject->error() == false);
    QTRY_VERIFY(listview->currentItem());
    QTRY_VERIFY(listview->highlightItem() != 0);

    // set an empty model and confirm that items are destroyed
    T model2;
    ctxt->setContextProperty("testModel", &model2);

    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    QTRY_VERIFY(itemCount == 0);

    QTRY_COMPARE(listview->highlightResizeSpeed(), 1000.0);
    QTRY_COMPARE(listview->highlightMoveSpeed(), 1000.0);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeFlickable *listview = findItem<QDeclarativeFlickable>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.modifyItem(1, "Will", "9876");
    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.insertItem(1, "Will", "9876");

    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        QTRY_COMPARE(item->y(), i*20.0);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    name = findItem<QDeclarativeText>(contentItem, "textName", 0);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(contentItem, "textNumber", 0);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(0));

    QTRY_COMPARE(listview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        QTRY_COMPARE(item->y(), i*20.0);
    }

    for (int i = model.count(); i < 30; ++i)
        model.insertItem(i, "Hello", QString::number(i));

    listview->setContentY(80);

    // Insert item outside visible area
    model.insertItem(1, "Hello", "1324");

    QTRY_VERIFY(listview->contentY() == 80);

    // Confirm items positioned correctly
    for (int i = 5; i < 5+15; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.0 - 20.0);
    }

//    QTRY_COMPARE(listview->contentItemHeight(), model.count() * 20.0);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.removeItem(1);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);  // post: top item starts at 20

    QTest::qWait(300);

    name = findItem<QDeclarativeText>(contentItem, "textName", 0);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(contentItem, "textNumber", 0);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(0));

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(),i*20.0 + 20.0);
    }

    // Remove items not visible
    model.removeItem(18);
    qApp->processEvents();

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(),i*20.0+20.0);
    }

    // Remove items before visible
    listview->setContentY(80);
    listview->setCurrentIndex(10);

    model.removeItem(1); // post: top item will be at 40
    qApp->processEvents();

    // Confirm items positioned correctly
    for (int i = 2; i < 18; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(),40+i*20.0);
    }

    // Remove current index
    QTRY_VERIFY(listview->currentIndex() == 9);
    QDeclarativeItem *oldCurrent = listview->currentItem();
    model.removeItem(9);

    QTRY_COMPARE(listview->currentIndex(), 9);
    QTRY_VERIFY(listview->currentItem() != oldCurrent);

    listview->setContentY(40); // That's the top now
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(),40+i*20.0);
    }

    // remove current item beyond visible items.
    listview->setCurrentIndex(20);
    listview->setContentY(40);
    model.removeItem(20);

    QTRY_COMPARE(listview->currentIndex(), 20);
    QTRY_VERIFY(listview->currentItem() != 0);

    // remove item before current, but visible
    listview->setCurrentIndex(8);
    oldCurrent = listview->currentItem();
    model.removeItem(6);

    QTRY_COMPARE(listview->currentIndex(), 7);
    QTRY_VERIFY(listview->currentItem() == oldCurrent);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.clear();

    QTRY_VERIFY(listview->count() == 0);
    QTRY_VERIFY(listview->currentItem() == 0);
    QTRY_VERIFY(listview->contentY() == 0);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.moveItem(1, 4);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    name = findItem<QDeclarativeText>(contentItem, "textName", 4);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(4));
    number = findItem<QDeclarativeText>(contentItem, "textNumber", 4);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(4));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
    }

    listview->setContentY(80);

    // move outside visible area
    model.moveItem(1, 18);

    // Confirm items positioned correctly and indexes correct
    for (int i = 3; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.0 + 20);
        name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    // move from outside visible into visible
    model.moveItem(20, 4);

    // Confirm items positioned correctly and indexes correct
    for (int i = 3; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.0 + 20);
        name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
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
    QTRY_VERIFY(listview != 0);

    QTRY_COMPARE(listview->preferredHighlightBegin(), 100.0);
    QTRY_COMPARE(listview->preferredHighlightEnd(), 100.0);
    QTRY_COMPARE(listview->highlightRangeMode(), QDeclarativeListView::StrictlyEnforceRange);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // view should be positioned at the top of the range.
    QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", 0);
    QTRY_VERIFY(item);
    QTRY_COMPARE(listview->contentY(), -100.0);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 0);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(0));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 0);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(0));

    // Check currentIndex is updated when contentItem moves
    listview->setContentY(20);

    QTRY_COMPARE(listview->currentIndex(), 6);

    // change model
    TestModel model2;
    for (int i = 0; i < 5; i++)
        model2.addItem("Item" + QString::number(i), "");

    ctxt->setContextProperty("testModel", &model2);
    QCOMPARE(listview->count(), 5);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
    }

    listview->setSpacing(10);
    QTRY_VERIFY(listview->spacing() == 10);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*30);
    }

    listview->setSpacing(0);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.0);
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
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), qreal(i*20 + ((i+4)/5) * 20));
        QDeclarativeText *next = findItem<QDeclarativeText>(item, "nextSection");
        QCOMPARE(next->text().toInt(), (i+1)/5);
    }

    // Remove section boundary
    model.removeItem(5);

    // New section header created
    QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", 5);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 40.0);

    model.insertItem(3, "New Item", "0");

    // Section header moved
    item = findItem<QDeclarativeItem>(contentItem, "wrapper", 5);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 20.0);

    item = findItem<QDeclarativeItem>(contentItem, "wrapper", 6);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 40.0);

    // insert item which will become a section header
    model.insertItem(6, "Replace header", "1");

    item = findItem<QDeclarativeItem>(contentItem, "wrapper", 6);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 40.0);

    item = findItem<QDeclarativeItem>(contentItem, "wrapper", 7);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 20.0);

    QTRY_COMPARE(listview->currentSection(), QString("0"));

    listview->setContentY(140);
    QTRY_COMPARE(listview->currentSection(), QString("1"));

    listview->setContentY(20);
    QTRY_COMPARE(listview->currentSection(), QString("0"));

    item = findItem<QDeclarativeItem>(contentItem, "wrapper", 1);
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->height(), 20.0);

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
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // current item should be 20th item at startup
    // and current item should be in view
    QCOMPARE(listview->currentIndex(), 20);
    QCOMPARE(listview->contentY(), 99.0);
    QCOMPARE(listview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 20));
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

    QTRY_COMPARE(listview->contentY(), 279.0);

    listview->incrementCurrentIndex();
    QCOMPARE(listview->currentIndex(), 0);

    QTRY_COMPARE(listview->contentY(), 0.0);

    // Test keys
    canvas->show();
    qApp->setActiveWindow(canvas);
#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(canvas);
#endif
    QTRY_VERIFY(canvas->hasFocus());
    QTRY_VERIFY(canvas->scene()->hasFocus());
    qApp->processEvents();

    QTest::keyClick(canvas, Qt::Key_Down);
    QCOMPARE(listview->currentIndex(), 1);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(listview->currentIndex(), 0);

    // turn off auto highlight
    listview->setHighlightFollowsCurrentItem(false);
    QVERIFY(listview->highlightFollowsCurrentItem() == false);

    QVERIFY(listview->highlightItem());
    qreal hlPos = listview->highlightItem()->y();

    listview->setCurrentIndex(4);
    QTRY_COMPARE(listview->highlightItem()->y(), hlPos);

    // insert item before currentIndex
    listview->setCurrentIndex(28);
    model.insertItem(0, "Foo", "1111");
    QTRY_COMPARE(canvas->rootObject()->property("current").toInt(), 29);

    delete canvas;
}

void tst_QDeclarativeListView::itemList()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/itemlist.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "view");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QDeclarativeVisualItemModel *model = canvas->rootObject()->findChild<QDeclarativeVisualItemModel*>("itemModel");
    QTRY_VERIFY(model != 0);

    QTRY_VERIFY(model->count() == 3);
    QTRY_COMPARE(listview->currentIndex(), 0);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "item1");
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->x(), 0.0);

    QDeclarativeText *text = findItem<QDeclarativeText>(contentItem, "text1");
    QTRY_VERIFY(text);
    QTRY_COMPARE(text->text(), QLatin1String("index: 0"));

    listview->setCurrentIndex(2);

    item = findItem<QDeclarativeItem>(contentItem, "item3");
    QTRY_VERIFY(item);
    QTRY_COMPARE(item->x(), 480.0);

    text = findItem<QDeclarativeText>(contentItem, "text3");
    QTRY_VERIFY(text);
    QTRY_COMPARE(text->text(), QLatin1String("index: 2"));

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);
    QTRY_VERIFY(listview->delegate() != 0);
    QTRY_VERIFY(listview->model() != 0);
    QTRY_VERIFY(listview->highlight() != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
    }

    testObject->setCacheBuffer(400);
    QTRY_VERIFY(listview->cacheBuffer() == 400);

    int newItemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    QTRY_VERIFY(newItemCount > itemCount);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count() && i < newItemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.);
    }

    // Position on a currently visible item
    listview->positionViewAtIndex(3, QDeclarativeListView::Beginning);
    QTRY_COMPARE(listview->contentY(), 60.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 3; i < model.count() && i < itemCount-3-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.);
    }

    // Position on an item beyond the visible items
    listview->positionViewAtIndex(22, QDeclarativeListView::Beginning);
    QTRY_COMPARE(listview->contentY(), 440.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 22; i < model.count() && i < itemCount-22-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.);
    }

    // Position on an item that would leave empty space if positioned at the top
    listview->positionViewAtIndex(28, QDeclarativeListView::Beginning);
    QTRY_COMPARE(listview->contentY(), 480.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 24; i < model.count() && i < itemCount-24-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.);
    }

    // Position at the beginning again
    listview->positionViewAtIndex(0, QDeclarativeListView::Beginning);
    QTRY_COMPARE(listview->contentY(), 0.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->y(), i*20.);
    }

    // Position at End
    listview->positionViewAtIndex(20, QDeclarativeListView::End);
    QTRY_COMPARE(listview->contentY(), 100.);

    // Position in Center
    listview->positionViewAtIndex(15, QDeclarativeListView::Center);
    QTRY_COMPARE(listview->contentY(), 150.);

    // Ensure at least partially visible
    listview->positionViewAtIndex(15, QDeclarativeListView::Visible);
    QTRY_COMPARE(listview->contentY(), 150.);

    listview->setContentY(302);
    listview->positionViewAtIndex(15, QDeclarativeListView::Visible);
    QTRY_COMPARE(listview->contentY(), 302.);

    listview->setContentY(320);
    listview->positionViewAtIndex(15, QDeclarativeListView::Visible);
    QTRY_COMPARE(listview->contentY(), 300.);

    listview->setContentY(85);
    listview->positionViewAtIndex(20, QDeclarativeListView::Visible);
    QTRY_COMPARE(listview->contentY(), 85.);

    listview->setContentY(75);
    listview->positionViewAtIndex(20, QDeclarativeListView::Visible);
    QTRY_COMPARE(listview->contentY(), 100.);

    // Ensure completely visible
    listview->setContentY(120);
    listview->positionViewAtIndex(20, QDeclarativeListView::Contain);
    QTRY_COMPARE(listview->contentY(), 120.);

    listview->setContentY(302);
    listview->positionViewAtIndex(15, QDeclarativeListView::Contain);
    QTRY_COMPARE(listview->contentY(), 300.);

    listview->setContentY(85);
    listview->positionViewAtIndex(20, QDeclarativeListView::Contain);
    QTRY_COMPARE(listview->contentY(), 100.);

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
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QTRY_COMPARE(listview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(contentItem, "displayText", i);
        QTRY_VERIFY(display != 0);
        QTRY_COMPARE(display->text(), strings.at(i));
    }

    strings.clear();
    strings << "four" << "five" << "six" << "seven";
    model.setStringList(strings);

    QTRY_COMPARE(listview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(contentItem, "displayText", i);
        QTRY_VERIFY(display != 0);
        QTRY_COMPARE(display->text(), strings.at(i));
    }
}

void tst_QDeclarativeListView::propertyChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QTRY_VERIFY(listView);

    QSignalSpy highlightFollowsCurrentItemSpy(listView, SIGNAL(highlightFollowsCurrentItemChanged()));
    QSignalSpy preferredHighlightBeginSpy(listView, SIGNAL(preferredHighlightBeginChanged()));
    QSignalSpy preferredHighlightEndSpy(listView, SIGNAL(preferredHighlightEndChanged()));
    QSignalSpy highlightRangeModeSpy(listView, SIGNAL(highlightRangeModeChanged()));
    QSignalSpy keyNavigationWrapsSpy(listView, SIGNAL(keyNavigationWrapsChanged()));
    QSignalSpy cacheBufferSpy(listView, SIGNAL(cacheBufferChanged()));
    QSignalSpy snapModeSpy(listView, SIGNAL(snapModeChanged()));

    QTRY_COMPARE(listView->highlightFollowsCurrentItem(), true);
    QTRY_COMPARE(listView->preferredHighlightBegin(), 0.0);
    QTRY_COMPARE(listView->preferredHighlightEnd(), 0.0);
    QTRY_COMPARE(listView->highlightRangeMode(), QDeclarativeListView::ApplyRange);
    QTRY_COMPARE(listView->isWrapEnabled(), true);
    QTRY_COMPARE(listView->cacheBuffer(), 10);
    QTRY_COMPARE(listView->snapMode(), QDeclarativeListView::SnapToItem);

    listView->setHighlightFollowsCurrentItem(false);
    listView->setPreferredHighlightBegin(1.0);
    listView->setPreferredHighlightEnd(1.0);
    listView->setHighlightRangeMode(QDeclarativeListView::StrictlyEnforceRange);
    listView->setWrapEnabled(false);
    listView->setCacheBuffer(3);
    listView->setSnapMode(QDeclarativeListView::SnapOneItem);

    QTRY_COMPARE(listView->highlightFollowsCurrentItem(), false);
    QTRY_COMPARE(listView->preferredHighlightBegin(), 1.0);
    QTRY_COMPARE(listView->preferredHighlightEnd(), 1.0);
    QTRY_COMPARE(listView->highlightRangeMode(), QDeclarativeListView::StrictlyEnforceRange);
    QTRY_COMPARE(listView->isWrapEnabled(), false);
    QTRY_COMPARE(listView->cacheBuffer(), 3);
    QTRY_COMPARE(listView->snapMode(), QDeclarativeListView::SnapOneItem);

    QTRY_COMPARE(highlightFollowsCurrentItemSpy.count(),1);
    QTRY_COMPARE(preferredHighlightBeginSpy.count(),1);
    QTRY_COMPARE(preferredHighlightEndSpy.count(),1);
    QTRY_COMPARE(highlightRangeModeSpy.count(),1);
    QTRY_COMPARE(keyNavigationWrapsSpy.count(),1);
    QTRY_COMPARE(cacheBufferSpy.count(),1);
    QTRY_COMPARE(snapModeSpy.count(),1);

    listView->setHighlightFollowsCurrentItem(false);
    listView->setPreferredHighlightBegin(1.0);
    listView->setPreferredHighlightEnd(1.0);
    listView->setHighlightRangeMode(QDeclarativeListView::StrictlyEnforceRange);
    listView->setWrapEnabled(false);
    listView->setCacheBuffer(3);
    listView->setSnapMode(QDeclarativeListView::SnapOneItem);

    QTRY_COMPARE(highlightFollowsCurrentItemSpy.count(),1);
    QTRY_COMPARE(preferredHighlightBeginSpy.count(),1);
    QTRY_COMPARE(preferredHighlightEndSpy.count(),1);
    QTRY_COMPARE(highlightRangeModeSpy.count(),1);
    QTRY_COMPARE(keyNavigationWrapsSpy.count(),1);
    QTRY_COMPARE(cacheBufferSpy.count(),1);
    QTRY_COMPARE(snapModeSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeListView::componentChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QTRY_VERIFY(listView);

    QDeclarativeComponent component(canvas->engine());
    component.setData("import Qt 4.7; Rectangle { color: \"blue\"; }", QUrl::fromLocalFile(""));

    QDeclarativeComponent delegateComponent(canvas->engine());
    delegateComponent.setData("import Qt 4.7; Text { text: '<b>Name:</b> ' + name }", QUrl::fromLocalFile(""));

    QSignalSpy highlightSpy(listView, SIGNAL(highlightChanged()));
    QSignalSpy delegateSpy(listView, SIGNAL(delegateChanged()));
    QSignalSpy headerSpy(listView, SIGNAL(headerChanged()));
    QSignalSpy footerSpy(listView, SIGNAL(footerChanged()));

    listView->setHighlight(&component);
    listView->setHeader(&component);
    listView->setFooter(&component);
    listView->setDelegate(&delegateComponent);

    QTRY_COMPARE(listView->highlight(), &component);
    QTRY_COMPARE(listView->header(), &component);
    QTRY_COMPARE(listView->footer(), &component);
    QTRY_COMPARE(listView->delegate(), &delegateComponent);

    QTRY_COMPARE(highlightSpy.count(),1);
    QTRY_COMPARE(delegateSpy.count(),1);
    QTRY_COMPARE(headerSpy.count(),1);
    QTRY_COMPARE(footerSpy.count(),1);

    listView->setHighlight(&component);
    listView->setHeader(&component);
    listView->setFooter(&component);
    listView->setDelegate(&delegateComponent);

    QTRY_COMPARE(highlightSpy.count(),1);
    QTRY_COMPARE(delegateSpy.count(),1);
    QTRY_COMPARE(headerSpy.count(),1);
    QTRY_COMPARE(footerSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeListView::modelChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeListView *listView = canvas->rootObject()->findChild<QDeclarativeListView*>("listView");
    QTRY_VERIFY(listView);

    QDeclarativeListModel *alternateModel = canvas->rootObject()->findChild<QDeclarativeListModel*>("alternateModel");
    QTRY_VERIFY(alternateModel);
    QVariant modelVariant = QVariant::fromValue(alternateModel);
    QSignalSpy modelSpy(listView, SIGNAL(modelChanged()));

    listView->setModel(modelVariant);
    QTRY_COMPARE(listView->model(), modelVariant);
    QTRY_COMPARE(modelSpy.count(),1);

    listView->setModel(modelVariant);
    QTRY_COMPARE(modelSpy.count(),1);

    listView->setModel(QVariant());
    QTRY_COMPARE(modelSpy.count(),2);

    delete canvas;
}

void tst_QDeclarativeListView::QTBUG_9791()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/strictlyenforcerange.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = qobject_cast<QDeclarativeListView*>(canvas->rootObject());
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);
    QTRY_VERIFY(listview->delegate() != 0);
    QTRY_VERIFY(listview->model() != 0);

    QMetaObject::invokeMethod(listview, "fillModel");
    qApp->processEvents();

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    QVERIFY(itemCount == 3);

    for (int i = 0; i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), i*300.0);
    }

    // check that view is positioned correctly
    QTRY_COMPARE(listview->contentX(), 590.0);

    delete canvas;
}

void tst_QDeclarativeListView::manualHighlight()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    QDeclarativeContext *ctxt = canvas->rootContext();

    QString filename(SRCDIR "/data/manual-highlight.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QTRY_COMPARE(listview->currentIndex(), 0);
    QTRY_COMPARE(listview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 0));
    QTRY_COMPARE(listview->highlightItem()->y(), listview->currentItem()->y());

    listview->setCurrentIndex(2);

    QTRY_COMPARE(listview->currentIndex(), 2);
    QTRY_COMPARE(listview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 2));
    QTRY_COMPARE(listview->highlightItem()->y(), listview->currentItem()->y());
}

void tst_QDeclarativeListView::QTBUG_11105()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/listviewtest.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->y() == i*20);
    }

    listview->positionViewAtIndex(20, QDeclarativeListView::Beginning);
    QCOMPARE(listview->contentY(), 280.);

    TestModel model2;
    for (int i = 0; i < 5; i++)
        model2.addItem("Item" + QString::number(i), "");

    ctxt->setContextProperty("testModel", &model2);

    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    QCOMPARE(itemCount, 5);

    delete canvas;
}

void tst_QDeclarativeListView::footer()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 3; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/footer.qml"));
    qApp->processEvents();

    QDeclarativeListView *listview = findItem<QDeclarativeListView>(canvas->rootObject(), "list");
    QTRY_VERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QDeclarativeText *footer = findItem<QDeclarativeText>(contentItem, "footer");
    QVERIFY(footer);
    QCOMPARE(footer->y(), 60.0);

    model.removeItem(1);
    QTRY_COMPARE(footer->y(), 40.0);

    model.clear();
    QTRY_COMPARE(footer->y(), 0.0);
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
                QDeclarativeExpression e(qmlContext(item), item, "index");
                if (e.evaluate().toInt() == index)
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
