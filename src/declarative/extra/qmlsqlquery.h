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

#ifndef QMLSQLQUERYMODEL_H
#define QMLSQLQUERYMODEL_H

#include <QtDeclarative/qml.h>
#include <QtDeclarative/QListModelInterface>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlSqlBindPrivate;
class Q_DECLARATIVE_EXPORT QmlSqlBind : public QObject, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)

    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QVariant value READ value WRITE setValue)

    Q_CLASSINFO("DefaultValue", "value")

public:
    QmlSqlBind(QObject *parent = 0);
    ~QmlSqlBind();

    QString name() const;
    QVariant value() const;

    void setName(const QString &name);
    void setValue(const QVariant &);

    virtual void classComplete();

Q_SIGNALS:
    void valueChanged();

private:
    Q_DISABLE_COPY(QmlSqlBind)
    Q_DECLARE_PRIVATE(QmlSqlBind)
};

QML_DECLARE_TYPE(QmlSqlBind)

class QSqlQuery;
class QmlSqlQueryPrivate;
class Q_DECLARATIVE_EXPORT QmlSqlQuery : public QListModelInterface, public QmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QmlParserStatus)

    Q_PROPERTY(QString query READ query WRITE setQuery)
    Q_PROPERTY(QVariant connection READ connection WRITE setConnection)
    Q_PROPERTY(QString lastError READ lastError)
    
    Q_PROPERTY(QmlList<QmlSqlBind *> *bindings READ bindings)

    Q_CLASSINFO("DefaultProperty", "query")
public:
    QmlSqlQuery(QObject *parent = 0);
    ~QmlSqlQuery();

    QString query() const;
    void setQuery(const QString &);

    QVariant connection() const;
    void setConnection(const QVariant &);

    virtual QHash<int,QVariant> data(int index, const QList<int> &roles = (QList<int>())) const;
    virtual int count() const;
    virtual QList<int> roles() const;
    virtual QString toString(int role) const;

    QString lastError() const;

    virtual void classComplete();

    QmlList<QmlSqlBind *> *bindings();
    const QmlList<QmlSqlBind *> *bindings() const;

public slots:
    void exec();

protected:
    void timerEvent(QTimerEvent *);

private slots:
    void resetBinds();
    void resetQuery();

private:
    void emitChanges(int oldcount);

    Q_DISABLE_COPY(QmlSqlQuery)
    Q_DECLARE_PRIVATE(QmlSqlQuery)
};

QML_DECLARE_TYPE(QmlSqlQuery)

QT_END_NAMESPACE

QT_END_HEADER
#endif

