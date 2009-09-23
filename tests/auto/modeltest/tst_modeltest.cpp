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
#include <QtGui/QtGui>

#include "modeltest.h"


class tst_ModelTest : public QObject
{
    Q_OBJECT

public:
    tst_ModelTest() {}
    virtual ~tst_ModelTest() {}

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void stringListModel();
    void treeWidgetModel();
    void standardItemModel();
};



void tst_ModelTest::initTestCase()
{
}

void tst_ModelTest::cleanupTestCase()
{
}

void tst_ModelTest::init()
{

}

void tst_ModelTest::cleanup()
{
}
/*
  tests
*/

void tst_ModelTest::stringListModel()
{
    QStringListModel model;
    QSortFilterProxyModel proxy;
    
    ModelTest t1(&model);
    ModelTest t2(&proxy);
    
    proxy.setSourceModel(&model);
  
    model.setStringList(QStringList() << "2" << "3" << "1");
    model.setStringList(QStringList() << "a" << "e" << "plop" << "b" << "c" );
    
    proxy.setDynamicSortFilter(true);
    proxy.setFilterRegExp(QRegExp("[^b]"));
}

void tst_ModelTest::treeWidgetModel()
{
    QTreeWidget widget;

    ModelTest t1(widget.model());
    
    QTreeWidgetItem *root = new QTreeWidgetItem(&widget, QStringList("root"));
    for (int i = 0; i < 20; ++i) {
        new QTreeWidgetItem(root, QStringList(QString::number(i)));
    }
    QTreeWidgetItem *remove = root->child(2);
    root->removeChild(remove);
    QTreeWidgetItem *parent = new QTreeWidgetItem(&widget, QStringList("parent"));
    new QTreeWidgetItem(parent, QStringList("child"));
    widget.setItemHidden(parent, true);
    
    widget.sortByColumn(0);
}

void tst_ModelTest::standardItemModel()
{
    QStandardItemModel model(10,10);
    QSortFilterProxyModel proxy;
    
    
    ModelTest t1(&model);
    ModelTest t2(&proxy);
    
    proxy.setSourceModel(&model);
    
    model.insertRows(2, 5);
    model.removeRows(4, 5);

    model.insertColumns(2, 5);
    model.removeColumns(4, 5);
    
    model.insertRows(0,5, model.index(1,1));
    model.insertColumns(0,5, model.index(1,3));
    
}

QTEST_MAIN(tst_ModelTest)
#include "tst_modeltest.moc"
