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
<<<<<<< HEAD
#include <private/qlistmodelinterface_p.h>
#include <qdeclarativeview.h>
#include <private/qdeclarativegridview_p.h>
#include <private/qdeclarativetext_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeexpression.h>
=======
#include <QtGui/qstringlistmodel.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/private/qlistmodelinterface_p.h>
#include <QtDeclarative/private/qdeclarativegridview_p.h>
#include <QtDeclarative/private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativelistmodel_p.h>
>>>>>>> Add NOTIFY signals to list, grid and path views

class tst_QDeclarativeGridView : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeGridView();

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
    void propertyChanges();
    void componentChanges();
    void modelChanges();
    void positionViewAtIndex();
    void resetModel();
    void QTBUG_8456();

private:
    QDeclarativeView *createView();
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id, int index=-1);
    template<typename T>
    QList<T*> findItems(QGraphicsObject *parent, const QString &objectName);
    void dumpTree(QDeclarativeItem *parent, int depth = 0);
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

private:
    QList<QPair<QString,QString> > list;
};

tst_QDeclarativeGridView::tst_QDeclarativeGridView()
{
}

void tst_QDeclarativeGridView::items()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Billy", "22345");
    model.addItem("Sam", "2945");
    model.addItem("Ben", "04321");
    model.addItem("Jim", "0780");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    QCOMPARE(gridview->count(), model.count());
    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // set an empty model and confirm that items are destroyed
    TestModel model2;
    ctxt->setContextProperty("testModel", &model2);

    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    QVERIFY(itemCount == 0);

    delete canvas;
}

void tst_QDeclarativeGridView::changed()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Billy", "22345");
    model.addItem("Sam", "2945");
    model.addItem("Ben", "04321");
    model.addItem("Jim", "0780");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeFlickable *gridview = findItem<QDeclarativeFlickable>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
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

void tst_QDeclarativeGridView::inserted()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.insertItem(1, "Will", "9876");

    // let transitions settle.
    QTest::qWait(100);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Checks that onAdd is called
    int added = canvas->rootObject()->property("added").toInt();
    QCOMPARE(added, 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        QCOMPARE(item->x(), (i%3)*80.0);
        QCOMPARE(item->y(), (i/3)*60.0);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    // let transitions settle.
    QTest::qWait(100);

    QCOMPARE(viewport->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    name = findItem<QDeclarativeText>(viewport, "textName", 0);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(viewport, "textNumber", 0);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(0));

    QCOMPARE(gridview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    for (int i = model.count(); i < 30; ++i)
        model.insertItem(i, "Hello", QString::number(i));
    QTest::qWait(100);

    gridview->setContentY(120);
    QTest::qWait(100);

    // Insert item outside visible area
    model.insertItem(1, "Hello", "1324");
    QTest::qWait(100);

    QVERIFY(gridview->contentY() == 120);

    delete canvas;
}

void tst_QDeclarativeGridView::removed()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 40; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(100);

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    // Checks that onRemove is called
    QString removed = canvas->rootObject()->property("removed").toString();
    QCOMPARE(removed, QString("Item1"));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);

    // let transitions settle.
    QTest::qWait(100);

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
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove items not visible
    model.removeItem(25);
    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove items before visible
    gridview->setContentY(120);
    QTest::qWait(100);
    gridview->setCurrentIndex(10);

    // let transitions settle.
    QTest::qWait(100);

    // Setting currentIndex above shouldn't cause view to scroll
    QCOMPARE(gridview->contentY(), 120.0);

    model.removeItem(1);

    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly
    for (int i = 6; i < 18; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // Remove currentIndex
    QDeclarativeItem *oldCurrent = gridview->currentItem();
    model.removeItem(9);
    QTest::qWait(100);

    QCOMPARE(gridview->currentIndex(), 9);
    QVERIFY(gridview->currentItem() != oldCurrent);

    gridview->setContentY(0);
    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    // remove item outside current view.
    gridview->setCurrentIndex(32);
    QTest::qWait(100);
    gridview->setContentY(240);

    model.removeItem(30);
    QVERIFY(gridview->currentIndex() == 31);

    // remove current item beyond visible items.
    gridview->setCurrentIndex(20);
    QTest::qWait(100);
    gridview->setContentY(0);
    model.removeItem(20);
    QTest::qWait(100);

    QCOMPARE(gridview->currentIndex(), 20);
    QVERIFY(gridview->currentItem() != 0);

    // remove item before current, but visible
    gridview->setCurrentIndex(8);
    QTest::qWait(100);
    gridview->setContentY(240);
    oldCurrent = gridview->currentItem();
    model.removeItem(6);
    QTest::qWait(100);

    QCOMPARE(gridview->currentIndex(), 7);
    QVERIFY(gridview->currentItem() == oldCurrent);

    delete canvas;
}

