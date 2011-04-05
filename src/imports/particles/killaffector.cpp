#include "killaffector.h"
#include "particleemitter.h"
QT_BEGIN_NAMESPACE
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
QT_END_NAMESPACE
