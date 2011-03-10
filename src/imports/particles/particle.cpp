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
    if(c == m_count)
        return;
    m_count = c;
    emit countChanged();
}

int Particle::count()
{
    return m_count;
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
