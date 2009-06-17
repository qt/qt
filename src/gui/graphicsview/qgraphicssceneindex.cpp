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
    \class QGraphicsSceneIndex
    \brief The QGraphicsSceneIndex class provides a base class to implement
    a custom indexing algorithm for discovering items in QGraphicsScene.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api
    \mainclass
    \internal

    The QGraphicsSceneIndex class provides a base class to implement
    a custom indexing algorithm for discovering items in QGraphicsScene. You
    need to subclass it and reimplement addItem, removeItem, estimateItems
    and items in order to have an functional indexing.

    \sa QGraphicsScene, QGraphicsView
*/

#include "qgraphicssceneindex.h"
#include "qgraphicssceneindex_p.h"
#include "qgraphicsscene.h"
#include "qgraphicsitem_p.h"
#include "qgraphicsscene_p.h"

#ifndef QT_NO_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

class QGraphicsSceneIndexRectIntersector : public QGraphicsSceneIndexIntersector
{
public:
    QGraphicsSceneIndexRectIntersector(QGraphicsScene *scene) : QGraphicsSceneIndexIntersector(scene) {}
    bool intersect(const QRectF &brect) const
    {
        bool keep = true;
        if (mode == Qt::ContainsItemShape || mode == Qt::ContainsItemBoundingRect)
            keep = rect.contains(transform.mapRect(brect)) && rect != brect;
        else
            keep = rect.intersects(transform.mapRect(brect));

        if (keep && (mode == Qt::ContainsItemShape || mode == Qt::IntersectsItemShape)) {
            QPainterPath rectPath;
            rectPath.addRect(rect);
            keep = QGraphicsScenePrivate::get(scene)->itemCollidesWithPath(item, transform.inverted().map(rectPath), mode);
        }
        return keep;
    }
};

class QGraphicsSceneIndexPointIntersector : public QGraphicsSceneIndexIntersector
{
public:
    QGraphicsSceneIndexPointIntersector(QGraphicsScene *scene) : QGraphicsSceneIndexIntersector(scene) {}
    bool intersect(const QRectF &brect) const
    {
        bool keep = false;
        if (rect.intersects(transform.mapRect(brect)))
            if (item->contains(transform.inverted().map(pos)))
                keep = true;
        if (keep && (mode == Qt::ContainsItemShape || mode == Qt::IntersectsItemShape)) {
            QPainterPath rectPath;
            rectPath.addRect(rect);
            keep = QGraphicsScenePrivate::get(scene)->itemCollidesWithPath(item, transform.inverted().map(rectPath), mode);
        }
        return keep;
    }
    QPointF pos;
};

class QGraphicsSceneIndexPathIntersector : public QGraphicsSceneIndexIntersector
{
public:
    QGraphicsSceneIndexPathIntersector(QGraphicsScene *scene) : QGraphicsSceneIndexIntersector(scene) {}
    bool intersect(const QRectF &brect) const
    {
        bool keep = true;
        if (mode == Qt::ContainsItemShape || mode == Qt::ContainsItemBoundingRect)
            keep = rect.contains(transform.mapRect(brect)) && rect != brect;
        else
            keep = rect.intersects(transform.mapRect(brect));

        if (keep && (mode == Qt::ContainsItemShape || mode == Qt::IntersectsItemShape))
            keep = QGraphicsScenePrivate::get(scene)->itemCollidesWithPath(item, transform.inverted().map(path), mode);
        return keep;
    }
    QPainterPath path;
};

/*!
    Constructs a private scene index.
*/
QGraphicsSceneIndexPrivate::QGraphicsSceneIndexPrivate(QGraphicsScene *scene) : scene(scene)
{
    pointIntersector = new QGraphicsSceneIndexPointIntersector(scene);
    rectIntersector = new QGraphicsSceneIndexRectIntersector(scene);
    pathIntersector =  new QGraphicsSceneIndexPathIntersector(scene);
}

/*!
    Destructor of private scene index.
*/
QGraphicsSceneIndexPrivate::~QGraphicsSceneIndexPrivate()
{
    delete pointIntersector;
    delete rectIntersector;
    delete pathIntersector;
}

