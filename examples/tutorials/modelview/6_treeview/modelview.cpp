//! [Quoting ModelView Tutorial]
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>
#include "modelview.h"


const int ROWS = 2;
const int COLUMNS = 3;

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
{
    treeView = new QTreeView(this);
    setCentralWidget(treeView);
    standardModel = new QStandardItemModel ;

    QList<QStandardItem *> preparedColumn =prepareColumn("first", "second", "third");
    QStandardItem *item = standardModel->invisibleRootItem();
    // adding a row to the invisible root item produces a root element
    item->appendRow(preparedColumn);

    QList<QStandardItem *> secondRow =prepareColumn("111", "222", "333");
    // adding a row to an item starts a subtree
    preparedColumn.first()->appendRow(secondRow);

    treeView->setModel( standardModel );
    treeView->expandAll();
}

//---------------------------------------------------------------------------
QList<QStandardItem *> ModelView::prepareColumn(const QString &first,
                                                const QString &second,
                                                const QString &third )
{
    QList<QStandardItem *> colItems;
    colItems << new QStandardItem(first);
    colItems << new QStandardItem(second);
    colItems << new QStandardItem(third);
    return colItems;
}
//! [Quoting ModelView Tutorial]