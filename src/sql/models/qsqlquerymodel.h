/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQLQUERYMODEL_H
#define QSQLQUERYMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtSql/qsqldatabase.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Sql)

class QSqlQueryModelPrivate;
class QSqlError;
class QSqlRecord;
class QSqlQuery;

class Q_SQL_EXPORT QSqlQueryModel: public QAbstractTableModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSqlQueryModel)

public:
    explicit QSqlQueryModel(QObject *parent = 0);
    virtual ~QSqlQueryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QSqlRecord record(int row) const;
    QSqlRecord record() const;

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                       int role = Qt::EditRole);

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    void setQuery(const QSqlQuery &query);
    void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
    QSqlQuery query() const;

    virtual void clear();

    QSqlError lastError() const;

    void fetchMore(const QModelIndex &parent = QModelIndex());
    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const;

protected:
    virtual void queryChange();

    QModelIndex indexInQuery(const QModelIndex &item) const;
    void setLastError(const QSqlError &error);
    QSqlQueryModel(QSqlQueryModelPrivate &dd, QObject *parent = 0);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQLQUERYMODEL_H
