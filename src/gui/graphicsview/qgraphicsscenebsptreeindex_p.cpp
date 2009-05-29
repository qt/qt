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

static const int QGRAPHICSSCENE_INDEXTIMER_TIMEOUT = 2000;

#include "qgraphicsscenebsptreeindex_p.h"
#include "qgraphicsitem_p.h"
#include "qgraphicsscene_p.h"

#include <QtCore/qmath.h>

#include <QtCore/qdebug.h>

QGraphicsSceneBspTreeIndex::QGraphicsSceneBspTreeIndex(QGraphicsScene *scene)
    : QGraphicsSceneIndex(scene),
    bspTreeDepth(0),
    indexTimerId(0),
    restartIndexTimer(false),
    regenerateIndex(true),
    lastItemCount(0),
    purgePending(false)
{

}


QRectF QGraphicsSceneBspTreeIndex::indexedRect()
{
    _q_updateIndex();
     return scene()->d_func()->hasSceneRect ? scene()->d_func()->sceneRect : scene()->d_func()->growingItemsBoundingRect;
}

void QGraphicsSceneBspTreeIndex::clear()
{
    bsp.clear();
    lastItemCount = 0;
    freeItemIndexes.clear();
    m_indexedItems.clear();
    unindexedItems.clear();
}

void QGraphicsSceneBspTreeIndex::addItem(QGraphicsItem *item)
{
    // Prevent reusing a recently deleted pointer: purge all removed items
    // from our lists.
    purgeRemovedItems();

    // Indexing requires sceneBoundingRect(), but because \a item might
    // not be completely constructed at this point, we need to store it in
    // a temporary list and schedule an indexing for later.
    unindexedItems << item;
    item->d_func()->index = -1;
    startIndexTimer();
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndex::addToIndex(QGraphicsItem *item)
{
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

void QGraphicsSceneBspTreeIndex::removeItem(QGraphicsItem *item)
{
    // Note: This will access item's sceneBoundingRect(), which (as this is
    // C++) is why we cannot call removeItem() from QGraphicsItem's
    // destructor.
    removeFromIndex(item);

    // Remove from our item lists.
    int index = item->d_func()->index;
    if (index != -1) {
        freeItemIndexes << index;
        m_indexedItems[index] = 0;
    } else {
        unindexedItems.removeAll(item);
    }
}

void QGraphicsSceneBspTreeIndex::deleteItem(QGraphicsItem *item)
{
    int index = item->d_func()->index;
    if (index != -1) {
        // Important: The index is useless until purgeRemovedItems() is
        // called.
        m_indexedItems[index] = (QGraphicsItem *)0;
        if (!purgePending) {
            purgePending = true;
            scene()->update();
        }
        removedItems << item;
    } else {
        // Recently added items are purged immediately. unindexedItems() never
        // contains stale items.
        unindexedItems.removeAll(item);
        scene()->update();
    }
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndex::removeFromIndex(QGraphicsItem *item)
{
    if (item->d_func()->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren) {
        // ### remove from child index only if applicable
        return;
    }
    int index = item->d_func()->index;
    if (index != -1) {
        bsp.removeItem(item, item->sceneBoundingRect());
        freeItemIndexes << index;
        m_indexedItems[index] = 0;
        item->d_func()->index = -1;
        unindexedItems << item;

        //prepareGeometryChange will call updateItem
        foreach (QGraphicsItem *child, item->children())
            child->prepareGeometryChange();
    }
    startIndexTimer();
}

void QGraphicsSceneBspTreeIndex::prepareBoundingRectChange(const QGraphicsItem *item)
{
    // Note: This will access item's sceneBoundingRect(), which (as this is
    // C++) is why we cannot call removeItem() from QGraphicsItem's
    // destructor.
    removeFromIndex(const_cast<QGraphicsItem *>(item));
}

QList<QGraphicsItem *> QGraphicsSceneBspTreeIndex::estimateItems(const QRectF &rect, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    const_cast<QGraphicsSceneBspTreeIndex*>(this)->purgeRemovedItems();
    QList<QGraphicsItem *> rectItems = bsp.items(rect);
    // Fill in with any unindexed items
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            if (!item->d_ptr->itemDiscovered && item->d_ptr->visible && !(item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)) {
                QRectF boundingRect = item->sceneBoundingRect();
                if (boundingRect.intersects(rect)) {
                    item->d_ptr->itemDiscovered = 1;
                    rectItems << item;
                }
            }
        }
    }

     // Reset the discovered state of all discovered items
    for (int i = 0; i < rectItems.size(); ++i)
        rectItems.at(i)->d_func()->itemDiscovered = 0;

    return rectItems;
}

QList<QGraphicsItem *> QGraphicsSceneBspTreeIndex::items() const
{
    const_cast<QGraphicsSceneBspTreeIndex*>(this)->purgeRemovedItems();
     // If freeItemIndexes is empty, we know there are no holes in indexedItems and
    // unindexedItems.
    if (freeItemIndexes.isEmpty()) {
        if (unindexedItems.isEmpty())
            return m_indexedItems;
        return m_indexedItems + unindexedItems;
    }

    // Rebuild the list of items to avoid holes. ### We could also just
    // compress the item lists at this point.
    QList<QGraphicsItem *> itemList;
    foreach (QGraphicsItem *item, m_indexedItems + unindexedItems) {
        if (item)
            itemList << item;
    }
    return itemList;
}

int QGraphicsSceneBspTreeIndex::bspDepth()
{
    return bspTreeDepth;
}

void QGraphicsSceneBspTreeIndex::setBspDepth(int depth)
{
    bspTreeDepth = depth;
    resetIndex();
}

void QGraphicsSceneBspTreeIndex::sceneRectChanged(const QRectF &rect)
{
    m_sceneRect = rect;
    resetIndex();
}

bool QGraphicsSceneBspTreeIndex::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Timer:
            if (indexTimerId && static_cast<QTimerEvent *>(event)->timerId() == indexTimerId) {
                if (restartIndexTimer) {
                    restartIndexTimer = false;
                } else {
                    // this call will kill the timer
                    _q_updateIndex();
                }
            }
     // Fallthrough intended - support timers in subclasses.
    default:
        return QObject::event(event);
    }
    return true;
}

