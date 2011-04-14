/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QStandardItemModel>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsglistview_p.h>
#include <private/qsgtext_p.h>
#include <private/qsgvisualitemmodel_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include <math.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

static void initStandardTreeModel(QStandardItemModel *model)
{
    QStandardItem *item;
    item = new QStandardItem(QLatin1String("Row 1 Item"));
    model->insertRow(0, item);

    item = new QStandardItem(QLatin1String("Row 2 Item"));
    item->setCheckable(true);
    model->insertRow(1, item);

    QStandardItem *childItem = new QStandardItem(QLatin1String("Row 2 Child Item"));
    item->setChild(0, childItem);

    item = new QStandardItem(QLatin1String("Row 3 Item"));
    item->setIcon(QIcon());
    model->insertRow(2, item);
}

class SingleRoleModel : public QAbstractListModel
{
    Q_OBJECT

public:
    SingleRoleModel(QObject *parent = 0) {
        QHash<int, QByteArray> roles;
        roles.insert(Qt::DisplayRole , "name");
        setRoleNames(roles);
        list << "one" << "two" << "three" << "four";
    }

public slots:
    void set(int idx, QString string) {
        list[idx] = string;
        emit dataChanged(index(idx,0), index(idx,0));
    }

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        return list.count();
    }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
        if (role == Qt::DisplayRole)
            return list.at(index.row());
        return QVariant();
    }

private:
    QStringList list;
};


class tst_qsgvisualdatamodel : public QObject
{
    Q_OBJECT
public:
    tst_qsgvisualdatamodel();

private slots:
    void rootIndex();
    void updateLayout();
    void childChanged();
    void objectListModel();
    void singleRole();
    void modelProperties();
    void noDelegate();

private:
    QDeclarativeEngine engine;
    template<typename T>
    T *findItem(QSGItem *parent, const QString &objectName, int index);
};

class DataObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString color READ color WRITE setColor NOTIFY colorChanged)

public:
    DataObject(QObject *parent=0) : QObject(parent) {}
    DataObject(const QString &name, const QString &color, QObject *parent=0)
        : QObject(parent), m_name(name), m_color(color) { }


    QString name() const { return m_name; }
    void setName(const QString &name) {
        if (name != m_name) {
            m_name = name;
            emit nameChanged();
        }
    }

    QString color() const { return m_color; }
    void setColor(const QString &color) {
        if (color != m_color) {
            m_color = color;
            emit colorChanged();
        }
    }

signals:
    void nameChanged();
    void colorChanged();

private:
    QString m_name;
    QString m_color;
};

tst_qsgvisualdatamodel::tst_qsgvisualdatamodel()
{
}

void tst_qsgvisualdatamodel::rootIndex()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/visualdatamodel.qml"));

    QStandardItemModel model;
    initStandardTreeModel(&model);

    engine.rootContext()->setContextProperty("myModel", &model);

    QSGVisualDataModel *obj = qobject_cast<QSGVisualDataModel*>(c.create());
    QVERIFY(obj != 0);

    QMetaObject::invokeMethod(obj, "setRoot");
    QVERIFY(qvariant_cast<QModelIndex>(obj->rootIndex()) == model.index(0,0));

    QMetaObject::invokeMethod(obj, "setRootToParent");
    QVERIFY(qvariant_cast<QModelIndex>(obj->rootIndex()) == QModelIndex());

    delete obj;
}

void tst_qsgvisualdatamodel::updateLayout()
{
    QSGView view;

    QStandardItemModel model;
    initStandardTreeModel(&model);

    view.rootContext()->setContextProperty("myModel", &model);

    view.setSource(QUrl::fromLocalFile(SRCDIR "/data/datalist.qml"));

    QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
    QVERIFY(listview != 0);

    QSGItem *contentItem = listview->contentItem();
    QVERIFY(contentItem != 0);

    QSGText *name = findItem<QSGText>(contentItem, "display", 0);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 1 Item"));
    name = findItem<QSGText>(contentItem, "display", 1);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 2 Item"));
    name = findItem<QSGText>(contentItem, "display", 2);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 3 Item"));

    model.invisibleRootItem()->sortChildren(0, Qt::DescendingOrder);

    name = findItem<QSGText>(contentItem, "display", 0);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 3 Item"));
    name = findItem<QSGText>(contentItem, "display", 1);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 2 Item"));
    name = findItem<QSGText>(contentItem, "display", 2);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 1 Item"));
}

