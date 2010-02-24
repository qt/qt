/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlxmllistmodel_p.h"

#include <qmlcontext.h>
#include <qmlengine.h>

#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QXmlNodeModelIndex>
#include <QBuffer>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,XmlRole,QmlXmlListModelRole)
QML_DEFINE_TYPE(Qt,4,6,XmlListModel,QmlXmlListModel)

typedef QPair<int, int> QmlXmlListRange;

/*!
    \qmlclass XmlRole QmlXmlListModelRole
  \since 4.7
    \brief The XmlRole element allows you to specify a role for an XmlListModel.
*/

/*!
    \qmlproperty string XmlRole::name
    The name for the role. This name is used to access the model data for this role from Qml.

    \qml
    XmlRole { name: "title"; query: "title/string()" }

    ...

    Component {
        id: myDelegate
        Text { text: title }
    }
    \endqml
*/

/*!
    \qmlproperty string XmlRole::query
    The relative XPath query for this role. The query should not start with a '/' (i.e. it must be
    relative).

    \qml
    XmlRole { name: "title"; query: "title/string()" }
    \endqml
*/

/*!
    \qmlproperty bool XmlRole::isKey
    Defines whether this is a key role.
    
    Key roles are used to to determine whether a set of values should
    be updated or added to the XML list model when XmlListModel::reload()
    is called.

    \sa XmlListModel
*/

class Q_DECLARATIVE_EXPORT QmlXmlListModelRole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString query READ query WRITE setQuery)
    Q_PROPERTY(bool isKey READ isKey WRITE setIsKey)

public:
    QmlXmlListModelRole() : m_isKey(false) {}
    ~QmlXmlListModelRole() {}

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString query() const { return m_query; }
    void setQuery(const QString &query)
    {
        if (query.startsWith(QLatin1Char('/'))) {
            qmlInfo(this) << tr("An XmlRole query must not start with '/'");
            return;
        }
        m_query = query;
    }

    bool isKey() const { return m_isKey; }
    void setIsKey(bool b) { m_isKey = b; }

    bool isValid() {
        return !m_name.isEmpty() && !m_query.isEmpty();
    }

private:
    QString m_name;
    QString m_query;
    bool m_isKey;
};
QT_END_NAMESPACE
QML_DECLARE_TYPE(QmlXmlListModelRole)
QT_BEGIN_NAMESPACE


class QmlXmlQuery : public QThread
{
    Q_OBJECT
public:
    QmlXmlQuery(QObject *parent=0)
        : QThread(parent), m_quit(false), m_restart(false), m_abort(false), m_queryId(0) {
    }
    ~QmlXmlQuery() {
        m_mutex.lock();
        m_quit = true;
        m_condition.wakeOne();
        m_mutex.unlock();

        wait();
    }

    void abort() {
        QMutexLocker locker(&m_mutex);
        m_abort = true;
    }

    int doQuery(QString query, QString namespaces, QByteArray data, QList<QmlXmlListModelRole *> *roleObjects) {
        QMutexLocker locker(&m_mutex);
        m_size = 0;
        m_data = data;
        m_query = QLatin1String("doc($src)") + query;
        m_namespaces = namespaces;
        m_roleObjects = roleObjects;
        if (!isRunning()) {
            m_abort = false;
            start();
        } else {
            m_restart = true;
            m_condition.wakeOne();
        }
        m_queryId++;
        return m_queryId;
    }

    QList<QList<QVariant> > modelData() {
        QMutexLocker locker(&m_mutex);
        return m_modelData;
    }

    QList<QmlXmlListRange> insertedItemRanges() {
        QMutexLocker locker(&m_mutex);
        return m_insertedItemRanges;
    }

