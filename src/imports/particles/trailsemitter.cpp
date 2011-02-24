#include "trailsemitter.h"
#include "particlesystem.h"
#include "particle.h"

TrailsEmitter::TrailsEmitter(QObject* parent)
    : ParticleEmitter(parent)
    , m_particle_size(16)
    , m_particle_end_size(-1)
    , m_particle_size_variation(0)
    , m_emitter_x(0)
    , m_emitter_y(0)
    , m_emitter_x_variation(0)
    , m_emitter_y_variation(0)
    , m_x_speed(0)
    , m_y_speed(0)
    , m_x_speed_variation(0)
    , m_y_speed_variation(0)
    , m_speed_from_movement(0)
    , m_x_accel(0)
    , m_y_accel(0)
    , m_x_accel_variation(0)
    , m_y_accel_variation(0)
    , m_particle_count(0)
    , m_reset_last(true)
    , m_last_timestamp(0)
    , m_last_particle(0)
{
//    setFlag(ItemHasContents);
}


void TrailsEmitter::setParticleSize(qreal size)
{
    if (size == m_particle_size)
        return;
    m_particle_size = size;
    emit particleSizeChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setParticleEndSize(qreal size)
{
    if (size == m_particle_end_size)
        return;
    m_particle_end_size = size;
    emit particleEndSizeChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setParticleSizeVariation(qreal var)
{
    if (var == m_particle_size_variation)
        return;
    m_particle_size_variation = var;
    emit particleSizeVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setEmitterX(qreal x)
{
    if (x == m_emitter_x)
        return;
    m_emitter_x = x;
    emit emitterXChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setEmitterY(qreal y)
{
    if (y == m_emitter_y)
        return;
    m_emitter_y = y;
    emit emitterYChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setEmitterXVariation(qreal var)
{
    if (var == m_emitter_x_variation)
        return;
    m_emitter_x_variation = var;
    emit emitterXVariationChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setEmitterYVariation(qreal var)
{
    if (var == m_emitter_y_variation)
        return;
    m_emitter_y_variation = var;
    emit emitterYVariationChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setXSpeed(qreal x)
{
    if (x == m_x_speed)
        return;
    m_x_speed = x;
    emit xSpeedChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setYSpeed(qreal y)
{
    if (y == m_y_speed)
        return;
    m_y_speed = y;
    emit ySpeedChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setXSpeedVariation(qreal x)
{
    if (x == m_x_speed_variation)
        return;
    m_x_speed_variation = x;
    emit xSpeedVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setYSpeedVariation(qreal y)
{
    if (y == m_y_speed_variation)
        return;
    m_y_speed_variation = y;
    emit ySpeedVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setSpeedFromMovement(qreal t)
{
    if (t == m_speed_from_movement)
        return;
    m_speed_from_movement = t;
    emit speedFromMovementChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setXAccel(qreal x)
{
    if (x == m_x_accel)
        return;
    m_x_accel = x;
    emit xAccelChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setYAccel(qreal y)
{
    if (y == m_y_accel)
        return;
    m_y_accel = y;
    emit yAccelChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setXAccelVariation(qreal x)
{
    if (x == m_x_accel_variation)
        return;
    m_x_accel_variation = x;
    emit xAccelVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setYAccelVariation(qreal y)
{
    if (y == m_y_accel_variation)
        return;
    m_y_accel_variation = y;
    emit yAccelVariationChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::emitWindow(int timeStamp)
{
    if (m_system == 0)
        return;
    if(!m_emitting){
        m_reset_last = true;
        return;
    }

    if (m_reset_last) {
        m_last_emitter = m_last_last_emitter = QPointF(m_emitter_x, m_emitter_y);
        m_last_timestamp = timeStamp/1000.;
        m_reset_last = false;
    }

    //TODO:track properly
    m_particle_count = m_particlesPerSecond * (m_particleDuration / 1000.);

    qreal time = timeStamp / 1000.;


    qreal particleRatio = 1. / m_particlesPerSecond;
    qreal pt = m_last_particle * particleRatio;

    qreal opt = pt; // original particle time
    qreal dt = time - m_last_timestamp; // timestamp delta...

    // emitter difference since last...
    qreal dex = (m_emitter_x - m_last_emitter.x());
    qreal dey = (m_emitter_y - m_last_emitter.y());

    qreal ax = (m_last_last_emitter.x() + m_last_emitter.x()) / 2;
    qreal bx = m_last_emitter.x();
    qreal cx = (m_emitter_x + m_last_emitter.x()) / 2;
    qreal ay = (m_last_last_emitter.y() + m_last_emitter.y()) / 2;
    qreal by = m_last_emitter.y();
    qreal cy = (m_emitter_y + m_last_emitter.y()) / 2;

    float sizeAtEnd = m_particle_end_size >= 0 ? m_particle_end_size : m_particle_size;

    while (pt < time) {
        int pos = m_last_particle % m_particle_count;
        ParticleData* datum = m_system->newDatum();
        datum->p = m_particle;
        datum->e = this;
        datum->emitterIndex = pos;
        ParticleVertices &p = datum->pv;

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
        p.v1.t = p.v2.t = p.v3.t = p.v4.t = pt;
        p.v1.dt = p.v2.dt = p.v3.dt = p.v4.dt = pt;

        // Particle position
        p.v1.x = p.v2.x = p.v3.x = p.v4.x =
                m_last_emitter.x() + dex * (pt - opt) / dt
                - m_emitter_x_variation + rand() / float(RAND_MAX) * m_emitter_x_variation * 2;
        p.v1.y = p.v2.y = p.v3.y = p.v4.y =
                m_last_emitter.y() + dey * (pt - opt) / dt
                - m_emitter_y_variation + rand() / float(RAND_MAX) * m_emitter_y_variation * 2;

        // Particle speed
        p.v1.sx = p.v2.sx = p.v3.sx = p.v4.sx =
                m_x_speed
                - m_x_speed_variation + rand() / float(RAND_MAX) * m_x_speed_variation * 2
                + m_speed_from_movement * vx;
        p.v1.sy = p.v2.sy = p.v3.sy = p.v4.sy =
                m_y_speed
                - m_y_speed_variation + rand() / float(RAND_MAX) * m_y_speed_variation * 2
                + m_speed_from_movement * vy;

        // Particle acceleration
        p.v1.ax = p.v2.ax = p.v3.ax = p.v4.ax =
                m_x_accel - m_x_accel_variation + rand() / float(RAND_MAX) * m_x_accel_variation * 2;
        p.v1.ay = p.v2.ay = p.v3.ay = p.v4.ay =
                m_y_accel - m_y_accel_variation + rand() / float(RAND_MAX) * m_y_accel_variation * 2;

        // Particle size
        float sizeVariation = -m_particle_size_variation
                + rand() / float(RAND_MAX) * m_particle_size_variation * 2;

        float size = m_particle_size + sizeVariation;
        float endSize = sizeAtEnd + sizeVariation;

        p.v1.size = p.v2.size = p.v3.size = p.v4.size = size * float(m_emitting);
        p.v1.endSize = p.v2.endSize = p.v3.endSize = p.v4.endSize = endSize * float(m_emitting);

        ++m_last_particle;
        pt = m_last_particle * particleRatio;
        m_system->emitParticle(datum);
    }

    m_last_last_last_emitter = m_last_last_emitter;
    m_last_last_emitter = m_last_emitter;
    m_last_emitter = QPointF(m_emitter_x, m_emitter_y);
    m_last_timestamp = time;
}

