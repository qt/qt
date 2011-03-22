#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(QSGItem *parent) :
    QSGItem(parent)
  , m_particlesPerSecond(10)
  , m_particleDuration(1000)
  , m_particleDurationVariation(0)
  , m_emitting(true)
  , m_system(0)
  , m_extruder(0)
  , m_defaultExtruder(0)

{
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
