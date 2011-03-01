#include "killaffector.h"
#include "particleemitter.h"

KillAffector::KillAffector(QObject *parent) :
    ParticleAffector(parent)
{
}


bool KillAffector::affect(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    d->pv.t += d->e->particleDuration();//particleDuration is in msec, t is in sec, but I'm just making double sure
    return true;
}
