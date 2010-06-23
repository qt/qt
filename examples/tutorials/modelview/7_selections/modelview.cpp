//! [quoting modelview_a]
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "modelview.h"

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
{
    treeView = new QTreeView(this);
    setCentralWidget(treeView);
    standardModel = new QStandardItemModel ;
    QStandardItem *rootNode = standardModel->invisibleRootItem();


    //defining a couple of items
    QStandardItem *americaItem = new QStandardItem("America");
    QStandardItem *mexicoItem =  new QStandardItem("Canada");
    QStandardItem *usaItem =     new QStandardItem("USA");
    QStandardItem *bostonItem =  new QStandardItem("Boston");
    QStandardItem *europeItem =  new QStandardItem("Europe");
    QStandardItem *italyItem =   new QStandardItem("Italy");
    QStandardItem *romeItem =    new QStandardItem("Rome");
    QStandardItem *veronaItem =  new QStandardItem("Verona");

    //building up the hierarchy
    rootNode->    appendRow(americaItem);
    rootNode->    appendRow(europeItem);
    americaItem-> appendRow(mexicoItem);
    americaItem-> appendRow(usaItem);
    usaItem->     appendRow(bostonItem);
    europeItem->  appendRow(italyItem);
    italyItem->   appendRow(romeItem);
    italyItem->   appendRow(veronaItem);
    
    //register the model
    treeView->setModel( standardModel );
    treeView->expandAll();

    //selection changes shall trigger a slot
    QItemSelectionModel *selectionModel= treeView->selectionModel();
    connect(selectionModel, SIGNAL(selectionChanged ( const QItemSelection & , const QItemSelection & )),
            this, SLOT(selectionChangedSlot(const QItemSelection & , const QItemSelection & )));
}
//! [quoting modelview_a]

//------------------------------------------------------------------------------------

//! [quoting modelview_b]
void ModelView::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();
    int hierarchyLevel=1;
    QModelIndex seekRoot = index;
    while(seekRoot.parent() != QModelIndex() )
    {
        seekRoot = seekRoot.parent();
        hierarchyLevel++;
    }
    QString showString = QString("%1, Level %2").arg(selectedText)
                         .arg(hierarchyLevel);
    setWindowTitle(showString);
}
//! [quoting modelview_b]


