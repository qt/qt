/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
