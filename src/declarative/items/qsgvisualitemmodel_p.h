// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGVISUALITEMMODEL_P_H
#define QSGVISUALITEMMODEL_P_H

#include <QtDeclarative/qdeclarative.h>
#include <QtCore/qobject.h>
#include <QtCore/qabstractitemmodel.h>

QT_BEGIN_HEADER

Q_DECLARE_METATYPE(QModelIndex)

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGItem;
class QDeclarativeComponent;
class QDeclarativePackage;
class QSGVisualDataModelPrivate;

class Q_AUTOTEST_EXPORT QSGVisualModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    virtual ~QSGVisualModel() {}

    enum ReleaseFlag { Referenced = 0x01, Destroyed = 0x02 };
    Q_DECLARE_FLAGS(ReleaseFlags, ReleaseFlag)

    virtual int count() const = 0;
    virtual bool isValid() const = 0;
    virtual QSGItem *item(int index, bool complete=true) = 0;
    virtual ReleaseFlags release(QSGItem *item) = 0;
    virtual bool completePending() const = 0;
    virtual void completeItem() = 0;
    virtual QString stringValue(int, const QString &) = 0;
    virtual void setWatchedRoles(QList<QByteArray> roles) = 0;

    virtual int indexOf(QSGItem *item, QObject *objectContext) const = 0;

Q_SIGNALS:
    void countChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void itemsChanged(int index, int count);
    void modelReset();
    void createdItem(int index, QSGItem *item);
    void destroyingItem(QSGItem *item);

protected:
    QSGVisualModel(QObjectPrivate &dd, QObject *parent = 0)
        : QObject(dd, parent) {}

private:
    Q_DISABLE_COPY(QSGVisualModel)
};

class QSGVisualItemModelAttached;
class QSGVisualItemModelPrivate;
class Q_AUTOTEST_EXPORT QSGVisualItemModel : public QSGVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGVisualItemModel)

    Q_PROPERTY(QDeclarativeListProperty<QSGItem> children READ children NOTIFY childrenChanged DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    QSGVisualItemModel(QObject *parent=0);
    virtual ~QSGVisualItemModel() {}

    virtual int count() const;
    virtual bool isValid() const;
    virtual QSGItem *item(int index, bool complete=true);
    virtual ReleaseFlags release(QSGItem *item);
    virtual bool completePending() const;
    virtual void completeItem();
    virtual QString stringValue(int index, const QString &role);
    virtual void setWatchedRoles(QList<QByteArray>) {}

    virtual int indexOf(QSGItem *item, QObject *objectContext) const;

    QDeclarativeListProperty<QSGItem> children();

    static QSGVisualItemModelAttached *qmlAttachedProperties(QObject *obj);

Q_SIGNALS:
    void childrenChanged();

private:
    Q_DISABLE_COPY(QSGVisualItemModel)
};


class Q_AUTOTEST_EXPORT QSGVisualDataModel : public QSGVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGVisualDataModel)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString part READ part WRITE setPart)
    Q_PROPERTY(QObject *parts READ parts CONSTANT)
    Q_PROPERTY(QVariant rootIndex READ rootIndex WRITE setRootIndex NOTIFY rootIndexChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    QSGVisualDataModel();
    QSGVisualDataModel(QDeclarativeContext *, QObject *parent=0);
    virtual ~QSGVisualDataModel();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    QVariant rootIndex() const;
    void setRootIndex(const QVariant &root);

    Q_INVOKABLE QVariant modelIndex(int idx) const;
    Q_INVOKABLE QVariant parentModelIndex() const;

    QString part() const;
    void setPart(const QString &);

    int count() const;
    bool isValid() const { return delegate() != 0; }
    QSGItem *item(int index, bool complete=true);
    QSGItem *item(int index, const QByteArray &, bool complete=true);
    ReleaseFlags release(QSGItem *item);
    bool completePending() const;
    void completeItem();
    virtual QString stringValue(int index, const QString &role);
    virtual void setWatchedRoles(QList<QByteArray> roles);

    int indexOf(QSGItem *item, QObject *objectContext) const;

    QObject *parts();

Q_SIGNALS:
    void createdPackage(int index, QDeclarativePackage *package);
    void destroyingPackage(QDeclarativePackage *package);
    void rootIndexChanged();

private Q_SLOTS:
    void _q_itemsChanged(int, int, const QList<int> &);
    void _q_itemsInserted(int index, int count);
    void _q_itemsRemoved(int index, int count);
    void _q_itemsMoved(int from, int to, int count);
    void _q_rowsInserted(const QModelIndex &,int,int);
    void _q_rowsRemoved(const QModelIndex &,int,int);
    void _q_rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int);
    void _q_dataChanged(const QModelIndex&,const QModelIndex&);
    void _q_layoutChanged();
    void _q_modelReset();
    void _q_createdPackage(int index, QDeclarativePackage *package);
    void _q_destroyingPackage(QDeclarativePackage *package);

private:
    Q_DISABLE_COPY(QSGVisualDataModel)
};

class QSGVisualItemModelAttached : public QObject
{
    Q_OBJECT

public:
    QSGVisualItemModelAttached(QObject *parent)
        : QObject(parent), m_index(0) {}
    ~QSGVisualItemModelAttached() {
        attachedProperties.remove(parent());
    }

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const { return m_index; }
    void setIndex(int idx) {
        if (m_index != idx) {
            m_index = idx;
            emit indexChanged();
        }
    }

    static QSGVisualItemModelAttached *properties(QObject *obj) {
        QSGVisualItemModelAttached *rv = attachedProperties.value(obj);
        if (!rv) {
            rv = new QSGVisualItemModelAttached(obj);
            attachedProperties.insert(obj, rv);
        }
        return rv;
    }

Q_SIGNALS:
    void indexChanged();

public:
    int m_index;

    static QHash<QObject*, QSGVisualItemModelAttached*> attachedProperties;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGVisualModel)
QML_DECLARE_TYPE(QSGVisualItemModel)
QML_DECLARE_TYPEINFO(QSGVisualItemModel, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QSGVisualDataModel)

QT_END_HEADER

#endif // QSGVISUALITEMMODEL_P_H
