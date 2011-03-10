#include "particle.h"

ParticleType::ParticleType(QObject *parent) :
    QObject(parent)
{
}

void ParticleType::load(ParticleData*)
{
}

void ParticleType::reload(ParticleData*)
{
}

void ParticleType::setCount(int c)
{
    if(c == m_count)
        return;
    m_count = c;
    emit countChanged();
}

int ParticleType::count()
{
    return m_count;
}

Node* ParticleType::buildParticleNode()
{
    return 0;
}

void ParticleType::reset()
{
}

void ParticleType::prepareNextFrame(uint timeStamp)
{
}
