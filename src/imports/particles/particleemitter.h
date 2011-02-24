#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <QObject>
#include <QDebug>
#include "particlesystem.h"

class ParticleEmitter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Particle* particle READ particle WRITE setParticle NOTIFY particleChanged)
    Q_PROPERTY(bool emitting READ emitting WRITE setEmitting NOTIFY emittingChanged)

    Q_PROPERTY(int particlesPerSecond READ particlesPerSecond WRITE setParticlesPerSecond NOTIFY particlesPerSecondChanged)
    Q_PROPERTY(int particleDuration READ particleDuration WRITE setParticleDuration NOTIFY particleDurationChanged)

public:
    explicit ParticleEmitter(QObject *parent = 0);
    virtual void emitWindow(int timeStamp);

    ParticleSystem* m_system;//###Needs to be set from classless function? Needed at all?
    Particle* particle() {return m_particle;}

    bool emitting() const
    {
        return m_emitting;
    }

    int particlesPerSecond() const
    {
        return m_particlesPerSecond;
    }

    int particleDuration() const
    {
        return m_particleDuration;
    }

signals:
    void particlesPerSecondChanged(int);
    void particleDurationChanged(int);
    void emittingChanged(bool);

    void particleChanged(Particle* arg);

public slots:

    void setEmitting(bool arg);

    void setParticlesPerSecond(int arg)
    {
        if (m_particlesPerSecond != arg) {
            m_particlesPerSecond = arg;
            emit particlesPerSecondChanged(arg);
        }
    }

    void setParticleDuration(int arg)
    {
        if (m_particleDuration != arg) {
            m_particleDuration = arg;
            emit particleDurationChanged(arg);
        }
    }

    void setParticle(Particle* arg)
    {
        if (m_particle != arg) {
            m_particle = arg;
            emit particleChanged(arg);
        }
    }

protected:
    Particle* m_particle;
    int m_particlesPerSecond;
    int m_particleDuration;
    bool m_emitting;
private:

};

#endif // PARTICLEEMITTER_H