/*!
    \internal
*/
void QGraphicsSceneIndexPrivate::recursive_items_helper(QGraphicsItem *item, QGraphicsSceneIndexIntersector *intersector,
                                                   QList<QGraphicsItem *> *items,
                                                   const QTransform &parentTransform,
                                                   const QTransform &viewTransform,
                                                   Qt::ItemSelectionMode mode, Qt::SortOrder order,
                                                   qreal parentOpacity) const
{
    // Calculate opacity.
    qreal opacity;
    if (item) {
        if (!item->d_ptr->visible)
            return;
        QGraphicsItem *p = item->d_ptr->parent;
        bool itemIgnoresParentOpacity = item->d_ptr->flags & QGraphicsItem::ItemIgnoresParentOpacity;
        bool parentDoesntPropagateOpacity = (p && (p->d_ptr->flags & QGraphicsItem::ItemDoesntPropagateOpacityToChildren));
        if (!itemIgnoresParentOpacity && !parentDoesntPropagateOpacity) {
            opacity = parentOpacity * item->opacity();
        } else {
            opacity = item->d_ptr->opacity;
        }
        if (opacity == 0.0 && !(item->d_ptr->flags & QGraphicsItem::ItemDoesntPropagateOpacityToChildren))
            return;
    } else {
        opacity = parentOpacity;
    }

    // Calculate the full transform for this item.
    QTransform transform = parentTransform;
    bool keep = false;
    if (item) {
        item->d_ptr->combineTransformFromParent(&transform, &viewTransform);

        // ### This does not take the clip into account.
        QRectF brect = item->boundingRect();
        _q_adjustRect(&brect);

        //We fill the intersector with needed informations
        intersector->transform = transform;
        intersector->item = item;
        keep = intersector->intersect(brect);
    }

    bool childClip = (item && (item->d_ptr->flags & QGraphicsItem::ItemClipsChildrenToShape));
    bool dontProcessItem = !item || !keep;
    bool dontProcessChildren = item && dontProcessItem && childClip;

    // Find and sort children.
    QList<QGraphicsItem *> &children = item ? item->d_ptr->children : const_cast<QGraphicsScenePrivate *>(scene->d_func())->topLevelItems;
    if (!dontProcessChildren) {
        if (item && item->d_ptr->needSortChildren) {
            item->d_ptr->needSortChildren = 0;
            qStableSort(children.begin(), children.end(), qt_notclosestLeaf);
        } else if (!item && scene->d_func()->needSortTopLevelItems) {
            const_cast<QGraphicsScenePrivate *>(scene->d_func())->needSortTopLevelItems = false;
            qStableSort(children.begin(), children.end(), qt_notclosestLeaf);
        }
    }

    childClip &= !dontProcessChildren & !children.isEmpty();

    // Clip.
    if (childClip)
        intersector->rect &= transform.map(item->shape()).controlPointRect();

    // Process children behind
    int i = 0;
    if (!dontProcessChildren) {
        for (i = 0; i < children.size(); ++i) {
            QGraphicsItem *child = children.at(i);
            if (!(child->d_ptr->flags & QGraphicsItem::ItemStacksBehindParent))
                break;
            recursive_items_helper(child, intersector, items, transform, viewTransform,
                                   mode, order, opacity);
        }
    }

    // Process item
    if (!dontProcessItem)
        items->append(item);

    // Process children in front
    if (!dontProcessChildren) {
        for (; i < children.size(); ++i)
            recursive_items_helper(children.at(i), intersector, items, transform, viewTransform,
                                   mode, order, opacity);
    }

    if (!item && order == Qt::AscendingOrder) {
        int n = items->size();
        for (int i = 0; i < n / 2; ++i) {
            QGraphicsItem *tmp = (*items)[n - i - 1];
            (*items)[n - i - 1] = (*items)[i];
            (*items)[i] = tmp;
        }
    }
}

/*!
    Constructs an abstract scene index for a given \a scene.
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
    \fn QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPointF &pos,
    Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform
    &deviceTransform) const

    Returns all visible items that, depending on \a mode, are at the specified
    \a pos and return a list sorted using \a order.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with \a pos are returned.

    \a deviceTransform is the transformation apply to the view.

    This method use the estimation of the index (estimateItems) and refine the
    list to get an exact result. If you want to implement your own refinement
    algorithm you can reimplement this method.

    \sa estimateItems()

*/
QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPointF &pos, Qt::ItemSelectionMode mode,
                                                  Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> itemList;
    d->pointIntersector->mode = mode;
    d->pointIntersector->rect = QRectF(pos, QSize(1,1));
    d->pointIntersector->pos = pos;
    d->recursive_items_helper(0, d->pointIntersector, &itemList, QTransform(), deviceTransform, mode, order);
    return itemList;
}

/*!
    \fn QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QRectF &rect,
    Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform
    &deviceTransform) const

    \overload

    Returns all visible items that, depending on \a mode, are either inside or
    intersect with the specified \a rect and return a list sorted using \a order.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a rect are returned.

    \a deviceTransform is the transformation apply to the view.

    This method use the estimation of the index (estimateItems) and refine
    the list to get an exact result. If you want to implement your own
    refinement algorithm you can reimplement this method.

    \sa estimateItems()

*/
QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QRectF &rect, Qt::ItemSelectionMode mode,
                                                  Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> itemList;
    d->rectIntersector->mode = mode;
    d->rectIntersector->rect = rect;
    d->recursive_items_helper(0, d->rectIntersector, &itemList, QTransform(), deviceTransform, mode, order);
    return itemList;
}

