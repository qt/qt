#ifndef STICKMAN_H
#define STICKMAN_H

#include <QGraphicsItem>

const int LimbCount = 16;

class Node;
class QTimer;
class StickMan: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor WRITE setPenColor READ penColor)
    Q_PROPERTY(QColor fillColor WRITE setFillColor READ fillColor)
    Q_PROPERTY(bool isDead WRITE setIsDead READ isDead)
public:
    StickMan();
    ~StickMan();

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    int nodeCount() const; 
    Node *node(int idx) const;

    void setDrawSticks(bool on);
    bool drawSticks() const { return m_sticks; }

    QColor penColor() const { return m_penColor; }
    void setPenColor(const QColor &color) { m_penColor = color; }

    QColor fillColor() const { return m_fillColor; }
    void setFillColor(const QColor &color) { m_fillColor = color; }

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = isDead; }
   
public slots:
    void stabilize();
    void childPositionChanged();

protected:
    void timerEvent(QTimerEvent *e);

private:
    QPointF posFor(int idx) const;

    Node **m_nodes;
    qreal *m_perfectBoneLengths;
    
    uint m_sticks : 1;
    uint m_isDead : 1;
    uint m_reserved : 30;

    QPixmap m_pixmap;
    QColor m_penColor;
    QColor m_fillColor;
};

#endif // STICKMAN_H
