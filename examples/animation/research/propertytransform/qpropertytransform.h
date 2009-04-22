#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtGui/QGraphicsItem>
#include <QtGui/QTransform>
#include <QtCore/QDebug>

/**
 * Experimental.
 * Pros:
 *          1. Does not add ugly/confusing API to QGraphicsItem.
 *
 * Cons:
 *          1. apply() /m_item->setTransform() is called too many times. (FIXED NOW?)
 *
 * 
 */
class QPropertyTransform : public QObject {
    Q_OBJECT
public:
    Q_PROPERTY(QPointF center READ center WRITE setCenter);
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation);
    Q_PROPERTY(qreal scaleX READ scaleX WRITE setScaleX);
    Q_PROPERTY(qreal scaleY READ scaleY WRITE setScaleY);
public:
    QPropertyTransform() : m_item(0), m_rotationZ(0), m_scaleX(1.), m_scaleY(1.) {}

    void setTargetItem(QGraphicsItem *item) {
        m_item = item;
    }

    void setCenter(const QPointF &center) {
        m_center = center;
        apply();
    }

    QPointF center() const { return m_center; }

    void setRotation(qreal rotation) {
        m_rotationZ = rotation;
        apply();
    }

    qreal rotation() const { return m_rotationZ; }

    void setScaleX(qreal scale) {
        m_scaleX = scale;
        apply();
    }

    qreal scaleX() const { return m_scaleX; }

    void setScaleY(qreal scale) {
        m_scaleY = scale;
        apply();
    }

    qreal scaleY() const { return m_scaleY; }
    
private:
    QTransform transform() const {
        return QTransform().translate(m_center.x(), m_center.y())
                         .rotate(m_rotationZ)
                         .scale(m_scaleX, m_scaleY)
                         .translate(-m_center.x(), -m_center.y());
    }

    void apply() {
        if (m_item)
            m_item->setTransform(transform());
    }

    QGraphicsItem *m_item;
    QPointF m_center;
    qreal m_rotationZ;
    qreal m_scaleX;
    qreal m_scaleY;
};

