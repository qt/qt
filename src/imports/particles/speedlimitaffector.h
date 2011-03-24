#ifndef SPEEDLIMITAFFECTOR_H
#define SPEEDLIMITAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class SpeedLimitAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal speedLimit READ speedLimit WRITE setSpeedLimit NOTIFY speedLimitChanged)


public:
    explicit SpeedLimitAffector(QSGItem *parent = 0);

    qreal speedLimit() const
    {
        return m_speedLimit;
    }

protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void speedLimitChanged(qreal arg);

public slots:
void setSpeedLimit(qreal arg)
{
    if (m_speedLimit != arg) {
        m_speedLimit = arg;
        emit speedLimitChanged(arg);
    }
}

private:
qreal m_speedLimit;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // SPEEDLIMITAFFECTOR_H
