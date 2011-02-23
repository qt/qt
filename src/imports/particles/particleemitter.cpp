#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(QObject *parent) :
    QObject(parent)
{
}

Node* ParticleEmitter::buildParticleNode()
{
    return 0;
}

void ParticleEmitter::prepareNextFrame(uint timestamp)
{
    Q_UNUSED(timestamp);
}

void ParticleEmitter::reset()
{

}

uint ParticleEmitter::particleCount()
{
    return 0;
}

ParticleVertices* ParticleEmitter::particles()
{
    return 0;
}
