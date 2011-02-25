#include "particle.h"

Particle::Particle(QObject *parent) :
    QObject(parent)
{
}

void Particle::load(ParticleData*)
{
}

void Particle::reload(ParticleData*)
{
}

void Particle::setCount(int c)
{
}

Node* Particle::buildParticleNode()
{
    return 0;
}

void Particle::reset()
{
}

void Particle::prepareNextFrame(uint timeStamp)
{
}
