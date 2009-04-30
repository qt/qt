#ifndef GAMEITEM_H
#define GAMEITEM_H

#include <QGraphicsItem>

class QLineF;
class GameItem: public QGraphicsItem
{
public:
    virtual void idle(qreal elapsed) = 0;

protected:
    QPointF tryMove(const QPointF &requestedPosition, QLineF *collidedLine = 0, 
        QGraphicsItem **collidedItem = 0) const;
};

#endif
