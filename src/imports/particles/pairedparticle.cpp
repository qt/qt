#include "pairedparticle.h"
#include <QDebug>

PairedParticle::PairedParticle(QObject *parent) :
    ParticleType(parent), m_count(0), m_particle(0)
{
    //TODO: null pointer safety with m_particle
    //TODO: Still seems sensitive to creation order.
    //TODO: start of sim seems to happen before others reset, and so then they lose particles.
}

void PairedParticle::componentComplete()
{
    //TODO: re calculate when appropriate
    //### Will recalculation help for a moving particle system? Do we need to fake reloads?
    if(m_pairs.isEmpty())
        return;
    foreach(PairedParticle* pair, m_pairs)
        m_differences << m_system->mapToScene(QPointF(0,0)) - pair->m_system->mapToScene(QPointF(0,0));
}

ParticleData* PairedParticle::translate(ParticleData* d, const QPointF &difference, bool forwards)
{
    if(difference.isNull())
        return d;
    d->pv.x += difference.x() * (forwards?1:-1);
    d->pv.y += difference.y() * (forwards?1:-1);
    return d;
}

PairedParticle::~PairedParticle()
{
    if(m_particle)
        delete m_particle;//### Is it wholly owned by this?
}

void PairedParticle::load(ParticleData* d)
{
    m_particle->load(d);
    int i = 0;
    foreach(PairedParticle* pair, m_pairs){
        pair->m_particle->load(translate(d, m_differences[i]));
        translate(d, m_differences[i], false);//faster than allocating and deallocating the whole thing
        i++;
    }
}

void PairedParticle::reload(ParticleData* d)
{

    m_particle->reload(d);
    int i = 0;
    foreach(PairedParticle* pair, m_pairs){
        pair->m_particle->reload(translate(d, m_differences[i]));
        translate(d, m_differences[i], false);//faster than allocating and deallocating the whole thing
        i++;
    }
}

void PairedParticle::setCount(int c)
{
    if(m_count == c)
        return;
    m_count = c;
    //TODO: Track wantsReset properly
    if(!m_pairs.isEmpty()){
        m_particle->setCount(c);
        m_pleaseReset = true;
        foreach(PairedParticle* pair, m_pairs){
            pair->m_particle->setCount(c);
            pair->m_pleaseReset = true;
        }
    }else{
        m_particle->setCount(qMax(c,1));
        m_pleaseReset = true;
    }
}

Node* PairedParticle::buildParticleNode()
{
    return m_particle->buildParticleNode();
}

void PairedParticle::reset()
{
    m_particle->reset();
}

void PairedParticle::prepareNextFrame(uint timeStamp)
{
    if(!m_pairs.isEmpty()){//Only pair master calls this, for both
        m_particle->prepareNextFrame(timeStamp);
        foreach(PairedParticle* pair, m_pairs)
            pair->m_particle->prepareNextFrame(timeStamp);
    }
    //###Sprite states will not sync
}
