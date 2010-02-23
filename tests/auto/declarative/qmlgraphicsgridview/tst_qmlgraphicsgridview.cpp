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

#include <qmlengine.h>
#include <qmlcomponent.h>
#include <QtTest/QtTest>
#include <private/qlistmodelinterface_p.h>
#include <qmlview.h>
#include <private/qmlgraphicsgridview_p.h>
#include <private/qmlgraphicstext_p.h>
#include <qmlcontext.h>
#include <qmlexpression.h>

class tst_QmlGraphicsGridView : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsGridView();

private slots:
    void items();
    void changed();
    void inserted();
    void removed();
    void moved();
    void changeFlow();
    void currentIndex();
    void defaultValues();
    void properties();
    void positionViewAtIndex();

private:
    QmlView *createView();
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id, int index=-1);
    template<typename T>
    QList<T*> findItems(QGraphicsObject *parent, const QString &objectName);
    void dumpTree(QmlGraphicsItem *parent, int depth = 0);
};

class TestModel : public QAbstractListModel
{
public:
    enum Roles { Name = Qt::UserRole+1, Number = Qt::UserRole+2 };

    TestModel(QObject *parent=0) : QAbstractListModel(parent) {
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

    void moveItem(int from, int to) {
        emit beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
        list.move(from, to);
        emit endMoveRows();
    }

    void modifyItem(int idx, const QString &name, const QString &number) {
        list[idx] = QPair<QString,QString>(name, number);
        emit dataChanged(index(idx,0), index(idx,0));
    }

private:
    QList<QPair<QString,QString> > list;
};

tst_QmlGraphicsGridView::tst_QmlGraphicsGridView()
{
}

void tst_QmlGraphicsGridView::items()
{
    QmlView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Billy", "22345");
    model.addItem("Sam", "2945");
    model.addItem("Ben", "04321");
    model.addItem("Jim", "0780");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    QCOMPARE(gridview->count(), model.count());
    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    for (int i = 0; i < model.count(); ++i) {
        QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // set an empty model and confirm that items are destroyed
    TestModel model2;
    ctxt->setContextProperty("testModel", &model2);

    int itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    QVERIFY(itemCount == 0);

    delete canvas;
}

void tst_QmlGraphicsGridView::changed()
{
    QmlView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Billy", "22345");
    model.addItem("Sam", "2945");
    model.addItem("Ben", "04321");
    model.addItem("Jim", "0780");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsFlickable *gridview = findItem<QmlGraphicsFlickable>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.modifyItem(1, "Will", "9876");
    QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    delete canvas;
}

void tst_QmlGraphicsGridView::inserted()
{
    QmlView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.insertItem(1, "Will", "9876");

    // let transitions settle.
    QTest::qWait(300);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        QCOMPARE(item->x(), (i%3)*80.0);
        QCOMPARE(item->y(), (i/3)*60.0);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    // let transitions settle.
    QTest::qWait(300);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    name = findItem<QmlGraphicsText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QmlGraphicsText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    QCOMPARE(gridview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    for (int i = model.count(); i < 30; ++i)
        model.insertItem(i, "Hello", QString::number(i));
    QTest::qWait(300);

    gridview->setViewportY(120);
    QTest::qWait(300);

    // Insert item outside visible area
    model.insertItem(1, "Hello", "1324");
    QTest::qWait(300);

    QVERIFY(gridview->viewportY() == 120);

    delete canvas;
}

void tst_QmlGraphicsGridView::removed()
{
    QmlView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 40; i++)
        model.addItem("Item" + QString::number(i), "");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(300);

    QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Confirm items positioned correctly
    int itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);

    // let transitions settle.
    QTest::qWait(300);

    name = findItem<QmlGraphicsText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QmlGraphicsText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove items not visible
    model.removeItem(25);
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove items before visible
    gridview->setViewportY(120);
    QTest::qWait(500);
    gridview->setCurrentIndex(10);

    // let transitions settle.
    QTest::qWait(300);

    // Setting currentIndex above shouldn't cause view to scroll
    QCOMPARE(gridview->viewportY(), 120.0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    for (int i = 6; i < 18; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove currentIndex
    QmlGraphicsItem *oldCurrent = gridview->currentItem();
    model.removeItem(9);
    QTest::qWait(500);

    QCOMPARE(gridview->currentIndex(), 9);
    QVERIFY(gridview->currentItem() != oldCurrent);

    gridview->setViewportY(0);
    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // remove item outside current view.
    gridview->setCurrentIndex(32);
    QTest::qWait(500);
    gridview->setViewportY(240);

    model.removeItem(30);
    QVERIFY(gridview->currentIndex() == 31);

    // remove current item beyond visible items.
    gridview->setCurrentIndex(20);
    QTest::qWait(500);
    gridview->setViewportY(0);
    model.removeItem(20);
    QTest::qWait(500);

    QCOMPARE(gridview->currentIndex(), 20);
    QVERIFY(gridview->currentItem() != 0);

    // remove item before current, but visible
    gridview->setCurrentIndex(8);
    QTest::qWait(500);
    gridview->setViewportY(240);
    oldCurrent = gridview->currentItem();
    model.removeItem(6);
    QTest::qWait(500);

    QCOMPARE(gridview->currentIndex(), 7);
    QVERIFY(gridview->currentItem() == oldCurrent);

    delete canvas;
}

void tst_QmlGraphicsGridView::moved()
{
    QmlView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.moveItem(1, 8);

    // let transitions settle.
    QTest::qWait(300);

    QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    name = findItem<QmlGraphicsText>(viewport, "textName", 8);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(8));
    number = findItem<QmlGraphicsText>(viewport, "textNumber", 8);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(8));

    // Confirm items positioned correctly
    int itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    gridview->setViewportY(120);

    // move outside visible area
    model.moveItem(1, 25);

    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count()-1;
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i%3)*80));
        QCOMPARE(item->y(), qreal((i/3)*60));
        name = findItem<QmlGraphicsText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QmlGraphicsText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // move from outside visible into visible
    model.moveItem(28, 8);

    // let transitions settle.
    QTest::qWait(300);

    // Confirm items positioned correctly and indexes correct
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
        name = findItem<QmlGraphicsText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QmlGraphicsText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QmlGraphicsGridView::currentIndex()
{
    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    QString filename(SRCDIR "/data/gridview-initCurrent.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    QTest::qWait(500);

    // current item should be third item
    QCOMPARE(gridview->currentIndex(), 5);
    QCOMPARE(gridview->currentItem(), findItem<QmlGraphicsItem>(viewport, "wrapper", 5));
    QCOMPARE(gridview->currentItem()->y(), gridview->highlightItem()->y());

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), 6);
    gridview->moveCurrentIndexDown();
    QCOMPARE(gridview->currentIndex(), 9);
    gridview->moveCurrentIndexUp();
    QCOMPARE(gridview->currentIndex(), 6);
    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), 5);

    // no wrap
    gridview->setCurrentIndex(0);
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->moveCurrentIndexUp();
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->setCurrentIndex(model.count()-1);
    QTest::qWait(500);
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    gridview->moveCurrentIndexDown();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    // with wrap
    gridview->setWrapEnabled(true);

    gridview->setCurrentIndex(0);
    QCOMPARE(gridview->currentIndex(), 0);
    QTest::qWait(500);

    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    QTest::qWait(500);
    QCOMPARE(gridview->viewportY(), 279.0);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), 0);

    QTest::qWait(500);
    QCOMPARE(gridview->viewportY(), 0.0);

    // Test keys
    qApp->setActiveWindow(canvas);
    canvas->show();
    canvas->setFocus();
    qApp->processEvents();

    QTest::keyClick(canvas, Qt::Key_Down);
    QCOMPARE(gridview->currentIndex(), 3);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->setFlow(QmlGraphicsGridView::TopToBottom);

    QTest::keyClick(canvas, Qt::Key_Right);
    QCOMPARE(gridview->currentIndex(), 5);

    QTest::keyClick(canvas, Qt::Key_Left);
    QCOMPARE(gridview->currentIndex(), 0);

    QTest::keyClick(canvas, Qt::Key_Down);
    QCOMPARE(gridview->currentIndex(), 1);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(gridview->currentIndex(), 0);


    // turn off auto highlight
    gridview->setHighlightFollowsCurrentItem(false);
    QVERIFY(gridview->highlightFollowsCurrentItem() == false);

    QTest::qWait(500);
    QVERIFY(gridview->highlightItem());
    qreal hlPosX = gridview->highlightItem()->x();
    qreal hlPosY = gridview->highlightItem()->y();

    gridview->setCurrentIndex(5);
    QTest::qWait(500);
    QCOMPARE(gridview->highlightItem()->x(), hlPosX);
    QCOMPARE(gridview->highlightItem()->y(), hlPosY);

    delete canvas;
}

