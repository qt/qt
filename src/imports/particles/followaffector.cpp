#include "followaffector.h"
#include "followemitter.h"

FollowAffector::FollowAffector(QObject *parent) :
    ParticleAffector(parent), m_emitter(0)
{
}

bool FollowAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_emitter)
        return false;
    Q_UNUSED(dt);
    if(d->p == m_emitter->m_follow && !m_emitter->m_pending.contains(d)){
        m_emitter->m_pending << d;
        m_emitter->m_lastEmission[d->particleIndex] = d->pv.dt;
    }
    return false;
}

void FollowAffector::reset(int systemIdx){
    if(!m_emitter)
        return;
    QSet<ParticleData *> removals;
    foreach(ParticleData* d, m_emitter->m_pending)
        if(d->systemIndex == systemIdx)
            removals << d;

    foreach(ParticleData* d, removals)
        m_emitter->m_pending.remove(d);
}
