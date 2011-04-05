// Commit: a9f1eaa6a368bf7a72b52c428728a3e3e0a76209
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

#include "qsgrepeater_p.h"
#include "qsgrepeater_p_p.h"
#include "qsgvisualitemmodel_p.h"

#include <private/qdeclarativeglobal_p.h>
#include <private/qdeclarativelistaccessor_p.h>
#include <private/qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE

QSGRepeaterPrivate::QSGRepeaterPrivate()
: model(0), ownModel(false)
{
}

QSGRepeaterPrivate::~QSGRepeaterPrivate()
{
    if (ownModel)
        delete model;
}

QSGRepeater::QSGRepeater(QSGItem *parent)
  : QSGItem(*(new QSGRepeaterPrivate), parent)
{
}

QSGRepeater::~QSGRepeater()
{
}

QVariant QSGRepeater::model() const
{
    Q_D(const QSGRepeater);
    return d->dataSource;
}

void QSGRepeater::setModel(const QVariant &model)
{
    Q_D(QSGRepeater);
    if (d->dataSource == model)
        return;

    clear();
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        /*
        disconnect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
    */
    }
    d->dataSource = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QSGVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QSGVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QSGVisualDataModel(qmlContext(this));
            d->ownModel = true;
        }
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        /*
        connect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        connect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
        */
        regenerate();
    }
    emit modelChanged();
    emit countChanged();
}

QDeclarativeComponent *QSGRepeater::delegate() const
{
    Q_D(const QSGRepeater);
    if (d->model) {
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QSGRepeater::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QSGRepeater);
    if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
       if (delegate == dataModel->delegate())
           return;

    if (!d->ownModel) {
        d->model = new QSGVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        regenerate();
        emit delegateChanged();
    }
}

int QSGRepeater::count() const
{
    Q_D(const QSGRepeater);
    if (d->model)
        return d->model->count();
    return 0;
}

QSGItem *QSGRepeater::itemAt(int index) const
{
    Q_D(const QSGRepeater);
    if (index >= 0 && index < d->deletables.count())
        return d->deletables[index];
    return 0;
}

void QSGRepeater::componentComplete()
{
    QSGItem::componentComplete();
    regenerate();
}

void QSGRepeater::itemChange(ItemChange change, const ItemChangeData &value)
{
    QSGItem::itemChange(change, value);
    if (change == ItemParentHasChanged) {
        regenerate();
    }
}

void QSGRepeater::clear()
{
    Q_D(QSGRepeater);
    bool complete = isComponentComplete();

    if (d->model) {
        while (d->deletables.count() > 0) {
            QSGItem *item = d->deletables.takeLast();
            if (complete)
                emit itemRemoved(d->deletables.count()-1, item);
            d->model->release(item);
        }
    }
    d->deletables.clear();
}

void QSGRepeater::regenerate()
{
    Q_D(QSGRepeater);
    if (!isComponentComplete())
        return;

    clear();

    if (!d->model || !d->model->count() || !d->model->isValid() || !parentItem() || !isComponentComplete())
        return;

    for (int ii = 0; ii < count(); ++ii) {
        QSGItem *item = d->model->item(ii);
        if (item) {
            QDeclarative_setParent_noEvent(item, parentItem());
            item->setParentItem(parentItem());
            item->stackBefore(this);
            d->deletables << item;
            emit itemAdded(ii, item);
        }
    }
}

void QSGRepeater::itemsInserted(int index, int count)
{
    Q_D(QSGRepeater);
    if (!isComponentComplete())
        return;
    for (int i = 0; i < count; ++i) {
        int modelIndex = index + i;
        QSGItem *item = d->model->item(modelIndex);
        if (item) {
            QDeclarative_setParent_noEvent(item, parentItem());
            item->setParentItem(parentItem());
            if (modelIndex < d->deletables.count())
                item->stackBefore(d->deletables.at(modelIndex));
            else
                item->stackBefore(this);
            d->deletables.insert(modelIndex, item);
            emit itemAdded(modelIndex, item);
        }
    }
    emit countChanged();
}

void QSGRepeater::itemsRemoved(int index, int count)
{
    Q_D(QSGRepeater);
    if (!isComponentComplete() || count <= 0)
        return;
    while (count--) {
        QSGItem *item = d->deletables.takeAt(index);
        emit itemRemoved(index, item);
        if (item)
            d->model->release(item);
        else
            break;
    }
    emit countChanged();
}

void QSGRepeater::itemsMoved(int from, int to, int count)
{
    Q_D(QSGRepeater);
    if (!isComponentComplete() || count <= 0)
        return;
    if (from + count > d->deletables.count()) {
        regenerate();
        return;
    }
    QList<QSGItem*> removed;
    int removedCount = count;
    while (removedCount--)
        removed << d->deletables.takeAt(from);
    for (int i = 0; i < count; ++i)
        d->deletables.insert(to + i, removed.at(i));
    d->deletables.last()->stackBefore(this);
    for (int i = d->model->count()-1; i > 0; --i) {
        QSGItem *item = d->deletables.at(i-1);
        item->stackBefore(d->deletables.at(i));
    }
}

void QSGRepeater::modelReset()
{
    if (!isComponentComplete())
        return;
    regenerate();
    emit countChanged();
}

QT_END_NAMESPACE
