#ifndef TANKITEM_H
#define TANKITEM_H

#include "tank.h"
#include "gameitem.h"

#include <QColor>

class Action;
class TankItem: public Tank, public GameItem
{
    Q_OBJECT
public:
    TankItem(QObject *parent = 0);
    
    virtual void moveForwards(qreal length);
    virtual void moveBackwards(qreal length);
    virtual void turn(qreal newDirection);
    virtual void stop();
    virtual qreal direction() const;
    virtual qreal distanceToObstacle() const;

    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

    void idle(qreal elapsed);
    void setDirection(qreal newDirection);

    qreal speed() const { return 90.0; }
    qreal angularSpeed() const { return 90.0; }

    QRectF boundingRect() const;

    void hitByRocket();

signals:
    virtual void fireCannon();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);    
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
    void setAction(Action *newAction);

    Action *m_currentAction;
    qreal m_currentDirection;
    QColor m_color;
};

#endif