void tst_QmlGraphicsGridView::changeFlow()
{
    QmlView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly and indexes correct
    int itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i%3)*80));
        QCOMPARE(item->y(), qreal((i/3)*60));
        QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    ctxt->setContextProperty("testTopToBottom", QVariant(true));
    QTest::qWait(500);

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i/5)*80));
        QCOMPARE(item->y(), qreal((i%5)*60));
        QmlGraphicsText *name = findItem<QmlGraphicsText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QmlGraphicsText *number = findItem<QmlGraphicsText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QmlGraphicsGridView::defaultValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview3.qml"));
    QmlGraphicsGridView *obj = qobject_cast<QmlGraphicsGridView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->model() == QVariant());
    QVERIFY(obj->delegate() == 0);
    QCOMPARE(obj->currentIndex(), -1);
    QVERIFY(obj->currentItem() == 0);
    QCOMPARE(obj->count(), 0);
    QVERIFY(obj->highlight() == 0);
    QVERIFY(obj->highlightItem() == 0);
    QCOMPARE(obj->highlightFollowsCurrentItem(), true);
    QVERIFY(obj->flow() == 0);
    QCOMPARE(obj->isWrapEnabled(), false);
    QCOMPARE(obj->cacheBuffer(), 0);
    QCOMPARE(obj->cellWidth(), 100); //### Should 100 be the default?
    QCOMPARE(obj->cellHeight(), 100);
    delete obj;
}

