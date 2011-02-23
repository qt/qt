#include "particlesystem.h"
#include <node.h>
#include "particleemitter.h"
#include "particleaffector.h"

ParticleSystem::ParticleSystem(QSGItem *parent) :
    QSGItem(parent), m_running(true)
{
    setFlag(ItemHasContents);
}

void emitterAppend(QDeclarativeListProperty<ParticleEmitter> *p, ParticleEmitter* pe)
{
    pe->m_system = qobject_cast<ParticleSystem*>(p->object);
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

void ParticleSystem::buildParticleNodes()
{
    m_node = new Node;
    foreach(ParticleEmitter* emitter, m_emitters)
        m_node->appendChildNode(emitter->buildParticleNode());

    m_timestamp.start();
}

void ParticleSystem::reset()
{
    m_do_reset = true;
}

Node *ParticleSystem::updatePaintNode(Node *, UpdatePaintNodeData *)
{
    if(m_do_reset){
        foreach(ParticleEmitter* emitter, m_emitters)
            emitter->reset();
        delete m_node;
        m_node = 0;
        m_do_reset = false;
    }

    prepareNextFrame();

    if(m_running){
        update();
        if (m_node)
            m_node->markDirty(Node::DirtyMaterial);
    }

    return m_node;
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
    foreach(ParticleEmitter* emitter, m_emitters){
        emitter->prepareNextFrame(timeInt);
        ParticleVertices* particles = emitter->particles();
        for(uint i=0; i < emitter->particleCount(); i++){
            ParticleVertices* p = &particles[i];
            qreal dt = time - p->v1.dt;
            p->v1.dt = p->v2.dt = p->v3.dt = p->v4.dt = time;
            foreach(ParticleAffector* a, m_affectors){
                a->affect(p, i, dt, qobject_cast<QObject*>(emitter));//TODO: fixup the damn interface
            }
        }
    }

}
