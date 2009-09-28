#ifndef WATCHTABLEMODEL_H
#define WATCHTABLEMODEL_H

#include <QWidget>
#include <QtCore/qpointer.h>
#include <QtCore/qlist.h>
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE

class QmlDebugWatch;

class WatchTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    WatchTableModel(QObject *parent = 0);

    void addWatch(QmlDebugWatch *watch, const QString &title);
    void updateWatch(QmlDebugWatch *watch, const QVariant &value);

    QmlDebugWatch *findWatch(int column) const;
    QmlDebugWatch *findWatch(int objectDebugId, const QString &property) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void watchStateChanged();

private:
    int columnForWatch(QmlDebugWatch *watch) const;
    void addValue(int column, const QVariant &value);
    void removeWatch(QmlDebugWatch *watch);

    struct WatchedEntity
    {
        QString title;
        bool hasFirstValue;
        QString property;
        QPointer<QmlDebugWatch> watch;
    };

    struct Value {
        int column;
        QVariant variant;
        bool first;
    };

    QList<WatchedEntity> m_columns;
    QList<Value> m_values;
};


QT_END_NAMESPACE

#endif // WATCHTABLEMODEL_H