void tst_qsgvisualdatamodel::childChanged()
{
    QSGView view;

    QStandardItemModel model;
    initStandardTreeModel(&model);

    view.rootContext()->setContextProperty("myModel", &model);

    view.setSource(QUrl::fromLocalFile(SRCDIR "/data/datalist.qml"));

    QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
    QVERIFY(listview != 0);

    QSGItem *contentItem = listview->contentItem();
    QVERIFY(contentItem != 0);

    QSGVisualDataModel *vdm = listview->findChild<QSGVisualDataModel*>("visualModel");
    vdm->setRootIndex(QVariant::fromValue(model.indexFromItem(model.item(1,0))));

    QSGText *name = findItem<QSGText>(contentItem, "display", 0);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 2 Child Item"));

    model.item(1,0)->child(0,0)->setText("Row 2 updated child");

    name = findItem<QSGText>(contentItem, "display", 0);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 2 updated child"));

    model.item(1,0)->appendRow(new QStandardItem(QLatin1String("Row 2 Child Item 2")));
    QTest::qWait(300);

    name = findItem<QSGText>(contentItem, "display", 1);
    QVERIFY(name != 0);
    QCOMPARE(name->text(), QString("Row 2 Child Item 2"));

    model.item(1,0)->takeRow(1);
    name = findItem<QSGText>(contentItem, "display", 1);
    QVERIFY(name == 0);

    vdm->setRootIndex(QVariant::fromValue(QModelIndex()));
    QTest::qWait(300);
    name = findItem<QSGText>(contentItem, "display", 0);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 1 Item"));
    name = findItem<QSGText>(contentItem, "display", 1);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 2 Item"));
    name = findItem<QSGText>(contentItem, "display", 2);
    QVERIFY(name);
    QCOMPARE(name->text(), QString("Row 3 Item"));
}

void tst_qsgvisualdatamodel::objectListModel()
{
    QSGView view;

    QList<QObject*> dataList;
    dataList.append(new DataObject("Item 1", "red"));
    dataList.append(new DataObject("Item 2", "green"));
    dataList.append(new DataObject("Item 3", "blue"));
    dataList.append(new DataObject("Item 4", "yellow"));

    QDeclarativeContext *ctxt = view.rootContext();
    ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));

    view.setSource(QUrl::fromLocalFile(SRCDIR "/data/objectlist.qml"));

    QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
    QVERIFY(listview != 0);

    QSGItem *contentItem = listview->contentItem();
    QVERIFY(contentItem != 0);

    QSGText *name = findItem<QSGText>(contentItem, "name", 0);
    QCOMPARE(name->text(), QString("Item 1"));

    QSGText *section = findItem<QSGText>(contentItem, "section", 0);
    QCOMPARE(section->text(), QString("Item 1"));

    dataList[0]->setProperty("name", QLatin1String("Changed"));
    QCOMPARE(name->text(), QString("Changed"));
}

void tst_qsgvisualdatamodel::singleRole()
{
    {
        QSGView view;

        SingleRoleModel model;

        QDeclarativeContext *ctxt = view.rootContext();
        ctxt->setContextProperty("myModel", &model);

        view.setSource(QUrl::fromLocalFile(SRCDIR "/data/singlerole1.qml"));

        QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
        QVERIFY(listview != 0);

        QSGItem *contentItem = listview->contentItem();
        QVERIFY(contentItem != 0);

        QSGText *name = findItem<QSGText>(contentItem, "name", 1);
        QCOMPARE(name->text(), QString("two"));

        model.set(1, "Changed");
        QCOMPARE(name->text(), QString("Changed"));
    }
    {
        QSGView view;

        SingleRoleModel model;

        QDeclarativeContext *ctxt = view.rootContext();
        ctxt->setContextProperty("myModel", &model);

        view.setSource(QUrl::fromLocalFile(SRCDIR "/data/singlerole2.qml"));

        QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
        QVERIFY(listview != 0);

        QSGItem *contentItem = listview->contentItem();
        QVERIFY(contentItem != 0);

        QSGText *name = findItem<QSGText>(contentItem, "name", 1);
        QCOMPARE(name->text(), QString("two"));

        model.set(1, "Changed");
        QCOMPARE(name->text(), QString("Changed"));
    }
}

