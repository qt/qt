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
#ifndef BOOKMARKFILTERMODEL_H
#define BOOKMARKFILTERMODEL_H

#include <QtCore/QPersistentModelIndex>

#include <QtGui/QAbstractProxyModel>
#include <QtGui/QSortFilterProxyModel>

QT_BEGIN_NAMESPACE

class BookmarkItem;
class BookmarkModel;

typedef QList<QPersistentModelIndex> PersistentModelIndexCache;

class BookmarkFilterModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit BookmarkFilterModel(QObject *parent = 0);

    void setSourceModel(QAbstractItemModel *sourceModel);

    int rowCount(const QModelIndex &index) const;
    int columnCount(const QModelIndex &index) const;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

    Qt::DropActions supportedDropActions () const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    void filterBookmarks();
    void filterBookmarkFolders();

private slots:
    void changed(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void layoutAboutToBeChanged();
    void layoutChanged();
    void modelAboutToBeReset();
    void modelReset();

private:
    void setupCache(const QModelIndex &parent);
    void collectItems(const QModelIndex &parent);

private:
    bool hideBookmarks;
    BookmarkModel *sourceModel;
    PersistentModelIndexCache cache;
    QPersistentModelIndex indexToRemove;
};

// -- BookmarkTreeModel

class BookmarkTreeModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    BookmarkTreeModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

QT_END_NAMESPACE

#endif // BOOKMARKFILTERMODEL_H
