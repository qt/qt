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

#include "qmlsqlconnection.h"
#include "private/qobject_p.h"

#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>

#include <qml.h>
#include <qmlcontext.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,SqlConnection,QmlSqlConnection)

class QmlSqlConnectionPrivate: public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlSqlConnection)
public:
    QmlSqlConnectionPrivate() : port(0) {}

    int port;
    QString name, databaseName, connectionOptions;
    QString hostName, userName, password, driver;
};

/*!
    \qmlclass SqlConnection QmlSqlConnection
    \brief The SqlConnection element describes a connection to an SQL database.

    The SqlConnection element works in a similar way to
    QSqlDatabase::addDatabase().  It allows setting the database properties
    such that the connection does not need to be set up in C++ code.
    It differs from QSqlDatabase::addDatabase() in that it will automatically
    open the database.

    The database can then either be used from an SqlQuery element using its id
    as a bind, or using its name.  If the database is set up externally to
    Qml the query should connect using its name.

    \qml
    SqlConnection {
        id: myConnection
        name: "qmlConnection"
        driver: "QSQLITE"
        databaseName: "mydb.sqlite"
    }
    SqlQuery { id: listmodel; connection: myConnection; query: "SELECT * FROM mytable" }
    SqlQuery { id: othermodel; connection: "qmlConnection"; query: "SELECT * FROM myothertable" }
    \endqml
*/

/*!
    \qmlproperty QString SqlConnection::name

    Defines the connection's name.  The name allows the connection to be
    retrieved using the connection property of SqlQuery or when
    QSqlDatabase::database()
*/

/*!
    \qmlproperty list<string> SqlConnection::tables

    Defines the set of tables that exist in the database for the connection.
*/

/*!
    \qmlproperty string SqlConnection::databaseName

    Defines the connection's database name.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::driver

    Defines the driver type of the connection.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::connectOptions

    Defines the options used when connecting to the database.  These are used
    when opening the connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::hostName

    Defines the connection's host name.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty int SqlConnection::port

    Defines the connection's port number.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::userName

    Defines the connection's user name.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::password

    Defines the connection's password.  This is used when opening the
    connection to the database.
*/

/*!
    \qmlproperty string SqlConnection::lastError

    Defines the last error, if one occurred, when working with the database.
    If the error occurred in conjunction with an SQL query the error will be
    defined by SqlQuery::lastError.
*/

/*!
    \internal
    \class QmlSqlConnection
    \brief The QmlSqlConnection class manages a connection to an SQL database.
*/

/*!
    Constructs a QmlSqlConnection with the given \a parent.
*/
QmlSqlConnection::QmlSqlConnection(QObject *parent)
: QObject(*(new QmlSqlConnectionPrivate), parent)
{
}

/*!
    Destroys the QmlSqlConnection.
*/
QmlSqlConnection::~QmlSqlConnection()
{
    QSqlDatabase db = database();
    if (db.isOpen())
        db.close();
}

/*!
    Returns the connection's name.
    This is equivalent to QSqlDatabase::connectionName().
    \sa setName()
*/
QString QmlSqlConnection::name() const
{
    Q_D(const QmlSqlConnection);
    return d->name;
}

/*!
    Returns the connection's database name.
    This is equivalent to QSqlDatabase::databaseName().
    \sa setDatabaseName()
*/
QString QmlSqlConnection::databaseName() const
{
    Q_D(const QmlSqlConnection);
    return d->databaseName;
}

/*!
    Returns the connect options string used for this connection.

    \sa setConnectOptions()
*/
QString QmlSqlConnection::connectOptions() const
{
    Q_D(const QmlSqlConnection);
    return d->connectionOptions;
}

/*!
    Returns the connection's host name.
    \sa setHostName()
*/
QString QmlSqlConnection::hostName() const
{
    Q_D(const QmlSqlConnection);
    return d->hostName;
}

/*!
    Returns the connection's port number.  The value is undefined if the port
    number has not been set.

    \sa setPort()
*/
int QmlSqlConnection::port() const
{
    Q_D(const QmlSqlConnection);
    return d->port;
}

/*!
    Returns the connection's user name.
    
    \sa setUserName()
*/
QString QmlSqlConnection::userName() const
{
    Q_D(const QmlSqlConnection);
    return d->userName;
}

