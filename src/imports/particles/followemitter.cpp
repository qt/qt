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

#include "followemitter.h"
#include "particle.h"
#include <cmath>
QT_BEGIN_NAMESPACE

FollowEmitter::FollowEmitter(QSGItem *parent) :
    ParticleEmitter(parent)
  , m_lastTimeStamp(0)
  , m_emitterXVariation(0)
  , m_emitterYVariation(0)
  , m_followCount(0)
  , m_emissionExtruder(0)
  , m_defaultEmissionExtruder(new ParticleExtruder(this))
{
    connect(this, SIGNAL(followChanged(QString)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particleDurationChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
    connect(this, SIGNAL(particlesPerParticlePerSecondChanged(int)),
            this, SLOT(recalcParticlesPerSecond()));
}

void FollowEmitter::recalcParticlesPerSecond(){
    if(!m_system)
        return;
    m_followCount = m_system->m_groupData[m_system->m_groupIds[m_follow]]->size;
    if(!m_followCount){
        setParticlesPerSecond(1000);//XXX: Fix this horrendous hack, needed so they aren't turned off from start
    }else{
        setParticlesPerSecond(m_particlesPerParticlePerSecond * m_followCount);
        m_lastEmission.resize(m_followCount);
        m_lastEmission.fill(0);
    }
}

void FollowEmitter::reset()
{
    m_followCount = 0;
}

void FollowEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if(!m_emitting && !m_burstLeft && !m_emitLeft)
        return;
    if(m_followCount != m_system->m_groupData[m_system->m_groupIds[m_follow]]->size){
        qreal oldPPS = m_particlesPerSecond;
        recalcParticlesPerSecond();
        if(m_particlesPerSecond != oldPPS)
            return;//system may need to update
    }

    if(m_burstLeft){
        m_burstLeft -= timeStamp - m_lastTimeStamp * 1000.;
        if(m_burstLeft < 0){
            timeStamp += m_burstLeft;
            m_burstLeft = 0;
        }
    }

    qreal time = timeStamp / 1000.;
    qreal particleRatio = 1. / m_particlesPerParticlePerSecond;
    qreal pt;

    //Have to map it into this system, because particlesystem automaps it back
    QPointF offset = m_system->mapFromItem(this, QPointF(0, 0));
    qreal sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;

    int gId = m_system->m_groupIds[m_follow];
    int gId2 = m_system->m_groupIds[m_particle];
    for(int i=0; i<m_system->m_groupData[gId]->size; i++){
        pt = m_lastEmission[i];
        ParticleData* d = m_system->m_data[i + m_system->m_groupData[gId]->start];
        if(!d || !d->stillAlive())
            continue;
        if(pt < d->pv.t)
            pt = d->pv.t;

        if(!effectiveExtruder()->contains(QRectF(offset.x(), offset.y(), width(), height()),QPointF(d->curX(), d->curY()))){
            m_lastEmission[i] = time;//jump over this time period without emitting, because it's outside
            continue;
        }
        while(pt < time || m_emitLeft){
            ParticleData* datum = m_system->newDatum(gId2);
            if(!datum){//skip this emission
                if(m_emitLeft)
                    --m_emitLeft;
                else
                    pt += particleRatio;
                continue;
            }
            datum->e = this;//###useful?
            ParticleVertex &p = datum->pv;

            // Particle timestamp
            p.t = pt;
            p.lifeSpan =
                    (m_particleDuration
                     + ((rand() % ((m_particleDurationVariation*2) + 1)) - m_particleDurationVariation))
                    / 1000.0;

            // Particle position
            qreal followT =  pt - d->pv.t;
            qreal followT2 = followT * followT * 0.5;
            qreal sizeOffset = d->pv.size/2;//TODO: Current size? As an option
            //TODO: Set variations
            //Subtract offset, because PS expects this in emitter coordinates
            QRectF boundsRect(d->pv.x - offset.x() + d->pv.sx * followT + d->pv.ax * followT2 - m_emitterXVariation/2,
                              d->pv.y - offset.y() + d->pv.sy * followT + d->pv.ay * followT2 - m_emitterYVariation/2,
                              m_emitterXVariation,
                              m_emitterYVariation);
//            QRectF boundsRect(d->pv.x + d->pv.sx * followT + d->pv.ax * followT2 + offset.x() - sizeOffset,
//                              d->pv.y + d->pv.sy * followT + d->pv.ay * followT2 + offset.y() - sizeOffset,
//                              sizeOffset*2,
//                              sizeOffset*2);

            ParticleExtruder* effectiveEmissionExtruder = m_emissionExtruder ? m_emissionExtruder : m_defaultEmissionExtruder;
            const QPointF &newPos = effectiveEmissionExtruder->extrude(boundsRect);
            p.x = newPos.x();
            p.y = newPos.y();

            // Particle speed
            const QPointF &speed = m_speed->sample(newPos);
            p.sx = speed.x();
            p.sy = speed.y();

            // Particle acceleration
            const QPointF &accel = m_acceleration->sample(newPos);
            p.ax = accel.x();
            p.ay = accel.y();

            // Particle size
            float sizeVariation = -m_particleSizeVariation
                    + rand() / float(RAND_MAX) * m_particleSizeVariation * 2;

            float size = qMax((qreal)0.0, m_particleSize + sizeVariation);
            float endSize = qMax((qreal)0.0, sizeAtEnd + sizeVariation);

            p.size = size * float(m_emitting);
            p.endSize = endSize * float(m_emitting);

            if(m_emitLeft)
                --m_emitLeft;
            else
                pt += particleRatio;
            m_system->emitParticle(datum);
        }
        m_lastEmission[i] = pt;
    }

    m_lastTimeStamp = time;
}
QT_END_NAMESPACE
