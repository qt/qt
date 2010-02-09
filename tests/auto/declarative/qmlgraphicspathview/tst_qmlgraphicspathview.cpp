/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <private/qmlgraphicspathview_p.h>
#include <private/qmlgraphicspath_p.h>
#include <qmlcontext.h>
#include <qmlexpression.h>
#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <QtDeclarative/private/qmlgraphicstext_p.h>
#include <QAbstractListModel>
#include <QFile>
#include <private/qmlvaluetype_p.h>
#include "../../../shared/util.h"

class tst_QmlGraphicsPathView : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsPathView();

private slots:
    void initValues();
    void dataModel();
    void pathview2();
    void pathview3();
    void path();

private:
    QmlView *createView(const QString &filename);
    template<typename T>
    T *findItem(QmlGraphicsItem *parent, const QString &objectName, int index=-1);
    template<typename T>
    QList<T*> findItems(QmlGraphicsItem *parent, const QString &objectName);
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


tst_QmlGraphicsPathView::tst_QmlGraphicsPathView()
{
}

void tst_QmlGraphicsPathView::initValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview1.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() == 0);
    QVERIFY(obj->delegate() == 0);
    QCOMPARE(obj->model(), QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->snapPosition(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 0);
    QCOMPARE(obj->pathItemCount(), -1);
}

void tst_QmlGraphicsPathView::pathview2()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview2.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->snapPosition(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 10);
}

void tst_QmlGraphicsPathView::pathview3()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/pathview3.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 50.); // ???
    QCOMPARE(obj->snapPosition(), 0.5); // ???
    QCOMPARE(obj->dragMargin(), 24.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 4);
}

void tst_QmlGraphicsPathView::path()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/path.qml"));
    QmlGraphicsPath *obj = qobject_cast<QmlGraphicsPath*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->startX(), 120.);
    QCOMPARE(obj->startY(), 100.);
    QVERIFY(obj->path() != QPainterPath());

    QList<QmlGraphicsPathElement*> *list = static_cast<QList<QmlGraphicsPathElement*> *>(obj->pathElements().data);
    QCOMPARE(list->count(), 5);

    QmlGraphicsPathAttribute* attr = qobject_cast<QmlGraphicsPathAttribute*>(list->at(0));
    QVERIFY(attr != 0);
    QCOMPARE(attr->name(), QString("scale"));
    QCOMPARE(attr->value(), 1.0);

    QmlGraphicsPathQuad* quad = qobject_cast<QmlGraphicsPathQuad*>(list->at(1));
    QVERIFY(quad != 0);
    QCOMPARE(quad->x(), 120.);
    QCOMPARE(quad->y(), 25.);
    QCOMPARE(quad->controlX(), 260.);
    QCOMPARE(quad->controlY(), 75.);

    QmlGraphicsPathPercent* perc = qobject_cast<QmlGraphicsPathPercent*>(list->at(2));
    QVERIFY(perc != 0);
    QCOMPARE(perc->value(), 0.3);

    QmlGraphicsPathLine* line = qobject_cast<QmlGraphicsPathLine*>(list->at(3));
    QVERIFY(line != 0);
    QCOMPARE(line->x(), 120.);
    QCOMPARE(line->y(), 100.);

    QmlGraphicsPathCubic* cubic = qobject_cast<QmlGraphicsPathCubic*>(list->at(4));
    QVERIFY(cubic != 0);
    QCOMPARE(cubic->x(), 180.);
    QCOMPARE(cubic->y(), 0.);
    QCOMPARE(cubic->control1X(), -10.);
    QCOMPARE(cubic->control1Y(), 90.);
    QCOMPARE(cubic->control2X(), 210.);
    QCOMPARE(cubic->control2Y(), 90.);
}

void tst_QmlGraphicsPathView::dataModel()
{
    QmlView *canvas = createView(SRCDIR "/data/datamodel.qml");

    QmlContext *ctxt = canvas->rootContext();
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

    canvas->execute();
    qApp->processEvents();

    QmlGraphicsPathView *pathview = qobject_cast<QmlGraphicsPathView*>(canvas->root());
    QVERIFY(pathview != 0);

    QMetaObject::invokeMethod(canvas->root(), "checkProperties");
    QVERIFY(testObject->error() == false);

    QmlGraphicsItem *item = findItem<QmlGraphicsItem>(pathview, "wrapper", 0);
    QVERIFY(item);
    QCOMPARE(item->x(), 110.0);
    QCOMPARE(item->y(), 10.0);

    model.insertItem(4, "orange", "10");

    int itemCount = findItems<QmlGraphicsItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 10);

    QmlGraphicsText *text = findItem<QmlGraphicsText>(pathview, "myText", 4);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(4));

    model.removeItem(2);
    text = findItem<QmlGraphicsText>(pathview, "myText", 2);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(2));

    testObject->setPathItemCount(5);
    QMetaObject::invokeMethod(canvas->root(), "checkProperties");
    QVERIFY(testObject->error() == false);

    itemCount = findItems<QmlGraphicsItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);

    model.insertItem(2, "pink", "2");

    itemCount = findItems<QmlGraphicsItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);

    text = findItem<QmlGraphicsText>(pathview, "myText", 2);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(2));

    model.removeItem(3);
    itemCount = findItems<QmlGraphicsItem>(pathview, "wrapper").count();
    QCOMPARE(itemCount, 5);
    text = findItem<QmlGraphicsText>(pathview, "myText", 3);
    QVERIFY(text);
    QCOMPARE(text->text(), model.name(3));

    delete canvas;
}

QmlView *tst_QmlGraphicsPathView::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
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
T *tst_QmlGraphicsPathView::findItem(QmlGraphicsItem *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->QGraphicsObject::children().count() << "children";
    for (int i = 0; i < parent->QGraphicsObject::children().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->QGraphicsObject::children().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
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

template<typename T>
QList<T*> tst_QmlGraphicsPathView::findItems(QmlGraphicsItem *parent, const QString &objectName)
{
    QList<T*> items;
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->QGraphicsObject::children().count() << "children";
    for (int i = 0; i < parent->QGraphicsObject::children().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->QGraphicsObject::children().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName))
            items.append(static_cast<T*>(item));
        items += findItems<T>(item, objectName);
    }

    return items;
}

QTEST_MAIN(tst_QmlGraphicsPathView)

#include "tst_qmlgraphicspathview.moc"
