#include "frictionaffector.h"

FrictionAffector::FrictionAffector(QObject *parent) :
    ParticleAffector(parent), m_factor(0.0)
{
}

bool FrictionAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_factor)
        return false;
    d->setInstantaneousAX(d->pv.ax + (d->curSX() * m_factor * -1 * dt));
    d->setInstantaneousAY(d->pv.ay + (d->curSY() * m_factor * -1 * dt));
}
