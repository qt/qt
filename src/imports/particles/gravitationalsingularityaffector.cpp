/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "gravitationalsingularityaffector.h"
#include <cmath>
#include <QDebug>
QT_BEGIN_NAMESPACE
GravitationalSingularityAffector::GravitationalSingularityAffector(QSGItem *parent) :
    ParticleAffector(parent), m_strength(0.0), m_x(0), m_y(0)
{
}

const qreal LIMIT = 200;
qreal limit(qreal val){
    if(qAbs(val) > LIMIT){
        return val < 0 ? LIMIT * -1 : LIMIT;
    }else{
        return val;
    }
}

bool GravitationalSingularityAffector::affectParticle(ParticleData *d, qreal dt)
{
    if(!m_strength)
        return false;
    qreal dx = m_x - d->curX();
    qreal dy = m_y - d->curY();
    qreal r = sqrt((dx*dx) + (dy*dy));
    if(r < 0.1 ){//Simulated event horizion - It's right on top of it, and will never escape again. just stick it here.
        d->pv.ax = 0;
        d->pv.ay = 0;
        d->pv.sx = 0;
        d->pv.sy = 0;
        d->pv.x = m_x;
        d->pv.y = m_y;
        return true;
    }else if(r < 50.0){//Too close, typical dt values are far too coarse for simulation. This may kill perf though
        int parts = floor(100.0/r);
        ParticleData* f = new ParticleData;//Fake, where it's all in real time for convenience
        f->pv.x = d->curX();
        f->pv.y = d->curY();
        f->pv.sx = limit(d->curSX());
        f->pv.sy = limit(d->curSY());
        f->pv.ax = d->pv.ax;
        f->pv.ay = d->pv.ay;
        subaffect(f, dt/parts, true);
        for(int i=1; i<parts; i++)
            subaffect(f, dt/parts, false);

        //Copy values from f, and turn into 'from start' values
        qreal t = (m_system->m_timeInt/1000.) - d->pv.t;
        qreal sy = limit(f->pv.sy) - t*f->pv.ay;
        qreal y = f->pv.y - t*sy - 0.5 * t*t*f->pv.ay;
        qreal sx = limit(f->pv.sx) - t*f->pv.ax;
        qreal x = f->pv.x - t*sx - 0.5 * t*t*f->pv.ax;

        d->pv.ay = f->pv.ay;
        d->pv.sy = sy;
        d->pv.y = y;
        d->pv.ax = f->pv.ax;
        d->pv.sx = sx;
        d->pv.x = x;
        return true;
    }
    qreal theta = atan2(dy,dx);
    qreal ds = (m_strength / (r*r)) * dt;
    dx = ds * cos(theta);
    dy = ds * sin(theta);
    d->setInstantaneousSX(limit(d->pv.sx + dx));
    d->setInstantaneousSY(limit(d->pv.sy + dy));
    return true;
}

const qreal EPSILON = 0.1;
bool fuzzyCompare(qreal a, qreal b)
{
    //Not using qFuzzyCompare because I want control of epsilon
    return (a >= b - EPSILON && a <= b + EPSILON);
}

bool fuzzyLess(qreal a, qreal b)
{
    //Not using qFuzzyCompare because I want control of epsilon
    return a <= b + EPSILON;
}

bool fuzzyMore(qreal a, qreal b)
{
    //Not using qFuzzyCompare because I want control of epsilon
    return a >= b - EPSILON;
}

bool lineIntersect(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3)
{
    if(x3 < qMin(x1,x2) || x3 > qMax(x1,x2) || y3 < qMin(y1,y2) || y3 > qMax(y1,y2))
        return false;
    qreal m,c;
    m = (y2-y1) / (x2-x1);
    c = y1 - m*x1;
    return (fuzzyCompare(y3, m*x3 + c));
}

void GravitationalSingularityAffector::subaffect(ParticleData *d, qreal dt, bool first)
{
    if(!first){
        qreal nextX = d->pv.x + d->pv.sx * dt + d->pv.ax * dt * dt * 0.5;
        qreal nextY = d->pv.y + d->pv.sy * dt + d->pv.ay * dt * dt * 0.5;
        if(lineIntersect(d->pv.x, d->pv.y, nextX, nextY, m_x, m_y)){
            d->pv.ax = 0;
            d->pv.ay = 0;
            d->pv.sx = 0;
            d->pv.sy = 0;
            d->pv.x = m_x;
            d->pv.y = m_y;
            return;
            //Passed center - the near infinite forces cancel out
//            d->pv.x = m_x + m_x - d->pv.x;
//            d->pv.y = m_y + m_y - d->pv.y;
//            d->pv.sx *= -1;
//            d->pv.sy *= -1;
//            return;
        }
        //Simulate advancing a dt
        d->pv.x = nextX;
        d->pv.y = nextY;
        d->pv.sx += d->pv.ax * dt;
        d->pv.sy += d->pv.ay * dt;
    }
    qreal dx = m_x - d->pv.x;
    qreal dy = m_y - d->pv.y;
    qreal r = sqrt((dx*dx) + (dy*dy));
    if(!r)
        return;
    qreal theta = atan2(dy,dx);
    qreal ds = (m_strength / (r*r)) * dt;
    dx = ds * cos(theta);
    dy = ds * sin(theta);
    d->pv.sx = d->pv.sx + dx;
    d->pv.sy = d->pv.sy + dy;
}
QT_END_NAMESPACE
