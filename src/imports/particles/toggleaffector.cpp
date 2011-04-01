#include "toggleaffector.h"

QT_BEGIN_NAMESPACE

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

QT_END_NAMESPACE
