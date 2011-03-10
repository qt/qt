#include "particlesystem.h"
#include <node.h>
#include "particleemitter.h"
#include "particleaffector.h"
#include "particle.h"
#include <cmath>

ParticleData::ParticleData()
    : p(0)
    , e(0)
    , particleIndex(0)
    , systemIndex(0)
{
    pv.x = 0;
    pv.y = 0;
    pv.t = -1;
    pv.size = 0;
    pv.endSize = 0;
    pv.sx = 0;
    pv.sy = 0;
    pv.ax = 0;
    pv.ay = 0;
}

ParticleSystem::ParticleSystem(QSGItem *parent) :
    QSGItem(parent), m_running(true) , m_startTime(0)
{
}

void ParticleSystem::registerParticleType(ParticleType* p)
{
    m_particles << p;//###Set or uniqueness checking?
    reset();
}

void ParticleSystem::registerParticleEmitter(ParticleEmitter* e)
{
    m_emitters << e;//###How to get them out?
    reset();
}

void ParticleSystem::registerParticleAffector(ParticleAffector* a)
{
    m_affectors << a;
    reset();//TODO: Slim down the huge batch of resets at the start
}

void ParticleSystem::countChanged()
{
    reset();//Need to give Particles new Count
}

void ParticleSystem::setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        emit runningChanged(arg);
        reset();
    }
}

void ParticleSystem::componentComplete()
{
    QSGItem::componentComplete();
    reset();
}

void ParticleSystem::initializeSystem()
{
    m_particle_count = 0;//TODO: Only when changed?
    qDeleteAll(m_emitterData);
    m_emitterData.clear();
    if(!m_emitters.count() || !m_particles.count())
        return;

    foreach(ParticleEmitter* e, m_emitters){
        if(!e->particle())
            e->setParticle(m_particles[0]);
        m_emitterData << new EmitterData;
        m_emitterData.last()->size = ceil(e->particlesPerSecond()*(e->particleDuration()/1000.0));
        m_emitterData.last()->start = m_particle_count;
        m_emitterData.last()->nextIdx = 0;
        m_particle_count += m_emitterData.last()->size;
    }//TODO: Particle based, not emitter based, blocks?
    data.resize(m_particle_count);
    if(m_particle_count > 16000)
        qWarning() << "Particle system contains a vast number of particles (>16000). Expect poor performance";

    foreach(ParticleType* particle, m_particles){
        int particleCount = 0;
        for(int i=0; i<m_emitters.count(); i++){
            if(m_emitters[i]->particle() == particle){
                m_emitterData[i]->particleOffsets.insert(particle, particleCount);
                particleCount += m_emitterData[i]->size;
            }
        }
        particle->setCount(particleCount);
        particle->m_pleaseReset = true;
    }

    m_timestamp.start();
    m_initialized = true;
}

void ParticleSystem::reset()
{
    if(!m_running)
        return;
    initializeSystem();
    foreach(ParticleType* p, m_particles)
        p->update();
}

ParticleData* ParticleSystem::newDatum(ParticleEmitter* e, ParticleType* p)
{
    //TODO: Keep datums within one emitter? And within one particle.
    ParticleData* ret;
    int eIdx = m_emitters.indexOf(e);
    int nextIdx = m_emitterData[eIdx]->start + m_emitterData[eIdx]->nextIdx++;
    if( m_emitterData[eIdx]->nextIdx >= m_emitterData[eIdx]->size)
        m_emitterData[eIdx]->nextIdx = 0;

    if(data[nextIdx]){//Recycle, it's faster. //###Reset?
        ret = data[nextIdx];
    }else{
        ret = new ParticleData;
        data[nextIdx] = ret;
    }

    ret->systemIndex = nextIdx;
    ret->particleIndex = nextIdx - m_emitterData[eIdx]->start + m_emitterData[eIdx]->particleOffsets[p];
    ret->e = e;
    ret->p = p;
    return ret;
}
//TODO: Merge with newDatum
void ParticleSystem::emitParticle(ParticleData* pd)
{// called from prepareNextFrame()->emitWindow - enforce?
    if(!pd->p){
        if(m_particles.isEmpty())
            return;
        pd->p = m_particles.first();
    }

    //Account for relative emitter position
    QPointF offset = this->mapFromItem(pd->e, QPointF(0, 0));
    if(offset != QPointF(0,0)){
        pd->pv.x += offset.x();
        pd->pv.y += offset.y();
    }

    foreach(ParticleAffector *a, m_affectors)
        a->reset(pd->systemIndex);
    pd->p->load(pd);
}



