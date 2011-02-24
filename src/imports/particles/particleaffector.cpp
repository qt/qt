#include "particleaffector.h"

ParticleAffector::ParticleAffector(QObject *parent) :
    QObject(parent)
{
}

bool ParticleAffector::affect(ParticleData *, qreal)
{
    return false;
}

void ParticleAffector::reset(int idx)
{
    Q_UNUSED(idx);
}
