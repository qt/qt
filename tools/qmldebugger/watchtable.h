#ifndef WATCHTABLEMODEL_H
#define WATCHTABLEMODEL_H

#include <QtCore/qpointer.h>
#include <QtCore/qlist.h>

#include <QWidget>
#include <QHeaderView>
#include <QAbstractTableModel>
#include <QTableView>

QT_BEGIN_NAMESPACE

class QmlDebugWatch;
class QmlEngineDebug;
class QmlDebugConnection;
class QmlDebugPropertyReference;
class QmlDebugObjectReference;

class WatchTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    WatchTableModel(QmlEngineDebug *client = 0, QObject *parent = 0);
    ~WatchTableModel();

    void setEngineDebug(QmlEngineDebug *client);
    
    QmlDebugWatch *findWatch(int column) const;
    int columnForWatch(QmlDebugWatch *watch) const;

    void stopWatching(int column);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

signals:
    void watchCreated(QmlDebugWatch *watch);

public slots:
    void togglePropertyWatch(const QmlDebugObjectReference &obj, const QmlDebugPropertyReference &prop);
    void expressionWatchRequested(const QmlDebugObjectReference &, const QString &);

private slots:
    void watchStateChanged();
    void watchedValueChanged(const QByteArray &propertyName, const QVariant &value);

private:
    void addWatch(QmlDebugWatch *watch, const QString &title);
    void removeWatch(QmlDebugWatch *watch);
    void updateWatch(QmlDebugWatch *watch, const QVariant &value);

    QmlDebugWatch *findWatch(int objectDebugId, const QString &property) const;

    void addValue(int column, const QVariant &value);

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

    QmlEngineDebug *m_client;
    QList<WatchedEntity> m_columns;
    QList<Value> m_values;
};


class WatchTableHeaderView : public QHeaderView
{
    Q_OBJECT
public:
    WatchTableHeaderView(WatchTableModel *model, QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *me);

private:
    WatchTableModel *m_model;
};


class WatchTableView : public QTableView
{
    Q_OBJECT
public:
    WatchTableView(WatchTableModel *model, QWidget *parent = 0);

signals:
    void objectActivated(int objectDebugId);

private slots:
    void indexActivated(const QModelIndex &index);
    void watchCreated(QmlDebugWatch *watch);

private:
    WatchTableModel *m_model;
};


QT_END_NAMESPACE

#endif // WATCHTABLEMODEL_H