    QList<QmlXmlListRange> removedItemRanges() {
        QMutexLocker locker(&m_mutex);
        return m_removedItemRanges;
    }

Q_SIGNALS:
    void queryCompleted(int queryId, int size);

protected:
    void run() {
        while (!m_quit) {
            m_mutex.lock();
            int queryId = m_queryId;
            doQueryJob();
            doSubQueryJob();
            m_data.clear(); // no longer needed
            m_mutex.unlock();

            m_mutex.lock();
            if (!m_abort)
                emit queryCompleted(queryId, m_size);
            if (!m_restart)
                m_condition.wait(&m_mutex);
            m_abort = false;
            m_restart = false;
            m_mutex.unlock();
        }
    }

private:
    void doQueryJob();
    void doSubQueryJob();
    void getValuesOfKeyRoles(QStringList *values, QXmlQuery *query) const;
    void addIndexToRangeList(QList<QmlXmlListRange> *ranges, int index) const;

private:
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_quit;
    bool m_restart;
    bool m_abort;
    QByteArray m_data;
    QString m_query;
    QString m_namespaces;
    QString m_prefix;
    int m_size;
    int m_queryId;
    const QList<QmlXmlListModelRole *> *m_roleObjects;
    QList<QList<QVariant> > m_modelData;
    QStringList m_keysValues;
    QList<QmlXmlListRange> m_insertedItemRanges;
    QList<QmlXmlListRange> m_removedItemRanges;
};

void QmlXmlQuery::doQueryJob()
{
    QString r;
    QXmlQuery query;
    QBuffer buffer(&m_data);
    buffer.open(QIODevice::ReadOnly);
    query.bindVariable(QLatin1String("src"), &buffer);
    query.setQuery(m_namespaces + m_query);
    query.evaluateTo(&r);

    //qDebug() << r;

    //always need a single root element
    QByteArray xml = "<dummy:items xmlns:dummy=\"http://qtsotware.com/dummy\">\n" + r.toUtf8() + "</dummy:items>";
    QBuffer b(&xml);
    b.open(QIODevice::ReadOnly);
    //qDebug() << xml;

    QString namespaces = QLatin1String("declare namespace dummy=\"http://qtsotware.com/dummy\";\n") + m_namespaces;
    QString prefix = QLatin1String("doc($inputDocument)/dummy:items") +
                     m_query.mid(m_query.lastIndexOf(QLatin1Char('/')));

    //figure out how many items we are dealing with
    int count = -1;
    {
        QXmlResultItems result;
        QXmlQuery countquery;
        countquery.bindVariable(QLatin1String("inputDocument"), &b);
        countquery.setQuery(namespaces + QLatin1String("count(") + prefix + QLatin1Char(')'));
        countquery.evaluateTo(&result);
        QXmlItem item(result.next());
        if (item.isAtomicValue())
            count = item.toAtomicValue().toInt();
    }
    //qDebug() << count;

    m_prefix = namespaces + prefix + QLatin1Char('/');
    m_data = xml;
    if (count > 0)
        m_size = count;
}

void QmlXmlQuery::getValuesOfKeyRoles(QStringList *values, QXmlQuery *query) const
{
    QStringList keysQueries;
    for (int i=0; i<m_roleObjects->count(); i++) {
        if (m_roleObjects->at(i)->isKey())
            keysQueries << m_roleObjects->at(i)->query();
    }
    QString keysQuery;
    if (keysQueries.count() == 1)
        keysQuery = m_prefix + keysQueries[0];
    else if (keysQueries.count() > 1)
        keysQuery = m_prefix + QLatin1String("concat(") + keysQueries.join(QLatin1String(",")) + QLatin1String(")");

    if (!keysQuery.isEmpty()) {
        query->setQuery(keysQuery);
        QXmlResultItems resultItems;
        query->evaluateTo(&resultItems);
        QXmlItem item(resultItems.next());
        while (!item.isNull()) {
            values->append(item.toAtomicValue().toString());
            item = resultItems.next();
        }
    }
}

