/*!
    \class QGraphicsSceneLinearIndex
    \brief The QGraphicsSceneLinearIndex class provides an implementation of
    a linear indexing algorithm for discovering items in QGraphicsScene.
    \since 4.6
    \ingroup multimedia
    \ingroup graphicsview-api
    \mainclass
    \internal

    QGraphicsSceneLinearIndex index is default linear implementation to discover items.
    It basically store all items in a list and return them to the scene.

    \sa QGraphicsScene, QGraphicsView, QGraphicsSceneIndex, QGraphicsSceneBspTreeIndex
*/

#include <private/qgraphicsscenelinearindex_p.h>

/*!
    \fn QGraphicsSceneLinearIndex::QGraphicsSceneLinearIndex(QGraphicsScene *scene = 0):

    Construct a linear index for the given \a scene.
*/

/*!
    \fn QList<QGraphicsItem *> QGraphicsSceneLinearIndex::items(Qt::SortOrder order = Qt::AscendingOrder) const;

    Return all items in the index and sort them using \a order.
*/


/*!
    \fn virtual QList<QGraphicsItem *> QGraphicsSceneLinearIndex::estimateItems(const QRectF &rect, Qt::SortOrder order, const QTransform &deviceTransform) const;

    Returns an estimation visible items that are either inside or
    intersect with the specified \a rect and return a list sorted using \a order.

    \a deviceTransform is the transformation apply to the view.

*/

/*!
    \fn QRectF QGraphicsSceneLinearIndex::indexedRect() const;
    \reimp
    Return the rect indexed by the the index.
*/

/*!
    \fn void QGraphicsSceneLinearIndex::clear();
    \reimp
    Clear the all the BSP index.
*/

/*!
    \fn virtual void QGraphicsSceneLinearIndex::addItem(QGraphicsItem *item);

    Add the \a item into the index.
*/

/*!
    \fn virtual void QGraphicsSceneLinearIndex::removeItem(QGraphicsItem *item);

    Add the \a item from the index.
*/

