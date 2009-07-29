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

#include "qmlsqlquery.h"
#include "qmlsqlconnection.h"
#include "private/qobject_p.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlDriver>

#include <QContiguousCache>
#include <QTimerEvent>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,SqlBind,QmlSqlBind)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,SqlQuery,QmlSqlQuery)

class QmlSqlBindPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlSqlBind)
public:
    QmlSqlBindPrivate() {}

    QString name;
    QVariant value;
};

/*!
    \qmlclass SqlBind QmlSqlBind
    \brief The SqlBind element specifies a value binding for an SqlQuery element.

    By using bindings its possible to cause a SqlQuery to update itself
    when values bound through the SqlBind change.  Hence in the example
    below the results for the SqlQuery will change as searchText changes.

    If the query is not a SELECT statement, the effects of the bound
    values will only apply when the SqlQuery exec() slot is called.

    \qml
    SqlQuery {
        query: "SELECT * FROM mytable WHERE name LIKE :value"
        bindings: SqlBind {
            name: ":value"
            value: searchText + '%'
        }
    }
    SqlQuery {
        query: "SELECT * FROM mytable WHERE type = ?"
        bindings: SqlBind {
            value: "simple"
        }
    }
    \endqml
*/

/*!
    \internal
    \class QmlSqlBind
    \brief The QmlSqlBind class specifies a value binding for a QmlSqlQuery.
*/

/*!
    \fn void QmlSqlBind::valueChanged()

    This signal is emitted when the value property of the SqlBind changes.
*/

/*!
    \qmlproperty QString SqlBind::name

    Defines the placeholder name of the bind. If no name is specified the 
    bind will use its position within the SqlQuery's bindings to bind
    into the query.
*/

/*!
    \qmlproperty QVariant SqlBind::value

    Defines the value to bind into the query.
*/

/*!
    Constructs a QmlSqlVind with the given \a parent
*/
QmlSqlBind::QmlSqlBind(QObject *parent)
: QObject(*(new QmlSqlBindPrivate()), parent)
{
}

/*!
    Destroys the QmlSqlBind.
*/
QmlSqlBind::~QmlSqlBind()
{
}

/*!
    Returns the binding's name.

    \sa setName()
*/
QString QmlSqlBind::name() const
{
    Q_D(const QmlSqlBind);
    return d->name;
}

/*!
    Returns the binding's value.

    \sa setValue()
*/
QVariant QmlSqlBind::value() const
{
    Q_D(const QmlSqlBind);
    return d->value;
}

/*!
    Sets the binding's name to the given \a name.

    \sa name()
*/
void QmlSqlBind::setName(const QString &name)
{
    Q_D(QmlSqlBind);
    d->name = name;
}

/*!
    Sets the binding's value to the given \a value.

    \sa value()
*/
void QmlSqlBind::setValue(const QVariant &value)
{
    Q_D(QmlSqlBind);
    if (d->value != value) {
        d->value = value;
        emit valueChanged();
    }
}

class QmlSqlQueryPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlSqlQuery)
public:
    QmlSqlQueryPrivate(QmlSqlQuery *owner)
        : isSel(false), query(NULL), requireCache(false), count(-1),
        cacheNear(100), cacheRetain(100), cacheSize(1000),
        cacheInterval(250), scheduledRow(-1), cacheTimer(-1),
        binds(owner) {}
    void prepareQuery() const;
    void bindQuery() const;
    void grabRoles() const;

    void clearCache();
    void initCache() const;
    void hitCache(int row) const;

    void enactShift(int row) const;

    QString queryText;
    bool isSel;
    QVariant connectionVariant;
    mutable QSqlQuery *query;
    mutable bool requireCache;
    mutable int count;
    mutable QList<int> roles;
    mutable QStringList roleNames;

    typedef QContiguousCache<QVariant> Column;
    mutable QVector< Column * > cache;

    int cacheNear, cacheRetain, cacheSize, cacheInterval;

    mutable int scheduledRow;
    mutable int cacheTimer;

    class QmlSqlBindList : public QmlList<QmlSqlBind *>
    {
    public:
        QmlSqlBindList(QmlSqlQuery *owner)
            : q(owner){}

        void append(QmlSqlBind *o) {
            m_contents.append(o);
            QObject::connect(o, SIGNAL(valueChanged()), q, SLOT(resetBinds()));
        }
        void clear() { m_contents.clear(); }
        int count() const { return m_contents.count(); }
        void removeAt(int pos) { return m_contents.removeAt(pos); }
        QmlSqlBind *at(int pos) const { return m_contents.at(pos); }
        void insert(int pos, QmlSqlBind *o) { m_contents.insert(pos, o); }
    private:
        QList<QmlSqlBind *> m_contents;
        QmlSqlQuery *q;
    };

    QmlSqlBindList binds;
};

