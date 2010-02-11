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

#ifndef QMLGRAPHICSVISUALDATAMODEL_H
#define QMLGRAPHICSVISUALDATAMODEL_H

#include <qml.h>

#include <QtCore/qobject.h>
#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
/*****************************************************************************
 *****************************************************************************
 XXX Experimental
 *****************************************************************************
*****************************************************************************/

class QmlGraphicsItem;
class QmlComponent;
class QmlPackage;
class QmlGraphicsVisualDataModelPrivate;

class Q_DECLARATIVE_EXPORT QmlGraphicsVisualModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    QmlGraphicsVisualModel() {}
    virtual ~QmlGraphicsVisualModel() {}

    enum ReleaseFlag { Referenced = 0x01, Destroyed = 0x02 };
    Q_DECLARE_FLAGS(ReleaseFlags, ReleaseFlag)

    virtual int count() const = 0;
    virtual bool isValid() const = 0;
    virtual QmlGraphicsItem *item(int index, bool complete=true) = 0;
    virtual ReleaseFlags release(QmlGraphicsItem *item) = 0;
    virtual void completeItem() = 0;
    virtual QVariant evaluate(int index, const QString &expression, QObject *objectContext) = 0;
    virtual QString stringValue(int, const QString &) { return QString(); }

    virtual int indexOf(QmlGraphicsItem *item, QObject *objectContext) const = 0;

Q_SIGNALS:
    void countChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void createdItem(int index, QmlGraphicsItem *item);
    void destroyingItem(QmlGraphicsItem *item);

protected:
    QmlGraphicsVisualModel(QObjectPrivate &dd, QObject *parent = 0)
        : QObject(dd, parent) {}

private:
    Q_DISABLE_COPY(QmlGraphicsVisualModel)
};

class QmlGraphicsVisualItemModelAttached;
class QmlGraphicsVisualItemModelPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsVisualItemModel : public QmlGraphicsVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlGraphicsVisualItemModel)

    Q_PROPERTY(QmlList<QmlGraphicsItem *>* children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    QmlGraphicsVisualItemModel();
    virtual ~QmlGraphicsVisualItemModel() {}

    virtual int count() const;
    virtual bool isValid() const;
    virtual QmlGraphicsItem *item(int index, bool complete=true);
    virtual ReleaseFlags release(QmlGraphicsItem *item);
    virtual void completeItem();
    virtual QString stringValue(int index, const QString &role);
    virtual QVariant evaluate(int index, const QString &expression, QObject *objectContext);

    virtual int indexOf(QmlGraphicsItem *item, QObject *objectContext) const;

    QmlList<QmlGraphicsItem *> *children();

    static QmlGraphicsVisualItemModelAttached *qmlAttachedProperties(QObject *obj);

Q_SIGNALS:
    void childrenChanged();

private:
    Q_DISABLE_COPY(QmlGraphicsVisualItemModel)
};


class Q_DECLARATIVE_EXPORT QmlGraphicsVisualDataModel : public QmlGraphicsVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlGraphicsVisualDataModel)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString part READ part WRITE setPart)
    Q_PROPERTY(QObject *parts READ parts CONSTANT)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    QmlGraphicsVisualDataModel();
    QmlGraphicsVisualDataModel(QmlContext *);
    virtual ~QmlGraphicsVisualDataModel();

    QVariant model() const;
    void setModel(const QVariant &);

    QmlComponent *delegate() const;
    void setDelegate(QmlComponent *);

    QString part() const;
    void setPart(const QString &);

    int count() const;
    bool isValid() const { return delegate() != 0; }
    QmlGraphicsItem *item(int index, bool complete=true);
    QmlGraphicsItem *item(int index, const QByteArray &, bool complete=true);
    ReleaseFlags release(QmlGraphicsItem *item);
    void completeItem();
    virtual QString stringValue(int index, const QString &role);
    QVariant evaluate(int index, const QString &expression, QObject *objectContext);

    int indexOf(QmlGraphicsItem *item, QObject *objectContext) const;

    QObject *parts();

Q_SIGNALS:
    void createdPackage(int index, QmlPackage *package);
    void destroyingPackage(QmlPackage *package);

private Q_SLOTS:
    void _q_itemsChanged(int, int, const QList<int> &);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_rowsInserted(const QModelIndex &,int,int);
    void _q_rowsRemoved(const QModelIndex &,int,int);
    void _q_rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int);
    void _q_dataChanged(const QModelIndex&,const QModelIndex&);
    void _q_createdPackage(int index, QmlPackage *package);
    void _q_destroyingPackage(QmlPackage *package);

private:
    Q_DISABLE_COPY(QmlGraphicsVisualDataModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsVisualModel)
QML_DECLARE_TYPE(QmlGraphicsVisualItemModel)
QML_DECLARE_TYPEINFO(QmlGraphicsVisualItemModel, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QmlGraphicsVisualDataModel)

QT_END_HEADER

#endif // QMLGRAPHICSVISUALDATAMODEL_H
