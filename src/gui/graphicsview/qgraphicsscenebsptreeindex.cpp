/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

/*!
    \class QGraphicsSceneBspTreeIndex
    \brief The QGraphicsSceneBspTreeIndex class provides an implementation of
    a BSP indexing algorithm for discovering items in QGraphicsScene.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api
    \mainclass
    \internal

    QGraphicsSceneBspTreeIndex index use a BSP(Binary Space Partitioning)
    implementation to discover items quickly. This implementation is
    very efficient for static scene. It has a depth that you can set.
    The depth directly affects performance and memory usage; the latter
    growing exponentially with the depth of the tree. With an optimal tree
    depth, the index can instantly determine the locality of items, even
    for scenes with thousands or millions of items. This also greatly improves
    rendering performance.

    By default, the value is 0, in which case Qt will guess a reasonable
    default depth based on the size, location and number of items in the
    scene. If these parameters change frequently, however, you may experience
    slowdowns as the index retunes the depth internally. You can avoid
    potential slowdowns by fixating the tree depth through setting this
    property.

    The depth of the tree and the size of the scene rectangle decide the
    granularity of the scene's partitioning. The size of each scene segment is
    determined by the following algorithm:

    The BSP tree has an optimal size when each segment contains between 0 and
    10 items.

    \sa QGraphicsScene, QGraphicsView, QGraphicsSceneIndex
*/

#ifndef QT_NO_GRAPHICSVIEW

#include <private/qgraphicsscene_p.h>
#include <private/qgraphicsscenebsptreeindex_p.h>
#include <private/qgraphicssceneindex_p.h>

#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

static inline int intmaxlog(int n)
{
    return  (n > 0 ? qMax(qCeil(qLn(qreal(n)) / qLn(qreal(2))), 5) : 0);
}

/*!
    Constructs a private scene bsp index.
*/
QGraphicsSceneBspTreeIndexPrivate::QGraphicsSceneBspTreeIndexPrivate(QGraphicsScene *scene)
    : QGraphicsSceneIndexPrivate(scene),
    bspTreeDepth(0),
    indexTimerId(0),
    restartIndexTimer(false),
    regenerateIndex(true),
    lastItemCount(0),
    purgePending(false),
    sortCacheEnabled(false),
    updatingSortCache(false)
{
}


/*!
    This method will update the BSP index by removing the items from the temporary
    unindexed list and add them in the indexedItems list. This will also
    update the growingItemsBoundingRect if needed. This will update the BSP
    implementation as well.

    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::_q_updateIndex()
{
    Q_Q(QGraphicsSceneBspTreeIndex);
    if (!indexTimerId)
        return;

    q->killTimer(indexTimerId);
    indexTimerId = 0;

    purgeRemovedItems();

     // Add unindexedItems to indexedItems
    QRectF unindexedItemsBoundingRect;
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            unindexedItemsBoundingRect |= item->sceneBoundingRect();
            if (!freeItemIndexes.isEmpty()) {
                int freeIndex = freeItemIndexes.takeFirst();
                item->d_func()->index = freeIndex;
                indexedItems[freeIndex] = item;
            } else {
                item->d_func()->index = indexedItems.size();
                indexedItems << item;
            }
        }
    }

    // Determine whether we should regenerate the BSP tree.
    if (bspTreeDepth == 0) {
        int oldDepth = intmaxlog(lastItemCount);
        bspTreeDepth = intmaxlog(indexedItems.size());
        static const int slack = 100;
        if (bsp.leafCount() == 0 || (oldDepth != bspTreeDepth && qAbs(lastItemCount - indexedItems.size()) > slack)) {
            // ### Crude algorithm.
            regenerateIndex = true;
        }
    }

    // Regenerate the tree.
    if (regenerateIndex) {
        regenerateIndex = false;
        bsp.initialize(q->scene()->sceneRect(), bspTreeDepth);
        unindexedItems = indexedItems;
        lastItemCount = indexedItems.size();
    }

    // Insert all unindexed items into the tree.
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            QRectF rect = item->sceneBoundingRect();
            if (item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)
                continue;

            bsp.insertItem(item, rect);
        }
    }
    unindexedItems.clear();
}


/*!
    \internal

    Removes stale pointers from all data structures.
*/
void QGraphicsSceneBspTreeIndexPrivate::purgeRemovedItems()
{
    if (!purgePending && removedItems.isEmpty())
        return;

    // Remove stale items from the BSP tree.
    bsp.removeItems(removedItems);
    // Purge this list.
    removedItems.clear();
    freeItemIndexes.clear();
    for (int i = 0; i < indexedItems.size(); ++i) {
        if (!indexedItems.at(i))
            freeItemIndexes << i;
    }
    purgePending = false;
}

