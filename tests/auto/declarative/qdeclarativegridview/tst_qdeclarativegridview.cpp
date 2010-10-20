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
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

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
    void noCurrentIndex();
    void defaultValues();
    void properties();
    void propertyChanges();
    void componentChanges();
    void modelChanges();
    void positionViewAtIndex();
    void resetModel();
    void enforceRange();
    void QTBUG_8456();
    void manualHighlight();
    void footer();
    void header();

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

    void clear() {
        int count = list.count();
        emit beginRemoveRows(QModelIndex(), 0, count-1);
        list.clear();
        emit endRemoveRows();
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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QTRY_COMPARE(gridview->count(), model.count());
    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    // set an empty model and confirm that items are destroyed
    TestModel model2;
    ctxt->setContextProperty("testModel", &model2);

    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    QTRY_VERIFY(itemCount == 0);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeFlickable *gridview = findItem<QDeclarativeFlickable>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.insertItem(1, "Will", "9876");

    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    // Checks that onAdd is called
    int added = canvas->rootObject()->property("added").toInt();
    QTRY_COMPARE(added, 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        QTRY_COMPARE(item->x(), (i%3)*80.0);
        QTRY_COMPARE(item->y(), (i/3)*60.0);
    }

    model.insertItem(0, "Foo", "1111"); // zero index, and current item

    QTRY_COMPARE(contentItem->childItems().count(), model.count()+1); // assumes all are visible, +1 for the (default) highlight item

    name = findItem<QDeclarativeText>(contentItem, "textName", 0);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(0));
    number = findItem<QDeclarativeText>(contentItem, "textNumber", 0);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(0));

    QTRY_COMPARE(gridview->currentIndex(), 1);

    // Confirm items positioned correctly
    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    for (int i = model.count(); i < 30; ++i)
        model.insertItem(i, "Hello", QString::number(i));

    gridview->setContentY(120);

    // Insert item outside visible area
    model.insertItem(1, "Hello", "1324");

    QTRY_VERIFY(gridview->contentY() == 120);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.removeItem(1);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    // Checks that onRemove is called
    QString removed = canvas->rootObject()->property("removed").toString();
    QTRY_COMPARE(removed, QString("Item1"));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    // Remove first item (which is the current item);
    model.removeItem(0);

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
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    // Remove items not visible
    model.removeItem(25);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    // Remove items before visible
    gridview->setContentY(120);
    gridview->setCurrentIndex(10);

    // Setting currentIndex above shouldn't cause view to scroll
    QTRY_COMPARE(gridview->contentY(), 120.0);

    model.removeItem(1);

    // Confirm items positioned correctly
    for (int i = 6; i < 18; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    // Remove currentIndex
    QDeclarativeItem *oldCurrent = gridview->currentItem();
    model.removeItem(9);

    QTRY_COMPARE(gridview->currentIndex(), 9);
    QTRY_VERIFY(gridview->currentItem() != oldCurrent);

    gridview->setContentY(0);
    // let transitions settle.
    QTest::qWait(100);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    // remove item outside current view.
    gridview->setCurrentIndex(32);
    gridview->setContentY(240);

    model.removeItem(30);
    QTRY_VERIFY(gridview->currentIndex() == 31);

    // remove current item beyond visible items.
    gridview->setCurrentIndex(20);
    gridview->setContentY(0);
    model.removeItem(20);

    QTRY_COMPARE(gridview->currentIndex(), 20);
    QTRY_VERIFY(gridview->currentItem() != 0);

    // remove item before current, but visible
    gridview->setCurrentIndex(8);
    gridview->setContentY(240);
    oldCurrent = gridview->currentItem();
    model.removeItem(6);

    QTRY_COMPARE(gridview->currentIndex(), 7);
    QTRY_VERIFY(gridview->currentItem() == oldCurrent);

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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    model.moveItem(1, 8);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 1);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(1));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 1);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(1));

    name = findItem<QDeclarativeText>(contentItem, "textName", 8);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(8));
    number = findItem<QDeclarativeText>(contentItem, "textNumber", 8);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(8));

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
    }

    gridview->setContentY(120);

    // move outside visible area
    model.moveItem(1, 25);

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count()-1;
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), qreal((i%3)*80));
        QTRY_COMPARE(item->y(), qreal((i/3)*60));
        name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    // move from outside visible into visible
    model.moveItem(28, 8);

    // Confirm items positioned correctly and indexes correct
    for (int i = 6; i < model.count()-6 && i < itemCount+6; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_VERIFY(item->x() == (i%3)*80);
        QTRY_VERIFY(item->y() == (i/3)*60);
        name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    // ensure content position is stable
    gridview->setContentY(0);
    model.moveItem(10, 0);
    QTRY_VERIFY(gridview->contentY() == 0);

    delete canvas;
}

