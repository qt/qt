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

#include "trailsemitter.h"
#include "particlesystem.h"
#include "particle.h"
QT_BEGIN_NAMESPACE

TrailsEmitter::TrailsEmitter(QSGItem* parent)
    : ParticleEmitter(parent)
    , m_speed_from_movement(0)
    , m_particle_count(0)
    , m_reset_last(true)
    , m_last_timestamp(0)
    , m_last_emission(0)
{
//    setFlag(ItemHasContents);
}

void TrailsEmitter::setSpeedFromMovement(qreal t)
{
    if (t == m_speed_from_movement)
        return;
    m_speed_from_movement = t;
    emit speedFromMovementChanged();
}

void TrailsEmitter::reset()
{
    m_reset_last = true;
}

void TrailsEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if((!m_emitting || !m_particlesPerSecond)&& !m_burstLeft && !m_emitLeft){
        m_reset_last = true;
        return;
    }

    if (m_reset_last) {
        m_last_emitter = m_last_last_emitter = QPointF(x(), y());
        m_last_timestamp = timeStamp/1000.;
        m_last_emission = m_last_timestamp;
        m_reset_last = false;
    }

    if(m_burstLeft){
        m_burstLeft -= timeStamp - m_last_timestamp * 1000.;
        if(m_burstLeft < 0){
            if(!m_emitting)
                timeStamp += m_burstLeft;
            m_burstLeft = 0;
        }
    }
    qreal time = timeStamp / 1000.;

    qreal particleRatio = 1. / m_particlesPerSecond;
    qreal pt = m_last_emission;

    qreal opt = pt; // original particle time
    qreal dt = time - m_last_timestamp; // timestamp delta...
    if(!dt)
        dt = 0.000001;

    // emitter difference since last...
    qreal dex = (x() - m_last_emitter.x());
    qreal dey = (y() - m_last_emitter.y());

    qreal ax = (m_last_last_emitter.x() + m_last_emitter.x()) / 2;
    qreal bx = m_last_emitter.x();
    qreal cx = (x() + m_last_emitter.x()) / 2;
    qreal ay = (m_last_last_emitter.y() + m_last_emitter.y()) / 2;
    qreal by = m_last_emitter.y();
    qreal cy = (y() + m_last_emitter.y()) / 2;

    qreal sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;
    qreal emitter_x_offset = m_last_emitter.x() - x();
    qreal emitter_y_offset = m_last_emitter.y() - y();
    while (pt < time || m_emitLeft) {
        //int pos = m_last_particle % m_particle_count;
        ParticleData* datum = m_system->newDatum(m_system->m_groupIds[m_particle]);
        if(!datum){//skip this emission
            if(!m_emitLeft)
                pt += particleRatio;
            else
                --m_emitLeft;
            continue;
        }
        datum->e = this;//###useful?
        ParticleVertex &p = datum->pv;
        qreal t = 1 - (pt - opt) / dt;
        qreal vx =
          - 2 * ax * (1 - t)
          + 2 * bx * (1 - 2 * t)
          + 2 * cx * t;
        qreal vy =
          - 2 * ay * (1 - t)
          + 2 * by * (1 - 2 * t)
          + 2 * cy * t;


        // Particle timestamp
        p.t = pt;
        p.lifeSpan = //TODO:Promote to base class?
                (m_particleDuration
                 + ((rand() % ((m_particleDurationVariation*2) + 1)) - m_particleDurationVariation))
                / 1000.0;

        // Particle position
        QRectF boundsRect(emitter_x_offset + dex * (pt - opt) / dt, emitter_y_offset + dey * (pt - opt) / dt
                          , width(), height());
        QPointF newPos = effectiveExtruder()->extrude(boundsRect);
        p.x = newPos.x();
        p.y = newPos.y();

        // Particle speed
        const QPointF &speed = m_speed->sample(newPos);
        p.sx = speed.x()
                + m_speed_from_movement * vx;
        p.sy = speed.y()
                + m_speed_from_movement * vy;

        // Particle acceleration
        const QPointF &accel = m_acceleration->sample(newPos);
        p.ax = accel.x();
        p.ay = accel.y();

        // Particle size
        float sizeVariation = -m_particleSizeVariation
                + rand() / float(RAND_MAX) * m_particleSizeVariation * 2;

        float size = qMax((qreal)0.0 , m_particleSize + sizeVariation);
        float endSize = qMax((qreal)0.0 , sizeAtEnd + sizeVariation);

        p.size = size;// * float(m_emitting);
        p.endSize = endSize;// * float(m_emitting);

        if(!m_emitLeft)
            pt += particleRatio;
        else
            --m_emitLeft;

        m_system->emitParticle(datum);
    }
    m_last_emission = pt;

    m_last_last_last_emitter = m_last_last_emitter;
    m_last_last_emitter = m_last_emitter;
    m_last_emitter = QPointF(x(), y());
    m_last_timestamp = time;
}


QT_END_NAMESPACE
