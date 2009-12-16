/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLXMLLISTMODEL_H
#define QMLXMLLISTMODEL_H

#include <qml.h>
#include <qmlinfo.h>

#include "../3rdparty/qlistmodelinterface_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlContext;

class QmlXmlListModelRole;

class QmlXmlListModelPrivate;
class Q_DECLARATIVE_EXPORT QmlXmlListModel : public QListModelInterface, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)
    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource)
    Q_PROPERTY(QString xml READ xml WRITE setXml)
    Q_PROPERTY(QString query READ query WRITE setQuery)
    Q_PROPERTY(QString namespaceDeclarations READ namespaceDeclarations WRITE setNamespaceDeclarations)
    Q_PROPERTY(QmlList<QmlXmlListModelRole *> *roles READ roleObjects)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_CLASSINFO("DefaultProperty", "roles")

public:
    QmlXmlListModel(QObject *parent = 0);
    ~QmlXmlListModel();

    virtual QHash<int,QVariant> data(int index, const QList<int> &roles = (QList<int>())) const;
    virtual QVariant data(int index, int role) const;
    virtual int count() const;
    virtual QList<int> roles() const;
    virtual QString toString(int role) const;

    QmlList<QmlXmlListModelRole *> *roleObjects();

    QUrl source() const;
    void setSource(const QUrl&);

    QString xml() const;
    void setXml(const QString&);

    QString query() const;
    void setQuery(const QString&);

    QString namespaceDeclarations() const;
    void setNamespaceDeclarations(const QString&);

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    virtual void classBegin();
    virtual void componentComplete();

Q_SIGNALS:
    void statusChanged(Status);
    void progressChanged(qreal progress);
    void countChanged();

public Q_SLOTS:
    // ### need to use/expose Expiry to guess when to call this?
    // ### property to auto-call this on reasonable Expiry?
    // ### LastModified/Age also useful to guess.
    // ### Probably also applies to other network-requesting types.
    void reload();

private Q_SLOTS:
    void requestFinished();
    void requestProgress(qint64,qint64);
    void queryCompleted(int,int);

private:
    Q_DECLARE_PRIVATE(QmlXmlListModel)
    Q_DISABLE_COPY(QmlXmlListModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlXmlListModel)

QT_END_HEADER

#endif // QMLXMLLISTMODEL_H