static inline int intmaxlog(int n)
{
    return  (n > 0 ? qMax(qCeil(qLn(qreal(n)) / qLn(qreal(2))), 5) : 0);
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndex::_q_updateIndex()
{
    if (!indexTimerId)
        return;

    killTimer(indexTimerId);
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
                m_indexedItems[freeIndex] = item;
            } else {
                item->d_func()->index = m_indexedItems.size();
                m_indexedItems << item;
            }
        }
    }

    // Update growing scene rect.
    QRectF oldGrowingItemsBoundingRect = scene()->d_func()->growingItemsBoundingRect;
    scene()->d_func()->growingItemsBoundingRect |= unindexedItemsBoundingRect;

    // Determine whether we should regenerate the BSP tree.
    if (bspTreeDepth == 0) {
        int oldDepth = intmaxlog(lastItemCount);
        bspTreeDepth = intmaxlog(m_indexedItems.size());
        static const int slack = 100;
        if (bsp.leafCount() == 0 || (oldDepth != bspTreeDepth && qAbs(lastItemCount - m_indexedItems.size()) > slack)) {
            // ### Crude algorithm.
            regenerateIndex = true;
        }
    }

    // Regenerate the tree.
    if (regenerateIndex) {
        regenerateIndex = false;
        bsp.initialize(scene()->sceneRect(), bspTreeDepth);
        unindexedItems = m_indexedItems;
        lastItemCount = m_indexedItems.size();
        scene()->update();

        // Take this opportunity to reset our largest-item counter for
        // untransformable items. When the items are inserted into the BSP
        // tree, we'll get an accurate calculation.
        scene()->d_func()->largestUntransformableItem = QRectF();
    }

    // Insert all unindexed items into the tree.
    for (int i = 0; i < unindexedItems.size(); ++i) {
        if (QGraphicsItem *item = unindexedItems.at(i)) {
            QRectF rect = item->sceneBoundingRect();
            if (item->d_ptr->ancestorFlags & QGraphicsItemPrivate::AncestorClipsChildren)
                continue;

            bsp.insertItem(item,rect);

            // If the item ignores view transformations, update our
            // largest-item-counter to ensure that the view can accurately
            // discover untransformable items when drawing.
            if (item->d_ptr->itemIsUntransformable()) {
                QGraphicsItem *topmostUntransformable = item;
                while (topmostUntransformable && (topmostUntransformable->d_ptr->ancestorFlags
                                                  & QGraphicsItemPrivate::AncestorIgnoresTransformations)) {
                    topmostUntransformable = topmostUntransformable->parentItem();
                }
                // ### Verify that this is the correct largest untransformable rectangle.
                scene()->d_func()->largestUntransformableItem |= item->mapToItem(topmostUntransformable, item->boundingRect()).boundingRect();
            }
        }
    }
    unindexedItems.clear();

    // Notify scene rect changes.
    if (!scene()->d_func()->hasSceneRect && scene()->d_func()->growingItemsBoundingRect != oldGrowingItemsBoundingRect)
        emit scene()->sceneRectChanged(scene()->d_func()->growingItemsBoundingRect);
}


/*!
    \internal

    Removes stale pointers from all data structures.
*/
void QGraphicsSceneBspTreeIndex::purgeRemovedItems()
{
    if (!purgePending && removedItems.isEmpty())
        return;

    // Remove stale items from the BSP tree.
    bsp.removeItems(removedItems.toSet());
    // Purge this list.
    removedItems.clear();
    freeItemIndexes.clear();
    for (int i = 0; i < m_indexedItems.size(); ++i) {
        if (!m_indexedItems.at(i))
            freeItemIndexes << i;
    }
    purgePending = false;

    // No locality info for the items; update the whole scene.
    scene()->update();
}

/*!
    \internal

    Starts or restarts the timer used for reindexing unindexed items.
*/
void QGraphicsSceneBspTreeIndex::startIndexTimer()
{
    if (indexTimerId) {
        restartIndexTimer = true;
    } else {
        indexTimerId = startTimer(QGRAPHICSSCENE_INDEXTIMER_TIMEOUT);
    }
}

/*!
    \internal
*/
void QGraphicsSceneBspTreeIndex::resetIndex()
{
    purgeRemovedItems();
    for (int i = 0; i < m_indexedItems.size(); ++i) {
        if (QGraphicsItem *item = m_indexedItems.at(i)) {
            item->d_ptr->index = -1;
            unindexedItems << item;
        }
    }
    m_indexedItems.clear();
    freeItemIndexes.clear();
    regenerateIndex = true;
    startIndexTimer();
}
