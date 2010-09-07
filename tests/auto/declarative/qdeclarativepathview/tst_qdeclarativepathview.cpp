/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/private/qdeclarativepathview_p.h>
#include <QtDeclarative/private/qdeclarativepath_p.h>
#include <QtDeclarative/private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativerectangle_p.h>
#include <QtDeclarative/private/qdeclarativelistmodel_p.h>
#include <QtDeclarative/private/qdeclarativevaluetype_p.h>
#include <QAbstractListModel>
#include <QStringListModel>
#include <QFile>

#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QDeclarativePathView : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativePathView();

private slots:
    void initValues();
    void items();
    void dataModel();
    void pathview2();
    void pathview3();
    void path();
    void pathMoved();
    void setCurrentIndex();
    void resetModel();
    void propertyChanges();
    void pathChanges();
    void componentChanges();
    void modelChanges();
    void pathUpdateOnStartChanged();
    void package();


private:
    QDeclarativeView *createView();
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &objectName, int index=-1);
    template<typename T>
    QList<T*> findItems(QGraphicsObject *parent, const QString &objectName);
};

class TestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool error READ error WRITE setError)
    Q_PROPERTY(bool useModel READ useModel NOTIFY useModelChanged)
    Q_PROPERTY(int pathItemCount READ pathItemCount NOTIFY pathItemCountChanged)

public:
    TestObject() : QObject(), mError(true), mUseModel(true), mPathItemCount(-1) {}

    bool error() const { return mError; }
    void setError(bool err) { mError = err; }

    bool useModel() const { return mUseModel; }
    void setUseModel(bool use) { mUseModel = use; emit useModelChanged(); }

    int pathItemCount() const { return mPathItemCount; }
    void setPathItemCount(int count) { mPathItemCount = count; emit pathItemCountChanged(); }

signals:
    void useModelChanged();
    void pathItemCountChanged();

private:
    bool mError;
    bool mUseModel;
    int mPathItemCount;
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


tst_QDeclarativePathView::tst_QDeclarativePathView()
{
}

void tst_QDeclarativePathView::initValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview1.qml"));
    QDeclarativePathView *obj = qobject_cast<QDeclarativePathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() == 0);
    QVERIFY(obj->delegate() == 0);
    QCOMPARE(obj->model(), QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->preferredHighlightBegin(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 0);
    QCOMPARE(obj->pathItemCount(), -1);
}

void tst_QDeclarativePathView::items()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Bill", "4321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pathview0.qml"));
    qApp->processEvents();

    QDeclarativePathView *pathview = findItem<QDeclarativePathView>(canvas->rootObject(), "view");
    QVERIFY(pathview != 0);

    QCOMPARE(pathview->childItems().count(), model.count()+1); // assumes all are visible, including highlight

    for (int i = 0; i < model.count(); ++i) {
        QDeclarativeText *name = findItem<QDeclarativeText>(pathview, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QDeclarativeText *number = findItem<QDeclarativeText>(pathview, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    QDeclarativePath *path = qobject_cast<QDeclarativePath*>(pathview->path());
    QVERIFY(path);

    QVERIFY(pathview->highlightItem());
    QPointF start = path->pointAt(0.0);
    QPointF offset;
    offset.setX(pathview->highlightItem()->width()/2);
    offset.setY(pathview->highlightItem()->height()/2);
    QCOMPARE(pathview->highlightItem()->pos() + offset, start);

    delete canvas;
}

void tst_QDeclarativePathView::pathview2()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview2.qml"));
    QDeclarativePathView *obj = qobject_cast<QDeclarativePathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->preferredHighlightBegin(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 10);
}

void tst_QDeclarativePathView::pathview3()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview3.qml"));
    QDeclarativePathView *obj = qobject_cast<QDeclarativePathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 1.0);
    QCOMPARE(obj->preferredHighlightBegin(), 0.5);
    QCOMPARE(obj->dragMargin(), 24.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 4);
}

