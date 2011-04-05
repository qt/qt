#include "particleaffector.h"
#include <QDebug>
QT_BEGIN_NAMESPACE
ParticleAffector::ParticleAffector(QSGItem *parent) :
    QSGItem(parent), m_needsReset(false), m_system(false), m_active(true), m_updateIntSet(false)
{
}

void ParticleAffector::affectSystem(qreal dt)
{
    if(!m_active)
        return;
    if(!m_system){
        qDebug() << "No system" << this;
        return;
    }
    //If not reimplemented, calls affect particle per particle
    //But only on particles in targeted system/area
    if(m_updateIntSet){
        m_groups.clear();
        foreach(const QString &p, m_particles)
            m_groups << m_system->m_groupIds[p];//###Can this occur before group ids are properly assigned?
        m_updateIntSet = false;
    }
    foreach(ParticleData* d, m_system->m_data){
        if(!d)
            return;
        if(m_groups.isEmpty() || m_groups.contains(d->group))
            if(width() == 0 || height() == 0 || QRectF(x(), y(), width(), height()).contains(d->curX(), d->curY()))//TODO: Offset
                 if(affectParticle(d, dt))
                     m_system->m_needsReset << d;
    }
}

bool ParticleAffector::affectParticle(ParticleData *d, qreal dt)
{
    Q_UNUSED(d);
    Q_UNUSED(dt);
    return false;
}

void ParticleAffector::reset(int idx)
{
    Q_UNUSED(idx);
}
QT_END_NAMESPACE
