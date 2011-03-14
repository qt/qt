#include "particleemitter.h"

ParticleEmitter::ParticleEmitter(QSGItem *parent) :
    QSGItem(parent)
  , m_particlesPerSecond(10)
  , m_particleDuration(1000)
  , m_particleDurationVariation(0)
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
    }
}