/*!
    \qmlclass SqlQuery QmlSqlQuery
    \brief The SqlQuery element describes a query into an SQL database.

    The SqlQuery element has three parts.  The first is the query itself,
    which can either be specified using the query property or by the
    default text for the element.  The second specifies the connection
    to the database.  This can either be a bound id from an SqlConnection
    or the connections name.  If the connection is specified in a QML
    SqlConnection it is recommend to bind to the id to help ensure the
    database is complete before attempting to attach to it.  If no
    connectoin is specified the default connection is used.

    It is also possible to bind values into the query using the bindings
    property.  See SqlBind for more information on how to bind values into
    the query.

    If the query is a select statement it can be used as a model for a ListView.
    The roles will be the columns of the result set.  Use the SQL AS keyword
    in the query if you want to override the column names from that of the
    table selected.  You should also use the AS keyword if there is no
    appropriate table column name for the result column.

    \qml
    SqlQuery { connection: qmlConnectionId; query: "DELETE FROM mytable" }
    SqlQuery {
        connection: "connectionName"
        query: "SELECT * FROM mytable"
    }
    SqlQuery { query: "SELECT id AS recordId, (firstName || ' ' || lastName) AS fullName FROM mytable" }
    \endqml
*/

/*
    \class QmlSqlQuery
    \brief the QmlSqlQuery class manages a query into an SQL database.
*/

/*!
    \qmlproperty QString SqlQuery::query

    Defines the query text.
*/

/*!
    \qmlproperty QVariant SqlQuery::connection

    Defines the connection to an SQL database used by the query.
*/

/*!
    \qmlproperty QString SqlQuery::lastError

    Defines the last error, if one occurred, when working with the query.
*/
    
/*!
    \qmlproperty list<SqlBind> SqlQuery::bindings

    The bindings property contains the list of values to bind into the
    query.  See SqlBind for more information.
*/

/*!
    Constructs a QmlSqlQuery with the given \a parent.
*/
QmlSqlQuery::QmlSqlQuery(QObject *parent)
: QListModelInterface(*(new QmlSqlQueryPrivate(this)), parent)
{
}

/*!
    Destroys the QmlSqlQuery.
*/
QmlSqlQuery::~QmlSqlQuery()
{
    Q_D(QmlSqlQuery);
    if (d->query)
        delete d->query;
}

/*!
    Returns the query's bound variables.
*/
QmlList<QmlSqlBind *> *QmlSqlQuery::bindings()
{
    Q_D(QmlSqlQuery);
    return &d->binds;
}

/*!
    Returns the query's bound variables.
*/
const QmlList<QmlSqlBind *> *QmlSqlQuery::bindings() const
{
    Q_D(const QmlSqlQuery);
    return &d->binds;
}

/*!
    Returns the query text.

    \sa setQuery()
*/
QString QmlSqlQuery::query() const
{
    Q_D(const QmlSqlQuery);
    return d->queryText;
}

/*!
    Sets the query text to the given \a text.
*/
void QmlSqlQuery::setQuery(const QString &text)
{
    Q_D(QmlSqlQuery);
    if (text != d->queryText) {
        d->queryText = text;

        static const QLatin1String select("select");
        d-> isSel = text.trimmed().indexOf(select, 0, Qt::CaseInsensitive) == 0;

        if (d->query)
            resetQuery();
    }
}

/*!
    Returns the query's connection specifier.

    \sa setConnection()
*/
QVariant QmlSqlQuery::connection() const
{
    Q_D(const QmlSqlQuery);
    return d->connectionVariant;
}

/*!
    Sets the query's connection specifier.

    \sa connection()
*/
void QmlSqlQuery::setConnection(const QVariant &connection)
{
    Q_D(QmlSqlQuery);
    if (connection != d->connectionVariant) {
        d->connectionVariant = connection;
        if (d->query)
            resetQuery();
        else if (d->count == 0) // data has been requested
            d->prepareQuery();
    }
}

