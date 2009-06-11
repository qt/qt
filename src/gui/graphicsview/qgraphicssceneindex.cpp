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

#include "qgraphicssceneindex.h"
#include "qgraphicssceneindex_p.h"
#include "qgraphicsscenebsptreeindex_p_p.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem_p.h"
#include "qgraphicsscene_p.h"

#ifndef QT_NO_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

/*!
    Constructs a private scene index.
*/
QGraphicsSceneIndexPrivate::QGraphicsSceneIndexPrivate(QGraphicsScene *scene) : scene(scene)
{
}

void QGraphicsSceneIndexPrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QPointF &pos) const
{
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    if (parentClip && parent->d_ptr->isClippedAway())
        return;
    // ### is this needed?
    if (parentClip && !parent->boundingRect().contains(pos))
        return;

    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->transformData && !item->d_ptr->transformData->computedFullTransform().isInvertible())
            continue;

        // Skip invisible items and all their children.
        if (item->d_ptr->isInvisible())
            continue;

        bool keep = false;
        if (!item->d_ptr->isClippedAway()) {
            if (item->contains(item->mapFromParent(pos))) {
                items->append(item);
                keep = true;
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty())
            // Recurse into children.
            childItems_helper(items, item, item->mapFromParent(pos));
    }
}


void QGraphicsSceneIndexPrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QRectF &rect,
                                              Qt::ItemSelectionMode mode) const
{
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    if (parentClip && parent->d_ptr->isClippedAway())
        return;
    QRectF adjustedRect(rect);
    _q_adjustRect(&adjustedRect);
    QRectF r = !parentClip ? adjustedRect : adjustedRect.intersected(adjustedItemBoundingRect(parent));
    if (r.isEmpty())
        return;

    QPainterPath path;
    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->transformData && !item->d_ptr->transformData->computedFullTransform().isInvertible())
            continue;

        // Skip invisible items and all their children.
        if (item->d_ptr->isInvisible())
            continue;

        bool keep = false;
        if (!item->d_ptr->isClippedAway()) {
            // ### _q_adjustedRect is only needed because QRectF::intersects,
            // QRectF::contains and QTransform::map() and friends don't work with
            // flat rectangles.
            const QRectF br(adjustedItemBoundingRect(item));
            QRectF mbr = item->mapRectToParent(br);
            if (mode >= Qt::ContainsItemBoundingRect) {
                // Rect intersects/contains item's bounding rect
                if ((mode == Qt::IntersectsItemBoundingRect && QRectF_intersects(rect, mbr))
                    || (mode == Qt::ContainsItemBoundingRect && rect != mbr && rect.contains(br))) {
                    items->append(item);
                    keep = true;
                }
            } else {
                // Rect intersects/contains item's shape
                if (QRectF_intersects(rect, mbr)) {
                    if (path == QPainterPath())
                        path.addRect(rect);
                    if (scene->d_func()->itemCollidesWithPath(item, item->mapFromParent(path), mode)) {
                        items->append(item);
                        keep = true;
                    }
                }
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty()) {
            // Recurse into children.
            if (!item->d_ptr->transformData || item->d_ptr->transformData->computedFullTransform().type() <= QTransform::TxScale) {
                // Rect
                childItems_helper(items, item, item->mapRectFromParent(rect), mode);
            } else {
                // Polygon
                childItems_helper(items, item, item->mapFromParent(rect), mode);
            }
        }
    }
}


void QGraphicsSceneIndexPrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QPolygonF &polygon,
                                              Qt::ItemSelectionMode mode) const
{
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    if (parentClip && parent->d_ptr->isClippedAway())
        return;
    QRectF polyRect(polygon.boundingRect());
    _q_adjustRect(&polyRect);
    QRectF r = !parentClip ? polyRect : polyRect.intersected(adjustedItemBoundingRect(parent));
    if (r.isEmpty())
        return;

    QPainterPath path;
    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->transformData && !item->d_ptr->transformData->computedFullTransform().isInvertible())
            continue;

        // Skip invisible items.
        if (item->d_ptr->isInvisible())
            continue;

        bool keep = false;
        if (!item->d_ptr->isClippedAway()) {
            // ### _q_adjustedRect is only needed because QRectF::intersects,
            // QRectF::contains and QTransform::map() and friends don't work with
            // flat rectangles.
            const QRectF br(adjustedItemBoundingRect(item));
            if (mode >= Qt::ContainsItemBoundingRect) {
                // Polygon contains/intersects item's bounding rect
                if (path == QPainterPath())
                    path.addPolygon(polygon);
                if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(item->mapRectToParent(br)))
                    || (mode == Qt::ContainsItemBoundingRect && path.contains(item->mapRectToParent(br)))) {
                    items->append(item);
                    keep = true;
                }
            } else {
                // Polygon contains/intersects item's shape
                if (QRectF_intersects(polyRect, item->mapRectToParent(br))) {
                    if (path == QPainterPath())
                        path.addPolygon(polygon);
                    if (scene->d_func()->itemCollidesWithPath(item, item->mapFromParent(path), mode)) {
                        items->append(item);
                        keep = true;
                    }
                }
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty()) {
            // Recurse into children that clip children.
            childItems_helper(items, item, item->mapFromParent(polygon), mode);
        }
    }
}

void QGraphicsSceneIndexPrivate::childItems_helper(QList<QGraphicsItem *> *items,
                                              const QGraphicsItem *parent,
                                              const QPainterPath &path,
                                              Qt::ItemSelectionMode mode) const
{
    bool parentClip = (parent->flags() & QGraphicsItem::ItemClipsChildrenToShape);
    if (parentClip && parent->d_ptr->isClippedAway())
        return;
    QRectF pathRect(path.boundingRect());
    _q_adjustRect(&pathRect);
    QRectF r = !parentClip ? pathRect : pathRect.intersected(adjustedItemBoundingRect(parent));
    if (r.isEmpty())
        return;

    QList<QGraphicsItem *> &children = parent->d_ptr->children;
    for (int i = 0; i < children.size(); ++i) {
        QGraphicsItem *item = children.at(i);
        if (item->d_ptr->transformData && !item->d_ptr->transformData->computedFullTransform().isInvertible())
            continue;

        // Skip invisible items.
        if (item->d_ptr->isInvisible())
            continue;

        bool keep = false;
        if (!item->d_ptr->isClippedAway()) {
            // ### _q_adjustedRect is only needed because QRectF::intersects,
            // QRectF::contains and QTransform::map() and friends don't work with
            // flat rectangles.
            const QRectF br(adjustedItemBoundingRect(item));
            if (mode >= Qt::ContainsItemBoundingRect) {
                // Polygon contains/intersects item's bounding rect
                if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(item->mapRectToParent(br)))
                    || (mode == Qt::ContainsItemBoundingRect && path.contains(item->mapRectToParent(br)))) {
                    items->append(item);
                    keep = true;
                }
            } else {
                // Path contains/intersects item's shape
                if (QRectF_intersects(pathRect, item->mapRectToParent(br))) {
                    if (scene->d_func()->itemCollidesWithPath(item, item->mapFromParent(path), mode)) {
                        items->append(item);
                        keep = true;
                    }
                }
            }
        }

        if ((keep || !(item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) && !item->d_ptr->children.isEmpty()) {
            // Recurse into children that clip children.
            childItems_helper(items, item, item->mapFromParent(path), mode);
        }
    }
}

/*!
    Constructs an abstract scene index.
*/
QGraphicsSceneIndex::QGraphicsSceneIndex(QGraphicsScene *scene)
: QObject(*new QGraphicsSceneIndexPrivate(scene), scene)
{
}

/*!
    \internal
*/
QGraphicsSceneIndex::QGraphicsSceneIndex(QObjectPrivate &dd, QGraphicsScene *scene)
    : QObject(dd, scene)
{
}

/*!
    Destroys the scene index.
*/
QGraphicsSceneIndex::~QGraphicsSceneIndex()
{

}

/*!
    Returns the scene of this index.
*/
QGraphicsScene* QGraphicsSceneIndex::scene() const
{
    Q_D(const QGraphicsSceneIndex);
    return d->scene;
}

/*!
    Returns the indexed area for the index
*/
QRectF QGraphicsSceneIndex::indexedRect() const
{
    Q_D(const QGraphicsSceneIndex);
    return d->scene->d_func()->sceneRect;
}

