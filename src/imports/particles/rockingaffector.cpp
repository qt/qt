#include "rockingaffector.h"
#include "spriteparticles.h" //for ParticlesVertices
#include <cmath>

const qreal PI = 3.141582653;//TODO: Use trancendental PI?
const qreal CONV = PI/180;


RockingAffector::RockingAffector(QObject *parent) :
    ParticleAffector(parent),
    m_angle(45.0),
    m_angleVariance(0.0),
    m_pace(1.0),
    m_minSpeed(10.0)
{
}


RockingAffector::~RockingAffector()
{
    for(QHash<int, RockerData*>::const_iterator iter=m_rockerData.constBegin();
        iter != m_rockerData.constEnd(); iter++)
        delete (*iter);
}

RockerData* RockingAffector::getData(int idx)
{
    if(m_rockerData.contains(idx))
        return m_rockerData[idx];
    RockerData* d = new RockerData;
    d->curAngle = 0;
    d->peakAngle = (m_angle * CONV) +  (m_angleVariance * CONV) * (qreal(qrand()) / RAND_MAX);
    d->dir = (qreal)(qrand() % 2);
    if(d->dir == 0.0)
        d->dir = -1.0;
    m_rockerData.insert(idx, d);
    return d;
}

qreal sign(qreal a){
    return a<0.0 ? -1.0 : 1.0;
}

void RockingAffector::affect(ParticleVertices *p, int idx, qreal dt)
{
    RockerData* d = getData(idx);
    qreal da =  d->dir * m_pace * dt * (d->peakAngle - qAbs(d->curAngle));
    if(qAbs(da/dt) < m_minSpeed*CONV){//TODO: This trips up instantly if pace is too slow
        d->dir = sign(d->curAngle) * -1;//Takes effect next tick
        da = d->dir * m_minSpeed*CONV * dt;
    }
    d->curAngle += da;

    p->v2.x = cos(da) * (p->v2.x - p->v1.x) - sin(da) * (p->v2.y-p->v1.y) + p->v1.x;
    p->v2.y = sin(da) * (p->v2.x - p->v1.x) + cos(da) * (p->v2.y-p->v1.y) + p->v1.y;

    p->v3.x = cos(da) * (p->v3.x - p->v1.x) - sin(da) * (p->v3.y-p->v1.y) + p->v1.x;
    p->v3.y = sin(da) * (p->v3.x - p->v1.x) + cos(da) * (p->v3.y-p->v1.y) + p->v1.y;

    p->v4.x = cos(da) * (p->v4.x - p->v1.x) - sin(da) * (p->v4.y-p->v1.y) + p->v1.x;
    p->v4.y = sin(da) * (p->v4.x - p->v1.x) + cos(da) * (p->v4.y-p->v1.y) + p->v1.y;
}
