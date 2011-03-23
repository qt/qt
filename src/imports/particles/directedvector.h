#ifndef DIRECTEDVECTOR_H
#define DIRECTEDVECTOR_H
#include "varyingvector.h"
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class DirectedVector : public VaryingVector
{
    Q_OBJECT
    Q_PROPERTY(qreal targetX READ targetX WRITE setTargetX NOTIFY targetXChanged)
    Q_PROPERTY(qreal targetY READ targetY WRITE setTargetY NOTIFY targetYChanged)

    Q_PROPERTY(qreal targetVariation READ targetVariation WRITE setTargetVariation NOTIFY targetVariationChanged)

    Q_PROPERTY(bool proportionalMagnitude READ proportionalMagnitude WRITE setProportionalMagnitude NOTIFY proprotionalMagnitudeChanged)
    Q_PROPERTY(qreal magnitude READ magnitude WRITE setMagnitude NOTIFY magnitudeChanged)
    Q_PROPERTY(qreal magnitudeVariation READ magnitudeVariation WRITE setMagnitudeVariation NOTIFY magnitudeVariationChanged)

public:
    explicit DirectedVector(QObject *parent = 0);
    virtual const QPointF &sample(const QPointF &from);

    qreal targetX() const
    {
        return m_targetX;
    }

    qreal targetY() const
    {
        return m_targetY;
    }

    qreal targetVariation() const
    {
        return m_targetVariation;
    }

    qreal magnitude() const
    {
        return m_magnitude;
    }

    bool proportionalMagnitude() const
    {
        return m_proportionalMagnitude;
    }

    qreal magnitudeVariation() const
    {
        return m_magnitudeVariation;
    }

signals:

    void targetXChanged(qreal arg);

    void targetYChanged(qreal arg);

    void targetVariationChanged(qreal arg);

    void magnitudeChanged(qreal arg);

    void proprotionalMagnitudeChanged(bool arg);

    void magnitudeVariationChanged(qreal arg);

public slots:
    void setTargetX(qreal arg)
    {
        if (m_targetX != arg) {
            m_targetX = arg;
            emit targetXChanged(arg);
        }
    }

    void setTargetY(qreal arg)
    {
        if (m_targetY != arg) {
            m_targetY = arg;
            emit targetYChanged(arg);
        }
    }

    void setTargetVariation(qreal arg)
    {
        if (m_targetVariation != arg) {
            m_targetVariation = arg;
            emit targetVariationChanged(arg);
        }
    }

    void setMagnitude(qreal arg)
    {
        if (m_magnitude != arg) {
            m_magnitude = arg;
            emit magnitudeChanged(arg);
        }
    }

    void setProportionalMagnitude(bool arg)
    {
        if (m_proportionalMagnitude != arg) {
            m_proportionalMagnitude = arg;
            emit proprotionalMagnitudeChanged(arg);
        }
    }

    void setMagnitudeVariation(qreal arg)
    {
        if (m_magnitudeVariation != arg) {
            m_magnitudeVariation = arg;
            emit magnitudeVariationChanged(arg);
        }
    }

private:
    qreal m_targetX;
    qreal m_targetY;
    qreal m_targetVariation;
    bool m_proportionalMagnitude;
    qreal m_magnitude;
    qreal m_magnitudeVariation;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // DIRECTEDVECTOR_H
