//! [Quoting ModelView Tutorial]
#include <QTableView>
#include "modelview.h"
#include "mymodel.h"

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
{
    tableView = new QTableView(this);
    setCentralWidget(tableView);
    tableView->setModel(new MyModel(this) );
}
//! [Quoting ModelView Tutorial]