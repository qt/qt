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
    QSGItem(parent), m_running(true)
{
    setFlag(ItemHasContents);
}

void emitterAppend(QDeclarativeListProperty<ParticleEmitter> *p, ParticleEmitter* pe)
{
    pe->m_system = qobject_cast<ParticleSystem*>(p->object);
    QObject::connect(pe, SIGNAL(particlesPerSecondChanged(int)),
            p->object, SLOT(countChanged()));
    QObject::connect(pe, SIGNAL(particleDurationChanged(int)),
            p->object, SLOT(countChanged()));
    p->object->metaObject()->invokeMethod(p->object, "countChanged");
    reinterpret_cast<QList<ParticleEmitter  *> *>(p->data)->append(pe);
}

ParticleEmitter* emitterAt(QDeclarativeListProperty<ParticleEmitter> *p, int idx)
{
    return reinterpret_cast<QList<ParticleEmitter  *> *>(p->data)->at(idx);
}

void emitterClear(QDeclarativeListProperty<ParticleEmitter> *p)
{
    reinterpret_cast<QList<ParticleEmitter  *> *>(p->data)->clear();
}

int emitterCount(QDeclarativeListProperty<ParticleEmitter> *p)
{
    return reinterpret_cast<QList<ParticleEmitter *> *>(p->data)->count();
}

QDeclarativeListProperty<ParticleEmitter> ParticleSystem::emitters()
{
    return QDeclarativeListProperty<ParticleEmitter>(this, &m_emitters, emitterAppend, emitterCount, emitterAt, emitterClear);
}

void ParticleSystem::registerEmitter(ParticleEmitter *emitter)
{
    QDeclarativeListProperty<ParticleEmitter> tmp(this, &m_emitters, emitterAppend, emitterCount, emitterAt, emitterClear);
    emitterAppend(&tmp, emitter);
}

void ParticleSystem::countChanged()
{
    reset();//Need to give Particles new Count
}

void ParticleSystem::buildParticleNodes()
{
    //TODO: Staggered loading (as emitted)
    if(m_node)
        delete m_node;
    m_node = 0;

    //TODO: update m_last_particle?
    m_particle_count = 0;//TODO: Only when changed?
    qDeleteAll(m_emitterData);
    m_emitterData.clear();

    if(!m_emitters.count() || !m_particles.count())
        return;

    m_node = new Node;
    foreach(ParticleEmitter* e, m_emitters){
        if(!e->particle())
            e->setParticle(m_particles[0]);
        m_emitterData << new EmitterData;
        m_emitterData.last()->size = ceil(e->particlesPerSecond()*(e->particleDuration()/1000.0));
        m_emitterData.last()->start = m_particle_count;
        m_emitterData.last()->nextIdx = 0;
        m_particle_count += m_emitterData.last()->size;
    }
    data.resize(m_particle_count);

    foreach(Particle* particle, m_particles){
        int particleCount = 0;
        for(int i=0; i<m_emitters.count(); i++){
            if(m_emitters[i]->particle() == particle){
                m_emitterData[i]->particleOffsets.insert(particle, particleCount);
                particleCount += m_emitterData[i]->size;
            }
        }
        particle->setCount(particleCount);
        Node* child = particle->buildParticleNode();
        if(child)
            m_node->appendChildNode(child);
        else
            qDebug() << "Couldn't build" << particle;
    }

    m_timestamp.start();
}

void ParticleSystem::reset()
{
    m_do_reset = true;
}

Node *ParticleSystem::updatePaintNode(Node *, UpdatePaintNodeData *)
{
    if(m_do_reset){
        foreach(Particle* particle, m_particles)
            particle->reset();
        delete m_node;
        m_node = 0;
        m_do_reset = false;
    }

    prepareNextFrame();

    if(m_running){
        update();
        if (m_node)
            m_node->markDirty(Node::DirtyMaterial);//### Will this propagate to Particle Nodes?
    }

    return m_node;
}

ParticleData* ParticleSystem::newDatum(ParticleEmitter* e, Particle* p)
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

void ParticleSystem::prepareNextFrame()
{
    if (!m_running)
        return;

    if (m_node == 0)
        buildParticleNodes();

    if (m_node == 0) //error creating nodes
        return;

    //### Elapsed time never shrinks - may cause problems if left emitting for weeks at a time.
    uint timeInt = m_timestamp.elapsed();
    qreal time =  timeInt / 1000.;
    foreach(ParticleEmitter* emitter, m_emitters)
        emitter->emitWindow(timeInt);
    if(m_affectors.count()){//Optimize the common no-affectors case
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
    }
    foreach(Particle* particle, m_particles)
        particle->prepareNextFrame(timeInt);
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