/*!
    Returns the connection's password.

    \sa setPassword)()
*/
QString QmlSqlConnection::password() const
{
    Q_D(const QmlSqlConnection);
    return d->password;
}

/*!
    Returns the connection's driver name.

    \sa setDriver()
*/
QString QmlSqlConnection::driver() const
{
    Q_D(const QmlSqlConnection);
    return d->driver;
}

/*!
    Returns a list of the database's tables.
*/
QStringList QmlSqlConnection::tables() const
{
    return database().tables();
}

/*!
    Sets the connection's name to the given \a name.

    \sa name()
*/
void QmlSqlConnection::setName(const QString &name)
{
    Q_D(QmlSqlConnection);
    d->name = name;
}

/*!
    Set's the connection's database name to the given \a name.

    \sa databaseName()
*/
void QmlSqlConnection::setDatabaseName(const QString &name)
{
    Q_D(QmlSqlConnection);
    d->databaseName = name;
}

/*!
    Sets the connection's options to the given \a options.

    \sa connectOptions(), QSqlDatabase::setConnectOptions()
*/
void QmlSqlConnection::setConnectOptions(const QString &options)
{
    Q_D(QmlSqlConnection);
    d->connectionOptions = options;
}

/*!
    Sets the connection's host name to the given \a name.

    \sa hostName()
*/
void QmlSqlConnection::setHostName(const QString &name)
{
    Q_D(QmlSqlConnection);
    d->hostName = name;
}

/*!
    Sets the connection's port number to the given \a port.

    \sa port()
*/
void QmlSqlConnection::setPort(int port)
{
    Q_D(QmlSqlConnection);
    d->port = port;
}

/*!
    Sets the connection's user name to the given \a name.

    \sa userName()
*/
void QmlSqlConnection::setUserName(const QString &name)
{
    Q_D(QmlSqlConnection);
    d->userName = name;
}

/*!
    Sets the connection's password to the given \a password.

    \sa password()
*/
void QmlSqlConnection::setPassword(const QString &password)
{
    Q_D(QmlSqlConnection);
    d->password = password;
}

/*!
    Returns information about the last error that occurred on the database.

    Failures that occur in conjunction with an individual query are
    reported by QmlSqlQuery::lastError()
*/
QString QmlSqlConnection::lastError() const
{
    return database().lastError().text();
}

/*!
    Sets the connection's driver to the specified driver \a type.

    \sa driver(), QSqlDatabase::addDatabase()
*/
void QmlSqlConnection::setDriver(const QString &type)
{
    Q_D(QmlSqlConnection);
    d->driver = type;
}

/*!
    \reimp
*/
void QmlSqlConnection::classComplete()
{
}

/*!
    Returns the database object associated with this connection.
    If the database is not yet open, it will open the database
    passed on the settings specified for the SQL connection.
*/
QSqlDatabase QmlSqlConnection::database() const
{
    Q_D(const QmlSqlConnection);

    QSqlDatabase db;
    if (QSqlDatabase::connectionNames().contains(d->name)) {
        db = QSqlDatabase::database(d->name);
    } else {
        db = QSqlDatabase::addDatabase(
                d->driver.isEmpty()
                ? QLatin1String("QSQLITE")
                : d->driver,
                d->name.isEmpty()
                ? QLatin1String(QSqlDatabase::defaultConnection)
                : d->name);
    }
    if (db.isOpen())
        return db;
    if ((d->driver.isEmpty() || d->driver == QLatin1String("QSQLITE")) && 
            qmlContext(this)) {
        // SQLITE uses files for databases, hence use relative pathing
        // if possible.
        QUrl url = qmlContext(this)->resolvedUrl(QUrl(d->databaseName));
        if (url.isRelative() || url.scheme() == QLatin1String("file"))
            db.setDatabaseName(url.toLocalFile());
        else
            db.setDatabaseName(d->databaseName);
    } else {
        db.setDatabaseName(d->databaseName);
    }
    db.setConnectOptions(d->connectionOptions);
    db.setHostName(d->hostName);
    db.setPassword(d->password);
    db.setPort(d->port);
    db.setUserName(d->userName);
    if (!db.open())
        qWarning() << "Failed to open database" << lastError();

    return db;
}

QT_END_NAMESPACE
