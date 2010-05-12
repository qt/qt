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

#include "private/qdeclarativerepeater_p.h"
#include "private/qdeclarativerepeater_p_p.h"

#include "private/qdeclarativevisualitemmodel_p.h"
#include <private/qdeclarativeglobal_p.h>
#include <qdeclarativelistaccessor_p.h>

#include <qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE
QDeclarativeRepeaterPrivate::QDeclarativeRepeaterPrivate()
: model(0), ownModel(false)
{
}

QDeclarativeRepeaterPrivate::~QDeclarativeRepeaterPrivate()
{
    if (ownModel)
        delete model;
}

/*!
    \qmlclass Repeater QDeclarativeRepeater
    \since 4.7
    \inherits Item

    \brief The Repeater item allows you to repeat an Item-based component using a model.

    The Repeater item is used to create a large number of
    similar items.  For each entry in the model, an item is instantiated
    in a context seeded with data from the model.  If the repeater will
    be instantiating a large number of instances, it may be more efficient to
    use one of Qt Declarative's \l {xmlViews}{view items}.

    The model may be either an object list, a string list, a number or a Qt model.
    In each case, the data element and the index is exposed to each instantiated
    component.  
    
    The index is always exposed as an accessible \c index property.
    In the case of an object or string list, the data element (of type string
    or object) is available as the \c modelData property.  In the case of a Qt model,
    all roles are available as named properties just like in the view classes. The
    following example shows how to use the index property inside the instantiated
    items.

    \snippet doc/src/snippets/declarative/repeater-index.qml 0

    \image repeater-index.png

    Items instantiated by the Repeater are inserted, in order, as
    children of the Repeater's parent.  The insertion starts immediately after
    the repeater's position in its parent stacking list.  This is to allow
    you to use a Repeater inside a layout.  The following QML example shows how
    the instantiated items would visually appear stacked between the red and
    blue rectangles.

    \snippet doc/src/snippets/declarative/repeater.qml 0

    \image repeater.png

    The repeater instance continues to own all items it instantiates, even
    if they are otherwise manipulated.  It is illegal to manually remove an item
    created by the Repeater.

    \note Repeater is Item-based, and cannot be used to repeat non-Item-derived objects.
    For example, it cannot be used to repeat QtObjects.
    \badcode
    Item {
        //XXX illegal. Can't repeat QtObject as it doesn't derive from Item.
        Repeater {
            model: 10
            QtObject {}
        }
    }
    \endcode
 */

/*!
    \internal
    \class QDeclarativeRepeater
    \qmlclass Repeater
 */

/*!
    Create a new QDeclarativeRepeater instance.
 */
QDeclarativeRepeater::QDeclarativeRepeater(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeRepeaterPrivate), parent)
{
}

/*!
    Destroy the repeater instance.  All items it instantiated are also
    destroyed.
 */
QDeclarativeRepeater::~QDeclarativeRepeater()
{
}

/*!
    \qmlproperty any Repeater::model

    The model providing data for the repeater.

    The model may be either an object list, a string list, a number or a Qt model.
    In each case, the data element and the index is exposed to each instantiated
    component.  The index is always exposed as an accessible \c index property.
    In the case of an object or string list, the data element (of type string
    or object) is available as the \c modelData property.  In the case of a Qt model,
    all roles are available as named properties just like in the view classes.

    As a special case the model can also be merely a number. In this case it will
    create that many instances of the component. They will also be assigned an index
    based on the order they are created.

    Models can also be created directly in QML, using a \l{ListModel} or \l{XmlListModel}.

    \sa {qmlmodels}{Data Models}
*/
QVariant QDeclarativeRepeater::model() const
{
    Q_D(const QDeclarativeRepeater);
    return d->dataSource;
}

void QDeclarativeRepeater::setModel(const QVariant &model)
{
    Q_D(QDeclarativeRepeater);
    if (d->dataSource == model)
        return;

    clear();
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        /*
        disconnect(d->model, SIGNAL(createdItem(int, QDeclarativeItem*)), this, SLOT(createdItem(int,QDeclarativeItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QDeclarativeItem*)), this, SLOT(destroyingItem(QDeclarativeItem*)));
    */
    }
    d->dataSource = model;
    emit modelChanged();
    QObject *object = qvariant_cast<QObject*>(model);
    QDeclarativeVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QDeclarativeVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QDeclarativeVisualDataModel(qmlContext(this), this);
            d->ownModel = true;
        }
        if (QDeclarativeVisualDataModel *dataModel = qobject_cast<QDeclarativeVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        /*
        connect(d->model, SIGNAL(createdItem(int, QDeclarativeItem*)), this, SLOT(createdItem(int,QDeclarativeItem*)));
        connect(d->model, SIGNAL(destroyingItem(QDeclarativeItem*)), this, SLOT(destroyingItem(QDeclarativeItem*)));
        */
        regenerate();
        emit countChanged();
    }
}

