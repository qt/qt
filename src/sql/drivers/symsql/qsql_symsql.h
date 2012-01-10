/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#ifndef QSQL_SYMSQL_H
#define QSQL_SYMSQL_H

#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlresult.h>
#include <QtSql/private/qsqlcachedresult_p.h>



#ifdef QT_PLUGIN
#define Q_EXPORT_SQLDRIVER_SYMSQL
#else
#define Q_EXPORT_SQLDRIVER_SYMSQL Q_SQL_EXPORT
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QSymSQLDriverPrivate;
class QSymSQLResultPrivate;
class QSymSQLDriver;
class RSqlDatabase;

class QSymSQLResult : public QSqlResult
{
    friend class QSymSQLDriver;
    friend class QSymSQLResultPrivate;
public:
    explicit QSymSQLResult(const QSymSQLDriver* db);
    ~QSymSQLResult();
    QVariant handle() const;

protected:
    QVariant data(int field);
    bool isNull(int i);
    bool fetch(int i);
    bool fetchNext();
    bool fetchPrevious();
    bool fetchFirst();
    bool fetchLast();
    
    bool reset(const QString &query);
    bool prepare(const QString &query);
    bool exec();
    int size();
    int numRowsAffected();
    QSqlRecord record() const;
    void virtual_hook(int id, void *data);
    
    QVariant lastInsertId() const;
    
private:
    QSymSQLResultPrivate* d;
};

class Q_EXPORT_SQLDRIVER_SYMSQL QSymSQLDriver : public QSqlDriver
{
    Q_OBJECT
    friend class QSymSQLResult;
public:
    explicit QSymSQLDriver(QObject *parent = 0);
    explicit QSymSQLDriver(RSqlDatabase& connection, QObject *parent = 0);
    ~QSymSQLDriver();
    bool hasFeature(DriverFeature f) const;
    bool open(const QString & db,
                   const QString & user,
                   const QString & password,
                   const QString & host,
                   int port,
                   const QString & connOpts);
    void close();
    QSqlResult *createResult() const;
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    QStringList tables(QSql::TableType)const;

    QSqlRecord record(const QString& tablename) const;
    QSqlIndex primaryIndex(const QString &table) const;
    QVariant handle() const;
    QString escapeIdentifier(const QString &identifier, IdentifierType) const;
    
private:
    QSymSQLDriverPrivate* d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSQL_SYMSQL_H
