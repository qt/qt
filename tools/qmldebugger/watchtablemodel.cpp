#include "watchtablemodel.h"

#include <QtCore/qdebug.h>
#include <QtDeclarative/qmldebug.h>
#include <QtDeclarative/qmlmetatype.h>

QT_BEGIN_NAMESPACE


WatchTableModel::WatchTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void WatchTableModel::addWatch(QmlDebugWatch *watch, const QString &title)
{
    QString property;
    if (qobject_cast<QmlDebugPropertyWatch *>(watch))
        property = qobject_cast<QmlDebugPropertyWatch *>(watch)->name();

    // Watch will be automatically removed when its state is Inactive
    QObject::connect(watch, SIGNAL(stateChanged(State)), SLOT(watchStateChanged()));

    int col = columnCount(QModelIndex());
    beginInsertColumns(QModelIndex(), col, col);

    WatchedEntity e;
    e.title = title;
    e.hasFirstValue = false;
    e.property = property;
    e.watch = watch;
    m_columns.append(e);

    endInsertColumns();
}

void WatchTableModel::removeWatch(QmlDebugWatch *watch)
{
    int column = columnForWatch(watch);
    if (column == -1)
        return;

    WatchedEntity entity = m_columns.takeAt(column);

    for (QList<Value>::Iterator iter = m_values.begin(); iter != m_values.end();) {
        if (iter->column == column) {
            iter = m_values.erase(iter);
        } else {
            if(iter->column > column)
                --iter->column;
            ++iter;
        }
    }

    reset();
}

void WatchTableModel::updateWatch(QmlDebugWatch *watch, const QVariant &value)
{
    int column = columnForWatch(watch);
    if (column == -1)
        return;

    addValue(column, value);

    if (!m_columns[column].hasFirstValue) {
        m_columns[column].hasFirstValue = true;
        m_values[m_values.count() - 1].first = true;
    }
}

QmlDebugWatch *WatchTableModel::findWatch(int column) const
{
    if (column < m_columns.count())
        return m_columns.at(column).watch;
    return 0;
}

QmlDebugWatch *WatchTableModel::findWatch(int objectDebugId, const QString &property) const
{
    for (int i=0; i<m_columns.count(); i++) {
        if (m_columns[i].watch->objectDebugId() == objectDebugId
                && m_columns[i].property == property) {
            return m_columns[i].watch;
        }
    }
    return 0;
}

QList<QmlDebugWatch *> WatchTableModel::watches() const
{
    QList<QmlDebugWatch *> watches;
    for (int i=0; i<m_columns.count(); i++)
        watches << m_columns[i].watch;
    return watches;
}

int WatchTableModel::rowCount(const QModelIndex &) const
{
    return m_values.count();
}

int WatchTableModel::columnCount(const QModelIndex &) const
{
    return m_columns.count();
}

QVariant WatchTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (section < m_columns.count() && role == Qt::DisplayRole)
            return m_columns.at(section).title;
    } else {
        if (role == Qt::DisplayRole)
            return section + 1;
    }
    return QVariant();
}

QVariant WatchTableModel::data(const QModelIndex &idx, int role) const
{
    if (m_values.at(idx.row()).column == idx.column()) {
        if (role == Qt::DisplayRole) {
            const QVariant &value = m_values.at(idx.row()).variant;
            QString str = value.toString();

            if (str.isEmpty() && QmlMetaType::isObject(value.userType())) {
                QObject *o = QmlMetaType::toQObject(value);
                if(o) {
                    QString objectName = o->objectName();
                    if(objectName.isEmpty())
                        objectName = QLatin1String("<unnamed>");
                    str = QLatin1String(o->metaObject()->className()) +
                          QLatin1String(": ") + objectName;
                }
            }

            if(str.isEmpty()) {
                QDebug d(&str);
                d << value;
            }
            return QVariant(str);
        } else if(role == Qt::BackgroundRole) {
            if(m_values.at(idx.row()).first)
                return QColor(Qt::green);
            else
                return QVariant();
        } else {
            return QVariant();
        }
    } else {
        return QVariant();
    }
}

void WatchTableModel::watchStateChanged()
{
    QmlDebugWatch *watch = qobject_cast<QmlDebugWatch*>(sender());
    if (watch && watch->state() == QmlDebugWatch::Inactive)
        removeWatch(watch);
}

int WatchTableModel::columnForWatch(QmlDebugWatch *watch) const
{
    for (int i=0; i<m_columns.count(); i++) {
        if (m_columns.at(i).watch == watch)
            return i;
    }
    return -1;
}

void WatchTableModel::addValue(int column, const QVariant &value)
{
    int row = columnCount(QModelIndex());
    beginInsertRows(QModelIndex(), row, row);

    Value v;
    v.column = column;
    v.variant = value;
    v.first = false;
    m_values.append(v);

    endInsertRows();
}

QT_END_NAMESPACE