uint ParticleSystem::systemSync(ParticleType* p)
{
    if (!m_running)
        return 0;
    if (!m_initialized)
        return 0;//error in initialization

    if(m_syncList.isEmpty() || m_syncList.contains(p)){//Need to advance the simulation
        m_syncList.clear();

        //### Elapsed time never shrinks - may cause problems if left emitting for weeks at a time.
        m_timeInt = m_timestamp.elapsed() + m_startTime;
        qreal time =  m_timeInt / 1000.;
        foreach(ParticleEmitter* emitter, m_emitters)
            emitter->emitWindow(m_timeInt);
        if(m_affectors.count()){//Optimizes the common no-affectors case//TODO: Forced per-particle is restrictive. Give it all
            for(QVector<ParticleData*>::iterator iter=data.begin(); iter != data.end(); iter++){
                if(!(*iter))
                    continue;
                ParticleVertex* p = &((*iter)->pv);
                qreal dt = time - p->dt;
                p->dt = time;
                bool modified = false;
                foreach(ParticleAffector* a, m_affectors){
                    if (a->affect(*iter, dt)){
                        modified = true;
                    }
                }
                if(modified)
                    (*iter)->p->reload(*iter);
            }
        }//TODO:Move particle positions along with system element moves?
    }
    m_syncList << p;
    return m_timeInt;
}

//sets the x accleration without affecting the instantaneous x velocity or position
void ParticleData::setInstantaneousAX(qreal ax)
{
    qreal t = pv.dt - pv.t;
    qreal sx = (pv.sx + t*pv.ax) - t*ax;
    qreal ex = pv.x + pv.sx * t + 0.5 * pv.ax * t * t;
    qreal x = ex - t*sx - 0.5 * t*t*ax;

    pv.ax = ax;
    pv.sx = sx;
    pv.x = x;
}

//sets the x velocity without affecting the instantaneous x postion
void ParticleData::setInstantaneousSX(qreal vx)
{
    qreal t = pv.dt - pv.t;
    qreal sx = vx - t*pv.ax;
    qreal ex = pv.x + pv.sx * t + 0.5 * pv.ax * t * t;
    qreal x = ex - t*sx - 0.5 * t*t*pv.ax;

    pv.sx = sx;
    pv.x = x;
}

//sets the instantaneous x postion
void ParticleData::setInstantaneousX(qreal x)
{
    qreal t = pv.dt - pv.t;
    pv.x = x - t*pv.sx - 0.5 * t*t*pv.ax;
}

//sets the y accleration without affecting the instantaneous y velocity or position
void ParticleData::setInstantaneousAY(qreal ay)
{
    qreal t = pv.dt - pv.t;
    qreal sy = (pv.sy + t*pv.ay) - t*ay;
    qreal ey = pv.y + pv.sy * t + 0.5 * pv.ay * t * t;
    qreal y = ey - t*sy - 0.5 * t*t*ay;

    pv.ay = ay;
    pv.sy = sy;
    pv.y = y;
}

//sets the y velocity without affecting the instantaneous y position
void ParticleData::setInstantaneousSY(qreal vy)
{
    qreal t = pv.dt - pv.t;
    qreal sy = vy - t*pv.ay;
    qreal ey = pv.y + pv.sy * t + 0.5 * pv.ay * t * t;
    qreal y = ey - t*sy - 0.5 * t*t*pv.ay;

    pv.sy = sy;
    pv.y = y;
}

//sets the instantaneous Y position
void ParticleData::setInstantaneousY(qreal y)
{
    qreal t = pv.dt - pv.t;
    pv.y = y - t*pv.sy - 0.5 * t*t*pv.ay;
}

qreal ParticleData::curX() const
{
    qreal t = pv.dt - pv.t;
    return pv.x + pv.sx * t + 0.5 * pv.ax * t * t;
}

qreal ParticleData::curSX() const
{
    qreal t = pv.dt - pv.t;
    return pv.sx + t*pv.ax;
}

qreal ParticleData::curY() const
{
    qreal t = pv.dt - pv.t;
    return pv.y + pv.sy * t + 0.5 * pv.ay * t * t;
}

qreal ParticleData::curSY() const
{
    qreal t = pv.dt - pv.t;
    return pv.sy + t*pv.ay;
}
