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
#include <QtTest/QSignalSpy>
#include <private/qlistmodelinterface_p.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <private/qdeclarativerepeater_p.h>
#include <private/qdeclarativetext_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

class tst_QDeclarativeRepeater : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeRepeater();

private slots:
    void numberModel();
    void objectList();
    void stringList();
    void dataModel();
    void itemModel();
    void properties();

private:
    QDeclarativeView *createView();
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &objectName, int index);
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id);
};

class TestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool error READ error WRITE setError)
    Q_PROPERTY(bool useModel READ useModel NOTIFY useModelChanged)

public:
    TestObject() : QObject(), mError(true), mUseModel(false) {}

    bool error() const { return mError; }
    void setError(bool err) { mError = err; }

    bool useModel() const { return mUseModel; }
    void setUseModel(bool use) { mUseModel = use; emit useModelChanged(); }

signals:
    void useModelChanged();

private:
    bool mError;
    bool mUseModel;
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


tst_QDeclarativeRepeater::tst_QDeclarativeRepeater()
{
}

void tst_QDeclarativeRepeater::numberModel()
{
    QDeclarativeView *canvas = createView();

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testData", 5);
    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/intmodel.qml"));
    qApp->processEvents();

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(canvas->rootObject(), "repeater");
    QVERIFY(repeater != 0);
    QCOMPARE(repeater->parentItem()->childItems().count(), 5+1);

    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);

    delete testObject;
    delete canvas;
}

class MyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int idx READ idx CONSTANT)
public:
    MyObject(int i) : QObject(), m_idx(i) {}

    int idx() const { return m_idx; }

    int m_idx;
};

void tst_QDeclarativeRepeater::objectList()
{
    QDeclarativeView *canvas = createView();
    QObjectList data;
    for(int i=0; i<100; i++)
        data << new MyObject(i);

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testData", QVariant::fromValue(data));

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/objlist.qml"));
    qApp->processEvents();

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(canvas->rootObject(), "repeater");
    QVERIFY(repeater != 0);
    QCOMPARE(repeater->property("errors").toInt(), 0);//If this fails either they are out of order or can't find the object's data
    QCOMPARE(repeater->property("instantiated").toInt(), 100);

    qDeleteAll(data);
    delete canvas;
}

/*
The Repeater element creates children at its own position in its parent's
stacking order.  In this test we insert a repeater between two other Text
elements to test this.
*/
void tst_QDeclarativeRepeater::stringList()
{
    QDeclarativeView *canvas = createView();

    QStringList data;
    data << "One";
    data << "Two";
    data << "Three";
    data << "Four";

    QDeclarativeContext *ctxt = canvas->rootContext();
    ctxt->setContextProperty("testData", data);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/repeater1.qml"));
    qApp->processEvents();

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(canvas->rootObject(), "repeater");
    QVERIFY(repeater != 0);

    QDeclarativeItem *container = findItem<QDeclarativeItem>(canvas->rootObject(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->childItems().count(), data.count() + 3);

    bool saw_repeater = false;
    for (int i = 0; i < container->childItems().count(); ++i) {

        if (i == 0) {
            QDeclarativeText *name = qobject_cast<QDeclarativeText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Zero"));
        } else if (i == container->childItems().count() - 2) {
            // The repeater itself
            QDeclarativeRepeater *rep = qobject_cast<QDeclarativeRepeater*>(container->childItems().at(i));
            QCOMPARE(rep, repeater);
            saw_repeater = true;
            continue;
        } else if (i == container->childItems().count() - 1) {
            QDeclarativeText *name = qobject_cast<QDeclarativeText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), QLatin1String("Last"));
        } else {
            QDeclarativeText *name = qobject_cast<QDeclarativeText*>(container->childItems().at(i));
            QVERIFY(name != 0);
            QCOMPARE(name->text(), data.at(i-1));
        }
    }
    QVERIFY(saw_repeater);

    delete canvas;
}

