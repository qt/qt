#ifndef GAMEITEM_H
#define GAMEITEM_H

#include <QGraphicsItem>

QT_BEGIN_NAMESPACE
class QLineF;
QT_END_NAMESPACE
class GameItem: public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    enum { Type = UserType + 1 };
    int type() const { return Type; }

    GameItem(QObject *parent = 0);

    virtual void idle(qreal elapsed) = 0;

protected:
    QPointF tryMove(const QPointF &requestedPosition, QLineF *collidedLine = 0, 
        QGraphicsItem **collidedItem = 0) const;
};

#endif
