#include "particleaffector.h"

ParticleAffector::ParticleAffector(QObject *parent) :
    QObject(parent)
{
}

void ParticleAffector::affect(ParticleVertices *p, int idx, qreal dt)
{
}
