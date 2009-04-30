#ifndef ROCKETITEM_H
#define ROCKETITEM_H

#include "gameitem.h"

class RocketItem: public GameItem
{
public:
    enum { Type = UserType + 2 };

    RocketItem();

    int type() const { return Type; }
    virtual void idle(qreal elapsed);
    qreal speed() const { return 100.0; }
    void setDirection(qreal direction) { m_direction = direction; }
    
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;


private:
    qreal m_direction;
    qreal m_distance;
};


#endif
