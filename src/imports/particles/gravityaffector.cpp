#include "gravityaffector.h"
#include <cmath>
const qreal CONV = 0.017453292520444443;
GravityAffector::GravityAffector(QSGItem *parent) :
    ParticleAffector(parent), m_acceleration(-10), m_angle(90), m_xAcc(0), m_yAcc(0)
{
    connect(this, SIGNAL(accelerationChanged(qreal)),
            this, SLOT(recalc()));
    connect(this, SIGNAL(angleChanged(qreal)),
            this, SLOT(recalc()));
    recalc();
}

void GravityAffector::recalc()
{
    qreal theta = m_angle * CONV;
    m_xAcc = m_acceleration * cos(theta);
    m_yAcc = m_acceleration * sin(theta);
}

bool GravityAffector::affectParticle(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    bool changed = false;
    if(d->pv.ax != m_xAcc){
        d->setInstantaneousAX(m_xAcc);
        changed = true;
    }
    if(d->pv.ay != m_yAcc){
        d->setInstantaneousAY(m_yAcc);
        changed = true;
    }
    return changed;
}
