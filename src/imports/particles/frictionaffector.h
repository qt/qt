#ifndef FRICTIONAFFECTOR_H
#define FRICTIONAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


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

QT_END_NAMESPACE
#endif // FRICTIONAFFECTOR_H