void tst_QDeclarativeGridView::currentIndex()
{
    TestModel model;
    for (int i = 0; i < 60; i++)
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

    QDeclarativeItem *contentItem = gridview->contentItem();
    QVERIFY(contentItem != 0);

    // current item should be third item
    QCOMPARE(gridview->currentIndex(), 35);
    QCOMPARE(gridview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 35));
    QCOMPARE(gridview->currentItem()->y(), gridview->highlightItem()->y());
    QCOMPARE(gridview->contentY(), 400.0);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), 36);
    gridview->moveCurrentIndexDown();
    QCOMPARE(gridview->currentIndex(), 39);
    gridview->moveCurrentIndexUp();
    QCOMPARE(gridview->currentIndex(), 36);
    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), 35);

    // no wrap
    gridview->setCurrentIndex(0);
    QCOMPARE(gridview->currentIndex(), 0);
    // confirm that the velocity is updated
    QTRY_VERIFY(gridview->verticalVelocity() != 0.0);

    gridview->moveCurrentIndexUp();
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->setCurrentIndex(model.count()-1);
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    gridview->moveCurrentIndexDown();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    // with wrap
    gridview->setWrapEnabled(true);

    gridview->setCurrentIndex(0);
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->moveCurrentIndexLeft();
    QCOMPARE(gridview->currentIndex(), model.count()-1);

    QTRY_COMPARE(gridview->contentY(), 880.0);

    gridview->moveCurrentIndexRight();
    QCOMPARE(gridview->currentIndex(), 0);

    QTRY_COMPARE(gridview->contentY(), 0.0);

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
    QCOMPARE(gridview->currentIndex(), 3);

    QTest::keyClick(canvas, Qt::Key_Up);
    QCOMPARE(gridview->currentIndex(), 0);

    gridview->setFlow(QDeclarativeGridView::TopToBottom);

    qApp->setActiveWindow(canvas);
#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(canvas);
#endif
    QTRY_VERIFY(canvas->hasFocus());
    QTRY_VERIFY(canvas->scene()->hasFocus());
    qApp->processEvents();

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
    QTRY_COMPARE(gridview->highlightItem()->x(), hlPosX);
    QTRY_COMPARE(gridview->highlightItem()->y(), hlPosY);

    // insert item before currentIndex
    gridview->setCurrentIndex(28);
    model.insertItem(0, "Foo", "1111");
    QTRY_COMPARE(canvas->rootObject()->property("current").toInt(), 29);

    // check removing highlight by setting currentIndex to -1;
    gridview->setCurrentIndex(-1);

    QCOMPARE(gridview->currentIndex(), -1);
    QVERIFY(!gridview->highlightItem());
    QVERIFY(!gridview->currentItem());

    delete canvas;
}

