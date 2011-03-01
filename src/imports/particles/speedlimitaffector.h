#ifndef SPEEDLIMITAFFECTOR_H
#define SPEEDLIMITAFFECTOR_H
#include "particleaffector.h"

class SpeedLimitAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal speedLimit READ speedLimit WRITE setSpeedLimit NOTIFY speedLimitChanged)


public:
    explicit SpeedLimitAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);

    qreal speedLimit() const
    {
        return m_speedLimit;
    }

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

#endif // SPEEDLIMITAFFECTOR_H
