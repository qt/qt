#include "toggleaffector.h"

ToggleAffector::ToggleAffector(QObject *parent) :
    ParticleAffector(parent)
{
}

bool ToggleAffector::affect(ParticleData *d, qreal dt)
{
    if(m_affecting)
        return m_affector->affect(d, dt);
    else
        return false;
}