/*!
    \fn QList<QGraphicsItem *> items() const  = 0

    This pure virtual function return the list of items that are actually in the index.

*/

QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPointF &pos, Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> items;

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    QRectF adjustedRect = QRectF(pos, QSize(1,1));
    foreach (QGraphicsItem *item, estimateItems(adjustedRect, order, deviceTransform)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        const QRectF br(adjustedItemBoundingRect(item));
            // Rect intersects/contains item's shape
        if (QRectF_intersects(adjustedRect, x.mapRect(br))) {
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok) {
                if (item->contains(xinv.map(pos))) {
                    items << item;
                    keep = true;
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            // Recurse into children that clip children.
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok)
                d->childItems_helper(&items, item, xinv.map(pos));
        }
    }
    //### Needed but it should be handle differently
    if (order != Qt::SortOrder(-1))
        QGraphicsSceneBspTreeIndexPrivate::sortItems(&items, order, false);
    return items;
}

QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QRectF &rect, Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> items;

    QPainterPath path;

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    QRectF adjustedRect(rect);
    _q_adjustRect(&adjustedRect);
    foreach (QGraphicsItem *item, estimateItems(adjustedRect, order, deviceTransform)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        const QRectF br(adjustedItemBoundingRect(item));
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Rect intersects/contains item's bounding rect
            QRectF mbr = x.mapRect(br);
            if ((mode == Qt::IntersectsItemBoundingRect && QRectF_intersects(rect, mbr))
                || (mode == Qt::ContainsItemBoundingRect && rect != mbr && rect.contains(mbr))) {
                items << item;
                keep = true;
            }
        } else {
            // Rect intersects/contains item's shape
            if (QRectF_intersects(adjustedRect, x.mapRect(br))) {
                bool ok;
                QTransform xinv = x.inverted(&ok);
                if (ok) {
                    if (path.isEmpty())
                        path.addRect(rect);
                    if (d->scene->d_func()->itemCollidesWithPath(item, xinv.map(path), mode)) {
                        items << item;
                        keep = true;
                    }
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            // Recurse into children that clip children.
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok) {
                if (x.type() <= QTransform::TxScale) {
                    // Rect
                    d->childItems_helper(&items, item, xinv.mapRect(rect), mode);
                } else {
                    // Polygon
                    d->childItems_helper(&items, item, xinv.map(rect), mode);
                }
            }
        }
    }
    //### Needed but it should be handle differently
    if (order != Qt::SortOrder(-1))
        QGraphicsSceneBspTreeIndexPrivate::sortItems(&items, order, false);
    return items;
}

QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPolygonF &polygon, Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> items;

    QRectF polyRect(polygon.boundingRect());
     _q_adjustRect(&polyRect);
    QPainterPath path;

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    foreach (QGraphicsItem *item, estimateItems(polyRect, order, deviceTransform)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        const QRectF br(adjustedItemBoundingRect(item));
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Polygon contains/intersects item's bounding rect
            if (path == QPainterPath())
                path.addPolygon(polygon);
            if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(x.mapRect(br)))
                || (mode == Qt::ContainsItemBoundingRect && path.contains(x.mapRect(br)))) {
                items << item;
                keep = true;
            }
        } else {
            // Polygon contains/intersects item's shape
            if (QRectF_intersects(polyRect, x.mapRect(br))) {
                bool ok;
                QTransform xinv = x.inverted(&ok);
                if (ok) {
                    if (path == QPainterPath())
                        path.addPolygon(polygon);
                    if (d->scene->d_func()->itemCollidesWithPath(item, xinv.map(path), mode)) {
                        items << item;
                        keep = true;
                    }
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            // Recurse into children that clip children.
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok)
                d->childItems_helper(&items, item, xinv.map(polygon), mode);
        }
    }
    //### Needed but it should be handle differently
    if (order != Qt::SortOrder(-1))
        QGraphicsSceneBspTreeIndexPrivate::sortItems(&items, order, false);
    return items;
}
QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPainterPath &path, Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> items;
    QRectF pathRect(path.controlPointRect());
    _q_adjustRect(&pathRect);

    // The index returns a rough estimate of what items are inside the rect.
    // Refine it by iterating through all returned items.
    foreach (QGraphicsItem *item, estimateItems(pathRect, order, deviceTransform)) {
        // Find the item's scene transform in a clever way.
        QTransform x = item->sceneTransform();
        bool keep = false;

        // ### _q_adjustedRect is only needed because QRectF::intersects,
        // QRectF::contains and QTransform::map() and friends don't work with
        // flat rectangles.
        const QRectF br(adjustedItemBoundingRect(item));
        if (mode >= Qt::ContainsItemBoundingRect) {
            // Path contains/intersects item's bounding rect
            if ((mode == Qt::IntersectsItemBoundingRect && path.intersects(x.mapRect(br)))
                || (mode == Qt::ContainsItemBoundingRect && path.contains(x.mapRect(br)))) {
                items << item;
                keep = true;
            }
        } else {
            // Path contains/intersects item's shape
            if (QRectF_intersects(pathRect, x.mapRect(br))) {
                bool ok;
                QTransform xinv = x.inverted(&ok);
                if (ok) {
                    if (d->scene->d_func()->itemCollidesWithPath(item, xinv.map(path), mode)) {
                        items << item;
                        keep = true;
                    }
                }
            }
        }

        if (keep && (item->flags() & QGraphicsItem::ItemClipsChildrenToShape)) {
            bool ok;
            QTransform xinv = x.inverted(&ok);
            if (ok)
                d->childItems_helper(&items, item, xinv.map(path), mode);
        }
    }
    //### Needed but it should be handle differently
    if (order != Qt::SortOrder(-1))
        QGraphicsSceneBspTreeIndexPrivate::sortItems(&items, order, false);
    return items;
}

