#include "zoneaffector.h"

ZoneAffector::ZoneAffector(QObject *parent) :
    ParticleAffector(parent), m_affector(0)
{
}

bool ZoneAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_affector)
        return false;
    //TODO: Better bounding than just x,y?
    ParticleVertex v = d->pv;
    if(v.x > m_x && v.x < m_x+m_width && v.y > m_y && v.y < m_y+m_width)
        return m_affector->affect(d, dt);
    return false;
}
