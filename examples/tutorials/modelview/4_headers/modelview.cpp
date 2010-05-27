#include <QTableView>
#include <QHeaderView>
#include "modelview.h"
#include "mymodel.h"

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
{
    tableView = new QTableView(this);
    setCentralWidget(tableView);
    tableView->setModel(new MyModel(this) );
    tableView->verticalHeader()->hide();
}

