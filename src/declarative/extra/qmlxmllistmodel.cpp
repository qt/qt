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
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QXmlNodeModelIndex>
#include <QBuffer>
#include <QNetworkRequest>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(XmlListModelRole, Role);
QML_DEFINE_TYPE(QmlXmlListModel, XmlListModel);

//TODO: do something smart while waiting for data to load
//      error handling (currently quite fragile)
//      profile doQuery and doSubquery
//      some sort of loading indication while we wait for initial data load (status property similar to QWebImage?)
//      support complex/nested objects?
//      how do we handle data updates (like rss feed -- usually items inserted at beginning)

/*!
    \qmlclass XmlListModel
    \brief The XmlListModel class allows you to specify a model using XQuery.

    XmlListModel allows you to construct a model from XML data that can then be used as a data source
    for the view classes (ListView, PathView, GridView) and any other classes that interact with model
    data (like Repeater).

    The following is an example of a model containing news from a Yahoo RSS feed:
    \qml
    <XmlListModel id="FeedModel" src="http://rss.news.yahoo.com/rss/oceania" query="doc($src)/rss/channel/item">
        <Role name="title" query="title/string()"/>
        <Role name="link" query="link/string()"/>
        <Role name="description" query="description/string()" isCData="true"/>
    </XmlListModel>
    \endqml
    \note The model is currently static, so the above is really just a snapshot of an RSS feed.
*/

class QmlXmlListModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlXmlListModel)
public:
    QmlXmlListModelPrivate() : size(-1), highestRole(Qt::UserRole), reply(0), context(0), roleObjects(this) {}

    QString src;
    QString query;
    QString namespaces;
    QList<int> roles;
    QStringList roleNames;
    mutable QList<QList<QVariant> > data;
    int size;
    int highestRole;
    QNetworkReply *reply;
    mutable QByteArray xml;
    QString prefix;
    QmlContext *context;

    struct RoleList : public QmlConcreteList<XmlListModelRole *>
    {
        RoleList(QmlXmlListModelPrivate *p)
        : model(p) {}
        virtual void append(XmlListModelRole *role) {
            QmlConcreteList<XmlListModelRole *>::append(role);
            model->roles << model->highestRole;
            model->roleNames << role->name();
            ++model->highestRole;
        }
        //XXX clear, removeAt, and insert need to invalidate any cached data (in data table) as well
        //    (and the model should emit the appropriate signals)
        virtual void clear()
        {
            model->roles.clear();
            model->roleNames.clear();
            QmlConcreteList<XmlListModelRole *>::clear();
        }
        virtual void removeAt(int i)
        {
            model->roles.removeAt(i);
            model->roleNames.removeAt(i);
            QmlConcreteList<XmlListModelRole *>::removeAt(i);
        }
        virtual void insert(int i, XmlListModelRole *role)
        {
            QmlConcreteList<XmlListModelRole *>::insert(i, role);
            model->roles.insert(i, model->highestRole);
            model->roleNames.insert(i, role->name());
            ++model->highestRole;
        }

        QmlXmlListModelPrivate *model;
    };

    RoleList roleObjects;
};

QmlXmlListModel::QmlXmlListModel(QObject *parent)
    : QListModelInterface(*(new QmlXmlListModelPrivate), parent)
{
    Q_D(QmlXmlListModel);
    d->context = QmlContext::activeContext();
}

QmlXmlListModel::~QmlXmlListModel()
{
}

QmlList<XmlListModelRole *> *QmlXmlListModel::roleObjects()
{
    Q_D(QmlXmlListModel);
    return &d->roleObjects;
}