/*!
    \internal

    Starts or restarts the timer used for reindexing unindexed items.
*/
void QGraphicsSceneBspTreeIndexPrivate::startIndexTimer(int interval)
{
    Q_Q(QGraphicsSceneBspTreeIndex);
    if (indexTimerId) {
        restartIndexTimer = true;
    } else {
        indexTimerId = q->startTimer(interval);
    }
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::resetIndex()
{
    purgeRemovedItems();
    for (int i = 0; i < indexedItems.size(); ++i) {
        if (QGraphicsItem *item = indexedItems.at(i)) {
            item->d_ptr->index = -1;
            unindexedItems << item;
        }
    }
    indexedItems.clear();
    freeItemIndexes.clear();
    regenerateIndex = true;
    startIndexTimer();
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::climbTree(QGraphicsItem *item, int *stackingOrder)
{
    if (!item->d_ptr->children.isEmpty()) {
        QList<QGraphicsItem *> childList = item->d_ptr->children;
        qStableSort(childList.begin(), childList.end(), qt_closestLeaf);
        for (int i = 0; i < childList.size(); ++i) {
            QGraphicsItem *item = childList.at(i);
            if (!(item->flags() & QGraphicsItem::ItemStacksBehindParent))
                climbTree(childList.at(i), stackingOrder);
        }
        item->d_ptr->globalStackingOrder = (*stackingOrder)++;
        for (int i = 0; i < childList.size(); ++i) {
            QGraphicsItem *item = childList.at(i);
            if (item->flags() & QGraphicsItem::ItemStacksBehindParent)
                climbTree(childList.at(i), stackingOrder);
        }
    } else {
        item->d_ptr->globalStackingOrder = (*stackingOrder)++;
    }
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::_q_updateSortCache()
{
    Q_Q(QGraphicsSceneBspTreeIndex);
    _q_updateIndex();

    if (!sortCacheEnabled || !updatingSortCache)
        return;

    updatingSortCache = false;
    int stackingOrder = 0;

    QList<QGraphicsItem *> topLevels;

    for (int i = 0; i < q->items().count(); ++i) {
        QGraphicsItem *item = q->items().at(i);
        if (item && item->parentItem() == 0)
            topLevels << item;
    }

    qStableSort(topLevels.begin(), topLevels.end(), qt_closestLeaf);
    for (int i = 0; i < topLevels.size(); ++i)
        climbTree(topLevels.at(i), &stackingOrder);
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::invalidateSortCache()
{
    Q_Q(QGraphicsSceneBspTreeIndex);
    if (!sortCacheEnabled || updatingSortCache)
        return;

    updatingSortCache = true;
    QMetaObject::invokeMethod(q, "_q_updateSortCache", Qt::QueuedConnection);
}

/*!
    Returns true if \a item1 is on top of \a item2.

    \internal
*/
bool QGraphicsSceneBspTreeIndexPrivate::closestItemFirst_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    // Siblings? Just check their z-values.
    const QGraphicsItemPrivate *d1 = item1->d_ptr;
    const QGraphicsItemPrivate *d2 = item2->d_ptr;
    if (d1->parent == d2->parent)
        return qt_closestLeaf(item1, item2);

    // Find common ancestor, and each item's ancestor closest to the common
    // ancestor.
    int item1Depth = d1->depth;
    int item2Depth = d2->depth;
    const QGraphicsItem *p = item1;
    const QGraphicsItem *t1 = item1;
    while (item1Depth > item2Depth && (p = p->d_ptr->parent)) {
        if (p == item2) {
            // item2 is one of item1's ancestors; item1 is on top
            return !(t1->d_ptr->flags & QGraphicsItem::ItemStacksBehindParent);
        }
        t1 = p;
        --item1Depth;
    }
    p = item2;
    const QGraphicsItem *t2 = item2;
    while (item2Depth > item1Depth && (p = p->d_ptr->parent)) {
        if (p == item1) {
            // item1 is one of item2's ancestors; item1 is not on top
            return (t2->d_ptr->flags & QGraphicsItem::ItemStacksBehindParent);
        }
        t2 = p;
        --item2Depth;
    }

    // item1Ancestor is now at the same level as item2Ancestor, but not the same.
    const QGraphicsItem *a1 = t1;
    const QGraphicsItem *a2 = t2;
    while (a1) {
        const QGraphicsItem *p1 = a1;
        const QGraphicsItem *p2 = a2;
        a1 = a1->parentItem();
        a2 = a2->parentItem();
        if (a1 && a1 == a2)
            return qt_closestLeaf(p1, p2);
    }

    // No common ancestor? Then just compare the items' toplevels directly.
    return qt_closestLeaf(t1->topLevelItem(), t2->topLevelItem());
}

/*!
    Returns true if \a item2 is on top of \a item1.

    \internal
*/
bool QGraphicsSceneBspTreeIndexPrivate::closestItemLast_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
{
    return closestItemFirst_withoutCache(item2, item1);
}

/*!
    Sort a list of \a itemList in a specific \a order and use the cache if requested.

    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::sortItems(QList<QGraphicsItem *> *itemList, Qt::SortOrder order,
                                      bool sortCacheEnabled)
{
    if (sortCacheEnabled) {
        if (order == Qt::AscendingOrder) {
            qStableSort(itemList->begin(), itemList->end(), closestItemFirst_withCache);
        } else if (order == Qt::DescendingOrder) {
            qStableSort(itemList->begin(), itemList->end(), closestItemLast_withCache);
        }
    } else {
        if (order == Qt::AscendingOrder) {
            qStableSort(itemList->begin(), itemList->end(), closestItemFirst_withoutCache);
        } else if (order == Qt::DescendingOrder) {
            qStableSort(itemList->begin(), itemList->end(), closestItemLast_withoutCache);
        }
    }
}

/*!
    Constructs a BSP scene index for the given \a scene.
*/
QGraphicsSceneBspTreeIndex::QGraphicsSceneBspTreeIndex(QGraphicsScene *scene)
    : QGraphicsSceneIndex(*new QGraphicsSceneBspTreeIndexPrivate(scene), scene)
{

}

/*!
    \reimp
    Clear the all the BSP index.
*/
void QGraphicsSceneBspTreeIndex::clear()
{
    Q_D(QGraphicsSceneBspTreeIndex);
    d->bsp.clear();
    d->lastItemCount = 0;
    d->freeItemIndexes.clear();
    d->indexedItems.clear();
    d->unindexedItems.clear();
}

/*!
    Add the \a item into the BSP index.
*/
void QGraphicsSceneBspTreeIndex::addItem(QGraphicsItem *item)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    // Prevent reusing a recently deleted pointer: purge all removed items
    // from our lists.
    d->purgeRemovedItems();

    // Invalidate any sort caching; arrival of a new item means we need to
    // resort.
    // Update the scene's sort cache settings.
    item->d_ptr->globalStackingOrder = -1;
    d->invalidateSortCache();

    // Indexing requires sceneBoundingRect(), but because \a item might
    // not be completely constructed at this point, we need to store it in
    // a temporary list and schedule an indexing for later.
    item->d_ptr->index = -1;
    if (item->d_ptr->itemIsUntransformable()) {
        d->untransformableItems << item;
    } else {
        d->unindexedItems << item;
        d->startIndexTimer(0);
    }
}

/*!
    This really add the item in the BSP.
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::addToBspTree(QGraphicsItem *item)
{
    if (item->d_ptr->itemIsUntransformable())
        return;
    if (item->d_func()->index != -1) {
        bsp.insertItem(item, item->sceneBoundingRect());
        foreach (QGraphicsItem *child, item->children())
            child->addToIndex();
    } else {
        // The BSP tree is regenerated if the number of items grows to a
        // certain threshold, or if the bounding rect of the graph doubles in
        // size.
        startIndexTimer();
    }
}

/*!
    Remove the \a item from the BSP index.
*/
void QGraphicsSceneBspTreeIndex::removeItem(QGraphicsItem *item)
{
    Q_D(QGraphicsSceneBspTreeIndex);

    // Note: This will access item's sceneBoundingRect(), which (as this is
    // C++) is why we cannot call removeItem() from QGraphicsItem's
    // destructor.
    d->removeFromBspTree(item);

    // Invalidate any sort caching; arrival of a new item means we need to
    // resort.
    d->invalidateSortCache();

    // Remove from our item lists.
    int index = item->d_func()->index;
    if (index != -1) {
        d->freeItemIndexes << index;
        d->indexedItems[index] = 0;
    } else {
        if (item->d_ptr->itemIsUntransformable())
            d->untransformableItems.removeOne(item);
        else
            d->unindexedItems.removeOne(item);
    }
}

/*!
    \reimp
    Delete the \a item from the BSP index (without accessing its boundingRect).
*/
void QGraphicsSceneBspTreeIndex::deleteItem(QGraphicsItem *item)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    // Invalidate any sort caching; arrival of a new item means we need to
    // resort.
    d->invalidateSortCache();

    int index = item->d_func()->index;
    if (index != -1) {
        // Important: The index is useless until purgeRemovedItems() is
        // called.
        d->indexedItems[index] = (QGraphicsItem *)0;
        if (!d->purgePending) {
            d->purgePending = true;
            scene()->update();
        }
        d->removedItems << item;
    } else {
        // Recently added items are purged immediately. unindexedItems() never
        // contains stale items.
        if (item->d_ptr->itemIsUntransformable())
            d->untransformableItems.removeOne(item);
        else
            d->unindexedItems.removeOne(item);
        scene()->update();
    }
}

/*!
    Really remove the item from the BSP
    \internal
*/
void QGraphicsSceneBspTreeIndexPrivate::removeFromBspTree(QGraphicsItem *item)
{
    if (item->d_ptr->itemIsUntransformable())
        return;

    if (item->d_func()->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren) {
        // ### remove from child index only if applicable
        return;
    }

    if (item->d_func()->index != -1) {
        bsp.removeItem(item, item->sceneBoundingRect());
        //prepareGeometryChange will call prepareBoundingRectChange
        foreach (QGraphicsItem *child, item->children())
            child->prepareGeometryChange();
    }
    startIndexTimer();
}

/*!
    \reimp
    Update the BSP when the \a item 's bounding rect has changed.
*/
void QGraphicsSceneBspTreeIndex::prepareBoundingRectChange(const QGraphicsItem *item)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    if (!item->d_ptr->itemIsUntransformable()) {
        // Note: This will access item's sceneBoundingRect(), which (as this is
        // C++) is why we cannot call removeItem() from QGraphicsItem's
        // destructor.
        QGraphicsItem *thatItem = const_cast<QGraphicsItem *>(item);
        d->removeFromBspTree(thatItem);
        int index = item->d_func()->index;
        if (index != -1) {
            d->freeItemIndexes << index;
            d->indexedItems[index] = 0;
            thatItem->d_func()->index = -1;
            d->unindexedItems << thatItem;
        }
    }
}