void tst_QDeclarativeGridView::moved()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    model.moveItem(1, 8);

    // let transitions settle.
    QTest::qWait(100);

    QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", 1);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(1));

    name = findItem<QDeclarativeText>(viewport, "textName", 8);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), model.name(8));
    number = findItem<QDeclarativeText>(viewport, "textNumber", 8);
    QVERIFY(number != 0);
    QCOMPARE(number->text(), model.number(8));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
    }

    gridview->setContentY(120);

    // move outside visible area
    model.moveItem(1, 25);

    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count()-1;
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i%3)*80));
        QCOMPARE(item->y(), qreal((i/3)*60));
        name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    // move from outside visible into visible
    model.moveItem(28, 8);

    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly and indexes correct
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QVERIFY(item->x() == (i%3)*80);
        QVERIFY(item->y() == (i/3)*60);
        name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QDeclarativeGridView::currentIndex()
{
    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    QString filename(SRCDIR "/data/gridview-initCurrent.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    QTest::qWait(300);

    // current item should be third item
    QCOMPARE(gridview->currentIndex(), 5);
    QCOMPARE(gridview->currentItem(), findItem<QDeclarativeItem>(viewport, "wrapper", 5));
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
    QTest::qWait(100);
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
    QCOMPARE(gridview->contentY(), 279.0);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), 0);

    QTest::qWait(500);
    QCOMPARE(gridview->contentY(), 0.0);

    // Test keys
    qApp->setActiveWindow(canvas);
    canvas->show();
    canvas->setFocus();
    qApp->processEvents();

    QTest::keyClick(canvas, Qt::Key_Down);
    QCOMPARE(gridview->currentIndex(), 3);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->setFlow(QDeclarativeGridView::TopToBottom);

    QEXPECT_FAIL("", "QTBUG-8475", Abort);
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
    QVERIFY(gridview->highlightItem());
    qreal hlPosX = gridview->highlightItem()->x();
    qreal hlPosY = gridview->highlightItem()->y();

    gridview->setCurrentIndex(5);
    QTest::qWait(500);
    QCOMPARE(gridview->highlightItem()->x(), hlPosX);
    QCOMPARE(gridview->highlightItem()->y(), hlPosY);

    delete canvas;
}

