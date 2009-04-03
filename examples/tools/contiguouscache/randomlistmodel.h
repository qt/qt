#ifndef RANDOMLISTMODEL_H
#define RANDOMLISTMODEL_H

#include <QContiguousCache>
#include <QAbstractListModel>

class QTimer;
class RandomListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    RandomListModel(QObject *parent = 0);
    ~RandomListModel();

    int rowCount(const QModelIndex & = QModelIndex()) const;
    QVariant data(const QModelIndex &, int) const;

private:
    void cacheRows(int, int) const;
    QString fetchRow(int) const;

    mutable QContiguousCache<QString> m_rows;
    const int m_count;
};

#endif
