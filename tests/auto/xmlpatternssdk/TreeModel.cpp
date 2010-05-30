/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDebug>

#include "TestContainer.h"

#include "TreeModel.h"

using namespace QPatternistSDK;

TreeModel::TreeModel(const QStringList columnData,
                     QObject *p) : QAbstractItemModel(p),
                                   m_root(0),
                                   m_columnData(columnData)
{
}

TreeModel::~TreeModel()
{
}

QVariant TreeModel::data(const QModelIndex &idx, int role) const
{
    if(!idx.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem *>(idx.internalPointer());
    Q_ASSERT(item);

    return item->data(static_cast<Qt::ItemDataRole>(role), idx.column());
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_columnData.value(section);

    return QVariant();
}

void TreeModel::childChanged(TreeItem *item)
{
    if (item) {
        const QModelIndex index = createIndex(item->row(), 0, item);
        dataChanged(index, index);
    } else {
        layoutChanged();
    }
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &p) const
{
    const int c = columnCount(p);

    if(row < 0 || column < 0 || column >= c)
        return QModelIndex();

    TreeItem *parentItem;

    if(p.isValid())
        parentItem = static_cast<TreeItem *>(p.internalPointer());
    else
        parentItem = m_root;

    if(!parentItem)
        return QModelIndex();

    TreeItem *childItem = parentItem->child(row);

    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &idx) const
{
    if(!idx.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem *>(idx.internalPointer());
    Q_ASSERT(childItem);
    TreeItem *parentItem = childItem->parent();

    if(!parentItem || parentItem == m_root)
        return QModelIndex();

    Q_ASSERT(parentItem);
    return createIndex(parentItem->row(), 0, parentItem);
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &idx) const
{
    /* Not sure about this code. */
    if(!idx.isValid())
        return Qt::ItemFlags();

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int TreeModel::rowCount(const QModelIndex &p) const
{
    if(p.column() > 0)
        return 0;

    const TreeItem *parentItem;

    if(p.isValid())
        parentItem = static_cast<TreeItem *>(p.internalPointer());
    else
    {
        if(m_root)
            parentItem = m_root;
        else
            return 0;
    }

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &p) const
{
    if(p.isValid())
        return static_cast<TreeItem *>(p.internalPointer())->columnCount();
    else
        return m_columnData.count();
}

TreeItem *TreeModel::root() const
{
    return m_root;
}

void TreeModel::setRoot(TreeItem *r)
{
    TreeItem *const oldRoot = m_root;
    m_root = r;

    if(m_root)
        connect(r, SIGNAL(changed(TreeItem *)), SLOT(childChanged(TreeItem *)));
    reset(); /* Notify views that we have radically changed. */
    delete oldRoot;
}

// vim: et:ts=4:sw=4:sts=4
