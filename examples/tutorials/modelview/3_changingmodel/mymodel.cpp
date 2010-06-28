#include <QTimer>
#include <QTime>
#include <QBrush>
#include "mymodel.h"

//! [quoting mymodel_a]
MyModel::MyModel(QObject *parent)
    :QAbstractTableModel(parent)
{
//    selectedCell = 0;
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()) , this, SLOT(timerHit()) );
    timer->start();
}
//! [quoting mymodel_a]
//-------------------------------------------------------
int MyModel::rowCount(const QModelIndex  & /*parent */ ) const
{
    return 2;
}

//-------------------------------------------------------
int MyModel::columnCount(const QModelIndex  & /*parent */ ) const
{
    return 3;
}

//-------------------------------------------------------
//! [quoting mymodel_QVariant ]
QVariant MyModel::data(const QModelIndex &index, int role ) const
{
    int row = index.row();
    int col = index.column();

    if(role == Qt::DisplayRole)
    {
        if(row == 0 && col == 0 )
        {
            return QTime::currentTime().toString();
        }
    }
    return QVariant();
}
//! [quoting mymodel_QVariant ]
//-------------------------------------------------------
//! [quoting mymodel_b ]
void MyModel::timerHit()
{
    //we identify the top left cell
    QModelIndex topLeft = createIndex ( 0,0 );
    //emit a signal to make the view reread identified data
    emit dataChanged ( topLeft, topLeft );
}
//! [quoting mymodel_b ]
