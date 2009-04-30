#ifndef TANKITEM_H
#define TANKITEM_H

#include "gameitem.h"

#include <QColor>

class Action;
class TankItem: public GameItem
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(qreal direction READ direction WRITE turn)
    Q_PROPERTY(qreal distanceToObstacle READ distanceToObstacle)
public:   
    TankItem(QObject *parent = 0);
        
    void setColor(const QColor &color) { m_color = color; }
    QColor color() const { return m_color; }

    void idle(qreal elapsed);
    void setDirection(qreal newDirection);

    qreal speed() const { return 90.0; }
    qreal angularSpeed() const { return 90.0; }

    QRectF boundingRect() const;

    void hitByRocket();

    void setEnabled(bool b) { m_enabled = b; }
    bool enabled() const { return m_enabled; }

    qreal direction() const;
    qreal distanceToObstacle() const;
    qreal distanceToObstacle(QGraphicsItem **item) const;

signals:
    void tankSpotted(qreal direction, qreal distance);
    void collision(const QLineF &collidedLine);
    void actionCompleted();
    void cannonFired();

public slots:
    void moveForwards(qreal length);
    void moveBackwards(qreal length);
    void turn(qreal newDirection);
    void stop();
    void fireCannon();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);    
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);

private:
    void setAction(Action *newAction);

    Action *m_currentAction;
    qreal m_currentDirection;
    QColor m_color;
    bool m_enabled;
};

#endif