void tst_QDeclarativeGridView::noCurrentIndex()
{
    TestModel model;
    for (int i = 0; i < 60; i++)
        model.addItem("Item" + QString::number(i), QString::number(i));

    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    QString filename(SRCDIR "/data/gridview-noCurrent.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QVERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QVERIFY(contentItem != 0);

    // current index should be -1
    QCOMPARE(gridview->currentIndex(), -1);
    QVERIFY(!gridview->currentItem());
    QVERIFY(!gridview->highlightItem());
    QCOMPARE(gridview->contentY(), 0.0);

    gridview->setCurrentIndex(5);
    QCOMPARE(gridview->currentIndex(), 5);
    QVERIFY(gridview->currentItem());
    QVERIFY(gridview->highlightItem());
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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly and indexes correct
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), qreal((i%3)*80));
        QTRY_COMPARE(item->y(), qreal((i/3)*60));
        QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    ctxt->setContextProperty("testTopToBottom", QVariant(true));

    // Confirm items positioned correctly and indexes correct
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), qreal((i/5)*80));
        QTRY_COMPARE(item->y(), qreal((i%5)*60));
        QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", i);
        QTRY_VERIFY(name != 0);
        QTRY_COMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", i);
        QTRY_VERIFY(number != 0);
        QTRY_COMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QDeclarativeGridView::defaultValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview3.qml"));
    QDeclarativeGridView *obj = qobject_cast<QDeclarativeGridView*>(c.create());

    QTRY_VERIFY(obj != 0);
    QTRY_VERIFY(obj->model() == QVariant());
    QTRY_VERIFY(obj->delegate() == 0);
    QTRY_COMPARE(obj->currentIndex(), -1);
    QTRY_VERIFY(obj->currentItem() == 0);
    QTRY_COMPARE(obj->count(), 0);
    QTRY_VERIFY(obj->highlight() == 0);
    QTRY_VERIFY(obj->highlightItem() == 0);
    QTRY_COMPARE(obj->highlightFollowsCurrentItem(), true);
    QTRY_VERIFY(obj->flow() == 0);
    QTRY_COMPARE(obj->isWrapEnabled(), false);
    QTRY_COMPARE(obj->cacheBuffer(), 0);
    QTRY_COMPARE(obj->cellWidth(), 100); //### Should 100 be the default?
    QTRY_COMPARE(obj->cellHeight(), 100);
    delete obj;
}

void tst_QDeclarativeGridView::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/gridview2.qml"));
    QDeclarativeGridView *obj = qobject_cast<QDeclarativeGridView*>(c.create());

    QTRY_VERIFY(obj != 0);
    QTRY_VERIFY(obj->model() != QVariant());
    QTRY_VERIFY(obj->delegate() != 0);
    QTRY_COMPARE(obj->currentIndex(), 0);
    QTRY_VERIFY(obj->currentItem() != 0);
    QTRY_COMPARE(obj->count(), 4);
    QTRY_VERIFY(obj->highlight() != 0);
    QTRY_VERIFY(obj->highlightItem() != 0);
    QTRY_COMPARE(obj->highlightFollowsCurrentItem(), false);
    QTRY_VERIFY(obj->flow() == 0);
    QTRY_COMPARE(obj->isWrapEnabled(), true);
    QTRY_COMPARE(obj->cacheBuffer(), 200);
    QTRY_COMPARE(obj->cellWidth(), 100);
    QTRY_COMPARE(obj->cellHeight(), 100);
    delete obj;
}

