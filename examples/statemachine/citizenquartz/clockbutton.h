#ifndef CLOCKBUTTON_H
#define CLOCKBUTTON_H

#include <QGraphicsItem>

class ClockButton: public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    ClockButton(const QString &name, QGraphicsItem *parent = 0);

    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

protected: 
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals: 
    void pressed();
    void released();
};

#endif //CLOCKBUTTON_H
