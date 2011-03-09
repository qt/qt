#include "swarmaffector.h"
#include "particle.h"
#include <cmath>

SwarmAffector::SwarmAffector(QObject *parent) :
    ParticleAffector(parent), m_system(0), m_strength(0)
{
}

void SwarmAffector::ensureInit(ParticleData* d)
{
    if(m_system == d->p->m_system)
        return;
    m_system = d->p->m_system;
    m_lastPos.resize(m_system->count());
}

const qreal epsilon = 0.0000001;
bool SwarmAffector::affect(ParticleData *d, qreal dt)
{
    ensureInit(d);

    QPointF curPos(d->curX(), d->curY());
    if(m_leaders.isEmpty() || m_leaders.contains(d->p)){
        m_lastPos[d->systemIndex] = curPos;
        if(m_leaders.contains(d->p))
            return false;
    }

    qreal fx = 0.0;
    qreal fy = 0.0;
    for(int i=0; i < m_lastPos.count(); i++){
        if(m_lastPos[i].isNull())
            continue;
        QPointF diff = m_lastPos[i] - curPos;
        qreal r = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if(r == 0.0)
            continue;
        qreal f = m_strength * (1/r);
        if(f < epsilon)
            continue;
        qreal theta = atan2(diff.y(), diff.x());
        fx += cos(theta) * f;
        fy += sin(theta) * f;
    }
    if(!fx && !fy)
        return false;
    d->setInstantaneousSX(d->curSX()+fx * dt);
    d->setInstantaneousSY(d->curSY()+fy * dt);
    return true;
}

void SwarmAffector::reset(int systemIdx)
{
    if(!m_system)
        return;
    if(!m_lastPos[systemIdx].isNull())
        m_lastPos[systemIdx] = QPointF();
}
