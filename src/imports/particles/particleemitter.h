#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <QSGItem>
#include <QDebug>
#include "particlesystem.h"
#include "particleextruder.h"
#include "varyingvector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleEmitter : public QSGItem
{
    Q_OBJECT
    //###currently goes in emitters OR sets system. Pick one?
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(QString particle READ particle WRITE setParticle NOTIFY particleChanged)
    Q_PROPERTY(ParticleExtruder* shape READ extruder WRITE setExtruder NOTIFY extruderChanged)
    Q_PROPERTY(bool emitting READ emitting WRITE setEmitting NOTIFY emittingChanged)

    Q_PROPERTY(qreal particlesPerSecond READ particlesPerSecond WRITE setParticlesPerSecond NOTIFY particlesPerSecondChanged)
    Q_PROPERTY(int particleDuration READ particleDuration WRITE setParticleDuration NOTIFY particleDurationChanged)
    Q_PROPERTY(int particleDurationVariation READ particleDurationVariation WRITE setParticleDurationVariation NOTIFY particleDurationVariationChanged)

    Q_PROPERTY(qreal particleSize READ particleSize WRITE setParticleSize NOTIFY particleSizeChanged)
    Q_PROPERTY(qreal particleEndSize READ particleEndSize WRITE setParticleEndSize NOTIFY particleEndSizeChanged)
    Q_PROPERTY(qreal particleSizeVariation READ particleSizeVariation WRITE setParticleSizeVariation NOTIFY particleSizeVariationChanged)

    Q_PROPERTY(VaryingVector *speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(VaryingVector *acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
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

    void particleSizeChanged(qreal arg);

    void particleEndSizeChanged(qreal arg);

void particleSizeVariationChanged(qreal arg);

void speedChanged(VaryingVector * arg);

void accelerationChanged(VaryingVector * arg);

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

       void setParticleSize(qreal arg)
       {
           if (m_particleSize != arg) {
               m_particleSize = arg;
               emit particleSizeChanged(arg);
           }
       }

       void setParticleEndSize(qreal arg)
       {
           if (m_particleEndSize != arg) {
               m_particleEndSize = arg;
               emit particleEndSizeChanged(arg);
           }
       }

       void setParticleSizeVariation(qreal arg)
       {
           if (m_particleSizeVariation != arg) {
               m_particleSizeVariation = arg;
               emit particleSizeVariationChanged(arg);
           }
       }

       void setSpeed(VaryingVector * arg)
       {
           if (m_speed != arg) {
               m_speed = arg;
               emit speedChanged(arg);
           }
       }

       void setAcceleration(VaryingVector * arg)
       {
           if (m_acceleration != arg) {
               m_acceleration = arg;
               emit accelerationChanged(arg);
           }
       }

public:
       virtual void reset(){;}
       ParticleExtruder* extruder() const
       {
           return m_extruder;
       }

       qreal particleSize() const
       {
           return m_particleSize;
       }

       qreal particleEndSize() const
       {
           return m_particleEndSize;
       }

       qreal particleSizeVariation() const
       {
           return m_particleSizeVariation;
       }

       VaryingVector * speed() const
       {
           return m_speed;
       }

       VaryingVector * acceleration() const
       {
           return m_acceleration;
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
       qreal m_particleSize;
       qreal m_particleEndSize;
       qreal m_particleSizeVariation;
       VaryingVector * m_speed;
       VaryingVector * m_acceleration;
private:
       VaryingVector m_nullVector;
};

QT_END_NAMESPACE
#endif // PARTICLEEMITTER_H