void QmlXmlQuery::addIndexToRangeList(QList<QmlXmlListRange> *ranges, int index) const {
    if (ranges->isEmpty())
        ranges->append(qMakePair(index, 1));
    else if (ranges->last().first + ranges->last().second == index)
        ranges->last().second += 1;
    else
        ranges->append(qMakePair(index, 1));
}

void QmlXmlQuery::doSubQueryJob()
{
    m_modelData.clear();

    QBuffer b(&m_data);
    b.open(QIODevice::ReadOnly);

    QXmlQuery subquery;
    subquery.bindVariable(QLatin1String("inputDocument"), &b);

    QStringList keysValues;
    getValuesOfKeyRoles(&keysValues, &subquery);

    // See if any values of key roles have been inserted or removed.
    m_insertedItemRanges.clear();
    m_removedItemRanges.clear();
    if (m_keysValues.isEmpty()) {
        m_insertedItemRanges << qMakePair(0, m_size);
    } else {
        if (keysValues != m_keysValues) {
            QStringList temp;
            for (int i=0; i<m_keysValues.count(); i++) {
                if (!keysValues.contains(m_keysValues[i]))
                    addIndexToRangeList(&m_removedItemRanges, i);
                else 
                    temp << m_keysValues[i];
            }

            for (int i=0; i<keysValues.count(); i++) {
                if (temp.count() == i || keysValues[i] != temp[i]) {
                    temp.insert(i, keysValues[i]);
                    addIndexToRangeList(&m_insertedItemRanges, i);
                }
            }
        }
    }
    m_keysValues = keysValues;

    // Get the new values for each role.
    //### we might be able to condense even further (query for everything in one go)
    for (int i = 0; i < m_roleObjects->size(); ++i) {
        QmlXmlListModelRole *role = m_roleObjects->at(i);
        if (!role->isValid()) {
            QList<QVariant> resultList;
            for (int j = 0; j < m_size; ++j)
                resultList << QVariant();
            m_modelData << resultList;
            continue;
        }
        subquery.setQuery(m_prefix + QLatin1String("(let $v := ") + role->query() + QLatin1String(" return if ($v) then ") + role->query() + QLatin1String(" else \"\")"));
        QXmlResultItems resultItems;
        subquery.evaluateTo(&resultItems);
        QXmlItem item(resultItems.next());
        QList<QVariant> resultList;
        while (!item.isNull()) {
            resultList << item.toAtomicValue(); //### we used to trim strings
            item = resultItems.next();
        }
        //### should warn here if things have gone wrong.
        while (resultList.count() < m_size)
            resultList << QVariant();
        m_modelData << resultList;
        b.seek(0);
    }

    //this method is much slower, but works better for incremental loading
    /*for (int j = 0; j < m_size; ++j) {
        QList<QVariant> resultList;
        for (int i = 0; i < m_roleObjects->size(); ++i) {
            QmlXmlListModelRole *role = m_roleObjects->at(i);
            subquery.setQuery(m_prefix.arg(j+1) + role->query());
            if (role->isStringList()) {
                QStringList data;
                subquery.evaluateTo(&data);
                resultList << QVariant(data);
                //qDebug() << data;
            } else {
                QString s;
                subquery.evaluateTo(&s);
                if (role->isCData()) {
                    //un-escape
                    s.replace(QLatin1String("&lt;"), QLatin1String("<"));
                    s.replace(QLatin1String("&gt;"), QLatin1String(">"));
                    s.replace(QLatin1String("&amp;"), QLatin1String("&"));
                }
                resultList << s.trimmed();
                //qDebug() << s;
            }
            b.seek(0);
        }
        m_modelData << resultList;
    }*/
}

class QmlXmlListModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlXmlListModel)
public:
    QmlXmlListModelPrivate()
        : isComponentComplete(true), size(-1), highestRole(Qt::UserRole)
        , reply(0), status(QmlXmlListModel::Null), progress(0.0)
        , queryId(-1), roleObjects() {}

    bool isComponentComplete;
    QUrl src;
    QString xml;
    QString query;
    QString namespaces;
    int size;
    QList<int> roles;
    QStringList roleNames;
    int highestRole;
    QNetworkReply *reply;
    QmlXmlListModel::Status status;
    qreal progress;
    QmlXmlQuery qmlXmlQuery;
    int queryId;
    QList<QmlXmlListModelRole *> roleObjects;
    static void append_role(QmlListProperty<QmlXmlListModelRole> *list, QmlXmlListModelRole *role);
    static void clear_role(QmlListProperty<QmlXmlListModelRole> *list);
    static void removeAt_role(QmlListProperty<QmlXmlListModelRole> *list, int i);
    static void insert_role(QmlListProperty<QmlXmlListModelRole> *list, int i, QmlXmlListModelRole *role);
    QList<QList<QVariant> > data;
};


void QmlXmlListModelPrivate::append_role(QmlListProperty<QmlXmlListModelRole> *list, QmlXmlListModelRole *role)
{
    QmlXmlListModel *_this = qobject_cast<QmlXmlListModel *>(list->object);
    if (_this) {
        int i = _this->d_func()->roleObjects.count();
        _this->d_func()->roleObjects.append(role);
        if (_this->d_func()->roleNames.contains(role->name())) {
            qmlInfo(role) << QObject::tr("\"%1\" duplicates a previous role name and will be disabled.").arg(role->name());
            return;
        }
        _this->d_func()->roles.insert(i, _this->d_func()->highestRole);
        _this->d_func()->roleNames.insert(i, role->name());
        ++_this->d_func()->highestRole;
    }
}

//### clear needs to invalidate any cached data (in data table) as well
//    (and the model should emit the appropriate signals)
void QmlXmlListModelPrivate::clear_role(QmlListProperty<QmlXmlListModelRole> *list)
{
    QmlXmlListModel *_this = static_cast<QmlXmlListModel *>(list->object);
    _this->d_func()->roles.clear();
    _this->d_func()->roleNames.clear();
    _this->d_func()->roleObjects.clear();
}

/*!
    \class QmlXmlListModel
    \internal
*/

/*!
    \qmlclass XmlListModel QmlXmlListModel
  \since 4.7
    \brief The XmlListModel element is used to specify a model using XPath expressions.

    XmlListModel is used to create a model from XML data that can be used as a data source
    for the view classes (such as ListView, PathView, GridView) and other classes that interact with model
    data (such as Repeater).

    Here is an example of a model containing news from a Yahoo RSS feed:
    \qml
    XmlListModel {
        id: feedModel
        source: "http://rss.news.yahoo.com/rss/oceania"
        query: "/rss/channel/item"
        XmlRole { name: "title"; query: "title/string()" }
        XmlRole { name: "pubDate"; query: "pubDate/string()" }
        XmlRole { name: "description"; query: "description/string()" }
    }
    \endqml

    You can also define certain roles as "keys" so that the model only adds data
    that contains new values for these keys when reload() is called.

    For example, if the roles above were defined like this:

    \qml
        XmlRole { name: "title"; query: "title/string()"; isKey: true }
        XmlRole { name: "pubDate"; query: "pubDate/string()"; isKey: true }
    \endqml

    Then when reload() is called, the model will only add new items with a
    "title" and "pubDate" value combination that is not already present in
    the model.

    This is useful to provide incremental updates and avoid repainting an
    entire model in a view.
*/

QmlXmlListModel::QmlXmlListModel(QObject *parent)
    : QListModelInterface(*(new QmlXmlListModelPrivate), parent)
{
    Q_D(QmlXmlListModel);
    connect(&d->qmlXmlQuery, SIGNAL(queryCompleted(int,int)),
            this, SLOT(queryCompleted(int,int)));
}

QmlXmlListModel::~QmlXmlListModel()
{
}

