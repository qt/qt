#ifndef GAMEITEM_H
#define GAMEITEM_H

#include <QGraphicsItem>

class QLineF;
class GameItem: public QGraphicsItem
{
public:
    enum { Type = UserType + 1 };

    int type() const { return Type; }
    virtual void idle(qreal elapsed) = 0;

    virtual void hitByRocket() = 0;

protected:
    QPointF tryMove(const QPointF &requestedPosition, QLineF *collidedLine = 0, 
        QGraphicsItem **collidedItem = 0) const;
};

#endif