void tst_QDeclarativePathView::path()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathtest.qml"));
    QDeclarativePath *obj = qobject_cast<QDeclarativePath*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->startX(), 120.);
    QCOMPARE(obj->startY(), 100.);
    QVERIFY(obj->path() != QPainterPath());

    QDeclarativeListReference list(obj, "pathElements");
    QCOMPARE(list.count(), 5);

    QDeclarativePathAttribute* attr = qobject_cast<QDeclarativePathAttribute*>(list.at(0));
    QVERIFY(attr != 0);
    QCOMPARE(attr->name(), QString("scale"));
    QCOMPARE(attr->value(), 1.0);

    QDeclarativePathQuad* quad = qobject_cast<QDeclarativePathQuad*>(list.at(1));
    QVERIFY(quad != 0);
    QCOMPARE(quad->x(), 120.);
    QCOMPARE(quad->y(), 25.);
    QCOMPARE(quad->controlX(), 260.);
    QCOMPARE(quad->controlY(), 75.);

    QDeclarativePathPercent* perc = qobject_cast<QDeclarativePathPercent*>(list.at(2));
    QVERIFY(perc != 0);
    QCOMPARE(perc->value(), 0.3);

    QDeclarativePathLine* line = qobject_cast<QDeclarativePathLine*>(list.at(3));
    QVERIFY(line != 0);
    QCOMPARE(line->x(), 120.);
    QCOMPARE(line->y(), 100.);

    QDeclarativePathCubic* cubic = qobject_cast<QDeclarativePathCubic*>(list.at(4));
    QVERIFY(cubic != 0);
    QCOMPARE(cubic->x(), 180.);
    QCOMPARE(cubic->y(), 0.);
    QCOMPARE(cubic->control1X(), -10.);
    QCOMPARE(cubic->control1Y(), 90.);
    QCOMPARE(cubic->control2X(), 210.);
    QCOMPARE(cubic->control2Y(), 90.);
}

void tst_QDeclarativePathView::dataModel()
{
    QDeclarativeView *canvas = createView();

    QDeclarativeContext *ctxt = canvas->rootContext();
    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    TestModel model;
    model.addItem("red", "1");
    model.addItem("green", "2");
    model.addItem("blue", "3");
    model.addItem("purple", "4");
    model.addItem("gray", "5");
    model.addItem("brown", "6");
    model.addItem("yellow", "7");
    model.addItem("thistle", "8");
    model.addItem("cyan", "9");

    ctxt->setContextProperty("testData", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/datamodel.qml"));
    qApp->processEvents();

    QDeclarativePathView *pathview = qobject_cast<QDeclarativePathView*>(canvas->rootObject());
    QVERIFY(pathview != 0);

    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(pathview, "wrapper", 0);
    QVERIFY(item);
    QCOMPARE(item->x(), 110.0);
    QCOMPARE(item->y(), 10.0);

    model.insertItem(4, "orange", "10");

    int itemCount = findItems<QDeclarativeItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 10);

    QDeclarativeText *text = findItem<QDeclarativeText>(pathview, "myText", 4);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(4));

    model.removeItem(2);
    text = findItem<QDeclarativeText>(pathview, "myText", 2);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(2));

    testObject->setPathItemCount(5);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);

    itemCount = findItems<QDeclarativeItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);

    QDeclarativeRectangle *testItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 4);
    QVERIFY(testItem != 0);
    testItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 5);
    QVERIFY(testItem == 0);

    model.insertItem(2, "pink", "2");

    itemCount = findItems<QDeclarativeItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);

    text = findItem<QDeclarativeText>(pathview, "myText", 2);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(2));

    model.removeItem(3);
    itemCount = findItems<QDeclarativeItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);
    text = findItem<QDeclarativeText>(pathview, "myText", 3);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(3));

    delete canvas;
}

void tst_QDeclarativePathView::pathMoved()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Ben", "12345");
    model.addItem("Bohn", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Bill", "4321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pathview0.qml"));
    qApp->processEvents();

    QDeclarativePathView *pathview = findItem<QDeclarativePathView>(canvas->rootObject(), "view");
    QVERIFY(pathview != 0);

    QDeclarativeRectangle *firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QDeclarativePath *path = qobject_cast<QDeclarativePath*>(pathview->path());
    QVERIFY(path);
    QPointF start = path->pointAt(0.0);
    QPointF offset;//Center of item is at point, but pos is from corner
    offset.setX(firstItem->width()/2);
    offset.setY(firstItem->height()/2);
    QCOMPARE(firstItem->pos() + offset, start);
    pathview->setOffset(1.0);

    for(int i=0; i<model.count(); i++){
        QDeclarativeRectangle *curItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", i);
        QPointF itemPos(path->pointAt(0.25 + i*0.25));
        QCOMPARE(curItem->pos() + offset, QPointF(qRound(itemPos.x()), qRound(itemPos.y())));
    }

    pathview->setOffset(0.0);
    QCOMPARE(firstItem->pos() + offset, start);

    // Change delegate size
    pathview->setOffset(0.1);
    pathview->setOffset(0.0);
    canvas->rootObject()->setProperty("delegateWidth", 30);
    QCOMPARE(firstItem->width(), 30.0);
    offset.setX(firstItem->width()/2);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    // Change delegate scale
    pathview->setOffset(0.1);
    pathview->setOffset(0.0);
    canvas->rootObject()->setProperty("delegateScale", 1.2);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    delete canvas;
}

