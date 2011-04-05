#ifndef POINTVECTOR_H
#define POINTVECTOR_H
#include "varyingvector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class PointVector : public VaryingVector
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal xVariation READ xVariation WRITE setXVariation NOTIFY xVariationChanged)
    Q_PROPERTY(qreal yVariation READ yVariation WRITE setYVariation NOTIFY yVariationChanged)
public:
    explicit PointVector(QObject *parent = 0);
    virtual const QPointF &sample(const QPointF &from);
    qreal x() const
    {
        return m_x;
    }

    qreal y() const
    {
        return m_y;
    }

    qreal xVariation() const
    {
        return m_xVariation;
    }

    qreal yVariation() const
    {
        return m_yVariation;
    }

signals:

    void xChanged(qreal arg);

    void yChanged(qreal arg);

    void xVariationChanged(qreal arg);

    void yVariationChanged(qreal arg);

public slots:
    void setX(qreal arg)
    {
        if (m_x != arg) {
            m_x = arg;
            emit xChanged(arg);
        }
    }

    void setY(qreal arg)
    {
        if (m_y != arg) {
            m_y = arg;
            emit yChanged(arg);
        }
    }

    void setXVariation(qreal arg)
    {
        if (m_xVariation != arg) {
            m_xVariation = arg;
            emit xVariationChanged(arg);
        }
    }

    void setYVariation(qreal arg)
    {
        if (m_yVariation != arg) {
            m_yVariation = arg;
            emit yVariationChanged(arg);
        }
    }

private:

    qreal m_x;
    qreal m_y;
    qreal m_xVariation;
    qreal m_yVariation;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // POINTVECTOR_H
