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
    if(d->p == m_emitter->m_follow)
        m_emitter->m_pending << d;
    return false;
}

void FollowAffector::reset(int systemIdx){
    if(!m_emitter)
        return;
    foreach(ParticleData* d, m_emitter->m_pending)
        if(d->systemIndex == systemIdx)
            m_emitter->m_pending.remove(d);//safe to do in the loop?
}
