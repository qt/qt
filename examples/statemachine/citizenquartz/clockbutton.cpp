#include "clockbutton.h"

#include <QPainter>

ClockButton::ClockButton(const QString &name, QGraphicsItem *parent) : QGraphicsItem(parent)
{
    setObjectName(name);
    setToolTip(name);
    setAcceptedMouseButtons(Qt::LeftButton);
}

QRectF ClockButton::boundingRect() const
{
    return QRectF(-10.0, -10.0, 20.0, 20.0);
}

QPainterPath ClockButton::shape() const
{
    QPainterPath path;
    path.addRoundedRect(boundingRect(), 15.0, 15.0, Qt::RelativeSize);

    return path;
}

void ClockButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::black);
    painter->drawPath(shape());
}

void ClockButton::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    emit pressed();
}

void ClockButton::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    emit released();
}