void tst_QDeclarativePathView::setCurrentIndex()
{
    QDeclarativeView *canvas = createView();

    TestModel model;
    model.addItem("Ben", "12345");
    model.addItem("Bohn", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Bill", "4321");

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pathview0.qml"));
    qApp->processEvents();

    QDeclarativePathView *pathview = findItem<QDeclarativePathView>(canvas->rootObject(), "view");
    QVERIFY(pathview != 0);

    QDeclarativeRectangle *firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QDeclarativePath *path = qobject_cast<QDeclarativePath*>(pathview->path());
    QVERIFY(path);
    QPointF start = path->pointAt(0.0);
    QPointF offset;//Center of item is at point, but pos is from corner
    offset.setX(firstItem->width()/2);
    offset.setY(firstItem->height()/2);
    QCOMPARE(firstItem->pos() + offset, start);
    QCOMPARE(canvas->rootObject()->property("currentA").toInt(), 0);
    QCOMPARE(canvas->rootObject()->property("currentB").toInt(), 0);

    pathview->setCurrentIndex(2);

    firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 2);
    QTRY_COMPARE(firstItem->pos() + offset, start);
    QCOMPARE(canvas->rootObject()->property("currentA").toInt(), 2);
    QCOMPARE(canvas->rootObject()->property("currentB").toInt(), 2);

    pathview->decrementCurrentIndex();
    QTRY_COMPARE(pathview->currentIndex(), 1);
    firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 1);
    QVERIFY(firstItem);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    pathview->decrementCurrentIndex();
    QTRY_COMPARE(pathview->currentIndex(), 0);
    firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    pathview->decrementCurrentIndex();
    QTRY_COMPARE(pathview->currentIndex(), 3);
    firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 3);
    QVERIFY(firstItem);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    pathview->incrementCurrentIndex();
    QTRY_COMPARE(pathview->currentIndex(), 0);
    firstItem = findItem<QDeclarativeRectangle>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QTRY_COMPARE(firstItem->pos() + offset, start);

    delete canvas;
}

void tst_QDeclarativePathView::resetModel()
{
    QDeclarativeView *canvas = createView();

    QStringList strings;
    strings << "one" << "two" << "three";
    QStringListModel model(strings);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/displaypath.qml"));
    qApp->processEvents();

    QDeclarativePathView *pathview = findItem<QDeclarativePathView>(canvas->rootObject(), "view");
    QVERIFY(pathview != 0);

    QCOMPARE(pathview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(pathview, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }

    strings.clear();
    strings << "four" << "five" << "six" << "seven";
    model.setStringList(strings);

    QCOMPARE(pathview->count(), model.rowCount());

    for (int i = 0; i < model.rowCount(); ++i) {
        QDeclarativeText *display = findItem<QDeclarativeText>(pathview, "displayText", i);
        QVERIFY(display != 0);
        QCOMPARE(display->text(), strings.at(i));
    }
}

void tst_QDeclarativePathView::propertyChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("pathView");
    QVERIFY(pathView);

    QSignalSpy snapPositionSpy(pathView, SIGNAL(preferredHighlightBeginChanged()));
    QSignalSpy dragMarginSpy(pathView, SIGNAL(dragMarginChanged()));

    QCOMPARE(pathView->preferredHighlightBegin(), 0.1);
    QCOMPARE(pathView->dragMargin(), 5.0);

    pathView->setPreferredHighlightBegin(0.4);
    pathView->setPreferredHighlightEnd(0.4);
    pathView->setDragMargin(20.0);

    QCOMPARE(pathView->preferredHighlightBegin(), 0.4);
    QCOMPARE(pathView->preferredHighlightEnd(), 0.4);
    QCOMPARE(pathView->dragMargin(), 20.0);

    QCOMPARE(snapPositionSpy.count(), 1);
    QCOMPARE(dragMarginSpy.count(), 1);

    pathView->setPreferredHighlightBegin(0.4);
    pathView->setPreferredHighlightEnd(0.4);
    pathView->setDragMargin(20.0);

    QCOMPARE(snapPositionSpy.count(), 1);
    QCOMPARE(dragMarginSpy.count(), 1);
    delete canvas;
}