void tst_QDeclarativeGridView::propertyChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QTRY_VERIFY(gridView);

    QSignalSpy keyNavigationWrapsSpy(gridView, SIGNAL(keyNavigationWrapsChanged()));
    QSignalSpy cacheBufferSpy(gridView, SIGNAL(cacheBufferChanged()));
    QSignalSpy flowSpy(gridView, SIGNAL(flowChanged()));

    QTRY_COMPARE(gridView->isWrapEnabled(), true);
    QTRY_COMPARE(gridView->cacheBuffer(), 10);
    QTRY_COMPARE(gridView->flow(), QDeclarativeGridView::LeftToRight);

    gridView->setWrapEnabled(false);
    gridView->setCacheBuffer(3);
    gridView->setFlow(QDeclarativeGridView::TopToBottom);

    QTRY_COMPARE(gridView->isWrapEnabled(), false);
    QTRY_COMPARE(gridView->cacheBuffer(), 3);
    QTRY_COMPARE(gridView->flow(), QDeclarativeGridView::TopToBottom);

    QTRY_COMPARE(keyNavigationWrapsSpy.count(),1);
    QTRY_COMPARE(cacheBufferSpy.count(),1);
    QTRY_COMPARE(flowSpy.count(),1);

    gridView->setWrapEnabled(false);
    gridView->setCacheBuffer(3);
    gridView->setFlow(QDeclarativeGridView::TopToBottom);

    QTRY_COMPARE(keyNavigationWrapsSpy.count(),1);
    QTRY_COMPARE(cacheBufferSpy.count(),1);
    QTRY_COMPARE(flowSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeGridView::componentChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QTRY_VERIFY(gridView);

    QDeclarativeComponent component(canvas->engine());
    component.setData("import QtQuick 1.0; Rectangle { color: \"blue\"; }", QUrl::fromLocalFile(""));

    QDeclarativeComponent delegateComponent(canvas->engine());
    delegateComponent.setData("import QtQuick 1.0; Text { text: '<b>Name:</b> ' + name }", QUrl::fromLocalFile(""));

    QSignalSpy highlightSpy(gridView, SIGNAL(highlightChanged()));
    QSignalSpy delegateSpy(gridView, SIGNAL(delegateChanged()));
    QSignalSpy headerSpy(gridView, SIGNAL(headerChanged()));
    QSignalSpy footerSpy(gridView, SIGNAL(footerChanged()));

    gridView->setHighlight(&component);
    gridView->setDelegate(&delegateComponent);
    gridView->setHeader(&component);
    gridView->setFooter(&component);

    QTRY_COMPARE(gridView->highlight(), &component);
    QTRY_COMPARE(gridView->delegate(), &delegateComponent);
    QTRY_COMPARE(gridView->header(), &component);
    QTRY_COMPARE(gridView->footer(), &component);

    QTRY_COMPARE(highlightSpy.count(),1);
    QTRY_COMPARE(delegateSpy.count(),1);
    QTRY_COMPARE(headerSpy.count(),1);
    QTRY_COMPARE(footerSpy.count(),1);

    gridView->setHighlight(&component);
    gridView->setDelegate(&delegateComponent);
    gridView->setHeader(&component);
    gridView->setFooter(&component);

    QTRY_COMPARE(highlightSpy.count(),1);
    QTRY_COMPARE(delegateSpy.count(),1);
    QTRY_COMPARE(headerSpy.count(),1);
    QTRY_COMPARE(footerSpy.count(),1);

    delete canvas;
}

