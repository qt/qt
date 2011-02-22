#include "particleaffector.h"

ParticleAffector::ParticleAffector(QObject *parent) :
    QObject(parent)
{
}

void ParticleAffector::affect(ParticleVertices *p, int idx, qreal dt, SpriteParticles *sp)
{
    Q_UNUSED(p);
    Q_UNUSED(idx);
    Q_UNUSED(dt);
    Q_UNUSED(sp);
}

void ParticleAffector::reset(int idx)
{
    Q_UNUSED(idx);
}