/*!
    \qmlproperty Component Repeater::delegate
    \default

    The delegate provides a template defining each item instantiated by the repeater.
    The index is exposed as an accessible \c index property.  Properties of the
    model are also available depending upon the type of \l {qmlmodels}{Data Model}.
 */
QDeclarativeComponent *QDeclarativeRepeater::delegate() const
{
    Q_D(const QDeclarativeRepeater);
    if (d->model) {
        if (QDeclarativeVisualDataModel *dataModel = qobject_cast<QDeclarativeVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QDeclarativeRepeater::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QDeclarativeRepeater);
    if (QDeclarativeVisualDataModel *dataModel = qobject_cast<QDeclarativeVisualDataModel*>(d->model))
       if (delegate == dataModel->delegate())
           return;

    if (!d->ownModel) {
        d->model = new QDeclarativeVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QDeclarativeVisualDataModel *dataModel = qobject_cast<QDeclarativeVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        regenerate();
        emit delegateChanged();
    }
}

/*!
    \qmlproperty int Repeater::count

    This property holds the number of items in the repeater.
*/
int QDeclarativeRepeater::count() const
{
    Q_D(const QDeclarativeRepeater);
    if (d->model)
        return d->model->count();
    return 0;
}


/*!
    \internal
 */
void QDeclarativeRepeater::componentComplete()
{
    QDeclarativeItem::componentComplete();
    regenerate();
}

/*!
    \internal
 */
QVariant QDeclarativeRepeater::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    QVariant rv = QDeclarativeItem::itemChange(change, value);
    if (change == ItemParentHasChanged) {
        regenerate();
    }

    return rv;
}

void QDeclarativeRepeater::clear()
{
    Q_D(QDeclarativeRepeater);
    if (d->model) {
        foreach (QDeclarativeItem *item, d->deletables) {
            d->model->release(item);
        }
    }
    d->deletables.clear();
}

/*!
    \internal
 */
void QDeclarativeRepeater::regenerate()
{
    Q_D(QDeclarativeRepeater);
    if (!isComponentComplete())
        return;

    clear();

    if (!d->model || !d->model->count() || !d->model->isValid() || !parentItem() || !isComponentComplete())
        return;

    for (int ii = 0; ii < count(); ++ii) {
        QDeclarativeItem *item = d->model->item(ii);
        if (item) {
            QDeclarative_setParent_noEvent(item, parentItem());
            item->setParentItem(parentItem());
            item->stackBefore(this);
            d->deletables << item;
        }
    }
}

void QDeclarativeRepeater::itemsInserted(int index, int count)
{
    Q_D(QDeclarativeRepeater);
    if (!isComponentComplete())
        return;
    for (int i = 0; i < count; ++i) {
        int modelIndex = index + i;
        QDeclarativeItem *item = d->model->item(modelIndex);
        if (item) {
            QDeclarative_setParent_noEvent(item, parentItem());
            item->setParentItem(parentItem());
            if (modelIndex < d->deletables.count())
                item->stackBefore(d->deletables.at(modelIndex));
            else
                item->stackBefore(this);
            d->deletables.insert(modelIndex, item);
        }
    }
}

void QDeclarativeRepeater::itemsRemoved(int index, int count)
{
    Q_D(QDeclarativeRepeater);
    if (!isComponentComplete() || count <= 0)
        return;
    while (count--) {
        QDeclarativeItem *item = d->deletables.takeAt(index);
        if (item)
            d->model->release(item);
        else
            break;
    }
}

void QDeclarativeRepeater::itemsMoved(int from, int to, int count)
{
    Q_D(QDeclarativeRepeater);
    if (!isComponentComplete() || count <= 0)
        return;
    if (from + count > d->deletables.count()) {
        regenerate();
        return;
    }
    QList<QDeclarativeItem*> removed;
    int removedCount = count;
    while (removedCount--)
        removed << d->deletables.takeAt(from);
    for (int i = 0; i < count; ++i)
        d->deletables.insert(to + i, removed.at(i));
    d->deletables.last()->stackBefore(this);
    for (int i = d->model->count()-1; i > 0; --i) {
        QDeclarativeItem *item = d->deletables.at(i-1);
        item->stackBefore(d->deletables.at(i));
    }
}

void QDeclarativeRepeater::modelReset()
{
    if (!isComponentComplete())
        return;
    regenerate();
}

QT_END_NAMESPACE
