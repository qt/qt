#ifndef MYMODEL_H
#define MYMODEL_H

#include <QAbstractTableModel>

class QTimer;  // forward declaration

class MyModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MyModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QTimer *timer;
private:
    int selectedCell;
private slots:
    void timerHit();
};

#endif // MYMODEL_H
