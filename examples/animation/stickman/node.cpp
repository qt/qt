#include "node.h"

#include <QRectF>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

Node::Node(const QPointF &pos, QGraphicsItem *parent)
  : QGraphicsItem(parent), m_dragging(false)
{
    setPos(pos);
}

Node::~Node()
{
}

QRectF Node::boundingRect() const
{
    return QRectF(-6.0, -6.0, 12.0, 12.0);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{    
    painter->setPen(Qt::white);
    painter->drawEllipse(QPointF(0.0, 0.0), 5.0, 5.0);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragging = true;
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_dragging)
        setPos(mapToParent(event->pos()));    
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_dragging = false;
}