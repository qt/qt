#include "tankitem.h"

#include <QPainter>
#include <QGraphicsScene>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Action
{
public:
    Action(TankItem *item) : m_item(item)
    {
    }

    TankItem *item() const { return m_item; }
    void setItem(TankItem *item) { m_item = item; }

    virtual bool apply(qreal timeDelta) = 0;

private:
    TankItem *m_item;
};

class MoveAction: public Action
{
public:
    MoveAction(TankItem *item, qreal distance)
        : Action(item), m_distance(distance)
    {
    }

    bool apply(qreal timeDelta) 
    {
        qreal dist = timeDelta * item()->speed();
        m_distance -= dist;
        if (qFuzzyCompare(m_distance, 0.0))
            return false;        

        qreal a = item()->direction() * M_PI / 180.0;

        qreal yd = dist * sin(a);
        qreal xd = dist * sin(M_PI / 2.0 - a);

        item()->setPos(item()->pos() + QPointF(xd, yd));
        return true;
    }

private:
    qreal m_distance;
};

class TurnAction: public Action
{
public:
    TurnAction(TankItem *item, qreal distance)
        : Action(item), m_distance(distance)
    {
    }

    bool apply(qreal timeDelta) 
    {
        qreal dist = timeDelta * item()->angularSpeed();
        m_distance -= dist;
        if (qFuzzyCompare(m_distance, 0.0))
            return false;        

        item()->setDirection(item()->direction() + dist);
        return true;
    }

private:
    qreal m_distance;
};

class FireCannonAction: public Action
{
public:
    FireCannonAction(TankItem *item, qreal distance)
        : Action(item), m_distance(distance)
    {
    }

    bool apply(qreal )
    {
        return false;
    }

private:
    qreal m_distance;
};

TankItem::TankItem(QObject *parent) : Tank(parent), m_currentAction(0), m_currentDirection(0.0)
{
}

void TankItem::idle(qreal elapsed)
{
    if (m_currentAction != 0) {
        if (!m_currentAction->apply(elapsed)) {
            setAction(0);
            emit actionCompleted();
        }
    }
}

void TankItem::setAction(Action *newAction)
{
    if (m_currentAction != 0) 
        delete m_currentAction;

    m_currentAction = newAction;
}

void TankItem::moveForwards(qreal length)
{
    setAction(new MoveAction(this, length));
}

void TankItem::moveBackwards(qreal length)
{
    setAction(new MoveAction(this, -length));
}

void TankItem::turn(qreal newDirection)
{
    setAction(new TurnAction(this, m_currentDirection - newDirection));
}

void TankItem::stop()
{
    setAction(0);
}

void TankItem::fireCannon(qreal distance)
{
    setAction(new FireCannonAction(this, distance));
}

QPointF TankItem::tryMove(const QPointF &requestedPosition) const
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

    m_brp = mapFromScene(boundingRectPath);

    QList<QGraphicsItem *> itemsInRect = scene()->items(boundingRectPath, Qt::IntersectsItemBoundingRect);

    QPointF nextPoint = requestedPosition;
    QRectF sceneRect = scene()->sceneRect();

    QLineF collidedLine;
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
                collidedLine = rectLine;
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

    if (nextPoint != requestedPosition) {
        emit collision(collidedLine);        
    }

    return nextPoint;
}

QVariant TankItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene())
        return tryMove(value.toPointF());
    else 
        return QGraphicsItem::itemChange(change, value);
}


void TankItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QRectF brect = boundingRect();

    painter->setBrush(m_color);
    painter->setPen(Qt::black);

    // body
    painter->drawRect(brect.adjusted(0.0, 4.0, -2.0, -4.0));

    // cannon
    QRectF cannonBase = brect.adjusted(10.0, 6.0, -12.0, -6.0);
    painter->drawEllipse(cannonBase);

    painter->drawRect(QRectF(QPointF(cannonBase.center().x(), cannonBase.center().y() - 2.0), 
                             QPointF(brect.right(), cannonBase.center().y() + 2.0)));
    
    // left track
    painter->setBrush(QBrush(Qt::black, Qt::VerPattern));
    QRectF leftTrackRect = QRectF(brect.topLeft(), QPointF(brect.right() - 2.0, brect.top() + 4.0));
    painter->fillRect(leftTrackRect, Qt::darkYellow);
    painter->drawRect(leftTrackRect);

    // right track
    QRectF rightTrackRect = QRectF(QPointF(brect.left(), brect.bottom() - 4.0),
                                   QPointF(brect.right() - 2.0, brect.bottom()));
    painter->fillRect(rightTrackRect, Qt::darkYellow);
    painter->drawRect(rightTrackRect);

    painter->setBrush(QColor::fromRgb(255, 0, 0, 128));
    painter->drawPolygon(m_brp);

}

QRectF TankItem::boundingRect() const
{
    return QRectF(-20.0, -10.0, 40.0, 20.0);
}

qreal TankItem::direction() const
{
    return m_currentDirection;
}

void TankItem::setDirection(qreal newDirection)
{
    m_currentDirection = newDirection;
    rotate(newDirection);
}

qreal TankItem::distanceToObstacle() const
{
    // ###

    return 0.0;
}



