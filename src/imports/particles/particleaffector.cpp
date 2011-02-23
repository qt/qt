#include "particleaffector.h"
#include "particlesystem.h"
#include "spriteemitter.h"

ParticleAffector::ParticleAffector(QObject *parent) :
    QObject(parent)
{
}

void ParticleAffector::affect(ParticleVertices *p, int idx, qreal dt, QObject* emitter)
{
    Q_UNUSED(p);
    Q_UNUSED(idx);
    Q_UNUSED(dt);
    Q_UNUSED(emitter);
}

void ParticleAffector::reset(int idx)
{
    Q_UNUSED(idx);
}
