#include "rockingaffector.h"
#include "spriteemitter.h"
#include "particlesystem.h"//for ParticlesVertices
#include <cmath>
#include <QDebug>

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

void RockingAffector::reset(int idx)
{
    if(!m_rockerData.contains(idx))
        return;
    RockerData* d = m_rockerData[idx];
    d->curAngle = 0;
    d->peakAngle = (m_angle * CONV) +  (m_angleVariance * CONV) * (qreal(qrand()) / RAND_MAX);
    d->dir = (qreal)(qrand() % 2);
    if(d->dir == 0.0)
        d->dir = -1.0;
    d->initialAngleSet = false;
}

RockerData* RockingAffector::getData(int idx)
{
    if(m_rockerData.contains(idx))
        return m_rockerData[idx];
    RockerData* d = new RockerData;
    m_rockerData.insert(idx, d);
    reset(idx);
    return d;
}

qreal sign(qreal a){
    return a<0.0 ? -1.0 : 1.0;
}

void RockingAffector::affect(ParticleVertices *p, int idx, qreal dt, QObject*)
{
    RockerData* d = getData(idx);
    qreal da = 0.0;
    if(d->initialAngleSet){
        da =  d->dir * m_pace * dt * (d->peakAngle - qAbs(d->curAngle));
        if(qAbs(da/dt) < m_minSpeed*CONV){//TODO: This trips up instantly if pace is too slow
            d->dir = sign(d->curAngle) * -1;//Takes effect next tick
            da = d->dir * m_minSpeed*CONV * dt;
        }
        d->curAngle += da;
    }else{//Jump to starting angle
        da = m_initialAngle * CONV;
        d->curAngle = 0;//curAngle is relative to initialAngle
        d->initialAngleSet = true;
    }

    qreal v2x = cos(da) * (p->v2.x - p->v1.x) - sin(da) * (p->v2.y-p->v1.y) + p->v1.x;
    qreal v2y = sin(da) * (p->v2.x - p->v1.x) + cos(da) * (p->v2.y-p->v1.y) + p->v1.y;
    p->v2.x = v2x;
    p->v2.y = v2y;

    qreal v3x = cos(da) * (p->v3.x - p->v1.x) - sin(da) * (p->v3.y-p->v1.y) + p->v1.x;
    qreal v3y = sin(da) * (p->v3.x - p->v1.x) + cos(da) * (p->v3.y-p->v1.y) + p->v1.y;
    p->v3.x = v3x;
    p->v3.y = v3y;

    qreal v4x = cos(da) * (p->v4.x - p->v1.x) - sin(da) * (p->v4.y-p->v1.y) + p->v1.x;
    qreal v4y = sin(da) * (p->v4.x - p->v1.x) + cos(da) * (p->v4.y-p->v1.y) + p->v1.y;
    p->v4.x = v4x;
    p->v4.y = v4y;
}
