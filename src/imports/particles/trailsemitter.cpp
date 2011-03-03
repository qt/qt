#include "trailsemitter.h"
#include "particlesystem.h"
#include "particle.h"

TrailsEmitter::TrailsEmitter(QSGItem* parent)
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
        m_last_emitter = m_last_last_emitter = QPointF(x() + m_emitter_x, y() + m_emitter_y);
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
    qreal emitter_x_variation = m_emitter_x_variation + width()/2;
    qreal emitter_y_variation = m_emitter_y_variation + height()/2;
    qreal emitter_x_offset = m_last_emitter.x() - x() + width()/2;
    qreal emitter_y_offset = m_last_emitter.y() - y() + height()/2;
    while (pt < time) {
        int pos = m_last_particle % m_particle_count;
        ParticleData* datum = m_system->newDatum();
        datum->p = m_particle;
        datum->e = this;
        datum->emitterIndex = pos;
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
        p.t = p.dt = pt;

        // Particle position
        p.x =
                emitter_x_offset + dex * (pt - opt) / dt
                - emitter_x_variation + rand() / float(RAND_MAX) * emitter_x_variation * 2;
        p.y =
                emitter_y_offset + dey * (pt - opt) / dt
                - emitter_y_variation + rand() / float(RAND_MAX) * emitter_y_variation * 2;

        // Particle speed
        p.sx =
                m_x_speed
                - m_x_speed_variation + rand() / float(RAND_MAX) * m_x_speed_variation * 2
                + m_speed_from_movement * vx;
        p.sy =
                m_y_speed
                - m_y_speed_variation + rand() / float(RAND_MAX) * m_y_speed_variation * 2
                + m_speed_from_movement * vy;

        // Particle acceleration
        p.ax =
                m_x_accel - m_x_accel_variation + rand() / float(RAND_MAX) * m_x_accel_variation * 2;
        p.ay =
                m_y_accel - m_y_accel_variation + rand() / float(RAND_MAX) * m_y_accel_variation * 2;

        // Particle size
        float sizeVariation = -m_particle_size_variation
                + rand() / float(RAND_MAX) * m_particle_size_variation * 2;

        float size = m_particle_size + sizeVariation;
        float endSize = sizeAtEnd + sizeVariation;

        p.size = size * float(m_emitting);
        p.endSize = endSize * float(m_emitting);

        ++m_last_particle;
        pt = m_last_particle * particleRatio;
        m_system->emitParticle(datum);
    }

    m_last_last_last_emitter = m_last_last_emitter;
    m_last_last_emitter = m_last_emitter;
    m_last_emitter = QPointF(x() + m_emitter_x, y() + m_emitter_y);
    m_last_timestamp = time;
}