/*!
    \qmlproperty list<XmlRole> XmlListModel::roles

    The roles to make available for this model.
*/
QmlListProperty<QmlXmlListModelRole> QmlXmlListModel::roleObjects()
{
    Q_D(QmlXmlListModel);
    QmlListProperty<QmlXmlListModelRole> list(this, d->roleObjects);
    list.append = &QmlXmlListModelPrivate::append_role;
    list.clear = &QmlXmlListModelPrivate::clear_role;
    return list;
}

QHash<int,QVariant> QmlXmlListModel::data(int index, const QList<int> &roles) const
{
    Q_D(const QmlXmlListModel);
    QHash<int, QVariant> rv;
    for (int i = 0; i < roles.size(); ++i) {
        int role = roles.at(i);
        int roleIndex = d->roles.indexOf(role);
        rv.insert(role, roleIndex == -1 ? QVariant() : d->data.at(roleIndex).at(index));
    }
    return rv;
}

QVariant QmlXmlListModel::data(int index, int role) const
{
    Q_D(const QmlXmlListModel);
    int roleIndex = d->roles.indexOf(role);
    return (roleIndex == -1) ? QVariant() : d->data.at(roleIndex).at(index);
}

/*!
    \qmlproperty int XmlListModel::count
    The number of data entries in the model.
*/
int QmlXmlListModel::count() const
{
    Q_D(const QmlXmlListModel);
    return d->size;
}

QList<int> QmlXmlListModel::roles() const
{
    Q_D(const QmlXmlListModel);
    return d->roles;
}

QString QmlXmlListModel::toString(int role) const
{
    Q_D(const QmlXmlListModel);
    int index = d->roles.indexOf(role);
    if (index == -1)
        return QString();
    return d->roleNames.at(index);
}

/*!
    \qmlproperty url XmlListModel::source
    The location of the XML data source.

    If both source and xml are set, xml will be used.
*/
QUrl QmlXmlListModel::source() const
{
    Q_D(const QmlXmlListModel);
    return d->src;
}

void QmlXmlListModel::setSource(const QUrl &src)
{
    Q_D(QmlXmlListModel);
    if (d->src != src) {
        d->src = src;
        reload();
    }
}

/*!
    \qmlproperty string XmlListModel::xml
    This property holds XML text set directly.

    The text is assumed to be UTF-8 encoded.

    If both source and xml are set, xml will be used.
*/
QString QmlXmlListModel::xml() const
{
    Q_D(const QmlXmlListModel);
    return d->xml;
}

void QmlXmlListModel::setXml(const QString &xml)
{
    Q_D(QmlXmlListModel);
    d->xml = xml;
    reload();
}

/*!
    \qmlproperty string XmlListModel::query
    An absolute XPath query representing the base query for the model items. The query should start with
    '/' or '//'.
*/
QString QmlXmlListModel::query() const
{
    Q_D(const QmlXmlListModel);
    return d->query;
}

void QmlXmlListModel::setQuery(const QString &query)
{
    Q_D(QmlXmlListModel);
    if (!query.startsWith(QLatin1Char('/'))) {
        qmlInfo(this) << QCoreApplication::translate("QmlXmlRoleList", "An XmlListModel query must start with '/' or \"//\"");
        return;
    }

    if (d->query != query) {
        d->query = query;
        reload();
    }
}

/*!
    \qmlproperty string XmlListModel::namespaceDeclarations
    A set of declarations for the namespaces used in the query.
*/
QString QmlXmlListModel::namespaceDeclarations() const
{
    Q_D(const QmlXmlListModel);
    return d->namespaces;
}

void QmlXmlListModel::setNamespaceDeclarations(const QString &declarations)
{
    Q_D(QmlXmlListModel);
    if (d->namespaces != declarations) {
        d->namespaces = declarations;
        reload();
    }
}

/*!
    \qmlproperty enum XmlListModel::status

    This property holds the status of data source loading.  It can be one of:
    \list
    \o Null - no data source has been set
    \o Ready - nthe data source has been loaded
    \o Loading - the data source is currently being loaded
    \o Error - an error occurred while loading the data source
    \endlist

    \sa progress

*/
QmlXmlListModel::Status QmlXmlListModel::status() const
{
    Q_D(const QmlXmlListModel);
    return d->status;
}