QHash<int,QVariant> QmlXmlListModel::data(int index, const QList<int> &roles) const
{
    Q_D(const QmlXmlListModel);
    QHash<int, QVariant> rv;

    if (index > d->data.count() - 1)
        doSubquery(index);

    for (int i = 0; i < roles.size(); ++i) {
        int role = roles.at(i);
        int roleIndex = d->roles.indexOf(role);
        rv.insert(role, d->data.at(index).at(roleIndex));
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

QString QmlXmlListModel::src() const
{
    Q_D(const QmlXmlListModel);
    return d->src;
}

void QmlXmlListModel::setSrc(const QString &src)
{
    Q_D(QmlXmlListModel);
    d->src = src;
}

QString QmlXmlListModel::query() const
{
    Q_D(const QmlXmlListModel);
    return d->query;
}

void QmlXmlListModel::setQuery(const QString &query)
{
    Q_D(QmlXmlListModel);
    d->query = query;
}

QString QmlXmlListModel::namespaceDeclarations() const
{
    Q_D(const QmlXmlListModel);
    return d->namespaces;
}

void QmlXmlListModel::setNamespaceDeclarations(const QString &declarations)
{
    Q_D(QmlXmlListModel);
    d->namespaces = declarations;
}

void QmlXmlListModel::classComplete()
{
    fetch();
}

void QmlXmlListModel::fetch()
{
    Q_D(QmlXmlListModel);
    if (d->src.isEmpty()) {
        qWarning() << "Can't fetch empty src string";
        //clear existing data?
        //int count = d->data.count();
        //d->data.clear();
        //emit itemsRemoved(0, count);
        return;
    }

    QNetworkRequest req((QUrl(d->src)));
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    d->reply = d->context->engine()->networkAccessManager()->get(req);
    QObject::connect(d->reply, SIGNAL(finished()),
                    this, SLOT(requestFinished()));
}

void QmlXmlListModel::requestFinished()
{
    Q_D(QmlXmlListModel);
    if(d->reply->error() != QNetworkReply::NoError) {
        d->reply->deleteLater();
        d->reply = 0;
    } else {
        QByteArray data = d->reply->readAll();
        doQuery(data);
        d->reply->deleteLater();
        d->reply = 0;
    }
}

void QmlXmlListModel::doQuery(QByteArray &rawData)
{
    Q_D(QmlXmlListModel);
    QString r;
    QXmlQuery query;
    QBuffer rawBuffer(&rawData);
    rawBuffer.open(QIODevice::ReadOnly);
    query.bindVariable(QLatin1String("src"), &rawBuffer);
    query.setQuery(d->namespaces + d->query);
    query.evaluateTo(&r);
    //qDebug() << r;

    //always need a single root element
    QByteArray xml = "<dummy:items xmlns:dummy=\"http://qtsotware.com/dummy\">\n" + r.toUtf8() + "</dummy:items>";
    QBuffer b(&xml);
    b.open(QIODevice::ReadOnly);
    //qDebug() << xml;

    QString namespaces = QLatin1String("declare namespace dummy=\"http://qtsotware.com/dummy\";\n") + d->namespaces;
    QString prefix = QLatin1String("doc($inputDocument)/dummy:items") +
                     d->query.mid(d->query.lastIndexOf(QLatin1Char('/')));

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
        b.seek(0);
        prefix += QLatin1String("[%1]/");
    }
    //qDebug() << count;

    QXmlQuery subquery;
    subquery.bindVariable(QLatin1String("inputDocument"), &b);
    d->prefix = namespaces + prefix;
    d->xml = xml;

    d->size = count;
    emit itemsInserted(0, count);
}

void QmlXmlListModel::doSubquery(int index) const
{
    Q_D(const QmlXmlListModel);
    //qDebug() << "doSubQuery" << index;
    QBuffer b(&d->xml);
    b.open(QIODevice::ReadOnly);

    QXmlQuery subquery;
    subquery.bindVariable(QLatin1String("inputDocument"), &b);

    //XXX should we use an array of objects or something else rather than a table?
    for (int j = d->data.count(); j <= index; ++j) {
        QList<QVariant> resultList;
        for (int i = 0; i < d->roleObjects.size(); ++i) {
            XmlListModelRole *role = d->roleObjects.at(i);
            subquery.setQuery(d->prefix.arg(j+1) + role->query());
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
                resultList << s;
                //qDebug() << s;
            }
            b.seek(0);
        }
        d->data << resultList;
    }
}

QT_END_NAMESPACE
