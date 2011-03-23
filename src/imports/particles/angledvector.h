#ifndef ANGLEDVECTOR_H
#define ANGLEDVECTOR_H
#include "varyingvector.h"
QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class AngledVector : public VaryingVector
{
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(qreal magnitude READ magnitude WRITE setMagnitude NOTIFY magnitudeChanged)
    Q_PROPERTY(qreal angleVariation READ angleVariation WRITE setAngleVariation NOTIFY angleVariationChanged)
    Q_PROPERTY(qreal magnitudeVariation READ magnitudeVariation WRITE setMagnitudeVariation NOTIFY magnitudeVariationChanged)
public:
    explicit AngledVector(QObject *parent = 0);
    const QPointF &sample(const QPointF &from);
    qreal angle() const
    {
        return m_angle;
    }

    qreal magnitude() const
    {
        return m_magnitude;
    }

    qreal angleVariation() const
    {
        return m_angleVariation;
    }

    qreal magnitudeVariation() const
    {
        return m_magnitudeVariation;
    }

signals:

    void angleChanged(qreal arg);

    void magnitudeChanged(qreal arg);

    void angleVariationChanged(qreal arg);

    void magnitudeVariationChanged(qreal arg);

public slots:
void setAngle(qreal arg)
{
    if (m_angle != arg) {
        m_angle = arg;
        emit angleChanged(arg);
    }
}

void setMagnitude(qreal arg)
{
    if (m_magnitude != arg) {
        m_magnitude = arg;
        emit magnitudeChanged(arg);
    }
}

void setAngleVariation(qreal arg)
{
    if (m_angleVariation != arg) {
        m_angleVariation = arg;
        emit angleVariationChanged(arg);
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
qreal m_angle;
qreal m_magnitude;
qreal m_angleVariation;
qreal m_magnitudeVariation;
};

QT_END_NAMESPACE
#endif // ANGLEDVECTOR_H
