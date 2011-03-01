#ifndef GRAVITYAFFECTOR_H
#define GRAVITYAFFECTOR_H
#include "particleaffector.h"

class GravityAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
public:
    explicit GravityAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    qreal acceleration() const
    {
        return m_acceleration;
    }

    qreal angle() const
    {
        return m_angle;
    }

signals:

    void accelerationChanged(qreal arg);

    void angleChanged(qreal arg);

public slots:
void setAcceleration(qreal arg)
{
    if (m_acceleration != arg) {
        m_acceleration = arg;
        emit accelerationChanged(arg);
    }
}

void setAngle(qreal arg)
{
    if (m_angle != arg) {
        m_angle = arg;
        emit angleChanged(arg);
    }
}

private slots:
    void recalc();
private:
    qreal m_acceleration;
    qreal m_angle;

    qreal m_xAcc;
    qreal m_yAcc;
};

#endif // GRAVITYAFFECTOR_H
