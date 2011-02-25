#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(QObject *parent) :
    QObject(parent)
  , m_particle(0)
  , m_particlesPerSecond(10)
  , m_particleDuration(1000)
  , m_emitting(true)

{
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
        m_system->pleaseUpdate();
    }
}
