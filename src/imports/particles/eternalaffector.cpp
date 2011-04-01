#include "eternalaffector.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

EternalAffector::EternalAffector(QSGItem *parent) :
    ParticleAffector(parent)
{
}

bool EternalAffector::affectParticle(ParticleData *d, qreal dt)
{
    qreal target = (m_system->m_timeInt - m_targetLife)/1000.0;
    if(d->pv.t < target)
        d->pv.t = target;
    return true;
}

QT_END_NAMESPACE