void tst_QmlGraphicsGridView::properties()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview2.qml"));
    QmlGraphicsGridView *obj = qobject_cast<QmlGraphicsGridView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->model() != QVariant());
    QVERIFY(obj->delegate() != 0);
    QCOMPARE(obj->currentIndex(), 0);
    QVERIFY(obj->currentItem() != 0);
    QCOMPARE(obj->count(), 4);
    QVERIFY(obj->highlight() != 0);
    QVERIFY(obj->highlightItem() != 0);
    QCOMPARE(obj->highlightFollowsCurrentItem(), false);
    QVERIFY(obj->flow() == 0);
    QCOMPARE(obj->isWrapEnabled(), true);
    QCOMPARE(obj->cacheBuffer(), 200);
    QCOMPARE(obj->cellWidth(), 100);
    QCOMPARE(obj->cellHeight(), 100);
    delete obj;
}

void tst_QmlGraphicsGridView::positionViewAtIndex()
{
    QmlView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 40; i++)
        model.addItem("Item" + QString::number(i), "");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QmlGraphicsGridView *gridview = findItem<QmlGraphicsGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QmlGraphicsItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on a currently visible item
    gridview->positionViewAtIndex(4);
    QCOMPARE(gridview->viewportY(), 60.);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 3; i < model.count() && i < itemCount-3-1; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item beyond the visible items
    gridview->positionViewAtIndex(21);
    QCOMPARE(gridview->viewportY(), 420.);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 22; i < model.count() && i < itemCount-22-1; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item that would leave empty space if positioned at the top
    gridview->positionViewAtIndex(31);
    QCOMPARE(gridview->viewportY(), 520.);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 24; i < model.count() && i < itemCount-24-1; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position at the beginning again
    gridview->positionViewAtIndex(0);
    QCOMPARE(gridview->viewportY(), 0.);

    // Confirm items positioned correctly
    itemCount = findItems<QmlGraphicsItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QmlGraphicsItem *item = findItem<QmlGraphicsItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    delete canvas;
}

QmlView *tst_QmlGraphicsGridView::createView()
{
    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    return canvas;
}

/*
   Find an item with the specified objectName.  If index is supplied then the
   item must also evaluate the {index} expression equal to index
*/
template<typename T>
T *tst_QmlGraphicsGridView::findItem(QGraphicsObject *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            if (index != -1) {
                QmlContext *context = QmlEngine::contextForObject(item);
                if (context) {
                    if (context->contextProperty("index").toInt() == index) {
                        return static_cast<T*>(item);
                    }
                }
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
QList<T*> tst_QmlGraphicsGridView::findItems(QGraphicsObject *parent, const QString &objectName)
{
    QList<T*> items;
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            items.append(static_cast<T*>(item));
            //qDebug() << " found:" << item;
        }
        items += findItems<T>(item, objectName);
    }

    return items;
}

void tst_QmlGraphicsGridView::dumpTree(QmlGraphicsItem *parent, int depth)
{
    static QString padding("                       ");
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        QmlContext *context = QmlEngine::contextForObject(item);
        qDebug() << padding.left(depth*2) << item << (context ? context->contextProperty("index").toInt() : -1);
        dumpTree(item, depth+1);
    }
}


QTEST_MAIN(tst_QmlGraphicsGridView)

#include "tst_qmlgraphicsgridview.moc"