/*!
    Returns an estimation visible items that are either inside or
    intersect with the specified \a rect and return a list sorted using \a order.

    \a deviceTransform is the transformation apply to the view.

*/
QList<QGraphicsItem *> QGraphicsSceneBspTreeIndex::estimateItems(const QRectF &rect, Qt::SortOrder order,
                                                                 const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneBspTreeIndex);
    const_cast<QGraphicsSceneBspTreeIndexPrivate*>(d)->purgeRemovedItems();
    const_cast<QGraphicsSceneBspTreeIndexPrivate*>(d)->_q_updateSortCache();

    // ### Handle items that ignore transformations
    Q_UNUSED(deviceTransform);

    QList<QGraphicsItem *> rectItems = d->bsp.items(rect);
    // Fill in with any unindexed items
    for (int i = 0; i < d->unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = d->unindexedItems.at(i)) {
            if (!item->d_ptr->itemDiscovered && item->d_ptr->visible && !(item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)) {
                QRectF boundingRect = item->sceneBoundingRect();
                if (QRectF_intersects(boundingRect, rect)) {
                    item->d_ptr->itemDiscovered = 1;
                    rectItems << item;
                }
            }
        }
    }

     // Reset the discovered state of all discovered items
    for (int i = 0; i < rectItems.size(); ++i)
        rectItems.at(i)->d_func()->itemDiscovered = 0;

    d->sortItems(&rectItems, order, d->sortCacheEnabled);

    return rectItems;
}


