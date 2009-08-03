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

#ifndef QFXVISUALDATAMODEL_H
#define QFXVISUALDATAMODEL_H

#include <QtCore/qobject.h>
#include <QtCore/qabstractitemmodel.h>
#include <QtDeclarative/qml.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
/*****************************************************************************
 *****************************************************************************
 XXX Experimental
 *****************************************************************************
*****************************************************************************/

class QFxItem;
class QmlComponent;
class QmlPackage;
class QFxVisualDataModelPrivate;

class Q_DECLARATIVE_EXPORT QFxVisualModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    QFxVisualModel() {}
    virtual ~QFxVisualModel() {}

    enum ReleaseFlag { Referenced = 0x01, Destroyed = 0x02 };
    Q_DECLARE_FLAGS(ReleaseFlags, ReleaseFlag)

    virtual int count() const = 0;
    virtual bool isValid() const = 0;
    virtual QFxItem *item(int index, bool complete=true) = 0;
    virtual ReleaseFlags release(QFxItem *item) = 0;
    virtual void completeItem() = 0;
    virtual QVariant evaluate(int index, const QString &expression, QObject *objectContext) = 0;

    virtual int indexOf(QFxItem *item, QObject *objectContext) const = 0;

Q_SIGNALS:
    void countChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void createdItem(int index, QFxItem *item);
    void destroyingItem(QFxItem *item);

protected:
    QFxVisualModel(QObjectPrivate &dd, QObject *parent = 0)
        : QObject(dd, parent) {}

private:
    Q_DISABLE_COPY(QFxVisualModel)
};

class QFxVisualItemModelPrivate;
class Q_DECLARATIVE_EXPORT QFxVisualItemModel : public QFxVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFxVisualItemModel)

    Q_PROPERTY(QmlList<QFxItem *>* children READ children DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "children")

public:
    QFxVisualItemModel();
    virtual ~QFxVisualItemModel() {}

    virtual int count() const;
    virtual bool isValid() const;
    virtual QFxItem *item(int index, bool complete=true);
    virtual ReleaseFlags release(QFxItem *item);
    virtual void completeItem();
    virtual QVariant evaluate(int index, const QString &expression, QObject *objectContext);

    virtual int indexOf(QFxItem *item, QObject *objectContext) const;

    QmlList<QFxItem *> *children();

private:
    Q_DISABLE_COPY(QFxVisualItemModel)
};


class Q_DECLARATIVE_EXPORT QFxVisualDataModel : public QFxVisualModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFxVisualDataModel)

    Q_PROPERTY(QVariant model READ model WRITE setModel)
    Q_PROPERTY(QmlComponent *delegate READ delegate WRITE setDelegate)
    Q_PROPERTY(QString part READ part WRITE setPart)
    Q_PROPERTY(QObject *parts READ parts CONSTANT)
    Q_CLASSINFO("DefaultProperty", "delegate")
public:
    QFxVisualDataModel();
    QFxVisualDataModel(QmlContext *);
    virtual ~QFxVisualDataModel();

    QVariant model() const;
    void setModel(const QVariant &);

    QmlComponent *delegate() const;
    void setDelegate(QmlComponent *);

    QString part() const;
    void setPart(const QString &);

    int count() const;
    bool isValid() const { return delegate() != 0; }
    QFxItem *item(int index, bool complete=true);
    QFxItem *item(int index, const QByteArray &, bool complete=true);
    ReleaseFlags release(QFxItem *item);
    void completeItem();
    QVariant evaluate(int index, const QString &expression, QObject *objectContext);

    int indexOf(QFxItem *item, QObject *objectContext) const;

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
    void _q_dataChanged(const QModelIndex&,const QModelIndex&);
    void _q_createdPackage(int index, QmlPackage *package);
    void _q_destroyingPackage(QmlPackage *package);

private:
    Q_DISABLE_COPY(QFxVisualDataModel)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxVisualItemModel)
QML_DECLARE_TYPE(QFxVisualDataModel)

QT_END_HEADER

#endif // QFXVISUALDATAMODEL_H
