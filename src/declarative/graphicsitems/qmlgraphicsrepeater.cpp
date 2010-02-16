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

#include "qmlgraphicsrepeater_p.h"
#include "qmlgraphicsrepeater_p_p.h"

#include "qmlgraphicsvisualitemmodel_p.h"

#include <qmllistaccessor_p.h>

#include <qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE
QmlGraphicsRepeaterPrivate::QmlGraphicsRepeaterPrivate()
: model(0), ownModel(false)
{
}

QmlGraphicsRepeaterPrivate::~QmlGraphicsRepeaterPrivate()
{
    if (ownModel)
        delete model;
}

QML_DEFINE_TYPE(Qt,4,6,Repeater,QmlGraphicsRepeater)

/*!
    \qmlclass Repeater QmlGraphicsRepeater
    \inherits Item

    \brief The Repeater item allows you to repeat a component based on a model.

    The Repeater item is used when you want to create a large number of
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
 */

/*!
    \internal
    \class QmlGraphicsRepeater
    \qmlclass Repeater

    XXX Repeater is very conservative in how it instatiates/deletes items.  Also
    new model entries will not be created and old ones will not be removed.
 */

/*!
    Create a new QmlGraphicsRepeater instance.
 */
QmlGraphicsRepeater::QmlGraphicsRepeater(QmlGraphicsItem *parent)
  : QmlGraphicsItem(*(new QmlGraphicsRepeaterPrivate), parent)
{
}

/*!
    Destroy the repeater instance.  All items it instantiated are also
    destroyed.
 */
QmlGraphicsRepeater::~QmlGraphicsRepeater()
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
QVariant QmlGraphicsRepeater::model() const
{
    Q_D(const QmlGraphicsRepeater);
    return d->dataSource;
}

void QmlGraphicsRepeater::setModel(const QVariant &model)
{
    Q_D(QmlGraphicsRepeater);
    clear();
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        /*
        disconnect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QmlGraphicsItem*)), this, SLOT(destroyingItem(QmlGraphicsItem*)));
    */
    }
    d->dataSource = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QmlGraphicsVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QmlGraphicsVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
            d->ownModel = true;
        }
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        /*
        connect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
        connect(d->model, SIGNAL(destroyingItem(QmlGraphicsItem*)), this, SLOT(destroyingItem(QmlGraphicsItem*)));
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
QmlComponent *QmlGraphicsRepeater::delegate() const
{
    Q_D(const QmlGraphicsRepeater);
    if (d->model) {
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QmlGraphicsRepeater::setDelegate(QmlComponent *delegate)
{
    Q_D(QmlGraphicsRepeater);
    if (!d->ownModel) {
        d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        regenerate();
    }
}

/*!
    \qmlproperty int Repeater::count

    This property holds the number of items in the repeater.
*/
int QmlGraphicsRepeater::count() const
{
    Q_D(const QmlGraphicsRepeater);
    if (d->model)
        return d->model->count();
    return 0;
}


/*!
    \internal
 */
void QmlGraphicsRepeater::componentComplete()
{
    QmlGraphicsItem::componentComplete();
    regenerate();
}

/*!
    \internal
 */
QVariant QmlGraphicsRepeater::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    QVariant rv = QmlGraphicsItem::itemChange(change, value);
    if (change == ItemParentHasChanged) {
        regenerate();
    }

    return rv;
}

void QmlGraphicsRepeater::clear()
{
    Q_D(QmlGraphicsRepeater);
    if (d->model) {
        foreach (QmlGraphicsItem *item, d->deletables) {
            item->setParentItem(this);
            d->model->release(item);
        }
    }
    d->deletables.clear();
}

/*!
    \internal
 */
void QmlGraphicsRepeater::regenerate()
{
    Q_D(QmlGraphicsRepeater);

    clear();

    if (!d->model || !d->model->count() || !d->model->isValid() || !parentItem() || !isComponentComplete())
        return;

    for (int ii = 0; ii < count(); ++ii) {
        QmlGraphicsItem *item = d->model->item(ii);
        if (item) {
            item->setParent(parentItem());
            item->stackBefore(this);
            d->deletables << item;
        }
    }
}

void QmlGraphicsRepeater::itemsInserted(int, int)
{
    regenerate();
}

void QmlGraphicsRepeater::itemsRemoved(int, int)
{
    regenerate();
}

void QmlGraphicsRepeater::itemsMoved(int,int,int)
{
    regenerate();
}

QT_END_NAMESPACE
