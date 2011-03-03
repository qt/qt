#include "gravitationalsingularityaffector.h"
#include <cmath>
#include <QDebug>

GravitationalSingularityAffector::GravitationalSingularityAffector(QObject *parent) :
    ParticleAffector(parent), m_strength(0.0), m_x(0), m_y(0)
{
}

bool GravitationalSingularityAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_strength)
        return false;
    subaffect(d, dt, true);
    return true;
}

void GravitationalSingularityAffector::subaffect(ParticleData *d, qreal dt, bool first)
{
    qreal dx = m_x - d->curX();
    qreal dy = m_y - d->curY();
    qreal r = sqrt((dx*dx) + (dy*dy));
    if(first && r < 1 ){//It's right on top of it, and will never escape again. just stick it here.
        d->pv.ax = 0;
        d->pv.ay = 0;
        d->pv.sx = 0;
        d->pv.sy = 0;
        d->pv.x = m_x;
        d->pv.y = m_y;
        return;
    }else if(first && r < 10.0){//Too close, typical dt values are far too coarse for simulation. This may kill perf though
        int parts = floor(100.0/r);
        for(int i=0; i<parts; i++)
            subaffect(d, dt/parts, false);
        return;
    }
    qreal theta = atan2(dy,dx);
    qreal ds = (m_strength / (r*r)) * dt;
    dx = ds * cos(theta);
    dy = ds * sin(theta);
    d->setInstantaneousSX(d->pv.sx + dx);
    d->setInstantaneousSY(d->pv.sy + dy);
}