/*!
    \fn QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPolygonF
    &polygon, Qt::ItemSelectionMode mode, Qt::SortOrder order, const
    QTransform &deviceTransform) const

    \overload

    Returns all visible items that, depending on \a mode, are either inside or
    intersect with the specified \a polygon and return a list sorted using \a order.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a polygon are returned.

    \a deviceTransform is the transformation apply to the view.

    This method use the estimation of the index (estimateItems) and refine
    the list to get an exact result. If you want to implement your own
    refinement algorithm you can reimplement this method.

    \sa estimateItems()

*/
QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPolygonF &polygon, Qt::ItemSelectionMode mode,
                                                  Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> itemList;
    QRectF polyRect(polygon.boundingRect());
     _q_adjustRect(&polyRect);
    d->pathIntersector->mode = mode;
    d->pathIntersector->rect = polyRect;
    QPainterPath path;
    path.addPolygon(polygon);
    d->pathIntersector->path = path;
    d->recursive_items_helper(0, d->pathIntersector, &itemList, QTransform(), deviceTransform, mode, order);
    return itemList;
}

/*!
    \fn QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPainterPath
    &path, Qt::ItemSelectionMode mode, Qt::SortOrder order, const QTransform
    &deviceTransform) const

    \overload

    Returns all visible items that, depending on \a mode, are either inside or
    intersect with the specified \a path and return a list sorted using \a order.

    The default value for \a mode is Qt::IntersectsItemShape; all items whose
    exact shape intersects with or is contained by \a path are returned.

    \a deviceTransform is the transformation apply to the view.

    This method use the estimation of the index (estimateItems) and refine
    the list to get an exact result. If you want to implement your own
    refinement algorithm you can reimplement this method.

    \sa estimateItems()

*/
QList<QGraphicsItem *> QGraphicsSceneIndex::items(const QPainterPath &path, Qt::ItemSelectionMode mode,
                                                  Qt::SortOrder order, const QTransform &deviceTransform) const
{
    Q_D(const QGraphicsSceneIndex);
    QList<QGraphicsItem *> itemList;
    QRectF pathRect(path.controlPointRect());
    _q_adjustRect(&pathRect);
    d->pathIntersector->mode = mode;
    d->pathIntersector->rect = pathRect;
    d->pathIntersector->path = path;
    d->recursive_items_helper(0, d->pathIntersector, &itemList, QTransform(), deviceTransform, mode, order);
    return itemList;
}

/*!
    This virtual function return an estimation of items at position \a point.
    This method return a list sorted using \a order.
    \a deviceTransform is the transformation apply to the view.
*/
QList<QGraphicsItem *> QGraphicsSceneIndex::estimateItems(const QPointF &point, Qt::SortOrder order,
                                                          const QTransform &deviceTransform) const
{
    return estimateItems(QRectF(point, QSize(1,1)), order, deviceTransform);
}

/*!
    \fn virtual QList<QGraphicsItem *>
    QGraphicsSceneIndex::estimateItems(const QRectF &rect, Qt::SortOrder
    order, const QTransform &deviceTransform) const = 0

    This pure virtual function return an estimation of items in the \a rect.
    This method return a list sorted using \a order.

    \a deviceTransform is the transformation apply to the view.
*/

/*!
    \fn virtual QList<QGraphicsItem *>
    QGraphicsSceneIndex::items(Qt::SortOrder order = Qt::AscendingOrder) const
    = 0; This pure virtual function all items in the index and sort them using
    \a order.
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
    \fn virtual void QGraphicsSceneIndex::addItem(QGraphicsItem *item) = 0

    This pure virtual function inserts an \a item to the scene index.

    \sa removeItem(), deleteItem()
*/

/*!
    \fn virtual void QGraphicsSceneIndex::removeItem(QGraphicsItem *item) = 0

    This pure virtual function removes an \a item to the scene index.

    \sa addItem(), deleteItem()
*/

/*!
    This method is called when an \a item has been deleted.
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
    that some part of the \a item 's state changes. By reimplementing this
    function, your can react to a change, and in some cases, (depending on \a
    change,) adjustments in the index can be made.

    \a change is the parameter of the item that is changing. \a value is the
    value that changed; the type of the value depends on \a change.

    The default implementation does nothing.

    \sa QGraphicsItem::GraphicsItemChange
*/
void QGraphicsSceneIndex::itemChanged(const QGraphicsItem *item, QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    Q_UNUSED(item);
    Q_UNUSED(change);
    Q_UNUSED(value);
}

/*!
    Notify the index for a geometry change of an \a item.

    \sa QGraphicsItem::prepareGeometryChange()
*/
void QGraphicsSceneIndex::prepareBoundingRectChange(const QGraphicsItem *item)
{
    Q_UNUSED(item);
}

/*!
    This virtual function is called when the scene changes its bounding
    rectangle. \a rect is the new value of the scene rectangle.
    \sa QGraphicsScene::sceneRect()
*/
void QGraphicsSceneIndex::sceneRectChanged(const QRectF &rect)
{
    Q_UNUSED(rect);
}

QT_END_NAMESPACE

#include "moc_qgraphicssceneindex.cpp"

#endif // QT_NO_GRAPHICSVIEW
