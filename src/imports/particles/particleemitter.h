#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <QSGItem>
#include <QDebug>
#include "particlesystem.h"
#include "particleextruder.h"

class ParticleEmitter : public QSGItem
{
    Q_OBJECT
    //###currently goes in emitters OR sets system. Pick one?
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(QString particle READ particle WRITE setParticle NOTIFY particleChanged)
    Q_PROPERTY(ParticleExtruder* extruder READ extruder WRITE setExtruder NOTIFY extruderChanged)
    Q_PROPERTY(bool emitting READ emitting WRITE setEmitting NOTIFY emittingChanged)

    Q_PROPERTY(qreal particlesPerSecond READ particlesPerSecond WRITE setParticlesPerSecond NOTIFY particlesPerSecondChanged)
    Q_PROPERTY(int particleDuration READ particleDuration WRITE setParticleDuration NOTIFY particleDurationChanged)
    Q_PROPERTY(int particleDurationVariation READ particleDurationVariation WRITE setParticleDurationVariation NOTIFY particleDurationVariationChanged)


public:
    explicit ParticleEmitter(QSGItem *parent = 0);
    virtual ~ParticleEmitter();
    virtual void emitWindow(int timeStamp);

    bool emitting() const
    {
        return m_emitting;
    }

    qreal particlesPerSecond() const
    {
        return m_particlesPerSecond;
    }

    int particleDuration() const
    {
        return m_particleDuration;
    }

    ParticleSystem* system() const
    {
        return m_system;
    }

    QString particle() const
    {
        return m_particle;
    }

    int particleDurationVariation() const
    {
        return m_particleDurationVariation;
    }

signals:
    void particlesPerSecondChanged(qreal);
    void particleDurationChanged(int);
    void emittingChanged(bool);

    void systemChanged(ParticleSystem* arg);

    void particleChanged(QString arg);

    void particleDurationVariationChanged(int arg);

    void extruderChanged(ParticleExtruder* arg);

public slots:

    void setEmitting(bool arg);

    void setParticlesPerSecond(qreal arg)
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

       void setSystem(ParticleSystem* arg)
    {
        if (m_system != arg) {
            m_system = arg;
            m_system->registerParticleEmitter(this);
            emit systemChanged(arg);
        }
       }

       void setParticle(QString arg)
       {
           if (m_particle != arg) {
               m_particle = arg;
               emit particleChanged(arg);
           }
       }

       void setParticleDurationVariation(int arg)
       {
           if (m_particleDurationVariation != arg) {
               m_particleDurationVariation = arg;
               emit particleDurationVariationChanged(arg);
           }
       }
       void setExtruder(ParticleExtruder* arg)
{
    if (m_extruder != arg) {
    m_extruder = arg;
emit extruderChanged(arg);
}
}

public:
virtual void reset(){;}
ParticleExtruder* extruder() const
{
    return m_extruder;
}

protected:
    qreal m_particlesPerSecond;
    int m_particleDuration;
    int m_particleDurationVariation;
    bool m_emitting;
    ParticleSystem* m_system;
    QString m_particle;
    ParticleExtruder* m_extruder;
    ParticleExtruder* m_defaultExtruder;
    ParticleExtruder* effectiveExtruder();
private:
};

#endif // PARTICLEEMITTER_H
