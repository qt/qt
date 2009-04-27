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
        m_reverse = m_distance < 0.0;
    }

    bool apply(qreal timeDelta) 
    {
        qreal dist = timeDelta * item()->speed() * (m_reverse ? -1.0 : 1.0);

        m_distance -= dist;
        if (m_reverse && m_distance > 0.0)
            return false;
        else if (m_distance < 0.0)
            return false;

        qreal a = item()->direction() * M_PI / 180.0;

        qreal yd = dist * sin(a);
        qreal xd = dist * sin(M_PI / 2.0 - a);

        item()->setPos(item()->pos() + QPointF(xd, yd));
        return true;
    }

private:
    qreal m_distance;
    bool m_reverse;
};

class TurnAction: public Action
{
public:
    TurnAction(TankItem *item, qreal distance)
        : Action(item), m_distance(distance)
    {
        m_reverse = m_distance < 0.0;
    }

    bool apply(qreal timeDelta) 
    {
        qreal dist = timeDelta * item()->angularSpeed() * (m_reverse ? -1.0 : 1.0);
        m_distance -= dist;
        if (m_reverse && m_distance > 0.0)
            return false;
        else if (m_distance < 0.0)
            return false;

        item()->setDirection(item()->direction() + dist);
        return true;
    }

private:
    qreal m_distance;
    bool m_reverse;
};

TankItem::TankItem(QObject *parent) : Tank(parent), m_currentAction(0), m_currentDirection(0.0)
{
    connect(this, SIGNAL(fireCannon()), this, SIGNAL(actionCompleted()));
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

void TankItem::hitByRocket()
{
    deleteLater();
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

void TankItem::turn(qreal degrees)
{
    setAction(new TurnAction(this, degrees));
}

void TankItem::stop()
{
    setAction(0);
}

QVariant TankItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF requestedPosition = value.toPointF();
        QLineF collidedLine;
        QPointF nextPoint = tryMove(requestedPosition, &collidedLine);
        if (nextPoint != requestedPosition)
            emit collision(collidedLine);
        return nextPoint;
    } else {
        return QGraphicsItem::itemChange(change, value);
    }
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
    qreal diff = newDirection - m_currentDirection;
    m_currentDirection = newDirection;
    rotate(diff);
}

qreal TankItem::distanceToObstacle() const
{
    // ###

    return 0.0;
}



