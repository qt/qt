//! [quoting mymodel_d]
#include "mymodel.h"

const int COLS= 3;
const int ROWS= 2;

MyModel::MyModel(QObject *parent)
    :QAbstractTableModel(parent)
{
    //gridData needs to have 6 element, one for each table cell
    m_gridData << "1/1" << "1/2" << "1/3" << "2/1" << "2/2" << "2/3" ;
}
//! [quoting mymodel_d]

//! [quoting mymodel_e]
//-------------------------------------------------------
int MyModel::rowCount(const QModelIndex & /*parent*/ ) const
{
    return ROWS;
}

//-------------------------------------------------------
int MyModel::columnCount(const QModelIndex & /*parent*/ ) const
{
    return COLS;
}

//-------------------------------------------------------
QVariant MyModel::data(const QModelIndex &index, int role ) const
{
    if(role == Qt::DisplayRole)
    {
        return m_gridData[modelIndexToOffset(index)];
    }
    return QVariant();
}
//! [quoting mymodel_e]

//-----------------------------------------------------------------

//! [quoting mymodel_f]
bool MyModel::setData ( const QModelIndex & index, const QVariant & value, int role  )
{
    if(role == Qt::EditRole)
    {
        m_gridData[modelIndexToOffset(index)] = value.toString();
        emit editCompleted(m_gridData.join(" | ") );
    }
    return true;
}

//-----------------------------------------------------------------
Qt::ItemFlags MyModel::flags ( const QModelIndex & /*index*/ ) const
{
    return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
}

//-----------------------------------------------------------------
//convert row and column information to array offset
int MyModel::modelIndexToOffset(const QModelIndex & index) const
{
    return index.row()*COLS + index.column();
}
//! [quoting mymodel_f]