void tst_QDeclarativePathView::pathChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("pathView");
    QVERIFY(pathView);

    QDeclarativePath *path = canvas->rootObject()->findChild<QDeclarativePath*>("path");
    QVERIFY(path);

    QSignalSpy startXSpy(path, SIGNAL(startXChanged()));
    QSignalSpy startYSpy(path, SIGNAL(startYChanged()));

    QCOMPARE(path->startX(), 220.0);
    QCOMPARE(path->startY(), 200.0);

    path->setStartX(240.0);
    path->setStartY(220.0);

    QCOMPARE(path->startX(), 240.0);
    QCOMPARE(path->startY(), 220.0);

    QCOMPARE(startXSpy.count(),1);
    QCOMPARE(startYSpy.count(),1);

    path->setStartX(240);
    path->setStartY(220);

    QCOMPARE(startXSpy.count(),1);
    QCOMPARE(startYSpy.count(),1);

    QDeclarativePath *alternatePath = canvas->rootObject()->findChild<QDeclarativePath*>("alternatePath");
    QVERIFY(alternatePath);

    QSignalSpy pathSpy(pathView, SIGNAL(pathChanged()));

    QCOMPARE(pathView->path(), path);

    pathView->setPath(alternatePath);
    QCOMPARE(pathView->path(), alternatePath);
    QCOMPARE(pathSpy.count(),1);

    pathView->setPath(alternatePath);
    QCOMPARE(pathSpy.count(),1);

    QDeclarativePathAttribute *pathAttribute = canvas->rootObject()->findChild<QDeclarativePathAttribute*>("pathAttribute");
    QVERIFY(pathAttribute);

    QSignalSpy nameSpy(pathAttribute, SIGNAL(nameChanged()));
    QCOMPARE(pathAttribute->name(), QString("opacity"));

    pathAttribute->setName("scale");
    QCOMPARE(pathAttribute->name(), QString("scale"));
    QCOMPARE(nameSpy.count(),1);

    pathAttribute->setName("scale");
    QCOMPARE(nameSpy.count(),1);
    delete canvas;
}

void tst_QDeclarativePathView::componentChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("pathView");
    QVERIFY(pathView);

    QDeclarativeComponent delegateComponent(canvas->engine());
    delegateComponent.setData("import Qt 4.7; Text { text: '<b>Name:</b> ' + name }", QUrl::fromLocalFile(""));

    QSignalSpy delegateSpy(pathView, SIGNAL(delegateChanged()));

    pathView->setDelegate(&delegateComponent);
    QCOMPARE(pathView->delegate(), &delegateComponent);
    QCOMPARE(delegateSpy.count(),1);

    pathView->setDelegate(&delegateComponent);
    QCOMPARE(delegateSpy.count(),1);
    delete canvas;
}

void tst_QDeclarativePathView::modelChanges()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/propertychanges.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("pathView");
    QVERIFY(pathView);

    QDeclarativeListModel *alternateModel = canvas->rootObject()->findChild<QDeclarativeListModel*>("alternateModel");
    QVERIFY(alternateModel);
    QVariant modelVariant = QVariant::fromValue(alternateModel);
    QSignalSpy modelSpy(pathView, SIGNAL(modelChanged()));

    pathView->setModel(modelVariant);
    QCOMPARE(pathView->model(), modelVariant);
    QCOMPARE(modelSpy.count(),1);

    pathView->setModel(modelVariant);
    QCOMPARE(modelSpy.count(),1);

    pathView->setModel(QVariant());
    QCOMPARE(modelSpy.count(),2);

    delete canvas;
}

void tst_QDeclarativePathView::pathUpdateOnStartChanged()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pathUpdateOnStartChanged.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("pathView");
    QVERIFY(pathView);

    QDeclarativePath *path = canvas->rootObject()->findChild<QDeclarativePath*>("path");
    QVERIFY(path);
    QCOMPARE(path->startX(), 400.0);
    QCOMPARE(path->startY(), 300.0);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(pathView, "wrapper", 0);
    QVERIFY(item);
    QCOMPARE(item->x(), path->startX() - item->width() / 2.0);
    QCOMPARE(item->y(), path->startY() - item->height() / 2.0);

    delete canvas;
}

void tst_QDeclarativePathView::package()
{
    QDeclarativeView *canvas = createView();
    QVERIFY(canvas);
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pathview_package.qml"));

    QDeclarativePathView *pathView = canvas->rootObject()->findChild<QDeclarativePathView*>("photoPathView");
    QVERIFY(pathView);

    QDeclarativeItem *item = findItem<QDeclarativeItem>(pathView, "pathItem");
    QVERIFY(item);
    QVERIFY(item->scale() != 1.0);

    delete canvas;
}

QDeclarativeView *tst_QDeclarativePathView::createView()
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
T *tst_QDeclarativePathView::findItem(QGraphicsObject *parent, const QString &objectName, int index)
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
QList<T*> tst_QDeclarativePathView::findItems(QGraphicsObject *parent, const QString &objectName)
{
    QList<T*> items;
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->QGraphicsObject::children().count() << "children";
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

QTEST_MAIN(tst_QDeclarativePathView)

#include "tst_qdeclarativepathview.moc"