void tst_QDeclarativeGridView::changeFlow()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly and indexes correct
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i%3)*80));
        QCOMPARE(item->y(), qreal((i/3)*60));
        QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    ctxt->setContextProperty("testTopToBottom", QVariant(true));
    QTest::qWait(100);

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), qreal((i/5)*80));
        QCOMPARE(item->y(), qreal((i%5)*60));
        QDeclarativeText *name = findItem<QDeclarativeText>(viewport, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(viewport, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QDeclarativeGridView::defaultValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview3.qml"));
    QDeclarativeGridView *obj = qobject_cast<QDeclarativeGridView*>(c.create());

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

void tst_QDeclarativeGridView::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview2.qml"));
    QDeclarativeGridView *obj = qobject_cast<QDeclarativeGridView*>(c.create());

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

void tst_QDeclarativeGridView::propertyChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QVERIFY(gridView);

    QSignalSpy keyNavigationWrapsSpy(gridView, SIGNAL(keyNavigationWrapsChanged()));
    QSignalSpy cacheBufferSpy(gridView, SIGNAL(cacheBufferChanged()));
    QSignalSpy flowSpy(gridView, SIGNAL(flowChanged()));

    QCOMPARE(gridView->isWrapEnabled(), true);
    QCOMPARE(gridView->cacheBuffer(), 10);
    QCOMPARE(gridView->flow(), QDeclarativeGridView::LeftToRight);

    gridView->setWrapEnabled(false);
    gridView->setCacheBuffer(3);
    gridView->setFlow(QDeclarativeGridView::TopToBottom);

    QCOMPARE(gridView->isWrapEnabled(), false);
    QCOMPARE(gridView->cacheBuffer(), 3);
    QCOMPARE(gridView->flow(), QDeclarativeGridView::TopToBottom);

    QCOMPARE(keyNavigationWrapsSpy.count(),1);
    QCOMPARE(cacheBufferSpy.count(),1);
    QCOMPARE(flowSpy.count(),1);

    gridView->setWrapEnabled(false);
    gridView->setCacheBuffer(3);
    gridView->setFlow(QDeclarativeGridView::TopToBottom);

    QCOMPARE(keyNavigationWrapsSpy.count(),1);
    QCOMPARE(cacheBufferSpy.count(),1);
    QCOMPARE(flowSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeGridView::componentChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QVERIFY(gridView);

    QDeclarativeComponent component(canvas->engine());
    component.setData("import Qt 4.6; Rectangle { color: \"blue\"; }", QUrl::fromLocalFile(""));

    QDeclarativeComponent delegateComponent(canvas->engine());
    delegateComponent.setData("import Qt 4.6; Text { text: '<b>Name:</b> ' + name }", QUrl::fromLocalFile(""));

    QSignalSpy highlightSpy(gridView, SIGNAL(highlightChanged()));
    QSignalSpy delegateSpy(gridView, SIGNAL(delegateChanged()));

    gridView->setHighlight(&component);
    gridView->setDelegate(&delegateComponent);

    QCOMPARE(gridView->highlight(), &component);
    QCOMPARE(gridView->delegate(), &delegateComponent);

    QCOMPARE(highlightSpy.count(),1);
    QCOMPARE(delegateSpy.count(),1);

    gridView->setHighlight(&component);
    gridView->setDelegate(&delegateComponent);

    QCOMPARE(highlightSpy.count(),1);
    QCOMPARE(delegateSpy.count(),1);
    delete canvas;
}

void tst_QDeclarativeGridView::modelChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QVERIFY(gridView);

    QDeclarativeListModel *alternateModel = canvas->rootObject()->findChild<QDeclarativeListModel*>("alternateModel");
    QVERIFY(alternateModel);
    QVariant modelVariant = QVariant::fromValue(alternateModel);
    QSignalSpy modelSpy(gridView, SIGNAL(modelChanged()));

    gridView->setModel(modelVariant);
    QCOMPARE(gridView->model(), modelVariant);
    QCOMPARE(modelSpy.count(),1);

    gridView->setModel(modelVariant);
    QCOMPARE(modelSpy.count(),1);

    gridView->setModel(QVariant());
    QCOMPARE(modelSpy.count(),2);
    delete canvas;
}

void tst_QDeclarativeGridView::positionViewAtIndex()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 40; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);
    ctxt->setContextProperty("testTopToBottom", QVariant(false));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on a currently visible item
    gridview->positionViewAtIndex(4);
    QCOMPARE(gridview->contentY(), 60.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 3; i < model.count() && i < itemCount-3-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item beyond the visible items
    gridview->positionViewAtIndex(21);
    QCOMPARE(gridview->contentY(), 420.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 22; i < model.count() && i < itemCount-22-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item that would leave empty space if positioned at the top
    gridview->positionViewAtIndex(31);
    QCOMPARE(gridview->contentY(), 520.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 24; i < model.count() && i < itemCount-24-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    // Position at the beginning again
    gridview->positionViewAtIndex(0);
    QCOMPARE(gridview->contentY(), 0.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(viewport, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(viewport, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QVERIFY(item);
        QCOMPARE(item->x(), (i%3)*80.);
        QCOMPARE(item->y(), (i/3)*60.);
    }

    delete canvas;
}

void tst_QDeclarativeGridView::resetModel()
{
    QDeclarativeView *canvas = createView();

    QStringList strings;
    strings << "one" << "two" << "three";
    QStringListModel model(strings);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/displaygrid.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *viewport = gridview->viewport();
    QVERIFY(viewport != 0);

    QCOMPARE(gridview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(viewport, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }

    strings.clear();
    strings << "four" << "five" << "six" << "seven";
    model.setStringList(strings);

    QCOMPARE(gridview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(viewport, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }
}

void tst_QDeclarativeGridView::QTBUG_8456()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/setindex.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QCOMPARE(gridview->currentIndex(), 0);
}

QDeclarativeView *tst_QDeclarativeGridView::createView()
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
T *tst_QDeclarativeGridView::findItem(QGraphicsObject *parent, const QString &objectName, int index)
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
                QDeclarativeContext *context = QDeclarativeEngine::contextForObject(item);
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
QList<T*> tst_QDeclarativeGridView::findItems(QGraphicsObject *parent, const QString &objectName)
{
    QList<T*> items;
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
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

void tst_QDeclarativeGridView::dumpTree(QDeclarativeItem *parent, int depth)
{
    static QString padding("                       ");
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        QDeclarativeContext *context = QDeclarativeEngine::contextForObject(item);
        qDebug() << padding.left(depth*2) << item << (context ? context->contextProperty("index").toInt() : -1);
        dumpTree(item, depth+1);
    }
}


QTEST_MAIN(tst_QDeclarativeGridView)

#include "tst_qdeclarativegridview.moc"