/*!
    \fn QList<QGraphicsItem *> QGraphicsSceneBspTreeIndex::items(Qt::SortOrder order = Qt::AscendingOrder) const;

    Return all items in the BSP index and sort them using \a order.
*/
QList<QGraphicsItem *> QGraphicsSceneBspTreeIndex::items(Qt::SortOrder order) const
{
    Q_D(const QGraphicsSceneBspTreeIndex);
    const_cast<QGraphicsSceneBspTreeIndexPrivate*>(d)->purgeRemovedItems();
    QList<QGraphicsItem *> itemList;

    // If freeItemIndexes is empty, we know there are no holes in indexedItems and
    // unindexedItems.
    if (d->freeItemIndexes.isEmpty()) {
        if (d->unindexedItems.isEmpty()) {
            itemList = d->indexedItems;
        } else {
            itemList = d->indexedItems + d->unindexedItems;
        }
    } else {
        // Rebuild the list of items to avoid holes. ### We could also just
        // compress the item lists at this point.
        foreach (QGraphicsItem *item, d->indexedItems + d->unindexedItems) {
            if (item)
                itemList << item;
        }
    }
    itemList += d->untransformableItems;
    if (order != -1) {
        //We sort descending order
        d->sortItems(&itemList, order, d->sortCacheEnabled);
    }
    return itemList;
}

