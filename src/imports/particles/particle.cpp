#include "particle.h"
#include <QDebug>

ParticleType::ParticleType(QSGItem *parent) :
    QSGItem(parent),
    m_system(0)
{
    connect(this, SIGNAL(xChanged()),
            this, SLOT(calcSystemOffset()));
    connect(this, SIGNAL(yChanged()),
            this, SLOT(calcSystemOffset()));
}

void ParticleType::setSystem(ParticleSystem *arg)
{
    if (m_system != arg) {
        m_system = arg;
        if(m_system){
            m_system->registerParticleType(this);
            connect(m_system, SIGNAL(xChanged()),
                    this, SLOT(calcSystemOffset()));
            connect(m_system, SIGNAL(yChanged()),
                    this, SLOT(calcSystemOffset()));
            calcSystemOffset();
        }
        emit systemChanged(arg);
    }
}

void ParticleType::load(ParticleData*)
{
}

void ParticleType::reload(ParticleData*)
{
}

void ParticleType::setCount(int c)
{
    if(c == m_count)
        return;
    m_count = c;
    emit countChanged();
}

int ParticleType::count()
{
    return m_count;
}


int ParticleType::particleTypeIndex(ParticleData* d)
{
    if(!m_particleStarts.contains(d->group)){
        m_particleStarts.insert(d->group, m_lastStart);
        m_lastStart += m_system->m_groupData[d->group]->size;
    }
    return m_particleStarts[d->group] + d->particleIndex;
}


void ParticleType::calcSystemOffset()
{
    if(!m_system)
        return;
    QPointF lastOffset = m_systemOffset;
    m_systemOffset = this->mapFromItem(m_system, QPointF());
    if(lastOffset != m_systemOffset){
        //Reload all particles
        foreach(const QString &g, m_particles){
            int gId = m_system->m_groupIds[g];
            for(int i=0; i<m_system->m_groupData[gId]->size; i++)
                reload(m_system->m_data[m_system->m_groupData[gId]->start + i]);
        }
    }
}