/*!
    Returns the set of values for a given set of requested \a roles for the
    specified \a row of the query result set.  Returns an empty hash if the
    query is not a select statement.

    \sa count(), roles(), toString()
*/
QHash<int,QVariant> QmlSqlQuery::data(int row, const QList<int> &roles) const
{
    Q_D(const QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();
    QHash<int, QVariant> result;

    if (!d->isSel)
        return result;

    Q_ASSERT(row >= 0 && row <= d->count);

    if (!d->requireCache)
        d->query->seek(row);
    else
        d->hitCache(row);

    for (int i = 0; i < roles.count(); ++i) {
        int column = roles[i];
        if (d->requireCache) 
            result.insert(column, d->cache[column]->at(row));
        else
            result.insert(column, d->query->value(column));
    }
    return result;
}

/*!
    Returns the number of rows in the query result set.  Returns 0 if
    the query is not a select statement.

    \sa data(), roles(), toString()
*/
int QmlSqlQuery::count() const
{
    Q_D(const QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();
    return d->count;
}

/*!
    Returns the list of role integer identifiers for the query result set.
    Returns and empty list if the query is not a select statement.

    \sa data(), count(), toString()
*/
QList<int> QmlSqlQuery::roles() const
{
    Q_D(const QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();

    if (!d->isSel)
        return QList<int>();

    if (d->roleNames.isEmpty() && !d->requireCache) {
        d->query->seek(0);
        d->grabRoles();
    }

    return d->roles;
}

/*!
    Returns the corresponding role name for the given \a role identifier.

    \sa data(), roles(), toString()
*/
QString QmlSqlQuery::toString(int role) const
{
    Q_D(const QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();

    if (d->roleNames.isEmpty() && !d->requireCache) {
        d->query->seek(0);
        d->grabRoles();
    }

    return d->roleNames[role];
}

/*!
    Returns information about the last error that occurred on the query.
*/
QString QmlSqlQuery::lastError() const
{
    Q_D(const QmlSqlQuery);
    if (d->query)
        return d->query->lastError().text();
    return QString();
}

/*!
    \reimp
*/
void QmlSqlQuery::componentComplete()
{
    Q_D(QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();
}

/*!
    \internal
    Rebinds the query, and if needed, emits rows inserted or rows
    added so any attached ListView elements will correctly.

    This slot is called automatically when the SqlBind elements in
    the bindings property of the query change.
*/
void QmlSqlQuery::resetBinds()
{
    Q_D(QmlSqlQuery);
    if (!d->query)
        return;
    int oldcount = d->count;
    d->clearCache();
    d->roles.clear();
    d->roleNames.clear();
    d->bindQuery();
    if (d->isSel) {
        if (!d->query->isActive()) {
            if (!d->query->exec())
                qWarning() << "failed to execute query" << d->query->lastQuery() << d->query->boundValues() << d->query->lastError().text();
        }
        d->initCache(); // may finish query
        emitChanges(oldcount);
    }
}

/*!
    Executes the query.  For SELECT statements this is normally only required
    if the database changes outside of the SQL query element.  Statements that
    modify the database, such as UPDATE and INSERT, will not be applied until
    this function is called.
*/
void QmlSqlQuery::exec()
{
    Q_D(QmlSqlQuery);
    if (!d->query)
        d->prepareQuery();
    Q_ASSERT(d->query);

    if (d->isSel) {
        int oldcount = d->count;
        d->clearCache();
        d->roles.clear();
        d->roleNames.clear();
        if (!d->query->exec())
            qWarning() << "failed to execute query" << d->query->lastQuery() << d->query->boundValues() << d->query->lastError().text();
        d->initCache(); // may finish query
        emitChanges(oldcount);
    } else {
        if (!d->query->exec())
            qWarning() << "failed to execute query" << d->query->lastQuery() << d->query->boundValues() << d->query->lastError().text();
        d->query->finish();
    }
}

/*!
    \internal

    Resets the query and query cache.
*/
void QmlSqlQuery::resetQuery()
{
    Q_D(QmlSqlQuery);
    Q_ASSERT(d->query != 0);
    delete d->query;
    d->query = 0;
    d->clearCache();
    d->roles.clear();
    d->roleNames.clear();
    int oldcount = d->count;
    d->prepareQuery();
    emitChanges(oldcount);
}

/*!
    \internal

    emits row number changes based of differences between the given
    \a oldcount and the current count of the query result set.
*/
void QmlSqlQuery::emitChanges(int oldcount)
{
    Q_D(QmlSqlQuery);
    if (d->count > oldcount)
        emit itemsInserted(oldcount, d->count-oldcount);
    else if (d->count < oldcount)
        emit itemsRemoved(d->count, oldcount-d->count);
    if (d->count > 0 && oldcount > 0)
        emit itemsChanged(0, qMin(d->count, oldcount), roles());
}

/*
    \internal

    Handles delayed caching of the query.
*/
void QmlSqlQuery::timerEvent(QTimerEvent *event)
{
    Q_D(QmlSqlQuery);
    if (event->timerId() == d->cacheTimer) {
        killTimer(d->cacheTimer);
        d->cacheTimer = -1;
        d->enactShift(d->scheduledRow);
        d->scheduledRow = -1;
    }
}

/*
    Prepares the query.  If the query starts with SELECT it is assumed to
    be a SELECT statement and the query is also executed.
*/
void QmlSqlQueryPrivate::prepareQuery() const
{
    QObject *object = qvariant_cast<QObject*>(connectionVariant);
    QmlSqlConnection *connection = qobject_cast<QmlSqlConnection *>(object);
    QString connectionString = qvariant_cast<QString>(connectionVariant);

    Q_ASSERT(query == 0);
    QSqlDatabase db = connection ? connection->database()
            : QSqlDatabase::database(connectionString.isEmpty()
                ? QLatin1String(QSqlDatabase::defaultConnection)
                : connectionString, false);

    if (!db.isOpen()) {
        count = 0;
        return;
    }

    query = new QSqlQuery(db);

    requireCache =
        query->driver()->hasFeature(QSqlDriver::SimpleLocking)
        || !query->driver()->hasFeature(QSqlDriver::QuerySize);

    if (requireCache)
        query->setForwardOnly(true);
    if (!query->prepare(queryText))
        qWarning() << "failed to prepare query" << query->lastQuery() << query->lastError().text();
    bindQuery();
    if (isSel) {
        if (!query->exec())
            qWarning() << "failed to execute query" << query->lastQuery() << query->boundValues() << query->lastError().text();
        initCache();
    }
}

/*
    Binds values into the prepared query using the bindings property of the SqlQuery element.
*/
void QmlSqlQueryPrivate::bindQuery() const
{
    for (int i = 0; i < binds.count(); ++i) {
        QmlSqlBind *bind = binds.at(i);
        if (bind->name().isEmpty()) {
            query->bindValue(i, bind->value());
        } else {
            query->bindValue(bind->name(), bind->value());
        }
    }
}

/*
    Clears cached query results
*/
void QmlSqlQueryPrivate::clearCache()
{
    for (int i = 0; i < cache.size(); ++i)
        delete cache[i];
    cache.resize(0);
}

/*
    If caching is required, initializes the cache with
    column definitions and initial rows.

    Otherwise caches the count for the query.
*/
void QmlSqlQueryPrivate::initCache() const
{
    if (requireCache) {
        int row = 0;
        if (query->next()) {
            if (roleNames.isEmpty()) {
                grabRoles();
                cache.resize(roleNames.count());
                for (int i = 0; i < cache.size(); ++i)
                    cache[i] = new Column(cacheSize);
            }
            Q_ASSERT(cache.size() > 0);
            do {
                for (int i = 0; i < cache.size(); ++i)
                    cache[i]->insert(row, query->value(i));
                row++;
            } while (!cache[0]->isFull() && query->next());
            while(query->next()) row++;
        }
        count = row;
        query->finish();
    } else {
        count = query->size();
    }
}

/*
    Schedules future background cache loading based of
    the given row.

    Should only be called if caching is active.
*/
void QmlSqlQueryPrivate::hitCache(int row) const
{
    Q_Q(const QmlSqlQuery);

    Q_ASSERT(cache.size() > 0 && requireCache);
    if (cache[0]->containsIndex(row)) {
        int fi = cache[0]->firstIndex();
        int li = cache[0]->lastIndex();
        if (fi > 0 && row < fi + cacheNear)
            scheduledRow = qMax(0, row + cacheRetain - cacheSize);
        else if (li < count-1 && row > li - cacheNear)
            scheduledRow = qMax(0, row - cacheRetain);

        if (scheduledRow != -1 && cacheTimer == -1)
            cacheTimer = ((QmlSqlQuery *)q)->startTimer(cacheInterval);
    } else {
        if (cacheTimer != -1) {
            ((QmlSqlQuery *)q)->killTimer(cacheTimer);
            cacheTimer = -1;
        }
        scheduledRow = -1;
        qDebug() << "cache miss for row:" << row << "count:" << count;
        enactShift(row);
    }
}

/*
    Load items from sql centered around row.
*/
void QmlSqlQueryPrivate::enactShift(int targetRow) const
{
    Q_ASSERT(query && cache.size());
    query->exec();
    int row = targetRow < cache[0]->firstIndex() ? targetRow : qMax(targetRow, cache[0]->lastIndex()+1);

    if (query->seek(row)) {
        do {
            for (int i = 0; i < cache.size(); ++i) {
                cache[i]->insert(row, query->value(i));
            }
            row++;
        } while(row < targetRow + cacheSize && query->next());
    }
}

/*
    Gets the column names for the roles of the SqlQuery element.

    The query must be active and on a valid row.
*/
void QmlSqlQueryPrivate::grabRoles() const
{
    Q_ASSERT(query);
    Q_ASSERT(query->isValid());
    Q_ASSERT(roleNames.isEmpty());
    Q_ASSERT(roles.isEmpty());

    QSqlRecord record = query->record();
    for (int i = 0; i < record.count(); ++i) {
        roleNames.append(record.fieldName(i));
        roles.append(i);
    }
}

QT_END_NAMESPACE
