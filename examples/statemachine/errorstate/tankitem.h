#ifndef TANKITEM_H
#define TANKITEM_H

#include "tank.h"

#include <QGraphicsItem>
#include <QColor>

class Action;
class TankItem: public Tank, public QGraphicsItem
{
    Q_OBJECT
public:
    TankItem(QObject *parent = 0);
    
    virtual void moveForwards(qreal length);
    virtual void moveBackwards(qreal length);
    virtual void turn(qreal newDirection);
    virtual void stop();
    virtual void fireCannon(qreal distance);
    virtual qreal direction() const;
    virtual qreal distanceToObstacle() const;

    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

    void idle(qreal elapsed);
    void setDirection(qreal newDirection);

    qreal speed() const { return 20.0; }
    qreal angularSpeed() const { return 1.0; }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
    QPointF tryMove(const QPointF &requestedPosition) const;
    void setAction(Action *newAction);

    Action *m_currentAction;
    qreal m_currentDirection;
    QColor m_color;
    mutable QPolygonF m_brp;
};

#endif
