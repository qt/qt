#ifndef FRICTIONAFFECTOR_H
#define FRICTIONAFFECTOR_H
#include "particleaffector.h"

class FrictionAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal factor READ factor WRITE setFactor NOTIFY factorChanged)
public:
    explicit FrictionAffector(QSGItem *parent = 0);

    qreal factor() const
    {
        return m_factor;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
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
