#include "directedemitter.h"
#include <cmath>

DirectedEmitter::DirectedEmitter(QSGItem *parent) :
    ParticleEmitter(parent)
  , m_particleSize(16)
  , m_particleEndSize(-1)
  , m_particleSizeVariation(0)
  , m_targetX(0)
  , m_targetY(0)
  , m_speed(0)
  , m_speedVariation(0)
  , m_acceleration(0)
  , m_accelerationVariation(0)
  , m_resetLast(true)
  , m_lastTimestamp(0)
{
}

void DirectedEmitter::reset()
{
    m_resetLast = true;
    m_lastTimestamp = 0;
}

void DirectedEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if(!m_emitting){
        m_resetLast = true;
        m_lastTimestamp = timeStamp / 1000.0;
        return;
    }
    if(m_resetLast){
        m_resetLast = false;
        m_lastTarget = QPointF(m_targetX, m_targetY);
        m_lastParticle = ceil(m_lastTimestamp * m_particlesPerSecond);
    }

    qreal time = timeStamp / 1000.;

    qreal dex = m_targetX - m_lastTarget.x();
    qreal dey = m_targetY - m_lastTarget.y();

    qreal particleRatio = 1. / m_particlesPerSecond;
    qreal pt = m_lastParticle * particleRatio;
    qreal opt = pt; // original particle time
    qreal dt = time - m_lastTimestamp; // timestamp delta...
    if(!dt)
        dt = 0.000001;
    float sizeAtEnd = m_particleEndSize >= 0 ? m_particleEndSize : m_particleSize;
    while (pt < time) {
        //int pos = m_last_particle % m_particle_count;
        ParticleData* datum = m_system->newDatum(m_system->m_groupIds[m_particle]);
        datum->e = this;//###useful?
        ParticleVertex &p = datum->pv;


        // Particle timestamp
        p.t = pt;
        p.lifeSpan =
                (m_particleDuration
                 + ((rand() % ((m_particleDurationVariation*2) + 1)) - m_particleDurationVariation))
                / 1000.0;

        // Particle position
        QRectF boundsRect(x(), y(), width(), height());
        QPointF newPos = effectiveExtruder()->extrude(boundsRect);
        p.x = newPos.x();
        p.y = newPos.y();

        qreal theta = atan2((m_targetY - (dey *  (pt - opt) / dt)) - p.y,
                            (m_targetX - (dex *  (pt - opt) / dt)) - p.x);


        // Particle speed
        p.sx =
                (m_speed
                - m_speedVariation + rand() / float(RAND_MAX) * m_speedVariation * 2)
                * cos(theta);
        p.sy =
                (m_speed
                - m_speedVariation + rand() / float(RAND_MAX) * m_speedVariation * 2)
                * sin(theta);

        // Particle acceleration
        p.ax =
                (m_acceleration - m_accelerationVariation + rand() / float(RAND_MAX) * m_accelerationVariation * 2)
                * cos(theta);
        p.ay =
                (m_acceleration - m_accelerationVariation + rand() / float(RAND_MAX) * m_accelerationVariation * 2)
                * sin(theta);

        // Particle size
        float sizeVariation = -m_particleSizeVariation
                + rand() / float(RAND_MAX) * m_particleSizeVariation * 2;

        float size = m_particleSize + sizeVariation;
        float endSize = sizeAtEnd + sizeVariation;

        p.size = size * float(m_emitting);
        p.endSize = endSize * float(m_emitting);

        ++m_lastParticle;
        pt = m_lastParticle * particleRatio;
        m_system->emitParticle(datum);
    }

    m_lastTarget = QPointF(m_targetX, m_targetY);
    m_lastTimestamp = time;
}