void tst_QDeclarativeRepeater::dataModel()
{
    QDeclarativeView *canvas = createView();
    QDeclarativeContext *ctxt = canvas->rootContext();
    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    TestModel testModel;
    testModel.addItem("one", "1");
    testModel.addItem("two", "2");
    testModel.addItem("three", "3");

    ctxt->setContextProperty("testData", &testModel);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/repeater2.qml"));
    qApp->processEvents();

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(canvas->rootObject(), "repeater");
    QVERIFY(repeater != 0);

    QDeclarativeItem *container = findItem<QDeclarativeItem>(canvas->rootObject(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->childItems().count(), 4);

    QSignalSpy repeaterSpy(repeater, SIGNAL(countChanged()));
    testModel.addItem("four", "4");
    QCOMPARE(container->childItems().count(), 5);
    QCOMPARE(repeaterSpy.count(),1);

    testModel.removeItem(2);
    QCOMPARE(container->childItems().count(), 4);
    QCOMPARE(repeaterSpy.count(),2);

    // Check that model changes are propagated
    QDeclarativeText *text = findItem<QDeclarativeText>(canvas->rootObject(), "myName", 1);
    QVERIFY(text);
    QCOMPARE(text->text(), QString("two"));

    testModel.modifyItem(1, "Item two", "_2");
    text = findItem<QDeclarativeText>(canvas->rootObject(), "myName", 1);
    QVERIFY(text);
    QCOMPARE(text->text(), QString("Item two"));

    text = findItem<QDeclarativeText>(canvas->rootObject(), "myNumber", 1);
    QVERIFY(text);
    QCOMPARE(text->text(), QString("_2"));

    delete testObject;
    delete canvas;
}

void tst_QDeclarativeRepeater::itemModel()
{
    QDeclarativeView *canvas = createView();
    QDeclarativeContext *ctxt = canvas->rootContext();
    TestObject *testObject = new TestObject;
    ctxt->setContextProperty("testObject", testObject);

    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/itemlist.qml"));
    qApp->processEvents();

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(canvas->rootObject(), "repeater");
    QVERIFY(repeater != 0);

    QDeclarativeItem *container = findItem<QDeclarativeItem>(canvas->rootObject(), "container");
    QVERIFY(container != 0);

    QCOMPARE(container->childItems().count(), 1);

    testObject->setUseModel(true);
    QMetaObject::invokeMethod(canvas->rootObject(), "checkProperties");
    QVERIFY(testObject->error() == false);

    QCOMPARE(container->childItems().count(), 4);
    QVERIFY(qobject_cast<QObject*>(container->childItems().at(0))->objectName() == "item1");
    QVERIFY(qobject_cast<QObject*>(container->childItems().at(1))->objectName() == "item2");
    QVERIFY(qobject_cast<QObject*>(container->childItems().at(2))->objectName() == "item3");
    QVERIFY(container->childItems().at(3) == repeater);

    QMetaObject::invokeMethod(canvas->rootObject(), "switchModel");
    QCOMPARE(container->childItems().count(), 3);
    QVERIFY(qobject_cast<QObject*>(container->childItems().at(0))->objectName() == "item4");
    QVERIFY(qobject_cast<QObject*>(container->childItems().at(1))->objectName() == "item5");
    QVERIFY(container->childItems().at(2) == repeater);

    testObject->setUseModel(false);
    QCOMPARE(container->childItems().count(), 1);

    delete testObject;
    delete canvas;
}

void tst_QDeclarativeRepeater::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("/properties.qml"));

    QDeclarativeItem *rootObject = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(rootObject);

    QDeclarativeRepeater *repeater = findItem<QDeclarativeRepeater>(rootObject, "repeater");
    QVERIFY(repeater);

    QSignalSpy modelSpy(repeater, SIGNAL(modelChanged()));
    repeater->setModel(3);
    QCOMPARE(modelSpy.count(),1);
    repeater->setModel(3);
    QCOMPARE(modelSpy.count(),1);

    QSignalSpy delegateSpy(repeater, SIGNAL(delegateChanged()));

    QDeclarativeComponent rectComponent(&engine);
    rectComponent.setData("import QtQuick 1.0; Rectangle {}", QUrl::fromLocalFile(""));

    repeater->setDelegate(&rectComponent);
    QCOMPARE(delegateSpy.count(),1);
    repeater->setDelegate(&rectComponent);
    QCOMPARE(delegateSpy.count(),1);

    delete rootObject;
}

QDeclarativeView *tst_QDeclarativeRepeater::createView()
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    return canvas;
}

template<typename T>
T *tst_QDeclarativeRepeater::findItem(QGraphicsObject *parent, const QString &objectName, int index)
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
T *tst_QDeclarativeRepeater::findItem(QGraphicsObject *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    if (mo.cast(parent) && (objectName.isEmpty() || parent->objectName() == objectName))
        return static_cast<T*>(parent);
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QDeclarativeItem *child = qobject_cast<QDeclarativeItem*>(parent->childItems().at(i));
        if (!child)
            continue;
        QDeclarativeItem *item = findItem<T>(child, objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_QDeclarativeRepeater)

#include "tst_qdeclarativerepeater.moc"
