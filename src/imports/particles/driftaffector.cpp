#include "driftaffector.h"
#include "particlesystem.h"

DriftAffector::DriftAffector(QObject *parent) :
    ParticleAffector(parent)
{
}

DriftAffector::~DriftAffector()
{
    for(QHash<int, DriftData*>::const_iterator iter=m_driftData.constBegin();
        iter != m_driftData.constEnd(); iter++)
        delete (*iter);
}


DriftData* DriftAffector::getData(int idx)
{
    if(m_driftData.contains(idx))
        return m_driftData[idx];
    DriftData* d = new DriftData;
    d->xVel = 0;
    d->yVel = 0;
    m_driftData.insert(idx, d);
    return d;
}

void DriftAffector::reset(int idx)
{
    m_driftData.remove(idx);
}

void DriftAffector::affect(ParticleVertices *p, int idx, qreal dt, QObject*)
{
    DriftData* d = getData(idx);
    qreal dx = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_xDrift * dt;
    qreal dy = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_yDrift * dt;
    d->xVel += dx * dt;
    d->yVel += dy * dt;

    p->v1.x += d->xVel * dt;
    p->v1.y += d->yVel * dt;

    p->v2.x += d->xVel * dt;
    p->v2.y += d->yVel * dt;

    p->v3.x += d->xVel * dt;
    p->v3.y += d->yVel * dt;

    p->v4.x += d->xVel * dt;
    p->v4.y += d->yVel * dt;
}