void tst_QDeclarativeGridView::modelChanges()
{
    QDeclarativeView *canvas = createView();
    QTRY_VERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychangestest.qml"));

    QDeclarativeGridView *gridView = canvas->rootObject()->findChild<QDeclarativeGridView*>("gridView");
    QTRY_VERIFY(gridView);

    QDeclarativeListModel *alternateModel = canvas->rootObject()->findChild<QDeclarativeListModel*>("alternateModel");
    QTRY_VERIFY(alternateModel);
    QVariant modelVariant = QVariant::fromValue(alternateModel);
    QSignalSpy modelSpy(gridView, SIGNAL(modelChanged()));

    gridView->setModel(modelVariant);
    QTRY_COMPARE(gridView->model(), modelVariant);
    QTRY_COMPARE(modelSpy.count(),1);

    gridView->setModel(modelVariant);
    QTRY_COMPARE(modelSpy.count(),1);

    gridView->setModel(QVariant());
    QTRY_COMPARE(modelSpy.count(),2);
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

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview1.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // Confirm items positioned correctly
    int itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), (i%3)*80.);
        QTRY_COMPARE(item->y(), (i/3)*60.);
    }

    // Position on a currently visible item
    gridview->positionViewAtIndex(4, QDeclarativeGridView::Beginning);
    QTRY_COMPARE(gridview->contentY(), 60.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 3; i < model.count() && i < itemCount-3-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), (i%3)*80.);
        QTRY_COMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item beyond the visible items
    gridview->positionViewAtIndex(21, QDeclarativeGridView::Beginning);
    QTRY_COMPARE(gridview->contentY(), 420.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 22; i < model.count() && i < itemCount-22-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), (i%3)*80.);
        QTRY_COMPARE(item->y(), (i/3)*60.);
    }

    // Position on an item that would leave empty space if positioned at the top
    gridview->positionViewAtIndex(31, QDeclarativeGridView::Beginning);
    QTRY_COMPARE(gridview->contentY(), 520.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 24; i < model.count() && i < itemCount-24-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), (i%3)*80.);
        QTRY_COMPARE(item->y(), (i/3)*60.);
    }

    // Position at the beginning again
    gridview->positionViewAtIndex(0, QDeclarativeGridView::Beginning);
    QTRY_COMPARE(gridview->contentY(), 0.);

    // Confirm items positioned correctly
    itemCount = findItems<QDeclarativeItem>(contentItem, "wrapper").count();
    for (int i = 0; i < model.count() && i < itemCount-1; ++i) {
        QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", i);
        if (!item) qWarning() << "Item" << i << "not found";
        QTRY_VERIFY(item);
        QTRY_COMPARE(item->x(), (i%3)*80.);
        QTRY_COMPARE(item->y(), (i/3)*60.);
    }

    // Position at End
    gridview->positionViewAtIndex(30, QDeclarativeGridView::End);
    QTRY_COMPARE(gridview->contentY(), 340.);

    // Position in Center
    gridview->positionViewAtIndex(15, QDeclarativeGridView::Center);
    QTRY_COMPARE(gridview->contentY(), 170.);

    // Ensure at least partially visible
    gridview->positionViewAtIndex(15, QDeclarativeGridView::Visible);
    QTRY_COMPARE(gridview->contentY(), 170.);

    gridview->setContentY(302);
    gridview->positionViewAtIndex(15, QDeclarativeGridView::Visible);
    QTRY_COMPARE(gridview->contentY(), 302.);

    gridview->setContentY(360);
    gridview->positionViewAtIndex(15, QDeclarativeGridView::Visible);
    QTRY_COMPARE(gridview->contentY(), 300.);

    gridview->setContentY(60);
    gridview->positionViewAtIndex(20, QDeclarativeGridView::Visible);
    QTRY_COMPARE(gridview->contentY(), 60.);

    gridview->setContentY(20);
    gridview->positionViewAtIndex(20, QDeclarativeGridView::Visible);
    QTRY_COMPARE(gridview->contentY(), 100.);

    // Ensure completely visible
    gridview->setContentY(120);
    gridview->positionViewAtIndex(20, QDeclarativeGridView::Contain);
    QTRY_COMPARE(gridview->contentY(), 120.);

    gridview->setContentY(302);
    gridview->positionViewAtIndex(15, QDeclarativeGridView::Contain);
    QTRY_COMPARE(gridview->contentY(), 300.);

    gridview->setContentY(60);
    gridview->positionViewAtIndex(20, QDeclarativeGridView::Contain);
    QTRY_COMPARE(gridview->contentY(), 100.);

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
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QTRY_COMPARE(gridview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(contentItem, "displayText", i);
        QTRY_VERIFY(display != 0);
        QTRY_COMPARE(display->text(), strings.at(i));
    }

    strings.clear();
    strings << "four" << "five" << "six" << "seven";
    model.setStringList(strings);

    QTRY_COMPARE(gridview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(contentItem, "displayText", i);
        QTRY_VERIFY(display != 0);
        QTRY_COMPARE(display->text(), strings.at(i));
    }
}

