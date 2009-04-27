#include "gameitem.h"

#include <QGraphicsScene>

QPointF GameItem::tryMove(const QPointF &requestedPosition, QLineF *collidedLine, 
                          QGraphicsItem **collidedItem) const
{
    QLineF movementPath(pos(), requestedPosition);    

    qreal cannonLength = 0.0;
    {
        QPointF p1 = boundingRect().center();
        QPointF p2 = QPointF(boundingRect().right() + 10.0, p1.y());

        cannonLength = QLineF(mapToScene(p1), mapToScene(p2)).length();
    }

    movementPath.setLength(movementPath.length() + cannonLength);

    QRectF boundingRectPath(QPointF(qMin(movementPath.x1(), movementPath.x2()), qMin(movementPath.y1(), movementPath.y2())),
                            QPointF(qMax(movementPath.x1(), movementPath.x2()), qMax(movementPath.y1(), movementPath.y2())));

    QList<QGraphicsItem *> itemsInRect = scene()->items(boundingRectPath, Qt::IntersectsItemBoundingRect);

    QPointF nextPoint = requestedPosition;
    QRectF sceneRect = scene()->sceneRect();

    foreach (QGraphicsItem *item, itemsInRect) {
        if (item == static_cast<const QGraphicsItem *>(this))
            continue; 

        QPolygonF mappedBoundingRect = item->mapToScene(item->boundingRect());
        for (int i=0; i<mappedBoundingRect.size(); ++i) {
            QPointF p1 = mappedBoundingRect.at(i == 0 ? mappedBoundingRect.size()-1 : i-1);
            QPointF p2 = mappedBoundingRect.at(i);

            QLineF rectLine(p1, p2);
            QPointF intersectionPoint;
            QLineF::IntersectType intersectType = movementPath.intersect(rectLine, &intersectionPoint);

            if (intersectType == QLineF::BoundedIntersection) {
                movementPath.setP2(intersectionPoint);
                movementPath.setLength(movementPath.length() - cannonLength);
                nextPoint = movementPath.p2();

                if (collidedLine != 0)
                    *collidedLine = rectLine;

                if (collidedItem != 0)
                    *collidedItem = item;
            }
        }
    }

    // Don't go outside of map
    if (nextPoint.x() < sceneRect.left())
        nextPoint.rx() = sceneRect.left();
    if (nextPoint.x() > sceneRect.right())
        nextPoint.rx() = sceneRect.right();
    if (nextPoint.y() < sceneRect.top())
        nextPoint.ry() = sceneRect.top();
    if (nextPoint.y() > sceneRect.bottom())
        nextPoint.ry() = sceneRect.bottom();

    return nextPoint;
}

