#include "frictionaffector.h"
QT_BEGIN_NAMESPACE
FrictionAffector::FrictionAffector(QSGItem *parent) :
    ParticleAffector(parent), m_factor(0.0)
{
}

bool FrictionAffector::affectParticle(ParticleData *d, qreal dt)
{
    if(!m_factor)
        return false;
    qreal curSX = d->curSX();
    qreal curSY = d->curSY();
    d->setInstantaneousSX(curSX + (curSX * m_factor * -1 * dt));
    d->setInstantaneousSY(curSY + (curSY * m_factor * -1 * dt));
    return true;
}
QT_END_NAMESPACE
