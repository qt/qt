#include "wanderaffector.h"
#include "spriteemitter.h"
#include "particlesystem.h"//for ParticlesVertices

WanderAffector::WanderAffector(QObject *parent) :
    ParticleAffector(parent)
{
}

WanderAffector::~WanderAffector()
{
    for(QHash<int, WanderData*>::const_iterator iter=m_wanderData.constBegin();
        iter != m_wanderData.constEnd(); iter++)
        delete (*iter);
}

WanderData* WanderAffector::getData(int idx)
{
    if(m_wanderData.contains(idx))
        return m_wanderData[idx];
    WanderData* d = new WanderData;
    d->x_vel = 0;
    d->y_vel = 0;
    d->x_peak = m_xVariance;
    d->y_peak = m_yVariance;
    d->x_var = m_pace * qreal(qrand()) / RAND_MAX;
    d->y_var = m_pace * qreal(qrand()) / RAND_MAX;

    m_wanderData.insert(idx, d);
    return d;
}

void WanderAffector::reset(int systemIdx)
{
    if(m_wanderData.contains(systemIdx))
        delete m_wanderData[systemIdx];
    m_wanderData.remove(systemIdx);
}

bool WanderAffector::affect(ParticleData* data, qreal dt)
{
    WanderData* d = getData(data->systemIndex);
    if (m_xVariance != 0.) {
        if ((d->x_vel > d->x_peak && d->x_var > 0.0) || (d->x_vel < -d->x_peak && d->x_var < 0.0)) {
            d->x_var = -d->x_var;
            d->x_peak = m_xVariance + m_xVariance * qreal(qrand()) / RAND_MAX;
        }
        d->x_vel += d->x_var * dt;
    }
    qreal dx = dt * d->x_vel;

    if (m_yVariance != 0.) {
        if ((d->y_vel > d->y_peak && d->y_var > 0.0) || (d->y_vel < -d->y_peak && d->y_var < 0.0)) {
            d->y_var = -d->y_var;
            d->y_peak = m_yVariance + m_yVariance * qreal(qrand()) / RAND_MAX;
        }
        d->y_vel += d->y_var * dt;
    }
    qreal dy = dt * d->x_vel;

    //### Should we be amending vel instead?
    ParticleVertices* p = &(data->pv);
    p->v1.x += dx;
    p->v2.x += dx;
    p->v3.x += dx;
    p->v4.x += dx;

    p->v1.y += dy;
    p->v2.y += dy;
    p->v3.y += dy;
    p->v4.y += dy;
    return true;
}
