#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>

class Node: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF position READ pos WRITE setPos);
public:
    Node(const QPointF &pos, QGraphicsItem *parent = 0);
    ~Node();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void positionChanged();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);

private:
    bool m_dragging;
};

#endif