/*!
    \qmlproperty real XmlListModel::progress

    This property holds the progress of data source loading, from 0.0 (nothing loaded)
    to 1.0 (finished).

    \sa status
*/
qreal QmlXmlListModel::progress() const
{
    Q_D(const QmlXmlListModel);
    return d->progress;
}

void QmlXmlListModel::classBegin()
{
    Q_D(QmlXmlListModel);
    d->isComponentComplete = false;
}

void QmlXmlListModel::componentComplete()
{
    Q_D(QmlXmlListModel);
    d->isComponentComplete = true;
    reload();
}

/*!
    \qmlmethod XmlListModel::reload()

    Reloads the model.
    
    If no key roles have been specified, all existing model
    data is removed, and the model is rebuilt from scratch.

    Otherwise, items are only added if the model does not already
    contain items with matching key role values.
*/
void QmlXmlListModel::reload()
{
    Q_D(QmlXmlListModel);

    if (!d->isComponentComplete)
        return;

    d->qmlXmlQuery.abort();
    d->queryId = -1;

    if (d->size < 0)
        d->size = 0;

    if (d->src.isEmpty() && d->xml.isEmpty())
        return;

    if (d->reply) {
        d->reply->abort();
        d->reply->deleteLater();
        d->reply = 0;
    }

    if (!d->xml.isEmpty()) {
        d->queryId = d->qmlXmlQuery.doQuery(d->query, d->namespaces, d->xml.toUtf8(), &d->roleObjects);
        d->progress = 1.0;
        d->status = Ready;
        emit progressChanged(d->progress);
        emit statusChanged(d->status);
        return;
    }

    d->progress = 0.0;
    d->status = Loading;
    emit progressChanged(d->progress);
    emit statusChanged(d->status);

    QNetworkRequest req(d->src);
    d->reply = qmlContext(this)->engine()->networkAccessManager()->get(req);
    QObject::connect(d->reply, SIGNAL(finished()), this, SLOT(requestFinished()));
    QObject::connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                     this, SLOT(requestProgress(qint64,qint64)));
}

void QmlXmlListModel::requestFinished()
{
    Q_D(QmlXmlListModel);
    if (d->reply->error() != QNetworkReply::NoError) {
        disconnect(d->reply, 0, this, 0);
        d->reply->deleteLater();
        d->reply = 0;
        d->status = Error;
    } else {
        d->status = Ready;
        QByteArray data = d->reply->readAll();
        d->queryId = d->qmlXmlQuery.doQuery(d->query, d->namespaces, data, &d->roleObjects);
        disconnect(d->reply, 0, this, 0);
        d->reply->deleteLater();
        d->reply = 0;
    }
    d->progress = 1.0;
    emit progressChanged(d->progress);
    emit statusChanged(d->status);
}

void QmlXmlListModel::requestProgress(qint64 received, qint64 total)
{
    Q_D(QmlXmlListModel);
    if (d->status == Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}

void QmlXmlListModel::queryCompleted(int id, int size)
{
    Q_D(QmlXmlListModel);
    if (id != d->queryId)
        return;
    bool sizeChanged = size != d->size;
    d->size = size;
    d->data = d->qmlXmlQuery.modelData();

    QList<QmlXmlListRange> removed = d->qmlXmlQuery.removedItemRanges();
    for (int i=0; i<removed.count(); i++)
        emit itemsRemoved(removed[i].first, removed[i].second);
    QList<QmlXmlListRange> inserted = d->qmlXmlQuery.insertedItemRanges();
    for (int i=0; i<inserted.count(); i++)
        emit itemsInserted(inserted[i].first, inserted[i].second);

    if (sizeChanged)
        emit countChanged();
}

QT_END_NAMESPACE

#include <qmlxmllistmodel.moc>