/*!
    This pure virtual function return an estimation of items at position \a pos.

*/
QList<QGraphicsItem *> QGraphicsSceneIndex::estimateItems(const QPointF &point, Qt::SortOrder order, const QTransform &deviceTransform) const
{
    return estimateItems(QRectF(point, QSize(1,1)), order, deviceTransform);
}

/*!
    \fn virtual QList<QGraphicsItem *> estimateItems(const QRectF &rect, Qt::SortOrder order, const QTransform &deviceTransform) const = 0

    This pure virtual function return an estimation of items in the \a rect.

*/


/*!
    This virtual function removes all items in the scene index.
*/
void QGraphicsSceneIndex::clear()
{
    for (int i = 0 ; i < items().size(); ++i)
        removeItem(items().at(i));
}

/*!
    \fn virtual void addItem(QGraphicsItem *item) = 0

    This pure virtual function inserts an item to the scene index.

    \sa removeItem(), deleteItem()
*/

/*!
    \fn virtual void removeItem(QGraphicsItem *item) = 0

    This pure virtual function removes an item to the scene index.

    \sa addItem(), deleteItem()
*/

/*!
    This method is called when an item has been deleted.
    The default implementation call removeItem. Be carefull,
    if your implementation of removeItem use pure virtual method
    of QGraphicsItem like boundingRect(), then you should reimplement
    this method.

    \sa addItem(), removeItem()
*/
void QGraphicsSceneIndex::deleteItem(QGraphicsItem *item)
{
    removeItem(item);
}

/*!
    This virtual function is called by QGraphicsItem to notify the index
    that some part of the item's state changes. By reimplementing this
    function, your can react to a change, and in some cases, (depending on \a
    change,) adjustments in the index can be made.

    \a change is the parameter of the item that is changing. \a value is the
    value that changed; the type of the value depends on \a change.

    The default implementation does nothing.

    \sa GraphicsItemChange
*/
void QGraphicsSceneIndex::itemChanged(const QGraphicsItem *item, QGraphicsItem::GraphicsItemChange, const QVariant &value)
{
}

/*!
    Notify the index for a geometry change of an item.

    \sa QGraphicsItem::prepareGeometryChange
*/
void QGraphicsSceneIndex::prepareBoundingRectChange(const QGraphicsItem *item)
{
}

/*!
    This virtual function is called when the scene changes its bounding
    rectangle.
    \sa QGraphicsScene::sceneRect
*/
void QGraphicsSceneIndex::sceneRectChanged(const QRectF &rect)
{
}

QT_END_NAMESPACE

#include "moc_qgraphicssceneindex.cpp"

#endif // QT_NO_GRAPHICSVIEW