/*!
    \property QGraphicsSceneBspTreeIndex::bspTreeDepth
    \brief the depth of the BSP index tree
    \since 4.6

    This value determines the depth of BSP tree. The depth
    directly affects performance and memory usage; the latter
    growing exponentially with the depth of the tree. With an optimal tree
    depth, the index can instantly determine the locality of items, even
    for scenes with thousands or millions of items. This also greatly improves
    rendering performance.

    By default, the value is 0, in which case Qt will guess a reasonable
    default depth based on the size, location and number of items in the
    scene. If these parameters change frequently, however, you may experience
    slowdowns as the index retunes the depth internally. You can avoid
    potential slowdowns by fixating the tree depth through setting this
    property.

    The depth of the tree and the size of the scene rectangle decide the
    granularity of the scene's partitioning. The size of each scene segment is
    determined by the following algorithm:

    The BSP tree has an optimal size when each segment contains between 0 and
    10 items.

*/
int QGraphicsSceneBspTreeIndex::bspTreeDepth()
{
    Q_D(const QGraphicsSceneBspTreeIndex);
    return d->bspTreeDepth;
}

void QGraphicsSceneBspTreeIndex::setBspTreeDepth(int depth)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    if (d->bspTreeDepth == depth)
        return;
    d->bspTreeDepth = depth;
    d->resetIndex();
}

/*!
    \reimp

    This method react to the  \a rect change of the scene and
    reset the BSP tree index.
*/
void QGraphicsSceneBspTreeIndex::sceneRectChanged()
{
    Q_D(QGraphicsSceneBspTreeIndex);
    d->resetIndex();
}

/*!
    \reimp

    This method react to the \a change of the \a item and use the \a value to
    update the BSP tree if necessary.

*/
void QGraphicsSceneBspTreeIndex::itemChange(const QGraphicsItem *item, QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    switch (change) {
    case QGraphicsItem::ItemFlagsChange: {
        // Handle ItemIgnoresTransformations
        bool ignoredTransform = item->flags() & QGraphicsItem::ItemIgnoresTransformations;
        bool willIgnoreTransform = value.toUInt() & QGraphicsItem::ItemIgnoresTransformations;
        if (ignoredTransform != willIgnoreTransform) {
            QGraphicsItem *thatItem = const_cast<QGraphicsItem *>(item);
            removeItem(thatItem);
            thatItem->d_ptr->index = -1;
            if (willIgnoreTransform) {
                d->untransformableItems << thatItem;
            } else {
                d->unindexedItems << thatItem;
                d->startIndexTimer(0);
            }
        }
        break;
    }
    case QGraphicsItem::ItemZValueChange:
        d->invalidateSortCache();
        break;
    case QGraphicsItem::ItemParentChange: {
        d->invalidateSortCache();
        // Handle ItemIgnoresTransformations
        QGraphicsItem *newParent = qVariantValue<QGraphicsItem *>(value);
        bool ignoredTransform = item->d_ptr->itemIsUntransformable();
        bool willIgnoreTransform = (item->flags() & QGraphicsItem::ItemIgnoresTransformations) || (newParent && newParent->d_ptr->itemIsUntransformable());
        if (ignoredTransform != willIgnoreTransform) {
            QGraphicsItem *thatItem = const_cast<QGraphicsItem *>(item);
            removeItem(thatItem);
            thatItem->d_ptr->index = -1;
            if (willIgnoreTransform) {
                d->untransformableItems << thatItem;
            } else {
                d->unindexedItems << thatItem;
                d->startIndexTimer(0);
            }
        }
        break;
    }
    default:
        break;
    }
    return QGraphicsSceneIndex::itemChange(item, change, value);
}
/*!
    \reimp

    Used to catch the timer event.

    \internal
*/
bool QGraphicsSceneBspTreeIndex::event(QEvent *event)
{
    Q_D(QGraphicsSceneBspTreeIndex);
    switch (event->type()) {
    case QEvent::Timer:
            if (d->indexTimerId && static_cast<QTimerEvent *>(event)->timerId() == d->indexTimerId) {
                if (d->restartIndexTimer) {
                    d->restartIndexTimer = false;
                } else {
                    // this call will kill the timer
                    d->_q_updateIndex();
                }
            }
     // Fallthrough intended - support timers in subclasses.
    default:
        return QObject::event(event);
    }
    return true;
}

QT_END_NAMESPACE

#include "moc_qgraphicsscenebsptreeindex_p.cpp"

#endif  // QT_NO_GRAPHICSVIEW

