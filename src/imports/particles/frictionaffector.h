#ifndef FRICTIONAFFECTOR_H
#define FRICTIONAFFECTOR_H
#include "particleaffector.h"

class FrictionAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal factor READ factor WRITE setFactor NOTIFY factorChanged)
public:
    explicit FrictionAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);

    qreal factor() const
    {
        return m_factor;
    }

signals:

    void factorChanged(qreal arg);

public slots:

void setFactor(qreal arg)
{
    if (m_factor != arg) {
        m_factor = arg;
        emit factorChanged(arg);
    }
}

private:
qreal m_factor;
};

#endif // FRICTIONAFFECTOR_H