void tst_QDeclarativeGridView::enforceRange()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 30; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/gridview-enforcerange.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QTRY_COMPARE(gridview->preferredHighlightBegin(), 100.0);
    QTRY_COMPARE(gridview->preferredHighlightEnd(), 100.0);
    QTRY_COMPARE(gridview->highlightRangeMode(), QDeclarativeGridView::StrictlyEnforceRange);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    // view should be positioned at the top of the range.
    QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", 0);
    QTRY_VERIFY(item);
    QTRY_COMPARE(gridview->contentY(), -100.0);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "textName", 0);
    QTRY_VERIFY(name != 0);
    QTRY_COMPARE(name->text(), model.name(0));
    QDeclarativeText *number = findItem<QDeclarativeText>(contentItem, "textNumber", 0);
    QTRY_VERIFY(number != 0);
    QTRY_COMPARE(number->text(), model.number(0));

    // Check currentIndex is updated when contentItem moves
    gridview->setContentY(0);
    QTRY_COMPARE(gridview->currentIndex(), 2);

    gridview->setCurrentIndex(5);
    QTRY_COMPARE(gridview->contentY(), 100.);

    TestModel model2;
    for (int i = 0; i < 5; i++)
        model2.addItem("Item" + QString::number(i), "");

    ctxt->setContextProperty("testModel", &model2);
    QCOMPARE(gridview->count(), 5);

    delete canvas;
}

void tst_QDeclarativeGridView::QTBUG_8456()
{
    QDeclarativeView *canvas = createView();

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/setindex.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QTRY_COMPARE(gridview->currentIndex(), 0);
}

void tst_QDeclarativeGridView::manualHighlight()
{
    QDeclarativeView *canvas = createView();

    QString filename(SRCDIR "/data/manual-highlight.qml");
    canvas->setSource(QUrl::fromLocalFile(filename));

    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QTRY_COMPARE(gridview->currentIndex(), 0);
    QTRY_COMPARE(gridview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 0));
    QTRY_COMPARE(gridview->highlightItem()->y(), gridview->currentItem()->y());
    QTRY_COMPARE(gridview->highlightItem()->x(), gridview->currentItem()->x());

    gridview->setCurrentIndex(2);

    QTRY_COMPARE(gridview->currentIndex(), 2);
    QTRY_COMPARE(gridview->currentItem(), findItem<QDeclarativeItem>(contentItem, "wrapper", 2));
    QTRY_COMPARE(gridview->highlightItem()->y(), gridview->currentItem()->y());
    QTRY_COMPARE(gridview->highlightItem()->x(), gridview->currentItem()->x());
}

void tst_QDeclarativeGridView::footer()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 7; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/footer.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QDeclarativeText *footer = findItem<QDeclarativeText>(contentItem, "footer");
    QVERIFY(footer);

    QCOMPARE(footer->y(), 180.0);

    model.removeItem(2);
    QTRY_COMPARE(footer->y(), 120.0);

    model.clear();
    QTRY_COMPARE(footer->y(), 0.0);
}

void tst_QDeclarativeGridView::header()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    for (int i = 0; i < 7; i++)
        model.addItem("Item" + QString::number(i), "");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/header.qml"));
    qApp->processEvents();

    QDeclarativeGridView *gridview = findItem<QDeclarativeGridView>(canvas->rootObject(), "grid");
    QTRY_VERIFY(gridview != 0);

    QDeclarativeItem *contentItem = gridview->contentItem();
    QTRY_VERIFY(contentItem != 0);

    QDeclarativeText *header = findItem<QDeclarativeText>(contentItem, "header");
    QVERIFY(header);

    QCOMPARE(header->y(), 0.0);
    QCOMPARE(gridview->contentY(), 0.0);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(contentItem, "wrapper", 0);
    QVERIFY(item);
    QCOMPARE(item->y(), 30.0);

    model.clear();
    QTRY_COMPARE(header->y(), 0.0);
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
