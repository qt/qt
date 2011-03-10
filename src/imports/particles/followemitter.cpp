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
  , m_follow(0)
  , m_lastTimeStamp(0)
{
    connect(this, SIGNAL(followChanged(ParticleType*)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particleDurationChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particlesPerParticlePerSecondChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
}

void FollowEmitter::setFollow(ParticleType *arg)
{
    if (m_follow != arg) {
        m_follow = arg;
        emit followChanged(arg);
        connect(m_follow, SIGNAL(countChanged()),
                this, SLOT(recalcParticlesPerSecond()));
    }
}

void FollowEmitter::recalcParticlesPerSecond(){
    if(!m_follow){
        return;
    }else if(!m_follow->count()){//It hasn't been told how many it's emitting yet
        setParticlesPerSecond(1000);//TODO: Fix this horrendous hack
    }else{
        setParticlesPerSecond(m_particlesPerParticlePerSecond * m_follow->count());
        m_lastEmission.resize(m_follow->count());
    }
}

void FollowEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0 || m_follow == 0)
        return;
    if(!m_emitting)
        return;

    qreal time = timeStamp / 1000.;
    qreal particleRatio = 1. / m_particlesPerParticlePerSecond;
    qreal pt;

    QPointF offset = this->mapFromItem(m_system, QPointF(0, 0));
    //Have to map it into this system, because particlesystem automaps it back
    float sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;

    foreach(ParticleData* d, m_pending){
        if(d->p != m_follow){//WTF?
            m_pending.remove(d);
            continue;
        }
        pt = m_lastEmission[d->particleIndex];
        while(pt < time){
            ParticleData* datum = m_system->newDatum(this, m_particle);
            ParticleVertex &p = datum->pv;

            // Particle timestamp
            p.t = p.dt = pt;

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
        m_lastEmission[d->particleIndex] = pt;
    }

    m_lastTimeStamp = time;
}
