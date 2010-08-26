/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "bookmarkfiltermodel.h"

#include "bookmarkitem.h"
#include "bookmarkmodel.h"

BookmarkFilterModel::BookmarkFilterModel(QObject *parent)
    : QAbstractProxyModel(parent)
    , hideBookmarks(true)
    , sourceModel(0)
{
}

void
BookmarkFilterModel::setSourceModel(QAbstractItemModel *_sourceModel)
{
    beginResetModel();

    QAbstractProxyModel::setSourceModel(sourceModel);
    sourceModel = qobject_cast<BookmarkModel*> (_sourceModel);

    connect(sourceModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this,
        SLOT(changed(QModelIndex, QModelIndex)));

    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
        this, SLOT(rowsInserted(QModelIndex, int, int)));

    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
        this, SLOT(rowsAboutToBeRemoved(QModelIndex, int, int)));
    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex, int, int)), this,
        SLOT(rowsRemoved(QModelIndex, int, int)));

    connect(sourceModel, SIGNAL(layoutAboutToBeChanged()), this,
        SLOT(layoutAboutToBeChanged()));
    connect(sourceModel, SIGNAL(layoutChanged()), this,
        SLOT(layoutChanged()));

    connect(sourceModel, SIGNAL(modelAboutToBeReset()), this,
        SLOT(modelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()), this, SLOT(modelReset()));

    if (sourceModel)
        setupCache(sourceModel->index(0, 0, QModelIndex()));

    endResetModel();
}

int
BookmarkFilterModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return cache.count();
}

int
BookmarkFilterModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index)
    if (sourceModel)
        return sourceModel->columnCount();
    return 0;
}

QModelIndex
BookmarkFilterModel::mapToSource(const QModelIndex &proxyIndex) const
{
    const int row = proxyIndex.row();
    if (proxyIndex.isValid() && row >= 0 && row < cache.count())
        return cache[row];
    return QModelIndex();
}

QModelIndex
BookmarkFilterModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return index(cache.indexOf(sourceIndex), 0, QModelIndex());
}

QModelIndex
BookmarkFilterModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

QModelIndex
BookmarkFilterModel::index(int row, int column, const QModelIndex &index) const
{
    Q_UNUSED(index)
    if (row < 0 || column < 0 || cache.count() <= row
        || !sourceModel || sourceModel->columnCount() <= column) {
        return QModelIndex();
    }
    return createIndex(row, 0);
}

Qt::DropActions
BookmarkFilterModel::supportedDropActions () const
{
    if (sourceModel)
        return sourceModel->supportedDropActions();
    return Qt::IgnoreAction;
}

Qt::ItemFlags
BookmarkFilterModel::flags(const QModelIndex &index) const
{
    if (sourceModel)
        return sourceModel->flags(index);
    return Qt::NoItemFlags;
}

QVariant
BookmarkFilterModel::data(const QModelIndex &index, int role) const
{
    if (sourceModel)
        return sourceModel->data(mapToSource(index), role);
    return QVariant();
}

bool
BookmarkFilterModel::setData(const QModelIndex &index, const QVariant &value,
    int role)
{
    if (sourceModel)
        return sourceModel->setData(mapToSource(index), value, role);
    return false;
}

void
BookmarkFilterModel::filterBookmarks()
{
    if (sourceModel) {
        beginResetModel();
        hideBookmarks = true;
        setupCache(sourceModel->index(0, 0, QModelIndex()));
        endResetModel();
    }
}

void
BookmarkFilterModel::filterBookmarkFolders()
{
    if (sourceModel) {
        beginResetModel();
        hideBookmarks = false;
        setupCache(sourceModel->index(0, 0, QModelIndex()));
        endResetModel();
    }
}

void
BookmarkFilterModel::changed(const QModelIndex &topLeft,
    const QModelIndex &bottomRight)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}

void
BookmarkFilterModel::rowsInserted(const QModelIndex &parent, int start, int end)
{
    if (!sourceModel)
        return;

    QModelIndex cachePrevious = parent;
    if (BookmarkItem *parentItem = sourceModel->itemFromIndex(parent)) {
        BookmarkItem *newItem = parentItem->child(start);

        // iterate over tree hirarchie to find the previous folder
        for (int i = 0; i < parentItem->childCount(); ++i) {
            if (BookmarkItem *child = parentItem->child(i)) {
                const QModelIndex &tmp = sourceModel->indexFromItem(child);
                if (tmp.data(UserRoleFolder).toBool() && child != newItem)
                    cachePrevious = tmp;
            }
        }

        const QModelIndex &newIndex = sourceModel->indexFromItem(newItem);
        const bool isFolder = newIndex.data(UserRoleFolder).toBool();
        if ((isFolder && hideBookmarks) || (!isFolder && !hideBookmarks)) {
            beginInsertRows(mapFromSource(parent), start, end);
            cache.insert(cache.indexOf(cachePrevious) + 1, newIndex);
            endInsertRows();
        }
    }
}

void
BookmarkFilterModel::rowsAboutToBeRemoved(const QModelIndex &parent, int start,
    int end)
{
    if (!sourceModel)
        return;

    if (BookmarkItem *parentItem = sourceModel->itemFromIndex(parent)) {
        if (BookmarkItem *child = parentItem->child(start)) {
            indexToRemove = sourceModel->indexFromItem(child);
            if (cache.contains(indexToRemove))
                beginRemoveRows(mapFromSource(parent), start, end);
        }
    }
}

void
BookmarkFilterModel::rowsRemoved(const QModelIndex &/*parent*/, int, int)
{
    if (cache.contains(indexToRemove)) {
        cache.removeAll(indexToRemove);
        endRemoveRows();
    }
}

void
BookmarkFilterModel::layoutAboutToBeChanged()
{
    // TODO: ???
}

void
BookmarkFilterModel::layoutChanged()
{
    // TODO: ???
}

void
BookmarkFilterModel::modelAboutToBeReset()
{
    beginResetModel();
}

void
BookmarkFilterModel::modelReset()
{
    if (sourceModel)
        setupCache(sourceModel->index(0, 0, QModelIndex()));
    endResetModel();
}

void
BookmarkFilterModel::setupCache(const QModelIndex &parent)
{
    cache.clear();
    collectItems(parent);
}

void
BookmarkFilterModel::collectItems(const QModelIndex &parent)
{
    if (parent.isValid()) {
        bool isFolder = sourceModel->data(parent, UserRoleFolder).toBool();
        if ((isFolder && hideBookmarks) || (!isFolder && !hideBookmarks))
            cache.append(parent);

        if (sourceModel->hasChildren(parent)) {
            for (int i = 0; i < sourceModel->rowCount(parent); ++i)
                collectItems(sourceModel->index(i, 0, parent));
        }
    }
}

// -- BookmarkTreeModel

BookmarkTreeModel::BookmarkTreeModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

int
BookmarkTreeModel::columnCount(const QModelIndex &parent) const
{
    return qMin(1, QSortFilterProxyModel::columnCount(parent));
}

bool
BookmarkTreeModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    Q_UNUSED(row)
    BookmarkModel *model = qobject_cast<BookmarkModel*> (sourceModel());
    if (model->rowCount(parent) > 0
        && model->data(model->index(row, 0, parent), UserRoleFolder).toBool())
        return true;
    return false;
}
