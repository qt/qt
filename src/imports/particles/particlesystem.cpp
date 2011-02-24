#include "particlesystem.h"
#include <node.h>
#include "particleemitter.h"
#include "particleaffector.h"
#include "particle.h"

ParticleData::ParticleData()
    : p(0)
    , e(0)
    , emitterIndex(0)
    , systemIndex(0)
{
    //### A lot of this initialization was skipped before(tex) as it was constant
    ParticleVertex *vertices = (ParticleVertex *) &pv;
    for (int i=0; i<4; ++i) {
        vertices[i].x = 0;
        vertices[i].y = 0;
        vertices[i].t = -1;
        vertices[i].size = 0;
        vertices[i].endSize = 0;
        vertices[i].sx = 0;
        vertices[i].sy = 0;
        vertices[i].ax = 0;
        vertices[i].ay = 0;
        vertices[i].animIdx = 0;
        vertices[i].frameDuration = 1;
        vertices[i].frameCount = 1;
        vertices[i].animT = -1;
    }

    vertices[0].tx = 0;
    vertices[0].ty = 0;

    vertices[1].tx = 1;
    vertices[1].ty = 0;

    vertices[2].tx = 0;
    vertices[2].ty = 1;

    vertices[3].tx = 1;
    vertices[3].ty = 1;
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

void ParticleSystem::countChanged()
{
    reset();//Need to give Particles new Count
}

void ParticleSystem::buildParticleNodes()
{
    //TODO: Staggered loading (as emitted)
    m_node = new Node;

    //TODO: update m_last_particle?
    m_particle_count = 0;//TODO: Only when changed?
    foreach(ParticleEmitter* e, m_emitters)
        m_particle_count += e->particlesPerSecond()*(e->particleDuration()/1000.0);
    d.resize(m_particle_count);

    foreach(Particle* particle, m_particles){
        particle->setCount(m_particle_count);//TODO: only their count
        Node* child = particle->buildParticleNode();
        if(child)
            m_node->appendChildNode(child);
        else
            qDebug() << "Couldn't build" << particle;
    }

    m_next_particle = 0;
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

ParticleData* ParticleSystem::newDatum()
{
    //TODO: Keep datums within one emitter? And within one particle.
    //TODO: Switch to d->emitterIdx + eIdx*maxSize?
    ParticleData* ret;
    if(d[m_next_particle]){
        ret = d[m_next_particle];
    }else{
        ret = new ParticleData;
        d[m_next_particle] = ret;
    }
    ret->systemIndex = m_next_particle;
    m_next_particle++;
    if(m_next_particle >= m_particle_count)
        m_next_particle = 0;
    return ret;
}

void ParticleSystem::emitParticle(ParticleData* pd)
{// called from prepareNextFrame()->emitWindow - enforce?
    if(!pd->p){
        if(m_particles.isEmpty())
            return;
        pd->p = m_particles.first();
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
        for(QVector<ParticleData*>::iterator iter=d.begin(); iter != d.end(); iter++){
            if(!(*iter))
                continue;
            ParticleVertices* p = &((*iter)->pv);
            qreal dt = time - p->v1.dt;
            p->v1.dt = p->v2.dt = p->v3.dt = p->v4.dt = time;
            foreach(ParticleAffector* a, m_affectors)
                if (a->affect(*iter, dt))
                    (*iter)->p->reload(*iter);
        }
    }
    foreach(Particle* particle, m_particles)
        particle->prepareNextFrame(timeInt);
}
