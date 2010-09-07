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
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QStandardItemModel>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativelistview_p.h>
#include <private/qdeclarativetext_p.h>
#include <private/qdeclarativevisualitemmodel_p.h>
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

class tst_qdeclarativevisualdatamodel : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativevisualdatamodel();

private slots:
    void rootIndex();
    void objectListModel();

private:
    QDeclarativeEngine engine;
    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &objectName, int index);
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

tst_qdeclarativevisualdatamodel::tst_qdeclarativevisualdatamodel()
{
}

void tst_qdeclarativevisualdatamodel::rootIndex()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/visualdatamodel.qml"));

    QStandardItemModel model;
    initStandardTreeModel(&model);

    engine.rootContext()->setContextProperty("myModel", &model);

    QDeclarativeVisualDataModel *obj = qobject_cast<QDeclarativeVisualDataModel*>(c.create());
    QVERIFY(obj != 0);

    QMetaObject::invokeMethod(obj, "setRoot");
    QVERIFY(qvariant_cast<QModelIndex>(obj->rootIndex()) == model.index(0,0));

    QMetaObject::invokeMethod(obj, "setRootToParent");
    QVERIFY(qvariant_cast<QModelIndex>(obj->rootIndex()) == QModelIndex());

    delete obj;
}

void tst_qdeclarativevisualdatamodel::objectListModel()
{
    QDeclarativeView view;

    QList<QObject*> dataList;
    dataList.append(new DataObject("Item 1", "red"));
    dataList.append(new DataObject("Item 2", "green"));
    dataList.append(new DataObject("Item 3", "blue"));
    dataList.append(new DataObject("Item 4", "yellow"));

    QDeclarativeContext *ctxt = view.rootContext();
    ctxt->setContextProperty("myModel", QVariant::fromValue(dataList));

    view.setSource(QUrl::fromLocalFile(SRCDIR "/data/objectlist.qml"));

    QDeclarativeListView *listview = qobject_cast<QDeclarativeListView*>(view.rootObject());
    QVERIFY(listview != 0);

    QDeclarativeItem *contentItem = listview->contentItem();
    QVERIFY(contentItem != 0);

    QDeclarativeText *name = findItem<QDeclarativeText>(contentItem, "name", 0);
    QCOMPARE(name->text(), QString("Item 1"));

    QDeclarativeText *section = findItem<QDeclarativeText>(contentItem, "section", 0);
    QCOMPARE(section->text(), QString("Item 1"));

    dataList[0]->setProperty("name", QLatin1String("Changed"));
    QCOMPARE(name->text(), QString("Changed"));
}

template<typename T>
T *tst_qdeclarativevisualdatamodel::findItem(QGraphicsObject *parent, const QString &objectName, int index)
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

QTEST_MAIN(tst_qdeclarativevisualdatamodel)

#include "tst_qdeclarativevisualdatamodel.moc"
