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

bool DriftAffector::affect(ParticleData *data, qreal dt)
{
    DriftData* d = getData(data->systemIndex);
    qreal dx = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_xDrift * dt;
    qreal dy = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_yDrift * dt;
    d->xVel += dx * dt;
    d->yVel += dy * dt;

    data->pv.x += d->xVel * dt;
    data->pv.y += d->yVel * dt;

    return true;
}
