#include <QTableView>
#include "modelview.h"
#include "mymodel.h"

ModelView::ModelView(QWidget *parent)
    : QMainWindow(parent)
{
    tableView = new QTableView(this);
    setCentralWidget(tableView);
    QAbstractTableModel *myModel = new MyModel(this);
    tableView->setModel( myModel );

    //transfer changes to the model to the window title
    connect(myModel, SIGNAL(editCompleted(const QString &) ), this, SLOT(setWindowTitle(const QString &)));
}

void ModelView::showWindowTitle(const QString & title)
{
setWindowTitle( title );
}
