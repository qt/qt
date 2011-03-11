#include "speedlimitaffector.h"
#include <cmath>
#include <QDebug>

SpeedLimitAffector::SpeedLimitAffector(QSGItem *parent) :
    ParticleAffector(parent), m_speedLimit(-1)
{
}

bool SpeedLimitAffector::affectParticle(ParticleData *d, qreal dt){
    Q_UNUSED(dt);
    if(m_speedLimit <= 0)
        return false;

    qreal x = d->curSX();
    qreal y = d->curSY();
    qreal s = sqrt(x*x + y*y);
    if(s <= m_speedLimit)
        return false;


    if(s >= m_speedLimit*1.01){
        qreal theta = atan2(y,x);
        d->setInstantaneousSX(m_speedLimit * cos(theta));
        d->setInstantaneousSY(m_speedLimit * sin(theta));
    }

    d->setInstantaneousAY(0);
    d->setInstantaneousAX(0);

    return true;
}
