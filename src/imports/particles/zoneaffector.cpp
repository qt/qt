#include "zoneaffector.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

ZoneAffector::ZoneAffector(QObject *parent) :
    ParticleAffector(parent), m_x(0), m_y(0), m_width(0), m_height(0), m_affector(0)
{
}

bool ZoneAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_affector)
        return false;
    qreal x = d->curX();
    qreal y = d->curY();
    if(x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height)
        return m_affector->affect(d, dt);
    return false;
}

void ZoneAffector::reset(int systemIdx)
{
    if(m_affector)
        m_affector->reset(systemIdx);
}
QT_END_NAMESPACE
