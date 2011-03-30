#include "particleemitter.h"
QT_BEGIN_NAMESPACE
ParticleEmitter::ParticleEmitter(QSGItem *parent) :
    QSGItem(parent)
  , m_particlesPerSecond(10)
  , m_particleDuration(1000)
  , m_particleDurationVariation(0)
  , m_emitting(true)
  , m_system(0)
  , m_extruder(0)
  , m_defaultExtruder(0)
  , m_speed(&m_nullVector)
  , m_acceleration(&m_nullVector)
  , m_particleSize(16)
  , m_particleEndSize(-1)
  , m_particleSizeVariation(0)
  , m_maxParticleCount(-1)

{
    //TODO: Reset speed/acc back to null vector? Or allow null pointer?
}

ParticleEmitter::~ParticleEmitter()
{
    if(m_defaultExtruder)
        delete m_defaultExtruder;
}

void ParticleEmitter::emitWindow(int timeStamp)
{
    Q_UNUSED(timeStamp);
}


void ParticleEmitter::setEmitting(bool arg)
{
    if (m_emitting != arg) {
        m_emitting = arg;
        emit emittingChanged(arg);
    }
}


ParticleExtruder* ParticleEmitter::effectiveExtruder()
{
    if(m_extruder)
        return m_extruder;
    if(!m_defaultExtruder)
        m_defaultExtruder = new ParticleExtruder;
    return m_defaultExtruder;
}

void ParticleEmitter::burst(qreal seconds)
{
    if(!m_emitting)
        m_burstLeft = seconds*1000.0;
}

int ParticleEmitter::particleCount() const
{
    if(m_maxParticleCount > 0)
        return m_maxParticleCount;
    return m_particlesPerSecond*((m_particleDuration+m_particleDurationVariation)/1000.0);
}

QT_END_NAMESPACE