void tst_qsgvisualdatamodel::modelProperties()
{
    {
        QSGView view;

        SingleRoleModel model;

        QDeclarativeContext *ctxt = view.rootContext();
        ctxt->setContextProperty("myModel", &model);

        view.setSource(QUrl::fromLocalFile(SRCDIR "/data/modelproperties.qml"));

        QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
        QVERIFY(listview != 0);

        QSGItem *contentItem = listview->contentItem();
        QVERIFY(contentItem != 0);

        QSGItem *delegate = findItem<QSGItem>(contentItem, "delegate", 1);
        QVERIFY(delegate);
        QCOMPARE(delegate->property("test1").toString(),QString("two"));
        QCOMPARE(delegate->property("test2").toString(),QString("two"));
        QCOMPARE(delegate->property("test3").toString(),QString("two"));
        QCOMPARE(delegate->property("test4").toString(),QString("two"));
        QVERIFY(!delegate->property("test9").isValid());
        QCOMPARE(delegate->property("test5").toString(),QString(""));
        QVERIFY(delegate->property("test6").value<QObject*>() != 0);
        QCOMPARE(delegate->property("test7").toInt(),1);
        QCOMPARE(delegate->property("test8").toInt(),1);
    }

    {
        QSGView view;

        QList<QObject*> dataList;
        dataList.append(new DataObject("Item 1", "red"));
        dataList.append(new DataObject("Item 2", "green"));
        dataList.append(new DataObject("Item 3", "blue"));
        dataList.append(new DataObject("Item 4", "yellow"));

        QDeclarativeContext *ctxt = view.rootContext();
        ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));

        view.setSource(QUrl::fromLocalFile(SRCDIR "/data/modelproperties.qml"));

        QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
        QVERIFY(listview != 0);

        QSGItem *contentItem = listview->contentItem();
        QVERIFY(contentItem != 0);

        QSGItem *delegate = findItem<QSGItem>(contentItem, "delegate", 1);
        QVERIFY(delegate);
        QCOMPARE(delegate->property("test1").toString(),QString("Item 2"));
        QEXPECT_FAIL("", "QTBUG-13576", Continue);
        QCOMPARE(delegate->property("test2").toString(),QString("Item 2"));
        QVERIFY(qobject_cast<DataObject*>(delegate->property("test3").value<QObject*>()) != 0);
        QVERIFY(qobject_cast<DataObject*>(delegate->property("test4").value<QObject*>()) != 0);
        QCOMPARE(delegate->property("test5").toString(),QString("Item 2"));
        QCOMPARE(delegate->property("test9").toString(),QString("Item 2"));
        QVERIFY(delegate->property("test6").value<QObject*>() != 0);
        QCOMPARE(delegate->property("test7").toInt(),1);
        QCOMPARE(delegate->property("test8").toInt(),1);
    }

    {
        QSGView view;

        QStandardItemModel model;
        initStandardTreeModel(&model);

        view.rootContext()->setContextProperty("myModel", &model);

        QUrl source(QUrl::fromLocalFile(SRCDIR "/data/modelproperties2.qml"));

        //3 items, 3 warnings each
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":13: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":13: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":13: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":11: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":11: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":11: ReferenceError: Can't find variable: modelData");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":17: TypeError: Result of expression 'model.modelData' [undefined] is not an object.");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":17: TypeError: Result of expression 'model.modelData' [undefined] is not an object.");
        QTest::ignoreMessage(QtWarningMsg, source.toString().toLatin1() + ":17: TypeError: Result of expression 'model.modelData' [undefined] is not an object.");

        view.setSource(source);

        QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
        QVERIFY(listview != 0);

        QSGItem *contentItem = listview->contentItem();
        QVERIFY(contentItem != 0);

        QSGItem *delegate = findItem<QSGItem>(contentItem, "delegate", 1);
        QVERIFY(delegate);
        QCOMPARE(delegate->property("test1").toString(),QString("Row 2 Item"));
        QCOMPARE(delegate->property("test2").toString(),QString("Row 2 Item"));
        QVERIFY(!delegate->property("test3").isValid());
        QVERIFY(!delegate->property("test4").isValid());
        QVERIFY(!delegate->property("test5").isValid());
        QVERIFY(!delegate->property("test9").isValid());
        QVERIFY(delegate->property("test6").value<QObject*>() != 0);
        QCOMPARE(delegate->property("test7").toInt(),1);
        QCOMPARE(delegate->property("test8").toInt(),1);
    }

    //### should also test QStringList and QVariantList
}

void tst_qsgvisualdatamodel::noDelegate()
{
    QSGView view;

    QStandardItemModel model;
    initStandardTreeModel(&model);

    view.rootContext()->setContextProperty("myModel", &model);

    view.setSource(QUrl::fromLocalFile(SRCDIR "/data/datalist.qml"));

    QSGListView *listview = qobject_cast<QSGListView*>(view.rootObject());
    QVERIFY(listview != 0);

    QSGVisualDataModel *vdm = listview->findChild<QSGVisualDataModel*>("visualModel");
    QVERIFY(vdm != 0);
    QCOMPARE(vdm->count(), 3);

    vdm->setDelegate(0);
    QCOMPARE(vdm->count(), 0);
}


template<typename T>
T *tst_qsgvisualdatamodel::findItem(QSGItem *parent, const QString &objectName, int index)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QSGItem *item = qobject_cast<QSGItem*>(parent->childItems().at(i));
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

QTEST_MAIN(tst_qsgvisualdatamodel)

#include "tst_qsgvisualdatamodel.moc"
