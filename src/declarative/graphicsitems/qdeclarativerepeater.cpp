/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
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

    \brief The Repeater element allows you to repeat an Item-based component using a model.

    The Repeater element is used to create a large number of
    similar items. Like other view elements, a Repeater has a \l model and a \l delegate:
    for each entry in the model, the delegate is instantiated
    in a context seeded with data from the model. A Repeater item is usually
    enclosed in a positioner element such as \l Row or \l Column to visually
    position the multiple delegate items created by the Repeater.

    The following Repeater creates three instances of a \l Rectangle item within
    a \l Row:

    \snippet doc/src/snippets/declarative/repeater.qml import
    \codeline
    \snippet doc/src/snippets/declarative/repeater.qml simple

    \image repeater-simple.png

    The \l model of a Repeater can be any of the supported \l {qmlmodels}{Data Models}.

    Items instantiated by the Repeater are inserted, in order, as
    children of the Repeater's parent.  The insertion starts immediately after
    the repeater's position in its parent stacking list.  This allows
    a Repeater to be used inside a layout. For example, the following Repeater's
    items are stacked between a red rectangle and a blue rectangle:
   
    \snippet doc/src/snippets/declarative/repeater.qml layout

    \image repeater.png


    \section2 The \c index and \c modelData properties

    The index of a delegate is exposed as an accessible \c index property in the delegate.
    Properties of the model are also available depending upon the type of \l {qmlmodels}{Data Model}.

    Here is a Repeater that uses the \c index property inside the instantiated items:

    \table
    \row
    \o \snippet doc/src/snippets/declarative/repeater.qml index
    \o \image repeater-index.png
    \endtable

    Here is another Repeater that uses the \c modelData property to reference the data for a
    particular index:

    \table
    \row
    \o \snippet doc/src/snippets/declarative/repeater.qml modeldata
    \o \image repeater-modeldata.png
    \endtable


    A Repeater item owns all items it instantiates. Removing or dynamically destroying
    an item created by a Repeater results in unpredictable behavior.


    \section2 Considerations when using Repeater

    The Repeater element creates all of its delegate items when the repeater is first
    created. This can be inefficient if there are a large number of delegate items and
    not all of the items are required to be visible at the same time. If this is the case,
    consider using other view elements like ListView (which only creates delegate items 
    when they are scrolled into view) or use the \l {Dynamic Object Creation} methods to 
    create items as they are required.

    Also, note that Repeater is \l {Item}-based, and can only repeat \l {Item}-derived objects. 
    For example, it cannot be used to repeat QtObjects:
    \badcode
    Item {
        //XXX does not work! Can't repeat QtObject as it doesn't derive from Item.
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

    This property can be set to any of the following:

    \list
    \o A number that indicates the number of delegates to be created
    \o A model (e.g. a ListModel item, or a QAbstractItemModel subclass)
    \o A string list
    \o An object list
    \endlist

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
