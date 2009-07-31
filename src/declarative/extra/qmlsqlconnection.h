/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLSQLCONNECTION_H
#define QMLSQLCONNECTION_H

#include <QtDeclarative/qml.h>
#include <QtSql/QSqlDatabase>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSqlDatabase;
class QmlSqlConnectionPrivate;
class Q_DECLARATIVE_EXPORT QmlSqlConnection : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QStringList tables READ tables)
    Q_PROPERTY(QString databaseName READ databaseName WRITE setDatabaseName)
    Q_PROPERTY(QString driver READ driver WRITE setDriver)
    Q_PROPERTY(QString connectOptions READ connectOptions WRITE setConnectOptions)
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName)
    Q_PROPERTY(int port READ port WRITE setPort)
    Q_PROPERTY(QString userName READ userName WRITE setUserName)
    Q_PROPERTY(QString password READ password WRITE setPassword)
    Q_PROPERTY(QString lastError READ lastError)
public:
    QmlSqlConnection(QObject *parent = 0);
    ~QmlSqlConnection();

    QString name() const;
    void setName(const QString &);

    QStringList tables() const;

    QString databaseName() const;
    void setDatabaseName(const QString &);

    QString connectOptions() const;
    void setConnectOptions(const QString &);

    QString hostName() const;
    void setHostName(const QString &);

    int port() const;
    void setPort(int);

    QString userName() const;
    void setUserName(const QString &);

    QString password() const;
    void setPassword(const QString &);

    QString driver() const;
    void setDriver(const QString &);

    QString lastError() const;

    QSqlDatabase database() const;
private:
    Q_DISABLE_COPY(QmlSqlConnection)
    Q_DECLARE_PRIVATE(QmlSqlConnection)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlSqlConnection)

QT_END_HEADER

#endif // QMLXMLLISTMODEL_H

