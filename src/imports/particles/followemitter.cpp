#include "followemitter.h"
#include "particle.h"
#include <cmath>

FollowEmitter::FollowEmitter(QSGItem *parent) :
    ParticleEmitter(parent)
  , m_particleSize(16)
  , m_particleEndSize(-1)
  , m_particleSizeVariation(0)
  , m_xSpeed(0)
  , m_ySpeed(0)
  , m_xSpeedVariation(0)
  , m_ySpeedVariation(0)
  , m_xAccel(0)
  , m_yAccel(0)
  , m_xAccelVariation(0)
  , m_yAccelVariation(0)
  , m_lastTimeStamp(0)
{
    connect(this, SIGNAL(followChanged(QString)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particleDurationChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particlesPerParticlePerSecondChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
}

void FollowEmitter::recalcParticlesPerSecond(){
    if(!m_system)
        return;
    m_followCount = m_system->m_groupData[m_system->m_groupIds[m_follow]]->size;
    if(!m_followCount){
        setParticlesPerSecond(1000);//XXX: Fix this horrendous hack, needed so they aren't turned off from start
    }else{
        setParticlesPerSecond(m_particlesPerParticlePerSecond * m_followCount);
        m_lastEmission.resize(m_followCount);
    }
}

void FollowEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if(!m_emitting)
        return;
    if(m_followCount != m_system->m_groupData[m_system->m_groupIds[m_follow]]->size){
        recalcParticlesPerSecond();
        return;//system may need to update
    }
    qreal time = timeStamp / 1000.;
    qreal particleRatio = 1. / m_particlesPerParticlePerSecond;
    qreal pt;

    //Have to map it into this system, because particlesystem automaps it back
    QPointF offset = this->mapFromItem(m_system, QPointF(0, 0));
    float sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;

    int gId = m_system->m_groupIds[m_follow];
    int gId2 = m_system->m_groupIds[m_particle];
    for(int i=0; i<m_system->m_groupData[gId]->size; i++){
        pt = m_lastEmission[i];
        ParticleData* d = m_system->data[i + m_system->m_groupData[gId]->start];
        if(!d)
            continue;
        if(pt < d->pv.t)
            pt = d->pv.t;
        if(!QRect(offset.x(), offset.y(), width(), height()).contains(d->curX(), d->curY())){
            m_lastEmission[i] = time;//jump over this time period without emitting, because it's outside
            continue;
        }
        while(pt < time){
            ParticleData* datum = m_system->newDatum(gId2);
            datum->e = this;//###useful?
            ParticleVertex &p = datum->pv;

            // Particle timestamp
            p.t = pt;
            p.lifeSpan =
                    (m_particleDuration
                     + ((rand() % ((m_particleDurationVariation*2) + 1)) - m_particleDurationVariation))
                    / 1000.0;

            // Particle position
            qreal followT =  pt - d->pv.t;
            qreal followT2 = followT * followT * 0.5;
            qreal halfCurSize = d->pv.size / 2;//TODO: Actually cur size
            p.x = d->pv.x + d->pv.sx * followT + d->pv.ax * followT2
                    + offset.x()
                    - halfCurSize + rand() / float(RAND_MAX) * halfCurSize * 2;
                    //- m_emitterXVariation + rand() / float(RAND_MAX) * m_emitterXVariation * 2;
            p.y = d->pv.y + d->pv.sy * followT + d->pv.ay * followT2
                    + offset.y()
                    - halfCurSize + rand() / float(RAND_MAX) * halfCurSize * 2;
                    //- m_emitterYVariation + rand() / float(RAND_MAX) * m_emitterYVariation * 2;

            // Particle speed
            p.sx =
                    m_xSpeed
                    - m_xSpeedVariation + rand() / float(RAND_MAX) * m_xSpeedVariation * 2;
            p.sy =
                    m_ySpeed
                    - m_ySpeedVariation + rand() / float(RAND_MAX) * m_ySpeedVariation * 2;

            // Particle acceleration
            p.ax =
                    m_xAccel - m_xAccelVariation + rand() / float(RAND_MAX) * m_xAccelVariation * 2;
            p.ay =
                    m_yAccel - m_yAccelVariation + rand() / float(RAND_MAX) * m_yAccelVariation * 2;

            // Particle size
            float sizeVariation = -m_particleSizeVariation
                    + rand() / float(RAND_MAX) * m_particleSizeVariation * 2;

            float size = m_particleSize + sizeVariation;
            float endSize = sizeAtEnd + sizeVariation;

            p.size = size * float(m_emitting);
            p.endSize = endSize * float(m_emitting);

            pt += particleRatio;
            m_system->emitParticle(datum);
        }
        m_lastEmission[i] = pt;
    }

    m_lastTimeStamp = time;
}
