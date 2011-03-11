#include "killaffector.h"
#include "particleemitter.h"

KillAffector::KillAffector(QSGItem *parent) :
    ParticleAffector(parent)
{
}


bool KillAffector::affectParticle(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    d->pv.t += d->pv.lifeSpan;
    return true;
}
