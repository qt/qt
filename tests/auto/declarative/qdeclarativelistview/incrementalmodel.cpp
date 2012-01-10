/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "incrementalmodel.h"
#include <QApplication>
#include <QDebug>

IncrementalModel::IncrementalModel(QObject *parent)
    : QAbstractListModel(parent), count(0)
{
    for (int i = 0; i < 100; ++i)
        list.append("Item " + QString::number(i));
}

int IncrementalModel::rowCount(const QModelIndex & /* parent */) const
{
    return count;
}

QVariant IncrementalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= list.size() || index.row() < 0)
        return QVariant();
    
    if (role == Qt::DisplayRole)
        return list.at(index.row());
    return QVariant();
}

bool IncrementalModel::canFetchMore(const QModelIndex & /* index */) const
{
    if (count < list.size())
        return true;
    else
        return false;
}

void IncrementalModel::fetchMore(const QModelIndex & /* index */)
{
    int remainder = list.size() - count;
    int itemsToFetch = qMin(5, remainder);

    beginInsertRows(QModelIndex(), count, count+itemsToFetch-1);
    
    count += itemsToFetch;

    endInsertRows();
}
