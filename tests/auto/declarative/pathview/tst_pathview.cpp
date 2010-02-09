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
#include <QtTest/QtTest>
#include <private/qlistmodelinterface_p.h>
#include <qmlview.h>
#include <private/qmlgraphicspathview_p.h>
#include <private/qmlgraphicstext_p.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <qmlcontext.h>
#include <qmlexpression.h>

class tst_QmlGraphicsPathView : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsPathView();

private slots:
    void items();
    void pathMoved();
    void limitedItems();

private:
    QmlView *createView(const QString &filename);
    template<typename T>
    T *findItem(QmlGraphicsItem *parent, const QString &id, int index=-1);
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

    QVariant data(int index, int role) const {
        if (role == 0)
            return list.at(index).first;
        if (role == 1)
            return list.at(index).second;
        return QVariant();
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

tst_QmlGraphicsPathView::tst_QmlGraphicsPathView()
{
}

void tst_QmlGraphicsPathView::items()
{
    QmlView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    model.addItem("Fred", "12345");
    model.addItem("John", "2345");
    model.addItem("Bob", "54321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QmlGraphicsPathView *pathview = findItem<QmlGraphicsPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    QCOMPARE(pathview->childItems().count(), model.count()); // assumes all are visible

    for (int i = 0; i < model.count(); ++i) {
        QmlGraphicsText *name = findItem<QmlGraphicsText>(pathview, "textName", i);
        QVERIFY(name != 0);
        QCOMPARE(name->text(), model.name(i));
        QmlGraphicsText *number = findItem<QmlGraphicsText>(pathview, "textNumber", i);
        QVERIFY(number != 0);
        QCOMPARE(number->text(), model.number(i));
    }

    delete canvas;
}

void tst_QmlGraphicsPathView::pathMoved()
{
    QmlView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    model.addItem("Ben", "12345");
    model.addItem("Bohn", "2345");
    model.addItem("Bob", "54321");
    model.addItem("Bill", "4321");

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QmlGraphicsPathView *pathview = findItem<QmlGraphicsPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    QmlGraphicsRectangle *firstItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 0);
    QVERIFY(firstItem);
    QmlGraphicsPath *path = qobject_cast<QmlGraphicsPath*>(pathview->path());
    QVERIFY(path);
    QPointF start = path->pointAt(0.0);
    QPointF offset;//Center of item is at point, but pos is from corner
    offset.setX(firstItem->width()/2);
    offset.setY(firstItem->height()/2);
    QCOMPARE(firstItem->pos() + offset, start);
    pathview->setOffset(10);
    QTest::qWait(1000);//Moving is animated?

    for(int i=0; i<model.count(); i++){
        QmlGraphicsRectangle *curItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", i);
        QCOMPARE(curItem->pos() + offset, path->pointAt(0.1 + i*0.25));
    }

    pathview->setOffset(100);
    QTest::qWait(1000);//Moving is animated?
    QCOMPARE(firstItem->pos() + offset, start);

    delete canvas;
}

void tst_QmlGraphicsPathView::limitedItems()
{
    QmlView *canvas = createView(SRCDIR "/data/pathview.qml");

    TestModel model;
    for(int i=0; i<100; i++)
        model.addItem("Bob", QString::number(i));

    QmlContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testModel", &model);

    canvas->execute();
    qApp->processEvents();

    QmlGraphicsPathView *pathview = findItem<QmlGraphicsPathView>(canvas->root(), "view");
    QVERIFY(pathview != 0);

    pathview->setPathItemCount(10);
    QCOMPARE(pathview->pathItemCount(), 10);

    QmlGraphicsRectangle *testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 0);
    QVERIFY(testItem != 0);
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 9);
    QVERIFY(testItem != 0);
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 10);
    QVERIFY(testItem == 0);

    pathview->setCurrentIndex(50);
    QTest::qWait(5100);//Moving is animated and it's travelling far - should be reconsidered.
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 0);
    QVERIFY(testItem == 0);
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 1);
    QVERIFY(testItem == 0);
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 9);
    QVERIFY(testItem == 0);
    testItem = findItem<QmlGraphicsRectangle>(pathview, "wrapper", 50);
    QVERIFY(testItem != 0);
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
    for (int i = 0; i < parent->children().count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(parent->children().at(i));
        if(!item)
            continue;
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

QTEST_MAIN(tst_QmlGraphicsPathView)

#include "tst_pathview.moc"
