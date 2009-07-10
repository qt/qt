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

#include "qmlxmllistmodel.h"
#include "private/qobject_p.h"

#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlengine.h>
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

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(XmlListModelRole, XmlRole)
QML_DEFINE_TYPE(QmlXmlListModel, XmlListModel)

class QmlXmlListModelPrivate;
struct QmlXmlRoleList : public QmlConcreteList<XmlListModelRole *>
{
    QmlXmlRoleList(QmlXmlListModelPrivate *p)
        : model(p) {}
    virtual void append(XmlListModelRole *role);
    //XXX clear, removeAt, and insert need to invalidate any cached data (in data table) as well
    //    (and the model should emit the appropriate signals)
    virtual void clear();
    virtual void removeAt(int i);
    virtual void insert(int i, XmlListModelRole *role);

    QmlXmlListModelPrivate *model;
};



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

    int doQuery(QString query, QString namespaces, QByteArray data, QmlXmlRoleList *roleObjects) {
        QMutexLocker locker(&m_mutex);
        m_modelData.clear();
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

signals:
    void queryCompleted(int queryId, int size);

protected:
    void run() {
        while (!m_quit) {
            m_mutex.lock();
            int queryId = m_queryId;
            doQueryJob();
            if (m_size > 0)
                doSubQueryJob();
            m_data.clear(); // no longer needed
            m_mutex.unlock();

            m_mutex.lock();
            if (!m_abort && m_size > 0)
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
    const QmlXmlRoleList *m_roleObjects;
    QList<QList<QVariant> > m_modelData;
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
        countquery.setQuery(namespaces + QLatin1String("count(") + prefix + QLatin1String(")"));
        countquery.evaluateTo(&result);
        QXmlItem item(result.next());
        if (item.isAtomicValue())
            count = item.toAtomicValue().toInt();
    }
    //qDebug() << count;

    m_prefix = namespaces + prefix + QLatin1String("/");
    m_data = xml;
    if (count > 0)
        m_size = count;
}

void QmlXmlQuery::doSubQueryJob()
{
    m_modelData.clear();

    QBuffer b(&m_data);
    b.open(QIODevice::ReadOnly);

    QXmlQuery subquery;
    subquery.bindVariable(QLatin1String("inputDocument"), &b);

    //### we might be able to condense even further (query for everything in one go)
    for (int i = 0; i < m_roleObjects->size(); ++i) {
        XmlListModelRole *role = m_roleObjects->at(i);
        if (!role->isValid()) {
            QList<QVariant> resultList;
            for (int j = 0; j < m_size; ++j)
                resultList << QVariant();
            m_modelData << resultList;
            continue;
        }
        subquery.setQuery(m_prefix + QLatin1String("(let $v := ") + role->query() + QLatin1String(" return if ($v) then ") + role->query() + QLatin1String(" else \"\")"));
        QXmlResultItems output3;
        subquery.evaluateTo(&output3);
        QXmlItem item(output3.next());
        QList<QVariant> resultList;
        while (!item.isNull()) {
            resultList << item.toAtomicValue(); //### we used to trim strings
            item = output3.next();
        }
        //### should warn here if things have gone wrong.
        while (resultList.count() < m_size)
            resultList << QVariant();
        m_modelData << resultList;
        b.seek(0);
    }

    //XXX this method is much slower, but would work better for incremental loading
    /*for (int j = 0; j < m_size; ++j) {
        QList<QVariant> resultList;
        for (int i = 0; i < m_roleObjects->size(); ++i) {
            XmlListModelRole *role = m_roleObjects->at(i);
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


//TODO: do something smart while waiting for data to load
//      error handling (currently quite fragile)
//      profile doQuery and doSubquery
//      some sort of loading indication while we wait for initial data load (status property similar to QWebImage?)
//      support complex/nested objects?
//      how do we handle data updates (like rss feed -- usually items inserted at beginning)


class QmlXmlListModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlXmlListModel)
public:
    QmlXmlListModelPrivate()
        : isComponentComplete(true), size(-1), highestRole(Qt::UserRole)
        , reply(0), status(QmlXmlListModel::Idle), progress(0.0)
        , queryId(-1), roleObjects(this) {}

    bool isComponentComplete;
    QUrl src;
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
    QmlXmlRoleList roleObjects;
    QList<QList<QVariant> > data;
};


void QmlXmlRoleList::append(XmlListModelRole *role) {
    QmlConcreteList<XmlListModelRole *>::append(role);
    model->roles << model->highestRole;
    model->roleNames << role->name();
    ++model->highestRole;
}

/*!
    \qmlclass XmlRole
    \brief The XmlRole element allows you to specify a role for an XmlListModel.
*/

/*!
    \qmlproperty string XmlRole::name
    The name for the role. This name is used to access the model data for this role from Qml.

    \qml
    XmlRole { name: "title"; query: "title/string()" }

    ...

    Component {
        id: Delegate
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

//XXX clear, removeAt, and insert need to invalidate any cached data (in data table) as well
//    (and the model should emit the appropriate signals)
void QmlXmlRoleList::clear()
{
    model->roles.clear();
    model->roleNames.clear();
    QmlConcreteList<XmlListModelRole *>::clear();
}

void QmlXmlRoleList::removeAt(int i)
{
    model->roles.removeAt(i);
    model->roleNames.removeAt(i);
    QmlConcreteList<XmlListModelRole *>::removeAt(i);
}

//### we should enforce unique role names
void QmlXmlRoleList::insert(int i, XmlListModelRole *role)
{
    QmlConcreteList<XmlListModelRole *>::insert(i, role);
    model->roles.insert(i, model->highestRole);
    model->roleNames.insert(i, role->name());
    ++model->highestRole;
}

/*!
    \qmlclass XmlListModel
    \brief The XmlListModel element allows you to specify a model using XPath expressions.

    XmlListModel allows you to construct a model from XML data that can then be used as a data source
    for the view classes (ListView, PathView, GridView) and any other classes that interact with model
    data (like Repeater).

    The following is an example of a model containing news from a Yahoo RSS feed:
    \qml
    XmlListModel {
        id: FeedModel
        source: "http://rss.news.yahoo.com/rss/oceania"
        query: "/rss/channel/item"
        XmlRole { name: "title"; query: "title/string()" }
        XmlRole { name: "link"; query: "link/string()" }
        XmlRole { name: "description"; query: "description/string()" }
    }
    \endqml
    \note The model is currently static, so the above is really just a snapshot of an RSS feed. To force a
    reload of the entire model, you can call the reload function.
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
    \qmlproperty list<XmlRole> QmlListModel::roles

    The roles to make available for this model.
*/
QmlList<XmlListModelRole *> *QmlXmlListModel::roleObjects()
{
    Q_D(QmlXmlListModel);
    return &d->roleObjects;
}

QHash<int,QVariant> QmlXmlListModel::data(int index, const QList<int> &roles) const
{
    Q_D(const QmlXmlListModel);
    QHash<int, QVariant> rv;
    for (int i = 0; i < roles.size(); ++i) {
        int role = roles.at(i);
        int roleIndex = d->roles.indexOf(role);
        rv.insert(role, d->data.at(roleIndex).at(index));
    }
    return rv;
}

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
    \qmlproperty url XmlListModel::query
    An absolute XPath query representing the base query for the model items. The query should start with
    a '/' or '//'.
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
        qmlInfo(this) << "An XmlListModel query must start with '/' or \"//\"";
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
    \o Idle - no data source has been set, or the data source has been loaded
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

    Reloads the model. All the existing model data will be removed, and the model
    will be rebuilt from scratch.
*/
void QmlXmlListModel::reload()
{
    Q_D(QmlXmlListModel);

    if (!d->isComponentComplete)
        return;

    d->qmlXmlQuery.abort();
    d->queryId = -1;

    //clear existing data
    int count = d->size;
    d->size = 0;
    d->data.clear();
    if (count > 0)
        emit itemsRemoved(0, count);

    if (d->src.isEmpty()) {
        qmlInfo(this) << "Can't load empty src string";
        return;
    }

    if (d->reply) {
        d->reply->abort();
        d->reply->deleteLater();
        d->reply = 0;
    }
    d->progress = 0.0;
    d->status = Loading;
    emit progressChanged(d->progress);
    emit statusChanged(d->status);

    QNetworkRequest req(d->src);
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    d->reply = qmlContext(this)->engine()->networkAccessManager()->get(req);
    QObject::connect(d->reply, SIGNAL(finished()), this, SLOT(requestFinished()));
    QObject::connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                     this, SLOT(requestProgress(qint64,qint64)));
}

void QmlXmlListModel::requestFinished()
{
    Q_D(QmlXmlListModel);
    if (d->reply->error() != QNetworkReply::NoError) {
        d->reply->deleteLater();
        d->reply = 0;
        d->status = Error;
    } else {
        d->status = Idle;
        QByteArray data = d->reply->readAll();
        d->queryId = d->qmlXmlQuery.doQuery(d->query, d->namespaces, data, &d->roleObjects);
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
    d->size = size;
    if (size > 0) {
        d->data = d->qmlXmlQuery.modelData();
        emit itemsInserted(0, d->size);
    }
}

QT_END_NAMESPACE

#include "qmlxmllistmodel.moc"
