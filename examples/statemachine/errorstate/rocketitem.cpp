#include "rocketitem.h"
#include "tankitem.h"

#include <QPainter>
#include <QGraphicsScene>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RocketItem::RocketItem()
    : m_direction(0.0), m_distance(300.0)
{
}

QRectF RocketItem::boundingRect() const
{
    return QRectF(-1.0, -1.0, 2.0, 2.0);
}

void RocketItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::black);
    painter->drawEllipse(boundingRect());
}

void RocketItem::idle(qreal elapsed)
{
    qreal dist = elapsed * speed();

    m_distance -= dist;
    if (m_distance < 0.0) {
        scene()->removeItem(this);
        delete this;
        return;
    }

    qreal a = m_direction * M_PI / 180.0;

    qreal yd = dist * sin(a);
    qreal xd = dist * sin(M_PI / 2.0 - a);

    QPointF requestedPosition = pos() + QPointF(xd, yd);
    QGraphicsItem *collidedItem = 0;
    QPointF nextPosition = tryMove(requestedPosition, 0, &collidedItem);
    if (requestedPosition == nextPosition) {
        setPos(nextPosition);
    } else {
        if (TankItem *tankItem = qgraphicsitem_cast<TankItem *>(collidedItem))
            tankItem->hitByRocket();
        
        scene()->removeItem(this);
        delete this;
    }
}
