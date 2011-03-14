#include "swarmaffector.h"
#include "particle.h"
#include <cmath>
#include <QDebug>

SwarmAffector::SwarmAffector(QSGItem *parent) :
    ParticleAffector(parent), m_strength(1), m_inited(false)
{
    connect(this, SIGNAL(leadersChanged(QStringList)),
            this, SLOT(updateGroupList()));
}

void SwarmAffector::ensureInit()
{
    if(m_inited)
        return;
    m_inited = true;
    updateGroupList();
    m_lastPos.resize(m_system->count());
}

const qreal epsilon = 0.0000001;
bool SwarmAffector::affectParticle(ParticleData *d, qreal dt)
{
    ensureInit();
    QPointF curPos(d->curX(), d->curY());
    if(m_leaders.isEmpty() || m_leadGroups.contains(d->group)){
        m_lastPos[d->systemIndex] = curPos;
        if(m_leadGroups.contains(d->group))
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

void SwarmAffector::updateGroupList()
{
    if(!m_system || !m_system->m_initialized)
        return;
    m_leadGroups.clear();
    foreach(const QString &s, m_leaders)
        m_leadGroups << m_system->m_groupIds[s];
}
