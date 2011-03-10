#include "particle.h"

ParticleType::ParticleType(QSGItem *parent) :
    QSGItem(parent)
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

